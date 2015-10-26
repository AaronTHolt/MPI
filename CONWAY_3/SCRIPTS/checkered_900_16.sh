#!/bin/bash
#
#SBATCH --job-name=checkered
#SBATCH --qos janus
#SBATCH --nodes 16
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/checkered_900_16.txt


mpirun -np 16 ./hw7.1-holtat 2 60 1000 1000
