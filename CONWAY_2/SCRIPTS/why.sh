#!/bin/bash
#
#SBATCH --job-name=test_mpio
#SBATCH --qos janus
#SBATCH --nodes 9
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:5:00
#SBATCH --output RESULTS/test_mpio.txt

module load slurm
module load gcc
module load openmpi

mpirun -np 9 ./a.out
