import subprocess
import os
from time import sleep

# for ii in [1,4,9,16,25,36,64,81,100,121,144]:
for ii in [1,4,9,16]:
	for k in [1]:

		script = '''#!/bin/bash
#
#SBATCH --job-name=mpi_only_{NP}
#SBATCH --qos janus
#SBATCH --nodes {NP}
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:15:00
#SBATCH --output RESULTS/mpi_only_{NP}_{k}.txt


mpiexec -np {NP} ./mpi_only 0 {k} 0 10000
'''.format(NP=ii, k=k)

		f = open('SCRIPTS/mpi_only_{NP}_{k}.sh'.format(NP=ii, k=k), 'w')
		for line in script:
			f.write(line)
		f.close

		f = open('SCRIPTS/why.sh', 'w')
		for line in script:
			f.write(line)
		f.close

		cmd = 'sbatch SCRIPTS/mpi_only_{NP}_{k}.sh'.format(NP=ii, k=k)
		print cmd

		p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
		out, err = p.communicate()




