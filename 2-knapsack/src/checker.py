import sys
from pathlib import Path

def get_test_size(name):
    l = list(name.split('_'))
    for (i, x) in enumerate(l):
        if x == "ks":
            return int(l[i + 1])
    return -1

def check_file(input, output):
    if len(output) == 0 or output[0].find("Score: ") == -1:
        return "No score"

    promised_score = int(output[0].split()[1])

    item_count, capacity = map(int, input[0].split())
    items = {}
    for x in input[1:]:
        x = x.strip()
        items[x] = items.get(x, 0) + 1

    total_weight = 0
    total_cost = 0

    for x in output[1:]:
        x = x.strip()
        if items.get(x, 0) < 1:
            return f"Incorrect item ({x.strip()})"
        items[x] -= 1

        cost, weight = map(int, x.split())
        total_weight += weight
        total_cost += cost

    if total_weight > capacity:
        return f"You can't carry all this stuff"
    
    if total_cost != promised_score:
        return f"Wrong score: promised {promised_score}, real {total_cost}"
    
    return f"Ok {total_cost}"


dir = Path(sys.argv[1])
data = Path("./data")

with open(dir / "verdict.txt", "w") as verdict:
    files = list(x for x in Path(dir).iterdir() if x.is_file() and x.name != "verdict.txt")
    files.sort(key=lambda x: (get_test_size(x.name), x.name))
    for file in files:
        name = file.name
        with open(data / name, "r") as input, open(file, "r") as output:
            verdict.write(f"{name}: {check_file(input.readlines(), output.readlines())}\n")
