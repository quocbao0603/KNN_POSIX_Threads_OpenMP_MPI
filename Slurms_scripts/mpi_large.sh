#!/bin/bash
#SBATCH --ntasks 16
#SBATCH --cpus-per-task 4
#SBATCH --mem 1G
#SBATCH --output mpi_large_3.log

module load mpi/openmpi-4.1.6

echo "Job begins"
mpirun -np 1 mpi datasets/large-train.arff datasets/large-test.arff 3
mpirun -np 2 mpi datasets/large-train.arff datasets/large-test.arff 3
mpirun -np 4 mpi datasets/large-train.arff datasets/large-test.arff 3
mpirun -np 8 mpi datasets/large-train.arff datasets/large-test.arff 3
mpirun -np 16 mpi datasets/large-train.arff datasets/large-test.arff 3
echo "Job ends"