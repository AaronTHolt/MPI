#!/bin/bash
#
#SBATCH --job-name=serial_576
#SBATCH --qos janus
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:5:00
#SBATCH --output RESULTS/serial_576

module load slurm
module load gcc
module load openmpi

hostname
mpirun -np 1 ./hw4.1-holtat.out 576
