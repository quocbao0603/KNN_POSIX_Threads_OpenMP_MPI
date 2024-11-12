#!/bin/bash
#SBATCH --ntasks 1
#SBATCH --cpus-per-task 4
#SBATCH --mem 1G
#SBATCH --output serial_large_3.log
#SBATCH --mail-user=tranq3@vcu.edu

echo "Job begins"
./serial datasets/large-train.arff datasets/large-test.arff 3
echo "Job ends"