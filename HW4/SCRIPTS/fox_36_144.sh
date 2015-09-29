#!/bin/bash
#
#SBATCH --job-name=fox_36_144
#SBATCH --qos janus
#SBATCH --nodes 36
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:5:00
#SBATCH --output RESULTS/fox_36_144

module load slurm
module load gcc
module load openmpi

mpirun -np 36 ./hw4.3-holtat.out 144
