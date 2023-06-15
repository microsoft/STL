// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <forward_list>
#include <list>
#include <ranges>
#include <span>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;
using ranges::bidirectional_range, ranges::random_access_range, ranges::sized_range, ranges::common_range,
    ranges::iterator_t;

template <bool B, class T>
using maybe_const = conditional_t<B, const T, T>;

// Helper concepts from [range.cartesian.view]
template <bool IsConst, class First, class... Rest>
concept CartesianProductIsRandomAccess =
    (random_access_range<maybe_const<IsConst, First>> && ...
        && (random_access_range<maybe_const<IsConst, Rest>> && sized_range<maybe_const<IsConst, Rest>>) );

template <class Rng>
concept CartesianProductCommonArg = common_range<Rng> || (sized_range<Rng> && random_access_range<Rng>);

template <bool IsConst, class First, class... Rest>
concept CartesianProductIsBidirectional =
    (bidirectional_range<maybe_const<IsConst, First>> && ...
        && (bidirectional_range<maybe_const<IsConst, Rest>> && CartesianProductCommonArg<maybe_const<IsConst, Rest>>) );

template <class First, class... Rest>
concept CartesianProductIsCommon = CartesianProductCommonArg<First>;

template <class... Ranges>
concept CartesianProductIsSized = (sized_range<Ranges> && ...);

template <bool IsConst, template <class> class FirstSent, class First, class... Rest>
concept CartesianIsSizedSentinel =
    (sized_sentinel_for<FirstSent<maybe_const<IsConst, First>>, iterator_t<maybe_const<IsConst, First>>> && ...
        && (sized_range<maybe_const<IsConst, Rest>>
            && sized_sentinel_for<iterator_t<maybe_const<IsConst, Rest>>, iterator_t<maybe_const<IsConst, Rest>>>) );

template <class... Ranges>
concept CanViewCartesianProduct = requires(Ranges&&... rs) { views::cartesian_product(forward<Ranges>(rs)...); };

template <class T>
concept UnsignedIntegerLike = _Integer_like<T> && !_Signed_integer_like<T>;

template <class First, class... Rest>
constexpr bool is_iter_move_nothrow() {
    constexpr bool is_inner_iter_move_nothrow =
        (noexcept(ranges::iter_move(declval<const iterator_t<First>&>()))
            && ... //
                && noexcept(ranges::iter_move(declval<const iterator_t<Rest>&>())));
    constexpr bool are_references_nothrow_movable =
        conjunction_v<is_nothrow_move_constructible<ranges::range_rvalue_reference_t<First>>,
            is_nothrow_move_constructible<ranges::range_rvalue_reference_t<Rest>>...>;
    return is_inner_iter_move_nothrow && are_references_nothrow_movable;
}

template <class First, class... Rest>
constexpr bool is_iter_swap_nothrow() {
    return (noexcept(ranges::iter_swap(declval<const iterator_t<First>&>(), declval<const iterator_t<First>&>()))
            && ... //
                && noexcept(ranges::iter_swap(declval<const iterator_t<Rest>&>(), declval<const iterator_t<Rest>&>())));
}

