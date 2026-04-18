cmake_minimum_required(VERSION 3.10)
project(set_cover)

set(CMAKE_CXX_STANDARD 23)

include_directories(../..)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_DEBUG -fsanitize=address,undefined")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3)
endif()

add_executable(main src/main.cpp)
add_executable(greedy src/main.cpp)
add_executable(beam src/main.cpp)
add_executable(basic_linprog src/main.cpp)
add_executable(greedy_linprog src/main.cpp)

target_compile_definitions(greedy PUBLIC -DGREEDY)
target_compile_definitions(beam PUBLIC -DBEAM)
target_compile_definitions(basic_linprog PUBLIC -DBASIC_LINPROG)
target_compile_definitions(greedy_linprog PUBLIC -DGREEDY_LINPROG)
