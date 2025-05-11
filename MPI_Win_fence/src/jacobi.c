/**
 * @file  jacobi.c
 * @brief Implementation of Jacobi iteration functions.
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
 * @param[in] a     First grid array.
 * @param[in] b     Second grid array.
 * @param[in] nx    Number of interior grid points in x-axis.
 * @param[in] row_s Starting row index of local domain.
 * @param[in] row_e Ending row index of local domain.
 * @param[in] col_s Starting column index of local domain.
 * @param[in] col_e Ending column index of local domain.
 *
 * @returns Sum of squared differences between the two grid arrays.
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
 * point, computes the average of its four neighbors, adjusted by the right-hand
 * side function values, to solve the Poisson equation.
 *
 * @param[in]  a     Current iteration grid array.
 * @param[in]  f     Right-hand side function values.
 * @param[in]  nx    Number of interior grid points in x-axis.
 * @param[in]  row_s Starting row index of local domain.
 * @param[in]  row_e Ending row index of local domain.
 * @param[in]  col_s Starting column index of local domain.
 * @param[in]  col_e Ending column index of local domain.
 * @param[out] b     Next iteration grid array to store the updated values.
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
 * @brief Exchanges ghost cells with neighboring processes using RMA.
 *
 * @param[in,out] x        Solution values.
 * @param[in]     row_s    Starting row index of local domain.
 * @param[in]     row_e    Ending row index of local domain.
 * @param[in]     col_s    Starting column index of local domain.
 * @param[in]     col_e    Ending column index of local domain.
 * @param[in]     nbrleft  Rank of left neighbor.
 * @param[in]     nbrright Rank of right neighbor.
 * @param[in]     nbrup    Rank of upper neighbor.
 * @param[in]     nbrdown  Rank of lower neighbor.
 * @param[in]     row_type Custom MPI datatype for non-contiguous row data.
 * @param[in]     win      MPI window object exposing the grid array.
 */
void exchang2d_rma_fence(double x[][maxn], int row_s, int row_e, int col_s,
                         int col_e, int nbrleft, int nbrright, int nbrup,
                         int nbrdown, MPI_Datatype row_type, MPI_Win win) {
  int lny = row_e - row_s + 1; // Number of rows in local domain

  // Start the RMA access epoch
  MPI_Win_fence(0, win);

  // Get their (left neighbor) rightmost column into our left ghost column
  if (nbrleft != MPI_PROC_NULL) {

    // We want to get the data at column (col_s - 1) from our left neighbor
    MPI_Aint displacement = (MPI_Aint) (col_s - 1) * maxn + row_s;
    MPI_Get(&x[col_s - 1][row_s], lny, MPI_DOUBLE, nbrleft, displacement, lny,
            MPI_DOUBLE, win);
  }

  // Get their (right neighbor) leftmost column into our right ghost column
  if (nbrright != MPI_PROC_NULL) {

    // We want to get the data at column (col_e + 1) from our right neighbor
    MPI_Aint displacement = (MPI_Aint) (col_e + 1) * maxn + row_s;
    MPI_Get(&x[col_e + 1][row_s], lny, MPI_DOUBLE, nbrright, displacement, lny,
            MPI_DOUBLE, win);
  }

  // Get their (lower neighbor) topmost row into our bottom ghost row
  if (nbrdown != MPI_PROC_NULL) {

    // We want to get the data at row (row_s - 1) from our lower neighbor
    MPI_Aint displacement = (MPI_Aint) col_s * maxn + (row_s - 1);
    MPI_Get(&x[col_s][row_s - 1], 1, row_type, nbrdown, displacement, 1,
            row_type, win);
  }

  // Get their (upper neighbor) bottommost row into our top ghost row
  if (nbrup != MPI_PROC_NULL) {

    // We want to get the data at row (row_e + 1) from our upper neighbor
    MPI_Aint displacement = (MPI_Aint) col_s * maxn + (row_e + 1);
    MPI_Get(&x[col_s][row_e + 1], 1, row_type, nbrup, displacement, 1, row_type,
            win);
  }

  // End the RMA access epoch
  MPI_Win_fence(0, win);
}
