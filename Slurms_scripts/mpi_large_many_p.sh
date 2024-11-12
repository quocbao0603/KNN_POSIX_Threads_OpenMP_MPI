#!/bin/bash
#SBATCH --ntasks 64
#SBATCH --cpus-per-task 4
#SBATCH --mem 1G
#SBATCH --output mpi_large_many_p_2.log

module load mpi/openmpi-4.1.6

echo "Job begins"
mpirun -np 64 mpi datasets/large-train.arff datasets/large-test.arff 3
echo "Job ends"