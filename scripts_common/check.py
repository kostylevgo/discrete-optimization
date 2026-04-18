import sys
from pathlib import Path

def distance(a, b):
    x = a[0] - b[0]
    y = a[1] - b[1]
    return (x**2 + y**2)**0.5

def get_test_size(name):
    l = list(name.split('_'))
    l = list(int(x) if x.isdigit() else x for x in l)
    return l

def check(check_file):
    dir = Path(sys.argv[1])
    data = Path("./data")

    with open(dir / "verdict.txt", "w") as verdict:
        files = list(x for x in Path(dir).iterdir() if x.is_file() and x.name != "verdict.txt")
        files.sort(key=lambda x: get_test_size(x.name))
        for file in files:
            name = file.name
            with open(data / name, "r") as input, open(file, "r") as output:
                verdict.write(f"{name}: {check_file(input.readlines(), output.readlines())}\n")
