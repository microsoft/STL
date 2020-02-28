// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>
//
#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::copy_n, ranges::copy_n_result, ranges::iterator_t;
    using std::same_as;

    // Validate that copy_n_result aliases in_out_result
    STATIC_ASSERT(same_as<copy_n_result<int, double>, ranges::in_out_result<int, double>>);

    int const input[] = {13, 42, 1729};
    int output[]      = {-1, -1, -1};
    move_only_range x{input};
    auto result = copy_n(x.begin(), ranges::distance(input), move_only_range{output}.begin());
    STATIC_ASSERT(same_as<decltype(result),
        copy_n_result<iterator_t<move_only_range<int const>>, iterator_t<move_only_range<int>>>>);
    assert(result.in == x.end());
    assert(result.out == move_only_range{output}.end());
    assert(ranges::equal(output, input));
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In, class, class Out>
    static void call(In in = {}, std::iter_difference_t<In> const count = 42, Out out = {}) {
        (void) ranges::copy_n(std::move(in), count, std::move(out));
    }
};

template void test_counted_out<instantiator>();
