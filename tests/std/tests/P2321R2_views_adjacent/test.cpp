// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <forward_list>
#include <ranges>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

template <class R, size_t N>
concept CanViewAdjacent = requires(R&& r) { views::adjacent<N>(forward<R>(r)); };

template <class R, size_t N>
concept CanConstructAdjacentView = requires(R&& r) { ranges::adjacent_view<views::all_t<R>, N>{forward<R>(r)}; };

template <class T, class Indices>
struct repeated_tuple_impl;

template <class T, size_t... Indices>
struct repeated_tuple_impl<T, index_sequence<Indices...>> {
    template <size_t>
    using repeat_type = T;

    using type = tuple<repeat_type<Indices>...>;
};

template <class T, size_t N>
using repeated_tuple = repeated_tuple_impl<T, make_index_sequence<N>>::type;

static_assert(same_as<repeated_tuple<int, 0>, tuple<>>);
static_assert(same_as<repeated_tuple<int, 3>, tuple<int, int, int>>);
static_assert(same_as<repeated_tuple<int, 5>, tuple<int, int, int, int, int>>);

// Check views::pairwise
static_assert(same_as<decltype(views::pairwise), decltype(views::adjacent<2>)>);

template <size_t N, ranges::forward_range Rng, class Expected>
constexpr bool test_one(Rng&& rng, Expected&& expected) {
    using ranges::adjacent_view, ranges::forward_range, ranges::bidirectional_range, ranges::random_access_range,
        ranges::sized_range, ranges::common_range, ranges::iterator_t, ranges::sentinel_t, ranges::const_iterator_t,
        ranges::const_sentinel_t, ranges::range_difference_t, ranges::range_value_t, ranges::range_reference_t;

    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;

    using V = views::all_t<Rng>;
    using R = adjacent_view<V, N>;

    static_assert(ranges::view<R>);
    static_assert(forward_range<R>);
    static_assert(bidirectional_range<R> == bidirectional_range<Rng>);
    static_assert(random_access_range<R> == random_access_range<Rng>);
    static_assert(!ranges::contiguous_range<R>);

    // Check default-initializability
    static_assert(default_initializable<R> == default_initializable<V>);

    // Check borrowed_range
    static_assert(ranges::borrowed_range<R> == ranges::borrowed_range<V>);

    // Check range closure object
    constexpr auto closure = views::adjacent<N>;

    // ... with lvalue argument
    static_assert(CanViewAdjacent<Rng&, N> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewAdjacent<Rng&, N>) {
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<decltype(closure(rng)), R>);
        static_assert(noexcept(closure(rng)) == is_noexcept);

        static_assert(same_as<decltype(rng | closure), R>);
        static_assert(noexcept(rng | closure) == is_noexcept);
    }

    // ... with const lvalue argument
    static_assert(CanViewAdjacent<const remove_reference_t<Rng>&, N> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewAdjacent<const remove_reference_t<Rng>&, N>) {
        using RC                   = adjacent_view<views::all_t<const remove_reference_t<Rng>&>, N>;
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<decltype(closure(as_const(rng))), RC>);
        static_assert(noexcept(closure(as_const(rng))) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | closure), RC>);
        static_assert(noexcept(as_const(rng) | closure) == is_noexcept);
    }

    // ... with rvalue argument
    static_assert(CanViewAdjacent<remove_reference_t<Rng>, N> == (is_view || movable<remove_reference_t<Rng>>) );
    if constexpr (CanViewAdjacent<remove_reference_t<Rng>, N>) {
        using RS                   = adjacent_view<views::all_t<remove_reference_t<Rng>>, N>;
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;

        static_assert(same_as<decltype(closure(move(rng))), RS>);
        static_assert(noexcept(closure(move(rng))) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | closure), RS>);
        static_assert(noexcept(move(rng) | closure) == is_noexcept);
    }

    // ... with const rvalue argument
    static_assert(CanViewAdjacent<const remove_reference_t<Rng>, N> == (is_view && copy_constructible<V>) );
    if constexpr (CanViewAdjacent<const remove_reference_t<Rng>, N>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<decltype(closure(move(as_const(rng)))), R>);
        static_assert(noexcept(closure(move(as_const(rng)))) == is_noexcept);

        static_assert(same_as<decltype(move(as_const(rng)) | closure), R>);
        static_assert(noexcept(move(as_const(rng)) | closure) == is_noexcept);
    }

    R r{forward<Rng>(rng)};

    // Check adjacent_view::size
    static_assert(CanMemberSize<R> == sized_range<V>);
    if constexpr (CanMemberSize<R>) {
        same_as<ranges::range_size_t<V>> auto s = r.size();
        assert(_To_unsigned_like(s) == ranges::size(expected));
        static_assert(noexcept(r.size()) == noexcept(ranges::size(rng))); // strengthened
    }

    // Check adjacent_view::size (const)
    static_assert(CanMemberSize<const R> == sized_range<const V>);
    if constexpr (CanMemberSize<const R>) {
        same_as<ranges::range_size_t<const V>> auto s = as_const(r).size();
        assert(_To_unsigned_like(s) == ranges::size(expected));
        static_assert(noexcept(as_const(r).size()) == noexcept(ranges::size(as_const(rng)))); // strengthened
    }

    const bool is_empty = ranges::empty(expected);

    // Check view_interface::empty and operator bool
    static_assert(CanMemberEmpty<R>);
    static_assert(CanBool<R>);
    assert(r.empty() == is_empty);
    assert(static_cast<bool>(r) == !is_empty);

    // Check view_interface::empty and operator bool (const)
    static_assert(CanMemberEmpty<const R>);
    static_assert(CanBool<const R>);
    assert(as_const(r).empty() == is_empty);
    assert(static_cast<bool>(as_const(r)) == !is_empty);

    // Check content
    assert(ranges::equal(r, expected));

    // Check adjacent_view::begin
    static_assert(CanMemberBegin<R>);
    {
        const same_as<iterator_t<R>> auto i = r.begin();
        if (!is_empty) {
            assert(*i == *begin(expected));
        }

        if constexpr (copy_constructible<V>) {
            auto r2                              = r;
            const same_as<iterator_t<R>> auto i2 = r2.begin();
            if (!is_empty) {
                assert(*i2 == *i);
            }
        }
    }

    // Check adjacent_view::begin (const)
    static_assert(CanMemberBegin<const R> == ranges::range<const V>);
    if constexpr (CanMemberBegin<const R>) {
        const same_as<iterator_t<const R>> auto ci = as_const(r).begin();
        if (!is_empty) {
            assert(*ci == *begin(expected));
        }

        if constexpr (copy_constructible<V>) {
            const auto cr2                              = r;
            const same_as<iterator_t<const R>> auto ci2 = cr2.begin();
            if (!is_empty) {
                assert(*ci2 == *ci);
            }
        }
    }

    // Check adjacent_view::end
    static_assert(CanMemberEnd<R>);
    {
        const same_as<sentinel_t<R>> auto s = r.end();
        assert((r.begin() == s) == is_empty);

        if constexpr (sentinel_for<sentinel_t<R>, iterator_t<const R>>) {
            assert((as_const(r).begin() == s) == is_empty);
        }

        static_assert(common_range<R> == common_range<V>);
        if constexpr (common_range<V> && bidirectional_range<V>) {
            if (!is_empty) {
                assert(*prev(s) == *prev(end(expected)));
            }

            if constexpr (copy_constructible<V>) {
                auto r2 = r;
                if (!is_empty) {
                    assert(*prev(r2.end()) == *prev(end(expected)));
                }
            }
        }
    }

    // Check adjacent_view::end (const)
    static_assert(CanMemberEnd<const R> == ranges::range<const V>);
    if constexpr (CanMemberEnd<const R>) {
        const same_as<sentinel_t<const R>> auto cs = as_const(r).end();
        assert((as_const(r).begin() == cs) == is_empty);

        if constexpr (sentinel_for<sentinel_t<const R>, iterator_t<R>>) {
            assert((r.begin() == cs) == is_empty);
        }

        static_assert(common_range<const R> == common_range<const V>);
        if constexpr (common_range<const V> && bidirectional_range<const V>) {
            if (!is_empty) {
                assert(*prev(cs) == *prev(end(expected)));
            }

            if constexpr (copy_constructible<V>) {
                const auto r2 = r;
                if (!is_empty) {
                    assert(*prev(r2.end()) == *prev(end(expected)));
                }
            }
        }
    }

    // Check view_interface::cbegin
    static_assert(CanMemberCBegin<R>);
    static_assert(CanMemberCBegin<const R&> == ranges::range<const V>);
    {
        const same_as<const_iterator_t<R>> auto i = r.cbegin();
        if (!is_empty) {
            assert(*i == *cbegin(expected));
        }

        if constexpr (copyable<V>) {
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
    static_assert(CanMemberCEnd<R>);
    static_assert(CanMemberCEnd<const R&> == ranges::range<const V>);
    if (!is_empty) {
        same_as<const_sentinel_t<R>> auto i = r.cend();
        if constexpr (common_range<V> && sized_range<V> && bidirectional_range<V>) {
            assert(*prev(i) == *prev(cend(expected)));
        }

        if constexpr (CanCEnd<const R&>) {
            same_as<const_sentinel_t<const R>> auto i2 = as_const(r).cend();
            if constexpr (common_range<const V> && sized_range<const V> && bidirectional_range<const V>) {
                assert(*prev(i2) == *prev(cend(expected)));
            }
        }
    }

    // Check view_interface::data
    static_assert(!CanData<R>);
    static_assert(!CanData<const R>);

    if (is_empty) {
        return true;
    }

    // Check view_interface::operator[]
    static_assert(CanIndex<R> == random_access_range<V>);
    if constexpr (CanIndex<R>) {
        assert(r[0] == expected[0]);
    }

    // Check view_interface::operator[] (const)
    static_assert(CanIndex<const R> == random_access_range<const V>);
    if constexpr (CanIndex<const R>) {
        assert(as_const(r)[0] == expected[0]);
    }

    // Check view_interface::front
    static_assert(CanMemberFront<R>);
    assert(r.front() == *begin(expected));

    // Check view_interface::front (const)
    static_assert(CanMemberFront<const R>);
    assert(as_const(r).front() == *begin(expected));

    // Check view_interface::back
    static_assert(CanMemberBack<R> == (bidirectional_range<V> && common_range<V>) );
    if constexpr (CanMemberBack<R>) {
        assert(r.back() == *prev(end(expected)));
    }

    // Check view_interface::back (const)
    static_assert(CanMemberBack<const R> == (bidirectional_range<const V> && common_range<const V>) );
    if constexpr (CanMemberBack<const R>) {
        assert(as_const(r).back() == *prev(end(expected)));
    }

    { // Check adjacent_view::iterator<not const>
        using BI = iterator_t<V>;
        using I  = iterator_t<R>;
        static_assert(forward_iterator<I>);

        // Check iterator_category
        static_assert(same_as<typename I::iterator_category, input_iterator_tag>);

        // Check iterator_concept
        using IterConcept = I::iterator_concept;
        static_assert(random_access_range<V> == same_as<IterConcept, random_access_iterator_tag>);
        static_assert(
            (bidirectional_range<V> && !random_access_range<V>) == same_as<IterConcept, bidirectional_iterator_tag>);
        static_assert((forward_range<V> && !bidirectional_range<V>) == same_as<IterConcept, forward_iterator_tag>);

        // Check value_type
        static_assert(same_as<typename I::value_type, repeated_tuple<range_value_t<V>, N>>);

        // Check default-initializability
        static_assert(default_initializable<I>);

        auto i = r.begin();

        { // Check dereference
            same_as<repeated_tuple<range_reference_t<V>, N>> decltype(auto) v = *as_const(i);
            assert(v == expected[0]);
        }

        { // Check pre-incrementation
            same_as<I&> decltype(auto) i2 = ++i;
            assert(&i2 == &i);
            if (i != r.end()) {
#pragma warning(push)
#pragma warning(disable : 28020) // The expression '_Param_(1)<1' is not true at this call.
                assert(*i == expected[1]);
#pragma warning(pop)
            }
            i = r.begin();
        }

        { // Check post-incrementation
            same_as<I> decltype(auto) i2 = i++;
            assert(*i2 == expected[0]);
            if (i != r.end()) {
#pragma warning(push)
#pragma warning(disable : 28020) // The expression '_Param_(1)<1' is not true at this call.
                assert(*i == expected[1]);
#pragma warning(pop)
            }
            i = r.begin();
        }

        { // Check equality comparisons
            auto i2               = i;
            same_as<bool> auto b1 = i == i2;
            assert(b1);
            ++i2;
            same_as<bool> auto b2 = i != i2;
            assert(b2);
        }

        if constexpr (bidirectional_range<V>) {
            { // Check pre-decrementation
                i = ranges::next(r.begin());

                same_as<I&> decltype(auto) i2 = --i;
                assert(&i2 == &i);
                assert(*i2 == expected[0]);
            }

            { // Check post-decrementation
                i = ranges::next(r.begin());

                same_as<I> decltype(auto) i2 = i--;
                if (i2 != r.end()) {
#pragma warning(push)
#pragma warning(disable : 28020) // The expression '_Param_(1)<1' is not true at this call.
                    assert(*i2 == expected[1]);
#pragma warning(pop)
                }
                assert(*i == expected[0]);
            }
        }

        if constexpr (random_access_range<V>) {
            { // Check advancement operators
                same_as<I&> decltype(auto) i2 = (i += 1);
                assert(&i2 == &i);
                if (i != r.end()) {
#pragma warning(push)
#pragma warning(disable : 28020) // The expression '_Param_(1)<1' is not true at this call.
                    assert(*i == expected[1]);
#pragma warning(pop)
                }

                same_as<I&> decltype(auto) i3 = (i -= 1);
                assert(&i3 == &i);
                assert(*i == expected[0]);
            }

            { // Check subscript operator
                same_as<repeated_tuple<range_reference_t<V>, N>> decltype(auto) v = i[0];
                assert(v == expected[0]);
            }

            { // Check other comparisons
                auto i2               = ranges::next(i);
                same_as<bool> auto b1 = i < i2;
                assert(b1);
                same_as<bool> auto b2 = i2 > i;
                assert(b2);
                same_as<bool> auto b3 = i <= i2;
                assert(b3);
                same_as<bool> auto b4 = i2 >= i;
                assert(b4);
            }

            if constexpr (three_way_comparable<BI>) { // Check 3way comparisons
                using Cat              = compare_three_way_result_t<BI>;
                auto i2                = i;
                same_as<Cat> auto cmp1 = i <=> i2;
                assert(cmp1 == Cat::equivalent);
                ++i2;
                assert((i <=> i2) == Cat::less);
                assert((i2 <=> i) == Cat::greater);
            }

            { // Check operator+
                same_as<I> auto i2 = i + 1;
                if (i2 != r.end()) {
#pragma warning(push)
#pragma warning(disable : 28020) // The expression '_Param_(1)<1' is not true at this call.
                    assert(*i2 == expected[1]);
#pragma warning(pop)
                }

                same_as<I> auto i3 = 1 + i;
                if (i3 != r.end()) {
#pragma warning(push)
#pragma warning(disable : 28020) // The expression '_Param_(1)<1' is not true at this call.
                    assert(*i3 == expected[1]);
#pragma warning(pop)
                }
            }

            { // Check operator-(Iter, Diff)
                same_as<I> auto i2 = ranges::next(i) - 1;
                assert(*i2 == expected[0]);
            }
        }

        if constexpr (sized_sentinel_for<BI, BI>) { // Check differencing
            same_as<range_difference_t<V>> auto diff = i - i;
            assert(diff == 0);
            assert((i - ranges::next(i)) == -1);
            assert((ranges::next(i) - i) == 1);
        }

        if constexpr (sized_sentinel_for<sentinel_t<R>, I>) { // Check differencing with sentinel<not const>
            const auto s                                    = r.end();
            const auto size                                 = ranges::ssize(expected);
            const same_as<range_difference_t<V>> auto diff1 = i - s;
            assert(diff1 == -size);
            const same_as<range_difference_t<V>> auto diff2 = s - i;
            assert(diff2 == size);
        }

        if constexpr (sized_sentinel_for<sentinel_t<const R>, I>) { // Check differencing with sentinel<const>
            const auto s                                    = as_const(r).end();
            const auto size                                 = ranges::ssize(expected);
            const same_as<range_difference_t<V>> auto diff1 = i - s;
            assert(diff1 == -size);
            const same_as<range_difference_t<V>> auto diff2 = s - i;
            assert(diff2 == size);
        }

        { // Check iter_move
            same_as<repeated_tuple<iter_rvalue_reference_t<BI>, N>> decltype(auto) rval = iter_move(as_const(i));
            assert(rval == expected[0]);
            static_assert(noexcept(iter_move(i))
                          == (noexcept(ranges::iter_move(declval<BI>()))
                              && is_nothrow_move_constructible_v<iter_rvalue_reference_t<BI>>) );
        }

        if constexpr (indirectly_swappable<BI>) { // Check iter_swap
            static_assert(is_void_v<decltype(ranges::iter_swap(as_const(i), as_const(i)))>);
            static_assert(
                noexcept(iter_swap(i, i)) == noexcept(ranges::iter_swap(declval<const BI&>(), declval<const BI&>())));
            // Note: other tests are defined in 'test_iter_swap' function
        }
    }

    // Check adjacent_view::iterator<const>
    if constexpr (CanMemberBegin<const R>) {
        using CBI = iterator_t<const V>;
        using CI  = iterator_t<const R>;
        static_assert(forward_iterator<CI>);

        // Check iterator_category
        static_assert(same_as<typename CI::iterator_category, input_iterator_tag>);

        // Check iterator_concept
        using IterConcept = CI::iterator_concept;
        static_assert(random_access_range<const V> == same_as<IterConcept, random_access_iterator_tag>);
        static_assert((bidirectional_range<const V> && !random_access_range<const V>)
                      == same_as<IterConcept, bidirectional_iterator_tag>);
        static_assert(
            (forward_range<const V> && !bidirectional_range<const V>) == same_as<IterConcept, forward_iterator_tag>);

        // Check value_type
        static_assert(same_as<typename CI::value_type, repeated_tuple<range_value_t<const V>, N>>);

        // Check default-initializability
        static_assert(default_initializable<CI>);

        iterator_t<R> i = r.begin();

        // Check construction from non-const iterator
        CI ci = i;

        { // Check dereference
            same_as<repeated_tuple<range_reference_t<const V>, N>> decltype(auto) v = *as_const(ci);
            assert(v == expected[0]);
        }

        { // Check pre-incrementation
            same_as<CI&> decltype(auto) ci2 = ++ci;
            assert(&ci2 == &ci);
            if (ci != r.end()) {
#pragma warning(push)
#pragma warning(disable : 28020) // The expression '_Param_(1)<1' is not true at this call.
                assert(*ci == expected[1]);
#pragma warning(pop)
            }
            ci = r.begin();
        }

        { // Check post-incrementation
            same_as<CI> decltype(auto) ci2 = ci++;
            assert(*ci2 == expected[0]);
            if (ci != r.end()) {
#pragma warning(push)
#pragma warning(disable : 28020) // The expression '_Param_(1)<1' is not true at this call.
                assert(*ci == expected[1]);
#pragma warning(pop)
            }
            ci = r.begin();
        }

        { // Check equality comparisons
            auto ci2              = ci;
            same_as<bool> auto b1 = ci == ci2;
            assert(b1);
            ++ci2;
            same_as<bool> auto b2 = ci != ci2;
            assert(b2);
        }

        { // Check equality comparisons with non-const iterators
            same_as<bool> auto b1 = ci == i;
            assert(b1);
            ++i;
            same_as<bool> auto b2 = ci != i;
            assert(b2);
            i = r.begin();
        }

        if constexpr (bidirectional_range<const V>) {
            { // Check pre-decrementation
                ci = ranges::next(r.begin());

                same_as<CI&> decltype(auto) ci2 = --ci;
                assert(&ci2 == &ci);
                assert(*ci2 == expected[0]);
            }

            { // Check post-decrementation
                ci = ranges::next(r.begin());

                same_as<CI> decltype(auto) ci2 = ci--;
                if (ci2 != r.end()) {
#pragma warning(push)
#pragma warning(disable : 28020) // The expression '_Param_(1)<1' is not true at this call.
                    assert(*ci2 == expected[1]);
#pragma warning(pop)
                }
                assert(*ci == expected[0]);
            }
        }

        if constexpr (random_access_range<const V>) {
            { // Check advancement operators
                same_as<CI&> decltype(auto) ci2 = (ci += 1);
                assert(&ci2 == &ci);
                if (ci != r.end()) {
#pragma warning(push)
#pragma warning(disable : 28020) // The expression '_Param_(1)<1' is not true at this call.
                    assert(*ci == expected[1]);
#pragma warning(pop)
                }

                same_as<CI&> decltype(auto) ci3 = (ci -= 1);
                assert(&ci3 == &ci);
                assert(*ci == expected[0]);
            }

            { // Check subscript operator
                same_as<repeated_tuple<range_reference_t<const V>, N>> decltype(auto) v = ci[0];
                assert(v == expected[0]);
            }

            { // Check comparisons
                auto ci2              = ranges::next(ci);
                same_as<bool> auto b1 = ci < ci2;
                assert(b1);
                same_as<bool> auto b2 = ci2 > ci;
                assert(b2);
                same_as<bool> auto b3 = ci <= ci2;
                assert(b3);
                same_as<bool> auto b4 = ci2 >= ci;
                assert(b4);
            }

            { // Check comparisons with non-const iterators
                ++i;
                same_as<bool> auto b1 = ci < i;
                assert(b1);
                same_as<bool> auto b2 = i > ci;
                assert(b2);
                same_as<bool> auto b3 = ci <= i;
                assert(b3);
                same_as<bool> auto b4 = i >= ci;
                assert(b4);
                --i;
            }

            if constexpr (three_way_comparable<CBI>) { // Check 3way comparisons
                using Cat              = compare_three_way_result_t<CBI>;
                auto ci2               = ci;
                same_as<Cat> auto cmp1 = ci <=> ci2;
                assert(cmp1 == Cat::equivalent);
                ++ci2;
                assert((ci <=> ci2) == Cat::less);
                assert((ci2 <=> ci) == Cat::greater);
            }

            if constexpr (three_way_comparable<CBI>) { // Check 3way comparisons with non-const iterators
                using Cat              = compare_three_way_result_t<CBI>;
                same_as<Cat> auto cmp1 = ci <=> i;
                assert(cmp1 == Cat::equivalent);
                ++i;
                assert((ci <=> i) == Cat::less);
                assert((i <=> ci) == Cat::greater);
                --i;
            }

            { // Check operator+
                same_as<CI> auto ci2 = ci + 1;
                if (ci2 != r.end()) {
#pragma warning(push)
#pragma warning(disable : 28020) // The expression '_Param_(1)<1' is not true at this call.
                    assert(*ci2 == expected[1]);
#pragma warning(pop)
                }

                same_as<CI> auto ci3 = 1 + ci;
                if (ci3 != r.end()) {
#pragma warning(push)
#pragma warning(disable : 28020) // The expression '_Param_(1)<1' is not true at this call.
                    assert(*ci3 == expected[1]);
#pragma warning(pop)
                }
            }

            { // Check operator-(Iter, Diff)
                same_as<CI> auto ci2 = ranges::next(ci) - 1;
                assert(*ci2 == expected[0]);
            }
        }

        if constexpr (sized_sentinel_for<CBI, CBI>) { // Check differencing
            same_as<range_difference_t<const V>> auto diff = ci - ci;
            assert(diff == 0);
            assert((ci - ranges::next(ci)) == -1);
            assert((ranges::next(ci) - ci) == 1);
        }

        if constexpr (sized_sentinel_for<sentinel_t<R>, CI>) { // Check differencing with sentinel<not const>
            const auto s                                          = r.end();
            const auto size                                       = ranges::ssize(expected);
            const same_as<range_difference_t<const V>> auto diff1 = ci - s;
            assert(diff1 == -size);
            const same_as<range_difference_t<const V>> auto diff2 = s - ci;
            assert(diff2 == size);
        }

        if constexpr (sized_sentinel_for<sentinel_t<const R>, CI>) { // Check differencing with sentinel<const>
            const auto s                                          = as_const(r).end();
            const auto size                                       = ranges::ssize(expected);
            const same_as<range_difference_t<const V>> auto diff1 = ci - s;
            assert(diff1 == -size);
            const same_as<range_difference_t<const V>> auto diff2 = s - ci;
            assert(diff2 == size);
        }

        { // Check iter_move
            same_as<repeated_tuple<iter_rvalue_reference_t<CBI>, N>> decltype(auto) rval = iter_move(as_const(ci));
            assert(rval == expected[0]);
            static_assert(noexcept(iter_move(ci))
                          == (noexcept(ranges::iter_move(declval<CBI>()))
                              && is_nothrow_move_constructible_v<iter_rvalue_reference_t<CBI>>) );
        }

        if constexpr (indirectly_swappable<CBI>) { // Check iter_swap
            static_assert(is_void_v<decltype(ranges::iter_swap(as_const(ci), as_const(ci)))>);
            static_assert(noexcept(iter_swap(ci, ci))
                          == noexcept(ranges::iter_swap(declval<const CBI&>(), declval<const CBI&>())));
            // Note: other tests are defined in 'test_iter_swap' function
        }
    }

    // Check adjacent_view::base() const&
    static_assert(CanMemberBase<const R&> == copy_constructible<V>);
    if constexpr (copy_constructible<V>) {
        [[maybe_unused]] same_as<V> auto b1 = as_const(r).base();
        static_assert(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>); // strengthened
    }

    // Check adjacent_view::base() &&
    [[maybe_unused]] same_as<V> auto b2 = move(r).base();
    static_assert(noexcept(move(r).base()) == is_nothrow_move_constructible_v<V>); // strengthened

    return true;
}

template <ranges::forward_range Rng>
constexpr void test_adjacent0(Rng&& rng) {
    static_assert(!CanConstructAdjacentView<Rng, 0>);
    using V = views::all_t<Rng>;
    using E = ranges::empty_view<tuple<>>;

    // Check range closure object
    constexpr auto closure = views::adjacent<0>;
    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;

    // ... with lvalue argument
    static_assert(CanViewAdjacent<Rng&, 0> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewAdjacent<Rng&, 0>) {
        static_assert(same_as<decltype(closure(rng)), E>);
        static_assert(noexcept(closure(rng)));

        static_assert(same_as<decltype(rng | closure), E>);
        static_assert(noexcept(rng | closure));
    }

    // ... with const lvalue argument
    static_assert(CanViewAdjacent<const remove_reference_t<Rng>&, 0> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewAdjacent<const remove_reference_t<Rng>&, 0>) {
        static_assert(same_as<decltype(closure(as_const(rng))), E>);
        static_assert(noexcept(closure(as_const(rng))));

        static_assert(same_as<decltype(as_const(rng) | closure), E>);
        static_assert(noexcept(as_const(rng) | closure));
    }

    // ... with rvalue argument
    static_assert(CanViewAdjacent<remove_reference_t<Rng>, 0> == (is_view || movable<remove_reference_t<Rng>>) );
    if constexpr (CanViewAdjacent<remove_reference_t<Rng>, 0>) {
        static_assert(same_as<decltype(closure(move(rng))), E>);
        static_assert(noexcept(closure(move(rng))));

        static_assert(same_as<decltype(move(rng) | closure), E>);
        static_assert(noexcept(move(rng) | closure));
    }

    // ... with const rvalue argument
    static_assert(CanViewAdjacent<const remove_reference_t<Rng>, 0> == (is_view && copy_constructible<V>) );
    if constexpr (CanViewAdjacent<const remove_reference_t<Rng>, 0>) {
        static_assert(same_as<decltype(closure(move(as_const(rng)))), E>);
        static_assert(noexcept(closure(move(as_const(rng)))));

        static_assert(same_as<decltype(move(as_const(rng)) | closure), E>);
        static_assert(noexcept(move(as_const(rng)) | closure));
    }
}

template <size_t N, ranges::forward_range Rng>
    requires indirectly_swappable<ranges::iterator_t<Rng>>
constexpr void test_iter_swap(Rng& rng) {
    // This test assumes that 'ranges::size(views::adjacent<N>(rng))' is at least 2
    auto r = views::adjacent<N>(rng);

    { // Check iter_swap for adjacent_view::iterator<not const>
        auto i      = r.begin();
        auto first  = *i;
        auto j      = ranges::next(i);
        auto second = *j;

        // It takes N+1 swaps to get to initial state
        for ([[maybe_unused]] size_t _ : views::iota(0u, N)) {
            iter_swap(as_const(i), as_const(j));
        }
        ranges::iter_swap(as_const(i), as_const(j));

        assert(*i == first);
        assert(*j == second);
    }

    // Check iter_swap for adjacent_view::iterator<const>
    if constexpr (CanMemberBegin<const decltype(r)> && indirectly_swappable<ranges::iterator_t<const Rng>>) {
        auto i      = as_const(r).begin();
        auto first  = *i;
        auto j      = ranges::next(i);
        auto second = *j;

        // It takes N+1 swaps to get to initial state
        for ([[maybe_unused]] size_t _ : views::iota(0u, N)) {
            iter_swap(as_const(i), as_const(j));
        }
        ranges::iter_swap(as_const(i), as_const(j));

        assert(*i == first);
        assert(*j == second);
    }
}

// We have to use std::array instead of C-array, see LLVM-61025
constexpr auto some_ints        = array{1, 3, 5, 7, 11, 13};
constexpr auto adjacent1_result = to_array<repeated_tuple<int, 1>>({{1}, {3}, {5}, {7}, {11}, {13}});
constexpr auto pairwise_result  = to_array<repeated_tuple<int, 2>>({{1, 3}, {3, 5}, {5, 7}, {7, 11}, {11, 13}});
constexpr auto adjacent3_result = to_array<repeated_tuple<int, 3>>({{1, 3, 5}, {3, 5, 7}, {5, 7, 11}, {7, 11, 13}});
constexpr auto adjacent4_result = to_array<repeated_tuple<int, 4>>({{1, 3, 5, 7}, {3, 5, 7, 11}, {5, 7, 11, 13}});
constexpr auto adjacent5_result = to_array<repeated_tuple<int, 5>>({{1, 3, 5, 7, 11}, {3, 5, 7, 11, 13}});
constexpr auto adjacent6_result = to_array<repeated_tuple<int, 6>>({{1, 3, 5, 7, 11, 13}});
constexpr array<repeated_tuple<int, 7>, 0> adjacent7_result;

template <class ValTy, class Category, test::Common IsCommon, test::Sized IsSized>
using test_range =
    test::range<Category, ValTy, IsSized, test::CanDifference{derived_from<Category, random_access_iterator_tag>},
        IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
        test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}>;

