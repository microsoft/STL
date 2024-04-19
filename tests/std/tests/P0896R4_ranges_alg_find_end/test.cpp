// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

// Validate dangling story
static_assert(same_as<decltype(ranges::find_end(borrowed<false>{}, span<const int>{})), ranges::dangling>);
static_assert(same_as<decltype(ranges::find_end(borrowed<true>{}, span<const int>{})), ranges::subrange<int*>>);

struct instantiator {
    static constexpr pair<int, int> pairs[] = {{0, 42}, {1, 42}, {0, 42}, {1, 42}, {0, 42}};
    static constexpr auto pred              = [](const int x, const int y) { return x == y + 1; };

    static constexpr int good_needle[] = {-1, 0};
    static constexpr int bad_needle[]  = {0, 0};

    template <ranges::forward_range Fwd1, ranges::forward_range Fwd2>
    static constexpr void test() {
        using ranges::find_end, ranges::begin, ranges::end, ranges::iterator_t, ranges::next, ranges::subrange;

        constexpr bool sized_result = sized_sentinel_for<iterator_t<Fwd1>, iterator_t<Fwd1>>;

        {
            // Validate range overload [found case]
            Fwd1 haystack{pairs};
            Fwd2 needle{good_needle};
            const same_as<subrange<iterator_t<Fwd1>>> auto result = find_end(haystack, needle, pred, get_first);
            static_assert(CanMemberSize<subrange<iterator_t<Fwd1>>> == sized_result);
            if constexpr (sized_result) {
                assert(result.size() == 2);
            }
            assert(result.begin() == next(begin(haystack), 2));
            assert(result.end() == next(begin(haystack), 4));
        }

        {
            // Validate range overload [not found case]
            Fwd1 haystack{pairs};
            Fwd2 needle{bad_needle};
            const same_as<subrange<iterator_t<Fwd1>>> auto result = find_end(haystack, needle, pred, get_first);
            assert(result.empty());
            assert(result.begin() == end(haystack));
            assert(result.end() == end(haystack));
        }

        {
            // Validate iterator + sentinel overload [found case]
            Fwd1 haystack{pairs};
            Fwd2 needle{good_needle};
            const same_as<subrange<iterator_t<Fwd1>>> auto result =
                find_end(begin(haystack), end(haystack), begin(needle), end(needle), pred, get_first);
            if constexpr (sized_result) {
                assert(result.size() == 2);
            }
            assert(result.begin() == next(begin(haystack), 2));
            assert(result.end() == next(begin(haystack), 4));
        }

        {
            // Validate range overload [not found case]
            Fwd1 haystack{pairs};
            Fwd2 needle{bad_needle};
            const same_as<subrange<iterator_t<Fwd1>>> auto result =
                find_end(begin(haystack), end(haystack), begin(needle), end(needle), pred, get_first);
            assert(result.empty());
            assert(result.begin() == end(haystack));
            assert(result.end() == end(haystack));
        }
    }

    template <ranges::forward_range Fwd1, ranges::forward_range Fwd2>
    static void call() {
        test<Fwd1, Fwd2>();
        static_assert((test<Fwd1, Fwd2>(), true));
    }
};

constexpr bool memcmp_test() {
    // Validate the memcmp optimization
    const int haystack[]                                    = {1, 2, 3, 1, 2, 3, 1, 2, 3};
    const int needle[]                                      = {2, 3, 1};
    const same_as<ranges::subrange<const int*>> auto result = ranges::find_end(haystack, span<const int>{needle});
    assert(result.begin() == haystack + 4);
    assert(result.end() == haystack + 7);
    return true;
}

constexpr void test_devcom_1559808() {
    // Regression test for DevCom-1559808, an interaction between vector and the
    // use of structured bindings in the constexpr evaluator.

    vector<int> haystack(33, 42); // No particular significance to any numbers in this function
    vector<int> needle(8, 42);
    using size_type = vector<int>::size_type;

    auto result = ranges::find_end(haystack, needle);
    assert(static_cast<size_type>(result.begin() - haystack.begin()) == haystack.size() - needle.size());
    assert(result.end() == haystack.end());

    needle.assign(6, 1729);
    result = ranges::find_end(haystack, needle);
    assert(result.begin() == haystack.end());
    assert(result.end() == haystack.end());
}

int main() {
#if !defined(_PREFAST_) && !defined(__EDG__) // TRANSITION, GH-1030 and GH-3567
    test_fwd_fwd<instantiator, const pair<int, int>, const int>();
#endif // ^^^ no workaround ^^^

    static_assert(memcmp_test());
    memcmp_test();

    static_assert((test_devcom_1559808(), true));
    test_devcom_1559808();
}
