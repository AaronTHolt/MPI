import subprocess
import os
from time import sleep

for ii in [1,4,9,16,25,36]:
	script = '''#!/bin/bash
#
#SBATCH --job-name=checkered
#SBATCH --qos janus
#SBATCH --nodes {NP}
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:10:00
#SBATCH --output RESULTS/checkered_900_{NP}.txt


mpirun -np {NP} ./hw7.1-holtat 2 60 1000 1000
'''.format(NP=ii)

	f = open('SCRIPTS/checkered_900_{NP}.sh'.format(NP=ii), 'w')
	for line in script:
		f.write(line)
	f.close

	f = open('SCRIPTS/why.sh', 'w')
	for line in script:
		f.write(line)
	f.close

	cmd = 'sbatch SCRIPTS/checkered_900_{NP}.sh'.format(NP=ii)
	print cmd

	p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
	out, err = p.communicate()

# print err




