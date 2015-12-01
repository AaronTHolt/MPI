#!/bin/bash
#
#SBATCH --job-name=hw93_1
#SBATCH --qos janus
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:25:00
#SBATCH --output RESULTS/hw93_1

./hw9.3-holtat-openmp-runtime

