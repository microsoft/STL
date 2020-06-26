// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <range_algorithm_support.hpp>
#include <ranges>
#include <utility>

constexpr void smoke_test() {
    using ranges::fill, ranges::iterator_t;

    {
        int output[]    = {13, 42, 1367};
        const int value = 7;
        auto result     = fill(ranges::begin(output), ranges::end(output), value);
        for (const auto& elem : output) {
            assert(elem == value);
        }
        assert(result == ranges::end(output));
    }
    {
        int output[]    = {13, 42, 1367};
        const int value = 13;
        auto result     = fill(output, value);
        for (const auto& elem : output) {
            assert(elem == value);
        }
        assert(result == ranges::end(output));
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class Out>
    static void call(Out&& out = {}) {
        (void) ranges::fill(out, 42);
        (void) ranges::fill(ranges::begin(out), ranges::end(out), 42);
    }
};

template void test_out<instantiator>();
