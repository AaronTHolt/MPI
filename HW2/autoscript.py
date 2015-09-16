import subprocess
import os
from time import sleep

size = [1, 2**3, 2**6, 2**9, 2**12, 2**15, 2**18, 2**20, 4]
np=32
type1 = 'l'
for s in size:
	 
	script = '''#!/bin/bash
#
#SBATCH --job-name=mpi_bfly_{n}_{t}
#SBATCH --qos janus
#SBATCH --nodes {n}
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:3:00
#SBATCH --output results_{n}_{t}_{MB}.out

module load slurm
module load gcc
module load openmpi/1.8.5

mpirun -np {n} ./bfly.out -{t} {MB}
	'''.format(n=np, t=type1, MB=s)

	f = open('script_{t}_{MB}'.format(t=type1, MB=s), 'w')
	for line in script:
		f.write(line)
	f.close

sleep(2)

for s in size:
	cmd = 'sbatch script_{t}_{MB}'.format(t=type1, MB=s)
	print cmd

	p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
	out, err = p.communicate()

	print err
	sleep(0.1)




