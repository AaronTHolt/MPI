#!/bin/bash
#
#SBATCH --job-name=blocked_25
#SBATCH --qos janus
#SBATCH --nodes 25
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/blocked_900_25.txt


mpirun -np 25 ./hw7.1-holtat 1 60 1000 1000
