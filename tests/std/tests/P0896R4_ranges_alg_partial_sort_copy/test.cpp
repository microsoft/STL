// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <span>
#include <string>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;
using P = pair<int, int>;

// Validate that partial_sort_copy_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::partial_sort_copy_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::partial_sort_copy(borrowed<false>{}, borrowed<false>{})),
    ranges::partial_sort_copy_result<ranges::dangling, ranges::dangling>>);
STATIC_ASSERT(same_as<decltype(ranges::partial_sort_copy(borrowed<false>{}, borrowed<true>{})),
    ranges::partial_sort_copy_result<ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::partial_sort_copy(borrowed<true>{}, borrowed<false>{})),
    ranges::partial_sort_copy_result<int*, ranges::dangling>>);
STATIC_ASSERT(same_as<decltype(ranges::partial_sort_copy(borrowed<true>{}, borrowed<true>{})),
    ranges::partial_sort_copy_result<int*, int*>>);

constexpr P source[]        = {{5, 11}, {1, 12}, {3, 13}, {4, 15}, {0, 16}, {2, 17}};
constexpr P expected_vals[] = {{0, 16}, {1, 12}, {2, 17}, {3, 13}, {4, 15}, {5, 11}};

struct instantiator1 {
    template <ranges::input_range In, ranges::random_access_range Out>
    static constexpr void call() {
        using ranges::partial_sort_copy, ranges::partial_sort_copy_result, ranges::equal, ranges::iterator_t,
            ranges::less, ranges::min, ranges::next, ranges::size;

        P output[2 * size(source)];
        constexpr int sizes[] = {0, int{size(source) / 2}, int{size(source)}, int{2 * size(source)}};

        { // Validate range overload
            for (const int& i : sizes) {
                In range1{source};
                Out range2{span{output}.first(static_cast<size_t>(i))};
                const same_as<partial_sort_copy_result<iterator_t<In>, iterator_t<Out>>> auto result =
                    partial_sort_copy(range1, range2, less{}, get_first, get_first);
                assert(result.in == range1.end());
                const auto n = min(i, int{size(source)});
                assert(result.out == range2.begin() + n);
                assert(equal(range2.begin(), range2.begin() + n, expected_vals, expected_vals + n));
            }

            // also with empty input
            In range1{span<const P, 0>{}};
            Out range2{output};
            const same_as<partial_sort_copy_result<iterator_t<In>, iterator_t<Out>>> auto result =
                partial_sort_copy(range1, range2, less{}, get_first, get_first);
            assert(result.in == range1.end());
            assert(result.out == range2.begin());
        }
    }
};

struct instantiator2 {
    template <ranges::input_range In, ranges::random_access_range Out>
    static constexpr void call() {
        using ranges::partial_sort_copy, ranges::partial_sort_copy_result, ranges::equal, ranges::iterator_t,
            ranges::less, ranges::min, ranges::next, ranges::size;

        P output[2 * size(source)];
        constexpr int sizes[] = {0, int{size(source) / 2}, int{size(source)}, int{2 * size(source)}};

        { // Validate iterator overload
            for (const int& i : sizes) {
                In range1{source};
                Out range2{span{output}.first(static_cast<size_t>(i))};
                const same_as<partial_sort_copy_result<iterator_t<In>, iterator_t<Out>>> auto result =
                    partial_sort_copy(
                        range1.begin(), range1.end(), range2.begin(), range2.end(), less{}, get_first, get_first);
                assert(result.in == range1.end());
                const auto n = min(i, int{size(source)});
                assert(result.out == range2.begin() + n);
                assert(equal(range2.begin(), range2.begin() + n, expected_vals, expected_vals + n));
            }

            // also with empty input
            In range1{span<const P, 0>{}};
            Out range2{output};
            const same_as<partial_sort_copy_result<iterator_t<In>, iterator_t<Out>>> auto result = partial_sort_copy(
                range1.begin(), range1.end(), range2.begin(), range2.end(), less{}, get_first, get_first);
            assert(result.in == range1.end());
            assert(result.out == range2.begin());
        }
    }
};

#ifdef TEST_EVERYTHING
int main() {
    // No constexpr tests - these overrun the compilers' constexpr step limits quickly
    test_in_random<instantiator1, const P, P>();
    test_in_random<instantiator2, const P, P>();
}
#else // ^^^ test all range combinations / test only interesting range combos vvv
constexpr void run_tests() {
    using namespace test;
    using test::iterator, test::range;
    // The algorithm uses advance(i, s) in the input range, so it's slightly sensitive to that range's commonality
    // and/or difference capability. We therefore test three kinds of source ranges:
    using source_input = range<input, const P, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::yes>;
    using source_forward =
        range<fwd, const P, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::yes>;
    using source_random =
        range<random, const P, Sized::no, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::yes>;

    // The result range must be random access - it's passed to various heap algorithm internals. Let's go ahead and use
    // all 15 permutations of random access (or contiguous) ranges:
    with_random_ranges<instantiator1, P>::call<source_input>();
    with_random_ranges<instantiator1, P>::call<source_forward>();
    with_random_ranges<instantiator1, P>::call<source_random>();

    with_random_ranges<instantiator2, P>::call<source_input>();
    with_random_ranges<instantiator2, P>::call<source_forward>();
    with_random_ranges<instantiator2, P>::call<source_random>();
}

struct weird_pair : pair<string, string> {
    using pair<string, string>::pair;

    weird_pair& operator=(const P& p) {
        first  = to_string(p.second);
        second = to_string(p.first);
        return *this;
    }
};

int main() {
    STATIC_ASSERT((run_tests(), true));
    run_tests();

    {
        constexpr auto proj                = [](const weird_pair& s) { return stoi(s.second); };
        const weird_pair expected_result[] = {{"16", "0"}, {"12", "1"}, {"17", "2"}};
        weird_pair actual[ranges::size(expected_result)];

        auto result = ranges::partial_sort_copy(source, actual, ranges::less{}, get_first, proj);
        assert(result.in == ranges::end(source));
        assert(result.out == ranges::end(actual));

        assert(ranges::equal(actual, expected_result));
    }
}
#endif // TEST_EVERYTHING
