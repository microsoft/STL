// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>
#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::fill, ranges::iterator_t;
    using std::same_as;

    int output[] = {13, 42, 1367};
    {
        const int value = 7;
        auto result = fill(std::begin(output), std::end(output), value);
        for (int i = 0; i < 3; ++i) {
            assert(output[i] == 7);
        }
        assert(result == std::end(output));
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
