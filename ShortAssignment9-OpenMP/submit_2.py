import subprocess
import os
from time import sleep

# for ii in [4,16,36,64,144]:

script = '''#!/bin/bash
#
#SBATCH --job-name=hw9.2
#SBATCH --qos janus
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:25:00
#SBATCH --output RESULTS/hw9_2

./hw9.2-holtat-openmp-compiletime

'''

f = open('SCRIPTS/hw9_2.sh', 'w')
for line in script:
	f.write(line)
f.close

f = open('SCRIPTS/all_whyisthishappening.sh', 'w')
for line in script:
	f.write(line)
f.close


cmd = 'sbatch SCRIPTS/hw9_2.sh'
print cmd

p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
out, err = p.communicate()

print err
sleep(0.1)


