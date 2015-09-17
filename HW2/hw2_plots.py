import matplotlib.pyplot as plt

size = 		[1, 2**3, 2**6, 2**9, 2**12, 2**15, 2**18, 2**20]  #in doubles
for ii in range(0,len(size)):
	size[ii] = size[ii] * 8 #convert to bytes

data32_official = 	[0.000107, 0.000097, 0.000113, 0.000131, 0.000301, 0.000680, 0.003777, 0.011628]
data128_official = 	[0.000272, 0.000154, 0.000259, 0.000209, 0.001068, 0.001619, 0.006188, 0.023543]
data32_h2l = [0.000131, 0.000134, 0.000157, 0.000217, 0.000580, 0.003378, 0.034967, 0.120140]
data128_h2l = [0.000198 ,0.000206, 0.000314, 0.000323, 0.000824, 0.005373, 0.049437, 0.185455]
data32_l2h = [0.000130, 0.000133, 0.000159, 0.000222, 0.000573, 0.005297, 0.034659, 0.122329]
data128_l2h = [0.000199, 0.000202, 0.000241, 0.000333, 0.000835, 0.004774, 0.046847, 0.174552]

#plot (x,y,style)
line0, = plt.plot(size, data32_official, 'ro-')
line1, = plt.plot(size, data128_official, 'rs--')
line2, = plt.plot(size, data32_l2h, 'bo-')
line3, = plt.plot(size, data128_l2h, 'bs--')
line4, = plt.plot(size, data32_h2l, 'ko-')
line5, = plt.plot(size, data128_h2l, 'ks--')
#title and axis labels
plt.xlabel('Size of buffer (Bytes)')
plt.ylabel('Time (s)')
plt.title("Time vs Buffer size for np=32,128")
#log2 scale
plt.xscale('log', basex=2)
plt.yscale('log', basey=2)
#x axis limits (set just above last value)
plt.xlim([0,2**23+2**22])
#legend
plt.legend([line0,line1,line2,line3,line4,line5], 
	['32_official', '128_official', '32_l2h', '128_l2h', '32_h2l', '128_h2l'], loc=2)
plt.show()


nodes = [2, 4, 8, 16, 32]
simplereduce = [0.002748, 0.007317, 0.013728, 0.030057, 0.061194]
bflyreduce_h2l = [0.001785, 0.001010, 0.001229, 0.002714, 0.002542]
bflyreduce_l2h = [0.001770, 0.002024, 0.002280, 0.002396, 0.002604]
bfly_official = [0.001807, 0.001023, 0.001211, 0.002914, 0.003238]

line0, = plt.plot(nodes, simplereduce, 'ko-')
line1, = plt.plot(nodes, bflyreduce_h2l, 'bo-')
line2, = plt.plot(nodes, bflyreduce_l2h, 'go-')
line3, = plt.plot(nodes, bfly_official, 'ro-')
plt.xlabel('Processes (Nodes used)')
plt.ylabel('Time (s)')
plt.xscale('log', basex=2)
plt.yscale('log', basey=2)
plt.xlim([0, 34])
plt.ylim([2**-10,2**-4+2**-5])
plt.title('Time vs Nodes for Various Allreduce Algorithms')
plt.legend([line0,line1,line2,line3], 
	['Simple Allreduce', 'Bfly_h2l', 'Bfly_l2h', 'Official Allreduce'], loc=2)
plt.show()

line0, = plt.plot(nodes, simplereduce, 'ko-')
line1, = plt.plot(nodes, bflyreduce_h2l, 'bo-')
line2, = plt.plot(nodes, bflyreduce_l2h, 'go-')
line3, = plt.plot(nodes, bfly_official, 'ro-')
plt.xlabel('Processes (Nodes used)')
plt.ylabel('Time (s)')
plt.yscale('log', basey=2)
plt.xlim([0, 34])
plt.ylim([2**-10,2**-4+2**-5])
plt.title('Time vs Nodes for Various Allreduce Algorithms')
plt.legend([line0,line1,line2,line3], 
	['Simple Allreduce', 'Bfly_h2l', 'Bfly_l2h', 'Official Allreduce'], loc=2)
plt.show()

#import numpy as np
# print np.polyfit(nodes,bfly_official,2)
# print np.polyfit(nodes,bflyreduce_l2h,2)
# print np.polyfit(nodes,bflyreduce_h2l,2)
# print np.polyfit(nodes,simplereduce,2)