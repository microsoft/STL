// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <vector>

struct X {
    explicit X(const int n) : m_n(n) {}

    int m_n;
};

bool parity(const X& a, const X& b) {
    return a.m_n % 2 == b.m_n % 2;
}

int main() {
    const int arr[] = {1, 2, 3, 4, 6, 7, 9, 10, 12, 14, 15, 17, 19, 20};

    std::vector<X> v;

    for (const auto& e : arr) {
        v.emplace_back(e);
    }

    const X x(99);

    const std::vector<X>::const_iterator p = std::search_n(v.begin(), v.end(), 3, x, parity);

    assert(p != v.end() && p->m_n == 15);
}
