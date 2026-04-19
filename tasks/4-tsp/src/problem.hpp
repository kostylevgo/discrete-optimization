#pragma once

#include <util/points.hpp>
#include <util/vector_io.hpp>

#include <bits/stdc++.h>

using namespace std;

using Problem = vector<Point>;

struct Solution : public vector<int> {
    using Base = vector<int>;

    explicit Solution(const Problem* p): p(p) {}

    explicit Solution(const Problem& p): Solution(&p) {}

    int safe_index(int index) const {
        index %= (int)size();
        if (index < 0) {
            index += (int)size();
        }
        return index;
    }

    int& operator[](int index) {
        return Base::operator[](safe_index(index));
    }

    const int& operator[](int index) const {
        return Base::operator[](safe_index(index));
    }

    Point point(int index) const {
        return (*p)[(*this)[index]];
    }

    void add(int index) {
        if (ranges::find(*this, index) != end()) {
            exit(*end());
        }
        Base::push_back(index);
    }

    double length() const {
        double total_dist = 0;
        for (int i = 0; i < size(); ++i) {
            total_dist += distance(point(i), point(i + 1));
        }
        return total_dist;
    }

    void print() const {
        cout << setprecision(3) << fixed << "Score: " << length() << endl << *this;
    }

    partial_ordering operator<=>(const Solution& other) const {
        return length() <=> other.length();
    }

    bool operator==(const Solution& other) const = default;

    const Problem* p;
};
