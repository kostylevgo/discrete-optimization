#pragma once

#include "../classes/solution.hpp"

#include <bits/stdc++.h>

using namespace std;

Solution greedy_fixed_starting(const Problem& p, int starting = 0) {
    Solution s(p);
    set<int> unused;
    ranges::copy(views::iota(0, (int)p.size()), inserter(unused, unused.end()));
    s.add(starting);
    unused.erase(starting);
    while (!unused.empty()) {
        int last = s.back();
        int closest = -1;
        double dist = 1e18;
        for (auto x : unused) {
            if (distance(p[last], p[x]) < dist) {
                dist = distance(p[last], p[x]);
                closest = x;
            }
        }
        if (dist == 0) {
            exit(1);
        }
        unused.erase(closest);
        s.add(closest);
    }
    return s;
}

Solution greedy(const Problem& p) {
    if (p.size() >= 2000) {
        return greedy_fixed_starting(p, 0);
    }
    return ranges::min(views::iota(0, (int)p.size()) | views::transform([&](int s) {return greedy_fixed_starting(p, s);}));
}
