import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent.parent.parent))
from scripts_common.check import check

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


if __name__ == "__main__":
    check(check_file)
