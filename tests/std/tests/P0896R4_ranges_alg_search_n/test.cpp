// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::search_n(borrowed<false>{}, 13, 42)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::search_n(borrowed<true>{}, 42, 13)), ranges::subrange<int*>>);

using P = pair<int, int>;

struct instantiator {
    static constexpr array<P, 11> pairs = {
        P{0, 42}, P{1, 42}, P{1, 42}, P{0, 42}, P{1, 42}, P{1, 42}, P{0, 42}, P{1, 42}, P{1, 42}, P{1, 42}, P{0, 42}};

    template <class Fwd>
    static constexpr void call() {
        const Fwd range{pairs};

        // defaulted predicate and projections
        {
            const auto result = ranges::search_n(range, 2, P{1, 42});
            STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<ranges::iterator_t<Fwd>>>);
            assert(result.begin() == ranges::next(range.begin(), 1));
            assert(result.end() == ranges::next(range.begin(), 3));
        }
        {
            const auto result = ranges::search_n(ranges::begin(range), ranges::end(range), 2, P{1, 42});
            STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<ranges::iterator_t<Fwd>>>);
            assert(result.begin() == ranges::next(range.begin(), 1));
            assert(result.end() == ranges::next(range.begin(), 3));
        }

        // explicit predicate
        {
            const auto result = ranges::search_n(range, 2, P{1, 42}, ranges::equal_to{});
            STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<ranges::iterator_t<Fwd>>>);
            assert(result.begin() == ranges::next(range.begin(), 1));
            assert(result.end() == ranges::next(range.begin(), 3));
        }
        {
            const auto result =
                ranges::search_n(ranges::begin(range), ranges::end(range), 2, P{1, 42}, ranges::equal_to{});
            STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<ranges::iterator_t<Fwd>>>);
            assert(result.begin() == ranges::next(range.begin(), 1));
            assert(result.end() == ranges::next(range.begin(), 3));
        }

        // explicit predicate and projection
        constexpr auto cmp = [](auto&& x, auto&& y) { return x == y + 1; };
        {
            const auto result = ranges::search_n(range, 3, 0, cmp, get_first);
            STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<ranges::iterator_t<Fwd>>>);
            assert(result.begin() == ranges::next(range.begin(), 7));
            assert(result.end() == ranges::next(range.begin(), 10));
        }
        {
            const auto result = ranges::search_n(ranges::begin(range), ranges::end(range), 3, 0, cmp, get_first);
            STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<ranges::iterator_t<Fwd>>>);
            assert(result.begin() == ranges::next(range.begin(), 7));
            assert(result.end() == ranges::next(range.begin(), 10));
        }

        // negative case
        {
            const auto result = ranges::search_n(range, 4, 0, cmp, get_first);
            STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<ranges::iterator_t<Fwd>>>);
            assert(result.begin() == range.end());
            assert(result.end() == range.end());
        }
        {
            const auto result = ranges::search_n(ranges::begin(range), ranges::end(range), 4, 0, cmp, get_first);
            STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<ranges::iterator_t<Fwd>>>);
            assert(result.begin() == range.end());
            assert(result.end() == range.end());
        }

        // trivial case: empty needle
        {
            const auto result = ranges::search_n(range, 0, 0, cmp, get_first);
            STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<ranges::iterator_t<Fwd>>>);
            assert(result.begin() == range.begin());
            assert(result.end() == range.begin());
        }
        {
            const auto result = ranges::search_n(ranges::begin(range), ranges::end(range), 0, 0, cmp, get_first);
            STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<ranges::iterator_t<Fwd>>>);
            assert(result.begin() == range.begin());
            assert(result.end() == range.begin());
        }

        // trivial case: range too small
        {
            const auto result = ranges::search_n(range, 99999, 0, cmp, get_first);
            STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<ranges::iterator_t<Fwd>>>);
            assert(result.begin() == range.end());
            assert(result.end() == range.end());
        }
        {
            const auto result = ranges::search_n(ranges::begin(range), ranges::end(range), 99999, 0, cmp, get_first);
            STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<ranges::iterator_t<Fwd>>>);
            assert(result.begin() == range.end());
            assert(result.end() == range.end());
        }

        // trivial case: negative count
        {
            const auto result = ranges::search_n(range, -42, 0, cmp, get_first);
            STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<ranges::iterator_t<Fwd>>>);
            assert(result.begin() == range.begin());
            assert(result.end() == range.begin());
        }
        {
            const auto result = ranges::search_n(ranges::begin(range), ranges::end(range), -42, 0, cmp, get_first);
            STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<ranges::iterator_t<Fwd>>>);
            assert(result.begin() == range.begin());
            assert(result.end() == range.begin());
        }
    }
};

int main() {
    using Elem = const P;
    STATIC_ASSERT((test_fwd<instantiator, Elem>(), true));
    test_fwd<instantiator, Elem>();
}
