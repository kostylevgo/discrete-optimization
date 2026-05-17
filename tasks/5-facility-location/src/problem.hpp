#pragma once

#include <util/points.hpp>
#include <util/vector_io.hpp>

#include <bits/stdc++.h>

using namespace std;

struct Shop {
    double cost;
    int capacity;
    Point location;
};

istream& operator>>(istream& is, Shop& s) {
    return is >> s.cost >> s.capacity >> s.location;
}

struct Customer : public Point {
    int demand;
    Point location;
};

istream& operator>>(istream& is, Customer& c) {
    return is >> c.demand >> c.location;
}

struct Problem {
    vector<Shop> shops;
    vector<Customer> customers;

    void read() {
        int n, m;
        cin >> n >> m;
        shops.resize(n);
        customers.resize(m);
        cin >> shops >> customers;
    }
};

class Solution {
  public:
    static constexpr int kNoShop = -1;

    explicit Solution(const Problem& p)
            : p_(&p)
            , to_which_(p.customers.size(), kNoShop)
            , free_capacity_(from_range, p.shops | views::transform(&Shop::capacity)) {
    }

    int operator[](int i) const {
        return to_which_[i];
    }

    void set(int i, int shop) {
        if (to_which_[i] != kNoShop) {
            free_capacity_[to_which_[i]] += p_->customers[i].demand;
        }
        to_which_[i] = shop;
        if (shop != kNoShop) {
            free_capacity_[to_which_[i]] -= p_->customers[i].demand;
        }
    }

    int get(int i) const {
        return to_which_[i];
    }

    bool is_opened(int shop) const {
        return free_capacity(shop) < p_->shops[shop].capacity;
    }

    bool valid() const {
        for (auto x : free_capacity_) {
            if (x < 0) return false;
        }
        return true;
    }

    double score() const {
        double answer = 0;
        for (int i = 0; i < to_which_.size(); ++i) {
            answer += distance(p_->customers[i].location, p_->shops[to_which_[i]].location);
        }
        for (int i = 0; i < free_capacity_.size(); ++i) {
            if (is_opened(i)) {
                answer += p_->shops[i].cost;
            }
        }
        return answer;
    }

    int free_capacity(int i) const {
        return free_capacity_[i];
    }

    void print() const {
        cout << setprecision(3) << fixed << "Score: " << score() << endl << to_which_;
    }

    partial_ordering operator<=>(const Solution& other) const {
        return score() <=> other.score();
    }

    bool operator==(const Solution&) const = default;

  private:
    const Problem* p_;
    vector<int> to_which_;
    vector<int> free_capacity_;
};