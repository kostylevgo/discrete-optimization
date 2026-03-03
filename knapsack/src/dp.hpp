#pragma once

#include "knapsack.hpp"

struct Parent {
    Item taken_item;
    Parent* next_one = nullptr;
};

Parent* const kInitial = (Parent*)1;

Answer solve_dp(Knapsack problem) {
    std::ranges::sort(problem.items, GreedyItemComparator());
    std::vector<int64_t> dp(problem.W + 1, -INF);
    std::vector<Parent*> parent(problem.W + 1, nullptr);
    dp[0] = 0;
    parent[0] = kInitial;
    int sum_weights = 0;
    size_t parent_allocs = 0;
    for (auto item : problem.items) {
        auto [weight, cost] = item;
        sum_weights += weight;
        sum_weights = std::min(sum_weights, problem.W);
        for (int w = sum_weights; w >= weight; --w) {
            if (parent[w - weight] != nullptr && dp[w] < dp[w - weight] + cost) {
                dp[w] = dp[w - weight] + cost;
                parent[w] = new Parent{item, parent[w - weight]};
                ++parent_allocs;
            }
        }
    }
    auto best_it = std::ranges::max_element(dp);
    Parent* best_parent = parent[best_it - dp.begin()];
    Answer answer;
    while (best_parent != kInitial) {
        answer.add(best_parent->taken_item);
        best_parent = best_parent->next_one;
    }
    std::cerr << "Parent allocs: " << parent_allocs << std::endl;
    return answer;
}
