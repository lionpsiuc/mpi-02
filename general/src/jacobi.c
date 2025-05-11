/**
 * @file jacobi.c
 *
 * @brief Implementation of Jacobi iteration functions for the 2D parallel
 *        Poisson solver.
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/jacobi.h"
#include "../include/poisson2d.h"

/**
 * @brief Calculates the squared difference between two grid arrays.
 *
 * Computes the sum of squared differences between two grid arrays, which is
 * used to check for convergence between iterations of the Jacobi method.
 *
 * @param[in] a First grid array.
 * @param[in] b Second grid array.
 * @param[in] nx Number of interior grid points in x-axis.
 * @param[in] row_s Starting row index of local domain.
 * @param[in] row_e Ending row index of local domain.
 * @param[in] col_s Starting column index of local domain.
 * @param[in] col_e Ending column index of local domain.
 *
 * @return Sum of squared differences between the two grid arrays.
 */
double griddiff2d(double a[][maxn], double b[][maxn],
                  int nx __attribute__((unused)), int row_s, int row_e,
                  int col_s, int col_e) {
  double sum = 0.0;
  double tmp;
  for (int i = col_s; i <= col_e; i++) {
    for (int j = row_s; j <= row_e; j++) {
      tmp = (a[i][j] - b[i][j]);
      sum = sum + tmp * tmp;
    }
  }
  return sum;
}

/**
 * @brief Performs one Jacobi iteration step.
 *
 * Updates the grid values for one iteration of the Jacobi method. For each
 * point, computes the average of its four neighbours, adjusted by the
 * right-hand side function values, to solve the Poisson equation.
 *
 * @param[in] a Current iteration grid array.
 * @param[in] f Right-hand side function values.
 * @param[in] nx Number of interior grid points in x-axis.
 * @param[in] row_s Starting row index of local domain.
 * @param[in] row_e Ending row index of local domain.
 * @param[in] col_s Starting column index of local domain.
 * @param[in] col_e Ending column index of local domain.
 * @param[out] b Next iteration grid array to store the updated values.
 */
void sweep2d(double a[][maxn], double f[][maxn], int nx, int row_s, int row_e,
             int col_s, int col_e, double b[][maxn]) {
  double h = 1.0 / ((double) (nx + 1)); // Grid spacing
  for (int i = col_s; i <= col_e; i++) {
    for (int j = row_s; j <= row_e; j++) {
      b[i][j] = 0.25 * (a[i - 1][j] + a[i + 1][j] + a[i][j + 1] + a[i][j - 1] -
                        h * h * f[i][j]);
    }
  }
}

/**
 * @brief Exchanges ghost cells with neighboring processes using general active
 *        target RMA synchronization.
 *
 * @param[in,out] x Solution values.
 * @param[in] row_s Starting row index of local domain.
 * @param[in] row_e Ending row index of local domain.
 * @param[in] col_s Starting column index of local domain.
 * @param[in] col_e Ending column index of local domain.
 * @param[in] nbrleftRank of left neighbor.
 * @param[in] nbrright Rank of right neighbor.
 * @param[in] nbrup Rank of upper neighbor.
 * @param[in] nbrdown Rank of lower neighbor.
 * @param[in] row_type Custom MPI datatype for non-contiguous row data.
 * @param[in] win MPI window object exposing the grid array.
 * @param[in] group MPI group for RMA synchronization.
 */
void exchang2d_rma_pscw(double x[][maxn], int row_s, int row_e, int col_s,
                        int col_e, int nbrleft, int nbrright, int nbrup,
                        int nbrdown, MPI_Datatype row_type, MPI_Win win,
                        MPI_Group group) {
  int lny = row_e - row_s + 1; // Number of rows in local domain

  // Arrays to hold the ranks of processes we communicate with
  int access_neighbors[4];   // Processes we will read from
  int exposure_neighbors[4]; // Processes that will read from us
  int num_access   = 0;      // Counter for access neighbors
  int num_exposure = 0;      // Counter for exposure neighbors

  // Build the list of neighbors we need to access
  if (nbrleft != MPI_PROC_NULL)
    access_neighbors[num_access++] = nbrleft;
  if (nbrright != MPI_PROC_NULL)
    access_neighbors[num_access++] = nbrright;
  if (nbrup != MPI_PROC_NULL)
    access_neighbors[num_access++] = nbrup;
  if (nbrdown != MPI_PROC_NULL)
    access_neighbors[num_access++] = nbrdown;

  // The processes we read from are the same ones that read from us
  num_exposure = num_access;
  for (int i = 0; i < num_access; i++) {
    exposure_neighbors[i] = access_neighbors[i];
  }

  // Groups for synchronization
  MPI_Group access_group, exposure_group;

  // Create the access group
  if (num_access > 0) {
    MPI_Group_incl(group, num_access, access_neighbors, &access_group);
  }

  // Create the exposure group
  if (num_exposure > 0) {
    MPI_Group_incl(group, num_exposure, exposure_neighbors, &exposure_group);
  }

  // Post our window for exposure
  if (num_exposure > 0) {
    MPI_Win_post(exposure_group, 0, win);
  }

  // Start our access epoch
  if (num_access > 0) {
    MPI_Win_start(access_group, 0, win);
  }

  // Get their (left neighbor) rightmost column into our left ghost column
  if (nbrleft != MPI_PROC_NULL) {

    // Calculate the column position we want to access in the neighbor's memory
    int target_col = col_s - 1;

    // Calculate the memory displacement using row-major addressing
    MPI_Aint displacement = (MPI_Aint) target_col * maxn + row_s;

    MPI_Get(&x[col_s - 1][row_s], lny, MPI_DOUBLE, nbrleft, displacement, lny,
            MPI_DOUBLE, win);
  }

  // Get their (right neighbor) leftmost column into our right ghost column
  if (nbrright != MPI_PROC_NULL) {
    int      target_col   = col_e + 1;
    MPI_Aint displacement = (MPI_Aint) target_col * maxn + row_s;
    MPI_Get(&x[col_e + 1][row_s], lny, MPI_DOUBLE, nbrright, displacement, lny,
            MPI_DOUBLE, win);
  }

  // Get their (lower neighbor) topmost row into our bottom ghost row
  if (nbrdown != MPI_PROC_NULL) {
    int      target_row   = row_s - 1;
    MPI_Aint displacement = (MPI_Aint) col_s * maxn + target_row;
    MPI_Get(&x[col_s][row_s - 1], 1, row_type, nbrdown, displacement, 1,
            row_type, win);
  }

  // Get their (upper neighbor) bottommost row into our top ghost row
  if (nbrup != MPI_PROC_NULL) {
    int      target_row   = row_e + 1;
    MPI_Aint displacement = (MPI_Aint) col_s * maxn + target_row;
    MPI_Get(&x[col_s][row_e + 1], 1, row_type, nbrup, displacement, 1, row_type,
            win);
  }

  // Complete our access epoch
  if (num_access > 0) {
    MPI_Win_complete(win);
  }

  // Wait for our exposure to complete
  if (num_exposure > 0) {
    MPI_Win_wait(win);
  }

  // Free the groups we created
  if (num_access > 0)
    MPI_Group_free(&access_group);
  if (num_exposure > 0)
    MPI_Group_free(&exposure_group);
}
