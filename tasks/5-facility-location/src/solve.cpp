#include <bits/stdc++.h>

#include <util/points.hpp>
#include <util/vector_io.hpp>

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

    bool is_opened(int shop) const {
        return free_capacity(shop) < p_->shops[shop].capacity;
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

int main() {
    Problem p;
    p.read();
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
    best.print();
}
