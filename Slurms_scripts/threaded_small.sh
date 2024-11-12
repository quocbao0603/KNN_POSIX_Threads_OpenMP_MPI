#!/bin/bash
#SBATCH --ntasks 1
#SBATCH --cpus-per-task 4
#SBATCH --mem 1G
#SBATCH --output threaded_small_2.log
#SBATCH --mail-user=tranq3@vcu.edu

echo "Job begins"
./threaded datasets/small-train.arff datasets/small-test.arff 3 1
./threaded datasets/small-train.arff datasets/small-test.arff 3 2
./threaded datasets/small-train.arff datasets/small-test.arff 3 4
./threaded datasets/small-train.arff datasets/small-test.arff 3 8
./threaded datasets/small-train.arff datasets/small-test.arff 3 16
./threaded datasets/small-train.arff datasets/small-test.arff 3 32
./threaded datasets/small-train.arff datasets/small-test.arff 3 64
./threaded datasets/small-train.arff datasets/small-test.arff 3 128
echo "Job ends"