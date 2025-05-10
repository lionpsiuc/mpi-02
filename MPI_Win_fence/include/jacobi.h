/**
 * @file jacobi.h
 *
 * @brief Jacobi iteration functions for the 2D parallel Poisson solver.
 */

#include "poisson2d.h"

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
                  int col_s, int col_e);

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
             int col_s, int col_e, double b[][maxn]);

/**
 * @brief Exchanges ghost cells with neighboring processes using RMA fence
 *        synchronization.
 *
 * @param[in,out] x Solution values.
 * @param[in] row_s Starting row index of local domain.
 * @param[in] row_e Ending row index of local domain.
 * @param[in] col_s Starting column index of local domain.
 * @param[in] col_e Ending column index of local domain.
 * @param[in] nbrleft Rank of left neighbor.
 * @param[in] nbrright Rank of right neighbor.
 * @param[in] nbrup Rank of upper neighbor.
 * @param[in] nbrdown Rank of lower neighbor.
 * @param[in] row_type Custom MPI datatype for non-contiguous row data.
 * @param[in] win MPI window object exposing the grid array.
 */
void exchang2d_rma_fence(double x[][maxn], int row_s, int row_e, int col_s,
                         int col_e, int nbrleft, int nbrright, int nbrup,
                         int nbrdown, MPI_Datatype row_type, MPI_Win win);
