// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <ranges>

#include <range_algorithm_support.hpp>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

using test::CanCompare, test::CanDifference, test::WrappedState, test::ProxyRef, test::to_bool;

// Validate test::iterator and test::sentinel
template <class Category, class Element, CanDifference Diff, CanCompare Eq, ProxyRef Proxy, WrappedState Wrapped>
constexpr bool iter_test() {
    using test::iterator, test::sentinel;

    using I = iterator<Category, Element, Diff, Eq, Proxy, Wrapped>;

    STATIC_ASSERT(default_initializable<I> == (Eq == CanCompare::yes));
    STATIC_ASSERT(movable<I>);

    STATIC_ASSERT(!movable<Element> || indirectly_writable<I, Element>);

    constexpr bool can_write = derived_from<Category, output_iterator_tag>
                            || (derived_from<Category, forward_iterator_tag> && assignable_from<Element&, Element>);
    STATIC_ASSERT(!can_write || output_iterator<I, Element>);

    STATIC_ASSERT(!derived_from<Category, input_iterator_tag> || input_iterator<I>);
    STATIC_ASSERT(!derived_from<Category, forward_iterator_tag> || forward_iterator<I>);
    STATIC_ASSERT(!derived_from<Category, bidirectional_iterator_tag> || bidirectional_iterator<I>);
    STATIC_ASSERT(!derived_from<Category, random_access_iterator_tag> || random_access_iterator<I>);
    STATIC_ASSERT(!derived_from<Category, contiguous_iterator_tag> || contiguous_iterator<I>);

    using S = sentinel<Element, Wrapped>;
    STATIC_ASSERT(sentinel_for<S, I>);
    STATIC_ASSERT(!to_bool(Diff) || sized_sentinel_for<S, I>);
    STATIC_ASSERT(!to_bool(Eq) || sentinel_for<I, I>);
    STATIC_ASSERT(!to_bool(Eq) || !to_bool(Diff) || sized_sentinel_for<I, I>);

    if constexpr (Wrapped == WrappedState::ignorant) {
        STATIC_ASSERT(sentinel_for<S, iterator<Category, Element, Diff, Eq, Proxy, WrappedState::wrapped>>);
        STATIC_ASSERT(!sentinel_for<S, iterator<Category, Element, Diff, Eq, Proxy, WrappedState::unwrapped>>);

        STATIC_ASSERT(sentinel_for<sentinel<Element, WrappedState::wrapped>, I>);
        STATIC_ASSERT(!sentinel_for<sentinel<Element, WrappedState::unwrapped>, I>);

        STATIC_ASSERT(!_Unwrappable_v<I>);
        STATIC_ASSERT(!_Unwrappable_v<S>);
    } else if constexpr (Wrapped == WrappedState::wrapped) {
        STATIC_ASSERT(sentinel_for<S, iterator<Category, Element, Diff, Eq, Proxy, WrappedState::ignorant>>);
        STATIC_ASSERT(!sentinel_for<S, iterator<Category, Element, Diff, Eq, Proxy, WrappedState::unwrapped>>);

        STATIC_ASSERT(sentinel_for<sentinel<Element, WrappedState::ignorant>, I>);
        STATIC_ASSERT(!sentinel_for<sentinel<Element, WrappedState::unwrapped>, I>);

        if constexpr (derived_from<Category, contiguous_iterator_tag>) {
            STATIC_ASSERT(same_as<_Unwrapped_t<I>, Element*>);
        } else {
            STATIC_ASSERT(
                same_as<_Unwrapped_t<I>, iterator<Category, Element, Diff, Eq, Proxy, WrappedState::unwrapped>>);
        }
        STATIC_ASSERT(same_as<_Unwrapped_t<S>, sentinel<Element, WrappedState::unwrapped>>);
    }

    STATIC_ASSERT(convertible_to<I, typename I::Consterator>);
    STATIC_ASSERT(!to_bool(Eq) || convertible_to<const I&, typename I::Consterator>);

    return true;
}

STATIC_ASSERT(
    iter_test<output_iterator_tag, int, CanDifference::no, CanCompare::no, ProxyRef::no, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<output_iterator_tag, int, CanDifference::no, CanCompare::no, ProxyRef::no, WrappedState::wrapped>());
