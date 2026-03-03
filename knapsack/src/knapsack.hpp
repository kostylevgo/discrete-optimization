#pragma once

#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>

const int64_t INF = 1e18;

struct Item {
    int weight;
    int cost;
};

struct GreedyItemComparator {
    bool operator()(Item a, Item b) const {
        return a.cost * b.weight > b.cost * a.weight;
    }
};

struct Knapsack {
    int W;
    std::vector<Item> items;

    static Knapsack read() {
        Knapsack result;
        int n;
        std::cin >> n >> result.W;
        result.items.resize(n);
        for (auto& [weight, cost] : result.items) {
            std::cin >> cost >> weight;
        }
        return result;
    }
};

struct Answer {
    int64_t max_cost = 0;
    std::vector<Item> taken;

    void add(Item item) {
        max_cost += item.cost;
        taken.push_back(item);
    }

    void print() const {
        std::cout << "Score: " << max_cost << '\n';
        for (auto [weight, cost] : taken) {
            std::cout << cost << ' ' << weight << '\n';
        }
    }
};