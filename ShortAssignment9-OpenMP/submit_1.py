import subprocess
import os
from time import sleep

# for ii in [4,16,36,64,144]:

script = '''#!/bin/bash
#
#SBATCH --job-name=hw9.1
#SBATCH --qos janus
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 1
#SBATCH --time 00:25:00
#SBATCH --output RESULTS/hw9_1

./hw9.1-holtat-openmp-manual

'''

f = open('SCRIPTS/hw9_1.sh', 'w')
for line in script:
	f.write(line)
f.close

f = open('SCRIPTS/all_whyisthishappening.sh', 'w')
for line in script:
	f.write(line)
f.close


cmd = 'sbatch SCRIPTS/hw9_1.sh'
print cmd

p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
out, err = p.communicate()

print err
sleep(0.1)


