// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <ranges>
#include <span>

#include <range_algorithm_support.hpp>

using namespace std;

template <class I>
concept Countable = requires { typename iter_difference_t<remove_cvref_t<I>>; }
                 && requires(I&& i, iter_difference_t<remove_cvref_t<I>> n) { views::counted(forward<I>(i), n); };

template <class I>
concept CanConstructCountedSubrange = requires { typename iter_difference_t<remove_cvref_t<I>>; }
                                   && requires(I&& i, iter_difference_t<remove_cvref_t<I>> n) {
                                          ranges::subrange(counted_iterator(forward<I>(i), n), default_sentinel);
                                      };

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
        using ranges::equal, ranges::size, ranges::subrange;
        int input[] = {13, 42, 1729, -1, -1};

        static_assert(Countable<Iter>);
        static_assert(Countable<const Iter&> == copy_constructible<Iter>);

        auto result = ranges::views::counted(Iter{input}, convertible_difference<Iter>{3});
        if constexpr (contiguous_iterator<Iter>) {
            static_assert(same_as<decltype(result), span<remove_reference_t<iter_reference_t<Iter>>, dynamic_extent>>);

            const test::redifference_iterator<_Signed128, Iter> rediff_it{Iter{input}};
            ranges::contiguous_range auto rediff_result = ranges::views::counted(rediff_it, _Signed128{4});
            assert(size(rediff_result) == 4);
        } else if constexpr (random_access_iterator<Iter>) {
            static_assert(same_as<decltype(result), subrange<Iter, Iter>>);
        } else {
            static_assert(same_as<decltype(result), subrange<counted_iterator<Iter>, default_sentinel_t>>);
        }
        assert(size(result) == 3);
        if constexpr (input_iterator<Iter>) {
            assert(equal(result, expected));
        }
    }
};

// Also test GH-5183: "<ranges>: views::counted::_Choose() misses difference casting for contiguous_iterator case"
struct ExplicitCopyCtorIter {
    using difference_type = int;

    ExplicitCopyCtorIter()                                     = default;
    ExplicitCopyCtorIter(ExplicitCopyCtorIter&&)               = default;
    ExplicitCopyCtorIter& operator=(ExplicitCopyCtorIter&&)    = default;
    explicit ExplicitCopyCtorIter(const ExplicitCopyCtorIter&) = default;
    int operator*();
    ExplicitCopyCtorIter& operator++();
    void operator++(int);
};
static_assert(!Countable<ExplicitCopyCtorIter&>);
static_assert(!CanConstructCountedSubrange<ExplicitCopyCtorIter&>);

struct ImplicitCopyOnlyIter {
    using difference_type = int;

    ImplicitCopyOnlyIter()                                     = default;
    ImplicitCopyOnlyIter(ImplicitCopyOnlyIter&&)               = default;
    ImplicitCopyOnlyIter& operator=(ImplicitCopyOnlyIter&&)    = default;
    explicit ImplicitCopyOnlyIter(const ImplicitCopyOnlyIter&) = delete;
    template <int = 0>
    ImplicitCopyOnlyIter(const ImplicitCopyOnlyIter&);
    int operator*();
    ImplicitCopyOnlyIter& operator++();
    void operator++(int);
};
static_assert(Countable<ImplicitCopyOnlyIter&>);
static_assert(CanConstructCountedSubrange<ImplicitCopyOnlyIter&>);

int main() {
    static_assert(with_writable_iterators<instantiator, int>::call());
    with_writable_iterators<instantiator, int>::call();
}
