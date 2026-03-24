#include <bits/stdc++.h>

using namespace std;

int find_mex(vector<int>&& v) {
    ranges::sort(v);
    auto [le, ri] = ranges::unique(v);
    v.erase(le, ri);
    int i = 0;
    for (; i < v.size() && v[i] == i; ++i) {}
    return i;
}

const int kNoColor = -1;

int main() {
    int n, m;
    cin >> n >> m;
    vector<vector<int>> gr(n);
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        gr[u].push_back(v);
        gr[v].push_back(u);
    }
    vector<pair<int, int>> degrees(from_range, gr
        | views::transform([](auto& x) {return (int)x.size();})
        | views::enumerate);
    
    ranges::sort(degrees, greater<>(), &pair<int, int>::second);

    vector<int> best_color(from_range, views::iota(0, n));
    int best_color_count = n;

    vector<int> color, colors;
    mt19937 rnd(31);

    for (int it = 0; it < 1e8 / (n + m); ++it) {
        color.assign(n, kNoColor);
        for (auto v : degrees | views::keys) {
            colors.clear();
            
            ranges::copy(gr[v]
                    | views::transform([&](auto u) {return color[u];})
                    | views::filter([](auto col) {return col != kNoColor;}),
                back_inserter(colors));

            color[v] = find_mex(std::move(colors));
        }
        int answer = *ranges::max_element(color) + 1;
        if (answer < best_color_count) {
            best_color = color;
            best_color_count = answer;
        }
        ranges::shuffle(degrees, rnd);
    }

    cout << "Answer: " << best_color_count << endl;
    for (auto col : best_color) {
        cout << col << ' ';
    }
    cout << endl;
}