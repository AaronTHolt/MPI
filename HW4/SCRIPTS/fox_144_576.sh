#!/bin/bash
#
#SBATCH --job-name=fox_144_576
#SBATCH --qos janus
#SBATCH --nodes 144
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:5:00
#SBATCH --output RESULTS/fox_144_576

module load slurm
module load gcc
module load openmpi

mpirun -np 144 ./hw4.3-holtat.out 576
