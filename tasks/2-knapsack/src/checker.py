import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent.parent.parent))
from scripts_common.check import check

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


if __name__ == "__main__":
    check(check_file)
