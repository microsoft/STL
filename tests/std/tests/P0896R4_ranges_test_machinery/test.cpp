// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <ranges>
//
#include <range_algorithm_support.hpp>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

int main() {} // COMPILE-ONLY

// Validate test::iterator and test::sentinel
template <class Category, class Element, bool Sized, bool Common, bool Proxy, bool Wrapped>
constexpr bool iter_test() {
    using I = test::iterator<Category, Element, Sized, Common, Proxy, Wrapped>;

    STATIC_ASSERT(default_initializable<I>);
    STATIC_ASSERT(movable<I>);

    STATIC_ASSERT(!movable<Element> || indirectly_writable<I, Element>);

    constexpr bool can_write =
        derived_from<Category,
            output_iterator_tag> || (derived_from<Category, forward_iterator_tag> && assignable_from<Element&, Element>);
    STATIC_ASSERT(!can_write || output_iterator<I, Element>);

    STATIC_ASSERT(!derived_from<Category, input_iterator_tag> || input_iterator<I>);
    STATIC_ASSERT(!derived_from<Category, forward_iterator_tag> || forward_iterator<I>);
    STATIC_ASSERT(!derived_from<Category, bidirectional_iterator_tag> || bidirectional_iterator<I>);
    STATIC_ASSERT(!derived_from<Category, random_access_iterator_tag> || random_access_iterator<I>);
    STATIC_ASSERT(!derived_from<Category, contiguous_iterator_tag> || contiguous_iterator<I>);

    using S = test::sentinel<Element, Wrapped>;
    STATIC_ASSERT(sentinel_for<S, I>);
    STATIC_ASSERT(!Sized || sized_sentinel_for<S, I>);
    STATIC_ASSERT(!Common || sentinel_for<I, I>);
    STATIC_ASSERT(!Common || !Sized || sized_sentinel_for<I, I>);

    if constexpr (Wrapped) {
        STATIC_ASSERT(same_as<_Unwrapped_t<I>, test::iterator<Category, Element, Sized, Common, Proxy, false>>);
        STATIC_ASSERT(same_as<_Unwrapped_t<S>, test::sentinel<Element, false>>);
    }

    return true;
}

STATIC_ASSERT(iter_test<output_iterator_tag, int, false, false, false, false>());
STATIC_ASSERT(iter_test<output_iterator_tag, int, false, false, false, true>());
STATIC_ASSERT(iter_test<output_iterator_tag, int, false, false, true, false>());
STATIC_ASSERT(iter_test<output_iterator_tag, int, false, false, true, true>());
STATIC_ASSERT(iter_test<output_iterator_tag, int, false, true, false, false>());
STATIC_ASSERT(iter_test<output_iterator_tag, int, false, true, false, true>());
STATIC_ASSERT(iter_test<output_iterator_tag, int, false, true, true, false>());
STATIC_ASSERT(iter_test<output_iterator_tag, int, false, true, true, true>());
STATIC_ASSERT(iter_test<output_iterator_tag, int, true, false, false, false>());
STATIC_ASSERT(iter_test<output_iterator_tag, int, true, false, false, true>());
STATIC_ASSERT(iter_test<output_iterator_tag, int, true, false, true, false>());
STATIC_ASSERT(iter_test<output_iterator_tag, int, true, false, true, true>());
STATIC_ASSERT(iter_test<output_iterator_tag, int, true, true, false, false>());
STATIC_ASSERT(iter_test<output_iterator_tag, int, true, true, false, true>());
STATIC_ASSERT(iter_test<output_iterator_tag, int, true, true, true, false>());
STATIC_ASSERT(iter_test<output_iterator_tag, int, true, true, true, true>());

STATIC_ASSERT(iter_test<input_iterator_tag, int, false, false, false, false>());
STATIC_ASSERT(iter_test<input_iterator_tag, int, false, false, false, true>());
STATIC_ASSERT(iter_test<input_iterator_tag, int, false, false, true, false>());
STATIC_ASSERT(iter_test<input_iterator_tag, int, false, false, true, true>());
STATIC_ASSERT(iter_test<input_iterator_tag, int, false, true, false, false>());
STATIC_ASSERT(iter_test<input_iterator_tag, int, false, true, false, true>());
STATIC_ASSERT(iter_test<input_iterator_tag, int, false, true, true, false>());
STATIC_ASSERT(iter_test<input_iterator_tag, int, false, true, true, true>());
STATIC_ASSERT(iter_test<input_iterator_tag, int, true, false, false, false>());
STATIC_ASSERT(iter_test<input_iterator_tag, int, true, false, false, true>());
STATIC_ASSERT(iter_test<input_iterator_tag, int, true, false, true, false>());
STATIC_ASSERT(iter_test<input_iterator_tag, int, true, false, true, true>());
STATIC_ASSERT(iter_test<input_iterator_tag, int, true, true, false, false>());
STATIC_ASSERT(iter_test<input_iterator_tag, int, true, true, false, true>());
STATIC_ASSERT(iter_test<input_iterator_tag, int, true, true, true, false>());
STATIC_ASSERT(iter_test<input_iterator_tag, int, true, true, true, true>());

