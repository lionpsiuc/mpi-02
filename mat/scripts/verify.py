import struct
import numpy as np


def read_matrix_file(filename):
    with open(filename, "rb") as f:

        # Read dimension
        dim = struct.unpack("i", f.read(4))[0]
        print(f"Matrix dimension: {dim}")

        # Initialize matrix
        matrix = np.zeros((dim, dim))

        # Read block columns
        for col_block in range(4):  # Four block columns
            for row_block in range(4):  # Four blocks per column

                # Read 5 x 5 block
                for i in range(5):
                    for j in range(5):
                        value = struct.unpack("d", f.read(8))[0]
                        row = row_block * 5 + i
                        col = col_block * 5 + j
                        matrix[row, col] = value

        return matrix


def read_vector_file(filename):
    with open(filename, "rb") as f:

        # Read length
        length = struct.unpack("i", f.read(4))[0]
        print(f"Vector length: {length}")

        # Read vector values
        vector = []
        for i in range(length):
            value = struct.unpack("d", f.read(8))[0]
            vector.append(value)

        return np.array(vector)


def main():
    A = read_matrix_file("mat-d20-b5-p4.bin")
    x = read_vector_file("x-d20.txt.bin")

    # Perform multiplication
    y = np.dot(A, x)

    # Print matrix
    print("\nMatrix")
    print(A)

    # Print vector
    print("\nVector")
    print(x)

    # Print result
    print("\nResult")
    for i, val in enumerate(y):
        print(f"y[{i}] = {val:.2f}")


if __name__ == "__main__":
    main()