template <class Expected, ranges::input_range First, ranges::forward_range... Rest>
constexpr bool test_one(Expected&& expected_range, First&& first, Rest&&... rest) {
    using ranges::cartesian_product_view, ranges::view, ranges::input_range, ranges::forward_range, ranges::range,
        ranges::range_value_t, ranges::range_reference_t, ranges::range_rvalue_reference_t, ranges::range_difference_t,
        ranges::sentinel_t, ranges::prev, ranges::const_iterator_t, ranges::const_sentinel_t;
    using views::all_t;

    using VFirst = all_t<First>;
    using R      = cartesian_product_view<VFirst, all_t<Rest>...>;

    constexpr bool is_view                = (view<remove_cvref_t<First>> && ... && view<remove_cvref_t<Rest>>);
    constexpr bool all_copy_constructible = (copy_constructible<VFirst> && ... && copy_constructible<all_t<Rest>>);
    constexpr bool is_bidirectional       = CartesianProductIsBidirectional<false, VFirst, all_t<Rest>...>;
    constexpr bool is_const_bidirectional = CartesianProductIsBidirectional<true, VFirst, all_t<Rest>...>;
    constexpr bool is_random_access       = CartesianProductIsRandomAccess<false, VFirst, all_t<Rest>...>;
    constexpr bool is_const_random_access = CartesianProductIsRandomAccess<true, VFirst, all_t<Rest>...>;
    constexpr bool is_sized               = CartesianProductIsSized<VFirst, all_t<Rest>...>;
    constexpr bool is_const_sized         = CartesianProductIsSized<const VFirst, const all_t<Rest>...>;
    constexpr bool is_common              = CartesianProductIsCommon<VFirst, all_t<Rest>...>;
    constexpr bool is_const_common        = CartesianProductIsCommon<const VFirst, const all_t<Rest>...>;

    STATIC_ASSERT(view<R>);
    STATIC_ASSERT(input_range<R>);
    STATIC_ASSERT(forward_range<R> == forward_range<First>);
    STATIC_ASSERT(bidirectional_range<R> == is_bidirectional);
    STATIC_ASSERT(random_access_range<R> == is_random_access);
    STATIC_ASSERT(!ranges::contiguous_range<R>);
    STATIC_ASSERT(sized_range<R> == is_sized);
    STATIC_ASSERT(common_range<R> == is_common);

    // Check non-default-initializability
    STATIC_ASSERT(is_default_constructible_v<R>
                  == (is_default_constructible_v<VFirst> && ... && is_default_constructible_v<all_t<Rest>>) );

    // Check borrowed_range
    static_assert(!ranges::borrowed_range<R>);

    // Check range closure object
    constexpr auto closure = views::cartesian_product;

    // ... with lvalue argument
    STATIC_ASSERT(CanViewCartesianProduct<First&, Rest&...>
                  == (!is_view || (copy_constructible<VFirst> && ... && copy_constructible<all_t<Rest>>) ));
    if constexpr (CanViewCartesianProduct<First&, Rest&...>) {
        constexpr bool is_noexcept =
            !is_view
            || (is_nothrow_copy_constructible_v<VFirst> && ... && is_nothrow_copy_constructible_v<all_t<Rest>>);

        STATIC_ASSERT(same_as<decltype(closure(first, rest...)), R>);
        STATIC_ASSERT(noexcept(closure(first, rest...)) == is_noexcept);
    }

    // ... with const lvalue argument
    STATIC_ASSERT(CanViewCartesianProduct<const remove_reference_t<First>&, const remove_reference_t<Rest>&...>
                  == (!is_view || (copy_constructible<VFirst> && ... && copy_constructible<all_t<Rest>>) ));
    if constexpr (CanViewCartesianProduct<const remove_reference_t<First>&, const remove_reference_t<Rest>&...>) {
        using RC =
            cartesian_product_view<all_t<const remove_reference_t<First>&>, all_t<const remove_reference_t<Rest>&>...>;
        constexpr bool is_noexcept =
            !is_view
            || (is_nothrow_copy_constructible_v<VFirst> && ... && is_nothrow_copy_constructible_v<all_t<Rest>>);

        STATIC_ASSERT(same_as<decltype(closure(as_const(first), as_const(rest)...)), RC>);
        STATIC_ASSERT(noexcept(closure(as_const(first), as_const(rest)...)) == is_noexcept);
    }

    // ... with rvalue argument
    STATIC_ASSERT(CanViewCartesianProduct<remove_reference_t<First>, remove_reference_t<Rest>...>
                  == (is_view || (movable<remove_reference_t<First>> && ... && movable<remove_reference_t<Rest>>) ));
    if constexpr (CanViewCartesianProduct<remove_reference_t<First>, remove_reference_t<Rest>...>) {
        using RS = cartesian_product_view<all_t<remove_reference_t<First>>, all_t<remove_reference_t<Rest>>...>;
        constexpr bool is_noexcept =
            (is_nothrow_move_constructible_v<VFirst> && ... && is_nothrow_move_constructible_v<all_t<Rest>>);

        STATIC_ASSERT(same_as<decltype(closure(move(first), move(rest)...)), RS>);
        STATIC_ASSERT(noexcept(closure(move(first), move(rest)...)) == is_noexcept);
    }

    // ... with const rvalue argument
    STATIC_ASSERT(CanViewCartesianProduct<const remove_reference_t<First>, const remove_reference_t<Rest>...>
                  == (is_view && (copy_constructible<VFirst> && ... && copy_constructible<all_t<Rest>>) ));
    if constexpr (CanViewCartesianProduct<const remove_reference_t<First>, const remove_reference_t<Rest>...>) {
        constexpr bool is_noexcept =
            (is_nothrow_copy_constructible_v<VFirst> && ... && is_nothrow_copy_constructible_v<all_t<Rest>>);

        STATIC_ASSERT(same_as<decltype(closure(move(as_const(first)), move(as_const(rest))...)), R>);
        STATIC_ASSERT(noexcept(closure(move(as_const(first)), move(as_const(rest))...)) == is_noexcept);
    }

    // Check deduction guide
    same_as<R> auto r = cartesian_product_view{forward<First>(first), forward<Rest>(rest)...};

    // Check cartesian_product_view::size
    STATIC_ASSERT(CanMemberSize<R> == is_sized);
    if constexpr (CanMemberSize<R>) {
        UnsignedIntegerLike auto s = r.size();
        assert(s == ranges::size(expected_range));
    }

    // Check cartesian_product_view::size (const)
    STATIC_ASSERT(CanMemberSize<const R> == is_const_sized);
    if constexpr (CanMemberSize<const R>) {
        UnsignedIntegerLike auto s = as_const(r).size();
        assert(s == ranges::size(expected_range));
    }

    const bool is_empty = ranges::empty(expected_range);

    // Check view_interface::empty and operator bool
    STATIC_ASSERT(CanMemberEmpty<R> == (forward_range<VFirst> || is_sized));
    STATIC_ASSERT(CanBool<R> == CanEmpty<R>);
    if constexpr (CanMemberEmpty<R>) {
        assert(r.empty() == is_empty);
        assert(static_cast<bool>(r) == !is_empty);
    }

    // Check view_interface::empty and operator bool (const)
    STATIC_ASSERT(CanMemberEmpty<const R> == (forward_range<const VFirst> || is_const_sized));
    STATIC_ASSERT(CanBool<const R> == CanEmpty<const R>);
    if constexpr (CanMemberEmpty<const R>) {
        assert(as_const(r).empty() == is_empty);
        assert(static_cast<bool>(as_const(r)) == !is_empty);
    }

    assert(ranges::equal(r, expected_range));
    if (!forward_range<VFirst>) { // intentionally not if constexpr
        return true;
    }

    // Check cartesian_product_view::begin
    STATIC_ASSERT(CanMemberBegin<R>);
    {
        const same_as<iterator_t<R>> auto i = r.begin();
        if (!is_empty) {
            assert(*i == *begin(expected_range));
        }

        if constexpr (all_copy_constructible) {
            auto r2                              = r;
            const same_as<iterator_t<R>> auto i2 = r2.begin();
            if (!is_empty) {
                assert(*i2 == *i);
            }
        }
    }

    // Check cartesian_product_view::begin (const)
    STATIC_ASSERT(CanMemberBegin<const R> == (range<const VFirst> && ... && range<const all_t<Rest>>) );
    if constexpr (CanMemberBegin<const R>) {
        const same_as<iterator_t<const R>> auto ci = as_const(r).begin();
        if (!is_empty) {
            assert(*ci == *begin(expected_range));
        }

        if constexpr (all_copy_constructible) {
            const auto r2                               = r;
            const same_as<iterator_t<const R>> auto ci2 = r2.begin();
            if (!is_empty) {
                assert(*ci2 == *ci);
            }
        }
    }

    // Check cartesian_product_view::end
    STATIC_ASSERT(CanMemberEnd<R>);
    {
        const same_as<sentinel_t<R>> auto s = r.end();
        assert((r.begin() == s) == is_empty);
        STATIC_ASSERT(common_range<R> == is_common);

        if constexpr (same_as<sentinel_t<R>, default_sentinel_t>) {
            STATIC_ASSERT(!is_common);
            STATIC_ASSERT(noexcept(r.end()));
        } else if constexpr (common_range<R> && is_bidirectional) {
            if (!is_empty) {
                assert(*prev(s) == *prev(end(expected_range)));
            }

            if constexpr (all_copy_constructible) {
                auto r2 = r;
                if (!is_empty) {
                    assert(*prev(r2.end()) == *prev(end(expected_range)));
                }
            }
        }
    }

    // Check cartesian_product_view::end (const)
    STATIC_ASSERT(CanMemberEnd<const R>);
    if constexpr (CanMemberEnd<const R>) {
        const same_as<sentinel_t<const R>> auto cs = as_const(r).end();
        assert((r.begin() == cs) == is_empty);
        STATIC_ASSERT(common_range<const R> == is_const_common);

        if constexpr (same_as<sentinel_t<const R>, default_sentinel_t>) {
            STATIC_ASSERT(!is_const_common);
            STATIC_ASSERT(noexcept(as_const(r).end()));
        } else if constexpr (common_range<const R> && is_const_bidirectional) {
            if (!is_empty) {
                assert(*prev(cs) == *prev(end(expected_range)));
            }

            if constexpr (all_copy_constructible) {
                const auto r2 = r;
                if (!is_empty) {
                    assert(*prev(r2.end()) == *prev(end(expected_range)));
                }
            }
        }
    }

    // Check view_interface::cbegin
    STATIC_ASSERT(CanMemberCBegin<R>);
    STATIC_ASSERT(CanMemberCBegin<const R&> == (range<const VFirst> && ... && range<const all_t<Rest>>) );
    {
        const same_as<const_iterator_t<R>> auto i = r.cbegin();
        if (!is_empty) {
            assert(*i == *cbegin(expected_range));
        }

        if constexpr (all_copy_constructible) {
            auto r2                                    = r;
            const same_as<const_iterator_t<R>> auto i2 = r2.cbegin();
            if (!is_empty) {
                assert(*i2 == *i);
            }
        }

        if constexpr (CanCBegin<const R&>) {
            const same_as<const_iterator_t<const R>> auto i3 = as_const(r).cbegin();
            if (!is_empty) {
                assert(*i3 == *i);
            }
        }
    }

    // Check view_interface::cend
    STATIC_ASSERT(CanMemberCEnd<R>);
    STATIC_ASSERT(CanMemberCEnd<const R&>);
    if (!is_empty) {
        same_as<const_sentinel_t<R>> auto i = r.cend();
        if constexpr (common_range<R> && is_bidirectional) {
            assert(*prev(i) == *prev(cend(expected_range)));
        }

        if constexpr (CanCEnd<const R&>) {
            same_as<const_sentinel_t<const R>> auto i2 = as_const(r).cend();
            if constexpr (common_range<const R&> && is_const_bidirectional) {
                assert(*prev(i2) == *prev(cend(expected_range)));
            }
        }
    }

    if (is_empty) {
        return true;
    }

    // Check view_interface::data
    STATIC_ASSERT(!CanData<R>);
    STATIC_ASSERT(!CanData<const R>);

    // Check view_interface::operator[]
    STATIC_ASSERT(CanIndex<R> == is_random_access);
    if constexpr (CanIndex<R>) {
        assert(r[0] == expected_range[0]);
    }

    // Check view_interface::operator[] (const)
    STATIC_ASSERT(CanIndex<const R> == is_const_random_access);
    if constexpr (CanIndex<const R>) {
        assert(as_const(r)[0] == expected_range[0]);
    }

    // Check view_interface::front
    STATIC_ASSERT(CanMemberFront<R> == forward_range<VFirst>);
    if constexpr (CanMemberFront<R>) {
        assert(r.front() == *begin(expected_range));
    }

    // Check view_interface::front (const)
    STATIC_ASSERT(CanMemberFront<const R> == forward_range<const VFirst>);
    if constexpr (CanMemberFront<const R>) {
        assert(as_const(r).front() == *begin(expected_range));
    }

    // Check view_interface::back
    STATIC_ASSERT(CanMemberBack<R> == (is_bidirectional && is_common));
    if constexpr (CanMemberBack<R>) {
        assert(r.back() == *prev(end(expected_range)));
    }

    // Check view_interface::back (const)
    STATIC_ASSERT(CanMemberBack<const R> == (is_const_bidirectional && is_const_common));
    if constexpr (CanMemberBack<const R>) {
        assert(as_const(r).back() == *prev(end(expected_range)));
    }

    { // Check cartesian_product_view::iterator<not const>
        using I = iterator_t<R>;
        STATIC_ASSERT(input_iterator<I>);

        // Check iterator_category
        STATIC_ASSERT(same_as<typename I::iterator_category, input_iterator_tag>);

        // Check iterator_concept
        using IterConcept = typename I::iterator_concept;
        STATIC_ASSERT(is_random_access == same_as<IterConcept, random_access_iterator_tag>);
        STATIC_ASSERT((is_bidirectional && !is_random_access) == same_as<IterConcept, bidirectional_iterator_tag>);
        STATIC_ASSERT((forward_range<VFirst> && !is_bidirectional) == same_as<IterConcept, forward_iterator_tag>);

        // Check value_type
        STATIC_ASSERT(same_as<typename I::value_type, tuple<range_value_t<First>, range_value_t<Rest>...>>);

        // Check default-initializability
        STATIC_ASSERT(default_initializable<I> == default_initializable<iterator_t<VFirst>>);

        auto i = r.begin();

        { // Check dereference
            same_as<tuple<range_reference_t<First>, range_reference_t<Rest>...>> decltype(auto) v = *as_const(i);
            assert(v == expected_range[0]);
        }

        { // Check pre-incrementation
            same_as<I&> decltype(auto) i2 = ++i;
            assert(&i2 == &i);
            if (i != r.end()) {
                assert(*i == expected_range[1]);
            }
            i = r.begin();
        }

        if constexpr (forward_range<VFirst>) { // Check post-incrementation
            same_as<I> decltype(auto) i2 = i++;
            assert(*i2 == expected_range[0]);
            if (i != r.end()) {
                assert(*i == expected_range[1]);
            }
            i = r.begin();
        } else {
            STATIC_ASSERT(is_void_v<decltype(i++)>);
        }

        if constexpr (is_bidirectional) {
            { // Check pre-decrementation
                i = ranges::next(r.begin());

                same_as<I&> decltype(auto) i2 = --i;
                assert(&i2 == &i);
                assert(*i2 == expected_range[0]);
            }

            { // Check post-decrementation
                i = ranges::next(r.begin());

                same_as<I> decltype(auto) i2 = i--;
                if (i2 != r.end()) {
                    assert(*i2 == expected_range[1]);
                }
                assert(*i == expected_range[0]);
            }
        }

        if constexpr (is_random_access) {
            const auto half_max_distance = ranges::distance(r) / 2;

            { // Check advancement operators
                same_as<I&> decltype(auto) i2 = (i += half_max_distance);
                assert(&i2 == &i);
                if (i != r.end()) {
                    assert(*i == expected_range[static_cast<size_t>(half_max_distance)]);
                }

                same_as<I&> decltype(auto) i3 = (i -= half_max_distance);
                assert(&i3 == &i);
                assert(*i == expected_range[0]);
            }

            { // Check subscript operator
                same_as<tuple<range_reference_t<First>, range_reference_t<Rest>...>> decltype(auto) v = i[0];
                assert(v == expected_range[0]);
            }

            if constexpr (equality_comparable<iterator_t<VFirst>>) { // Check equality comparisons
                auto i2               = r.begin();
                same_as<bool> auto b1 = i == i2;
                assert(b1);
                ++i2;
                same_as<bool> auto b2 = i != i2;
                assert(b2);
                same_as<bool> auto b3 = i2 != default_sentinel;
                assert(b3);
                ranges::advance(i2, r.end());
                same_as<bool> auto b4 = i2 == default_sentinel;
                assert(b4);
            }

            if constexpr ((random_access_range<VFirst> && ...
                              && random_access_range<all_t<Rest>>) ) { // Check 3way comparisons
                using Cat              = common_comparison_category_t<compare_three_way_result_t<iterator_t<VFirst>>,
                    compare_three_way_result_t<iterator_t<all_t<Rest>>>...>;
                auto i2                = r.begin();
                same_as<Cat> auto cmp1 = i <=> i2;
                assert(cmp1 == Cat::equivalent);
                ++i2;
                assert((i <=> i2) == Cat::less);
                assert((i2 <=> i) == Cat::greater);

                same_as<bool> auto b1 = i < i2;
                assert(b1);
                same_as<bool> auto b2 = i2 > i;
                assert(b2);
                same_as<bool> auto b3 = i <= i2;
                assert(b3);
                same_as<bool> auto b4 = i2 >= i;
                assert(b4);
            }

            { // Check operator+
                same_as<I> auto i2 = i + half_max_distance;
                if (i2 != r.end()) {
                    assert(*i2 == expected_range[static_cast<size_t>(half_max_distance)]);
                }

                same_as<I> auto i3 = half_max_distance + i;
                if (i3 != r.end()) {
                    assert(*i3 == expected_range[static_cast<size_t>(half_max_distance)]);
                }
            }

            { // Check operator-(Iter, Diff)
                same_as<I> auto i2 = (i + half_max_distance) - half_max_distance;
                assert(*i2 == expected_range[0]);
            }
        }

        if constexpr (CartesianIsSizedSentinel<false, iterator_t, VFirst, all_t<Rest>...>) { // Check differencing
            _Signed_integer_like auto diff = i - i;
            assert(diff == 0);
            assert(i - ranges::next(i) == -1);
            assert(ranges::next(i) - i == 1);
        }

        STATIC_ASSERT(sized_sentinel_for<default_sentinel_t, I>
                      == CartesianIsSizedSentinel<false, sentinel_t, VFirst, all_t<Rest>...>);
        if constexpr (sized_sentinel_for<default_sentinel_t, I>) { // Check differencing with default_sentinel
            const auto expected_size              = ranges::ssize(expected_range);
            const _Signed_integer_like auto diff1 = i - default_sentinel;
            assert(diff1 == -expected_size);
            const _Signed_integer_like auto diff2 = default_sentinel - i;
            assert(diff2 == expected_size);
        }

        { // Check iter_move (hidden friend available via ADL)
            same_as<tuple<range_rvalue_reference_t<VFirst>, range_rvalue_reference_t<all_t<Rest>>...>> decltype(auto)
                rval = iter_move(as_const(i));
            assert(rval == expected_range[0]);
            static_assert(noexcept(iter_move(i)) == is_iter_move_nothrow<VFirst, all_t<Rest>...>());
        }

        if constexpr ((indirectly_swappable<iterator_t<VFirst>> && ...
                          && indirectly_swappable<iterator_t<all_t<Rest>>>) ) {
            // Check iter_swap, other tests are defined in test_iter_swap function
            static_assert(is_void_v<decltype(iter_swap(as_const(i), as_const(i)))>);
            static_assert(noexcept(iter_swap(i, i)) == is_iter_swap_nothrow<VFirst, all_t<Rest>...>());
        }
    }

    // Check cartesian_product_view::iterator<const>
    if constexpr (CanMemberBegin<const R>) {
        using CI = iterator_t<const R>;
        STATIC_ASSERT(input_iterator<CI>);

        // Check iterator_category
        STATIC_ASSERT(same_as<typename CI::iterator_category, input_iterator_tag>);

        // Check iterator_concept
        using IterConcept = typename CI::iterator_concept;
        STATIC_ASSERT(is_const_random_access == same_as<IterConcept, random_access_iterator_tag>);
        STATIC_ASSERT(
            (is_const_bidirectional && !is_const_random_access) == same_as<IterConcept, bidirectional_iterator_tag>);
        STATIC_ASSERT(
            (forward_range<const VFirst> && !is_const_bidirectional) == same_as<IterConcept, forward_iterator_tag>);

        // Check value_type
        static_assert(
            same_as<typename CI::value_type, tuple<range_value_t<const First>, range_value_t<const Rest>...>>);

        // Check default-initializability
        STATIC_ASSERT(default_initializable<CI> == default_initializable<iterator_t<const VFirst>>);

        // Check conversion from non-const iterator
        if constexpr ((convertible_to<iterator_t<VFirst>, iterator_t<const VFirst>> && ...
                          && convertible_to<iterator_t<Rest>, iterator_t<const Rest>>) ) {
            auto i = r.begin();
            [[maybe_unused]] CI ci{move(i)};
        }

        auto i = r.begin();
        CI ci  = as_const(r).begin();

        { // Check dereference
            same_as<tuple<range_reference_t<const First>, range_reference_t<const Rest>...>> decltype(auto) v =
                *as_const(ci);
            assert(v == expected_range[0]);
        }

        { // Check pre-incrementation
            same_as<CI&> decltype(auto) ci2 = ++ci;
            assert(&ci2 == &ci);
            if (ci != as_const(r).end()) {
                assert(*ci == expected_range[1]);
            }
            ci = as_const(r).begin();
        }

        if constexpr (forward_range<const VFirst>) { // Check post-incrementation
            same_as<CI> decltype(auto) ci2 = ci++;
            assert(*ci2 == expected_range[0]);
            if (ci != as_const(r).end()) {
                assert(*ci == expected_range[1]);
            }
            ci = as_const(r).begin();
        } else {
            STATIC_ASSERT(is_void_v<decltype(ci++)>);
        }

        if constexpr (is_const_bidirectional) {
            { // Check pre-decrementation
                ci = ranges::next(r.begin());

                same_as<CI&> decltype(auto) ci2 = --ci;
                assert(&ci2 == &ci);
                assert(*ci2 == expected_range[0]);
            }

            { // Check post-decrementation
                ci = ranges::next(r.begin());

                same_as<CI> decltype(auto) ci2 = ci--;
                if (ci2 != r.end()) {
                    assert(*ci2 == expected_range[1]);
                }
                assert(*ci == expected_range[0]);
            }
        }

        if constexpr (is_const_random_access) {
            const auto half_max_distance = ranges::distance(r) / 2;

            { // Check advancement operators
                same_as<CI&> decltype(auto) ci2 = (ci += half_max_distance);
                assert(&ci2 == &ci);
                if (ci != r.end()) {
                    assert(*ci == expected_range[static_cast<size_t>(half_max_distance)]);
                }

                same_as<CI&> decltype(auto) ci3 = (ci -= half_max_distance);
                assert(&ci3 == &ci);
                assert(*ci == expected_range[0]);
            }

            { // Check subscript operator
                same_as<tuple<range_reference_t<const First>, range_reference_t<const Rest>...>> decltype(auto) v =
                    ci[0];
                assert(v == expected_range[0]);
            }

            if constexpr (equality_comparable<iterator_t<const VFirst>>) { // Check equality comparisons
                CI ci2                = as_const(r).begin();
                same_as<bool> auto b1 = ci == ci2;
                assert(b1);
                ++ci2;
                same_as<bool> auto b2 = ci != ci2;
                assert(b2);
                same_as<bool> auto b3 = ci2 != default_sentinel;
                assert(b3);
                ranges::advance(ci2, r.end());
                same_as<bool> auto b4 = ci2 == default_sentinel;
                assert(b4);
            }

            if constexpr (equality_comparable_with<iterator_t<const VFirst>,
                              iterator_t<VFirst>>) { // Check equality comparisons (mixed)
                CI ci2                = as_const(r).begin();
                same_as<bool> auto b1 = i == ci2;
                assert(b1);
                ++ci2;
                same_as<bool> auto b2 = i != ci2;
                assert(b2);
            }

            if constexpr ((random_access_range<const VFirst> && ...
                              && random_access_range<const all_t<Rest>>) ) { // Check 3way comparisons
                using Cat = common_comparison_category_t<compare_three_way_result_t<iterator_t<const VFirst>>,
                    compare_three_way_result_t<iterator_t<const all_t<Rest>>>...>;
                CI ci2    = as_const(r).begin();
                same_as<Cat> auto cmp1 = ci <=> ci2;
                assert(cmp1 == Cat::equivalent);
                ++ci2;
                assert((ci <=> ci2) == Cat::less);
                assert((ci2 <=> ci) == Cat::greater);

                same_as<bool> auto b1 = ci < ci2;
                assert(b1);
                same_as<bool> auto b2 = ci2 > ci;
                assert(b2);
                same_as<bool> auto b3 = ci <= ci2;
                assert(b3);
                same_as<bool> auto b4 = ci2 >= ci;
                assert(b4);
            }

            if constexpr ((random_access_range<const VFirst> && ...
                              && random_access_range<const all_t<Rest>>) ) { // Check 3way comparisons (mixed)
                using Cat = common_comparison_category_t<compare_three_way_result_t<iterator_t<const VFirst>>,
                    compare_three_way_result_t<iterator_t<const all_t<Rest>>>...>;
                CI ci2    = as_const(r).begin();
                same_as<Cat> auto cmp1 = i <=> ci2;
                assert(cmp1 == Cat::equivalent);
                ++ci2;
                assert((i <=> ci2) == Cat::less);
                assert((ci2 <=> i) == Cat::greater);

                same_as<bool> auto b1 = i < ci2;
                assert(b1);
                same_as<bool> auto b2 = ci2 > i;
                assert(b2);
                same_as<bool> auto b3 = i <= ci2;
                assert(b3);
                same_as<bool> auto b4 = ci2 >= i;
                assert(b4);
            }

            { // Check operator+
                same_as<CI> auto ci2 = ci + 1;
                if (ci2 != r.end()) {
                    assert(*ci2 == expected_range[1]);
                }

                same_as<CI> auto ci3 = 1 + ci;
                if (ci3 != r.end()) {
                    assert(*ci3 == expected_range[1]);
                }
            }

            { // Check operator-(Iter, Diff)
                same_as<CI> auto ci2 = ranges::next(ci) - 1;
                assert(*ci2 == expected_range[0]);
            }
        }

        if constexpr (CartesianIsSizedSentinel<false, iterator_t, const VFirst,
                          const all_t<Rest>...>) { // Check differencing
            _Signed_integer_like auto diff = ci - ci;
            assert(diff == 0);
            assert(ci - ranges::next(ci) == -1);
            assert(ranges::next(ci) - ci == 1);
        }

        STATIC_ASSERT(sized_sentinel_for<default_sentinel_t, CI>
                      == CartesianIsSizedSentinel<false, sentinel_t, const VFirst, const all_t<Rest>...>);
        if constexpr (sized_sentinel_for<default_sentinel_t, CI>) { // Check differencing with default_sentinel
            const auto expected_size              = ranges::ssize(expected_range);
            const _Signed_integer_like auto diff1 = ci - default_sentinel;
            assert(diff1 == -expected_size);
            const _Signed_integer_like auto diff2 = default_sentinel - ci;
            assert(diff2 == expected_size);
        }

        { // Check iter_move
            same_as<tuple<range_rvalue_reference_t<const VFirst>,
                range_rvalue_reference_t<const all_t<Rest>>...>> decltype(auto) rval = iter_move(as_const(ci));
            assert(rval == expected_range[0]);
            static_assert(noexcept(iter_move(ci)) == is_iter_move_nothrow<const VFirst, const all_t<Rest>...>());
        }

        if constexpr ((indirectly_swappable<iterator_t<const VFirst>> && ...
                          && indirectly_swappable<iterator_t<const all_t<Rest>>>) ) {
            // Check iter_swap, other tests are defined in test_iter_swap function
            static_assert(is_void_v<decltype(iter_swap(as_const(ci), as_const(ci)))>);
            static_assert(noexcept(iter_swap(ci, ci)) == is_iter_swap_nothrow<const VFirst, const all_t<Rest>...>());
        }
    }

    return true;
}

