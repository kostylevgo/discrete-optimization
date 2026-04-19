#include "problem.hpp"

#include <util/stopwatch.hpp>

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

template <class Tag>
void random(const Problem& p, Solution& cur, optional<Solution>& best, Stopwatch& watch, mt19937& gen) {
    if (cur.are_all_visited()) {
        if (!std::is_same_v<Tag, Car::SolveGoodTag>) {
            cur.optimize();
        }
        if (!best || cur < *best) {
            best = cur;
        }
        return;
    }
    if (p.car_capacity == cur.car_count()) {
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

int main() {
    Problem p;
    p.read();
    Solution s(p);
    optional<Solution> ans;
    mt19937 gen(31);

    Stopwatch watch_fast("branching_fast", 5s);
    try {
    while(true) {
        random<Car::SolveFastTag>(p, s, ans, watch_fast, gen);
    }
    } catch (const Stopwatch::TimeoutException&) {}

    Stopwatch watch_good("branching_good", 5s);
    try {
    while(true) {
        random<Car::SolveGoodTag>(p, s, ans, watch_good, gen);
    }
    } catch (const Stopwatch::TimeoutException&) {}

    if (!ans) {
        cout << "Fail" << endl;
    } else {
        ans->print();
    }
}
