// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <span>

#include <range_algorithm_support.hpp>

using namespace std;

struct instantiator {
    static constexpr int expected[] = {13, 42, 1729};

    template <input_or_output_iterator Iter>
    static constexpr void call() {
        using ranges::contiguous_range, ranges::equal, ranges::iterator_t, ranges::random_access_range, ranges::size,
            ranges::subrange;
        using IterValue = remove_reference_t<iter_reference_t<Iter>>;
        int input[]     = {13, 42, 1729, -1, -1};

        auto result = ranges::views::counted(Iter{input}, 3);
        if constexpr (contiguous_iterator<Iter>) {
            STATIC_ASSERT(same_as<decltype(result), span<IterValue, dynamic_extent>>);
        } else if constexpr (random_access_iterator<Iter>) {
            STATIC_ASSERT(same_as<decltype(result), subrange<Iter, Iter>>);
        } else {
            STATIC_ASSERT(same_as<decltype(result), subrange<counted_iterator<Iter>, default_sentinel_t>>);
        }
        assert(size(result) == 3);
        if constexpr (input_iterator<Iter>) {
            assert(equal(result, expected));
        }
    }
};

int main() {
    STATIC_ASSERT((with_writable_iterators<instantiator, int>::call(), true));
    with_writable_iterators<instantiator, int>::call();
}
