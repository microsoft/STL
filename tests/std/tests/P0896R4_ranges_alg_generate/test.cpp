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
        const int value = 7;
        auto result     = generate(std::begin(output1), std::end(output1), []() { return value; });
        for (auto elem : output1) {
            assert(elem == value);
        }
        assert(result == std::end(output1));
    }
    {
        const int value = 13;
        auto result     = generate(output2, []() { return value; });
        for (auto elem : output2) {
            assert(elem == value);
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
        (void) ranges::generate(out, []() { return 42; });
        (void) ranges::generate(ranges::begin(out), ranges::end(out), []() { return 42; });
    }
};

template void test_out<instantiator>();
