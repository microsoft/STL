// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <cstdlib>
#include <ranges>
#include <span>

#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::equal, ranges::equal_to, ranges::begin, ranges::end;
    using std::abort, std::array, std::pair, std::same_as, std::unreachable_sentinel;

    array<pair<int, int>, 3> const x   = {{{0, 42}, {2, 42}, {4, 42}}};
    array<pair<long, long>, 3> const y = {{{13, -1}, {13, 1}, {13, 3}}};

    constexpr auto cmp = [](auto&& x, auto&& y) { return x == y + 1; };

    {
        // Validate sized ranges
        auto result = equal(x, y, cmp, get_first, get_second);
        STATIC_ASSERT(same_as<decltype(result), bool>);
        assert(result);
        assert(!equal(x, y, cmp, get_first, get_first));
    }
    {
        // Validate non-sized ranges
        auto result = equal(basic_borrowed_range{x}, basic_borrowed_range{y}, cmp, get_first, get_second);
        STATIC_ASSERT(same_as<decltype(result), bool>);
        assert(result);
        assert(!equal(basic_borrowed_range{x}, basic_borrowed_range{y}, cmp, get_first, get_first));
    }
    {
        // Validate sized iterator + sentinel pairs
        auto result = equal(x.begin(), x.end(), y.begin(), y.end(), cmp, get_first, get_second);
        STATIC_ASSERT(same_as<decltype(result), bool>);
        assert(result);
        assert(!equal(x.begin(), x.end(), y.begin(), y.end(), cmp, get_first, get_first));
    }
    {
        // Validate non-sized iterator + sentinel pairs
        basic_borrowed_range wrapped_x{x};
        basic_borrowed_range wrapped_y{y};
        auto result =
            equal(wrapped_x.begin(), wrapped_x.end(), wrapped_y.begin(), wrapped_y.end(), cmp, get_first, get_second);
        STATIC_ASSERT(same_as<decltype(result), bool>);
        assert(result);
    }
    {
        basic_borrowed_range wrapped_x{x};
        basic_borrowed_range wrapped_y{y};
        assert(
            !equal(wrapped_x.begin(), wrapped_x.end(), wrapped_y.begin(), wrapped_y.end(), cmp, get_first, get_first));
    }
    {
        // calls with sized ranges of differing size perform no comparisons nor projections
        constexpr auto proj  = [](auto&&) -> int { abort(); };
        constexpr auto comp  = [](auto&&, auto&&) -> bool { abort(); };
        int const one_int[]  = {0};
        int const two_ints[] = {0, 1};
        assert(!equal(one_int, two_ints, comp, proj, proj));
    }
    {
        // Validate memcmp case
        int arr1[3]{0, 2, 5};
        int arr2[3]{0, 2, 5};
        assert(equal(arr1, arr2));
        arr2[1] = 7;
        assert(!equal(arr1, arr2));
    }
    {
        // Validate unreachable_sentinel cases
        int arr1[3]{0, 2, 5};
        int arr2[3]{0, 2, 5};
        assert(!equal(begin(arr1), unreachable_sentinel, begin(arr2), end(arr2)));
        assert(!equal(begin(arr1), end(arr1), begin(arr2), unreachable_sentinel));
    }
#ifndef _M_CEE // TRANSITION, VSO-1666180
    {
        // Validate GH-3550: "<ranges>: ranges::equal does not work for ranges with integer-class range_difference_t"
        auto v = ranges::subrange{std::views::iota(0ull, 10ull)} | std::views::drop(2);
        assert(equal(v, v));
    }
#endif // _M_CEE
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In1, class In2>
    static void call() {
        using ranges::begin, ranges::end, ranges::equal, ranges::iterator_t;

        In1 in1{std::span<const int, 0>{}};
        In2 in2{std::span<const int, 0>{}};

        if constexpr (!is_permissive) {
            (void) equal(in1, in2);
            (void) equal(begin(in1), end(in1), begin(in2), end(in2));
        }

        BinaryPredicateFor<iterator_t<In1>, iterator_t<In2>> pred{};
        (void) equal(in1, in2, pred);
        (void) equal(begin(in1), end(in1), begin(in2), end(in2), pred);

        HalfProjectedBinaryPredicateFor<iterator_t<In2>> halfpred{};
        ProjectionFor<iterator_t<In1>> halfproj{};
        (void) equal(in1, in2, halfpred, halfproj);
        (void) equal(begin(in1), end(in1), begin(in2), end(in2), halfpred, halfproj);

        ProjectedBinaryPredicate<0, 1> projpred{};
        ProjectionFor<iterator_t<In1>, 0> proj1{};
        ProjectionFor<iterator_t<In2>, 1> proj2{};
        (void) equal(in1, in2, projpred, proj1, proj2);
        (void) equal(begin(in1), end(in1), begin(in2), end(in2), projpred, proj1, proj2);
    }
};

#ifndef _PREFAST_ // TRANSITION, GH-1030
template void test_in_in<instantiator, const int, const int>();
#endif // TRANSITION, GH-1030
