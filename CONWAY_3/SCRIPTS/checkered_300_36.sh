#!/bin/bash
#
#SBATCH --job-name=checkered
#SBATCH --qos janus
#SBATCH --nodes 36
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/checkered_300_36.txt


mpirun -np 36 ./hw7.1-holtat 2 60 1000 1000
