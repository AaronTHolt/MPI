import numpy as np
# import scipy as sp
# import scipy.stats
import matplotlib.pyplot as plt


serial_576 = []
serial_576_stats = []
all_4_576 = []
all_4_576_stats = []
fox_4_576 = []
fox_4_576_stats = []

for ii,name in enumerate(['all_4_576','fox_4_576','serial_576']):
	with open('RESULTS/{ff}'.format(ff=ii)) as f:
		# print "RESULTS/results_pp_sn_{s}".format(s=size[ii])
		lis = [line.split() for line in f]
		print lis
		mylist = lis[0][0].split(",")
		mylist2 = [float(x)/2 for x in mylist]
		if ii==0:
			all_4_576.append(mylist2)
			print mylist2
		if ii==1:
			fox_4_576.append(mylist2)
		if ii==2:
			serial_576.append(mylist2)	
	f.close()



# for ii in [144,576,1296,2304]:
# 	pass
# for ii in [4,16,36,64,144]:
# 

# def mean_confidence_interval(data, confidence=0.95):
#     a = 1.0*np.array(data)
#     n = len(a)
#     m, se = np.mean(a), scipy.stats.sem(a)
#     h = se * sp.stats.t._ppf((1+confidence)/2., n-1)
#     # return [m, [m-h, m+h]]
#     return [m, h]

# for ii in range(0,len(pp_sn)):
# 	pp_sn_stats.append(mean_confidence_interval(pp_sn[ii]))
# 	pp_ln_stats.append(mean_confidence_interval(pp_ln[ii]))
# 	pp_rn_stats.append(mean_confidence_interval(pp_rn[ii]))



# #plot (x,y,style)
# line0, = plt.plot(size, [row[0] for row in pp_sn_stats], 'ro-')
# plt.errorbar(size, [row[0] for row in pp_sn_stats], 
# 	yerr=[row[1] for row in pp_sn_stats], fmt='ro-')

# line1, = plt.plot(size, [row[0] for row in pp_ln_stats], 'bs-')
# plt.errorbar(size, [row[0] for row in pp_ln_stats], 
# 	yerr=[row[1] for row in pp_ln_stats], fmt='bs-')

# line2, = plt.plot(size, [row[0] for row in pp_rn_stats], 'k^-')
# plt.errorbar(size, [row[0] for row in pp_rn_stats], 
# 	yerr=[row[1] for row in pp_rn_stats], fmt='k^-')

# #title and axis labels
# plt.xlabel('Size of buffer (Bytes)')
# plt.ylabel('Time (s)')
# plt.title("Time vs Buffer Size for Exponential Message Size Growth")
# #log2 scale
# plt.xscale('log', basex=2)
# plt.yscale('log', basey=2)
# #x axis limits (set just above last value)
# plt.xlim([0,2**23+2**22])
# #legend
# plt.legend([line0,line1,line2], 
# 	['SingleNode', 'LeafNodes', 'RandomNodes'], loc=2)
# plt.show()

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

