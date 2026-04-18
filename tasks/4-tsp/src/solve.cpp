#include "classes/solution.hpp"

#include "solutions/greedy.hpp"
#include "solutions/christofides.hpp"
#include "solutions/genetics.hpp"

#include <util/stopwatch.hpp>
#include <util/points.hpp>
#include <util/vector_io.hpp>

#include <bits/stdc++.h>

using namespace std;

int main() {
    int n;
    cin >> n;
    Problem p(n);
    cin >> p;
    Solution s = (n <= 2000 ? christofides_serduykov : greedy)(p);
    if (n <= 2000) {
        kernighan_lin(s);
        full_local_search(s);
        s = genetics(std::move(s));
    } else {
        Stopwatch watch("kernighan_lin", 20min);
        try {
            kernighan_lin(s, watch);
            // ticks: 2537
        } catch (const Stopwatch::TimeoutException&) {}
        full_local_search(s);
    }
    s.print();
}
