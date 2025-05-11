# Question 1

The solutions to the first and second part of the first question can be found in MPI_Win_fence/ and general/, respectively.

## MPI_Win_fence

The main change is the inclusion of exchang2d_rma_fence within MPI_Win_fence/src/jacobi.c (and its associated declaration in MPI_Win_fence/include/jacobi.h); this function follows the instructions stating that we must use MPI_Win_fence-based synchronization along with MPI_Put and MPI_Get. The only changes we make to MPI_Win_fence/src/main.c is the creation of two MPI windows along with the use of exchang2d_rma_fence instead of the MPI_Send and MPI_Recv functions we created in the previous assignment.

### How to Compile, Run, and Compare

Once in MPI_Win_fence/, compile using:

```bash
make
```

Then, run the version using four processes using make run4 or 16 processes using make run16. This will generate the same files as in the previous assignment.

Now, in order to compare whether this RMA version follows the non-RMA version, we can simply (once all files are generated) run the following:

```bash
diff -w ~/mpi-02/2d/global2dnprocs4nx31.txt ~/mpi-02/MPI_Win_fence/global2dnprocs4nx31.txt
```

This will check whether the global solution is the same (for four processes) - the 16 process equivalent is:

```bash
diff -w ~/mpi-02/2d/global2dnprocs16nx31.txt ~/mpi-02/MPI_Win_fence/global2dnprocs16nx31.txt
```

Running this will not produce any output indicating that the two files are identical implying that our answers using RMA operations match the answers of our non-RMA version.

Cleaning can simply be done using make clean; note that this will not delete any of the generated solution files.

## PSCW

The main change is the inclusion of exchang2d_rma_pscw within general/src/jacobi.c (and its associated declaration in general/include/jacobi.h); this function follows the instructions stating that we must use general RMA-based synchronization (i.e., MPI_Win_start, MPI_Win_complete, MPI_Win_post, and MPI_Win_wait) along with MPI_Put and MPI_Get. The only changes we make to general/src/main.c is the creation of two MPI windows, a new communicator using MPI_Group (this is done to represent the access and exposure processes), and we use exchang2d_rma_pscw instead of the MPI_Send and MPI_Recv functions we created in the previous assignment.

### How to Compile, Run, and Compare

Once in general/, compile using:

```bash
make
```

Then, run the version using four processes using make run4 or 16 processes using make run16. This will generate the same files as in the previous assignment.

Now, in order to compare whether this RMA version follows the non-RMA version, we can simply (once all files are generated) run the following:

```bash
diff -w ~/mpi-02/2d/global2dnprocs4nx31.txt ~/mpi-02/general/global2dnprocs4nx31.txt
```

This will check whether the global solution is the same (for four processes) - the 16 process equivalent is:

```bash
diff -w ~/mpi-02/2d/global2dnprocs16nx31.txt ~/mpi-02/general/global2dnprocs16nx31.txt
```

Running this will not produce any output indicating that the two files are identical implying that our answers using RMA operations match the answers of our non-RMA version.

Cleaning can simply be done using make clean; note that this will not delete any of the generated solution files.

# Question 2

The solutions to this question can be found in mat/. Note that all functions are made to work with four processes; had we had more time, or if the assignment were different, a more generalised implementation would have been created.

# How to Compile and Run

Ensure you are in mat/ and then carry out the following:

```bash
make
```

This will compile all the files; run using:

```bash
make run
```

As part of the output, parts of the local matrices and vectors (on each process) are printed out to the terminal in order to confirm our results. The following is the output:

