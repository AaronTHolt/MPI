import subprocess
import os
from time import sleep

for ii in [144,576,1296,2304]:

	script = '''#!/bin/bash
#
#SBATCH --job-name=all_{np}_{size}
#SBATCH --qos janus
#SBATCH --nodes {np}
#SBATCH --ntasks-per-node 1
#SBATCH --time 03:45:00
#SBATCH --output RESULTS/all_{np}_{size}

module load slurm
module load gcc
module load openmpi

mpirun -np {np} ./hw4.2-holtat.out {size}
'''.format(np=144, size=ii)

	f = open('SCRIPTS/all_{np}_{size}.sh'.format(np=144, size=ii), 'w')
	for line in script:
		f.write(line)
	f.close

	f = open('SCRIPTS/all_whyisthishappening.sh', 'w')
	for line in script:
		f.write(line)
	f.close


	cmd = 'sbatch SCRIPTS/all_{np}_{size}.sh'.format(np=144, size=ii)
	print cmd

	p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
	out, err = p.communicate()

	print err
	sleep(0.1)