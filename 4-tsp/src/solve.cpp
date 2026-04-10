#include <bits/stdc++.h>
#include "max_general_matching.hpp"

using namespace std;

struct Point {
    friend istream& operator>>(istream& is, Point& p) {
        return is >> p.x >> p.y;
    }

    Point& operator-=(Point b) {
        x -= b.x;
        y -= b.y;
        return *this;
    }

    double x{0};
    double y{0};
};

Point operator-(Point a, Point b) {
    a -= b;
    return a;
}

double operator*(Point a, Point b) {
    return a.x * b.x + a.y * b.y;
}

double square_distance(Point a, Point b) {
    a -= b;
    return a * a;
}

double distance(Point a, Point b) {
    return sqrt(square_distance(a, b));
}

using Problem = vector<Point>;

template <class T>
istream& operator>>(istream& is, vector<T>& p) {
    for (auto& x : p) {
        is >> x;
    }
    return is;
}

template <class T>
ostream& operator<<(ostream& os, const vector<T>& p) {
    bool is_first = true;
    for (auto& x : p) {
        if (!is_first) {
            os << ' ';
        } else {
            is_first = false;
        }
        os << x;
    }
    os << endl;
    return os;
}

struct Solution : public vector<int> {
    using Base = vector<int>;

    explicit Solution(const Problem* p): p(p) {}

    explicit Solution(const Problem& p): Solution(&p) {}

    int safe_index(int index) const {
        index %= (int)size();
        if (index < 0) {
            index += (int)size();
        }
        return index;
    }

    int& operator[](int index) {
        return Base::operator[](safe_index(index));
    }

    const int& operator[](int index) const {
        return Base::operator[](safe_index(index));
    }

    Point point(int index) const {
        return (*p)[(*this)[index]];
    }

    void add(int index) {
        Base::push_back(index);
    }

    double length() const {
        double total_dist = 0;
        for (int i = 0; i < size(); ++i) {
            total_dist += distance(point(i), point(i + 1));
        }
        return total_dist;
    }

    void print() const {
        cout << setprecision(3) << fixed << "Score: " << length() << endl << *this;
    }

    partial_ordering operator<=>(const Solution& other) const {
        return length() <=> other.length();
    }

    bool operator==(const Solution& other) const = default;

    const Problem* p;
};

Solution greedy_fixed_starting(const Problem& p, int starting = 0) {
    Solution s(p);
    set<int> unused;
    ranges::copy(views::iota(0, (int)p.size()), inserter(unused, unused.end()));
    s.add(starting);
    unused.erase(starting);
    while (!unused.empty()) {
        int last = s.back();
        int closest = -1;
        double dist = 1e18;
        for (auto x : unused) {
            if (distance(p[last], p[x]) < dist) {
                dist = distance(p[last], p[x]);
                closest = x;
            }
        }
        if (dist == 0) {
            exit(1);
        }
        unused.erase(closest);
        s.add(closest);
    }
    return s;
}

Solution greedy(const Problem& p) {
    if (p.size() >= 2000) {
        return greedy_fixed_starting(p, 0);
    }
    return ranges::min(views::iota(0, (int)p.size()) | views::transform([&](int s) {return greedy_fixed_starting(p, s);}));
}

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


    double mst_len = 0;
    for (auto [u, v] : mst_edges) {
        mst_len += distance(p[u], p[v]);
    }
    double match_len = 0;
    for (auto [u, v] : match_edges) {
        match_len += distance(p[u], p[v]);
    }

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

template <class S>
requires (std::is_same_v<std::decay_t<S>, Solution>)
S&& local_search(S&& s, int permutation_length = 7) {
    const Problem& p = *s.p;
    int n = s.size();
    for (bool improved = true; improved;) {
        improved = false;
        for (int i = 0; i < n; ++i) {
            double current_length = distance(s.point(i - 1), s.point(i));
            vector<int> perm(permutation_length);
            for (int j = 0; j < permutation_length; ++j) {
                perm[j] = s[i + j];
                current_length += distance(s.point(i + j), s.point(i + j + 1));
            }
            ranges::sort(perm);
            do {
                double length_now = distance(s.point(i - 1), p[perm[0]]) + 
                                    distance(s.point(i + permutation_length), p[perm.back()]);
                for (int j = 1; j < permutation_length; ++j) {
                    length_now += distance(p[perm[j]], p[perm[j - 1]]);
                }
                if (length_now < current_length - 1e-3) {
                    improved = true;
                    current_length = length_now;
                    for (int j = 0; j < permutation_length; ++j) {
                        s[i + j] = perm[j];
                    }
                }
            } while (ranges::next_permutation(perm).found);
        }
    }
    return s;
}

int main() {
    int n;
    cin >> n;
    Problem p(n);
    cin >> p;
    local_search((n < 2000 ? christofides_serduykov : greedy)(p)).print();
}
