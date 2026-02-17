cmake_minimum_required(VERSION 3.20)
project(set_cover)

set(CMAKE_CXX_STANDARD 23)

add_definitions(-O3)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_DEBUG -fsanitize=address,undefined -g")

add_executable(main main.cpp)
add_executable(greedy main.cpp)
add_executable(beam main.cpp)
add_executable(basic_linprog main.cpp)
add_executable(greedy_linprog main.cpp)

target_compile_definitions(greedy PUBLIC -DGREEDY)
target_compile_definitions(beam PUBLIC -DBEAM)
target_compile_definitions(basic_linprog PUBLIC -DBASIC_LINPROG)
target_compile_definitions(greedy_linprog PUBLIC -DGREEDY_LINPROG)
