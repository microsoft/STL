// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <range_algorithm_support.hpp>
#include <ranges>
#include <utility>

constexpr void smoke_test() {
    using ranges::generate_n, ranges::iterator_t;
    using std::same_as;

    int output[]    = {13, 42, 1367};
    const int value = 7;
    auto result     = generate_n(std::begin(output), ranges::distance(output), []() { return value; });
    for (auto elem : output) {
        assert(elem == value);
    }
    assert(result == std::end(output));
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class Out>
    static void call(Out&& out = {}) {
        (void) ranges::generate_n(ranges::begin(out), 13, []() { return 13; });
    }
};

template void test_out<instantiator>();
