#pragma once

#include "../knapsack.hpp"

struct ConstructiveKnapsack {
    enum Action {
        Take,
        NotTake,
    };

    Knapsack ks;
    Answer ans;

    explicit ConstructiveKnapsack(Knapsack sack): ks(std::move(sack)) {
        std::ranges::sort(ks.items, GreedyItemComparator());
        std::ranges::reverse(ks.items);
    }

    bool has_actions() const {
        return !ks.items.empty();
    }

    std::vector<Action> actions() const {
        std::vector<Action> result;
        if (!has_actions()) {
            return result;
        }
        result.push_back(NotTake);
        if (ks.items.back().weight <= ks.W) {
            result.push_back(Take);
        }
        return result;
    }

    int64_t evaluate() const {
        return -ans.max_cost; // less is better
    }

    Action greedy_action() const {
        if (!ks.items.empty() && ks.items.back().weight <= ks.W) {
            return Take;
        }
        return NotTake;
    }

    void apply(Action action, bool ignored = false) {
        if (action == Take) {
            ks.W -= ks.items.back().weight;
            ans.add(ks.items.back());
        }
        ks.items.pop_back();
    }
};
