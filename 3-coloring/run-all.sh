#!/bin/bash

set -euo pipefail

data="data"
results="results"
exe_debug="./build-debug/solve"
exe_release="./build-release/solve"

compile_debug="cmake --build build-debug --target solve -j20"
compile_release="cmake --build build-release --target solve -j20"

echo compiling...

$compile_release

mkdir "$results" -p

for file in ./"$data"/*; do
    name=$(basename "$file")
    echo "running $name"
    if [ -f "$file" ]; then
        /usr/bin/time -f "time: %E" "$exe_release" <"$file" >"./$results/$name"
    fi
done

python3 "./src/checker.py" "$results"
cp "./$results/verdict.txt" .