struct instantiator {
#ifdef TEST_EVERYTHING
    template <ranges::forward_range R>
    static constexpr void call() {
        R r{some_ints};
        test_one<1>(r, adjacent1_result);
        test_one<2>(r, pairwise_result);
        test_one<4>(r, adjacent4_result);
        test_one<7>(r, adjacent7_result);
        test_adjacent0(r);

        // We cannot invoke 'test_iter_swap' in 'TEST_EVERYTHING' mode.
    }
#else // ^^^ test all input range permutations / test only "interesting" permutations vvv
    template <class Tag, test::Common IsCommon, test::Sized IsSized>
    static constexpr void call() {
        test_range<const int, Tag, IsCommon, IsSized> r{some_ints};
        test_one<1>(r, adjacent1_result);
        test_one<2>(r, pairwise_result);
        test_one<4>(r, adjacent4_result);
        test_one<7>(r, adjacent7_result);
        test_adjacent0(r);

        int swap_check[4] = {1, 2, 3, 4};
        test_range<int, Tag, IsCommon, IsSized> swap_check_r{swap_check};
        test_iter_swap<2>(swap_check_r);
        test_iter_swap<3>(swap_check_r);
    }
#endif // TEST_EVERYTHING
};

constexpr void instantiation_test() {
#ifdef TEST_EVERYTHING
    test_fwd<instantiator, const int>();
#else // ^^^ test all forward range permutations / test only "interesting" permutations vvv
    using test::Common, test::Sized;

    // The view is sensitive to category, commonality, and size, but oblivious to proxyness and differencing
    instantiator::call<forward_iterator_tag, Common::no, Sized::yes>();
    instantiator::call<forward_iterator_tag, Common::no, Sized::no>();
    instantiator::call<forward_iterator_tag, Common::yes, Sized::yes>();
    instantiator::call<forward_iterator_tag, Common::yes, Sized::no>();

    instantiator::call<bidirectional_iterator_tag, Common::no, Sized::yes>();
    instantiator::call<bidirectional_iterator_tag, Common::no, Sized::no>();
    instantiator::call<bidirectional_iterator_tag, Common::yes, Sized::yes>();
    instantiator::call<bidirectional_iterator_tag, Common::yes, Sized::no>();

    instantiator::call<random_access_iterator_tag, Common::no, Sized::yes>();
    instantiator::call<random_access_iterator_tag, Common::no, Sized::no>();
    instantiator::call<random_access_iterator_tag, Common::yes, Sized::yes>();
    instantiator::call<random_access_iterator_tag, Common::yes, Sized::no>();

    instantiator::call<contiguous_iterator_tag, Common::no, Sized::yes>();
    instantiator::call<contiguous_iterator_tag, Common::no, Sized::no>();
    instantiator::call<contiguous_iterator_tag, Common::yes, Sized::yes>();
    instantiator::call<contiguous_iterator_tag, Common::yes, Sized::no>();
#endif // TEST_EVERYTHING
}

