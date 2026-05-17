#include "tabu.hpp"
#include "problem.hpp"

#include <bits/stdc++.h>

using namespace std;

Solution greedy(const Problem& p, auto&& order, auto&& shop_friendliness) {
    Solution sol(p);
    for (auto [i, c] : views::iota(0, (int)p.customers.size())
                     | views::transform(order)
                     | views::transform([&](int i) {return pair{i, p.customers[i]};})) {
        int best_j = -1;
        double best_dist = 1e18;
        for (auto [j, s] : p.shops | views::enumerate) {
            double cost = distance(s.location, c.location) + (sol.is_opened(j) ? 0 : s.cost) * shop_friendliness(j);
            if (sol.free_capacity(j) >= c.demand && cost < best_dist) {
                best_dist = cost;
                best_j = j;
            }
        }
        sol.set(i, best_j);
    }
    return sol;
}

Solution multi_greed(const Problem& p) {
    mt19937 gen(31);
    Solution best = greedy(p, [](int i) {return i;}, [](int) {return 0.;});
    for (int part = 1; part <= 10; ++part) {
        vector<int> order(from_range, views::iota(0, (int)p.customers.size()));
        for (int it = 0; it < 100; ++it) {
            Solution other = greedy(p, [&](int i) {return order[i];}, [part](int) {return part / 10.;});
            if (other < best) {
                best = other;
            }
            ranges::shuffle(order, gen);
        }
    }
    return best;
}

int main(int argc, char** argv) {
    Problem p;
    p.read();
    // if (p.shops.size() <= 200) {
    //     call_python(argv[1], "./src/linprog.py");
    // }
    Solution ans = multi_greed(p);
    if (p.customers.size() > 25) {
        improve_with_tabu_search(p, ans, 10s);
    }
    ans.print();
}
