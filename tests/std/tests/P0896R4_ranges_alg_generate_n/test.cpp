// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::generate_n, ranges::equal;
    {
        int output[] = {13, 42, 1367};
        auto result  = generate_n(ranges::begin(output), ranges::distance(output), [calls_to_generate = -1]() mutable {
            ++calls_to_generate;
            return calls_to_generate;
        });
        for (int i = 0; i < 3; ++i) {
            assert(i == output[i]);
        }
        assert(result == ranges::end(output));
    }
    {
        int expected_output[] = {13, 42, 1367};
        int output[]          = {13, 42, 1367};
        auto result           = generate_n(ranges::begin(output), 0, [calls_to_generate = -1]() mutable {
            ++calls_to_generate;
            return calls_to_generate;
        });
        assert(ranges::equal(output, expected_output));
        assert(result == ranges::begin(output));
    }
    {
        int expected_output[] = {13, 42, 1367};
        int output[]          = {13, 42, 1367};
        auto result           = generate_n(ranges::begin(output), -1, [calls_to_generate = -1]() mutable {
            ++calls_to_generate;
            return calls_to_generate;
        });
        assert(ranges::equal(output, expected_output));
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
        (void) ranges::generate_n(ranges::begin(out), 13, []() { return 13; });
    }
};

template void test_out<instantiator>();
