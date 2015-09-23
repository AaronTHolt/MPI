import numpy as np
import scipy as sp
import scipy.stats
import matplotlib.pyplot as plt

pp_sn = []
pp_sn_stats = []
pp_ln = []
pp_ln_stats = []
pp_rn = []
pp_rn_stats =[]
pp_ln_small = []
pp_ln_small_stats = []
pp_rn_small = []
pp_rn_small_stats = []

small = [1,512,1024,1536,2048,2560,3072,3584,4096]
size = []
for ii in range(0,23):
	size.append(2**ii)

for ii in range(0,23):
	with open("RESULTS/results_pp_sn_{s}".format(s=size[ii])) as f:
		# print "RESULTS/results_pp_sn_{s}".format(s=size[ii])
		lis = [line.split() for line in f]
		mylist = lis[0][0].split(",")
		mylist2 = [float(x)/2 for x in mylist]
		pp_sn.append(mylist2)
	f.close()

	with open("RESULTS/results_pp_ln_{s}".format(s=size[ii])) as f:
		# print "RESULTS/results_pp_ln_{s}".format(s=size[ii])
		lis = [line.split() for line in f]
		mylist = lis[1][0].split(",")
		mylist2 = [float(x)/2 for x in mylist]
		pp_ln.append(mylist2)
	f.close()

	with open("RESULTS/results_pp_rn_{s}".format(s=size[ii])) as f:
		# print "RESULTS/results_pp_rn_{s}".format(s=size[ii])
		lis = [line.split() for line in f]
		mylist = lis[1][0].split(",")
		mylist2 = [float(x)/2 for x in mylist]
		pp_rn.append(mylist2)
	f.close()

for ii in [1,512,1024,1536,2048,2560,3072,3584,4096]:
	with open("RESULTS/results_pp_ln_small_{s}".format(s=ii)) as f:
		# print "RESULTS/results_pp_ln_small_{s}".format(s=ii)
		lis = [line.split() for line in f]
		mylist = lis[1][0].split(",")
		mylist2 = [float(x)/2 for x in mylist]
		pp_ln_small.append(mylist2)
	f.close()

	with open("RESULTS/results_pp_rn_small_{s}".format(s=ii)) as f:
		# print "RESULTS/results_pp_rn_small_{s}".format(s=ii)
		lis = [line.split() for line in f]
		mylist = lis[1][0].split(",")
		mylist2 = [float(x)/2 for x in mylist]
		pp_rn_small.append(mylist2)
		# print mylist2
	f.close()

def mean_confidence_interval(data, confidence=0.95):
    a = 1.0*np.array(data)
    n = len(a)
    m, se = np.mean(a), scipy.stats.sem(a)
    h = se * sp.stats.t._ppf((1+confidence)/2., n-1)
    # return [m, [m-h, m+h]]
    return [m, h]

for ii in range(0,len(pp_sn)):
	pp_sn_stats.append(mean_confidence_interval(pp_sn[ii]))
	pp_ln_stats.append(mean_confidence_interval(pp_ln[ii]))
	pp_rn_stats.append(mean_confidence_interval(pp_rn[ii]))



#plot (x,y,style)
line0, = plt.plot(size, [row[0] for row in pp_sn_stats], 'ro-')
plt.errorbar(size, [row[0] for row in pp_sn_stats], 
	yerr=[row[1] for row in pp_sn_stats], fmt='ro-')

line1, = plt.plot(size, [row[0] for row in pp_ln_stats], 'bs-')
plt.errorbar(size, [row[0] for row in pp_ln_stats], 
	yerr=[row[1] for row in pp_ln_stats], fmt='bs-')

line2, = plt.plot(size, [row[0] for row in pp_rn_stats], 'k^-')
plt.errorbar(size, [row[0] for row in pp_rn_stats], 
	yerr=[row[1] for row in pp_rn_stats], fmt='k^-')

#title and axis labels
plt.xlabel('Size of buffer (Bytes)')
plt.ylabel('Time (s)')
plt.title("Time vs Buffer Size for Exponential Message Size Growth")
#log2 scale
plt.xscale('log', basex=2)
plt.yscale('log', basey=2)
#x axis limits (set just above last value)
plt.xlim([0,2**23+2**22])
#legend
plt.legend([line0,line1,line2], 
	['SingleNode', 'LeafNodes', 'RandomNodes'], loc=2)
plt.show()

################################################################

for ii in range (0,len(small)):
	pp_ln_small_stats.append(mean_confidence_interval(pp_ln_small[ii]))
	pp_rn_small_stats.append(mean_confidence_interval(pp_rn_small[ii]))

line0, = plt.plot(small, [row[0]*(10**6) for row in pp_rn_small_stats], 'ro-')
plt.errorbar(small, [row[0]*(10**6) for row in pp_rn_small_stats], 
	yerr=[row[1]*(10**6) for row in pp_rn_small_stats], fmt='ro-')

line1, = plt.plot(small, [row[0]*(10**6) for row in pp_ln_small_stats], 'bs-')
plt.errorbar(small, [row[0]*(10**6) for row in pp_ln_small_stats], 
	yerr=[row[1]*(10**6) for row in pp_ln_small_stats], fmt='bs-')

#title and axis labels
plt.xlabel('Size of buffer (Bytes)')
plt.ylabel('Time (Microseconds)')
plt.title("Time vs Buffer Size for Latency Level Buffer Sizes")
plt.xlim([0,4300])
plt.ylim([0,7.5])
plt.xticks([0,512,1024,1536,2048,2560,3072,3584,4096])
#legend
plt.legend([line1,line0], 
	['LeafNodes', 'RandomNodes'], loc=2)
plt.show()


print np.polyfit(size,[row[0] for row in pp_sn_stats],1)
print np.polyfit(size,[row[0] for row in pp_ln_stats],1)
print np.polyfit(size,[row[0] for row in pp_rn_stats],1)

# alpha_range = []
# sn_alpha = []
# rn_alpha = []
# ln_alpha = []
# beta_range = []
# sn_beta = []
# rn_beta = []
# ln_beta = []
# for ii in range(0,23):
# 	if ii>=14:
# 		beta_range.append(2**ii)
# 		sn_beta.append(pp_sn_stats[ii][0])
# 		ln_beta.append(pp_ln_stats[ii][0])
# 		rn_beta.append(pp_rn_stats[ii][0])

# b_sn = np.polyfit(beta_range,sn_beta,1)[0]
# b_ln = np.polyfit(beta_range,ln_beta,1)[0]
# b_rn = np.polyfit(beta_range,rn_beta,1)[0]
# print "SINGLE", b_sn, 1/b_sn
# print "LEAF", b_ln, 1/b_ln
# print "RANDOM", b_rn, 1/b_rn
# b_sn = np.polyfit(beta_range,sn_beta,1)
# b_ln = np.polyfit(beta_range,ln_beta,1)
# b_rn = np.polyfit(beta_range,rn_beta,1)
# print "SINGLE", b_sn
# print "LEAF", b_ln
# print "RANDOM", b_rn


# for ii in range(0,5):
# 	sn_alpha.append(pp_sn_stats[ii])
# 	ln_alpha.append(pp_ln_stats[ii])
# 	rn_alpha.append(pp_rn_stats[ii])

# sn_alpha = np.mean(sn_alpha)
# ln_alpha = np.mean(ln_alpha)
# rn_alpha = np.mean(rn_alpha)

# print ""
# print "SINGLE", sn_alpha, sn_alpha*5.6*10**9
# print "LEAF", ln_alpha, ln_alpha*5.6*10**9
# print "RANDOM", rn_alpha, rn_alpha*5.6*10**9