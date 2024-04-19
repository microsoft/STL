// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <functional>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;
using P = pair<int, int>;

constexpr auto incr = [](auto& y) { ++y; };

// Validate that for_each_result aliases in_fun_result
static_assert(same_as<ranges::for_each_result<int, double>, ranges::in_fun_result<int, double>>);

// Validate dangling story
static_assert(same_as<decltype(ranges::for_each(borrowed<false>{}, identity{})),
    ranges::for_each_result<ranges::dangling, identity>>);
static_assert(
    same_as<decltype(ranges::for_each(borrowed<true>{}, identity{})), ranges::for_each_result<int*, identity>>);

struct instantiator {
    static constexpr P expected[3] = {{1, 42}, {3, 42}, {5, 42}};

    template <ranges::input_range ReadWrite>
    static constexpr void call() {
        using ranges::for_each, ranges::for_each_result, ranges::iterator_t;

        { // Validate iterator + sentinel overload
            P input[3] = {{0, 42}, {2, 42}, {4, 42}};
            ReadWrite wrapped_input{input};

            auto result = for_each(wrapped_input.begin(), wrapped_input.end(), incr, get_first);
            static_assert(
                same_as<decltype(result), for_each_result<iterator_t<ReadWrite>, remove_const_t<decltype(incr)>>>);
            assert(result.in == wrapped_input.end());
            assert(ranges::equal(expected, input));

            int some_value = 1729;
            result.fun(some_value);
            assert(some_value == 1730);
        }
        { // Validate range overload
            P input[3] = {{0, 42}, {2, 42}, {4, 42}};
            ReadWrite wrapped_input{input};

            auto result = for_each(wrapped_input, incr, get_first);
            static_assert(
                same_as<decltype(result), for_each_result<iterator_t<ReadWrite>, remove_const_t<decltype(incr)>>>);
            assert(result.in == wrapped_input.end());
            assert(ranges::equal(expected, input));

            int some_value = 1729;
            result.fun(some_value);
            assert(some_value == 1730);
        }
    }
};

int main() {
    static_assert((test_in<instantiator, P>(), true));
    test_in<instantiator, P>();
}
