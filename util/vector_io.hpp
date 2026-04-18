#pragma once

#include <bits/stdc++.h>

using namespace std;

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