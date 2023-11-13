#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include <time.h>
#include <sys/time.h>

MPI_Status status;

// Function to print a matrix

int main(int argc, char **argv) {
    int processCount, processId, slaveTaskCount, source, dest, rows, offset;

    struct timeval start, stop;
    // MPI environment is initialized
    MPI_Init(&argc, &argv);
    // Each process gets a unique ID (rank)
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    // Number of processes in the communicator is assigned to variable -> processCount
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);

    // Check if the correct number of command line arguments is provided
    if (argc != 2) {
        if (processId == 0) {
            printf("Usage: mpiexec -n <number_of_processes> ./matrix_sum_mpi <matrix_size>\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    // Number of rows and columns in a matrix
    const int N = atoi(argv[1]);

    // Number of slave tasks will be assigned to variable -> slaveTaskCount
    slaveTaskCount = processCount - 1;

    // Matrix holders are created
    double matrix_a[N][N], matrix_b[N][N], matrix_c[N][N];

    // Root (Master) process
    if (processId == 0) {
        // Matrix A and Matrix B both will be filled with random numbers
        srand(time(NULL));
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matrix_a[i][j] = rand() % 10;
                matrix_b[i][j] = rand() % 10;
            }
        }

        printf("\n\t\tMatrix - Matrix Multiplication using MPI\n");

        // Print Matrix A
        printf("\nMatrix A\n");
        for (int i = 0; i<N; i++) {
          for (int j = 0; j<N; j++)
            printf("%.0f\t", matrix_a[i][j]);
          printf ("\n");
        }
        printf ("\n");

        // Print Matrix B
        printf("\nMatrix B\n");
        for (int i = 0; i<N; i++) {
          for (int j = 0; j<N; j++)
            printf("%.0f\t", matrix_b[i][j]);
          printf ("\n");
        }
        printf ("\n");

        // Print Matrix B
        printf("\nMatrix B\n");
        for (int i = 0; i<N; i++) {
          for (int j = 0; j<N; j++)
            printf("%.0f\t", matrix_b[i][j]);
          printf ("\n");
        }
        printf ("\n");

        // Determine number of rows of the Matrix A that is sent to each slave process
        rows = N / slaveTaskCount;
        // Offset variable determines the starting point of the row which is sent to slave processes
        offset = 0;

        // Calculation details are assigned to slave tasks. Process 1 onwards;
        // Each message's tag is 1
        //
        gettimeofday(&start, NULL);

        for (dest = 1; dest <= slaveTaskCount; dest++) {
            // Acknowledging the offset of the Matrix A
            MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            // Acknowledging the number of rows
            MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            // Send rows of the Matrix A which will be assigned to slave processes to compute
            MPI_Send(&matrix_a[offset][0], rows * N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
            // Matrix B is sent
            MPI_Send(&matrix_b, N * N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);

            // Offset is modified according to the number of rows sent to each process
            offset = offset + rows;
        }

        // Root process waits until each slave process sends its calculated result with message tag 2
        for (int i = 1; i <= slaveTaskCount; i++) {
            source = i;
            // Receive the offset of a particular slave process
            MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
            // Receive the number of rows that each slave process processed
            MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
            // Calculated rows of each process will be stored in Matrix C according to their offset and
            // the processed number of rows
            MPI_Recv(&matrix_c[offset][0], rows * N, MPI_DOUBLE, source, 2, MPI_COMM_WORLD, &status);
        }

        gettimeofday(&stop, NULL);

        printf("\nResult Matrix C = Matrix A * Matrix B:\n\n");
        for (int i = 0; i<N; i++) {
          for (int j = 0; j<N; j++)
            printf("%.0f\t", matrix_c[i][j]);
          printf ("\n");
        }
        printf ("\n");
        printf("Elapsed time: %lu microseconds\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }



    // Slave Processes
    if (processId > 0) {
        // Source process ID is defined
        source = 0;

        // Slave process waits for the message buffers with tag 1, that Root process sent
        // Each process will receive and execute this separately on their processes

        // The slave process receives the offset value sent by the root process
        MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
        // The slave process receives the number of rows sent by the root process
        MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
        // The slave process receives the sub-portion of Matrix A which is assigned by Root
        MPI_Recv(&matrix_a, rows * N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
        // The slave process receives Matrix B
        MPI_Recv(&matrix_b, N * N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);

        // Matrix multiplication
        for (int k = 0; k < N; k++) {
            for (int i = 0; i < rows; i++) {
                // Set initial value of the row summation
                matrix_c[i][k] = 0.0;
                // Matrix A's element(i, j) will be multiplied with Matrix B's element(j, k)
                for (int j = 0; j < N; j++)
                    matrix_c[i][k] = matrix_c[i][k] + matrix_a[i][j] * matrix_b[j][k];
            }
        }

        // Calculated result will be sent back to Root process (process 0) with message tag 2

        // Offset will be sent to Root, which determines the starting point of the calculated
        // value in matrix C
        MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        // Number of rows the process calculated will be sent to the root process
        MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        // Resulting matrix with calculated rows will be sent to the root process
        MPI_Send(&matrix_c, rows * N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}