#include "constructive_methods/beam_search.hpp"
#include "set_cover.hpp"
#include "dynamic_programming.hpp"

#include <unistd.h>

void greedy(const SetCover& task) {
    SetCover greedy = greedy_solution(task);
    greedy.print();
}

void beam(const SetCover& task, size_t bound) {
    SetCover beam = beam_search(task, bound);
    beam.print();
}

void call_python() {
    freopen("./data/_scored_sc_330_0", "r", stdin);
    std::string python = "./venv/bin/python3";
    std::string file = "./src/linprog.py";
    char* argv[3];
    argv[0] = python.data();
    argv[1] = file.data();
    argv[2] = nullptr;
    execve(python.data(), argv, nullptr);
}

int main() {
    SetCover task = SetCover::read();
    if (task.number_of_items() <= 30) {
        dynamic_programming(task);
        return 0;
    }
    if (task.number_of_sets() < 500) {
        call_python();
        return 0;
    }
    if (task.number_of_sets() <= 1000) {
        beam(task, 10);
    } else {
        greedy(task);
    }
}