STATIC_ASSERT(iter_test<forward_iterator_tag, int, false, true, false, false>());
STATIC_ASSERT(iter_test<forward_iterator_tag, int, false, true, false, true>());
STATIC_ASSERT(iter_test<forward_iterator_tag, int, false, true, true, false>());
STATIC_ASSERT(iter_test<forward_iterator_tag, int, false, true, true, true>());
STATIC_ASSERT(iter_test<forward_iterator_tag, int, true, true, false, false>());
STATIC_ASSERT(iter_test<forward_iterator_tag, int, true, true, false, true>());
STATIC_ASSERT(iter_test<forward_iterator_tag, int, true, true, true, false>());
STATIC_ASSERT(iter_test<forward_iterator_tag, int, true, true, true, true>());

STATIC_ASSERT(iter_test<bidirectional_iterator_tag, int, false, true, false, false>());
STATIC_ASSERT(iter_test<bidirectional_iterator_tag, int, false, true, false, true>());
STATIC_ASSERT(iter_test<bidirectional_iterator_tag, int, false, true, true, false>());
STATIC_ASSERT(iter_test<bidirectional_iterator_tag, int, false, true, true, true>());
STATIC_ASSERT(iter_test<bidirectional_iterator_tag, int, true, true, false, false>());
STATIC_ASSERT(iter_test<bidirectional_iterator_tag, int, true, true, false, true>());
STATIC_ASSERT(iter_test<bidirectional_iterator_tag, int, true, true, true, false>());
STATIC_ASSERT(iter_test<bidirectional_iterator_tag, int, true, true, true, true>());

STATIC_ASSERT(iter_test<random_access_iterator_tag, int, false, true, false, false>());
STATIC_ASSERT(iter_test<random_access_iterator_tag, int, false, true, false, true>());
STATIC_ASSERT(iter_test<random_access_iterator_tag, int, false, true, true, false>());
STATIC_ASSERT(iter_test<random_access_iterator_tag, int, false, true, true, true>());
STATIC_ASSERT(iter_test<random_access_iterator_tag, int, true, true, false, false>());
STATIC_ASSERT(iter_test<random_access_iterator_tag, int, true, true, false, true>());
STATIC_ASSERT(iter_test<random_access_iterator_tag, int, true, true, true, false>());
STATIC_ASSERT(iter_test<random_access_iterator_tag, int, true, true, true, true>());

STATIC_ASSERT(iter_test<contiguous_iterator_tag, int, false, true, false, false>());
STATIC_ASSERT(iter_test<contiguous_iterator_tag, int, false, true, false, true>());
STATIC_ASSERT(iter_test<contiguous_iterator_tag, int, true, true, false, false>());
STATIC_ASSERT(iter_test<contiguous_iterator_tag, int, true, true, false, true>());

STATIC_ASSERT(same_as<_Unwrapped_t<test::sentinel<int, true>>, test::sentinel<int, false>>);

// Validate test::range
// clang-format off
template <class R>
concept has_member_size = requires(const R& r) {
    typename ranges::range_size_t<R>;
    { r.size() } -> same_as<ranges::range_size_t<R>>;
};
// clang-format on

