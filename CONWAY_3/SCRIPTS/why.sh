#!/bin/bash
#
#SBATCH --job-name=serial
#SBATCH --qos janus
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/serial_fast_300.txt


mpirun -np 1 ./hw7.2-holtat 0 60 1000 100
