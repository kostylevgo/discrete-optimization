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

solution = scipy.optimize.linprog(c=weights, A_ub=-match_matrix, b_ub=-at_least, bounds=(0, 1)).x

print(solution)

print(24, solution @ weights, file=sys.stderr)

la = 24 / solution @ weights
best_answer = np.ones(shape=set_count)
best_score = weights @ best_answer
while True:
    chance = solution * la
    rnd = np.random.random(size=set_count)
    taken = rnd < chance
    score = weights @ taken
    if score > 24 + 1e-6:
        continue
    coverage = match_matrix @ taken
    if not np.all(coverage >= 1 - 1e-6):
        continue
    best_score = score
    best_answer = taken
    break

print(f"Score: {best_score}")
for i in range(set_count):
    if best_answer[i] >= 1 - 1e-6:
        print(*datas[i])
