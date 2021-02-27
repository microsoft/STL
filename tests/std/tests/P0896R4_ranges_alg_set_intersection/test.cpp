// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <span>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;
using P = pair<int, int>;

// Validate that set_intersection_result aliases in_in_out_result
STATIC_ASSERT(
    same_as<ranges::set_intersection_result<int, void*, double>, ranges::in_in_out_result<int, void*, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::set_intersection(borrowed<false>{}, borrowed<false>{}, nullptr_to<int>,
                          ranges::less{}, identity{}, identity{})),
    ranges::set_intersection_result<ranges::dangling, ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::set_intersection(borrowed<false>{}, borrowed<true>{}, nullptr_to<int>,
                          ranges::less{}, identity{}, identity{})),
    ranges::set_intersection_result<ranges::dangling, int*, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::set_intersection(borrowed<true>{}, borrowed<false>{}, nullptr_to<int>,
                          ranges::less{}, identity{}, identity{})),
    ranges::set_intersection_result<int*, ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::set_intersection(
                          borrowed<true>{}, borrowed<true>{}, nullptr_to<int>, ranges::less{}, identity{}, identity{})),
    ranges::set_intersection_result<int*, int*, int*>>);

struct instantiator {
    static constexpr P elements1[] = {{0, 10}, {0, 11}, {0, 12}, {1, 10}, {1, 11}, {3, 10}};
    static constexpr P elements2[] = {{13, 0}, {14, 0}, {10, 2}, {11, 3}, {12, 3}};
    static constexpr P expected[]  = {{0, 10}, {0, 11}, {3, 10}};

    template <ranges::input_range R1, ranges::input_range R2, weakly_incrementable O>
    static constexpr void call() {
        using ranges::set_intersection, ranges::set_intersection_result, ranges::equal, ranges::iterator_t,
            ranges::less;

        constexpr auto osize = ranges::size(elements1) + ranges::size(elements2);

        { // Validate range overload
            P output[osize]{};
            R1 range1{elements1};
            R2 range2{elements2};
            const same_as<set_intersection_result<iterator_t<R1>, iterator_t<R2>, O>> auto result =
                set_intersection(range1, range2, O{output}, ranges::less{}, get_first, get_second);
            assert(result.in1 == range1.end());
            assert(result.in2 == range2.end());
            assert(result.out.peek() == output + ranges::size(expected));
            assert(equal(span{output}.first<ranges::size(expected)>(), expected));
        }
        { // Validate iterator overload
            P output[osize]{};
            R1 range1{elements1};
            R2 range2{elements2};
            const same_as<set_intersection_result<iterator_t<R1>, iterator_t<R2>, O>> auto result =
                set_intersection(range1.begin(), range1.end(), range2.begin(), range2.end(), O{output}, ranges::less{},
                    get_first, get_second);
            assert(result.in1 == range1.end());
            assert(result.in2 == range2.end());
            assert(result.out.peek() == output + ranges::size(expected));
            assert(equal(span{output}.first<ranges::size(expected)>(), expected));
        }

        { // Validate range overload, empty range1
            P output[osize]{};
            R1 range1{};
            R2 range2{elements2};
            const same_as<set_intersection_result<iterator_t<R1>, iterator_t<R2>, O>> auto result =
                set_intersection(range1, range2, O{output}, ranges::less{}, get_first, get_second);
            assert(result.in1 == range1.end());
            assert(result.in2 == range2.end());
            assert(result.out.peek() == output);
        }
        { // Validate iterator overload, empty range2
            P output[osize]{};
            R1 range1{elements1};
            R2 range2{};
            const same_as<set_intersection_result<iterator_t<R1>, iterator_t<R2>, O>> auto result =
                set_intersection(range1.begin(), range1.end(), range2.begin(), range2.end(), O{output}, ranges::less{},
                    get_first, get_second);
            assert(result.in1 == range1.end());
            assert(result.in2 == range2.end());
            assert(result.out.peek() == output);
        }
    }
};

template <class Continuation>
struct generate_readable_ranges {
    template <class... Args>
    static constexpr void call() {
        using namespace test;
        using test::range;

        // The algorithm calls advance(i, s), so it's sensitive to CanDifference and Common in addition to the normal
        // proxy sensitivity.

        Continuation::template call<Args...,
            range<input, const P, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<input, const P, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::yes>>();
        Continuation::template call<Args...,
            range<input, const P, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<input, const P, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::yes>>();

        Continuation::template call<Args...,
            range<input, const P, Sized::no, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<input, const P, Sized::no, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::yes>>();
    }
};

template <class Continuation>
struct generate_writable_iterators {
    template <class... Args>
    static constexpr void call() {
        using namespace test;
        using test::iterator;

        // The algorithm is completely oblivious to all properties except for proxyness,
        // so again we'll vary that property, and we'll also get coverage from input iterators to ensure the algorithm
        // doesn't inadvertently depend on the output_iterator-only `*i++ = meow` expression.

        Continuation::template call<Args..., iterator<output, P, CanDifference::no, CanCompare::no, ProxyRef::no>>();
        Continuation::template call<Args..., iterator<output, P, CanDifference::no, CanCompare::no, ProxyRef::yes>>();

        Continuation::template call<Args..., iterator<input, P, CanDifference::no, CanCompare::no, ProxyRef::no>>();
        Continuation::template call<Args..., iterator<input, P, CanDifference::no, CanCompare::no, ProxyRef::yes>>();
    }
};

constexpr void run_tests() {
    generate_readable_ranges<generate_readable_ranges<generate_writable_iterators<instantiator>>>::call();
}

int main() {
    STATIC_ASSERT((run_tests(), true));
    run_tests();
}
