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

cur_solution = np.zeros(set_count)

conditions = []

while True:
    coverage = match_matrix @ cur_solution
    if np.min(coverage) > 1 - 1e-6:
        break
    
    A_ub = -np.concatenate((match_matrix, np.array(conditions))) if len(conditions) > 0 else -match_matrix
    b_ub = -np.ones(item_count + len(conditions))

    solution = scipy.optimize.linprog(c=weights, A_ub=A_ub, b_ub=b_ub, bounds=(0, 1)).x
    diff = solution - cur_solution
    mx = np.argmax(diff)
    cur_solution[mx] = 1

    conditions.append(np.zeros(set_count))
    conditions[-1][mx] = 1

score = weights @ cur_solution

print(f"Score: {int(round(score))}")
for i in range(set_count):
    if cur_solution[i] >= 1 - 1e-6:
        print(*datas[i])
