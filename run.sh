#!/bin/bash

set -euo pipefail

cd 1-set-cover
rm results -rf
rm build -rf
cmake -S ./src -B build
make
cd ..

cd 2-knapsack
rm results -rf
./run-all.sh
cd ..

cd 3-coloring
rm results -rf
./install.sh
./run-all.sh
cd ..

cd 4-tsp
rm results -rf
./run-all.sh
cd ..
