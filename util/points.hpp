#pragma once

#include <bits/stdc++.h>

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
