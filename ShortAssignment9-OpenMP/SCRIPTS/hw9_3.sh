#!/bin/bash
#
#SBATCH --job-name=hw9.3
#SBATCH --qos janus
#SBATCH --nodes 64
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:25:00
#SBATCH --output RESULTS/hw9_3

./hw9.3-holtat-openmp-runtime