// Check calling views::cartesian_product without arguments
STATIC_ASSERT(same_as<decltype(views::cartesian_product()), decltype(views::single(tuple{}))>);

template <ranges::input_range... Rngs>
    requires (indirectly_swappable<iterator_t<Rngs>> && ...)
constexpr void test_iter_swap(Rngs&... rngs) {
    // This test assumes that 'ranges::size(rng)' is at least 2 for each rng in rngs
    auto r    = views::cartesian_product(rngs...);
    using R   = decltype(r);
    using Val = ranges::range_value_t<R>;

    { // Check iter_swap for cartesian_product_view::iterator<not const>
        auto i     = r.begin();
        Val first  = *i;
        auto j     = ranges::next(i);
        Val second = *j;

        iter_swap(i, j);
        assert(*i == second);
        assert(*j == first);

        ranges::iter_swap(i, j);
        assert(*i == first);
        assert(*j == second);

        static_assert(noexcept(iter_swap(i, j)) == is_iter_swap_nothrow<Rngs&...>());
    }

    // Check iter_swap for cartesian_product_view::iterator<const>
    if constexpr (((CanMemberBegin<const Rngs&> && indirectly_swappable<iterator_t<const Rngs&>>) &&...)) {
        using CVal  = ranges::range_value_t<const R>;
        auto i      = as_const(r).begin();
        CVal first  = *i;
        auto j      = ranges::next(i);
        CVal second = *j;

        iter_swap(i, j);
        assert(*i == second);
        assert(*j == first);

        ranges::iter_swap(i, j);
        assert(*i == first);
        assert(*j == second);

        static_assert(noexcept(iter_swap(i, j)) == is_iter_swap_nothrow<const Rngs&...>());
    }
}

