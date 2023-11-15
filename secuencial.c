#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char **argv) {
    int N;

    if (argc != 2) {
        printf("Usage: ./matrix_sum <matrix_size>\n");
        return EXIT_FAILURE;
    }

    N = atoi(argv[1]);

    double matrix_a[N][N], matrix_b[N][N], matrix_c[N][N];

    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix_a[i][j] = rand() % 10;
            matrix_b[i][j] = rand() % 10;
        }
    }

    struct timeval start, stop;
    gettimeofday(&start, NULL);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix_c[i][j] = 0.0;
            for (int k = 0; k < N; k++) {
                matrix_c[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }

    gettimeofday(&stop, NULL);

    printf("\nResult Matrix C = Matrix A * Matrix B:\n\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%.0f\t", matrix_c[i][j]);
        }
        printf("\n");
    }

    printf("\nElapsed time: %lu microseconds\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);

    return EXIT_SUCCESS;
}
