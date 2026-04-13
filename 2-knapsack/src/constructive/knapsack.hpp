#pragma once

#include "../knapsack.hpp"

struct ConstructiveKnapsack {
    enum Action {
        Take,
        NotTake,
    };

    Knapsack ks;
    Answer ans;

    explicit ConstructiveKnapsack(Knapsack ks): ks(std::move(ks)) {
        ranges::reverse(ks);
    }

    bool has_actions() const {
        return !ks.empty();
    }

    std::vector<Action> actions() const {
        std::vector<Action> result;
        if (!has_actions()) {
            return result;
        }
        result.push_back(NotTake);
        if (ks.back().weight <= ks.W) {
            result.push_back(Take);
        }
        return result;
    }

    int64_t evaluate() const {
        return -ans.total_cost; // less is better
    }

    Action greedy_action() const {
        if (!ks.empty() && ks.back().weight <= ks.W) {
            return Take;
        }
        return NotTake;
    }

    void apply(Action action, bool ignored = false) {
        if (action == Take) {
            ks.W -= ks.back().weight;
            ans.add(ks.back());
        }
        ks.pop_back();
    }
};
