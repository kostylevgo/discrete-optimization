#pragma once

// source: https://judge.yosupo.jp/submission/361332

#include <bits/stdc++.h>

using namespace std;

struct Double {
    double x = 0;

    constexpr Double() = default;
    constexpr Double(double x): x(x) {}

    Double& operator+=(Double other) {
        x += other.x;
        return *this;
    }

    Double& operator-=(Double other) {
        x -= other.x;
        return *this;
    }

    Double& operator*=(Double other) {
        x *= other.x;
        return *this;
    }

    Double& operator/=(Double other) {
        x /= other.x;
        return *this;
    }

    friend Double operator+(Double a, Double b) {
        a += b;
        return a;
    }

    friend Double operator-(Double a, Double b) {
        a -= b;
        return a;
    }

    friend Double operator*(Double a, Double b) {
        a *= b;
        return a;
    }

    friend Double operator/(Double a, Double b) {
        a /= b;
        return a;
    }

    partial_ordering operator<=>(const Double&) const = default;

    bool operator==(const Double& other) const {
        return fabs(x - other.x) < 1e-3;
    }
};

template <class W, W INF, int N>
struct MaxWeightGeneralMatching {
    struct edge {
        int u, v;
        W w;
        edge() : u(0), v(0), w(0) {}
        edge(int _u, int _v, W _w) : u(_u), v(_v), w(_w) {}
    };

    int n, n_x;
    edge g[N * 2][N * 2];
    W lab[N * 2];
    int match[N * 2], slack[N * 2], st[N * 2], pa[N * 2];
    int flo_from[N * 2][N + 1], S[N * 2], vis[N * 2];
    vector<int> flo[N * 2];
    queue<int> q;

    int flo_pos[N * 2][N * 2];
    int flo_start[N * 2], flo_dir[N * 2];

    int in_tree_base[N * 2];
    vector<int> tree_vertices;

    int in_tree_all_base[N * 2];
    vector<int> tree_all_vertices;

    int in_tree_root[N * 2];
    vector<int> tree_roots;

    int in_slack_root[N * 2];
    vector<int> slack_roots;

    MaxWeightGeneralMatching(int _n = 0) {
        if (_n > 0) init(_n);
    }

    void init(int _n) {
        n = _n;
        n_x = n;
        for (int i = 0; i <= 2 * n; i++) {
            match[i] = slack[i] = st[i] = pa[i] = S[i] = vis[i] = 0;
            flo[i].clear();
            in_tree_root[i] = in_slack_root[i] = 0;
            for (int j = 0; j <= 2 * n; j++) {
                g[i][j] = edge(i, j, W{0});
            }
        }
    }

    void add_edge(int u, int v, W w) {
        if (w <= g[u][v].w) return;
        g[u][v].w = g[v][u].w = w;
    }

    W e_delta(const edge &e) { return lab[e.u] + lab[e.v] - e.w * W{2}; }

    void touch_root(int x) {
        if (!in_tree_root[x]) {
            in_tree_root[x] = 1;
            tree_roots.push_back(x);
        }
    }

    void touch_slack_root(int x) {
        if (!in_slack_root[x]) {
            in_slack_root[x] = 1;
            slack_roots.push_back(x);
        }
    }

    void update_slack(int u, int x) {
        if (!slack[x] || e_delta(g[u][x]) < e_delta(g[slack[x]][x])) {
            if (!slack[x]) touch_slack_root(x);
            slack[x] = u;
        }
    }

    void set_slack(int x) {
        slack[x] = 0;
        for (int u : tree_vertices) {
            if (g[u][x].w > W{0} && st[u] != x && S[st[u]] == 0) update_slack(u, x);
        }
    }

    void q_push(int x) {
        if (x <= n) {
            q.push(x);
            if (!in_tree_base[x]) {
                in_tree_base[x] = 1;
                tree_vertices.push_back(x);
            }
            if (!in_tree_all_base[x]) {
                in_tree_all_base[x] = 1;
                tree_all_vertices.push_back(x);
            }
        } else for (int v_flo : flo[x]) q_push(v_flo);
    }

    void mark_tree(int x) {
        if (x <= n) {
            if (!in_tree_all_base[x]) {
                in_tree_all_base[x] = 1;
                tree_all_vertices.push_back(x);
            }
        } else for (int v_flo : flo[x]) mark_tree(v_flo);
    }

    void set_st(int x, int b) {
        st[x] = b;
        if (x > n) for (int v_flo : flo[x]) set_st(v_flo, b);
    }

