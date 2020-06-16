// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <range_algorithm_support.hpp>
#include <ranges>
#include <utility>

constexpr void smoke_test() {
    using ranges::generate, ranges::iterator_t;
    using std::same_as;

    int output1[] = {13, 42, 1367};
    int output2[] = {13, 42, 1367};
    {
        auto result = generate(std::begin(output1), std::end(output1), []() { return 7; });
        for (int i = 0; i < 3; ++i) {
            assert(output1[i] == 7);
        }
        assert(result == std::end(output1));
    }
    {
        auto result = generate(output2, []() { return 13; });
        for (int i = 0; i < 3; ++i) {
            assert(output2[i] == 13);
        }
        assert(result == std::end(output2));
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class Out>
    static void call(Out&& out = {}) {
        (void) ranges::generate(out, []() { return 13; });
        (void) ranges::generate(ranges::begin(out), ranges::end(out), []() { return 13; });
    }
};

template void test_out<instantiator>();
