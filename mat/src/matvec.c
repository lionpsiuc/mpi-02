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

  // Allocate memory for the complete input vector
  double* global_x = (double*) malloc(MATRIX_DIM * sizeof(double));
  if (!global_x) {
    fprintf(
        stderr,
        "Error: Could not allocate memory for global vector on process %d\n",
        rank);
    return 1;
  }

  // MPI_Allgather the complete input vector to all processes, where each
  // process contributes five elements
  MPI_Allgather(local_x, BLOCK_DIM, MPI_DOUBLE, global_x, BLOCK_DIM, MPI_DOUBLE,
                comm);

  // Initialize local result to zero
  for (int i = 0; i < BLOCK_DIM; i++) {
    local_y[i] = 0.0;
  }

  // Compute the local portion of the result vector; each process computes five
  // rows of the result
  int start_row = rank * BLOCK_DIM;
  int end_row   = start_row + BLOCK_DIM;

  // For each row this process is responsible for...
  for (int i = start_row; i < end_row; i++) {
    double sum = 0.0;

    // multiply the current row with the complete vector
    for (int j = 0; j < MATRIX_DIM; j++) {
      int local_row = i - start_row; // Local row index
      int local_col = j % BLOCK_DIM; // Local column index
      sum += local_matrix[local_row][local_col] * global_x[j];
    }

    // and store result in local output vector
    local_y[i - start_row] = sum;
  }

  // Clean up
  free(global_x);

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