STATIC_ASSERT(
    iter_test<output_iterator_tag, int, CanDifference::no, CanCompare::no, ProxyRef::yes, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<output_iterator_tag, int, CanDifference::no, CanCompare::no, ProxyRef::yes, WrappedState::wrapped>());
STATIC_ASSERT(
    iter_test<output_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::no, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<output_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::no, WrappedState::wrapped>());
STATIC_ASSERT(
    iter_test<output_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::yes, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<output_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::yes, WrappedState::wrapped>());
STATIC_ASSERT(
    iter_test<output_iterator_tag, int, CanDifference::yes, CanCompare::no, ProxyRef::no, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<output_iterator_tag, int, CanDifference::yes, CanCompare::no, ProxyRef::no, WrappedState::wrapped>());
STATIC_ASSERT(
    iter_test<output_iterator_tag, int, CanDifference::yes, CanCompare::no, ProxyRef::yes, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<output_iterator_tag, int, CanDifference::yes, CanCompare::no, ProxyRef::yes, WrappedState::wrapped>());
STATIC_ASSERT(
    iter_test<output_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::no, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<output_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::no, WrappedState::wrapped>());
STATIC_ASSERT(
    iter_test<output_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::yes, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<output_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::yes, WrappedState::wrapped>());

STATIC_ASSERT(
    iter_test<input_iterator_tag, int, CanDifference::no, CanCompare::no, ProxyRef::no, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<input_iterator_tag, int, CanDifference::no, CanCompare::no, ProxyRef::no, WrappedState::wrapped>());
STATIC_ASSERT(
    iter_test<input_iterator_tag, int, CanDifference::no, CanCompare::no, ProxyRef::yes, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<input_iterator_tag, int, CanDifference::no, CanCompare::no, ProxyRef::yes, WrappedState::wrapped>());
STATIC_ASSERT(
    iter_test<input_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::no, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<input_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::no, WrappedState::wrapped>());
STATIC_ASSERT(
    iter_test<input_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::yes, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<input_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::yes, WrappedState::wrapped>());
STATIC_ASSERT(
    iter_test<input_iterator_tag, int, CanDifference::yes, CanCompare::no, ProxyRef::no, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<input_iterator_tag, int, CanDifference::yes, CanCompare::no, ProxyRef::no, WrappedState::wrapped>());
STATIC_ASSERT(
    iter_test<input_iterator_tag, int, CanDifference::yes, CanCompare::no, ProxyRef::yes, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<input_iterator_tag, int, CanDifference::yes, CanCompare::no, ProxyRef::yes, WrappedState::wrapped>());
STATIC_ASSERT(
    iter_test<input_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::no, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<input_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::no, WrappedState::wrapped>());
STATIC_ASSERT(
    iter_test<input_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::yes, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<input_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::yes, WrappedState::wrapped>());

STATIC_ASSERT(
    iter_test<forward_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::no, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<forward_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::no, WrappedState::wrapped>());
STATIC_ASSERT(
    iter_test<forward_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::yes, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<forward_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::yes, WrappedState::wrapped>());
STATIC_ASSERT(
    iter_test<forward_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::no, WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<forward_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::no, WrappedState::wrapped>());
STATIC_ASSERT(iter_test<forward_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::yes,
    WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<forward_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::yes, WrappedState::wrapped>());

STATIC_ASSERT(iter_test<bidirectional_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::no,
    WrappedState::unwrapped>());
STATIC_ASSERT(iter_test<bidirectional_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::no,
    WrappedState::wrapped>());
STATIC_ASSERT(iter_test<bidirectional_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::yes,
    WrappedState::unwrapped>());
STATIC_ASSERT(iter_test<bidirectional_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::yes,
    WrappedState::wrapped>());
STATIC_ASSERT(iter_test<bidirectional_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::no,
    WrappedState::unwrapped>());
STATIC_ASSERT(iter_test<bidirectional_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::no,
    WrappedState::wrapped>());
