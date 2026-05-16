#pragma once

#include "coloring.hpp"

#include <util/stopwatch.hpp>

#include <bits/stdc++.h>

class ClashesAwareColoring {
  public:
    ClashesAwareColoring(const Graph& gr, Coloring c): gr_(&gr), c_(std::move(c)), colors(c_.colors()), clashes_(0) {
        for (int v = 0; v < gr.size(); ++v) {
            if (c_[v] == Coloring::kNoColor) {
                continue;
            }
            for (auto u : gr[v]) {
                if (u < v && c_[u] == c_[v]) {
                    ++clashes_;
                }
            }
        }
    }

    const Coloring& get() const {
        return c_;
    }

    void recolor(int v, int color) {
        account(v, -1);
        c_[v] = color;
        account(v, 1);
    }

    vector<int> neighbors_with_color(int v) {
        vector<int> ans(colors);
        for (auto u : (*gr_)[v]) {
            if (c_[u] != Coloring::kNoColor) {
                ++ans[c_[u]];
            }
        }
        return ans;
    }

    int clashes() const {
        return clashes_;
    }

    int clashes_at(int v) const {
        if (c_[v] == Coloring::kNoColor) {
            return 0;
        }
        int ans = 0;
        for (auto u : (*gr_)[v]) {
            if (c_[u] == c_[v]) {
                ++ans;
            }
        }
        return ans;
    }

  private:
    void account(int v, int sign) {
        if (c_[v] == Coloring::kNoColor) {
            return;
        }
        for (auto u : (*gr_)[v]) {
            if (c_[u] == c_[v]) {
                clashes_ += sign;
            }
        }
    }

  private:
    const Graph* gr_;
    Coloring c_;
    int colors;
    int clashes_;
};

bool improve_with_tabu_search(const Graph& g, Coloring& best, Stopwatch& watch) {
    int now_colors = best.colors();

    if (now_colors <= 2) {
        return false;
    }

    vector<set<int>> cant_use(g.size());
    queue<pair<int, int>> last_changed;

    Coloring colors = best;
    colors.reorder_colors_by_frequency();
    colors.drop_last_color_uniformly();

    ClashesAwareColoring c(g, std::move(colors));

    double color_ratio = ((double)g.size() / now_colors);
    double clash_coef = exp(color_ratio);

    int threshold = g.edges() / clash_coef;

    for (; c.clashes() > 0 && watch;) {
        int best_delta_clashes = 1e9;
        int best_v = -1;
        int best_new_color = -1;

        for (int v = 0; v < g.size(); ++v) {
            vector<int> table = c.neighbors_with_color(v);
            int now_color = c.get()[v];
            if (table[now_color] == 0) {
                continue;
            }
            int best_color = -1;
            for (int i = 0; i < table.size(); ++i) {
                if (i == now_color || cant_use[v].contains(i)) continue;
                if (best_color == -1 || table[i] < table[best_color]) {
                    best_color = i;
                }
            }
            if (best_color == -1) {
                continue;
            }
            int delta_clashes = table[best_color] - table[now_color];
            if (delta_clashes < best_delta_clashes) {
                best_delta_clashes = delta_clashes;
                best_v = v;
                best_new_color = best_color;
            }
        }

        if (best_v == -1) {
            return false;
        }

        last_changed.emplace(best_v, c.get()[best_v]);
        cant_use[best_v].insert(c.get()[best_v]);
        c.recolor(best_v, best_new_color);
        if (last_changed.size() > threshold) {
            auto [v, col] = last_changed.front();
            last_changed.pop();
            cant_use[v].erase(col);
        }
    }

    if (c.clashes() == 0) {
        best = c.get();
        return true;
    }

    return false;
}
