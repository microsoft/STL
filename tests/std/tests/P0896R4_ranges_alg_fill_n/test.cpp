// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

struct instantiator {
    template <class Out>
    static constexpr void call() {
        using ranges::fill_n;

        const int expected_output[] = {13, 42, 1367};
        const int value             = 7;
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
            assert(ranges::equal(output, expected_output));
            assert(result == ranges::begin(output));
        }
        {
            int output[] = {13, 42, 1367};
            auto result  = fill_n(ranges::begin(output), -1, value);
            assert(ranges::equal(output, expected_output));
            assert(result == ranges::begin(output));
        }
        { // Validate int is properly converted to bool
            bool output[] = {false, true, false};
            fill_n(ranges::begin(output), ranges::distance(output), 5);
            for (const bool& elem : output) {
                assert(elem == true);
            }
        }
    }
};

int main() {
    STATIC_ASSERT((test_out<instantiator, int>(), true));
    test_out<instantiator, int>();
}
