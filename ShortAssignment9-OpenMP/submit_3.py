import subprocess
import os
from time import sleep

# for ii in [1,2,4,8,16,32,64]:
for ii in [1]:

	script = '''#!/bin/bash
#
#SBATCH --job-name=hw93_{np}
#SBATCH --qos janus
#SBATCH --nodes {np}
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:25:00
#SBATCH --output RESULTS/hw93_{np}

./hw9.3-holtat-openmp-runtime

'''.format(np=ii)

	f = open('SCRIPTS/hw93_{np}'.format(np=ii), 'w')
	for line in script:
		f.write(line)
	f.close

	f = open('SCRIPTS/all_whyisthishappening.sh', 'w')
	for line in script:
		f.write(line)
	f.close


	cmd = 'sbatch SCRIPTS/hw93_{np}'.format(np=ii)
	print cmd

	p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
	out, err = p.communicate()

	print err
	sleep(0.1)


