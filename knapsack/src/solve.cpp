#include "greedy-random.hpp"
#include "constructive-methods/beam_search.hpp"
#include "dp.hpp"

int main() {
    auto problem = Knapsack::read();
    // auto solution = greedy_solution(ConstructiveKnapsack(problem)).ans;
    // auto solution = solve_greedy_random(problem);
    auto solution = beam_search(ConstructiveKnapsack(problem)).ans;
    // auto solution = solve_dp(problem);
    solution.print();
}
