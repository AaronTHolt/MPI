#!/bin/bash
#
#SBATCH --job-name=blocked_16
#SBATCH --qos janus
#SBATCH --nodes 16
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/blocked_300_16.txt


mpirun -np 16 ./hw7.1-holtat 1 60 1000 1000