```bash
Reading matrix from mat-d20-b5-p4.bin
Matrix dimension is 20 x 20
Reading vector from x-d20.txt.bin
Vector length is 20

Process 0 - Column 0
Block 0 - Rows 0-4
[[4. 4. 3. 1. 2.]
 [0. 4. 0. 3. 0.]
 [4. 3. 3. 2. 3.]
 [3. 4. 3. 4. 4.]
 [1. 1. 1. 2. 1.]]

Block 1 - Rows 5-9
[[1. 4. 0. 3. 2.]
 [4. 4. 4. 1. 3.]
 [1. 2. 0. 1. 2.]
 [2. 4. 3. 4. 1.]
 [2. 3. 0. 0. 4.]]

Block 2 - Rows 10-14
[[2. 1. 0. 0. 1.]
 [1. 1. 1. 0. 2.]
 [4. 0. 1. 0. 3.]
 [1. 1. 0. 3. 4.]
 [0. 0. 4. 4. 4.]]

Block 3 - Rows 15-19
[[3. 4. 3. 4. 0.]
 [1. 2. 3. 1. 3.]
 [3. 3. 2. 1. 2.]
 [3. 0. 3. 4. 0.]
 [3. 3. 3. 0. 2.]]

Process 0 - Rows 0-4
x[0] = 4
x[1] = 5
x[2] = 6
x[3] = 7
x[4] = 7

Process 1 - Column 1
Block 0 - Rows 0-4
[[3. 4. 3. 4. 1.]
 [2. 4. 3. 2. 2.]
 [3. 0. 4. 2. 0.]
 [4. 0. 4. 4. 4.]
 [0. 3. 3. 1. 4.]]

Block 1 - Rows 5-9
[[2. 0. 4. 1. 2.]
 [3. 2. 2. 2. 1.]
 [1. 3. 1. 1. 3.]
 [4. 0. 0. 3. 0.]
 [3. 2. 3. 3. 2.]]

Block 2 - Rows 10-14
[[0. 0. 3. 0. 3.]
 [0. 3. 2. 1. 3.]
 [2. 4. 1. 3. 1.]
 [0. 3. 0. 4. 4.]
 [3. 4. 2. 4. 1.]]

Block 3 - Rows 15-19
[[1. 3. 3. 3. 3.]
 [2. 1. 1. 1. 2.]
 [2. 4. 0. 1. 1.]
 [0. 0. 4. 3. 1.]
 [4. 3. 4. 1. 1.]]

Process 1 - Rows 5-9
x[5] = 5
x[6] = 7
x[7] = 7
x[8] = 7
x[9] = 4

Process 2 - Column 2
Block 0 - Rows 0-4
[[3. 4. 0. 1. 0.]
 [4. 4. 2. 2. 1.]
 [3. 4. 2. 4. 4.]
 [4. 4. 4. 2. 0.]
 [0. 0. 0. 2. 4.]]

Block 1 - Rows 5-9
[[0. 4. 3. 4. 2.]
 [3. 0. 4. 3. 3.]
 [4. 1. 3. 4. 1.]
 [1. 0. 4. 2. 4.]
 [1. 0. 1. 3. 0.]]

Block 2 - Rows 10-14
[[0. 1. 2. 2. 1.]
 [4. 4. 1. 0. 0.]
 [0. 0. 2. 0. 2.]
 [0. 1. 1. 0. 1.]
 [0. 1. 2. 0. 3.]]

Block 3 - Rows 15-19
[[3. 1. 2. 2. 2.]
 [1. 4. 4. 2. 3.]
 [1. 1. 3. 3. 3.]
 [4. 1. 1. 0. 3.]
 [3. 1. 3. 1. 2.]]

Process 2 - Rows 10-14
x[10] = 3
x[11] = 6
x[12] = 3
x[13] = 7
x[14] = 3

Process 3 - Column 3
Block 0 - Rows 0-4
[[0. 0. 0. 0. 4.]
 [3. 3. 1. 4. 2.]
 [3. 4. 0. 4. 0.]
 [4. 1. 2. 2. 2.]
 [4. 2. 1. 4. 1.]]

Block 1 - Rows 5-9
[[2. 3. 2. 3. 0.]
 [4. 2. 4. 3. 1.]
 [1. 0. 0. 2. 4.]
 [1. 2. 0. 3. 2.]
 [3. 0. 4. 1. 1.]]

Block 2 - Rows 10-14
[[2. 3. 3. 3. 0.]
 [1. 3. 1. 0. 1.]
 [1. 1. 0. 4. 0.]
 [4. 2. 0. 3. 0.]
 [3. 3. 0. 4. 1.]]

Block 3 - Rows 15-19
[[2. 1. 2. 4. 0.]
 [2. 2. 3. 1. 0.]
 [1. 0. 3. 3. 0.]
 [0. 4. 2. 2. 4.]
 [2. 4. 4. 1. 1.]]

Process 3 - Rows 15-19
x[15] = 6
x[16] = 4
x[17] = 1
x[18] = 7
x[19] = 6

Process 0 - Rows 0-4
y[0] = 235
y[1] = 252
y[2] = 282
y[3] = 316
y[4] = 194

Process 1 - Rows 5-9
y[5] = 226
y[6] = 263
y[7] = 189
y[8] = 210
y[9] = 192

Process 2 - Rows 10-14
y[10] = 130
y[11] = 147
y[12] = 163
y[13] = 188
y[14] = 254

Process 3 - Rows 15-19
y[15] = 245
y[16] = 191
y[17] = 187
y[18] = 197
y[19] = 221

Process 0 - Rows 0-19
y[0] = 235
y[1] = 252
y[2] = 282
y[3] = 316
y[4] = 194
y[5] = 226
y[6] = 263
y[7] = 189
y[8] = 210
y[9] = 192
y[10] = 130
y[11] = 147
y[12] = 163
y[13] = 188
y[14] = 254
y[15] = 245
y[16] = 191
y[17] = 187
y[18] = 197
y[19] = 221
```