constexpr tuple some_ranges = {
    array{0, 1, 2, 3, 4},
    array{11, 22, 33},
    array{'7'},
    array{"4"sv, "2"sv, "0"sv},
};

// Expected result of views::cartesian_product(get<0>(some_ranges))
constexpr array<tuple<int>, 5> expected_result_0{{{0}, {1}, {2}, {3}, {4}}};

// Expected result of views::cartesian_product(get<0>(some_ranges), get<1>(some_ranges))
constexpr array<tuple<int, int>, 15> expected_result_1{{{0, 11}, {0, 22}, {0, 33}, {1, 11}, {1, 22}, {1, 33}, {2, 11},
    {2, 22}, {2, 33}, {3, 11}, {3, 22}, {3, 33}, {4, 11}, {4, 22}, {4, 33}}};

// Expected result of views::cartesian_product(get<0>(some_ranges), ..., get<2>(some_ranges))
constexpr array<tuple<int, int, char>, 15> expected_result_2{
    {{0, 11, '7'}, {0, 22, '7'}, {0, 33, '7'}, {1, 11, '7'}, {1, 22, '7'}, {1, 33, '7'}, {2, 11, '7'}, {2, 22, '7'},
        {2, 33, '7'}, {3, 11, '7'}, {3, 22, '7'}, {3, 33, '7'}, {4, 11, '7'}, {4, 22, '7'}, {4, 33, '7'}}};

