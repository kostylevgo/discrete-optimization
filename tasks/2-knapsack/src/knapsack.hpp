#pragma once

#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>

const int64_t INF = 1e18;

using namespace std;

struct Item {
    int weight;
    int cost;
};

struct GreedyItemComparator {
    bool operator()(Item a, Item b) const {
        return int64_t{a.cost} * b.weight > int64_t{b.cost} * a.weight;
    }
};

struct Knapsack : public vector<Item> {
    int W;
    vector<int64_t> pref_weight, pref_cost;

    void read() {
        int n;
        cin >> n >> W;
        resize(n);
        for (auto& [weight, cost] : *this) {
            cin >> cost >> weight;
        }
        ranges::sort(*this, GreedyItemComparator());
        pref_weight.reserve(size() + 1);
        pref_cost.reserve(size() + 1);
        pref_weight.push_back(0);
        pref_cost.push_back(0);
        for (auto [weight, cost] : *this) {
            pref_weight.push_back(pref_weight.back() + weight);
            pref_cost.push_back(pref_cost.back() + cost);
        }
    }
};

struct Answer : public vector<Item> {
    int64_t total_cost = 0;
    int64_t total_weight = 0;

    void add(Item item) {
        total_cost += item.cost;
        total_weight += item.weight;
        push_back(item);
    }

    void pop() {
        total_cost -= back().cost;
        total_weight -= back().weight;
        pop_back();
    }

    void print() const {
        std::cout << "Score: " << total_cost << '\n';
        for (auto [weight, cost] : *this) {
            std::cout << cost << ' ' << weight << '\n';
        }
    }

  private:
    using vector<Item>::push_back;
    using vector<Item>::pop_back;
};