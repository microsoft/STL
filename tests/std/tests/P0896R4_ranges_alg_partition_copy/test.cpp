// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <numeric>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

#define ASSERT(...) assert((__VA_ARGS__))

using P = pair<int, int>;

constexpr auto is_even = [](int i) { return i % 2 == 0; };

// Validate that partition_copy_result aliases in_out_out_result
STATIC_ASSERT(
    same_as<ranges::partition_copy_result<int, double, void*>, ranges::in_out_out_result<int, double, void*>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::partition_copy(borrowed<false>{}, nullptr_to<int>, nullptr_to<long>, is_even)),
    ranges::partition_copy_result<ranges::dangling, int*, long*>>);
STATIC_ASSERT(same_as<decltype(ranges::partition_copy(borrowed<true>{}, nullptr_to<int>, nullptr_to<long>, is_even)),
    ranges::partition_copy_result<int*, int*, long*>>);

struct empty_test {
    template <ranges::input_range Range, indirectly_writable<ranges::range_reference_t<Range>> Out1,
        indirectly_writable<ranges::range_reference_t<Range>> Out2>
    static constexpr void call() {
        // Validate empty ranges
        using ranges::partition_copy, ranges::partition_copy_result, ranges::iterator_t;

        {
            Range range{};
            auto result = partition_copy(range, Out1{}, Out2{}, is_even, get_first);
            STATIC_ASSERT(same_as<decltype(partition_copy(range, Out1{}, Out2{}, is_even, get_first)),
                partition_copy_result<iterator_t<Range>, Out1, Out2>>);
            ASSERT(result.in == range.end());
            ASSERT(result.out1.peek() == nullptr);
            ASSERT(result.out2.peek() == nullptr);
        }
        {
            Range range{};
            auto result = partition_copy(range.begin(), range.end(), Out1{}, Out2{}, is_even, get_first);
            STATIC_ASSERT(
                same_as<decltype(partition_copy(range.begin(), range.end(), Out1{}, Out2{}, is_even, get_first)),
                    partition_copy_result<iterator_t<Range>, Out1, Out2>>);
            ASSERT(result.in == range.end());
            ASSERT(result.out1.peek() == nullptr);
            ASSERT(result.out2.peek() == nullptr);
        }
    }
};

struct partition_copy_test {
    static constexpr int N = 32;

    template <ranges::input_range R, indirectly_writable<ranges::range_reference_t<R>> O1,
        indirectly_writable<ranges::range_reference_t<R>> O2>
    static constexpr void call() {
        using ranges::partition_copy;

        P source[N];
        for (int i = 0; i < N; ++i) {
            source[i] = {i, 42};
        }

        for (int i = 0; i < N; ++i) {
            P dest[N];
            ranges::fill(dest, P{-1, 13});

            const R range{source};
            auto result = partition_copy(
                range, O1{dest}, O2{dest + i}, [i](int x) { return x < i; }, get_first);
            assert(result.in == range.end());
            assert(result.out1.peek() == dest + i);
            assert(result.out2.peek() == dest + N);
            assert(ranges::equal(source, dest));
        }
    }
};

template <class Instantiator, class Elem>
constexpr void run_tests() {
    // Call Instantiator::template call</*...stuff...*/>() with a range whose element type is Elem, and two iterators to
    // which Elem is writable, whose properties are "interesting" for ranges::partition_copy. What combinations of
    // properties are "interesting"?

    // For the input range, the algorithm simply unwraps iterators and chugs through looking for the end. It doesn't
    // * take advantage of any capabilities provided by stronger-than-input categories,
    // * care if the sentinel and iterator have the same type,
    // * care if it can difference iterators with sentinels or each other, or
    // * care about the size of the input range at all. (It can't even use size info to check the outputs, because we
    // don't how many of the input elements will be written through each output.)
    // TLDR: One input range with a proxy reference type and no other notable properties (the so-called "weakest" input
    // range) suffices.

    // For the outputs, both of which are treated equivalently, the algorithm is similarly oblivious to properties other
    // than reference type and the ability to unwrap/rewrap. These could simply be the "weakest" writable iterator type
    // in with_writable_iterators.

    // Out of simple paranoia, let's permute ProxyRef; seven extra pointless tests won't hurt.

    using test::range, test::iterator, test::input, test::output, test::CanCompare, test::CanDifference, test::Common,
        test::ProxyRef, test::Sized;

    using proxy_range     = range<input, Elem, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::yes>;
    using non_proxy_range = range<input, Elem, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::no>;
    using proxy_iterator  = iterator<output, remove_const_t<Elem>, CanDifference::no, CanCompare::no, ProxyRef::yes>;
    using non_proxy_iterator = iterator<output, remove_const_t<Elem>, CanDifference::no, CanCompare::no, ProxyRef::no>;

    Instantiator::template call<proxy_range, proxy_iterator, proxy_iterator>();
    Instantiator::template call<proxy_range, proxy_iterator, non_proxy_iterator>();
    Instantiator::template call<proxy_range, non_proxy_iterator, proxy_iterator>();
    Instantiator::template call<proxy_range, non_proxy_iterator, non_proxy_iterator>();
    Instantiator::template call<non_proxy_range, proxy_iterator, proxy_iterator>();
    Instantiator::template call<non_proxy_range, proxy_iterator, non_proxy_iterator>();
    Instantiator::template call<non_proxy_range, non_proxy_iterator, proxy_iterator>();
    Instantiator::template call<non_proxy_range, non_proxy_iterator, non_proxy_iterator>();
}

int main() {
    STATIC_ASSERT((run_tests<empty_test, const P>(), true));
    run_tests<empty_test, const P>();

    STATIC_ASSERT((run_tests<partition_copy_test, const P>(), true));
    run_tests<partition_copy_test, const P>();
}
