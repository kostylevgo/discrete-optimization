import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent.parent.parent))
from scripts_common.check import check, distance

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


if __name__ == "__main__":
    check(check_file)
