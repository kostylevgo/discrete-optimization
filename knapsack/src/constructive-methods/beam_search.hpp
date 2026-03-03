#pragma once

#include "greedy.hpp"

#include <algorithm>
#include <compare>
#include <queue>
#include <set>

template <class P>
struct Entry {
    int64_t evaluation; // less is better
    P state;

    explicit Entry(int64_t score, P state): evaluation(score), state(std::move(state)) {}

    bool operator==(const Entry& other) const {
        return evaluation == other.evaluation;
    }
    std::strong_ordering operator<=>(const Entry& other) const {
        return evaluation <=> other.evaluation;
    }
};

template <class T>
struct Beam {
    explicit Beam(size_t bound): bound(bound) {}

    void add(T item) {
        emplace(std::move(item));
    }

    template <class... Args>
    void emplace(Args&&... args) {
        items.emplace_back(std::forward<Args>(args)...);
        if (items.size() > bound) {
            kick();
        }
    }

    T take() {
        T item = std::move(items[0]);
        items.erase(items.begin());
        return item;
    }

    void kick() {
        auto iter = std::ranges::min_element(items);
        items.erase(iter);
    }

    bool empty() const {
        return items.empty();
    }

  private:
    size_t bound;
    std::vector<T> items;
};

template <class P>
P beam_search(const P& problem, size_t bound = 1000) {
    P greedy = problem;
    greedy_process(greedy);
    int64_t greedy_score = greedy.evaluate();
    Beam<Entry<P>> beam(bound);
    beam.emplace(greedy_score, std::move(problem));
    Entry<P> best(greedy_score, std::move(greedy));

    while (!beam.empty()) {
        auto [_, current] = beam.take();
        for (auto action : current.actions()) {
            P copy = current;
            copy.apply(action);
            P greedy_copy = copy;
            greedy_process(greedy_copy);
            int64_t score = greedy_copy.evaluate();
            if (score < best.evaluation) {
                best = Entry(score, std::move(greedy_copy));
            }
            beam.emplace(score, std::move(copy));
        }
    }
    return best.state;
}