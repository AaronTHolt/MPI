#!/bin/bash
#
#SBATCH --job-name=pp_rn_262144
#SBATCH --qos janus
#SBATCH --nodes 2
#SBATCH --nodelist=node0207,node0610
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:3:00
#SBATCH --output RESULTS/results_pp_rn_262144

module load slurm
module load gcc
module load openmpi/1.8.5

hostname
mpirun -np 2 ./pingpong.out 262144
