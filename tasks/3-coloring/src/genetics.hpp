#include "coloring.hpp"

#include <util/stopwatch.hpp>

vector<int> inverse_permutation(const vector<int>& p) {
    vector<int> ans(p.size());
    for (int i = 0; i < p.size(); ++i) {
        ans[p[i]] = i;
    }
    return ans;
}

Order common_subsequence(const Order& a, const Order& b) {
    int n = a.size();
    vector<int> p = inverse_permutation(b);
    vector<int> parent(n, -1);
    vector<int> dp(n + 1, n + 10);
    vector<int> who(n + 1, -1);
    dp[0] = -1;
    int best_last = -1;
    int len = 0;
    for (int i = 0; i < n; ++i) {
        int pos_in_b = p[a[i]];
        int it = ranges::lower_bound(dp, pos_in_b) - dp.begin();
        if (dp[it] == -1) {
            best_last = i;
            len = it;
        }
        dp[it] = pos_in_b;
        who[it] = i;
        parent[i] = who[it - 1];
    }
    Order ans;
    ans.reserve(len);
    while (best_last != -1) {
        ans.push_back(a[best_last]);
        best_last = parent[best_last];
    }
    ranges::reverse(ans);
    return ans;
}

Order crossingover(const Order& a, const Order& b) {
    static int seed = 31;
    mt19937 gen(++seed);

    Order l = common_subsequence(a, b);
    Order sorted_l = l;
    ranges::sort(sorted_l);
    vector<int> unused;
    unused.reserve(a.size() - l.size());
    for (int i = 0; i < a.size(); ++i) {
        auto it = ranges::lower_bound(sorted_l, i);
        if (it == sorted_l.end() || *it != i) {
            unused.push_back(i);
        }
    }
    vector<char> indices(a.size(), 'u');
    ranges::fill(indices.begin(), indices.begin() + l.size(), 'l');
    ranges::shuffle(indices, gen);

    int p_u = 0, p_l = 0;
    Order answer(from_range, indices
        | views::transform([&](char c) {
            if (c == 'u') {
                return unused[p_u++];
            }
            return l[p_l++];
    }));
    return answer;
}

void mutation(const Graph& g, pair<Coloring, Order>& p, int lr = 10) {
    auto& o = p.second;
    static int seed = 1;
    mt19937 gen(++seed);
    for (int it = 0; it < lr; ++it) {
        int i = gen() % o.size();
        int j = gen() % (o.size() - 1);
        if (j >= i) ++j;
        swap(o[i], o[j]);
    }
    p.first = g.evaluate(p.second);
}

pair<Coloring, Order> move_to_front(const Graph& g, const pair<Coloring, Order>& v) {
    Coloring::Statistic s(v.first.colors());
    s.account(v.first);
    int smallest_color = ranges::min_element(s) - s.begin();
    Order new_order;
    new_order.reserve(g.size());
    for (int i = 0; i < g.size(); ++i) {
        if (v.first[v.second[i]] == smallest_color) {
            new_order.push_back(v.second[i]);
        }
    }
    for (int i = 0; i < g.size(); ++i) {
        if (v.first[v.second[i]] != smallest_color) {
            new_order.push_back(v.second[i]);
        }
    }
    Coloring new_coloring = g.evaluate(new_order);
    return {new_coloring, new_order};
}

void local_search(const Graph& g, pair<Coloring, Order>& p) {
    pair<Coloring, Order> x = p;
    for (int i = 0; i < 10; ++i) {
        pair<Coloring, Order> new_x = move_to_front(g, x);
        if (new_x.first < p.first) {
            p = new_x;
        } else {
            break;
        }
        x = std::move(new_x);
    }
}

void genetics(const Graph& g, Order& answer, int population_size = 100, Stopwatch::Duration timeout = 10s) {
    static int seed = 1;
    mt19937 gen(++seed);

    Stopwatch watch("genetics", timeout);

    try {
    
    vector<pair<Coloring, Order>> population;
    Coloring ans_eval = g.evaluate(answer);

    population.reserve(population_size * 4);
    population.emplace_back(ans_eval, answer);

    auto birth = [&](Order&& o) {
        Coloring e = g.evaluate(o);
        auto paired = make_pair(std::move(e), std::move(o));
        local_search(g, paired);
        if (paired.first < ans_eval) {
            ans_eval = paired.first;
            answer = paired.second;
        }
        watch();
        mutation(g, paired);
        local_search(g, paired);
        if (paired.first < ans_eval) {
            ans_eval = paired.first;
            answer = paired.second;
        }
        watch();
        population.push_back(std::move(paired));
    };

    for (int i = 1; i < population_size; ++i) {
        Order o = population.back().second;
        birth(std::move(o));
    }

    while (true) {
        while (population.size() < 4 * population_size) {
            int i = gen() % population_size;
            int j = gen() % (population_size - 1);
            if (j >= i) ++j;
            Order x = crossingover(population[i].second, population[j].second);
            birth(std::move(x));
        }
        ranges::sort(population, {}, &pair<Coloring, Order>::first);
        population.erase(population.begin() + population_size, population.end());
    }

    } catch (const Stopwatch::TimeoutException&) {}
}
