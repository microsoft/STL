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
STATIC_ASSERT(same_as<decltype(ranges::shift_right(borrowed<false>{}, 1)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::shift_right(borrowed<true>{}, 1)), ranges::subrange<int*>>);

struct instantiator {
    template <ranges::forward_range Rng>
    static constexpr void call() {
        if constexpr (permutable<ranges::iterator_t<Rng>>) {
            using ranges::equal, ranges::iterator_t, ranges::begin, ranges::end;
            {
                int_wrapper io[5] = {13, 55, 44, 12345, 3333};
                Rng range(io);

                auto result = ranges::shift_right(range, 2);
                STATIC_ASSERT(same_as<decltype(result), ranges::subrange<iterator_t<Rng>>>);

                int_wrapper expected_result[3] = {13, 55, 44};
                int_wrapper expected_io[5]     = {-1, -1, 13, 55, 44};
                assert(equal(result, expected_result));
                assert(equal(io, expected_io));
            }
            {
                int_wrapper io[5] = {13, 55, 44, 12345, 3333};
                Rng range(io);

                auto result = ranges::shift_right(begin(range), end(range), 2);
                STATIC_ASSERT(same_as<decltype(result), ranges::subrange<iterator_t<Rng>>>);

                int_wrapper expected_result[3] = {13, 55, 44};
                int_wrapper expected_io[5]     = {-1, -1, 13, 55, 44};
                assert(equal(result, expected_result));
                assert(equal(io, expected_io));
            }
            {
                int_wrapper io[5] = {13, 55, 44, 12345, 3333};
                Rng range(io);

                auto result = ranges::shift_right(begin(range), end(range), 5);

                int_wrapper expected_io[5] = {13, 55, 44, 12345, 3333};
                assert(equal(result, ranges::empty_view<int_wrapper>{}));
                assert(equal(io, expected_io));
            }
            {
                int_wrapper io[7] = {13, 55, 44, 12345, 3333, 5555, 7777};
                Rng range(io);

                auto result = ranges::shift_right(begin(range), end(range), 2);

                int_wrapper expected_result[5] = {13, 55, 44, 12345, 3333};
                assert(equal(result, expected_result));
                assert(equal(begin(io) + 2, end(io), begin(expected_result), end(expected_result)));
            }
        }
    }
};

int main() {
    STATIC_ASSERT((test_fwd<instantiator, int_wrapper>(), true));
    test_fwd<instantiator, int_wrapper>();
}
