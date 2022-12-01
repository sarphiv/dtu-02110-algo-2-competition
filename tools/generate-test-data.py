import numpy as np

n = 100000
val_limit = 100000



x = np.random.choice(val_limit+1, n)
y = np.random.choice(val_limit+1, n)

p = np.unique(np.vstack((x,y)).T, axis=0)
p = np.random.permutation(p)
n = p.shape[0]

o1 = np.random.randint(0, val_limit+1, n)
o2 = np.random.randint(0, val_limit+1, n)
o3 = np.random.randint(0, val_limit+1, n)
o4 = np.random.randint(0, val_limit+1, n)


print(n)
for i in range(n):
    print(f"{x[i]} {y[i]} {o1[i]} {o2[i]} {o3[i]} {o4[i]}")
    # print(f"{x[i]} {y[i]} {o1[i]} {o2[i]} 0 {o4[i]}")
    # print(f"{x[i]} {y[i]} 0 0 {o3[i]} 0")
