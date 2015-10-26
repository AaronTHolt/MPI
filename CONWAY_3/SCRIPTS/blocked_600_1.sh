#!/bin/bash
#
#SBATCH --job-name=blocked_1
#SBATCH --qos janus
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/blocked_600_1.txt


mpirun -np 1 ./hw7.1-holtat 1 60 1000 1000
