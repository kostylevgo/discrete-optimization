#include "genetics.hpp"
#include "branching.hpp"

#include "util/stopwatch.hpp"

#include <bits/stdc++.h>

using namespace std;

Order greedy_order(const Graph& g) {
    Order result(from_range, views::iota(0, (int)g.size()));
    ranges::sort(result, greater(), [&](int v) {return g[v].size();});
    return result;
}

Coloring greedy(const Graph& g) {
    return g.evaluate(greedy_order(g));
}

Coloring timed_greedy_randomized(const Graph& g, Stopwatch::Duration timeout = 10s) {
    Stopwatch watch("greedy", timeout);
    int n = g.size();
    Order o = greedy_order(g);
    mt19937 rnd(31);
    Coloring answer = g.evaluate(o);
    try {

    for (;; watch()) {
        ranges::shuffle(o, rnd);
        Coloring other = g.evaluate(o);
        if (other < answer) {
            answer = other;
        }
    }

    } catch (const Stopwatch::TimeoutException&) {}
    return answer;
}

Coloring greedy_randomized(const Graph& g) {
    int n = g.size();
    Order o = greedy_order(g);
    mt19937 rnd(31);
    Coloring answer = g.evaluate(o);

    for (int it = 0; it <= 1e9 / (n * n); ++it) {
        ranges::shuffle(o, rnd);
        Coloring other = g.evaluate(o);
        if (other < answer) {
            answer = other;
        }
    }

    return answer;
}

Coloring from_genetics(const Graph& g, Stopwatch::Duration timeout = 10s) {
    Order o = greedy_order(g);
    genetics(g, o, 100, timeout);
    return g.evaluate(o);
}

int main() {
    Graph gr;
    gr.read();
    // Coloring answer = greedy(gr);
    // Coloring answer = greedy_randomized(gr);
    // Coloring answer = from_genetics(gr, 5min);
    Coloring answer = d_satur(gr);
    improve_with_branching(gr, answer, 1min);
    answer.print();
}
