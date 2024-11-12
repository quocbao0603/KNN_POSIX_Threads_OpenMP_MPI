#!/bin/bash
#SBATCH --ntasks 1
#SBATCH --cpus-per-task 4
#SBATCH --mem 1G
#SBATCH --output serial_medium.log
#SBATCH --mail-user=tranq3@vcu.edu

echo "Job begins"
./serial datasets/medium-train.arff datasets/medium-test.arff 3
echo "Job ends"