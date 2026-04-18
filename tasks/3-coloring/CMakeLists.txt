cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 23)
# set(ABSL_PROPAGATE_CXX_STD ON)

include_directories(../..)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_DEBUG -fsanitize=address,undefined")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3)
endif()

# add_subdirectory(abseil-cpp)

add_executable(solve ./src/solve.cpp)

# target_link_libraries(solve PRIVATE absl::btree)
