// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

struct int_wrapper {
    int val                 = 10;
    constexpr int_wrapper() = default;
    constexpr int_wrapper(int x) : val{x} {}
    constexpr int_wrapper(int_wrapper&& that) : val{exchange(that.val, -1)} {}
    constexpr int_wrapper& operator=(int_wrapper&& that) {
        val = exchange(that.val, -1);
        return *this;
    }
    auto operator<=>(const int_wrapper&) const = default;
};

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::shift_left(borrowed<false>{}, 1)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::shift_left(borrowed<true>{}, 1)), ranges::subrange<int*>>);

struct instantiator {
    static constexpr int_wrapper expected_result[2] = {12345, 3333};
    static constexpr int_wrapper expected_io[5]     = {12345, 3333, 44, -1, -1};

    template <ranges::forward_range Rng>
    static constexpr void call() {
        if constexpr (permutable<ranges::iterator_t<Rng>>) {
            using ranges::equal, ranges::iterator_t, ranges::begin, ranges::end;
            {
                int_wrapper io[5] = {13, 55, 44, 12345, 3333};
                Rng range(io);

                auto result = ranges::shift_left(range, 3);
                STATIC_ASSERT(same_as<decltype(result), ranges::subrange<iterator_t<Rng>>>);
                assert(equal(result, expected_result));
                assert(equal(io, expected_io));
            }
            {
                int_wrapper io[5] = {13, 55, 44, 12345, 3333};
                Rng range(io);

                auto result = ranges::shift_left(begin(range), end(range), 3);
                STATIC_ASSERT(same_as<decltype(result), ranges::subrange<iterator_t<Rng>>>);
                assert(equal(begin(result), end(result), begin(expected_result), end(expected_result)));
                assert(equal(io, expected_io));
            }
        }
    }
};

int main() {
    STATIC_ASSERT((test_fwd<instantiator, int_wrapper>(), true));
    test_fwd<instantiator, int_wrapper>();
}