STATIC_ASSERT(iter_test<bidirectional_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::yes,
    WrappedState::unwrapped>());
STATIC_ASSERT(iter_test<bidirectional_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::yes,
    WrappedState::wrapped>());

STATIC_ASSERT(iter_test<random_access_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::no,
    WrappedState::unwrapped>());
STATIC_ASSERT(iter_test<random_access_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::no,
    WrappedState::wrapped>());
STATIC_ASSERT(iter_test<random_access_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::yes,
    WrappedState::unwrapped>());
STATIC_ASSERT(iter_test<random_access_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::yes,
    WrappedState::wrapped>());
STATIC_ASSERT(iter_test<random_access_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::no,
    WrappedState::unwrapped>());
STATIC_ASSERT(iter_test<random_access_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::no,
    WrappedState::wrapped>());
STATIC_ASSERT(iter_test<random_access_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::yes,
    WrappedState::unwrapped>());
STATIC_ASSERT(iter_test<random_access_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::yes,
    WrappedState::wrapped>());

STATIC_ASSERT(iter_test<contiguous_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::no,
    WrappedState::unwrapped>());
STATIC_ASSERT(
    iter_test<contiguous_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::no, WrappedState::wrapped>());
STATIC_ASSERT(iter_test<contiguous_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::no,
    WrappedState::unwrapped>());
STATIC_ASSERT(iter_test<contiguous_iterator_tag, int, CanDifference::yes, CanCompare::yes, ProxyRef::no,
    WrappedState::wrapped>());

STATIC_ASSERT(
    same_as<_Unwrapped_t<test::sentinel<int, WrappedState::wrapped>>, test::sentinel<int, WrappedState::unwrapped>>);

// Validate test::range
template <class R>
concept has_member_size = requires(const R& r) {
    typename ranges::range_size_t<R>;
    { r.size() } -> same_as<ranges::range_size_t<R>>;
};

using test::Sized, test::Common;

template <class Category, class Element, Sized IsSized, CanDifference Diff, Common IsCommon, CanCompare Eq,
    ProxyRef Proxy>
constexpr bool range_test() {
    using R = test::range<Category, Element, IsSized, Diff, IsCommon, Eq, Proxy>;
    using I = ranges::iterator_t<R>;
    using S = ranges::sentinel_t<R>;
    STATIC_ASSERT(same_as<I, test::iterator<Category, Element, Diff, Eq, Proxy, WrappedState::wrapped>>);

    STATIC_ASSERT(!derived_from<Category, output_iterator_tag> || ranges::output_range<R, Element>);
    STATIC_ASSERT(!derived_from<Category, input_iterator_tag> || ranges::input_range<R>);
    STATIC_ASSERT(!derived_from<Category, forward_iterator_tag> || ranges::forward_range<R>);
    STATIC_ASSERT(!derived_from<Category, bidirectional_iterator_tag> || ranges::bidirectional_range<R>);
    STATIC_ASSERT(!derived_from<Category, random_access_iterator_tag> || ranges::random_access_range<R>);
    STATIC_ASSERT(!derived_from<Category, contiguous_iterator_tag> || ranges::contiguous_range<R>);

    if constexpr (to_bool(IsCommon)) {
        STATIC_ASSERT(to_bool(Eq));
        STATIC_ASSERT(ranges::common_range<R>);
    } else {
        STATIC_ASSERT(same_as<S, test::sentinel<Element, WrappedState::wrapped>>);
    }

    STATIC_ASSERT(!to_bool(Eq) || sentinel_for<I, I>);

    constexpr bool is_sized = to_bool(IsSized) || (to_bool(Diff) && derived_from<Category, forward_iterator_tag>);
    STATIC_ASSERT(ranges::sized_range<R> == is_sized);
    if constexpr (to_bool(IsSized)) {
        STATIC_ASSERT(has_member_size<R>);
    }

    return true;
}

STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::no>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::no, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::no, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::no>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::no, CanCompare::no, ProxyRef::no>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::no, CanCompare::no, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::no>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(range_test<output_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes,
    ProxyRef::yes>());

STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::no>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::no, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::no, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::no>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::no, CanCompare::no, ProxyRef::no>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::no, CanCompare::no, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::no>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<input_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::yes>());

