#!/bin/bash
#SBATCH --ntasks 1
#SBATCH --cpus-per-task 4
#SBATCH --mem 1G
#SBATCH --output openmp_small_3.log
#SBATCH --mail-user=tranq3@vcu.edu

echo "Job begins"
./openmp datasets/small-train.arff datasets/small-test.arff 3 1
./openmp datasets/small-train.arff datasets/small-test.arff 3 2
./openmp datasets/small-train.arff datasets/small-test.arff 3 4
./openmp datasets/small-train.arff datasets/small-test.arff 3 8
./openmp datasets/small-train.arff datasets/small-test.arff 3 16
./openmp datasets/small-train.arff datasets/small-test.arff 3 32
./openmp datasets/small-train.arff datasets/small-test.arff 3 64
./openmp datasets/small-train.arff datasets/small-test.arff 3 128
echo "Job ends"