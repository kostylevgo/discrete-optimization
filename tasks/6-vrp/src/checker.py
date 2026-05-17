import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent.parent.parent))
from scripts_common.check import check

def distance(a, b):
    x = a[0] - b[0]
    y = a[1] - b[1]
    return (x**2 + y**2)**0.5

def check_file(input, output):
    if len(output) == 0 or output[0].find("Score: ") == -1:
        return "No score"

    promised_answer = float(output[0].split()[1])

    customers_count, cars, capacity = map(int, input[0].split())
    customers = list(tuple(map(float, line.split())) for line in input[1:])
    origin = customers[0][1:]

    routes = list(list(map(int, l.split())) for l in output[1:])
    if len(routes) > cars:
        return f"Wrong answer: used {len(routes)} cars, but only {cars} are available"

    all_points = []
    for r in routes:
        all_points += r
    if len(all_points) != customers_count - 1 or set(all_points) != set(range(1, customers_count)):
        return "Wrong answer: incorrect routes"

    real_answer = 0
    for (i, route) in enumerate(routes):
        if any(x < 0 or x >= customers_count for x in route):
            return f"Wrong answer: incorrect route for car {i}"
        used_capacity = 0
        last_point = origin

        for customer in map(lambda i: customers[i], route):
            real_answer += distance(last_point, customer[1:])
            last_point = customer[1:]
            used_capacity += customer[0]
        real_answer += distance(last_point, origin)

        if used_capacity > capacity + 1e-2:
            return f"Wrong answer: car {i} is overflown"

    if abs(real_answer - promised_answer) > 1e-2:
        return f"Wrong answer: promised {promised_answer:.3f}, real {real_answer:.3f}"

    return f"Ok {real_answer:.3f}"


if __name__ == "__main__":
    check(check_file)
