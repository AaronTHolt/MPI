#!/bin/bash
#
#SBATCH --job-name=blocked_4
#SBATCH --qos janus
#SBATCH --nodes 4
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/blocked_600_4.txt


mpirun -np 4 ./hw7.1-holtat 1 60 1000 1000
