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
    static constexpr int input[]    = {13, 42, 1729, -1, -1};
    static constexpr int expected[] = {13, 42, 1729};

    template <ranges::input_range Read>
    static constexpr void call() {
        using ranges::contiguous_range, ranges::equal, ranges::iterator_t, ranges::random_access_range, ranges::size,
            ranges::subrange;
        using ReadIt    = iterator_t<Read>;
        using ReadValue = remove_reference_t<iter_reference_t<ReadIt>>;
        Read wrapped_input{input};

        auto result = ranges::views::counted(wrapped_input.begin(), 3);
        if constexpr (contiguous_range<Read>) {
            STATIC_ASSERT(same_as<decltype(result), span<ReadValue, dynamic_extent>>);
        } else if constexpr (random_access_range<Read>) {
            STATIC_ASSERT(same_as<decltype(result), subrange<ReadIt, ReadIt>>);
        } else {
            STATIC_ASSERT(same_as<decltype(result), subrange<counted_iterator<ReadIt>, default_sentinel_t>>);
        }
        assert(size(result) == 3);
        assert(equal(result, expected));
    }
};

int main() {
    STATIC_ASSERT((test_in<instantiator, const int>(), true));
    test_in<instantiator, const int>();
}