// Expected result of views::cartesian_product(get<0>(some_ranges), ..., get<3>(some_ranges))
constexpr array<tuple<int, int, char, string_view>, 45> expected_result_3{
    {{0, 11, '7', "4"sv}, {0, 11, '7', "2"sv}, {0, 11, '7', "0"sv}, {0, 22, '7', "4"sv}, {0, 22, '7', "2"sv},
        {0, 22, '7', "0"sv}, {0, 33, '7', "4"sv}, {0, 33, '7', "2"sv}, {0, 33, '7', "0"sv}, {1, 11, '7', "4"sv},
        {1, 11, '7', "2"sv}, {1, 11, '7', "0"sv}, {1, 22, '7', "4"sv}, {1, 22, '7', "2"sv}, {1, 22, '7', "0"sv},
        {1, 33, '7', "4"sv}, {1, 33, '7', "2"sv}, {1, 33, '7', "0"sv}, {2, 11, '7', "4"sv}, {2, 11, '7', "2"sv},
        {2, 11, '7', "0"sv}, {2, 22, '7', "4"sv}, {2, 22, '7', "2"sv}, {2, 22, '7', "0"sv}, {2, 33, '7', "4"sv},
        {2, 33, '7', "2"sv}, {2, 33, '7', "0"sv}, {3, 11, '7', "4"sv}, {3, 11, '7', "2"sv}, {3, 11, '7', "0"sv},
        {3, 22, '7', "4"sv}, {3, 22, '7', "2"sv}, {3, 22, '7', "0"sv}, {3, 33, '7', "4"sv}, {3, 33, '7', "2"sv},
        {3, 33, '7', "0"sv}, {4, 11, '7', "4"sv}, {4, 11, '7', "2"sv}, {4, 11, '7', "0"sv}, {4, 22, '7', "4"sv},
        {4, 22, '7', "2"sv}, {4, 22, '7', "0"sv}, {4, 33, '7', "4"sv}, {4, 33, '7', "2"sv}, {4, 33, '7', "0"sv}}};

