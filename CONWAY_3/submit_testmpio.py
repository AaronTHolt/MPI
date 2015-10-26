import subprocess
import os
from time import sleep


script = '''#!/bin/bash
#
#SBATCH --job-name=test_mpio
#SBATCH --qos janus
#SBATCH --nodes 4
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:5:00
#SBATCH --output RESULTS/test_mpio.txt

module load slurm
module load gcc
module load openmpi

mpirun -np 4 ./test_mpio
'''

f = open('SCRIPTS/test_mpio.sh', 'w')
for line in script:
	f.write(line)
f.close

f = open('SCRIPTS/why.sh', 'w')
for line in script:
	f.write(line)
f.close

cmd = 'sbatch SCRIPTS/test_mpio.sh'
print cmd

p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
out, err = p.communicate()

print err




