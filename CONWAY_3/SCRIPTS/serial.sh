#!/bin/bash
#
#SBATCH --job-name=serial
#SBATCH --qos janus
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/serial_300.txt


mpirun -np 1 ./hw7.1-holtat 0 60 1000 100
