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

all_np = []
all_np_stats = []
fox_np = []
fox_np_stats = []

p = [4,16,36,64,144]
for ii in [4,16,36,64,144]:
	with open('RESULTS/all_{np}_144'.format(np=ii)) as f:
		firstline = f.readline()
		lis = firstline.split()
		mylist = lis[0].split(",")
		mylist2 = [float(x)/2 for x in mylist]
		all_np.append(mylist2)
		all_np_stats.append(mean_confidence_interval(mylist2))
	with open('RESULTS/fox_{np}_144'.format(np=ii)) as f:
		firstline = f.readline()
		lis = firstline.split()
		mylist = lis[0].split(",")
		mylist2 = [float(x)/2 for x in mylist]
		fox_np.append(mylist2)
		fox_np_stats.append(mean_confidence_interval(mylist2))
		# print fox_np_stats

all_size = []
all_size_stats = []
fox_size = []
fox_size_stats = []

# size = [144,576,1296,2304]
# size = [144, 576]
# for ii in [144,576]:
# 	with open('RESULTS/all_144_{ss}'.format(ss=ii)) as f:
# 		firstline = f.readline()
# 		lis = firstline.split()
# 		mylist = lis[0].split(",")
# 		mylist2 = [float(x)/2 for x in mylist]
# 		all_size.append(mylist2)
# 		all_size_stats.append(mean_confidence_interval(mylist2))
# 	with open('RESULTS/fox_144_{ss}'.format(ss=ii)) as f:
# 		firstline = f.readline()
# 		lis = firstline.split()
# 		mylist = lis[0].split(",")
# 		mylist2 = [float(x)/2 for x in mylist]
# 		fox_size.append(mylist2)
# 		fox_size_stats.append(mean_confidence_interval(mylist2))


# line0, = plt.plot(size, [row[0] for row in all_size_stats], 'ro-')
# plt.errorbar(size, [row[0] for row in all_size_stats], 
# 	yerr=[row[1] for row in all_size_stats], fmt='ro-')

# line1, = plt.plot(size, [row[0] for row in fox_size_stats], 'bs-')
# plt.errorbar(size, [row[0] for row in fox_size_stats], 
# 	yerr=[row[1] for row in fox_size_stats], fmt='bs-')

# #title and axis labels
# plt.xlabel('Size of Matrix (doubles)')
# plt.ylabel('Time (s)')
# plt.title("Size of Matrix vs Time for NP=144")
# plt.xscale('log', basex=2)
# plt.yscale('log', basey=2)
# # plt.ylim([2**-13,2**-5])
# # plt.ylim([0,7.5])
# # plt.xticks([0,512,1024,1536,2048,2560,3072,3584,4096])
# #legend
# plt.legend([line0,line1], 
# 	['Allgather', 'Fox'], loc=2)
# plt.show()



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
plt.title("Time vs Number of Nodes for a 576X576 Matrix of Doubles")
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

################################################################


line0, = plt.plot(p, [row[0] for row in all_np_stats], 'ro-')
plt.errorbar(p, [row[0] for row in all_np_stats], 
	yerr=[row[1] for row in all_np_stats], fmt='ro-')

line1, = plt.plot(p, [row[0] for row in fox_np_stats], 'bs-')
plt.errorbar(p, [row[0] for row in fox_np_stats], 
	yerr=[row[1] for row in fox_np_stats], fmt='bs-')

#title and axis labels
plt.xlabel('Processors')
plt.ylabel('Time (s)')
plt.title('Number of Processors vs Time for 144x144')
plt.xscale('log', basex=2)
plt.yscale('log', basey=2)
plt.ylim([2**-13,2**-5])
#legend
plt.legend([line0,line1], 
	['Allgather', 'Fox'], loc=3)
plt.show()

