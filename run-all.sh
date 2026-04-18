#!/bin/bash

set -euo pipefail

cd tasks

cd 1-set-cover
rm venv -rf
rm build -rf
rm results -rf
# rm results* -rf
make
# make all_scored
cd ..

cd 2-knapsack
rm results -rf
./run.sh
cd ..

cd 3-coloring
rm results -rf
./install.sh
./run.sh
cd ..

cd 4-tsp
rm results -rf
./run.sh
cd ..
