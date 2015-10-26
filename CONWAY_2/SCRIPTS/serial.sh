#!/bin/bash
#
#SBATCH --job-name=serial
#SBATCH --qos janus
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/serial.txt


mpirun -np 1 psrun -f ./hw6.1-holtat 0 1 1000 0-10
