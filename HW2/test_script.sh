#!/bin/bash
#
#SBATCH --job-name=mpi_bfly
#SBATCH --qos janus
#SBATCH --nodes 32
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:3:00
#SBATCH --output results.out

module load slurm
module load gcc
module load openmpi/1.8.5

mpirun -np 32 ./bfly.out -h