template <test::CanDifference Difference>
struct test_input_range {
    template <class ValTy>
    using type = test::range<input_iterator_tag, ValTy, test::Sized::yes, Difference, test::Common::yes,
        test::CanCompare::yes, test::ProxyRef::no>;
};

template <class Category, test::Common IsCommon, test::Sized IsSized>
struct test_range {
    template <class ValTy>
    using type =
        test::range<Category, ValTy, IsSized, test::CanDifference{derived_from<Category, random_access_iterator_tag>},
            IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
            test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}>;
};

struct instantiator {
    template <class R>
    static constexpr void call() {
        typename R::template type<const int> r0{get<0>(some_ranges)};
        test_one(expected_result_0, r0);

        if constexpr (ranges::forward_range<typename R::template type<const int>>) {
            typename R::template type<const int> r1{get<1>(some_ranges)};
            test_one(expected_result_1, r0, r1);

#if !(defined(__clang__) && defined(_DEBUG) && !defined(_DLL)) // constexpr limit
            typename R::template type<const char> r2{get<2>(some_ranges)};
            test_one(expected_result_2, r0, r1, r2);
#endif // "Clang /MTd" configuration

            int swap_a1[] = {1, 2, 3};
            typename R::template type<int> swap_r1{swap_a1};
            int swap_a2[] = {9, 8, 7};
            typename R::template type<int> swap_r2{swap_a2};
            test_iter_swap(swap_r1, swap_r2);
        }
    }
};

