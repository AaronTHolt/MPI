#!/bin/bash
#
#SBATCH --job-name=checkered
#SBATCH --qos janus
#SBATCH --nodes 16
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/checkered.txt

module load slurm
module load gcc
module load openmpi

mpirun -np 16 ./hw6.1-holtat -v -a 2 501 100 1-500
