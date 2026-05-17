#pragma once

#include "problem.hpp"

#include <bits/stdc++.h>

using namespace std;

bool local_optimize(Solution& s) {
    for (int i = 0; i < s.car_count(); ++i) {
        for (int i_pos = 0; i_pos < s.car(i).size(); ++i_pos) {
            for (int j = 0; j < s.car_count(); ++j) {
                if (j == i) continue;

                Car new_j_car = s.car(j);
                new_j_car.add(s.car(i).route()[i_pos]);
                if (!new_j_car.valid()) continue;

                Car new_i_car = s.car(i);
                new_i_car.erase(i_pos);

                new_i_car.optimize();
                new_j_car.optimize();
                if (new_i_car.length() + new_j_car.length() < s.car(i).length() + s.car(j).length() - 1e-3) {
                    Solution s2 = s;
                    if (i > j) {
                        s2.erase(i);
                        s2.erase(j);
                    } else {
                        s2.erase(j);
                        s2.erase(i);
                    }
                    s2.add(new_i_car);
                    s2.add(new_j_car);
                    s = s2;
                    return true;
                }
            }
        }
    }
    for (int i = 0; i < s.car_count(); ++i) {
        for (int i_pos = 0; i_pos < s.car(i).size(); ++i_pos) {
            for (int j = 0; j < i; ++j) {
                for (int j_pos = 0; j_pos < s.car(j).size(); ++j_pos) {
                    Car new_i_car = s.car(i);
                    new_i_car.erase(i_pos);
                    new_i_car.add(s.car(j).route()[j_pos]);
                    if (!new_i_car.valid()) continue;

                    Car new_j_car = s.car(j);
                    new_j_car.erase(j_pos);
                    new_j_car.add(s.car(i).route()[i_pos]);
                    if (!new_j_car.valid()) continue;

                    new_i_car.optimize();
                    new_j_car.optimize();
                    if (new_i_car.length() + new_j_car.length() < s.car(i).length() + s.car(j).length() - 1e-3) {
                        Solution s2 = s;
                        s2.erase(i);
                        s2.erase(j);
                        s2.add(new_i_car);
                        s2.add(new_j_car);
                        s = s2;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
