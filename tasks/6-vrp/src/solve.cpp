#include "random.hpp"
#include "precise.hpp"
#include "local_opt.hpp"
#include "annealing.hpp"
#include "plain_tsp.hpp"

#include "problem.hpp"

#include <util/stopwatch.hpp>

#include <bits/stdc++.h>

using namespace std;

int main() {
    Problem p;
    p.read();

    if (p.customers.size() <= 18) {
        solve_precise(p)->print();
        return 0;
    }

    optional<Solution> ans = solve_plain_tsp(p, 1min);

    optional<Solution> ans2 = solve_annealings(p, 1min);
    if (ans2 && (!ans || ans2->length() < ans->length())) {
        ans = ans2;
    }

    // optional<Solution> ans2 = solve_random(p);
    // while (local_optimize(*ans2));
    // if (ans2 && (!ans || ans2->length() < ans->length())) {
    //     ans = ans2;
    // }

    // optional<Solution> ans3 = solve_random_optimize(p);
    // if (ans3 && (!ans || ans3->length() < ans->length())) {
    //     ans = ans3;
    // }

    if (!ans) {
        cout << "Fail" << endl;
    } else {
        while (local_optimize(*ans));
        ans->print();
    }
}
