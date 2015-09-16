import matplotlib.pyplot as plt
import numpy as np

size = 		[3, 6, 9, 12, 15, 18, 20]
data32_official = 	[0.003238, 0.001770, 0.003322, 0.002570, 0.001672, 0.002420, 0.004662]
data128_official = 	[0.006406, 0.007149, 0.007143, 0.004036, 0.003866, 0.003570, 0.004131]

data32_h2l = [0.007877, 0.007683, 0.007903, 0.007763, 0.007849, 0.015023, 0.059971]
data128_h2l = [0.010051, 0.009990, 0.010003, 0.010850, 0.009976, 0.022153, 0.064364 ]

data32_l2h = [0.007697, 0.007751, 0.007847, 0.007699, 0.007804, 0.017044, 0.061102]
data128_l2h = [0.011053, 0.010694, 0.010902, 0.010078, 0.010861, 0.020101, 0.071812]


line0, = plt.plot(size, data32_official, 'ro-')
line1, = plt.plot(size, data128_official, 'rs--')
line2, = plt.plot(size, data32_l2h, 'bo-')
line3, = plt.plot(size, data128_l2h, 'bs--')
line4, = plt.plot(size, data32_h2l, 'ko-')
line5, = plt.plot(size, data128_h2l, 'ks--')
plt.xlabel('Log2(size of buffer (Bytes))')
plt.ylabel('Time (s)')
plt.title("Time vs Buffer size for np=32,128")
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
plt.title('Time vs Nodes for Various Allreduce Algorithms')
plt.legend([line0,line1,line2,line3], 
	['Simple Allreduce', 'Bfly_h2l', 'Bfly_l2h', 'Official Allreduce'], loc=2)
plt.show()