template <class Category, class Element, bool Sized, bool SizedIterators, bool Common, bool CommonIterators, bool Proxy>
constexpr bool range_test() {
    using R = test::range<Category, Element, Sized, SizedIterators, Common, CommonIterators, Proxy>;
    using I = ranges::iterator_t<R>;
    using S = ranges::sentinel_t<R>;
    STATIC_ASSERT(same_as<I, test::iterator<Category, Element, SizedIterators, CommonIterators, Proxy, true>>);

    STATIC_ASSERT(!derived_from<Category, output_iterator_tag> || ranges::output_range<R, Element>);
    STATIC_ASSERT(!derived_from<Category, input_iterator_tag> || ranges::input_range<R>);
    STATIC_ASSERT(!derived_from<Category, forward_iterator_tag> || ranges::forward_range<R>);
    STATIC_ASSERT(!derived_from<Category, bidirectional_iterator_tag> || ranges::bidirectional_range<R>);
    STATIC_ASSERT(!derived_from<Category, random_access_iterator_tag> || ranges::random_access_range<R>);
    STATIC_ASSERT(!derived_from<Category, contiguous_iterator_tag> || ranges::contiguous_range<R>);

    if constexpr (Common) {
        STATIC_ASSERT(CommonIterators);
        STATIC_ASSERT(ranges::common_range<R>);
    } else {
        STATIC_ASSERT(same_as<S, test::sentinel<Element, true>>);
    }

    STATIC_ASSERT(!CommonIterators || sentinel_for<I, I>);

    constexpr bool is_sized = Sized || (SizedIterators && derived_from<Category, forward_iterator_tag>);
    STATIC_ASSERT(ranges::sized_range<R> == is_sized);
    if constexpr (Sized) {
        STATIC_ASSERT(has_member_size<R>);
    }

    return true;
}

STATIC_ASSERT(range_test<output_iterator_tag, int, false, false, false, false, false>());
STATIC_ASSERT(range_test<output_iterator_tag, int, false, false, false, false, true>());
STATIC_ASSERT(range_test<output_iterator_tag, int, false, false, false, true, false>());
STATIC_ASSERT(range_test<output_iterator_tag, int, false, false, false, true, true>());
STATIC_ASSERT(range_test<output_iterator_tag, int, false, false, true, true, false>());
STATIC_ASSERT(range_test<output_iterator_tag, int, false, false, true, true, true>());
STATIC_ASSERT(range_test<output_iterator_tag, int, false, true, false, false, false>());
STATIC_ASSERT(range_test<output_iterator_tag, int, false, true, false, false, true>());
STATIC_ASSERT(range_test<output_iterator_tag, int, false, true, false, true, false>());
STATIC_ASSERT(range_test<output_iterator_tag, int, false, true, false, true, true>());
STATIC_ASSERT(range_test<output_iterator_tag, int, false, true, true, true, false>());
STATIC_ASSERT(range_test<output_iterator_tag, int, false, true, true, true, true>());
STATIC_ASSERT(range_test<output_iterator_tag, int, true, false, false, false, false>());
STATIC_ASSERT(range_test<output_iterator_tag, int, true, false, false, false, true>());
STATIC_ASSERT(range_test<output_iterator_tag, int, true, false, false, true, false>());
STATIC_ASSERT(range_test<output_iterator_tag, int, true, false, false, true, true>());
STATIC_ASSERT(range_test<output_iterator_tag, int, true, false, true, true, false>());
STATIC_ASSERT(range_test<output_iterator_tag, int, true, false, true, true, true>());
STATIC_ASSERT(range_test<output_iterator_tag, int, true, true, false, false, false>());
STATIC_ASSERT(range_test<output_iterator_tag, int, true, true, false, false, true>());
STATIC_ASSERT(range_test<output_iterator_tag, int, true, true, false, true, false>());
STATIC_ASSERT(range_test<output_iterator_tag, int, true, true, false, true, true>());
STATIC_ASSERT(range_test<output_iterator_tag, int, true, true, true, true, false>());
STATIC_ASSERT(range_test<output_iterator_tag, int, true, true, true, true, true>());

