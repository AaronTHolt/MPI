#!/bin/bash
#
#SBATCH --job-name=hw9.s
#SBATCH --qos janus
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:25:00
#SBATCH --output RESULTS/hw9_s

./serial

