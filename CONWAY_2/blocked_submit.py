import subprocess
import os
from time import sleep

# cmd2 = 'mpirun -np 16 ./hw6.1-holtat -v -a 1 51 50 1-50'
# cmd2 = 'mpirun -np 16 ./hw6.1-holtat -v -a 1 1001 1000 1,2,3,4-6'
cmd2 = 'mpirun -np 16 ./hw6.1-holtat -v 1 1001 1000'

script = '''#!/bin/bash
#
#SBATCH --job-name=blocked
#SBATCH --qos janus
#SBATCH --nodes 16
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/blocked.txt

module load slurm
module load gcc
module load openmpi

{cc}
'''.format(cc=cmd2)

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
print cmd2

p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
out, err = p.communicate()

# print err




