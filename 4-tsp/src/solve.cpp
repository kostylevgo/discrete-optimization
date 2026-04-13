#include <bits/stdc++.h>
#include "max_general_matching.hpp"

#include "util/stopwatch.hpp"

using namespace std;
using namespace std::chrono_literals;

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
        if (ranges::find(*this, index) != end()) {
            exit(*end());
        }
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

Solution random(const Problem& p) {
    Solution s(p);
    mt19937 gen(1);
    vector<int> order(p.size());
    iota(order.begin(), order.end(), 0);
    ranges::shuffle(order, gen);
    for (auto x : order) s.add(x);
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

bool find_best_permutation(Solution& s, int i, int permutation_length) {
    const Problem& p = *s.p;
    bool improved = false;
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
    return improved;
}

void do_2opt(Solution& s, int i, int j) {
    reverse(s.begin() + min(i, j) + 1, s.begin() + max(i, j) + 1);
}

bool try_2opt(Solution& s, int i, int j) {
    Point a = s.point(i);
    Point b = s.point(i + 1);
    Point c = s.point(j);
    Point d = s.point(j + 1);
    if (distance(c, d) + distance(a, b) - 1e-3 > distance(c, a) + distance(d, b)) {
        do_2opt(s, i, j);
        return true;
    }
    return false;
}

bool try_swap(Solution& s, int i, int j) {
    if (distance(s.point(i - 1), s.point(i)) + distance(s.point(i), s.point(i + 1)) + 
        distance(s.point(j - 1), s.point(j)) + distance(s.point(j), s.point(j + 1))
        - 1e-3 > distance(s.point(i - 1), s.point(j)) + distance(s.point(j), s.point(i + 1)) + 
                 distance(s.point(j - 1), s.point(i)) + distance(s.point(i), s.point(j + 1))) {
        swap(s[i], s[j]);
        return true;
    }
    return false;
}

Solution& local_search(Solution& s, int permutation_length, bool do_2opts) {
    int n = s.size();
    for (bool improved = true; improved;) {
        improved = false;
        for (int i = 0; i < n; ++i) {
            improved |= find_best_permutation(s, i, permutation_length);
        }
        if (!improved && do_2opts) {
            for (int i = 0; i < n - 1; ++i) {
                for (int j = (i == n - 1); j <= i - 2; ++j) {
                    improved |= try_2opt(s, i, j);
                }
            }
            for (int i = 0; i < n - 1; ++i) {
                for (int j = (i == n - 1); j <= i - 2; ++j) {
                    improved |= try_swap(s, i, j);
                }
            }
        }
    }
    return s;
}

Solution& full_local_search(Solution& s) {
    return local_search(s, 7, true);
}

Solution& light_local_search(Solution& s) {
    return local_search(s, 5, false);
}

Solution& aggressive_mutation(Solution& s, double temperature = 0.1) {
    static int seed = 1;
    mt19937 gen(++seed);
    uniform_real_distribution<double> d(0, 1);
    
    vector<int> inds_to_shuffle;
    for (int i = 0; i < s.size(); ++i) {
        if (d(gen) < temperature) {
            inds_to_shuffle.push_back(i);
        }
    }

    vector<int> permutation = inds_to_shuffle;
    for (auto& x : permutation) {
        x = s[x];
    }
    ranges::shuffle(permutation, gen);

    for (int i = 0; i < inds_to_shuffle.size(); ++i) {
        s[inds_to_shuffle[i]] = permutation[i];
    }
    return s;
}

Solution& mutation(Solution& s, int c = 3) {
    static int seed = 1;
    mt19937 gen(++seed);

    int n = s.size();

    for (int it = 0; it < c; ++it) {
        int i = gen() % n;
        int j = i;
        while (abs(i - j) <= 1 || abs(i - j) == n - 1) {
            j = gen() % n;
        }
        do_2opt(s, i, j);
    }

    return s;
}

set<pair<int, int>> get_edges(const Solution& s) {
    set<pair<int, int>> result;
    for (int i = 0; i < s.size(); ++i) {
        int x = s[i];
        int y = s[i + 1];
        if (x > y) swap(x, y);
        result.emplace(x, y);
    }
    return result;
}

set<pair<int, int>> intersect(const set<pair<int, int>>& a, const set<pair<int, int>>& b) {
    set<pair<int, int>> result;
    for (auto t : a) {
        if (b.contains(t)) {
            result.emplace_hint(result.end(), t);
        }
    }
    return result;
}

const array<int pair<int, int>::*, 2> fields = {&pair<int, int>::first, &pair<int, int>::second};

int pair<int, int>::* get_other_ptr(int pair<int, int>::* ptr) {
    for (auto x : fields) {
        if (x != ptr) return x;
    }
    exit(1);
}

void path_dfs(int v, vector<bool>& used, Solution& path, const vector<vector<int>>& g) {
    path.add(v);
    used[v] = true;
    for (auto u : g[v]) {
        if (!used[u]) path_dfs(u, used, path, g);
    }
}

Solution path_from_edges(const Problem& p, auto&& edges) {
    Solution s(p);
    int n = p.size();
    vector<vector<int>> g(n);
    for (auto [u, v] : edges) {
        g[u].push_back(v);
        g[v].push_back(u);
    }
    vector<bool> used(n);
    path_dfs(0, used, s, g);
    return s;
}

pair<Solution, int> crossingover(const Solution& a, const Solution& b) {
    int n = a.size();
    set<pair<int, int>> common = intersect(get_edges(a), get_edges(b));
    int similarity = common.size();
    if (common.size() == n) {
        return {a, similarity};
    }
    vector<int> endpoint(n);
    iota(endpoint.begin(), endpoint.end(), 0);
    for (auto [u, v] : common) {
        int l = endpoint[u];
        int r = endpoint[v];
        endpoint[u] = -1;
        endpoint[v] = -1;
        endpoint[l] = r;
        endpoint[r] = l;
    }
    vector<pair<int, int>> endpoints;
    for (int i = 0; i < n; ++i) {
        if (endpoint[i] != -1 && endpoint[i] >= i) {
            endpoints.emplace_back(i, endpoint[i]);
        }
    }
    const Problem& p = *a.p;
    vector<bool> used(endpoints.size());
    pair<int, int> cur_endpoints = endpoints[0];
    used[0] = true;

    for (int i = 1; i < used.size(); ++i) {
        int best = -1;
        int pair<int, int>::* cur_ptr = nullptr;
        int pair<int, int>::* best_ptr = nullptr;
        double best_dist = 1e18;
        for (int j = 0; j < endpoints.size(); ++j) {
            if (used[j]) continue;
            for (auto c : fields) {
                for (auto o : fields) {
                    double dist = distance(p[cur_endpoints.*c], p[endpoints[j].*o]);
                    if (dist < best_dist) {
                        best_dist = dist;
                        best = j;
                        cur_ptr = c;
                        best_ptr = o;
                    }
                }
            }
        }
        int v = cur_endpoints.*cur_ptr;
        int u = endpoints[best].*best_ptr;
        common.emplace(v, u);
        cur_endpoints.*cur_ptr = endpoints[best].*get_other_ptr(best_ptr);
        used[best] = true;
    }

    common.emplace(cur_endpoints.first, cur_endpoints.second);
    if (common.size() != n) {
        exit(1);
    }

    return {path_from_edges(p, common), similarity};
}

struct NoOp {
    void operator()() {}
};

template <class C = NoOp>
Solution& kernighan_lin(Solution& s, C&& cb = NoOp()) {
    static int seed = 31;
    mt19937 gen(++seed);

    int n = s.size();

    for (bool improved = true; improved;) {
        cb();
        improved = false;

        vector<int> order(2 * n);
        iota(order.begin(), order.end(), 0);
        ranges::shuffle(order, gen);

        for (auto it : order) {
            set<pair<int, int>> x, y;
            int t_1 = s[it];
            int t_2 = s[it + 1];
            Solution cur = s;
            Solution best = s;

            x.emplace(t_1, t_2);
            ranges::rotate(cur, cur.begin() + (it + 1) % n);
            if (it >= n) ranges::reverse(cur);
            // cur[0] = t_2i

            double overall_gain = distance(cur.point(0), cur.point(-1));

            for (int it = 0; it < n; ++it) {
                priority_queue<pair<double, int>> smallest_y;

                for (int i = 2; i < n; ++i) {
                    if (x.contains({cur[0], cur[i]}) || x.contains({cur[i], cur[0]}) ||
                        y.contains({cur[i - 1], cur[i]}) || y.contains({cur[i], cur[i - 1]})) {
                        continue;
                    }
                    if (overall_gain - distance(cur.point(0), cur.point(i)) < 0) {
                        continue;
                    }
                    smallest_y.emplace(distance(cur.point(0), cur.point(i)), i);
                    if (smallest_y.size() > 5) {
                        smallest_y.pop();
                    }
                }

                int best_i = -1;
                double best_gain = -1e18;
                while (!smallest_y.empty()) {
                    auto [_, i] = smallest_y.top();
                    smallest_y.pop();
                    double gain = distance(cur.point(i - 1), cur.point(i)) - distance(cur.point(0), cur.point(i));
                    if (gain > best_gain) {
                        best_gain = gain;
                        best_i = i;
                    }
                }

                if (best_i == -1) break;

                overall_gain += best_gain;

                y.emplace(cur[0], cur[best_i]);
                x.emplace(cur[best_i - 1], cur[best_i]);
                ranges::reverse(cur.begin(), cur.begin() + best_i);
                if (cur < best) {
                    best = cur;
                }

                if (it > 2 * n + 10) {
                    exit(1);
                }
            }
            if (best.length() < s.length() - 1e-3) {
                s = best;
                improved = true;
                break;
            }
        }
    }

    return s;
}

Solution genetics(Solution starting) {
    int population_size = min((int)starting.size(), 2000);
    Stopwatch watch("genetics", starting.size() <= 2000 ? 1min : 5min);
    /* ticks:
     * 51_1:    40778
     * 100_3:   11888
     * 200_2:   2602
     * 574_1:   719
     * 1889_1:  72
     */
    mt19937 gen(31);
    try {

    double length = starting.length();

    auto update = [&](const Solution& s, double len) {
        if (len < length) {
            length = len;
            starting = s;
        }
    };

    vector<pair<Solution, double>> population;
    population.reserve(population_size);
    population.emplace_back(starting, starting.length());
    for (int i = 1; i < population_size; ++i) {
        Solution s = population.back().first;
        mutation(s);
        kernighan_lin(s);
        double len = s.length();
        update(s, len);
        population.emplace_back(std::move(s), len);
        watch();
    }

    while (true) {
        while (population.size() < 4 * population_size) {
            int i = gen() % population_size;
            int j = gen() % (population_size - 1);
            if (j >= i) ++j;
            auto [s, similarity] = crossingover(population[i].first, population[j].first);
            
            kernighan_lin(s);
            mutation(s, similarity >= (int)starting.size() * 0.9 ? 10 : 3);
            kernighan_lin(s);

            double l = s.length();
            update(s, l);
            population.emplace_back(std::move(s), l);

            watch();
        }
        ranges::sort(population, {}, &pair<Solution, double>::second);
        population.erase(population.begin() + population_size, population.end());
    }

    } catch (const Stopwatch::TimeoutException&) {
    }
    return starting;
}

int main() {
    int n;
    cin >> n;
    Problem p(n);
    cin >> p;
    Solution s = (n <= 2000 ? christofides_serduykov : greedy)(p);
    if (n <= 2000) {
        kernighan_lin(s);
        full_local_search(s);
        s = genetics(std::move(s));
    } else {
        Stopwatch watch("kernighan_lin", 20min);
        try {
            kernighan_lin(s, watch);
            // ticks: 2537
        } catch (const Stopwatch::TimeoutException&) {}
        full_local_search(s);
    }
    s.print();
}
