// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <compare>
#include <concepts>
#include <ranges>
#include <tuple>
#include <utility>

using namespace std;
using ranges::subrange;

struct Incomparable {};

struct NeverCompare {
    template <int = 0>
    constexpr bool operator==(const NeverCompare&) const {
        assert(false);
        return true;
    }

    template <int = 0>
    constexpr auto operator<=>(const NeverCompare&) const {
        assert(false);
        return strong_ordering::equal;
    }
};

template <class T, class U>
concept verify_comparisons = equality_comparable_with<T, U> && three_way_comparable_with<T, U>;

constexpr bool test() {
    { // Check comparisons of empty tuple-like types
        static_assert(verify_comparisons<tuple<>, array<int, 0>>);
        static_assert(!verify_comparisons<tuple<>, array<Incomparable, 0>>);

        static_assert(tuple{} == array<int, 0>{});

        constexpr same_as<strong_ordering> auto cmp = (tuple{} <=> array<int, 0>{});
        static_assert(is_eq(cmp));
    }

    { // Check comparisons of (tuple_size_v == 1) tuple-like types
        static_assert(verify_comparisons<tuple<int>, array<int, 1>>);
        static_assert(!verify_comparisons<tuple<int>, array<Incomparable, 1>>);
        static_assert(tuple{5} == array{5});
        static_assert(tuple{6} != array{7});
        static_assert(is_eq(tuple{1} <=> array{1}));
        static_assert(is_lt(tuple{1} <=> array{2}));
        static_assert(is_gt(tuple{3} <=> array{2}));
    }

    { // Check comparisons of (tuple_size_v == 2) tuple-like types
        static_assert(verify_comparisons<tuple<int, int>, array<int, 2>>);
        static_assert(!verify_comparisons<tuple<int, int>, array<Incomparable, 2>>);
        static_assert(!verify_comparisons<tuple<Incomparable, Incomparable>, array<Incomparable, 2>>);
        static_assert(verify_comparisons<tuple<int, int>, pair<int, int>>);
        static_assert(!verify_comparisons<tuple<int, int>, pair<int, Incomparable>>);
        static_assert(!verify_comparisons<tuple<int*, int*>,
                      subrange<int*, int*>>); // subrange does not model equality_comparable

        static_assert(tuple{1, 2} == array<int, 2>{1, 2});
        static_assert(tuple<int, int>{2, 1} != array<int, 2>{1, 2});
        static_assert(is_eq(tuple{1, 2} <=> array{1, 2}));
        static_assert(is_lt(tuple{1, 1} <=> array{1, 2}));
        static_assert(is_gt(tuple{2, 1} <=> array{1, 1}));

        static_assert(tuple{1, 2} == pair{1, 2});
        static_assert(tuple{2, 1} != pair{1, 2});
        static_assert(is_eq(tuple{1, 2} <=> pair{1, 2}));
        static_assert(is_lt(tuple{1, 1} <=> pair{1, 2}));
        static_assert(is_gt(tuple{2, 1} <=> pair{1, 1}));

        int a    = 0;
        int b    = 1;
        int c[2] = {2, 3};
        static_assert(tuple{&a, &b} == subrange{&a, &b});
        static_assert(tuple{&b, &a} != subrange{&a, &b});
        static_assert(is_eq(tuple{&a, &b} <=> pair{&a, &b}));
        static_assert(is_lt(tuple{&c[0], &c[0]} <=> pair{&c[0], &c[1]}));
        static_assert(is_gt(tuple{&c[1], &c[0]} <=> pair{&c[0], &c[1]}));
    }

    { // Check comparisons of (tuple_size_v == 3) tuple-like types
        static_assert(verify_comparisons<tuple<int, int, int>, array<int, 3>>);
        static_assert(!verify_comparisons<tuple<int, Incomparable, int>, array<Incomparable, 3>>);
        static_assert(tuple{1, 2, 3} == array{1, 2, 3});
        static_assert(tuple{6, 7, 8} != array{7, 8, 9});
        static_assert(is_eq(tuple{1, 2, 3} <=> array{1, 2, 3}));
        static_assert(is_lt(tuple{1, 2, 3} <=> array{1, 2, 4}));
        static_assert(is_gt(tuple{4, 3, 2} <=> array{3, 2, 1}));
    }

    { // Check short circuit evaluation
        static_assert(tuple{0, NeverCompare{}} != pair{1, NeverCompare{}});
        static_assert(is_lt(tuple{0, NeverCompare{}} <=> pair{1, NeverCompare{}}));
        static_assert(is_gt(tuple{1, NeverCompare{}} <=> pair{0, NeverCompare{}}));
    }

    { // Check result type of three-way comparison
        struct NeedsToSynth3Way {
            bool operator<(const NeedsToSynth3Way&) const; // not defined
        };

        static_assert(same_as<strong_ordering, decltype(tuple{0, 1} <=> array{0, 2})>);
        static_assert(same_as<weak_ordering, decltype(tuple{NeedsToSynth3Way{}} <=> array{NeedsToSynth3Way{}})>);
        static_assert(same_as<partial_ordering, decltype(tuple{0, 1.f} <=> array{0, 2})>);
    }

    { // Check incorrect three-way comparisons
        static_assert(!three_way_comparable_with<tuple<int>, array<int, 2>>);
        static_assert(!three_way_comparable_with<tuple<int>, subrange<int*, int*>>);
        static_assert(!three_way_comparable_with<tuple<int>, pair<int, int>>);
    }

    return true;
}

static_assert(test());
