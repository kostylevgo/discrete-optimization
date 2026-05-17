#!/bin/bash

set -euo pipefail

cd tasks

cd 1-set-cover
python3 ./src/checker.py ./results
cp ./results/verdict.txt .
cd ..

cd 2-knapsack
python3 ./src/checker.py ./results
cp ./results/verdict.txt .
cd ..

cd 3-coloring
python3 ./src/checker.py ./results
cp ./results/verdict.txt .
cd ..

cd 4-tsp
python3 ./src/checker.py ./results
cp ./results/verdict.txt .
cd ..

cd 5-facility-location
python3 ./src/checker.py ./results
cp ./results/verdict.txt .
cd ..

cd 6-vrp
python3 ./src/checker.py ./results
cp ./results/verdict.txt .
cd ..
