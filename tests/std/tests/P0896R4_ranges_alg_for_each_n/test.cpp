// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;
using P = pair<int, int>;

constexpr auto incr = [](auto& y) { ++y; };

// Validate that for_each_n_result aliases in_fun_result
STATIC_ASSERT(same_as<ranges::for_each_n_result<int, double>, ranges::in_fun_result<int, double>>);

struct instantiator {
    static constexpr P expected[3] = {{1, 42}, {3, 42}, {5, 42}};

    template <indirectly_writable<P> ReadWrite>
    static constexpr void call() {
        using ranges::for_each_n, ranges::for_each_n_result, ranges::iterator_t, ranges::distance;
        P input[3] = {{0, 42}, {2, 42}, {4, 42}};

        auto result = for_each_n(ReadWrite{input}, distance(input), incr, get_first);
        STATIC_ASSERT(same_as<decltype(result), for_each_n_result<ReadWrite, remove_const_t<decltype(incr)>>>);
        assert(result.in.peek() == end(input));
        assert(ranges::equal(expected, input));

        int some_value = 1729;
        result.fun(some_value);
        assert(some_value == 1730);
    }
};

int main() {
    STATIC_ASSERT((test_read<instantiator, P>(), true));
    test_read<instantiator, P>();
}
