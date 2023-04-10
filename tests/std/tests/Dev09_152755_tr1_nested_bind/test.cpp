// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <vector>

using namespace std;
using namespace std::placeholders;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

class point {
public:
    point(double x, double y) : m_x(x), m_y(y) {}

    double mag() const {
        return sqrt(m_x * m_x + m_y * m_y);
    }

private:
    double m_x;
    double m_y;
};

int main() {
    vector<point> v;

    v.push_back(point(3, 4));
    v.push_back(point(5, 12));
    v.push_back(point(8, 15));

    assert(count_if(v.begin(), v.end(), bind(greater<double>(), bind(&point::mag, _1), 10)) == 2);
}
