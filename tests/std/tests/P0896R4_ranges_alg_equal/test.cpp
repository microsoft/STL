// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <cstdlib>
#include <ranges>
//
#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::equal, ranges::equal_to;
    using std::abort, std::array, std::pair, std::same_as;

    array<pair<int, int>, 3> const x   = {{{0, 42}, {2, 42}, {4, 42}}};
    array<pair<long, long>, 3> const y = {{{13, 0}, {13, 2}, {13, 4}}};

    {
        // Validate sized ranges
        auto result = equal(x, y, equal_to{}, get_first, get_second);
        STATIC_ASSERT(same_as<decltype(result), bool>);
        assert(result);
        assert(!equal(x, y, equal_to{}, get_first, get_first));
    }
    {
        // Validate non-sized ranges
        auto result = equal(move_only_range{x}, move_only_range{y}, equal_to{}, get_first, get_second);
        STATIC_ASSERT(same_as<decltype(result), bool>);
        assert(result);
        assert(!equal(move_only_range{x}, move_only_range{y}, equal_to{}, get_first, get_first));
    }
    {
        // Validate sized iterator+sentinel pairs
        auto result = equal(x.begin(), x.end(), y.begin(), y.end(), equal_to{}, get_first, get_second);
        STATIC_ASSERT(same_as<decltype(result), bool>);
        assert(result);
        assert(!equal(x.begin(), x.end(), y.begin(), y.end(), equal_to{}, get_first, get_first));
    }
    {
        // Validate non-sized iterator+sentinel pairs
        move_only_range xx{x};
        move_only_range yy{y};
        auto result = equal(xx.begin(), xx.end(), yy.begin(), yy.end(), equal_to{}, get_first, get_second);
        STATIC_ASSERT(same_as<decltype(result), bool>);
        assert(result);
        xx = move_only_range{x};
        yy = move_only_range{y};
        assert(!equal(xx.begin(), xx.end(), yy.begin(), yy.end(), equal_to{}, get_first, get_first));
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

template void test_in_in<instantiator>();
