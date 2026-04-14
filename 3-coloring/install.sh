#!/bin/bash

set -euo pipefail

# rm abseil-cpp -rf
# git clone https://github.com/abseil/abseil-cpp.git
# cd abseil-cpp
# mkdir build && cd build
# cmake -DABSL_BUILD_TESTING=ON -DABSL_USE_GOOGLETEST_HEAD=ON -DCMAKE_CXX_STANDARD=23 ..
# cd ../..

rm build-debug -rf
rm build-release -rf
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
