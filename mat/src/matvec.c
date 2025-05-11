/**
 * @file matvec.c
 *
 * @brief Implementation of distributed matrix-vector multiplication.
 */

#include "../include/matvec.h"
#include "../include/io.h"

/**
 * @brief Perform distributed matrix-vector multiplication.
 *
 * @param[in]  local_matrix Local portion of matrix.
 * @param[in]  local_x      Local portion of input vector.
 * @param[out] local_y      Local portion of result vector.
 * @param[in]  rank         Process rank.
 * @param[in]  nprocs       Number of processes.
 * @param[in]  comm         MPI communicator.
 *
 * @return 0 on success, non-zero on error.
 */
int matvec(double local_matrix[][BLOCK_DIM], double* local_x, double* local_y,
           int rank, int nprocs, MPI_Comm comm) {

  // Verify we have exactly four processes
  if (nprocs != 4) {
    if (rank == 0) {
      fprintf(stderr, "Error: You must use four processes\n");
    }
    return 1;
  }

  // Allocate memory for the intermediate local result
  double* intermediate_y = (double*) malloc(MATRIX_DIM * sizeof(double));
  if (!intermediate_y) {
    fprintf(stderr,
            "Error: Process %d could not allocate memory for the intermediate "
            "local result\n",
            rank);
    return 1;
  }

  // Initialize intermediate_y to zero
  for (int i = 0; i < MATRIX_DIM; i++) {
    intermediate_y[i] = 0.0;
  }

  // Each process computes its partial product
  for (int i = 0; i < MATRIX_DIM; i++) { // Iterate over global rows
    double sum_for_row_i = 0.0;
    for (int k = 0; k < BLOCK_DIM; k++) {
      sum_for_row_i += local_matrix[i][k] * local_x[k];
    }
    intermediate_y[i] = sum_for_row_i;
  }

  // Allocate memory for the final global result vector
  double* global_y_final = (double*) malloc(MATRIX_DIM * sizeof(double));
  if (!global_y_final) {
    fprintf(
        stderr,
        "Error: Process %d could not allocate memory for the final solution\n",
        rank);
    free(intermediate_y);
    return 1;
  }

  // Sum up all intermediate_y vectors from all processes
  MPI_Allreduce(intermediate_y, global_y_final, MATRIX_DIM, MPI_DOUBLE, MPI_SUM,
                comm);

  // Each process extracts its assigned portion from global_y_final into local_y
  int start_row_for_this_rank = rank * BLOCK_DIM;
  for (int k = 0; k < BLOCK_DIM; k++) {
    local_y[k] = global_y_final[start_row_for_this_rank + k];
  }

  // Clean up allocated memory
  free(intermediate_y);
  free(global_y_final);

  return 0;
}

/**
 * @brief Print the local portion of the result vector.
 *
 * @param[in] local_y The local result vector data.
 * @param[in] rank    Process rank.
 */
void print_local_res(double* local_y, int rank) {
  printf("Process %d - Rows %d-%d\n", rank, rank * BLOCK_DIM,
         (rank + 1) * BLOCK_DIM - 1);
  for (int i = 0; i < BLOCK_DIM; i++) {
    printf("y[%d] = %.0f\n", rank * BLOCK_DIM + i, local_y[i]);
  }
  printf("\n");
}

/**
 * @brief Gather and print the complete result vector.
 *
 * @param[in] local_y Local portion of result vector.
 * @param[in] rank    Process rank.
 * @param[in] comm    MPI communicator.
 */
void print_res(double* local_y, int rank, MPI_Comm comm) {
  double* global_y = NULL;

  // Allocate memory for the global result on root rank
  if (rank == 0) {
    global_y = (double*) malloc(MATRIX_DIM * sizeof(double));
    if (!global_y) {
      fprintf(stderr, "Error: Could not allocate memory for global result\n");
      return;
    }
  }

  // Gather all local results
  MPI_Gather(local_y, BLOCK_DIM, MPI_DOUBLE, global_y, BLOCK_DIM, MPI_DOUBLE, 0,
             comm);

  // Print the complete result vector
  if (rank == 0) {
    printf("Process 0 - Rows 0-19\n");
    for (int i = 0; i < MATRIX_DIM; i++) {
      printf("y[%d] = %.0f\n", i, global_y[i]);
    }
    printf("\n");

    // Clean up
    free(global_y);
  }
}
