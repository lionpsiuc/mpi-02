/**
 * @file main.c
 *
 * @brief Main program for distributed matrix and vector reading.
 */

#include "../include/io.h"

/**
 * @brief Main function demonstrating distributed matrix and vector reading.
 *
 * @param[in] argc Number of command-line arguments.
 * @param[in] argv Command-line arguments.
 *
 * @return 0 on success, non-zero on error.
 */
int main(int argc, char** argv) {
  int  rank, nprocs;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int  namelen;

  // Initialize MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Get_processor_name(processor_name, &namelen);

  // Verify we have exactly four processes
  if (nprocs != 4) {
    if (rank == 0) {
      fprintf(stderr, "Error: You must use four processes\n");
      fprintf(stderr, "Usage is as follows: mpirun -np 4 %s\n", argv[0]);
    }
    MPI_Finalize();
    return 1;
  }

  // Allocate memory for local data
  double local_matrix[MATRIX_DIM][BLOCK_DIM];

  // Each process gets five elements of the vector
  double local_vector[BLOCK_DIM];

  // Filenames
  const char* matrix_filename = "mat-d20-b5-p4.bin";
  const char* vector_filename = "x-d20.txt.bin";

  // Read the distributed matrix
  if (rank == 0) {
    printf("Reading matrix from %s\n", matrix_filename);
  }

  int result = read_distributed_matrix(matrix_filename, local_matrix, rank,
                                       nprocs, MPI_COMM_WORLD);
  if (result != 0) {
    if (rank == 0) {
      fprintf(stderr, "Error: Failed to read from %s\n", matrix_filename);
    }
    MPI_Finalize();
    return 1;
  }

  // Read the distributed vector
  if (rank == 0) {
    printf("Reading vector from %s\n", vector_filename);
  }

  result = read_distributed_vector(vector_filename, local_vector, rank, nprocs,
                                   MPI_COMM_WORLD);
  if (result != 0) {
    if (rank == 0) {
      fprintf(stderr, "Error: Failed to read from %s\n", vector_filename);
    }
    MPI_Finalize();
    return 1;
  }

  // Barrier to ensure all processes have finished reading
  MPI_Barrier(MPI_COMM_WORLD);

  // // Print data
  // for (int p = 0; p < nprocs; p++) {
  //   if (p == rank) {
  //     print_local_matrix(local_matrix, rank);
  //     print_local_vector(local_vector, rank);
  //     fflush(stdout);
  //   }
  //   MPI_Barrier(MPI_COMM_WORLD);
  //   usleep(1000);
  // }

  // Clean up and finalize
  MPI_Finalize();
  return 0;
}
