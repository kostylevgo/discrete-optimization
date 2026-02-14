#include "constructive_methods/beam_search.hpp"
#include "set_cover.hpp"

void greedy(const SetCover& task) {
    SetCover greedy = greedy_solution(task);
    greedy.print();
}

void beam(const SetCover& task, size_t bound) {
    SetCover beam = beam_search(task, bound);
    beam.print();
}

int main() {
    SetCover task = SetCover::read();
    if (task.number_of_sets() < 200) {
        beam(task, 10000);
    } else if (task.number_of_sets() < 500) {
        beam(task, 100);
    } else if (task.number_of_sets() <= 1000) {
        beam(task, 10);
    } else {
        greedy(task);
    }
}
