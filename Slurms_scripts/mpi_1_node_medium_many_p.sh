#!/bin/bash
#SBATCH --ntasks 32
#SBATCH --cpus-per-task 1
#SBATCH --mem 1G
#SBATCH --output mpi_1_node_medium_many_p_32.log
#SBATCH --nodes 1
module load mpi/openmpi-4.1.6

echo "Job begins"
mpirun -np 32 mpi datasets/medium-train.arff datasets/medium-test.arff 3
echo "Job ends"