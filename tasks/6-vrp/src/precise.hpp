#pragma once

#include "problem.hpp"

#include <bits/stdc++.h>

using namespace std;

void sos_dp(int bits, vector<int>& dp) {
    for (int i = 0; i < bits; ++i) {
        for (int mask = 0; mask < dp.size(); ++mask) {
            if (mask >> i & 1) {
                dp[mask] += dp[mask ^ (1 << i)];
            }
        }
    }
}

template <typename T, typename U>
void update(T& a, U b) {
    a = min<T>(a, b);
}

optional<Solution> solve_precise(const Problem& p) {
    int n = p.customers.size();

    vector<int> dp_sum_weights(1 << n, 0);
    for (int i = 0; i < n; ++i) {
        dp_sum_weights[1 << i] = p.customers[i].demand;
    }
    sos_dp(n, dp_sum_weights);

    vector dp_hampath(n, vector<float>(1 << n, 1e9));
    for (int i = 0; i < n; ++i) {
        dp_hampath[i][1 << i] = distance(p.origin, p.customers[i].location);
    }
    for (int mask = 0; mask < 1 << n; ++mask) {
        if (dp_sum_weights[mask] > p.car_capacity) continue;
        for (int last = 0; last < n; ++last) {
            if (~mask >> last & 1) continue;
            for (int next = 0; next < n; ++next) {
                if (mask >> next & 1) continue;
                update(dp_hampath[next][mask ^ (1 << next)],
                       dp_hampath[last][mask] + distance(p.customers[last].location, p.customers[next].location));
            }
            dp_hampath[last][mask] += distance(p.origin, p.customers[last].location);
        }
    }

    auto get_shortest_path = [&](int mask) {
        float min_dist = 1e9;
        for (int last = 0; last < n; ++last) {
            if (mask >> last & 1) {
                update(min_dist, dp_hampath[last][mask]);
            }
        }
        return min_dist;
    };

    pair<float, int> answer{1e9, -1};
    vector dp_vrp(p.cars - 1, vector(1 << n, answer));
    int cur_mx = 0;
    for (int mask = 0; mask < 1 << n; ++mask) {
        if (dp_sum_weights[mask] > p.car_capacity
            // || dp_sum_weights[mask] < p.car_capacity * 0.9
        ) continue;
        cur_mx |= mask;
        float min_dist = get_shortest_path(mask);

        update(dp_vrp[0][mask], pair{min_dist, mask});
        for (int i = 0; i < p.cars - 2; ++i) {
            for (int other_mask = mask; other_mask < cur_mx; ++other_mask, other_mask |= mask) {
                float w = min_dist + dp_vrp[i][other_mask ^ mask].first;
                update(dp_vrp[i + 1][other_mask], pair{w, mask});
            }
        }
        update(answer, pair{min_dist + dp_vrp[p.cars - 2][mask ^ ((1 << n) - 1)].first, mask});
    }

    if (answer.second == -1) {
        return nullopt;
    }

    for (int mask = 0; mask < 1 << n; ++mask) {
        for (int last = 0; last < n; ++last) {
            if (mask >> last & 1) {
                dp_hampath[last][mask] -= distance(p.origin, p.customers[last].location);
            }
        }
    }

    int total = (1 << n) - 1;
    Solution ans(p);
    for (int it = 0; it < p.cars; ++it) {
        Car car(p);
        int mask = answer.second;
        total ^= mask;

        int last = -1;
        float shortest = 1e9;
        for (int i = 0; i < n; ++i) {
            if (~mask >> i & 1) continue;
            float dist = dp_hampath[i][mask] + distance(p.origin, p.customers[i].location);
            if (dist < shortest) {
                shortest = dist;
                last = i;
            }
        }
        while (mask != (1 << last)) {
            car.add(last);
            for (int new_last = 0; new_last < n; ++new_last) {
                if (new_last != last && (mask >> new_last & 1) &&
                        abs(dp_hampath[last][mask] - dp_hampath[new_last][mask ^ (1 << last)] - distance(p.customers[last].location, p.customers[new_last].location)) < 1e-3) {
                    mask ^= (1 << last);
                    last = new_last;
                    break;
                }
            }
        }
        car.add(last);

        if (it != p.cars - 1) {
            answer = dp_vrp[p.cars - 2 - it][total];
        }
        ans.add(car);
    }
    return ans;
}
