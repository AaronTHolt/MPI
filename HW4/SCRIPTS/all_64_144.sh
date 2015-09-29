#!/bin/bash
#
#SBATCH --job-name=all_64_144
#SBATCH --qos janus
#SBATCH --nodes 64
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:5:00
#SBATCH --output RESULTS/all_64_144

module load slurm
module load gcc
module load openmpi

mpirun -np 64 ./hw4.2-holtat.out 144
