#!/bin/bash
#
#SBATCH --job-name=all_4_144
#SBATCH --qos janus
#SBATCH --nodes 4
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:5:00
#SBATCH --output RESULTS/all_4_144

module load slurm
module load gcc
module load openmpi

mpirun -np 4 ./hw4.2-holtat.out 144
