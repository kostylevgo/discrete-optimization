#!/bin/bash

set -euo pipefail

data="data"
results="results-beam"
exe="./build/main"
mkdir $(dirname exe) -p

compile_debug="g++ -std=c++20 -O0 -D_GLIBCXX_DEBUG -fsanitize=address,undefined -g -o $exe"
compile_release="g++ -std=c++20 -O3 -o $exe"

echo compiling...

$compile_release "./src/solve.cpp"

mkdir "$results" -p

for file in ./"$data"/*; do
    name=$(basename "$file")
    echo "running $name"
    if [ -f "$file" ]; then
        /usr/bin/time -f "time: %E" "$exe" <"$file" >"./$results/$name"
    fi
done

python3 "./src/checker.py" "$results"