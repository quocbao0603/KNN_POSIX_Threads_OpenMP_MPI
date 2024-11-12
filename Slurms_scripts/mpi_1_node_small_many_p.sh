#!/bin/bash
#SBATCH --ntasks 126
#SBATCH --cpus-per-task 1
#SBATCH --mem 1G
#SBATCH --output mpi_1_node_small_many_p_126.log
#SBATCH --nodes 1
module load mpi/openmpi-4.1.6

echo "Job begins"
mpirun -np 126 mpi datasets/small-train.arff datasets/small-test.arff 3
echo "Job ends"