import sys

import numpy as np
import scipy

def solve(match_matrix, weights, seed=31, noise_ratio=3e-1):
    (item_count, set_count) = match_matrix.shape

    cur_solution = np.zeros(set_count)
    conditions = []

    np.random.seed(seed)

    while True:
        coverage = match_matrix @ cur_solution
        if np.min(coverage) > 1 - 1e-6:
            break
            
        # uncovered = coverage < 1 - 1e-6
        # will_cover = uncovered @ match_matrix
        # greedy_cost = will_cover / weights
        # greedy_order = np.argsort(-greedy_cost)
        # greedy_penalty = 1 / (greedy_order + 10)

        noise = np.random.random(set_count) * noise_ratio
        
        A_ub = -np.concatenate((match_matrix, np.array(conditions))) if len(conditions) > 0 else -match_matrix
        b_ub = -np.ones(item_count + len(conditions))

        solution = scipy.optimize.linprog(c=weights + noise, A_ub=A_ub, b_ub=b_ub, bounds=(0, 1)).x

        diff = solution - cur_solution
        best = np.argmax(diff)
        # mx = np.max(diff)
        # good = diff > mx - 1e-6
        # best = np.argmax(good * greedy_penalty)

        cur_solution[best] = 1
        conditions.append(np.zeros(set_count))
        conditions[-1][best] = 1

    score = weights @ cur_solution
    return cur_solution, score

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

best_answer, best_score = solve(match_matrix, weights)

lin_solution = scipy.optimize.linprog(c=weights, A_ub=-match_matrix, b_ub=-np.ones(item_count), bounds=(0, 1)).x

set_sizes = np.sum(match_matrix, axis=0)
greedy_cost = set_sizes / weights
approx_answer = weights @ lin_solution
low_cost = item_count / (approx_answer * 2.5)
print(approx_answer, low_cost, file=sys.stderr)
trash = greedy_cost < low_cost
not_trash = trash == 0

good_match_matrix = match_matrix[:, not_trash]
good_weights = weights[not_trash]
good_datas = list(x for (i, x) in enumerate(datas) if not_trash[i])
old_datas = datas

if np.min(good_match_matrix @ np.ones(np.sum(not_trash))) > 1e-6:
    match_matrix = good_match_matrix
    weights = good_weights
    datas = good_datas
    set_count = match_matrix.shape[1]

print(set_count, file=sys.stderr)

approx_answer_size = np.sum(best_answer)
approx_complexity = set_count * item_count * approx_answer_size
attempts = min(300, int(5e9 / approx_complexity))
print(f"Attempts: {attempts}", file=sys.stderr)

was_updated = False
for i in range(attempts):
    answer, score = solve(match_matrix, weights, seed=i)
    if score < best_score:
        best_score = score
        best_answer = answer
        was_updated = True

print(was_updated, file=sys.stderr)
print(f"Score: {int(round(best_score))}")
if not was_updated:
    datas = old_datas
for i in range(set_count):
    if best_answer[i] >= 1 - 1e-6:
        print(*datas[i])
