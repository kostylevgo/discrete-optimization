#pragma once

#include "problem.hpp"
#include "local_opt.hpp"

#include <bits/stdc++.h>

using namespace std;

template <class Tag>
Car find_containing(const Problem& p, int i, vector<int>& cnt) {
    vector<int> ans = {i};
    ++cnt[i];
    int cap = p.car_capacity - p.customers[i].demand;
    while (true) {
        int best_j = -1;
        double best_dist = 1e18;
        for (int j = 0; j < cnt.size(); ++j) {
            if (cnt[j] > 0 || cap < p.customers[j].demand) continue;
            ans.push_back(j);
            double dist = Car(Tag{}, p, ans).length();
            if (dist < best_dist) {
                best_dist = dist;
                best_j = j;
            }
            ans.pop_back();
        }
        if (best_j == -1) break;
        ++cnt[best_j];
        cap -= p.customers[best_j].demand;
        ans.push_back(best_j);
    }
    for (auto x : ans) {
        --cnt[x];
    }
    return Car(Tag{}, p, ans);
}

template <class Tag, bool kOptimize = false>
void random(const Problem& p, Solution& cur, optional<Solution>& best, Stopwatch& watch, mt19937& gen) {
    if (cur.are_all_visited()) {
        if (!std::is_same_v<Tag, Car::SolveGoodTag>) {
            cur.optimize();
        }
        if (kOptimize) {
            while (local_optimize(cur));
        }
        if (!best || cur < *best) {
            best = cur;
        }
        return;
    }
    if (p.cars == cur.car_count()) {
        return;
    }
    watch();
    vector<int> unused;
    for (int i = 0; i < p.customers.size(); ++i) {
        if (!cur.is_used(i)) {
            unused.push_back(i);
        }
    }
    int x = unused[gen() % unused.size()];
    Car c = find_containing<Tag>(p, x, cur.usages());
    cur.add(c);
    random<Tag>(p, cur, best, watch, gen);
    cur.pop();
}

optional<Solution> solve_random(const Problem& p, Stopwatch::Duration timeout_fast = 5s, Stopwatch::Duration timeout_good = 5s) {
    Solution s(p);
    optional<Solution> ans;
    mt19937 gen(31);

    Stopwatch watch_fast("random_fast", timeout_fast);
    try {
    while(true) {
        random<Car::SolveFastTag>(p, s, ans, watch_fast, gen);
    }
    } catch (const Stopwatch::TimeoutException&) {}

    Stopwatch watch_good("random_good", timeout_good);
    try {
    while(true) {
        random<Car::SolveGoodTag>(p, s, ans, watch_good, gen);
    }
    } catch (const Stopwatch::TimeoutException&) {}

    return ans;
}

optional<Solution> solve_random_optimize(const Problem& p, Stopwatch::Duration timeout_fast = 5s, Stopwatch::Duration timeout_good = 5s) {
    Solution s(p);
    optional<Solution> ans;
    mt19937 gen(31);

    Stopwatch watch_fast("random_fast_optimized", timeout_fast);
    try {
    while(true) {
        random<Car::SolveFastTag, true>(p, s, ans, watch_fast, gen);
    }
    } catch (const Stopwatch::TimeoutException&) {}

    Stopwatch watch_good("random_good_optimized", timeout_good);
    try {
    while(true) {
        random<Car::SolveGoodTag, true>(p, s, ans, watch_good, gen);
    }
    } catch (const Stopwatch::TimeoutException&) {}

    return ans;
}
