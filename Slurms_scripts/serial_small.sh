#!/bin/bash
#SBATCH --ntasks 1
#SBATCH --cpus-per-task 4
#SBATCH --mem 1G
#SBATCH --output serial_small.log
#SBATCH --mail-user=tranq3@vcu.edu

echo "Job begins"
./serial datasets/small-train.arff datasets/small-test.arff 3
echo "Job ends"