constexpr void instantiation_test() {
    // The cartesian_product_view is sensitive to category, commonality, and size, but oblivious to
    // differencing and proxyness.
    using test::Common, test::Sized, test::CanDifference;

    // When the base range is an input range, the view is sensitive to differencing
    instantiator::call<test_input_range<CanDifference::yes>>();
    instantiator::call<test_input_range<CanDifference::no>>();

    instantiator::call<test_range<input_iterator_tag, Common::no, Sized::yes>>();
    instantiator::call<test_range<input_iterator_tag, Common::no, Sized::no>>();
    instantiator::call<test_range<input_iterator_tag, Common::yes, Sized::yes>>();
    instantiator::call<test_range<input_iterator_tag, Common::yes, Sized::no>>();

    instantiator::call<test_range<forward_iterator_tag, Common::no, Sized::yes>>();
    instantiator::call<test_range<forward_iterator_tag, Common::no, Sized::no>>();
    instantiator::call<test_range<forward_iterator_tag, Common::yes, Sized::yes>>();
    instantiator::call<test_range<forward_iterator_tag, Common::yes, Sized::no>>();

    instantiator::call<test_range<bidirectional_iterator_tag, Common::no, Sized::yes>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Common::no, Sized::no>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Common::yes, Sized::yes>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Common::yes, Sized::no>>();

    instantiator::call<test_range<random_access_iterator_tag, Common::no, Sized::yes>>();
    instantiator::call<test_range<random_access_iterator_tag, Common::no, Sized::no>>();
    instantiator::call<test_range<random_access_iterator_tag, Common::yes, Sized::yes>>();
    instantiator::call<test_range<random_access_iterator_tag, Common::yes, Sized::no>>();

    instantiator::call<test_range<contiguous_iterator_tag, Common::no, Sized::yes>>();
    instantiator::call<test_range<contiguous_iterator_tag, Common::no, Sized::no>>();
    instantiator::call<test_range<contiguous_iterator_tag, Common::yes, Sized::yes>>();
    instantiator::call<test_range<contiguous_iterator_tag, Common::yes, Sized::no>>();
}

template <class ValTy, class Category, test::Common IsCommon, test::Sized IsSized,
    bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const ValTy, IsSized, test::CanDifference{is_random}, IsCommon,
    test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

namespace check_recommended_practice_implementation { // MSVC STL specific behavior
    using ranges::cartesian_product_view, ranges::empty_view, ranges::single_view, views::all_t, ranges::range_size_t,
        ranges::range_difference_t, ranges::ref_view, ranges::owning_view;
    using Arr  = array<int, 4>;
    using Vec  = vector<int>;
    using Span = span<int, 4>;

    // Computing product for such small array does not require big range_size_t
    STATIC_ASSERT(sizeof(range_size_t<cartesian_product_view<all_t<Arr>>>) <= sizeof(size_t));
    STATIC_ASSERT(sizeof(range_size_t<cartesian_product_view<all_t<Arr>, all_t<Arr>>>) <= sizeof(size_t));
    STATIC_ASSERT(sizeof(range_size_t<cartesian_product_view<all_t<Arr>, all_t<Arr>, all_t<Arr>>>) <= sizeof(size_t));

    // Same thing with range_difference_t<array>
    STATIC_ASSERT(sizeof(range_difference_t<cartesian_product_view<all_t<Arr>>>) <= sizeof(ptrdiff_t));
    STATIC_ASSERT(sizeof(range_difference_t<cartesian_product_view<all_t<Arr>, all_t<Arr>>>) <= sizeof(ptrdiff_t));
    STATIC_ASSERT(
        sizeof(range_difference_t<cartesian_product_view<all_t<Arr>, all_t<Arr>, all_t<Arr>>>) <= sizeof(ptrdiff_t));

