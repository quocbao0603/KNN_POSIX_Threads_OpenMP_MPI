#!/bin/bash
#SBATCH --ntasks 16
#SBATCH --cpus-per-task 4
#SBATCH --mem 1G
#SBATCH --output mpi_medium_3.log

module load mpi/openmpi-4.1.6

echo "Job begins"
mpirun -np 1 mpi datasets/medium-train.arff datasets/medium-test.arff 3
mpirun -np 2 mpi datasets/medium-train.arff datasets/medium-test.arff 3
mpirun -np 4 mpi datasets/medium-train.arff datasets/medium-test.arff 3
mpirun -np 8 mpi datasets/medium-train.arff datasets/medium-test.arff 3
mpirun -np 16 mpi datasets/medium-train.arff datasets/medium-test.arff 3
echo "Job ends"