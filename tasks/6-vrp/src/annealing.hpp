#pragma once

#include "random.hpp"
#include "problem.hpp"

#include <bits/stdc++.h>

using namespace std;

Solution mutate(Solution s, mt19937& gen) {
    Solution s2 = s;
    while (true) {
        if (gen() & 1) {
            int i = gen() % s.car_count();
            int j = gen() % (s.car_count() - 1);
            if (j >= i) ++j;
            if (j > i) swap(i, j);

            if (s.car(i).size() == 0 || s.car(j).size() == 0) {
                continue;
            }

            int i_pos = gen() % s.car(i).size();
            int j_pos = gen() % s.car(j).size();

            Car new_i_car = s.car(i);
            new_i_car.erase(i_pos);
            new_i_car.add(s.car(j).route()[j_pos]);

            Car new_j_car = s.car(j);
            new_j_car.erase(j_pos);
            new_j_car.add(s.car(i).route()[i_pos]);

            new_i_car.optimize();
            new_j_car.optimize();

            s2.erase(i);
            s2.erase(j);
            s2.add(new_i_car);
            s2.add(new_j_car);
            s = s2;
        } else {
            int i = gen() % s.car_count();
            int j = gen() % (s.car_count() - 1);
            if (j >= i) ++j;

            if (s.car(i).size() == 0) {
                continue;
            }

            int i_pos = gen() % s.car(i).size();

            Car new_j_car = s.car(j);
            new_j_car.add(s.car(i).route()[i_pos]);

            Car new_i_car = s.car(i);
            new_i_car.erase(i_pos);

            new_i_car.optimize();
            new_j_car.optimize();

            if (i > j) {
                s2.erase(i);
                s2.erase(j);
            } else {
                s2.erase(j);
                s2.erase(i);
            }
            s2.add(new_i_car);
            s2.add(new_j_car);
        }
        return s2;
    }
}

void simulated_annealing(const Problem& p, Solution& s, optional<Solution>& best, double t0, double t_step, int opts_per_step, double start_overflow_penalty, mt19937& gen) {
    uniform_real_distribution<double> distr(0.0, 1.0);

    for (bool improved = true; improved;) {
        improved = false;
        double cur_cost = s.length() + s.overflow() * start_overflow_penalty;
        for (int opt = 0; opt < opts_per_step; ++opt) {
            Solution s2 = mutate(s, gen);
            double s2_cost = s2.length() + s2.overflow() * start_overflow_penalty;
            if (s2_cost < cur_cost || (distr(gen) < exp((cur_cost - s2_cost) / t0))) {
                s = s2;
                improved = true;
            }
            if (s2.overflow() == 0 && (!best || s2_cost < best->length())) {
                best = s2;
            }
        }
        t0 *= t_step;
        start_overflow_penalty /= t_step;
    }
}

Solution generate_completely_random(const Problem& p, mt19937& gen) {
    Solution s(p);
    for (int i = 0; i < p.cars; ++i) {
        s.add(Car(p));
    }
    for (int i = 0; i < p.customers.size(); ++i) {
        s.add_to_car(gen() % p.cars, i);
    }
    for (int i = 0; i < p.cars; ++i) {
        s.shuffle_car(i, gen);
    }
    return s;
}

struct Success {};

void random_generate_step(const Problem& p, Solution& cur, mt19937& gen) {
    if (cur.are_all_visited()) {
        throw Success{};
    }
    if (p.cars == cur.car_count()) {
        return;
    }
    vector<int> unused;
    for (int i = 0; i < p.customers.size(); ++i) {
        if (!cur.is_used(i)) {
            unused.push_back(i);
        }
    }
    int x = unused[gen() % unused.size()];
    Car c = find_containing<Car::SolveFastTag>(p, x, cur.usages());
    cur.add(c);
    random_generate_step(p, cur, gen);
    cur.pop();
}

Solution generate_random_greedy(const Problem& p, mt19937& gen) {
    Solution s(p);
    try {

    while (true) random_generate_step(p, s, gen);

    } catch (const Success&) {}
    return s;
}

double estimate_t0(const Problem& p) {
    double max_dist = 0;
    for (auto x : p.customers) {
        max_dist = max(max_dist, distance(p.origin, x.location));
    }
    return max_dist / 20;
}

double estimate_start_overflow_penalty(const Problem& p) {
    double t0 = estimate_t0(p);
    t0 /= p.car_capacity;
    return t0 * 20;
}

optional<Solution> solve_annealings(const Problem& p, Stopwatch::Duration timeout = 10s) {
    mt19937 gen(31);
    Stopwatch watch("annealing", timeout);
    optional<Solution> best;

    double t0 = estimate_t0(p);
    double t_step = 0.90;
    int opts_per_step = 200;
    double start_overflow_penalty = estimate_start_overflow_penalty(p);

    while (!watch.expired()) {
        Solution s = generate_random_greedy(p, gen);
        simulated_annealing(p, s, best, t0, t_step, opts_per_step, start_overflow_penalty, gen);
        if (s.overflow() > 0) continue;
        if (!best || best->length() > s.length()) {
            best = s;
        }
    }

    return best;
}
