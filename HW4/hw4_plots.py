import numpy as np
import scipy as sp
import scipy.stats
import matplotlib.pyplot as plt


def mean_confidence_interval(data, confidence=0.95):
    a = 1.0*np.array(data)
    n = len(a)
    m, se = np.mean(a), scipy.stats.sem(a)
    h = se * sp.stats.t._ppf((1+confidence)/2., n-1)
    # return [m, [m-h, m+h]]
    return [m, h]

serial_576 = []
serial_576_stats = []
all_4_576 = []
all_4_576_stats = []
fox_4_576 = []
fox_4_576_stats = []

for ii,name in enumerate(['all_4_576','fox_4_576','serial_576']):
	with open('RESULTS/{ff}'.format(ff=name)) as f:
		# print "RESULTS/results_pp_sn_{s}".format(s=size[ii])
		firstline = f.readline()
		lis = firstline.split()
		mylist = lis[0].split(",")
		mylist2 = [float(x)/2 for x in mylist]
		if ii==0:
			all_4_576.append(mylist2)
			all_4_576_stats=mean_confidence_interval(mylist2)
			# print mylist2
		if ii==1:
			fox_4_576.append(mylist2)
			fox_4_576_stats=mean_confidence_interval(mylist2)
		if ii==2:
			serial_576.append(mylist2)
			serial_576_stats=mean_confidence_interval(mylist2)
			# print serial_576_stats
	f.close()



# for ii in [144,576,1296,2304]:
# 	pass
# for ii in [4,16,36,64,144]:
# 




#plot (x,y,style)
line0, = plt.plot(1, serial_576_stats[0], 'ro-')
plt.errorbar(1, serial_576_stats[0], 
	yerr=serial_576_stats[1], fmt='ro-')

line1, = plt.plot(4, fox_4_576_stats[0], 'bs-')
plt.errorbar(4, fox_4_576_stats[0], 
	yerr=fox_4_576_stats[1], fmt='bs-')

line2, = plt.plot(4, all_4_576_stats[0], 'kx-')
plt.errorbar(4, all_4_576_stats[0], 
	yerr=all_4_576_stats[1], fmt='kx-')

#title and axis labels
plt.xlabel('Number of Nodes')
plt.ylabel('Time (s)')
plt.title("Time vs Number of Nodes Serial vs Parallel Algorithms")
#log2 scale
# plt.xscale('log', basex=2)
# plt.yscale('log', basey=2)
#x axis limits (set just above last value)
plt.xlim([0,5])
plt.ylim([0,1.2])
#legend
plt.legend([line0,line1,line2], 
	['Serial', 'Fox', 'Allgather'], loc=1)
plt.show()

# ################################################################

# for ii in range (0,len(small)):
# 	pp_ln_small_stats.append(mean_confidence_interval(pp_ln_small[ii]))
# 	pp_rn_small_stats.append(mean_confidence_interval(pp_rn_small[ii]))

# line0, = plt.plot(small, [row[0]*(10**6) for row in pp_rn_small_stats], 'ro-')
# plt.errorbar(small, [row[0]*(10**6) for row in pp_rn_small_stats], 
# 	yerr=[row[1]*(10**6) for row in pp_rn_small_stats], fmt='ro-')

# line1, = plt.plot(small, [row[0]*(10**6) for row in pp_ln_small_stats], 'bs-')
# plt.errorbar(small, [row[0]*(10**6) for row in pp_ln_small_stats], 
# 	yerr=[row[1]*(10**6) for row in pp_ln_small_stats], fmt='bs-')

# #title and axis labels
# plt.xlabel('Size of buffer (Bytes)')
# plt.ylabel('Time (Microseconds)')
# plt.title("Time vs Buffer Size for Latency Level Buffer Sizes")
# plt.xlim([0,4300])
# plt.ylim([0,7.5])
# plt.xticks([0,512,1024,1536,2048,2560,3072,3584,4096])
# #legend
# plt.legend([line1,line0], 
# 	['LeafNodes', 'RandomNodes'], loc=2)
# plt.show()

