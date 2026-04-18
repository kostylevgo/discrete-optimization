#pragma once

#include "coloring.hpp"
#include <util/stopwatch.hpp>

struct Entry {
    int colors_nearby;
    int degree;
    int v;

    strong_ordering operator<=>(const Entry&) const = default;
    bool operator==(const Entry&) const = default;
};

// #include "absl/container/btree_set.h"
// #include "absl/container/btree_map.h"
// using Map = absl::btree_map<int, int>;
// using Set = absl::btree_set<Entry>;
// x1.5 performance

using Map = std::map<int, int>;
using Set = std::set<Entry>;

#include <bits/stdc++.h>

using namespace std;

class DSaturAwareGraph {
  private:
    friend struct Guard;

    class Guard {
      public:
        explicit Guard(DSaturAwareGraph& me, int v): me(me), ref(me.colors_[v]), old_size(ref.size()) {
        }

        ~Guard() {
            int v = &ref - me.colors_.data();
            if (ref.size() != old_size && me.c[v] == Coloring::kNoColor) {
                auto entry = me.entry(v);
                bool success = me.sorted_degrees.emplace(entry).second;
                assert(success);
                entry.colors_nearby = old_size;
                success = me.sorted_degrees.erase(entry) == 1;
                assert(success);
            }
        }

        Map& operator*() & {
            return ref;
        }

        const Map& operator*() const & {
            return ref;
        }

        Map* operator->() & {
            return &ref;
        }

        const Map* operator->() const & {
            return &ref;
        }

      private:
        DSaturAwareGraph& me;
        Map& ref;
        int old_size;
    };

  public:
    explicit DSaturAwareGraph(const Graph& g) 
      : g(g)
      , c(g)
      , colors_(g.size()) {
        for (int v = 0; v < g.size(); ++v) {
            sorted_degrees.emplace_hint(sorted_degrees.end(), entry(v));
        }
    }

    void color(int v, int color) {
        uncolor(v);
        c[v] = color;
        for (auto u : g[v]) {
            auto l = colors(u);
            auto it = l->insert({color, 0}).first;
            ++it->second;
        }
        bool success = sorted_degrees.erase(entry(v)) == 1;
        assert(success);
    }

    void uncolor(int v) {
        int old_color = c[v];
        if (old_color == Coloring::kNoColor) {
            return;
        }
        c[v] = Coloring::kNoColor;
        for (auto u : g[v]) {
            auto l = colors(u);
            auto it = l->find(old_color);
            --it->second;
            if (it->second == 0) {
                colors_[u].erase(it);
            }
        }
        bool success = sorted_degrees.insert(entry(v)).second;
        assert(success);
    }

    int pick() const {
        if (sorted_degrees.empty()) {
            return -1;
        }
        return (--sorted_degrees.end())->v;
    }

    Guard colors(int v) {
        return Guard(*this, v);
    }

    const Coloring& get_c() const & {
        return c;
    }

    Coloring&& get_c() && {
        return std::move(c);
    }

    vector<int> get_used_colors(int v) {
        vector<int> ans;
        auto l = colors(v);
        for (auto [col, cnt] : *l) {
            ans.push_back(col);
        }
        return ans;
    }

  private:
    Entry entry(int v) {
        return {(int)colors_[v].size(), (int)g[v].size(), v};
    }

  private:
    const Graph& g;
    Coloring c;
    Set sorted_degrees;
    vector<Map> colors_;
};

Coloring d_satur(const Graph& gr) {
    DSaturAwareGraph g(gr);
    for (int i = 0; i < gr.size(); ++i) {
        int v = g.pick();
        auto l = g.colors(v);
        int color = 0;
        for (auto [col, _] : *l) {
            if (col != color) {
                break;
            }
            ++color;
        }
        g.color(v, color);
    }
    return std::move(g).get_c();
}

void branching(DSaturAwareGraph& g, Coloring& answer, int& best_k, int my_max_k, Stopwatch& watch, bool is_first = true) {
    if (my_max_k >= best_k) {
        return;
    }
    int v = g.pick();
    if (v == -1) {
        answer = g.get_c();
        best_k = my_max_k;
        return;
    }
    watch();
    vector<int> used_colors = g.get_used_colors(v);
    int ptr = 0;
    for (int i = 0; i < best_k - 1; ++i) {
        if (ptr < used_colors.size() && used_colors[ptr] < i) {
            ++ptr;
        }
        if (ptr < used_colors.size() && used_colors[ptr] == i) {
            continue;
        }
        g.color(v, i);
        branching(g, answer, best_k, max(my_max_k, i + 1), watch, false);
        g.uncolor(v);
        if (is_first) {
            break;
        }
    }
}

void improve_with_branching(const Graph& g, Coloring& answer, Stopwatch::Duration timeout = 10s) {
    Stopwatch watch("branching", timeout);
    /* ticks:
     * 50:   2338
     * 70:   16841510
     * 100:  15477192
     * 250:  515657
     * 500:  16214348
     * 1000: 1754977
     */
    int best_k = answer.colors();
    DSaturAwareGraph gr(g);
    try {
        branching(gr, answer, best_k, 0, watch);
    } catch (const Stopwatch::TimeoutException&) {}
}
