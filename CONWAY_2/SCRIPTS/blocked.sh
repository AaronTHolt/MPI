#!/bin/bash
#
#SBATCH --job-name=blocked
#SBATCH --qos janus
#SBATCH --nodes 9
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/blocked.txt

module load slurm
module load gcc
module load openmpi

mpirun -np 9 ./hw6.1-holtat -v 1 1001 100 1,2,4-8
