import sys
from pathlib import Path

bounds = {
    "sc_157_0": (130000, 94402),
    "sc_330_0": (29, 24),
    "sc_1000_11": (240, 147),
    "sc_5000_1": (70, 31),
    "sc_10000_5": (120, 64),
    "sc_10000_2": (280, 167),

    "ks_30_0": (92000, 99798),
    "ks_50_0": (141956, 142156),
    "ks_200_0": (100062, 100236),
    "ks_400_0": (3966813, 3967028),
    "ks_1000_0": (109869, 109899),
    "ks_10000_0": (1099870, 1099881),

    "gc_50_3": (8, 6),
    "gc_70_7": (20, 17),
    "gc_100_5": (21, 16),
    "gc_250_9": (95, 78),
    "gc_500_1": (18, 16),
    "gc_1000_5": (124, 100),

    "tsp_51_1": (482, 430),
    "tsp_100_3": (23433, 20800),
    "tsp_200_2": (35985, 30000),
    "tsp_574_1": (40000, 37600),
    "tsp_1889_1": (378069, 323000),
    "tsp_33810_1": (78478868, 67700000),

    "fl_25_2": (4000000, 3269822),
    "fl_100_1": (26000000, 22724634),
    "fl_200_7": (5000000, 4711295),
    "fl_500_7": (30000000, 27006099),
    "fl_1000_2": (10000000, 8879294),
    "fl_2000_2": (10000000, 7453531),

    "vrp_16_3_1": (387, 280),
    "vrp_26_8_1": (1019, 630),
    "vrp_51_5_1": (713, 540),
    "vrp_101_10_1": (1193, 830),
    "vrp_200_16_1": (3719, 1400),
    "vrp_421_41_1": (2392, 2000),
}

def get_test_size(name):
    l = list(name.split('_'))
    l = list(int(x) if x.isdigit() else x for x in l)
    return l

def get_points(name, score):
    if name not in bounds:
        return None
    (left, right) = bounds[name]
    if left > right:
        left = -left
        right = -right
        score = -score
    if score >= right - 1e-3:
        return 5
    if score >= left - 1e-3:
        return 3
    return 0

def check(check_file):
    dir = Path(sys.argv[1])
    data = Path("./data")

    with open(dir / "verdict.txt", "w") as verdict:
        total_points = 0

        files = list(x for x in Path(dir).iterdir() if x.is_file() and x.name != "verdict.txt")
        files.sort(key=lambda x: get_test_size(x.name))
        for file in files:
            name = file.name
            with open(data / name, "r") as input, open(file, "r") as output:
                val = check_file(input.readlines(), output.readlines())
                if type(val) == str:
                    message = val
                    points = None
                else:
                    message, score = val
                    points = get_points(name, score)
                if points is None:
                    verdict.write(f"{name}: {message}, no points\n")
                else:
                    verdict.write(f"{name}: {message}, {points} points\n")
                    total_points += points

        verdict.write(f"{total_points} points\n")
