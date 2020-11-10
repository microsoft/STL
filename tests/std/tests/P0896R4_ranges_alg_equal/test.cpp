// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <cstdlib>
#include <ranges>

#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::equal, ranges::equal_to;
    using std::abort, std::array, std::pair, std::same_as;

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
        constexpr auto proj  = [](auto &&) -> int { abort(); };
        constexpr auto comp  = [](auto&&, auto &&) -> bool { abort(); };
        int const one_int[]  = {0};
        int const two_ints[] = {0, 1};
        assert(!equal(one_int, two_ints, comp, proj, proj));
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In1, class In2>
    static void call(In1&& in1 = {}, In2&& in2 = {}) {
        using ranges::begin, ranges::end, ranges::equal, ranges::iterator_t;

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
