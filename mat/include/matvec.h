/**
 * @file matvec.h
 *
 * @brief Header for distributed matrix-vector multiplication.
 */

#ifndef MATVEC_H
#define MATVEC_H

#include "io.h"

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
           int rank, int nprocs, MPI_Comm comm);

/**
 * @brief Print the local portion of the result vector.
 *
 * @param[in] local_y The local result vector data.
 * @param[in] rank    Process rank.
 */
void print_local_res(double* local_y, int rank);

/**
 * @brief Gather and print the complete result vector.
 *
 * @param[in] local_y Local portion of result vector.
 * @param[in] rank    Process rank.
 * @param[in] comm    MPI communicator.
 */
void print_res(double* local_y, int rank, MPI_Comm comm);

#endif