STATIC_ASSERT(range_test<input_iterator_tag, int, false, false, false, false, false>());
STATIC_ASSERT(range_test<input_iterator_tag, int, false, false, false, false, true>());
STATIC_ASSERT(range_test<input_iterator_tag, int, false, false, false, true, false>());
STATIC_ASSERT(range_test<input_iterator_tag, int, false, false, false, true, true>());
STATIC_ASSERT(range_test<input_iterator_tag, int, false, false, true, true, false>());
STATIC_ASSERT(range_test<input_iterator_tag, int, false, false, true, true, true>());
STATIC_ASSERT(range_test<input_iterator_tag, int, false, true, false, false, false>());
STATIC_ASSERT(range_test<input_iterator_tag, int, false, true, false, false, true>());
STATIC_ASSERT(range_test<input_iterator_tag, int, false, true, false, true, false>());
STATIC_ASSERT(range_test<input_iterator_tag, int, false, true, false, true, true>());
STATIC_ASSERT(range_test<input_iterator_tag, int, false, true, true, true, false>());
STATIC_ASSERT(range_test<input_iterator_tag, int, false, true, true, true, true>());
STATIC_ASSERT(range_test<input_iterator_tag, int, true, false, false, false, false>());
STATIC_ASSERT(range_test<input_iterator_tag, int, true, false, false, false, true>());
STATIC_ASSERT(range_test<input_iterator_tag, int, true, false, false, true, false>());
STATIC_ASSERT(range_test<input_iterator_tag, int, true, false, false, true, true>());
STATIC_ASSERT(range_test<input_iterator_tag, int, true, false, true, true, false>());
STATIC_ASSERT(range_test<input_iterator_tag, int, true, false, true, true, true>());
STATIC_ASSERT(range_test<input_iterator_tag, int, true, true, false, false, false>());
STATIC_ASSERT(range_test<input_iterator_tag, int, true, true, false, false, true>());
STATIC_ASSERT(range_test<input_iterator_tag, int, true, true, false, true, false>());
STATIC_ASSERT(range_test<input_iterator_tag, int, true, true, false, true, true>());
STATIC_ASSERT(range_test<input_iterator_tag, int, true, true, true, true, false>());
STATIC_ASSERT(range_test<input_iterator_tag, int, true, true, true, true, true>());

STATIC_ASSERT(range_test<forward_iterator_tag, int, false, false, false, true, false>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, false, false, false, true, true>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, false, false, true, true, false>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, false, false, true, true, true>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, false, true, false, true, false>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, false, true, false, true, true>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, false, true, true, true, false>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, false, true, true, true, true>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, true, false, false, true, false>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, true, false, false, true, true>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, true, false, true, true, false>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, true, false, true, true, true>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, true, true, false, true, false>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, true, true, false, true, true>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, true, true, true, true, false>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, true, true, true, true, true>());

STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, false, false, false, true, false>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, false, false, false, true, true>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, false, false, true, true, false>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, false, false, true, true, true>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, false, true, false, true, false>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, false, true, false, true, true>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, false, true, true, true, false>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, false, true, true, true, true>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, true, false, false, true, false>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, true, false, false, true, true>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, true, false, true, true, false>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, true, false, true, true, true>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, true, true, false, true, false>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, true, true, false, true, true>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, true, true, true, true, false>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, true, true, true, true, true>());

STATIC_ASSERT(range_test<random_access_iterator_tag, int, false, true, false, true, false>());
STATIC_ASSERT(range_test<random_access_iterator_tag, int, false, true, false, true, true>());
STATIC_ASSERT(range_test<random_access_iterator_tag, int, false, true, true, true, false>());
STATIC_ASSERT(range_test<random_access_iterator_tag, int, false, true, true, true, true>());
STATIC_ASSERT(range_test<random_access_iterator_tag, int, true, true, false, true, false>());
STATIC_ASSERT(range_test<random_access_iterator_tag, int, true, true, false, true, true>());
STATIC_ASSERT(range_test<random_access_iterator_tag, int, true, true, true, true, false>());
STATIC_ASSERT(range_test<random_access_iterator_tag, int, true, true, true, true, true>());

STATIC_ASSERT(range_test<contiguous_iterator_tag, int, false, true, false, true, false>());
STATIC_ASSERT(range_test<contiguous_iterator_tag, int, false, true, true, true, false>());
STATIC_ASSERT(range_test<contiguous_iterator_tag, int, true, true, false, true, false>());
STATIC_ASSERT(range_test<contiguous_iterator_tag, int, true, true, true, true, false>());

// Validate move_only_range
STATIC_ASSERT(ranges::input_range<move_only_range<int>>);
STATIC_ASSERT(!ranges::forward_range<move_only_range<int>>);
STATIC_ASSERT(movable<move_only_range<int>>);
STATIC_ASSERT(!copyable<move_only_range<int>>);

struct instantiate {
    template <class Range1, class Range2>
    static constexpr void call() {
        STATIC_ASSERT(ranges::input_range<Range1>);
        STATIC_ASSERT(same_as<ranges::range_value_t<Range1>, double>);
        STATIC_ASSERT(indirectly_writable<ranges::iterator_t<Range1>, const double&>);

        STATIC_ASSERT(ranges::input_range<Range2>);
        STATIC_ASSERT(same_as<ranges::range_value_t<Range2>, void*>);
        STATIC_ASSERT(!indirectly_writable<ranges::iterator_t<Range2>, void*>);
    }
};

template void test_in_in<instantiate, double, void* const>();
