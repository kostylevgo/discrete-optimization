import sys

import numpy as np
import scipy

np.random.seed(31)

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

at_least = np.ones(shape=item_count)
match_matrix = np.array(match_matrix)
weights = np.array(weights)

lin_solution = scipy.optimize.linprog(c=weights, A_ub=-match_matrix, b_ub=-at_least, bounds=(0, 1)).x
approx_answer_size = np.sum(lin_solution)
approx_complexity = set_count * item_count * approx_answer_size

best_score = 1e100
best_answer = []

print(approx_complexity, file=sys.stderr)

for i in range(10):
    cur_solution = np.zeros(set_count)

    conditions = []
    new_at_least = []

    while True:
        coverage = match_matrix @ cur_solution
        if np.min(coverage) > 1 - 1e-6:
            break
        
        A_ub = -np.concatenate((match_matrix, np.array(conditions))) if len(conditions) > 0 else -match_matrix
        b_ub = -np.concatenate((at_least, np.array(new_at_least))) if len(new_at_least) > 0 else -at_least

        noise = (np.random.random(set_count) * 2 - 1) * 1e-1
        solution = scipy.optimize.linprog(c=weights + noise, A_ub=A_ub, b_ub=b_ub, bounds=(0, 1)).x
        diff = solution - cur_solution
        mx = np.argmax(diff)
        cur_solution[mx] = 1

        conditions.append(np.zeros(set_count))
        conditions[-1][mx] = 1
        new_at_least.append(1)

    score = weights @ cur_solution
    if score < best_score:
        best_score = score
        best_answer = cur_solution

print(f"Score: {int(round(best_score))}")
for i in range(set_count):
    if best_answer[i] >= 1 - 1e-6:
        print(*datas[i])