    // Computing product for such small span does not require big range_size_t
    STATIC_ASSERT(sizeof(range_size_t<cartesian_product_view<all_t<Span>>>) <= sizeof(size_t));
    STATIC_ASSERT(sizeof(range_size_t<cartesian_product_view<all_t<Span>, all_t<Span>>>) <= sizeof(size_t));
    STATIC_ASSERT(
        sizeof(range_size_t<cartesian_product_view<all_t<Span>, all_t<Span>, all_t<Span>>>) <= sizeof(size_t));

    // Same thing with range_difference_t<span>
    STATIC_ASSERT(sizeof(range_difference_t<cartesian_product_view<all_t<Span>>>) <= sizeof(ptrdiff_t));
    STATIC_ASSERT(sizeof(range_difference_t<cartesian_product_view<all_t<Span>, all_t<Span>>>) <= sizeof(ptrdiff_t));
    STATIC_ASSERT(
        sizeof(range_difference_t<cartesian_product_view<all_t<Span>, all_t<Span>, all_t<Span>>>) <= sizeof(ptrdiff_t));

    // Check 'single_view' and 'empty_view'
    STATIC_ASSERT(sizeof(range_size_t<cartesian_product_view<empty_view<int>, single_view<int>>>) <= sizeof(size_t));
    STATIC_ASSERT(
        sizeof(range_difference_t<cartesian_product_view<empty_view<int>, single_view<int>>>) <= sizeof(ptrdiff_t));

    // Check 'ref_view<(const) V>' and 'owning_view<V>'
    STATIC_ASSERT(sizeof(range_size_t<cartesian_product_view<ref_view<Arr>, ref_view<const Arr>, owning_view<Arr>>>)
                  <= sizeof(size_t));
    STATIC_ASSERT(
        sizeof(range_difference_t<cartesian_product_view<ref_view<Arr>, ref_view<const Arr>, owning_view<Arr>>>)
        <= sizeof(ptrdiff_t));

    // One vector should not use big integer-class type...
    STATIC_ASSERT(sizeof(range_size_t<cartesian_product_view<all_t<Vec>>>) <= sizeof(size_t));
    STATIC_ASSERT(sizeof(range_difference_t<cartesian_product_view<all_t<Vec>>>) <= sizeof(ptrdiff_t));

    // ... but two vectors will
    STATIC_ASSERT(sizeof(range_size_t<cartesian_product_view<all_t<Vec>, all_t<Vec>>>) > sizeof(size_t));
    STATIC_ASSERT(sizeof(range_difference_t<cartesian_product_view<all_t<Vec>, all_t<Vec>>>) > sizeof(ptrdiff_t));
} // namespace check_recommended_practice_implementation

// GH-3733: cartesian_product_view would incorrectly reject a call to size() claiming that big*big*big*0 is not
// representable as range_size_t because big*big*big is not.
constexpr void test_gh_3733() {
    const auto r1   = views::repeat(0, (numeric_limits<ptrdiff_t>::max)());
    const auto r2   = views::repeat(1, 0);
    const auto cart = views::cartesian_product(r1, r1, r1, r2);
    assert(cart.size() == 0);
    assert(as_const(cart).size() == 0);
}

int main() {
    // Check views
    { // ... copyable
        constexpr span<const int> s{get<0>(some_ranges)};
        STATIC_ASSERT(test_one(expected_result_0, s));
        test_one(expected_result_0, s);
    }

    { // ... modifiable elements (so iterators are indirectly swappable)
        auto arr = get<0>(some_ranges);
        span<int> s{arr};
        test_one(expected_result_0, s);
    }

    { // ... move-only
        using test::Common, test::Sized;
        test_one(expected_result_3, //
            move_only_view<int, input_iterator_tag, Common::no, Sized::yes>{get<0>(some_ranges)},
            move_only_view<int, forward_iterator_tag, Common::no, Sized::yes>{get<1>(some_ranges)},
            move_only_view<char, bidirectional_iterator_tag, Common::no, Sized::yes>{get<2>(some_ranges)},
            move_only_view<string_view, random_access_iterator_tag, Common::no, Sized::yes>{get<3>(some_ranges)});
        test_one(expected_result_3, //
            move_only_view<int, input_iterator_tag, Common::no, Sized::no>{get<0>(some_ranges)},
            move_only_view<int, forward_iterator_tag, Common::no, Sized::no>{get<1>(some_ranges)},
            move_only_view<char, bidirectional_iterator_tag, Common::no, Sized::no>{get<2>(some_ranges)},
            move_only_view<string_view, random_access_iterator_tag, Common::no, Sized::no>{get<3>(some_ranges)});
        test_one(expected_result_2, //
            move_only_view<int, forward_iterator_tag, Common::yes, Sized::yes>{get<0>(some_ranges)},
            move_only_view<int, bidirectional_iterator_tag, Common::yes, Sized::yes>{get<1>(some_ranges)},
            move_only_view<char, random_access_iterator_tag, Common::yes, Sized::yes>{get<2>(some_ranges)});
        test_one(expected_result_2, //
            move_only_view<int, forward_iterator_tag, Common::yes, Sized::no>{get<0>(some_ranges)},
            move_only_view<int, bidirectional_iterator_tag, Common::yes, Sized::no>{get<1>(some_ranges)},
            move_only_view<char, random_access_iterator_tag, Common::yes, Sized::no>{get<2>(some_ranges)});
    }

    // Check non-views
    {
        constexpr auto& r0 = get<0>(some_ranges);
        STATIC_ASSERT(test_one(expected_result_0, r0));
        test_one(expected_result_0, r0);

        auto r1 = get<1>(some_ranges) | ranges::to<vector>();
        test_one(expected_result_1, r0, r1);

        auto r2 = get<2>(some_ranges) | ranges::to<forward_list>();
        test_one(expected_result_2, r0, r1, r2);

        auto r3 = get<3>(some_ranges) | ranges::to<list>();
        test_one(expected_result_3, r0, r1, r2, r3);
    }

#ifndef _PREFAST_ // TRANSITION, GH-1030
    STATIC_ASSERT((instantiation_test(), true));
#endif // TRANSITION, GH-1030
    instantiation_test();

    STATIC_ASSERT((test_gh_3733(), true));
    test_gh_3733();
}
