/**
 * @file  io.h
 * @brief Header for reading distributed matrices and vectors.
 */

#ifndef IO_H
#define IO_H

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MATRIX_DIM 20
#define BLOCK_DIM 5
#define BLOCKS_PER_DIM 4

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
             int nprocs, MPI_Comm comm);

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
             MPI_Comm comm);

/**
 * @brief Print the local portion of the matrix.
 *
 * @param[in] local_matrix The local matrix data.
 * @param[in] rank         Process rank.
 */
void print_local_mat(double local_matrix[][BLOCK_DIM], int rank);

/**
 * @brief Print the local portion of the vector.
 *
 * @param[in] local_vector The local vector data.
 * @param[in] rank         Process rank.
 */
void print_local_vec(double* local_vector, int rank);

#endif
