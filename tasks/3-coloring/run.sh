#!/bin/bash

set -euo pipefail

data="data"
results="results"

# profile="debug"
profile="release"

exe="./build-$profile/solve"

compile="cmake --build build-$profile --target solve -j20"

echo compiling...

$compile

mkdir "$results" -p

for file in ./"$data"/*; do
    name=$(basename "$file")
    echo "running $name"
    if [ -f "$file" ]; then
        /usr/bin/time -f "time: %E" "$exe" <"$file" >"./$results/$name"
    fi
done

python3 "./src/checker.py" "$results"
cp "./$results/verdict.txt" .
