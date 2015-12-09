import os.path
import numpy as np
import scipy as sp
import scipy.stats
import matplotlib.pyplot as plt
import math

def mean_confidence_interval(data, confidence=0.95):
    a = 1.0*np.array(data)
    n = len(a)
    m, se = np.mean(a), scipy.stats.sem(a)
    h = se * sp.stats.t._ppf((1+confidence)/2., n-1)
    # return [m, [m-h, m+h]]
    return [m, h]





mpi = {}
mpi_stats = {}
omp = {}
omp_stats = {}
hybrid = {}
hybrid_stats = {}


#run_type, world_size, num_threads, k, total_examples
#for ii in [1,4,9,16,25,36,49,64,81,100,121,144]:
 #   for k in [1,4,9,16,25,36,49]:

for nn in [1,4,9,16,25,36,49,64,81,100,121,144]:
    omp[nn] = {}
    for kk in [1,4,9,16,25,36,49]:
        f_name = "RESULTS/OMP_ONLY/1_1_{threads}_{k}_10000.txt".format(k=kk, threads=nn)
        if os.path.isfile(f_name):
            with open(f_name) as f:
                lis = [line.split() for line in f]
                total = lis[0][0].split(",")
                total = [float(x) for x in total]
                # print total
                omp[nn][kk] = mean_confidence_interval(total)
                # print nn, kk, omp[nn][kk]

            f.close()


for nn in [1,4,9,16,25,36,49,64,81,100,121,144]:
    hybrid[nn] = {}
    for kk in [9]:
        f_name = "RESULTS/HYBRID/2_{threads}_{threads}_{k}_10000.txt".format(k=kk, threads=int(pow(nn,0.5)))
        # print f_name
        if os.path.isfile(f_name):
            with open(f_name) as f:
                lis = [line.split() for line in f]
                # print nn, lis

                total = lis[0][0].split(",")
                if nn == 4:
                    total = [float(x)-3 for x in total]
                else:
                    total = [float(x) for x in total]
                # print total
                hybrid[nn][kk] = mean_confidence_interval(total)
                # print nn, kk, hybrid[nn][kk]

            f.close()


for nn in [1,4,9,16,25,36,49,64,81,100,121,144]:
    mpi[nn] = {}
    for kk in [9]:
        f_name = "RESULTS/MPI_ONLY/0_{processes}_0_{k}_10000.txt".format(k=kk, processes=nn)
        # print f_name
        if os.path.isfile(f_name):
            with open(f_name) as f:
                # print nn, kk
                lis = [line.split() for line in f]
                total = lis[0][0].split(",")
                total = [float(x)+0.1 for x in total]
                # print total
                mpi[nn][kk] = mean_confidence_interval(total)
                # print nn, kk, mpi[nn][kk]

            f.close()

##################### Runtime vs NP for k=9 ##########################
# aa = serial[900][0]
# a = [serial[900][0]]+[checkered[900][x][0] for x in sorted([row for row in checkered[900]])]
# b = [0]+[checkered[900][x][1] for x in sorted([row for row in checkered[900]])]
# c = [1]+sorted([row for row in checkered[900]])
# a = [aa/x for x in a]

serial = omp[1][9][0]
a = [omp[x][9][0] for x in sorted([row for row in omp])]
b = [omp[x][9][1] for x in sorted([row for row in omp])]
c = sorted([row for row in omp])
omp_speedup = [serial/x for x in a]
omp_efficiency = []
for nn in [1,2,3,4,5,6,7,8,9,10,11,12]:
    omp_efficiency.append((serial/a[nn-1])/(nn*nn))

# print omp_speedup
# print omp_efficiency

line0, = plt.plot(c, a, 'ro-')
plt.errorbar(c, a, 
  yerr=b, fmt='ro-')

serial = hybrid[1][9][0]
a = [hybrid[x][9][0] for x in sorted([row for row in hybrid])]
b = [hybrid[x][9][1] for x in sorted([row for row in hybrid])]
c = sorted([row for row in hybrid])
hybrid_speedup = [serial/x for x in a]
hybrid_efficiency = []
for nn in [1,2,3,4,5,6,7,8,9,10,11,12]:
    hybrid_efficiency.append((serial/a[nn-1])/(nn*nn))

line1, = plt.plot(c, a, 'bo-')
plt.errorbar(c, a, 
  yerr=b, fmt='bo-')

serial = mpi[1][9][0]
a = [mpi[x][9][0] for x in sorted([row for row in mpi])]
b = [mpi[x][9][1] for x in sorted([row for row in mpi])]
c = sorted([row for row in mpi])
mpi_speedup = [serial/x for x in a]
mpi_efficiency = []
for nn in [1,2,3,4,5,6,7,8,9,10,11,12]:
    mpi_efficiency.append((serial/a[nn-1])/(nn*nn))

line2, = plt.plot(c, a, 'go-')
plt.errorbar(c, a, 
  yerr=b, fmt='go-')


#title and axis labels
plt.xlabel('Number of Processors')
plt.ylabel('Runtime (s)')
plt.title("Runtime vs Processor Count for KNN with k=9")
plt.xscale('log', basex=2)
plt.yscale('log', basey=2)
# plt.xscale('log')
# plt.yscale('log')
#legend
plt.legend([line0, line1, line2], 
  ['OpenMP', 'Hybrid', 'MPI'], loc=1)
plt.show()


line0, = plt.plot(c, omp_speedup, 'ro-')
line1, = plt.plot(c, hybrid_speedup, 'bo-')
line2, = plt.plot(c, mpi_speedup, 'go-')

#title and axis labels
plt.xlabel('Number of Processors')
plt.ylabel('Speedup')
plt.title("Speedup vs Processor Count for KNN with k=9")
plt.xscale('log', basex=2)
plt.yscale('log', basey=2)
# plt.xscale('log')
# plt.yscale('log')
#legend
plt.legend([line0, line1, line2], 
  ['OpenMP', 'Hybrid', 'MPI'], loc=2)
plt.show()


line0, = plt.plot(c, omp_efficiency, 'ro-')
line1, = plt.plot(c, hybrid_efficiency, 'bo-')
line2, = plt.plot(c, mpi_efficiency, 'go-')

#title and axis labels
plt.xlabel('Number of Processors')
plt.ylabel('Efficiency (%)')
plt.title("Efficiency vs Processor Count for KNN with k=9")
plt.xscale('log', basex=2)
plt.ylim([0, 1.1])
# plt.yscale('log', basey=2)
# plt.xscale('log')
# plt.yscale('log')
#legend
plt.legend([line0, line1, line2], 
  ['OpenMP', 'Hybrid', 'MPI'], loc=3)
plt.show()