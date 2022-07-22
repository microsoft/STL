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

// Validate that merge_result aliases in_in_out_result
STATIC_ASSERT(same_as<ranges::merge_result<int, void*, double>, ranges::in_in_out_result<int, void*, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::merge(borrowed<false>{}, borrowed<false>{}, nullptr_to<int>)),
    ranges::merge_result<ranges::dangling, ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::merge(borrowed<false>{}, borrowed<true>{}, nullptr_to<int>)),
    ranges::merge_result<ranges::dangling, int*, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::merge(borrowed<true>{}, borrowed<false>{}, nullptr_to<int>)),
    ranges::merge_result<int*, ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::merge(borrowed<true>{}, borrowed<true>{}, nullptr_to<int>)),
    ranges::merge_result<int*, int*, int*>>);

struct instantiator {
    static constexpr P elements1[] = {{0, 10}, {0, 11}, {0, 12}, {1, 10}, {1, 11}, {3, 10}};
    static constexpr P elements2[] = {{13, 0}, {14, 0}, {10, 2}, {11, 3}, {12, 3}};
    static constexpr P expected[]  = {
         {0, 10}, {0, 11}, {0, 12}, {13, 0}, {14, 0}, {1, 10}, {1, 11}, {10, 2}, {3, 10}, {11, 3}, {12, 3}};

    static constexpr auto counting_compare(size_t& counter) {
        return [&counter](auto&& x, auto&& y) {
            ++counter;
            return ranges::less{}(x, y);
        };
    }

    template <ranges::input_range R1, ranges::input_range R2, weakly_incrementable O>
    static constexpr void call() {
        using ranges::merge, ranges::merge_result, ranges::end, ranges::equal, ranges::iterator_t, ranges::size;

        { // Validate range overload
            P output[size(expected)]{};
            R1 range1{elements1};
            R2 range2{elements2};
            size_t counter = 0;

            const same_as<merge_result<iterator_t<R1>, iterator_t<R2>, O>> auto result =
                merge(range1, range2, O{output}, counting_compare(counter), get_first, get_second);
            assert(result.in1 == range1.end());
            assert(result.in2 == range2.end());
            assert(result.out.peek() == end(output));
            assert(equal(output, expected));
            assert(counter <= size(elements1) + size(elements2) - 1);
        }
        { // Validate iterator overload
            P output[size(expected)]{};
            R1 range1{elements1};
            R2 range2{elements2};
            size_t counter = 0;

            const same_as<merge_result<iterator_t<R1>, iterator_t<R2>, O>> auto result =
                merge(range1.begin(), range1.end(), range2.begin(), range2.end(), O{output}, counting_compare(counter),
                    get_first, get_second);
            assert(result.in1 == range1.end());
            assert(result.in2 == range2.end());
            assert(result.out.peek() == end(output));
            assert(equal(output, expected));
            assert(counter <= size(elements1) + size(elements2) - 1);
        }

        { // Validate range overload, empty range1
            P output[size(elements2)]{};
            R1 range1{span<const P, 0>{}};
            R2 range2{elements2};
            size_t counter = 0;

            const same_as<merge_result<iterator_t<R1>, iterator_t<R2>, O>> auto result =
                merge(range1, range2, O{output}, counting_compare(counter), get_first, get_second);
            assert(result.in1 == range1.end());
            assert(result.in2 == range2.end());
            assert(result.out.peek() == end(output));
            assert(equal(output, elements2));
            assert(counter == 0);
        }
        { // Validate iterator overload, empty range2
            P output[size(elements1)]{};
            R1 range1{elements1};
            R2 range2{span<const P, 0>{}};
            size_t counter = 0;

            const same_as<merge_result<iterator_t<R1>, iterator_t<R2>, O>> auto result =
                merge(range1.begin(), range1.end(), range2.begin(), range2.end(), O{output}, counting_compare(counter),
                    get_first, get_second);
            assert(result.in1 == range1.end());
            assert(result.in2 == range2.end());
            assert(result.out.peek() == end(output));
            assert(equal(output, elements1));
            assert(counter == 0);
        }
    }
};

template <class Continuation>
struct generate_readable_ranges {
    template <class... Args>
    static constexpr void call() {
        using namespace test;
        using test::range;

        // The algorithm is completely oblivious to:
        // * categories stronger than input
        // * whether the end sentinel is an iterator
        // * size information
        // * iterator and/or sentinel differencing
        // so let's vary proxyness for coverage and call it good.

        Continuation::template call<Args...,
            range<input, const P, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::no>>();
        Continuation::template call<Args...,
            range<input, const P, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::yes>>();
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