Within mat/scripts/, there is verify.py, a Python script written to carry out the matrix-vector product to confirm our results - run using python3 scripts/verify.py. Indeed, if we compare the two outputs, we can see that the results match:

```bash
Matrix dimension: 20
Vector length: 20

Matrix
[[4. 4. 3. 1. 2. 3. 4. 3. 4. 1. 3. 4. 0. 1. 0. 0. 0. 0. 0. 4.]
 [0. 4. 0. 3. 0. 2. 4. 3. 2. 2. 4. 4. 2. 2. 1. 3. 3. 1. 4. 2.]
 [4. 3. 3. 2. 3. 3. 0. 4. 2. 0. 3. 4. 2. 4. 4. 3. 4. 0. 4. 0.]
 [3. 4. 3. 4. 4. 4. 0. 4. 4. 4. 4. 4. 4. 2. 0. 4. 1. 2. 2. 2.]
 [1. 1. 1. 2. 1. 0. 3. 3. 1. 4. 0. 0. 0. 2. 4. 4. 2. 1. 4. 1.]
 [1. 4. 0. 3. 2. 2. 0. 4. 1. 2. 0. 4. 3. 4. 2. 2. 3. 2. 3. 0.]
 [4. 4. 4. 1. 3. 3. 2. 2. 2. 1. 3. 0. 4. 3. 3. 4. 2. 4. 3. 1.]
 [1. 2. 0. 1. 2. 1. 3. 1. 1. 3. 4. 1. 3. 4. 1. 1. 0. 0. 2. 4.]
 [2. 4. 3. 4. 1. 4. 0. 0. 3. 0. 1. 0. 4. 2. 4. 1. 2. 0. 3. 2.]
 [2. 3. 0. 0. 4. 3. 2. 3. 3. 2. 1. 0. 1. 3. 0. 3. 0. 4. 1. 1.]
 [2. 1. 0. 0. 1. 0. 0. 3. 0. 3. 0. 1. 2. 2. 1. 2. 3. 3. 3. 0.]
 [1. 1. 1. 0. 2. 0. 3. 2. 1. 3. 4. 4. 1. 0. 0. 1. 3. 1. 0. 1.]
 [4. 0. 1. 0. 3. 2. 4. 1. 3. 1. 0. 0. 2. 0. 2. 1. 1. 0. 4. 0.]
 [1. 1. 0. 3. 4. 0. 3. 0. 4. 4. 0. 1. 1. 0. 1. 4. 2. 0. 3. 0.]
 [0. 0. 4. 4. 4. 3. 4. 2. 4. 1. 0. 1. 2. 0. 3. 3. 3. 0. 4. 1.]
 [3. 4. 3. 4. 0. 1. 3. 3. 3. 3. 3. 1. 2. 2. 2. 2. 1. 2. 4. 0.]
 [1. 2. 3. 1. 3. 2. 1. 1. 1. 2. 1. 4. 4. 2. 3. 2. 2. 3. 1. 0.]
 [3. 3. 2. 1. 2. 2. 4. 0. 1. 1. 1. 1. 3. 3. 3. 1. 0. 3. 3. 0.]
 [3. 0. 3. 4. 0. 0. 0. 4. 3. 1. 4. 1. 1. 0. 3. 0. 4. 2. 2. 4.]
 [3. 3. 3. 0. 2. 4. 3. 4. 1. 1. 3. 1. 3. 1. 2. 2. 4. 4. 1. 1.]]

Vector
[4. 5. 6. 7. 7. 5. 7. 7. 7. 4. 3. 6. 3. 7. 3. 6. 4. 1. 7. 6.]

Result
y[0] = 235.00
y[1] = 252.00
y[2] = 282.00
y[3] = 316.00
y[4] = 194.00
y[5] = 226.00
y[6] = 263.00
y[7] = 189.00
y[8] = 210.00
y[9] = 192.00
y[10] = 130.00
y[11] = 147.00
y[12] = 163.00
y[13] = 188.00
y[14] = 254.00
y[15] = 245.00
y[16] = 191.00
y[17] = 187.00
y[18] = 197.00
y[19] = 221.00
```
