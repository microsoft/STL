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

    int output1[] = {13, 42, 1367};
    {
        const int value = 7;
        auto result     = fill_n(std::begin(output1), ranges::distance(output1), value);
        for (int i = 0; i < 3; ++i) {
            assert(output1[i] == 7);
        }
        assert(result == std::end(output1));
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class Out>
    static void call(Out&& out = {}) {
        (void) ranges::fill_n(out, 13, 42);
    }
};

template void test_out<instantiator>();
