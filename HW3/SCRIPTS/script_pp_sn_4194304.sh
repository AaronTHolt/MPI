#!/bin/bash
#
#SBATCH --job-name=pp_sn_4194304
#SBATCH --qos janus
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 2
#SBATCH --time 00:3:00
#SBATCH --output RESULTS/results_pp_sn_4194304

module load slurm
module load gcc
module load openmpi/1.8.5

mpirun -np 2 ./pingpong.out 4194304
