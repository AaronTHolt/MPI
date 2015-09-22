#!/bin/bash
#
#SBATCH --job-name=pp_ln_small_3073
#SBATCH --qos janus
#SBATCH --nodes 2
#SBATCH --nodelist=node0208,node0209
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:3:00
#SBATCH --output RESULTS/results_pp_ln_small_3073

module load slurm
module load gcc
module load openmpi/1.8.5

hostname
mpirun -np 2 ./pingpong.out 3073
