#!/bin/bash
#
#SBATCH --job-name=blocked
#SBATCH --qos janus
#SBATCH --nodes 16
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/blocked.txt

module load slurm
module load gcc
module load openmpi

mpirun -np 16 ./hw6.1-holtat -v 1 1001 1000
