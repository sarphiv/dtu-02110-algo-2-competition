#%%
import numpy as np


#%%
naive = np.loadtxt("out-slow.txt", dtype=np.int32)
orig = np.loadtxt("out-fast2.txt", dtype=np.int32)

#%%
naive_set = {(val[0], val[1], val[2]) for val in naive}
orig_set = {(val[0], val[1], val[2]) for val in orig}


diff = naive_set - orig_set

#%%
next(iter(diff))





#%%
comp = np.loadtxt("Competition202.in", dtype=np.int32)