import sys
from pathlib import Path

def get_test_size(name):
    l = list(name.split('_'))
    for (i, x) in enumerate(l):
        if x == "gc":
            return int(l[i + 1])
    return -1

def check_file(input, output):
    if len(output) == 0 or output[0].find("Answer: ") == -1:
        return "No answer"

    promised_answer = int(output[0].split()[1])

    n, m = map(int, input[0].split())
    edges = []
    for x in input[1:]:
        edges.append(tuple(map(int, x.split(' '))))
    
    colors = list(map(int, output[1].split()))
    real_answer = len(set(colors))

    if real_answer != promised_answer:
        return f"Wrong answer: promised {promised_answer}, real {real_answer}"
    
    for (u, v) in edges:
        if colors[u] == colors[v]:
            return f"Wrong coloring: colors of {u} and {v} are the same"
    
    return f"Ok {real_answer}"


dir = Path(sys.argv[1])
data = Path("./data")

with open(dir / "report.txt", "w") as report:
    files = list(x for x in Path(dir).iterdir() if x.is_file() and x.name != "report.txt")
    files.sort(key=lambda x: (get_test_size(x.name), x.name))
    for file in files:
        name = file.name
        with open(data / name, "r") as input, open(file, "r") as output:
            report.write(f"{name}: {check_file(input.readlines(), output.readlines())}\n")
