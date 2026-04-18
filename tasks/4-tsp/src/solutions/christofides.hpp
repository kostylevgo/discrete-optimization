#pragma once

#include "max_general_matching.hpp"
#include "../classes/solution.hpp"

#include <util/points.hpp>

#include <bits/stdc++.h>

using namespace std;

vector<pair<int, int>> mst(const Problem& p) {
    vector<pair<double, int>> min_distance(p.size(), make_pair(1e18, -1));
    vector<pair<int, int>> ans;

    auto update = [&](int i) {
        for (int j = 0; j < p.size(); ++j) {
            auto& [best_dist, best] = min_distance[j];
            double cur_dist = distance(p[i], p[j]);
            if (cur_dist < best_dist) {
                best_dist = cur_dist;
                best = i;
            }
        }
    };

    update(0);
    for (int i = 1; i < p.size(); ++i) {
        tuple<double, int, int> best = {1e18, -1, -1};
        for (int j = 0; j < p.size(); ++j) {
            if (min_distance[j].first == 0) continue;
            best = min(best, tuple<double, int, int>{min_distance[j].first, min_distance[j].second, j});
        }
        auto [_, par, v] = best;
        update(v);
        ans.emplace_back(par, v);
    }
    return ans;
}

MaxWeightGeneralMatching<Double, 1e18, 2000> m;

vector<pair<int, int>> best_matching(const Problem& p, const vector<int>& vertices) {
    m.init(vertices.size());
    for (int i = 0; i < vertices.size(); ++i) {
        for (int j = 0; j < i; ++j) {
            m.add_edge(i + 1, j + 1, 1e9 - distance(p[vertices[i]], p[vertices[j]]));
        }
    }
    m.solve();
    vector<pair<int, int>> ans;
    for (int i = 1; i <= vertices.size(); ++i) {
        if (m.match[i] && i < m.match[i]) {
            ans.emplace_back(vertices[i - 1], vertices[m.match[i] - 1]);
        }
    }
    return ans;
}

void dfs(int v, vector<bool>& used, vector<pair<int, int>>& path, vector<vector<pair<int, int>>>& g) {
    while (!g[v].empty()) {
        auto [u, i] = g[v].back();
        g[v].pop_back();
        if (!used[i]) {
            used[i] = true;
            dfs(u, used, path, g);
            path.emplace_back(u, v);
        }
    }
}

vector<pair<int, int>> euler_path(int n, const vector<pair<int, int>>& edges) {
    vector<vector<pair<int, int>>> g(n);
    for (auto [i, p] : edges | views::enumerate) {
        auto [u, v] = p;
        g[u].emplace_back(v, i);
        g[v].emplace_back(u, i);
    }
    vector<bool> used(edges.size());
    vector<pair<int, int>> path;
    dfs(0, used, path, g);
    return path;
}

Solution christofides_serduykov(const Problem& p) {
    auto mst_edges = mst(p);
    vector<int> mst_degree(p.size());
    for (auto [u, v] : mst_edges) {
        ++mst_degree[u];
        ++mst_degree[v];
    }
    vector<int> odd_degree;
    ranges::copy(mst_degree | views::enumerate | views::filter([](pair<int, int> x) {return x.second % 2 == 1;}) | views::keys, back_inserter(odd_degree));
    auto match_edges = best_matching(p, odd_degree);

    ranges::copy(match_edges, back_inserter(mst_edges));
    auto edges = std::move(mst_edges);
    vector<pair<int, int>> path = euler_path(p.size(), edges);
    
    Solution s(p);
    s.add(path[0].first);
    vector<bool> is_used(p.size());
    is_used[path[0].first] = true;
    for (int i = 0; i < path.size(); ++i) {
        int v = path[i].second;
        if (!is_used[v]) {
            is_used[v] = true;
            s.add(v);
        }
    }
    return s;
}
