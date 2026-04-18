import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent.parent.parent))
from scripts_common.check import check, distance

def check_file(input, output):
    if len(output) == 0 or output[0].find("Score: ") == -1:
        return "No score"

    promised_answer = float(output[0].split()[1])

    shops_count, customers_count = map(int, input[0].split())
    shops = list(tuple(map(float, line.split())) for line in input[1:1 + shops_count])
    customers = list(tuple(map(float, line.split())) for line in input[1 + shops_count:])

    which_shop = list(map(int, output[1].split()))
    if len(which_shop) != customers_count or any(x < 0 or x >= shops_count for x in which_shop):
        return "Wrong answer: incorrect format"

    real_answer = 0
    is_opened = [False] * shops_count
    used_capacity = [0] * shops_count
    for (customer, shop) in zip(customers, which_shop):
        is_opened[shop] = True
        used_capacity[shop] += customer[0]
        real_answer += distance(customer[1:], shops[shop][2:])
    
    for i in range(shops_count):
        if used_capacity[i] > shops[i][1] + 1e-2:
            return f"Wrong answer: shop {i} is overflown"
        if is_opened[i]:
            real_answer += shops[i][0]

    if abs(real_answer - promised_answer) > 1e-2:
        return f"Wrong answer: promised {promised_answer:.3f}, real {real_answer:.3f}"

    return f"Ok {real_answer:.3f}"


if __name__ == "__main__":
    check(check_file)
