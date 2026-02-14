cmake_minimum_required(VERSION 3.20)
project(set_cover)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-O3)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_DEBUG -fsanitize=address,undefined -g")

add_executable(set_cover main.cpp)
add_executable(checker checker.cpp)
