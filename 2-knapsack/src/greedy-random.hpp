#pragma once

#include "knapsack.hpp"
#include "constructive-methods/greedy.hpp"
#include "constructive-methods/knapsack.hpp"

#include <random>

struct ItemWithRandom {
    double ratio;
    Item item;
};

Knapsack sort_shuffle(Knapsack problem, double temperature) {
    std::mt19937 gen(31);
    std::vector<ItemWithRandom> items(problem.items.size());
    auto distr = std::uniform_real_distribution<double>();
    for (int i = 0; i < problem.items.size(); ++i) {
        auto item = problem.items[i];
        items[i] = {(double)item.cost / item.weight + distr(gen) * temperature, item};
    }
    std::ranges::sort(items, [](ItemWithRandom a, ItemWithRandom b) {
        return a.ratio > b.ratio;
    });
    problem.items.clear();
    for (auto [_, item] : items) {
        problem.items.push_back(item);
    }
    return problem;
}

Answer solve_greedy_random(Knapsack initial_problem) {
    double t = 0.0001;

    int64_t best_ans = greedy_solution(ConstructiveKnapsack(initial_problem)).ans.max_cost;
    double best_temp = 0;

    for (int i = 0; i < 20000; ++i) {
        auto problem = sort_shuffle(initial_problem, t);
        int sum_weight = 0;
        int64_t sum_cost = 0;
        for (auto item : problem.items) {
            auto [weight, cost] = item;
            if (sum_weight + weight <= problem.W) {
                sum_weight += weight;
                sum_cost += cost;
            }
        }
        if (sum_cost > best_ans) {
            best_ans = sum_cost;
            best_temp = t;
        }
        t *= 1.001;
    }

    auto problem = sort_shuffle(initial_problem, best_temp);
    int sum_weight = 0;
    int64_t sum_cost = 0;
    std::vector<Item> answer;
    for (auto item : problem.items) {
        auto [weight, cost] = item;
        if (sum_weight + weight <= problem.W) {
            sum_weight += weight;
            sum_cost += cost;
            answer.push_back(item);
        }
    }

    return {sum_cost, answer};
}
