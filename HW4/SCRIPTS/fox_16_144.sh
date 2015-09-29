#!/bin/bash
#
#SBATCH --job-name=fox_16_144
#SBATCH --qos janus
#SBATCH --nodes 16
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:5:00
#SBATCH --output RESULTS/fox_16_144

module load slurm
module load gcc
module load openmpi

mpirun -np 16 ./hw4.3-holtat.out 144
