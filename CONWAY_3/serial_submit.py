import subprocess
import os
from time import sleep

# cmd2 = 'mpirun -np 1 psrun -f ./hw6.1-holtat 0 1 1000 0-10'


script = '''#!/bin/bash
#
#SBATCH --job-name=serial
#SBATCH --qos janus
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/serial_300.txt


mpirun -np 1 ./hw7.1-holtat 0 60 1000 100
'''

f = open('SCRIPTS/serial.sh', 'w')
for line in script:
	f.write(line)
f.close

f = open('SCRIPTS/why.sh', 'w')
for line in script:
	f.write(line)
f.close

cmd = 'sbatch SCRIPTS/serial.sh'
print cmd

p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
out, err = p.communicate()

# print err