STATIC_ASSERT(
    range_test<forward_iterator_tag, int, Sized::no, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<forward_iterator_tag, int, Sized::no, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<forward_iterator_tag, int, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<forward_iterator_tag, int, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<forward_iterator_tag, int, Sized::no, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<forward_iterator_tag, int, Sized::no, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<forward_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes, CanCompare::yes,
    ProxyRef::yes>());
STATIC_ASSERT(
    range_test<forward_iterator_tag, int, Sized::yes, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(
    range_test<forward_iterator_tag, int, Sized::yes, CanDifference::no, Common::no, CanCompare::yes, ProxyRef::yes>());
STATIC_ASSERT(
    range_test<forward_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes,
    ProxyRef::yes>());
STATIC_ASSERT(
    range_test<forward_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes, ProxyRef::no>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes,
    ProxyRef::yes>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes,
    ProxyRef::no>());
STATIC_ASSERT(range_test<forward_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes,
    ProxyRef::yes>());

STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, Sized::no, CanDifference::no, Common::no, CanCompare::yes,
    ProxyRef::no>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, Sized::no, CanDifference::no, Common::no, CanCompare::yes,
    ProxyRef::yes>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, Sized::no, CanDifference::no, Common::yes, CanCompare::yes,
    ProxyRef::no>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, Sized::no, CanDifference::no, Common::yes, CanCompare::yes,
    ProxyRef::yes>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, Sized::no, CanDifference::yes, Common::no, CanCompare::yes,
    ProxyRef::no>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, Sized::no, CanDifference::yes, Common::no, CanCompare::yes,
    ProxyRef::yes>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes, CanCompare::yes,
    ProxyRef::no>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes, CanCompare::yes,
    ProxyRef::yes>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, Sized::yes, CanDifference::no, Common::no, CanCompare::yes,
    ProxyRef::no>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, Sized::yes, CanDifference::no, Common::no, CanCompare::yes,
    ProxyRef::yes>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes,
    ProxyRef::no>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes,
    ProxyRef::yes>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes,
    ProxyRef::no>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes,
    ProxyRef::yes>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes,
    ProxyRef::no>());
STATIC_ASSERT(range_test<bidirectional_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes,
    ProxyRef::yes>());

STATIC_ASSERT(range_test<random_access_iterator_tag, int, Sized::no, CanDifference::yes, Common::no, CanCompare::yes,
    ProxyRef::no>());
STATIC_ASSERT(range_test<random_access_iterator_tag, int, Sized::no, CanDifference::yes, Common::no, CanCompare::yes,
    ProxyRef::yes>());
STATIC_ASSERT(range_test<random_access_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes, CanCompare::yes,
    ProxyRef::no>());
STATIC_ASSERT(range_test<random_access_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes, CanCompare::yes,
    ProxyRef::yes>());
STATIC_ASSERT(range_test<random_access_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes,
    ProxyRef::no>());
STATIC_ASSERT(range_test<random_access_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes,
    ProxyRef::yes>());
STATIC_ASSERT(range_test<random_access_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes,
    ProxyRef::no>());
STATIC_ASSERT(range_test<random_access_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes,
    ProxyRef::yes>());

STATIC_ASSERT(range_test<contiguous_iterator_tag, int, Sized::no, CanDifference::yes, Common::no, CanCompare::yes,
    ProxyRef::no>());
STATIC_ASSERT(range_test<contiguous_iterator_tag, int, Sized::no, CanDifference::yes, Common::yes, CanCompare::yes,
    ProxyRef::no>());
STATIC_ASSERT(range_test<contiguous_iterator_tag, int, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes,
    ProxyRef::no>());
STATIC_ASSERT(range_test<contiguous_iterator_tag, int, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes,
    ProxyRef::no>());

// Validate basic_borrowed_range
STATIC_ASSERT(ranges::input_range<basic_borrowed_range<int>>);
STATIC_ASSERT(!ranges::forward_range<basic_borrowed_range<int>>);
STATIC_ASSERT(!movable<basic_borrowed_range<int>>);

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
