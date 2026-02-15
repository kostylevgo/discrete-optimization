#pragma once

#include "set_cover.hpp"

struct State {
    int cost = -1;
    int parent = -1;
    int index = -1;
};

void dynamic_programming(const SetCover& cover) {
    size_t items = cover.number_of_items();
    std::vector<State> dp(1 << items);
    dp[0].cost = 0;
    auto sets = cover.get_sets();
    for (int index = 0; index < sets.size(); ++index) {
        auto s = sets[index];
        int mask = 0;
        for (auto x : s.items) {
            mask |= 1 << x;
        }
        for (int new_mask = mask; new_mask < dp.size(); ++new_mask, new_mask |= mask) {
            int old_mask = new_mask ^ mask;
            if (dp[old_mask].cost == -1) continue;
            int new_cost = dp[old_mask].cost + s.cost;
            if (dp[new_mask].cost == -1 || dp[new_mask].cost > new_cost) {
                dp[new_mask] = State{new_cost, old_mask, index};
            }
        }
    }
    if (dp.back().cost == -1) {
        std::cout << "Solution is incomplete" << '\n';
        return;
    }
    std::vector<int> indices;
    int mask = dp.size() - 1;
    while (mask != 0) {
        auto [_, parent, index] = dp[mask];
        indices.push_back(index);
        mask = parent;
    }
    int64_t answer = 0;
    for (auto x : indices) {
        answer += sets[x].cost;
    }
    std::cout << "Score: " << answer << '\n';
    for (auto x : indices) {
        std::cout << sets[x].cost;
        for (auto i : sets[x].items) {
            std::cout << ' ' << i;
        }
        std::cout << '\n';
    }
}