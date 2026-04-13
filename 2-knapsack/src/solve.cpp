#include "greedy_random.hpp"
#include "constructive/beam_search.hpp"
#include "dp.hpp"
#include "branching.hpp"

int main() {
    Knapsack problem;
    problem.read();
    // auto solution = greedy_solution(ConstructiveKnapsack(problem)).ans;
    // auto solution = solve_greedy_random(problem);
    // auto solution = beam_search(ConstructiveKnapsack(problem)).ans;
    auto solution = solve_dp(problem);
    // auto solution = solve_branching(problem);
    solution.print();
}
