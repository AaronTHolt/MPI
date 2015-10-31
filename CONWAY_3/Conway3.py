import os.path
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

# os.path.isfile(fname) 


serial = {}
serial_stats = {}
blocked = {}
blocked_stats = {}
checkered = {}
checkered_stats = {}

for ss in [300, 600, 900]:
	if os.path.isfile("RESULTS/serial_{ss}.txt".format(ss=ss)):
		with open("RESULTS/serial_{ss}.txt".format(ss=ss)) as f:
			lis = [line.split() for line in f]
			# comm = lis[0][0].split(",")
			# comm = [float(x) for x in comm]
			# comp = lis[1][0].split(",")
			# comp = [float(x) for x in comp]
			total = lis[2][0].split(",")
			total = [float(x) for x in total]
			serial[ss] = mean_confidence_interval(total)
		f.close()

for ss in [300, 600, 900]:
	blocked[ss] = {}
	for nn in [1,4,9,16,25,36]:
		if os.path.isfile("RESULTS/blocked_{ss}_{nn}.txt".format(ss=ss, nn=nn)):			
			with open("RESULTS/blocked_{ss}_{nn}.txt".format(ss=ss, nn=nn)) as f:
				lis = [line.split() for line in f]
				# comm = lis[0][0].split(",")
				# comm = [float(x) for x in comm]
				# comp = lis[1][0].split(",")
				# comp = [float(x) for x in comp]
				total = lis[2][0].split(",")
				total = [float(x) for x in total]
				blocked[ss][nn] = mean_confidence_interval(total)

			f.close()

for ss in [300, 600, 900]:
	checkered[ss] = {}
	for nn in [1,4,9,16,25,36]:
		if os.path.isfile("RESULTS/checkered_{ss}_{nn}.txt".format(ss=ss, nn=nn)):			
			with open("RESULTS/checkered_{ss}_{nn}.txt".format(ss=ss, nn=nn)) as f:
				lis = [line.split() for line in f]
				# comm = lis[0][0].split(",")
				# comm = [float(x) for x in comm]
				# comp = lis[1][0].split(",")
				# comp = [float(x) for x in comp]
				total = lis[2][0].split(",")
				total = [float(x) for x in total]
				checkered[ss][nn] = mean_confidence_interval(total)
			f.close()

# for ii in [4,9,16,25,36]:
# 	print checkered[300][ii]
# print [checkered[300][x][0] for x in sorted([row for row in checkered[300]])]

# print [blocked[900][x][0] for x in sorted([row for row in blocked[900]])]
# print [checkered[900][x][0] for x in sorted([row for row in checkered[900]])]
# print serial[900][0]

##################### Speedup vs NP ##########################
aa = serial[900][0]
a = [serial[900][0]]+[checkered[900][x][0] for x in sorted([row for row in checkered[900]])]
b = [0]+[checkered[900][x][1] for x in sorted([row for row in checkered[900]])]
c = [1]+sorted([row for row in checkered[900]])
a = [aa/x for x in a]


line0, = plt.plot(c, a, 'ro-')
plt.errorbar(c, a, 
	yerr=b, fmt='ro-')

aa = serial[600][0]
a = [serial[600][0]]+[checkered[600][x][0] for x in sorted([row for row in checkered[600]])]
b = [0]+[checkered[600][x][1] for x in sorted([row for row in checkered[600]])]
c = [1]+sorted([row for row in checkered[600]])
a = [aa/x for x in a]


line1, = plt.plot(c, a, 'bo-')
plt.errorbar(c, a, 
	yerr=b, fmt='bo-')

aa = serial[300][0]
a = [serial[300][0]]+[checkered[300][x][0] for x in sorted([row for row in checkered[300]])]
b = [0]+[checkered[300][x][1] for x in sorted([row for row in checkered[300]])]
c = [1]+sorted([row for row in checkered[300]])
a = [aa/x for x in a]

line2, = plt.plot(c, a, 'ko-')
plt.errorbar(c, a, 
	yerr=b, fmt='ko-')


aa = serial[900][0]
a = [serial[900][0]]+[blocked[900][x][0] for x in sorted([row for row in blocked[900]])]
b = [0]+[blocked[900][x][1] for x in sorted([row for row in blocked[900]])]
c = [1]+sorted([row for row in blocked[900]])
a = [aa/x for x in a]


line3, = plt.plot(c, a, 'r^--')
plt.errorbar(c, a, 
	yerr=b, fmt='r^--')

aa = serial[600][0]
a = [serial[600][0]]+[blocked[600][x][0] for x in sorted([row for row in blocked[600]])]
b = [0]+[blocked[600][x][1] for x in sorted([row for row in blocked[600]])]
c = [1]+sorted([row for row in blocked[600]])
a = [aa/x for x in a]


line4, = plt.plot(c, a, 'b^--')
plt.errorbar(c, a, 
	yerr=b, fmt='b^--')

aa = serial[300][0]
a = [serial[300][0]]+[blocked[300][x][0] for x in sorted([row for row in blocked[300]])]
b = [0]+[blocked[300][x][1] for x in sorted([row for row in blocked[300]])]
c = [1]+sorted([row for row in blocked[300]])
a = [aa/x for x in a]

line5, = plt.plot(c, a, 'k^--')
plt.errorbar(c, a, 
	yerr=b, fmt='k^--')


#title and axis labels
plt.xlabel('Number of Processors')
plt.ylabel('Speedup Ratio')
plt.title("Speedup vs Processor Count for Checkered with Different World Sizes")
#legend
plt.legend([line0, line1, line2, line3, line4, line5], 
	['Checker 900x900', 'Checker 600x600', 'Checker 300x300',
	'Block 900x900', 'Block 600x600', 'Block 300x300'], loc=2)
