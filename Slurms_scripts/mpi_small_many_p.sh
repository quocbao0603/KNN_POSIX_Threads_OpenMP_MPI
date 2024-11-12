#!/bin/bash
#SBATCH --ntasks 128
#SBATCH --cpus-per-task 4
#SBATCH --mem 1G
#SBATCH --output mpi_small_many_p_3.log

module load mpi/openmpi-4.1.6

echo "Job begins"
mpirun -np 32 mpi datasets/small-train.arff datasets/small-test.arff 3
mpirun -np 64 mpi datasets/small-train.arff datasets/small-test.arff 3
mpirun -np 128 mpi datasets/small-train.arff datasets/small-test.arff 3
echo "Job ends"