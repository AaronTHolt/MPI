import subprocess
import os
from time import sleep

size = []
for ii in range(0,23):
	size.append(2**ii)
size.append(7)
# print size
np=2

for s in size:
	script = '''#!/bin/bash
#
#SBATCH --job-name=pp_sn_{s}
#SBATCH --qos janus
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 2
#SBATCH --time 00:3:00
#SBATCH --output RESULTS/results_pp_sn_{s}

module load slurm
module load gcc
module load openmpi/1.8.5

mpirun -np {n} ./pingpong.out {s}
'''.format(n=np,  s=s)

	f = open('SCRIPTS/script_pp_sn_{s}.sh'.format(s=s), 'w')
	for line in script:
		f.write(line)
	f.close

# sleep(2)

for s in size:
	cmd = 'sbatch SCRIPTS/script_pp_sn_{s}.sh'.format(s=s)
	print cmd

	p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
	out, err = p.communicate()

	print err
	sleep(0.1)




