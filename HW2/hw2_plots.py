import matplotlib.pyplot as pyplot

size = 		[2**3, 2**6, 2**9, 2**12, 2**15, 2**18, 2**20]
data32_official = 	[0.003238, 0.001770, 0.003322, 0.002570, 0.001672, 0.002420, 0.004662]
data128_official = 	[0.006406, 0.007149, 0.007143, 0.004036, 0.003866, 0.003570, 0.004131]

data32_h2l = [0.003381, 0.001583, 0.003309, 0.004399]
data128_h2l = []

data32_l2h = []
data128_h2l = []

nodes = [2, 4, 8, 16, 32]

simplereduce = [0.002748, 0.007317, 0.013728, 0.030057, 0.061194]
bflyreduce_h2l = [0.001785, 0.001010, 0.001229, 0.002714, 0.002542]
bflyreduce_l2h = [0.001770, 0.002024, 0.002280, 0.002396, 0.002604]
bfly_official = [0.001807, 0.001023, 0.001211, 0.002914, 0.003238]