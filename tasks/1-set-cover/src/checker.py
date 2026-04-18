import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent.parent.parent))
from scripts_common.check import check

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

if __name__ == "__main__":
    check(check_file)
