#pragma once

#include <tr2/dynamic_bitset>

#include <vector>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

const int64_t INF = 1'000'000'000'000'000'018;

struct Set {
    int64_t cost;
    std::vector<size_t> items;
    bool is_taken = false;

    void print() {
        std::cout << cost;
        for (auto x : items) {
            std::cout << ' ' << x;
        }
        std::cout << '\n';
    }
};

enum Action {
    Take,
    NotTake,
};

struct SetCover {
  public:
    static SetCover read() {
        size_t item_count, set_count;
        std::cin >> item_count >> set_count;
        std::vector<Set> sets;
        sets.reserve(set_count);
        for (size_t i = 0; i < set_count; ++i) {
            int64_t cost;
            std::cin >> cost;
            std::string line;
            std::getline(std::cin, line);
            std::istringstream iss(line);
            std::vector<size_t> items;
            size_t value;
            while (iss >> value) {
                items.push_back(value);
            }
            sets.emplace_back(cost, items);
        }
        return SetCover(
            item_count,
            std::move(sets)
        );
    }

    std::vector<Action> actions() const {
        if (has_actions()) {
            return {Take, NotTake};
        }
        return {};
    }

    bool has_actions() const {
        return viewed_sets < sets.size();
    }

    size_t number_of_sets() const {
        return sets.size();
    }

    size_t number_of_items() const {
        return item_count;
    }

    const std::vector<Set>& get_sets() const {
        return sets;
    }

    int64_t evaluate() const {
        if (covered_items.count() != item_count) {
            return INF;
        }
        return answer;
    }

    Action greedy_action() const {
        for (auto x : sets[viewed_sets].items) {
            if (!covered_items[x]) {
                return Take;
            }
        }
        return NotTake;
    }

    void apply(Action action, bool faster = false) {
        if (action == NotTake) {
            next_item(faster);
            return;
        }
        answer += sets[viewed_sets].cost;
        sets[viewed_sets].is_taken = true;
        for (auto x : sets[viewed_sets].items) {
            covered_items[x] = true;
        }
        next_item(faster);
    }

    void print() {
        int64_t score = evaluate();
        if (score == INF) {
            std::cout << "Solution is incomplete" << '\n';
        } else {
            std::cout << "Score: " << score << '\n';
        }
        for (auto& x : sets) {
            if (x.is_taken) {
                x.print();
            }
        }
    }

  private:
    explicit SetCover(size_t item_count, std::vector<Set> all_sets):
            item_count(item_count), sets(std::move(all_sets)), covered_items(item_count), usage_count(item_count) {
        for (auto& s : sets) {
            for (auto x : s.items) {
                ++usage_count[x];
            }
        }
        find_next_greedy();
    }

    void next_item(bool faster = false) {
        for (auto x : sets[viewed_sets].items) {
            --usage_count[x];
        }
        ++viewed_sets;
        if (!faster) {
            find_next_greedy();
        }
    }

    double benefit(const Set& s) {
        double answer = 0;
        for (auto x : s.items) {
            if (!covered_items[x]) {
                answer += sqrt(usage_count[x]);
            }
        }
        return answer;
    }

    void find_next_greedy() {
        auto first = sets.begin() + viewed_sets;
        auto mn = std::ranges::min_element(first, sets.end(), [&](const Set& lhs, const Set& rhs) {
            return lhs.cost / benefit(lhs) < rhs.cost / benefit(rhs);
        });
        std::swap(*first, *mn);
    }

  private:
    size_t item_count;
    std::vector<Set> sets;
    std::tr2::dynamic_bitset<> covered_items;
    std::vector<int64_t> usage_count;
    size_t viewed_sets = 0;
    int64_t answer = 0;
};
