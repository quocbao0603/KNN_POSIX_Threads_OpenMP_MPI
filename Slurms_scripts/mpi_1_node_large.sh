#!/bin/bash
#SBATCH --ntasks 16
#SBATCH --cpus-per-task 1
#SBATCH --mem 1G
#SBATCH --output mpi_1_node_large_1.log
#SBATCH --nodes 1

module load mpi/openmpi-4.1.6

echo "Job begins"
mpirun -np 1 mpi datasets/large-train.arff datasets/large-test.arff 3
mpirun -np 2 mpi datasets/large-train.arff datasets/large-test.arff 3
mpirun -np 4 mpi datasets/large-train.arff datasets/large-test.arff 3
mpirun -np 8 mpi datasets/large-train.arff datasets/large-test.arff 3
mpirun -np 16 mpi datasets/large-train.arff datasets/large-test.arff 3
echo "Job ends"