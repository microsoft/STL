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
        using ranges::fill;
        {
            int output[]    = {13, 42, 1367};
            const int value = 7;
            auto result     = fill(ranges::begin(output), ranges::end(output), value);
            for (const auto& elem : output) {
                assert(elem == value);
            }
            assert(result == ranges::end(output));
        }
        { // Validate ranges overload
            int output[]    = {13, 42, 1367};
            const int value = 13;
            auto result     = fill(output, value);
            for (const auto& elem : output) {
                assert(elem == value);
            }
            assert(result == ranges::end(output));
        }
        { // Validate int is properly converted to bool
            bool output[] = {false, true, false};
            fill(output, 5);
            for (const bool& elem : output) {
                assert(elem == true);
            }
        }
        { // Validate zero-ing
            int output[] = {13, 42, 1367};
            auto result  = fill(output, 0);
            for (const auto& elem : output) {
                assert(elem == 0);
            }
            assert(result == ranges::end(output));
        }
    }
};

int main() {
    static_assert((test_out<instantiator, int>(), true));
    test_out<instantiator, int>();
}
