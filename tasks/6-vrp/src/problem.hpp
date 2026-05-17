#pragma once

#include <util/points.hpp>
#include <util/vector_io.hpp>

#include "tsp.hpp"

using namespace std;

struct Customer {
    int demand;
    Point location;
};

istream& operator>>(istream& is, Customer& c) {
    return is >> c.demand >> c.location;
}

struct Problem {
    int cars;
    int car_capacity;
    vector<Customer> customers;
    Point origin;

    void read() {
        int n;
        int zero;
        cin >> n >> cars >> car_capacity;
        customers.resize(n);
        cin >> customers;
        origin = customers[0].location;
        customers.erase(customers.begin());
    }

    int total_demand() {
        return ranges::fold_left(customers | views::transform(&Customer::demand), 0, plus());
    }

    void print_statistics() {
        cerr << "total demand: " << total_demand() << ", total capacity: " << car_capacity * cars << endl;
    }
};

class Car {
  public:
    struct SolveGoodTag {static constexpr tsp::Solution(*kMethod)(tsp::Problem) = tsp::solve;};
    struct SolveFastTag {static constexpr tsp::Solution(*kMethod)(tsp::Problem) = tsp::solve_fast;};

    explicit Car(const Problem& p): p_(&p), remaining_capacity(p.car_capacity) {}

    template <typename Tag>
    explicit Car(Tag, const Problem& p, const vector<int>& indices): Car(p) {
        vector<Point> points;
        points.reserve(indices.size() + 1);
        ranges::copy(indices | views::transform([&](int i) {return p.customers[i].location;}), back_inserter(points));
        points.push_back(p.origin);
        tsp::Solution s = Tag::kMethod(std::move(points));
        ranges::rotate(s, ranges::find(s, indices.size()));
        route_.reserve(indices.size());
        for (int i = 1; i < s.size(); ++i) {
            add(indices[s[i]]);
        }
    }

    int operator[](int i) const {
        return route_[i];
    }

    const vector<int>& route() const {
        return route_;
    }

    size_t size() const {
        return route().size();
    }

    void optimize() {
        *this = Car(SolveGoodTag{}, *p_, route_);
    }

    void shuffle(mt19937& gen) {
        ranges::shuffle(route_, gen);
    }

    double length() const {
        Point last_point = p_->origin;
        double ans = 0;
        for (auto x : route_) {
            ans += distance(last_point, p_->customers[x].location);
            last_point = p_->customers[x].location;
        }
        ans += distance(last_point, p_->origin);
        return ans;
    }

    void add(int index) {
        remaining_capacity -= p_->customers[index].demand;
        route_.push_back(index);
    }

    void erase(int pos) {
        remaining_capacity += p_->customers[route_[pos]].demand;
        route_.erase(route_.begin() + pos);
    }

    bool valid() const {
        return remaining_capacity >= 0;
    }

    int overflow() const {
        if (valid()) return 0;
        return -remaining_capacity;
    }

  private:
    const Problem* p_;
    int remaining_capacity;
    vector<int> route_;
};

struct Solution : private vector<Car> {
    explicit Solution(const Problem& p): usages_(p.customers.size()) {}

    double length() const {
        return ranges::fold_left(static_cast<const vector<Car>&>(*this) | views::transform(&Car::length), 0., plus());
    }

    bool is_used(int i) const {
        return usages_[i] > 0;
    }

    bool are_all_visited() const {
        for (auto x : usages_) {
            if (x == 0) {
                return false;
            }
        }
        return true;
    }

    void add_to_car(int car_index, int item) {
        ++usages_[item];
        (*this)[car_index].add(item);
    }

    void shuffle_car(int car_index, mt19937& gen) {
        (*this)[car_index].shuffle(gen);
    }

    void add(Car car) {
        for (auto x : car.route()) {
            ++usages_[x];
        }
        push_back(std::move(car));
    }

    void optimize() {
        for (auto& x : *this) {
            x.optimize();
        }
    }

    void pop() {
        erase(car_count() - 1);
    }

    void erase(int index) {
        auto it = (*this).begin() + index;
        for (auto x : it->route()) {
            --usages_[x];
        }
        vector::erase(it);
    }

    const Car& car(int index) const {
        return (*this)[index];
    }

    size_t car_count() const {
        return size();
    }

    vector<int>& usages() {
        return usages_;
    }

    void print() {
        cout << "Score: " << setprecision(3) << fixed << length() << endl;
        for (auto& car : *this) {
            for (auto x : car.route()) {
                cout << x + 1 << ' ';
            }
            cout << endl;
        }
    }

    int overflow() const {
        int ans = 0;
        for (auto& c : *this) {
            ans += c.overflow();
        }
        return ans;
    }

    partial_ordering operator<=>(const Solution& other) const {
        return length() <=> other.length();
    }

    bool operator==(const Solution&) const = default;

  private:
    vector<int> usages_;
};
