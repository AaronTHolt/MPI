#!/bin/bash
#
#SBATCH --job-name=hw9.1
#SBATCH --qos janus
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:25:00
#SBATCH --output RESULTS/hw9_1

./hw9.1-holtat-openmp-manual

