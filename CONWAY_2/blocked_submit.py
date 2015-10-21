import subprocess
import os
from time import sleep


script = '''#!/bin/bash
#
#SBATCH --job-name=blocked
#SBATCH --qos janus
#SBATCH --nodes 9
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/blocked.txt

module load slurm
module load gcc
module load openmpi

mpirun -np 9 ./hw6.1-holtat -v 1 1001 100 1,2,4-8
'''

f = open('SCRIPTS/blocked.sh', 'w')
for line in script:
	f.write(line)
f.close

f = open('SCRIPTS/why.sh', 'w')
for line in script:
	f.write(line)
f.close

cmd = 'sbatch SCRIPTS/blocked.sh'
print cmd

p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
out, err = p.communicate()

print err