// LWG-4098 views::adjacent<0> should reject non-forward ranges
template <size_t N, ranges::input_range Rng>
constexpr void test_input_only(Rng&&) {
    if constexpr (!ranges::forward_range<Rng>) {
        static_assert(!CanViewAdjacent<Rng&, N>);
        static_assert(!CanViewAdjacent<Rng, N>);
        static_assert(!CanConstructAdjacentView<Rng&, N>);
        static_assert(!CanConstructAdjacentView<Rng, N>);
    }

    if constexpr (!ranges::forward_range<const Rng>) {
        static_assert(!CanViewAdjacent<const Rng&, N>);
        static_assert(!CanViewAdjacent<const Rng, N>);
        static_assert(!CanConstructAdjacentView<const Rng&, N>);
        static_assert(!CanConstructAdjacentView<const Rng, N>);
    }
}

struct input_only_instantiator {
#ifdef TEST_EVERYTHING
    template <ranges::input_range R>
    static constexpr void call() {
        R r{some_ints};
        test_input_only<0>(r);
        test_input_only<1>(r);
        test_input_only<2>(r);
        test_input_only<4>(r);
        test_input_only<7>(r);
    }
#else // ^^^ test all input range permutations / test only "interesting" permutations vvv
    template <class Tag, test::Common IsCommon, test::Sized IsSized>
    static constexpr void call() {
        test_range<const int, Tag, IsCommon, IsSized> r{some_ints};
        test_input_only<0>(r);
        test_input_only<1>(r);
        test_input_only<2>(r);
        test_input_only<4>(r);
        test_input_only<7>(r);
    }
#endif // TEST_EVERYTHING
};