    int flo_at(int b, int i) {
        int k = (int)flo[b].size();
        int idx = (flo_start[b] + flo_dir[b] * i) % k;
        if (idx < 0) idx += k;
        return flo[b][idx];
    }

    int flo_pos_view(int b, int x) {
        int k = (int)flo[b].size();
        int pu = flo_pos[b][x];
        int res = (flo_dir[b] == 1) ? (pu - flo_start[b]) : (flo_start[b] - pu);
        res %= k;
        if (res < 0) res += k;
        return res;
    }

    void flo_rotate(int b, int shift) {
        int k = (int)flo[b].size();
        int idx = (flo_start[b] + flo_dir[b] * shift) % k;
        if (idx < 0) idx += k;
        flo_start[b] = idx;
    }

    int get_pr(int b, int xr) {
        int k = (int)flo[b].size();
        int pr = flo_pos_view(b, xr);
        if (pr & 1) {
            flo_dir[b] = -flo_dir[b];
            pr = k - pr;
        }
        return pr;
    }

    void set_match(int u, int v) {
        edge e = g[u][v];
        match[u] = g[u][v].v;
        if (u <= n) return;
        int xr = flo_from[u][e.u], pr = get_pr(u, xr);
        for (int i = 0; i < pr; i++) set_match(flo_at(u, i), flo_at(u, i ^ 1));
        set_match(xr, v);
        flo_rotate(u, pr);
    }

    void augment(int u, int v) {
        while (true) {
            int xnv = st[match[u]];
            set_match(u, v);
            if (!xnv) return;
            set_match(xnv, st[pa[xnv]]);
            u = st[pa[xnv]];
            v = xnv;
        }
    }

    int get_lca(int u, int v) {
        static int t = 0;
        for (++t; u || v; swap(u, v)) {
            if (u == 0) continue;
            if (vis[u] == t) return u;
            vis[u] = t;
            u = st[match[u]];
            if (u) u = st[pa[u]];
        }
        return 0;
    }

    void add_blossom(int u, int lca, int v) {
        int b = n + 1;
        while (b <= n_x && st[b]) ++b;
        if (b > n_x) ++n_x;
        lab[b] = W{0}; S[b] = 0; match[b] = match[lca];
        flo[b] = {lca};
        for (int x = u, y; x != lca; x = st[pa[y]]) {
            flo[b].push_back(x);
            flo[b].push_back(y = st[match[x]]);
            q_push(y);
        }
        reverse(flo[b].begin() + 1, flo[b].end());
        for (int x = v, y; x != lca; x = st[pa[y]]) {
            flo[b].push_back(x);
            flo[b].push_back(y = st[match[x]]);
            q_push(y);
        }
        flo_start[b] = 0; flo_dir[b] = 1;
        for (int i = 0; i < (int)flo[b].size(); i++) flo_pos[b][flo[b][i]] = i;
        touch_root(b); mark_tree(b); set_st(b, b);
        for (int x = 1; x <= n_x; x++) g[b][x].w = g[x][b].w = W{0};
        for (int x = 1; x <= n; x++) flo_from[b][x] = 0;
        for (int xs : flo[b]) {
            for (int x = 1; x <= n_x; x++)
                if (g[b][x].w == W{0} || e_delta(g[xs][x]) < e_delta(g[b][x]))
                    g[b][x] = g[xs][x], g[x][b] = g[x][xs];
            for (int x = 1; x <= n; x++) if (flo_from[xs][x]) flo_from[b][x] = xs;
        }
        set_slack(b);
    }

    void expand_blossom(int b) {
        for (int v_flo : flo[b]) set_st(v_flo, v_flo);
        int xr = flo_from[b][g[b][pa[b]].u], pr = get_pr(b, xr);
        for (int i = 0; i < pr; i += 2) {
            int xs = flo_at(b, i), xns = flo_at(b, i + 1);
            pa[xs] = g[xns][xs].u; S[xs] = 1; S[xns] = 0;
            touch_root(xs); mark_tree(xs); touch_root(xns);
            slack[xs] = 0; set_slack(xns); q_push(xns);
        }
        S[xr] = 1; pa[xr] = pa[b];
        touch_root(xr); mark_tree(xr);
        for (int i = pr + 1; i < (int)flo[b].size(); i++) S[flo_at(b, i)] = -1, set_slack(flo_at(b, i));
        st[b] = 0;
    }

