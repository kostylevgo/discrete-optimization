#pragma once

#include "tsp.hpp"
#include "problem.hpp"

#include <util/stopwatch.hpp>

#include <bits/stdc++.h>

using namespace std;

optional<Solution> solve_plain_tsp(const Problem& p, Stopwatch::Duration timeout) {
    tsp::Problem t;
    t.push_back(p.origin);
    for (auto c : p.customers) {
        t.push_back(c.location);
    }
    tsp::Solution s = tsp::solve(t, timeout);
    auto origin_pos = ranges::find(s, 0);
    ranges::rotate(s, origin_pos);
    s.erase(s.begin());
    Solution ans(p);
    Car car(p);
    int capacity = p.car_capacity;
    for (auto i : s) {
        --i;
        if (capacity < p.customers[i].demand) {
            ans.add(car);
            car = Car(p);
            capacity = p.car_capacity;
        }
        car.add(i);
        capacity -= p.customers[i].demand;
    }
    ans.add(car);
    if (ans.car_count() > p.cars) {
        return nullopt;
    }
    return ans;
}
