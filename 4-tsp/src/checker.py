import sys
from pathlib import Path

def get_test_size(name):
    l = list(name.split('_'))
    for (i, x) in enumerate(l):
        if x == "tsp":
            return int(l[i + 1])
    return -1

def distance(a, b):
    x = a[0] - b[0]
    y = a[1] - b[1]
    return (x**2 + y**2)**0.5

def check_file(input, output):
    if len(output) == 0 or output[0].find("Score: ") == -1:
        return "No score"

    promised_answer = float(output[0].split()[1])

    n = int(input[0])
    points = list(tuple(map(float, line.split())) for line in input[1:])
    
    order = list(map(int, output[1].split()))
    if len(order) != n or len(set(order)) != n:
        return "Wrong answer: path is not hamiltonian"
    
    points = list(points[i] for i in order)

    real_answer = distance(points[0], points[-1])
    for i in range(1, n):
        real_answer += distance(points[i], points[i - 1])
    
    if abs(real_answer - promised_answer) > 1e-2:
        return f"Wrong answer: promised {promised_answer:.3f}, real {real_answer:.3f}"
    
    return f"Ok {real_answer:.3f}"


dir = Path(sys.argv[1])
data = Path("./data")

with open(dir / "verdict.txt", "w") as verdict:
    files = list(x for x in Path(dir).iterdir() if x.is_file() and x.name != "verdict.txt")
    files.sort(key=lambda x: (get_test_size(x.name), x.name))
    for file in files:
        name = file.name
        with open(data / name, "r") as input, open(file, "r") as output:
            verdict.write(f"{name}: {check_file(input.readlines(), output.readlines())}\n")