    bool on_found_edge(const edge &e) {
        int u = st[e.u], v = st[e.v];
        if (S[v] == -1) {
            pa[v] = e.u; S[v] = 1;
            touch_root(v); mark_tree(v);
            int nu = st[match[v]];
            slack[v] = slack[nu] = 0; S[nu] = 0;
            touch_root(nu); q_push(nu);
        } else if (S[v] == 0) {
            int lca = get_lca(u, v);
            if (!lca) return augment(u, v), augment(v, u), true;
            else add_blossom(u, lca, v);
        }
        return false;
    }

    bool matching() {
        fill(S + 1, S + n_x + 1, -1);
        fill(slack + 1, slack + n_x + 1, 0);
        q = queue<int>();
        tree_vertices.clear(); tree_all_vertices.clear();
        fill(in_tree_base + 1, in_tree_base + n + 1, 0);
        fill(in_tree_all_base + 1, in_tree_all_base + n + 1, 0);
        tree_roots.clear(); slack_roots.clear();
        fill(in_tree_root + 1, in_tree_root + 2 * n + 1, 0);
        fill(in_slack_root + 1, in_slack_root + 2 * n + 1, 0);

        for (int x = 1; x <= n_x; x++) if (st[x] == x && !match[x]) {
            pa[x] = 0, S[x] = 0;
            touch_root(x); q_push(x);
        }
        if (q.empty()) return false;
        while (true) {
            while (q.size()) {
                int u = q.front(); q.pop();
                if (S[st[u]] == 1) continue;
                for (int v = 1; v <= n; v++) if (g[u][v].w > W{0} && st[u] != st[v]) {
                    if (e_delta(g[u][v]) == W{0}) { if (on_found_edge(g[u][v])) return true; }
                    else update_slack(u, st[v]);
                }
            }
            W d = INF;
            for (int b : tree_roots) if (b > n && st[b] == b && S[b] == 1) d = min(d, lab[b] / W{2});
            for (int x : slack_roots) if (st[x] == x && slack[x]) {
                if (S[x] == -1) d = min(d, e_delta(g[slack[x]][x]));
                else if (S[x] == 0) d = min(d, e_delta(g[slack[x]][x]) / W{2});
            }
            for (int u : tree_all_vertices) {
                if (S[st[u]] == 0) { if (lab[u] <= d) return 0; lab[u] -= d; }
                else if (S[st[u]] == 1) lab[u] += d;
            }
            for (int b : tree_roots) if (b > n && st[b] == b) {
                if (S[st[b]] == 0) lab[b] += d * W{2};
                else if (S[st[b]] == 1) lab[b] -= d * W{2};
            }
            q = queue<int>();
            for (int x : slack_roots)
                if (st[x] == x && slack[x] && st[slack[x]] != x && e_delta(g[slack[x]][x]) == W{0})
                    if (on_found_edge(g[slack[x]][x])) return true;
            for (int b : tree_roots)
                if (b > n && st[b] == b && S[b] == 1 && lab[b] == W{0}) expand_blossom(b);
        }
    }

    pair<W, int> solve() {
        memset(match + 1, 0, sizeof(int) * n);
        n_x = n;
        int n_matches = 0;
        W w_max{0};
        W tot_weight{0};
        for (int u = 0; u <= n * 2; u++) {
            st[u] = u;
            flo[u].clear();
            for (int v = 1; v <= n; v++) flo_from[u][v] = (u == v ? u : 0);
        }
        for (int u = 1; u <= n; u++) {
            for (int v = 1; v <= n; v++) w_max = max(w_max, g[u][v].w);
        }
        for (int u = 1; u <= n; u++) lab[u] = w_max;
        while (matching()) ++n_matches;
        for (int u = 1; u <= n; u++) if (match[u] && match[u] < u) tot_weight += g[u][match[u]].w;
        return {tot_weight, n_matches};
    }   
};

void example() {
    int n, m;

    MaxWeightGeneralMatching<long long, (long long)1e18, 505> solver(n);

    for (int i = 0; i < m; i++) {
        int x, y, z;
        cin >> x >> y >> z;
        solver.add_edge(x + 1, y + 1, z); // for min weight, add M-z instead, where M=1e9
    }

    auto ans = solver.solve();
    // check perfect matching by ans.second==n/2
    cout << ans.second << " " << ans.first << "\n"; // for min weight return min weight as (ll)ans.second * M - ans.first

    for (int i = 1; i <= n; i++) {
        if (solver.match[i] && i < solver.match[i]) {
            cout << i - 1 << " " << solver.match[i] - 1 << "\n";
        }
    }
}
