// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <ranges>
#include <span>

#include <range_algorithm_support.hpp>

using namespace std;

// clang-format off
template <class I>
concept Countable = requires { typename iter_difference_t<remove_cvref_t<I>>; }
    && requires(I&& i, iter_difference_t<remove_cvref_t<I>> n) {
        views::counted(forward<I>(i), n);
    };
// clang-format on

template <input_or_output_iterator Iter>
struct convertible_difference {
    constexpr convertible_difference(const int _val_) noexcept : _val(_val_) {}
    constexpr operator iter_difference_t<Iter>() const noexcept {
        return iter_difference_t<Iter>{_val};
    }
    convertible_difference(convertible_difference&&)            = delete;
    convertible_difference& operator=(convertible_difference&&) = delete;

    int _val = 0;
};

struct instantiator {
    static constexpr int expected[] = {13, 42, 1729};

    template <input_or_output_iterator Iter>
    static constexpr void call() {
        using ranges::contiguous_range, ranges::equal, ranges::iterator_t, ranges::random_access_range, ranges::size,
            ranges::subrange;
        int input[] = {13, 42, 1729, -1, -1};

        STATIC_ASSERT(Countable<Iter>);
        STATIC_ASSERT(Countable<const Iter&> == copy_constructible<Iter>);

        auto result = ranges::views::counted(Iter{input}, convertible_difference<Iter>{3});
        if constexpr (contiguous_iterator<Iter>) {
            STATIC_ASSERT(same_as<decltype(result), span<remove_reference_t<iter_reference_t<Iter>>, dynamic_extent>>);
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
    STATIC_ASSERT(with_writable_iterators<instantiator, int>::call());
    with_writable_iterators<instantiator, int>::call();
}
