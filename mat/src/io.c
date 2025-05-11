/**
 * @file io.c
 *
 * @brief Implementation of functions for reading distributed matrices and
 *        vectors.
 */

#include "../include/io.h"

/**
 * @brief Read a distributed matrix from a binary file.
 *
 * @param[in]  filename     Path to the matrix binary file.
 * @param[out] local_matrix Array to store the local portion of the matrix.
 * @param[in]  rank         Process rank.
 * @param[in]  nprocs       Number of processes.
 * @param[in]  comm         MPI communicator.
 *
 * @return 0 on success, non-zero on error.
 */
int read_mat(const char* filename, double local_matrix[][BLOCK_DIM], int rank,
             int nprocs, MPI_Comm comm) {
  MPI_File   file;
  MPI_Status status;
  int        ierr;

  // Verify we have exactly four processes
  if (nprocs != 4) {
    if (rank == 0) {
      fprintf(stderr, "Error: You must use four processes\n");
    }
    return 1;
  }

  // Open the file for reading
  ierr = MPI_File_open(comm, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
  if (ierr != MPI_SUCCESS) {
    if (rank == 0) {
      fprintf(stderr, "Error: Could not open %s\n", filename);
    }
    return 1;
  }

  // Verify matrix dimension
  if (rank == 0) {
    int dim;
    MPI_File_read(file, &dim, 1, MPI_INT, &status);
    if (dim != MATRIX_DIM) {
      fprintf(
          stderr,
          "Error: Matrix dimension is incorrect; expected %d, but read %d\n",
          MATRIX_DIM, dim);
      MPI_File_close(&file);
      return 1;
    }
    printf("Matrix dimension is %d x %d\n", dim, dim);
  }

  // Synchronize all processes
  MPI_Barrier(comm);

  // Calculate the offset for this process's data; note that the order of data
  // is as follows: integer representing the dimension of the matrix, followed
  // by first block column, then the second, then third, and finally the fourth
  MPI_Offset block_column_size =
      (MPI_Offset) BLOCKS_PER_DIM * BLOCK_DIM * BLOCK_DIM * sizeof(double);
  MPI_Offset offset = sizeof(int) + rank * block_column_size;

  // Set the file pointer to the beginning of this process's data
  MPI_File_seek(file, offset, MPI_SEEK_SET);

  // Read the local block column - four blocks of 5 x 5 matrices
  int blocks_per_column = BLOCKS_PER_DIM; // Four blocks per column
  for (int block = 0; block < blocks_per_column; block++) {

    // Calculate the row offset in the local_matrix array
    int row_offset = block * BLOCK_DIM;

    // Read one 5 x 5 matrix
    MPI_File_read(file, &local_matrix[row_offset][0], BLOCK_DIM * BLOCK_DIM,
                  MPI_DOUBLE, &status);
  }

  MPI_File_close(&file); // Close the file
  return 0;
}

/**
 * @brief Read a distributed vector from a binary file.
 *
 * @param[in]  filename     Path to the vector binary file.
 * @param[out] local_vector Array to store the local portion of the vector.
 * @param[in]  rank         Process rank.
 * @param[in]  nprocs       Number of processes.
 * @param[in]  comm         MPI communicator.
 *
 * @return 0 on success, non-zero on error.
 */
int read_vec(const char* filename, double* local_vector, int rank, int nprocs,
             MPI_Comm comm) {
  MPI_File   file;
  MPI_Status status;
  int        ierr;

  // Verify we have exactly four processes
  if (nprocs != 4) {
    if (rank == 0) {
      fprintf(stderr, "Error: You must use four processes\n");
    }
    return 1;
  }

  // Open the file for reading
  ierr = MPI_File_open(comm, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
  if (ierr != MPI_SUCCESS) {
    if (rank == 0) {
      fprintf(stderr, "Error: Could not open %s\n", filename);
    }
    return 1;
  }

  // Verify vector length
  if (rank == 0) {
    int length;
    MPI_File_read(file, &length, 1, MPI_INT, &status);
    if (length != MATRIX_DIM) {
      fprintf(stderr,
              "Error: Vector length is incorrect; expected %d, but read %d\n",
              MATRIX_DIM, length);
      MPI_File_close(&file);
      return 1;
    }
    printf("Vector length is %d\n\n", length);
  }

  // Synchronize all processes
  MPI_Barrier(comm);

  // Calculate the offset for this process's data; note that each process must
  // read five values
  MPI_Offset offset = sizeof(int) + rank * BLOCK_DIM * sizeof(double);

  // Set the file pointer to the beginning of this process's data
  MPI_File_seek(file, offset, MPI_SEEK_SET);

  // Read the local portion of the vector
  MPI_File_read(file, local_vector, BLOCK_DIM, MPI_DOUBLE, &status);

  MPI_File_close(&file); // Close the file
  return 0;
}

/**
 * @brief Print the local portion of the matrix.
 *
 * @param[in] local_matrix The local matrix data.
 * @param[in] rank         Process rank.
 */
void print_local_mat(double local_matrix[][BLOCK_DIM], int rank) {
  int blocks_per_column = BLOCKS_PER_DIM; // Four blocks per column
  printf("Process %d - Column %d\n", rank, rank);
  for (int block = 0; block < blocks_per_column; block++) {
    int row_offset = block * BLOCK_DIM;
    printf("Block %d - Rows %d-%d\n", block, row_offset,
           row_offset + BLOCK_DIM - 1);
    printf("[[");
    for (int i = 0; i < BLOCK_DIM; i++) {
      if (i > 0) {
        printf("[");
      }
      for (int j = 0; j < BLOCK_DIM; j++) {
        if (j > 0) {
          printf(" ");
        }
        printf("%.0f.", local_matrix[row_offset + i][j]);
      }
      if (i < BLOCK_DIM - 1) {
        printf("]\n ");
      }
    }
    printf("]]\n\n");
  }
}

/**
 * @brief Print the local portion of the vector.
 *
 * @param[in] local_vector The local vector data.
 * @param[in] rank         Process rank.
 */
void print_local_vec(double* local_vector, int rank) {
  printf("Process %d - Rows %d-%d\n", rank, rank * BLOCK_DIM,
         (rank + 1) * BLOCK_DIM - 1);
  for (int i = 0; i < BLOCK_DIM; i++) {
    printf("x[%d] = %.0f\n", rank * BLOCK_DIM + i, local_vector[i]);
  }
  printf("\n");
}
