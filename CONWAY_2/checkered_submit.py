import subprocess
import os
from time import sleep

cmd2 = 'mpirun -np 16 ./hw6.1-holtat -v 2 1001 1000'

script = '''#!/bin/bash
#
#SBATCH --job-name=checkered
#SBATCH --qos janus
#SBATCH --nodes 16
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/checkered.txt

module load slurm
module load gcc
module load openmpi

{cc}
'''.format(cc=cmd2)

f = open('SCRIPTS/checkered.sh', 'w')
for line in script:
	f.write(line)
f.close

f = open('SCRIPTS/why.sh', 'w')
for line in script:
	f.write(line)
f.close

cmd = 'sbatch SCRIPTS/checkered.sh'
print cmd
print cmd2

p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
out, err = p.communicate()

# print err




