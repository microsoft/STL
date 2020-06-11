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
        auto result = fill(output.begin(), output.end(), value);
        for (int i = 0; i < 3; ++i) {
            assert(output[i] == 7);
        }
        assert(result == output.end());
    }
}

int main() {
    // STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In>
    static void call(In&& in = {}) {
        (void) ranges::fill(in, SOMETHING);
        (void) ranges::fill(ranges::begin(in), ranges::end(in), SOMETHING);
    }
};

template void test_in_out<instantiator>();
