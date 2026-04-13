#pragma once

#include "knapsack.hpp"
#include "greedy_random.hpp"
#include "constructive/knapsack.hpp"
#include "constructive/beam_search.hpp"

void branch_n_bounds(int i, const Knapsack& ks, Answer& cur, Answer& best) {
    if (cur.total_cost > best.total_cost) {
        best = cur;
    }
    int64_t can_take = ks.W - cur.total_weight;
    int too_much = ranges::upper_bound(ks.pref_weight, ks.pref_weight[i] + can_take) - ks.pref_weight.begin();
    int64_t not_too_much_weight = ks.pref_weight[too_much - 1] - ks.pref_weight[i];
    if (not_too_much_weight + cur.total_weight > ks.W) {
        exit(1);
    }
    int64_t added_cost = ks.pref_cost[too_much - 1] - ks.pref_cost[i];
    int64_t cost_upper_bound = cur.total_cost + added_cost;
    if (too_much != ks.pref_weight.size()) {
        int64_t weight_available = ks.W - not_too_much_weight - cur.total_weight;
        double cost_by_weigth = double(ks[too_much - 1].cost) / ks[too_much - 1].weight;
        cost_upper_bound += floor(cost_by_weigth * weight_available);
    }
    if (cost_upper_bound <= best.total_cost) {
        return;
    }
    if (too_much - 1 > i) {
        cur.add(ks[i]);
        branch_n_bounds(i + 1, ks, cur, best);
        cur.pop();
    }
    branch_n_bounds(i + 1, ks, cur, best);
}

Answer solve_branching(const Knapsack& ks) {
    Answer answer = ks.size() > 1000 ? solve_greedy_random(ks) : beam_search(ConstructiveKnapsack(ks)).ans;
    Answer cur;
    branch_n_bounds(0, ks, cur, answer);
    return answer;
}