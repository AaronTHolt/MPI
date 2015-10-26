#!/bin/bash
#
#SBATCH --job-name=blocked_9
#SBATCH --qos janus
#SBATCH --nodes 9
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/blocked_600_9.txt


mpirun -np 9 ./hw7.1-holtat 1 60 1000 1000