plt.show()

# ##################### Speedup vs NP ##########################
# aa = serial[900][0]
# a = [serial[900][0]]+[checkered[900][x][0] for x in sorted([row for row in checkered[900]])]
# b = [0]+[checkered[900][x][1] for x in sorted([row for row in checkered[900]])]
# c = [1]+sorted([row for row in checkered[900]])
# a = [aa/x for x in a]


# line0, = plt.plot(c, a, 'ro-')
# plt.errorbar(c, a, 
# 	yerr=b, fmt='ro-')

# a = [serial[900][0]]+[blocked[900][x][0] for x in sorted([row for row in blocked[900]])]
# b = [0]+[blocked[900][x][1] for x in sorted([row for row in blocked[900]])]
# c = [1]+sorted([row for row in blocked[900]])
# a = [aa/x for x in a]


# line1, = plt.plot(c, a, 'bo-')
# plt.errorbar(c, a, 
# 	yerr=b, fmt='bo-')

# a = [1,4,9,16,25,36]
# c = [1,4,9,16,25,36]

# line2, = plt.plot(c, a, 'ko-')


# #title and axis labels
# plt.xlabel('Number of Processors')
# plt.ylabel('Speedup Ratio')
# plt.title("Speedup vs Processor Count for Conway's Game of Life")
# #legend
# plt.legend([line0, line1, line2], 
# 	['Checkered', 'Blocked', 'Ideal'], loc=2)
# plt.show()


# ########################### Efficiency vs NP ##################################

# aa = serial[900][0]
# a = [serial[900][0]]+[checkered[900][x][0] for x in sorted([row for row in checkered[900]])]
# b = [0]+[checkered[900][x][1] for x in sorted([row for row in checkered[900]])]
# c = [1]+sorted([row for row in checkered[900]])
# a = [aa/x for x in a]
# # b = [aa/x for x in b]
# for ii in range(0,len(a)):
# 	a[ii] = a[ii]/c[ii]

# line0, = plt.plot(c, a, 'ro-')
# plt.errorbar(c, a, 
# 	yerr=b, fmt='ro-')

# a = [serial[900][0]]+[blocked[900][x][0] for x in sorted([row for row in blocked[900]])]
# b = [0]+[blocked[900][x][1] for x in sorted([row for row in blocked[900]])]
# c = [1]+sorted([row for row in blocked[900]])
# a = [aa/x for x in a]
# # b = [aa/x for x in b]
# for ii in range(0,len(a)):
# 	a[ii] = a[ii]/c[ii]

# line1, = plt.plot(c, a, 'bo-')
# plt.errorbar(c, a, 
# 	yerr=b, fmt='bo-')

# a = [1,1,1,1,1,1]
# c = [1,4,9,16,25,36]


# line2, = plt.plot(c, a, 'ko-')

# #title and axis labels
# plt.xlabel('Number of Processors')
# plt.ylabel('Efficiency (%)')
# plt.title("Efficiency vs Processor Count for Conway's Game of Life")
# #y axis limits 
# plt.ylim([0.83,1.01])
# #legend
# plt.legend([line0, line1, line2], 
# 	['Checkered', 'Blocked', 'Ideal'], loc=3)
# plt.show()


# ########################## Time vs Efficiency ######################
# aa = serial[900][0]
# a = [serial[900][0]]+[checkered[900][x][0] for x in sorted([row for row in checkered[900]])]
# b = [0]+[checkered[900][x][1] for x in sorted([row for row in checkered[900]])]
# c = [1]+sorted([row for row in checkered[900]])
# a = [aa/x for x in a]
# time = [serial[900][0]]+[checkered[900][x][0] for x in sorted([row for row in checkered[900]])]
# for ii in range(0,len(a)):
# 	a[ii] = a[ii]/c[ii]

# line0, = plt.plot(a, time, 'ro-')
# plt.errorbar(a, time, 
# 	yerr=b, fmt='ro-')

# a = [serial[900][0]]+[blocked[900][x][0] for x in sorted([row for row in blocked[900]])]
# b = [0]+[blocked[900][x][1] for x in sorted([row for row in blocked[900]])]
# c = [1]+sorted([row for row in blocked[900]])
# a = [aa/x for x in a]
# time = [serial[900][0]]+[blocked[900][x][0] for x in sorted([row for row in blocked[900]])]
# for ii in range(0,len(a)):
# 	a[ii] = a[ii]/c[ii]

# line1, = plt.plot(a, time, 'bo-')
# plt.errorbar(a, time, 
# 	yerr=b, fmt='bo-')

# #title and axis labels
# plt.xlabel('Efficiency (%)')
# plt.ylabel('Time (s)')
# plt.title("Time vs Efficiency for Conway's Game of Life")
# plt.yscale('log', basey=2)
# #legend
# plt.legend([line0, line1], 
# 	['Checkered', 'Blocked'], loc=2)
# plt.show()







########################## Estimates #############################
instr_s = 5.6*10**9  #instructions/second
s_instr = instr_s**-1  #seconds/instruction
WS = [2*300**2,2*600**2,2*900**2] #world size in bytes
serial_c_time = s_instr*WS[2]*(12+14+8*(2+8)) #serial computation time 

Ts = 2.4*10**-6
Tc = 3.6*10**-10

comm_check = 0
comm_block = 0
time_check = []
time_block = []
for ii in [1,4,9,16,25,36]:
	if ii == 1:
		comm_check = 0
		comm_block = 0
	else:
		comm_check = 8*(Ts+Tc*(900*2)/(ii**(1/2)))
		comm_block = 4*(Ts+Tc*900*2)
	time_block.append(serial_c_time/ii+comm_block)
	time_check.append(serial_c_time/ii+comm_check)

# print time_block
# print time_check
