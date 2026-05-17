#pragma once

#include "problem.hpp"

#include <util/stopwatch.hpp>

#include <bits/stdc++.h>

using namespace std;

double get_shop_cost(Shop shop, int free_capacity, double mult) {
    double threshold = 1 - double(free_capacity) / shop.capacity;
    if (threshold > 1) {
        // return shop.cost + shop.cost * 10 * mult * (threshold - 1);
        return 1e9;
    }
    if (free_capacity == shop.capacity) {
        return 0;
    }
    return shop.cost;
    // int steps = ceil(threshold / 10);
    // return (steps + 10 * tanh(threshold * mult)) * shop.cost / (10 + steps);
}

void improve_with_tabu_search(const Problem& p, Solution& s, Stopwatch::Duration timeout = 10s) {
    double cost = s.score();

    queue<pair<int, int>> tabu;
    vector<set<int>> cant_use(p.customers.size());
    Solution best = s;

    Stopwatch watch("tabu", timeout);

    double mult = 5;
    int threshold = 1e9;

    int nxt = 10;

    for (int it = 1; watch; ++it) {
        if (it == nxt) {
            vector<vector<int>> inverse_shops(p.shops.size());
            for (int i = 0; i < p.customers.size(); ++i) {
                inverse_shops[s.get(i)].push_back(i);
            }
            priority_queue<pair<int, int>> smallest;
            for (int i = 0; i < p.shops.size(); ++i) {
                if (inverse_shops[i].empty()) continue;
                smallest.emplace(inverse_shops[i].size(), i);
                if (smallest.size() > 10) {
                    smallest.pop();
                }
            }
            Solution cur = s;
            optional<Solution> now_best;
            while (!smallest.empty()) {
                auto [_, i] = smallest.top();
                smallest.pop();
                s = cur;
                for (auto c : inverse_shops[i]) {
                    int best_new_shop = -1;
                    double best_delta = 1e9;
                    for (int shop = 0; shop < p.shops.size(); ++shop) {
                        if (shop == i || cant_use[c].contains(shop)) continue;
                        double new_dist = distance(p.shops[shop].location, p.customers[c].location);
                        double delta_cur_shop = get_shop_cost(p.shops[i], s.free_capacity(i) + p.customers[c].demand, mult) - get_shop_cost(p.shops[i], s.free_capacity(i), mult);
                        double delta_new_shop = get_shop_cost(p.shops[shop], s.free_capacity(shop) - p.customers[c].demand, mult) - get_shop_cost(p.shops[shop], s.free_capacity(shop), mult);
                        double delta = delta_new_shop + delta_cur_shop + new_dist;
                        if (delta < best_delta) {
                            best_delta = delta;
                            best_new_shop = shop;
                        }
                    }
                    if (best_new_shop == -1) {
                        for (int shop = 0; shop < p.shops.size(); ++shop) {
                            if (shop == i || !cant_use[c].contains(shop)) continue;
                            double new_dist = distance(p.shops[shop].location, p.customers[c].location);
                            double delta_cur_shop = get_shop_cost(p.shops[i], s.free_capacity(i) + p.customers[c].demand, mult) - get_shop_cost(p.shops[i], s.free_capacity(i), mult);
                            double delta_new_shop = get_shop_cost(p.shops[shop], s.free_capacity(shop) - p.customers[c].demand, mult) - get_shop_cost(p.shops[shop], s.free_capacity(shop), mult);
                            double delta = delta_new_shop + delta_cur_shop + new_dist;
                            if (delta < best_delta) {
                                best_delta = delta;
                                best_new_shop = shop;
                            }
                        }
                    }
                    s.set(c, best_new_shop);
                }
                if (!now_best || (s.score() < now_best->score())) {
                    now_best = s;
                }
            }
            if (now_best) {
                s = *now_best;
            }
            it = 0;
            nxt += 1;
            // nxt += 10;
        }

        double best_delta = 1e9;
        int best_c = -1;
        int best_new_shop = -1;
        for (int c = 0; c < p.customers.size(); ++c) {
            int cur_shop = s.get(c);
            double cur_dist = distance(p.shops[cur_shop].location, p.customers[c].location);
            for (int shop = 0; shop < p.shops.size(); ++shop) {
                if (shop == cur_shop || cant_use[c].contains(shop)) continue;
                double new_dist = distance(p.shops[shop].location, p.customers[c].location);
                double delta_cur_shop = get_shop_cost(p.shops[cur_shop], s.free_capacity(cur_shop) + p.customers[c].demand, mult) - get_shop_cost(p.shops[cur_shop], s.free_capacity(cur_shop), mult);
                double delta_new_shop = get_shop_cost(p.shops[shop], s.free_capacity(shop) - p.customers[c].demand, mult) - get_shop_cost(p.shops[shop], s.free_capacity(shop), mult);
                double delta = delta_new_shop + delta_cur_shop + new_dist - cur_dist;
                if (delta < best_delta) {
                    best_delta = delta;
                    best_c = c;
                    best_new_shop = shop;
                }
            }
        }

        if (best_c == -1) {
            break;
        }

        s.set(best_c, best_new_shop);

        // cerr << best_c << ' ' << best_new_shop << ' ' << s.valid() << ' ' << s.score() << endl;

        cant_use[best_c].insert(best_new_shop);
        tabu.emplace(best_c, best_new_shop);
        if (tabu.size() > threshold) {
            auto [c, shop] = tabu.front();
            tabu.pop();
            cant_use[c].erase(shop);
        }

        if (s.valid() && s.score() < best.score() - 1e-3) {
            best = s;
            // cerr << "UPDATE" << ' ' << i << endl;
            while (!tabu.empty()) {
                auto [c, shop] = tabu.front();
                tabu.pop();
                cant_use[c].erase(shop);
            }
        }

        // mult += 0.01;
    }
    
    s = best;
}
