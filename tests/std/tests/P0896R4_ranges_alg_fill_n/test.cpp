// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <range_algorithm_support.hpp>
#include <ranges>
#include <utility>

constexpr void smoke_test() {
    using ranges::fill_n, ranges::iterator_t;
    using std::same_as;

    int expected_output[] = {13, 42, 1367};
    const int value       = 7;
    {
        int output[] = {13, 42, 1367};
        auto result  = fill_n(ranges::begin(output), ranges::distance(output), value);
        for (const auto& elem : output) {
            assert(elem == value);
        }
        assert(result == ranges::end(output));
    }
    {
        int output[] = {13, 42, 1367};
        auto result  = fill_n(ranges::begin(output), 0, value);
        for (int i = 0; i < 3; ++i) {
            assert(output[i] == expected_output[i]);
        }
        assert(result == ranges::begin(output));
    }
    {
        int output[] = {13, 42, 1367};
        auto result  = fill_n(ranges::begin(output), -1, value);
        for (int i = 0; i < 3; ++i) {
            assert(output[i] == expected_output[i]);
        }
        assert(result == ranges::begin(output));
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class Out>
    static void call(Out&& out = {}) {
        (void) ranges::fill_n(ranges::begin(out), 13, 42);
    }
};

template void test_out<instantiator>();
