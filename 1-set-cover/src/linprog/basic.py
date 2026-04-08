import numpy as np
import scipy

item_count, set_count = map(int, input().split())
weights = [0] * set_count
match_matrix = list([0] * set_count for _ in range(item_count))
datas = []

for i in range(set_count):
    data = list(map(int, input().split()))
    datas.append(data)
    weights[i] = data[0]
    for j in range(len(data) - 1):
        match_matrix[data[j + 1]][i] = 1

match_matrix = np.array(match_matrix)
weights = np.array(weights)

lin_solution = scipy.optimize.linprog(c=weights, A_ub=-match_matrix, b_ub=-np.ones(item_count), bounds=(0, 1)).x

la = 1
step = 1.01

np.random.seed(31)

while True:
    chance = la * lin_solution
    rnd = np.random.random(set_count)
    cur_solution = rnd < chance

    coverage = match_matrix @ cur_solution
    if np.min(coverage) > 1 - 1e-6:
        break

    la *= step

score = weights @ cur_solution

print(f"Score: {int(round(score))}")
for i in range(set_count):
    if cur_solution[i] >= 1 - 1e-6:
        print(*datas[i])
