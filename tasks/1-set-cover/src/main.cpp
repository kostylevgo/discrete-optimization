#include "constructive_methods/beam_search.hpp"
#include "set_cover.hpp"
#include "precise_methods/dynamic_programming.hpp"

#include <unistd.h>

void greedy(const SetCover& task) {
    SetCover greedy = greedy_solution(task);
    greedy.print();
}

void beam(const SetCover& task, size_t bound) {
    SetCover beam = beam_search(task, bound);
    beam.print();
}

void call_python(char* input, std::string file) {
    freopen(input, "r", stdin);
    std::string python = "./venv/bin/python3";
    char* argv[3];
    argv[0] = python.data();
    argv[1] = file.data();
    argv[2] = nullptr;
    execve(python.data(), argv, nullptr);
}

int main(int argc, char** argv) {
    SetCover task = SetCover::read();
#ifdef BASIC_LINPROG
    call_python(argv[1], "./src/linprog/basic.py");
#elifdef GREEDY
    greedy(task);
#elifdef BEAM
    if (task.number_of_sets() < 500) {
        beam(task, 30);
    } else if (task.number_of_sets() <= 1000) {
        beam(task, 10);
    } else {
        greedy(task);
    }
#elifdef GREEDY_LINPROG
    call_python(argv[1], "./src/linprog/greedy.py");
#else
    if (task.number_of_items() <= 30) {
        dynamic_programming(task);
    } else {
        call_python(argv[1], "./src/linprog/filtering.py");
    }
#endif
}
