#!/bin/bash
#
#SBATCH --job-name=all_144_144
#SBATCH --qos janus
#SBATCH --nodes 144
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:45:00
#SBATCH --output RESULTS/all_144_144

module load slurm
module load gcc
module load openmpi

mpirun -np 144 ./hw4.2-holtat.out 144
