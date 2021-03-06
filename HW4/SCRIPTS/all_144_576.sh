#!/bin/bash
#
#SBATCH --job-name=all_144_576
#SBATCH --qos janus
#SBATCH --nodes 144
#SBATCH --ntasks-per-node 1
#SBATCH --time 03:45:00
#SBATCH --output RESULTS/all_144_576

module load slurm
module load gcc
module load openmpi

mpirun -np 144 ./hw4.2-holtat.out 576
