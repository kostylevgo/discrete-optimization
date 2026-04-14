#pragma once

#include <bits/stdc++.h>

using namespace std;

int find_mex(vector<int>&& v) {
    ranges::sort(v);
    auto [le, ri] = ranges::unique(v);
    v.erase(le, ri);
    int i = 0;
    for (; i < v.size() && v[i] == i; ++i) {}
    return i;
}

struct Coloring;
struct Order;

struct Graph : vector<vector<int>> {
    void read() {
        int n, m;
        cin >> n >> m;
        resize(n);
        for (int i = 0; i < m; ++i) {
            int u, v;
            cin >> u >> v;
            (*this)[u].push_back(v);
            (*this)[v].push_back(u);
        }
    }

    Coloring evaluate(const Order& order) const;
};

struct Order : vector<int> {
    using vector<int>::vector;
};

struct Coloring : vector<int> {
    static constexpr int kNoColor = -1;

    struct Statistic : vector<int> {
        using vector<int>::vector;

        void account(const Coloring& coloring) {
            for (auto x : coloring) {
                ++(*this)[x];
            }
        }
        
        strong_ordering operator<=>(const Statistic&) const = default;
        bool operator==(const Statistic&) const = default;
    };

    explicit Coloring(const Graph& g) : vector<int>(g.size(), kNoColor) {}

    int colors() const {
        return ranges::max(*this) + 1;
    }

    void print() const {
        cout << "Answer: " << colors() << endl;
        for (auto col : *this) {
            cout << col << ' ';
        }
        cout << endl;
    }

    strong_ordering operator<=>(const Coloring& other) const {
        int my_size = colors();
        int other_size = other.colors();
        if (my_size != other.size()) {
            return my_size <=> other_size;
        }
        Statistic mine(my_size), others(other_size);
        mine.account(*this);
        others.account(other);
        ranges::sort(mine);
        ranges::sort(others);
        // I and other both have n colors, but I have less vertices colored n => I am better
        return mine <=> others;
    }

    bool operator==(const Coloring& other) const = default;
};

Coloring Graph::evaluate(const Order& order) const {
    Coloring c(*this);
    vector<int> colors;

    for (auto v : order) {
        colors.clear();

        ranges::copy((*this)[v]
                | views::transform([&](auto u) {return c[u];})
                | views::filter([](auto col) {return col != Coloring::kNoColor;}),
            back_inserter(colors));

        c[v] = find_mex(std::move(colors));
    }

    return c;
}
