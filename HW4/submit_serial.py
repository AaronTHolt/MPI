import subprocess
import os
from time import sleep


script = '''#!/bin/bash
#
#SBATCH --job-name=serial_576
#SBATCH --qos janus
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:5:00
#SBATCH --output RESULTS/serial_576

module load slurm
module load gcc
module load openmpi

hostname
mpirun -np 1 ./hw4.1-holtat.out 576
'''

f = open('SCRIPTS/serial_576.sh', 'w')
for line in script:
	f.write(line)
f.close

f = open('SCRIPTS/serial_whyisthishappening.sh', 'w')
for line in script:
	f.write(line)
f.close

cmd = 'sbatch SCRIPTS/serial_576.sh'
print cmd

p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
out, err = p.communicate()

print err
sleep(0.1)




