# Tarea 02 MPI
Este repositorio posee el codigo para solucionar la tarea propuesta en el curso de Arquitectura de Computadores II de Ing. en Computadores, II Semestre 2023.

Primero cuenta con un codigo en c optimizado para realizar una multiplicacion de operaciones para matrices desde 2x2 hasta mas en donde se toma en cuenta la cantidad de procesos asignados. 

Dicha implementacion cuenta con optimizacion del calculo por medio del protocolo de Message Passing Interface o MPI y usa operaciones como las mostradas en el siguiente codigo: 

```c
Root Processes:
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

Worker Processes
        MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
        // The slave process receives the number of rows sent by the root process
        MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
        // The slave process receives the sub-portion of Matrix A which is assigned by Root
        MPI_Recv(&matrix_a, rows * N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
        // The slave process receives Matrix B
        MPI_Recv(&matrix_b, N * N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);

        MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        // Number of rows the process calculated will be sent to the root process
        MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        // Resulting matrix with calculated rows will be sent to the root process
        MPI_Send(&matrix_c, rows * N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
```

Donde el proceso Root enviara informacion como el offset, rows, matriz_b y la matriz_a con offset para cada uno de los workers con MPI_Send y luego recibe la solucion del extracto enviado por medio de MPI_Recv. Los workers hacen el proceso invers donde primero esperar su extracto de mensaje para luego de trabajarlo devolver los resultados obtenidos.

Para tener listo este codigo para ejecucion se debe compilar en un ambiente que presente MPICH y otras dependencias basicas para codigo en gcc:

MPICH version =< 3.4.2 

gcc version =< 4.8.5

Para la ejecucion con multiples procesos y nodos se hizo uso de Kabre, un cluster facilitado por el Colaboratio de Nacionaciol de Computacion Avanzada (CNCA) del Centro Nacional de Tecnologia (CeNAT). Por esta razon es necesario que para ejecutarlo tenga una cuenta activa en el o disponga de las misma versiones de las bibliotecas en su ambiente junto a los nodos y procesos por usados en esta implementacion.

Una vez accediendo a su espacio dado por el cluster puede clonar el repositorio, acceder a la carpeta que contiene el codigo y generar el ejecutable:

```bash
mpicc -std=c99 -g -o matrix_sum_mpi matrixSumMPI.c
```

Luego ejecute el siguiente .slurm con el siguiente comando: sbatch job.slurm

```bash
#!/bin/bash
#SBATCH --job-name=my_mpi_job
#SBATCH --output=result.txt
#SBATCH --partition=nu-wide
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=2
#SBATCH --time=00:10:00

# Load necessary modules
module load gcc/7.2.0
module load mpich/4.1.1 

export HYDRA_TOPO_DEBUG=1

# Run the MPI program
srun -n 4 ./matrix_sum_mpi 6
```

## Agradecimiento

Esta investigación contó con el apoyo de una asignación computacional en la supercomputadora Kabré del Centro Nacional de Alta Tecnología de Costa Rica

## Acknowledge
This research was partially supported by a machine allocation on Kabré supercomputer at the Costa Rica National High Technology Center
