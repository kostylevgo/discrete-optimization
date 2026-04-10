import sys
from pathlib import Path

def get_test_size(name):
    l = list(name.split('_'))
    for (i, x) in enumerate(l):
        if x == "sc":
            return int(l[i + 1])
    return -1

def check_file(input, output):
    if len(output) == 0 or output[0].find("Score: ") == -1:
        return "No score"

    promised_score = int(output[0].split()[1])
    real_score = 0

    item_count, set_count = map(int, input[0].split())
    sets = set(x.strip() for x in input[1:])

    is_covered = [0] * item_count

    for x in output[1:]:
        if x.strip() not in sets:
            return f"Incorrect set ({x.strip()})"
        values = list(map(int, x.split()))
        real_score += values[0]
        for y in values[1:]:
            is_covered[y] = 1

    for i in range(item_count):
        if is_covered[i] == 0:
            return f"Not a cover: element {i} is not covered"
    
    if real_score != promised_score:
        return f"Wrong score: promised {promised_score}, real {real_score}"
    
    return f"Ok {real_score}"


dir = Path(sys.argv[1])
data = Path("./data")

with open(dir / "verdict.txt", "w") as verdict:
    files = list(x for x in Path(dir).iterdir() if x.is_file() and x.name != "verdict.txt")
    files.sort(key=lambda x: (get_test_size(x.name), x.name))
    for file in files:
        name = file.name
        with open(data / name, "r") as input, open(file, "r") as output:
            verdict.write(f"{name}: {check_file(input.readlines(), output.readlines())}\n")

        