constexpr void instantiation_input_only_test() {
#ifdef TEST_EVERYTHING
    test_in<input_only_instantiator, const int>();
#else // ^^^ test all input range permutations / test only "interesting" permutations vvv
    using test::Common, test::Sized;

    // The view is sensitive to category, commonality, and size, but oblivious to proxyness and differencing
    input_only_instantiator::call<input_iterator_tag, Common::no, Sized::yes>();
    input_only_instantiator::call<input_iterator_tag, Common::no, Sized::no>();
    input_only_instantiator::call<input_iterator_tag, Common::yes, Sized::yes>();
    input_only_instantiator::call<input_iterator_tag, Common::yes, Sized::no>();
#endif // TEST_EVERYTHING
}

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare::yes, test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>},
    test::CanView::yes, test::Copyability::move_only>;

int main() {
    { // Check views
        // ... copyable
        constexpr span<const int> s{some_ints};
        static_assert(test_one<4>(s, adjacent4_result));
        test_one<4>(s, adjacent4_result);
    }

    { // ... move-only
        test_one<1>(move_only_view<forward_iterator_tag, test::Common::no>{some_ints}, adjacent1_result);
        test_one<2>(move_only_view<forward_iterator_tag, test::Common::yes>{some_ints}, pairwise_result);
        test_one<3>(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints}, adjacent3_result);
        test_one<4>(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints}, adjacent4_result);
        test_one<5>(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints}, adjacent5_result);
        test_one<6>(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints}, adjacent6_result);
    }

    { // Check non-views
        static_assert(test_one<2>(some_ints, pairwise_result));
        test_one<2>(some_ints, pairwise_result);

        auto vec = some_ints | ranges::to<vector>();
        test_one<3>(vec, adjacent3_result);

        auto lst = some_ints | ranges::to<forward_list>();
        test_one<4>(lst, adjacent4_result);
    }

    { // Check empty range
        static_assert(test_one<3>(span<const int>{}, span<tuple<int, int, int>>{}));
        test_one<3>(span<const int>{}, span<tuple<int, int, int>>{});
    }

#ifndef _PREFAST_ // TRANSITION, GH-1030
    static_assert((instantiation_test(), true));
#endif
    instantiation_test();

#ifndef _PREFAST_ // TRANSITION, GH-1030
    static_assert((instantiation_input_only_test(), true));
#endif
    instantiation_input_only_test();
}
