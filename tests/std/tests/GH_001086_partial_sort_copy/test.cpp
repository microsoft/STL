// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// GH-1086: "std::partial_sort_copy performs an unconstrained operation"
// partial_sort_copy was constructing an object of the source range's value type
// from the result of dereferencing an iterator, which is not allowed by the
// specification of the algorithm.

#include <algorithm>
#include <cassert>
#include <iterator>

using namespace std;

struct wrapper {
    wrapper() = default;
    constexpr explicit wrapper(int i) : x{i} {}

    bool operator<(const wrapper& that) const {
        return x < that.x;
    }

    int x;
};

struct source : wrapper {
    source() = default;

    using wrapper::wrapper;

    source(const source&)            = delete;
    source& operator=(const source&) = delete;
};

struct target : wrapper {
    target() = default;

    using wrapper::wrapper;

    target(target&&)            = default;
    target& operator=(target&&) = default;

    target& operator=(const source& w) {
        x = w.x;
        return *this;
    }
};

int main() {
    constexpr int src_size = 4;
    source src[src_size];
    constexpr int dst_size = 2;
    target dst[dst_size];

    for (int i = 0; i < src_size; ++i) {
        src[i].x = src_size - 1 - i;
    }

    partial_sort_copy(begin(src), end(src), begin(dst), end(dst));

    for (int i = 0; i < dst_size; ++i) {
        assert(dst[i].x == i);
    }
}
