import sys

import numpy as np
from scipy.optimize import linprog

def distance(a, b):
    x = a[0] - b[0]
    y = a[1] - b[1]
    return (x**2 + y**2)**0.5

shops_cnt, customers_cnt = map(int, input().split())

shops = list(tuple(map(float, input().split())) for i in range(shops_cnt))
customers = list(tuple(map(float, input().split())) for i in range(customers_cnt))

variables = shops_cnt * customers_cnt + shops_cnt

lower_bounds = [0] * variables
upper_bounds = [1] * variables

costs = [0] * variables

for i in range(customers_cnt):
    for j in range(shops_cnt):
        costs[i * shops_cnt + j] = distance(shops[j][2:], customers[i][1:])

for j in range(shops_cnt):
    costs[customers_cnt * shops_cnt + j] = shops[j][0]

eq_constraints = []
less_constraints = []

for i in range(customers_cnt):
    arr = np.zeros(variables)
    for j in range(shops_cnt):
        arr[i * shops_cnt + j] = 1
    eq_constraints.append((arr, 1))
    # all customers must be satisfied

for j in range(shops_cnt):
    arr = np.zeros(variables)
    for i in range(customers_cnt):
        arr[i * shops_cnt + j] = customers[i][0] # demand
    arr[customers_cnt * shops_cnt + j] = -shops[j][1] # capacity
    less_constraints.append((arr, 0))
    # shops shouldn't be overflown

A_eq = np.array(list(x[0] for x in eq_constraints))
A_less = np.array(list(x[0] for x in less_constraints))
b_eq = np.array(list(x[1] for x in eq_constraints))
b_less = np.array(list(x[1] for x in less_constraints))

print("solving...", file=sys.stderr)

solution = linprog(costs, A_less, b_less, A_eq, b_eq, list(zip(lower_bounds, upper_bounds))).x

cost = costs @ solution

shops_openings = solution[customers_cnt * shops_cnt:]
open_order = np.argsort(-shops_openings)

answer = [-1] * customers_cnt
remaining_capacity = list(x[1] for x in shops)

score = 0

confidence = 1
while -1 in answer:
    for shop in open_order:
        if shops_openings[shop] < confidence:
            break
        my_customers = list(solution[i * shops_cnt + shop] for i in range(customers_cnt))
        order = np.argsort(-np.array(my_customers))
        for customer in order:
            if my_customers[customer] < confidence:
                break
            if answer[customer] == -1 and customers[customer][0] <= remaining_capacity[shop]:
                remaining_capacity[shop] -= customers[customer][0]
                answer[customer] = shop
                score += distance(shops[shop][2:], customers[customer][1:])
    confidence *= 0.9
    if confidence < 1e-6:
        confidence = 0

for i in range(shops_cnt):
    if remaining_capacity[i] != shops[i][1]:
        score += shops[i][0]

print(f"Score: {score}")
print(*answer)
