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

template <class R, class F, size_t N>
concept CanViewAdjacentTransform =
    requires(R&& r, F&& f) { views::adjacent_transform<N>(forward<R>(r), forward<F>(f)); };

template <class T, class Indices>
struct repeated_tuple_impl;

template <class T, size_t... Indices>
struct repeated_tuple_impl<T, index_sequence<Indices...>> {
    template <size_t>
    using repeat_type = T;

    using type = tuple<repeat_type<Indices>...>;
};

template <class T, size_t N>
using repeated_tuple = typename repeated_tuple_impl<T, make_index_sequence<N>>::type;

STATIC_ASSERT(same_as<repeated_tuple<int, 0>, tuple<>>);
STATIC_ASSERT(same_as<repeated_tuple<int, 3>, tuple<int, int, int>>);
STATIC_ASSERT(same_as<repeated_tuple<int, 5>, tuple<int, int, int, int, int>>);

template <class Fn, class Ty, class Indices>
struct regular_invocable_with_repeated_type_impl;

template <class F, class T, size_t... Indices>
struct regular_invocable_with_repeated_type_impl<F, T, index_sequence<Indices...>> {
    template <size_t>
    using repeat_type = T;

    static constexpr bool value = regular_invocable<F, repeat_type<Indices>...>;
};

template <class Fn, class Ty, size_t Nx>
concept regular_invocable_with_repeated_type =
    regular_invocable_with_repeated_type_impl<Fn, Ty, make_index_sequence<Nx>>::value;

static_assert(regular_invocable_with_repeated_type<plus<int>, int, 2>);
static_assert(!regular_invocable_with_repeated_type<plus<int>, int, 3>);
static_assert(!regular_invocable_with_repeated_type<plus<int>, span<int>, 2>);

template <class Fn, class Ty, size_t Nx, class... Repeated>
struct invoke_result_with_repeated_type_impl : invoke_result_with_repeated_type_impl<Fn, Ty, Nx - 1, Ty, Repeated...> {
};

template <class Fn, class Ty, class... Repeated>
struct invoke_result_with_repeated_type_impl<Fn, Ty, 0, Repeated...> {
    using type = invoke_result_t<Fn, Repeated...>;
};

template <class Fn, class Ty, size_t Nx>
using invoke_result_with_repeated_type = typename invoke_result_with_repeated_type_impl<Fn, Ty, Nx>::type;

static_assert(same_as<invoke_result_with_repeated_type<plus<int>, int, 2>, int>);
static_assert(same_as<invoke_result_with_repeated_type<equal_to<int>, int, 2>, bool>);

template <size_t N, ranges::input_range Rng, class Expected, class Fn>
constexpr bool test_one(Rng&& rng, Fn func, Expected&& expected_rng) {
    using ranges::adjacent_transform_view, ranges::adjacent_view, ranges::forward_range, ranges::bidirectional_range,
        ranges::random_access_range, ranges::sized_range, ranges::common_range, ranges::iterator_t,
        ranges::const_iterator_t, ranges::sentinel_t, ranges::const_sentinel_t, ranges::range_reference_t,
        ranges::range_difference_t, ranges::begin, ranges::end;

    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;

    using V = views::all_t<Rng>;
    using R = adjacent_transform_view<V, decay_t<Fn>, N>;

    STATIC_ASSERT(ranges::view<R>);
    STATIC_ASSERT(ranges::input_range<R>);
    STATIC_ASSERT(forward_range<R>);
    STATIC_ASSERT(bidirectional_range<R> == bidirectional_range<Rng>);
    STATIC_ASSERT(random_access_range<R> == random_access_range<Rng>);
    STATIC_ASSERT(!ranges::contiguous_range<R>);

    // Check default-initializability
    STATIC_ASSERT(default_initializable<R> == default_initializable<V>);

    // Check borrowed_range
    STATIC_ASSERT(!ranges::borrowed_range<R>);

    // Check range closure object
    const auto closure = views::adjacent_transform<N>(func);

    // ... with lvalue argument
    STATIC_ASSERT(CanViewAdjacentTransform<Rng&, Fn, N> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewAdjacentTransform<Rng&, Fn, N>) {
        constexpr bool is_noexcept =
            !is_view || (is_nothrow_copy_constructible_v<V> && is_nothrow_copy_constructible_v<Fn>);

        STATIC_ASSERT(same_as<decltype(views::adjacent_transform<N>(rng, func)), R>);
        STATIC_ASSERT(noexcept(views::adjacent_transform<N>(rng, func)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(closure(rng)), R>);
        STATIC_ASSERT(noexcept(closure(rng)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(rng | closure), R>);
        STATIC_ASSERT(noexcept(rng | closure) == is_noexcept);
    }

    // ... with const lvalue argument
    STATIC_ASSERT(
        CanViewAdjacentTransform<const remove_reference_t<Rng>&, Fn, N> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewAdjacentTransform<const remove_reference_t<Rng>&, Fn, N>) {
        using RC = adjacent_transform_view<views::all_t<const remove_reference_t<Rng>&>, Fn, N>;
        constexpr bool is_noexcept =
            !is_view || (is_nothrow_copy_constructible_v<V> && is_nothrow_copy_constructible_v<Fn>);

        STATIC_ASSERT(same_as<decltype(views::adjacent_transform<N>(as_const(rng), func)), RC>);
        STATIC_ASSERT(noexcept(views::adjacent_transform<N>(as_const(rng), func)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(closure(as_const(rng))), RC>);
        STATIC_ASSERT(noexcept(closure(as_const(rng))) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | closure), RC>);
        STATIC_ASSERT(noexcept(as_const(rng) | closure) == is_noexcept);
    }

    // ... with rvalue argument
    STATIC_ASSERT(
        CanViewAdjacentTransform<remove_reference_t<Rng>, Fn, N> == (is_view || movable<remove_reference_t<Rng>>) );
    if constexpr (CanViewAdjacentTransform<remove_reference_t<Rng>, Fn, N>) {
        using RS                   = adjacent_transform_view<views::all_t<remove_reference_t<Rng>>, Fn, N>;
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V> && is_nothrow_copy_constructible_v<Fn>;

        STATIC_ASSERT(same_as<decltype(views::adjacent_transform<N>(move(rng), func)), RS>);
        STATIC_ASSERT(noexcept(views::adjacent_transform<N>(move(rng), func)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(closure(move(rng))), RS>);
        STATIC_ASSERT(noexcept(closure(move(rng))) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | closure), RS>);
        STATIC_ASSERT(noexcept(move(rng) | closure) == is_noexcept);
    }

    // ... with const rvalue argument
    STATIC_ASSERT(
        CanViewAdjacentTransform<const remove_reference_t<Rng>, Fn, N> == (is_view && copy_constructible<V>) );
    if constexpr (CanViewAdjacentTransform<const remove_reference_t<Rng>, Fn, N>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V> && is_nothrow_copy_constructible_v<Fn>;

        STATIC_ASSERT(same_as<decltype(views::adjacent_transform<N>(move(as_const(rng)), func)), R>);
        STATIC_ASSERT(noexcept(views::adjacent_transform<N>(move(as_const(rng)), func)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(closure(move(as_const(rng)))), R>);
        STATIC_ASSERT(noexcept(closure(move(as_const(rng)))) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | closure), R>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | closure) == is_noexcept);
    }

    // Check views::pairwise_transform
    if constexpr (N == 2) {
        using Pairwised  = decltype(views::pairwise_transform(forward<Rng>(rng), func));
        using Adjacented = decltype(views::adjacent_transform<2>(forward<Rng>(rng), func));
        STATIC_ASSERT(same_as<Pairwised, Adjacented>);
    }

    R r{forward<Rng>(rng), func};

    // Check adjacent_transform_view::size
    STATIC_ASSERT(CanMemberSize<R> == sized_range<V>);
    if constexpr (CanMemberSize<R>) {
        same_as<ranges::range_size_t<V>> auto s = r.size();
        assert(_To_unsigned_like(s) == ranges::size(expected_rng));
        STATIC_ASSERT(noexcept(r.size()) == noexcept(ranges::size(rng))); // strengthened
    }

    // Check adjacent_transform_view::size (const)
    STATIC_ASSERT(CanMemberSize<const R> == sized_range<const V>);
    if constexpr (CanMemberSize<const R>) {
        same_as<ranges::range_size_t<const V>> auto s = as_const(r).size();
        assert(_To_unsigned_like(s) == ranges::size(expected_rng));
        STATIC_ASSERT(noexcept(as_const(r).size()) == noexcept(ranges::size(as_const(rng)))); // strengthened
    }

    const bool is_empty = ranges::empty(expected_rng);

    // Check view_interface::empty and operator bool
    STATIC_ASSERT(CanMemberEmpty<R>);
    STATIC_ASSERT(CanBool<R>);
    assert(r.empty() == is_empty);
    assert(static_cast<bool>(r) == !is_empty);

    // Check view_interface::empty and operator bool (const)
    STATIC_ASSERT(CanMemberEmpty<const R>);
    STATIC_ASSERT(CanBool<const R>);
    assert(as_const(r).empty() == is_empty);
    assert(static_cast<bool>(as_const(r)) == !is_empty);

    // Check content
    assert(ranges::equal(r, expected_rng));

    // Check adjacent_transform_view::begin
    STATIC_ASSERT(CanMemberBegin<R>);
    {
        const same_as<iterator_t<R>> auto i = r.begin();
        if (!is_empty) {
            assert(*i == *begin(expected_rng));
        }

        if constexpr (copy_constructible<V>) {
            auto r2                              = r;
            const same_as<iterator_t<R>> auto i2 = r2.begin();
            if (!is_empty) {
                assert(*i2 == *i);
            }
        }
    }

    // Check adjacent_transform_view::begin (const)
    STATIC_ASSERT(
        CanMemberBegin<const R>
        == (ranges::range<const V> && regular_invocable_with_repeated_type<const Fn&, range_reference_t<const V>, N>) );
    if constexpr (CanMemberBegin<const R>) {
        const same_as<iterator_t<const R>> auto ci = as_const(r).begin();
        if (!is_empty) {
            assert(*ci == *begin(expected_rng));
        }

        if constexpr (copy_constructible<V>) {
            const auto cr2                              = r;
            const same_as<iterator_t<const R>> auto ci2 = cr2.begin();
            if (!is_empty) {
                assert(*ci2 == *ci);
            }
        }
    }

    // Check adjacent_transform_view::end
    STATIC_ASSERT(CanMemberEnd<R>);
    {
        const same_as<sentinel_t<R>> auto s = r.end();
        assert((r.begin() == s) == is_empty);

        if constexpr (sentinel_for<sentinel_t<R>, iterator_t<const R>>) {
            assert((as_const(r).begin() == s) == is_empty);
        }

        STATIC_ASSERT(common_range<R> == common_range<V>);
        if constexpr (common_range<V> && bidirectional_range<V>) {
            if (!is_empty) {
                assert(*ranges::prev(s) == *ranges::prev(end(expected_rng)));
            }

            if constexpr (copy_constructible<V>) {
                auto r2 = r;
                if (!is_empty) {
                    assert(*ranges::prev(r2.end()) == *ranges::prev(end(expected_rng)));
                }
            }
        }
    }

    // Check adjacent_transform_view::end (const)
    STATIC_ASSERT(
        CanMemberEnd<const R>
        == (ranges::range<const V> && regular_invocable_with_repeated_type<const Fn&, range_reference_t<const V>, N>) );
    if constexpr (CanMemberEnd<const R>) {
        const same_as<sentinel_t<const R>> auto cs = as_const(r).end();
        assert((as_const(r).begin() == cs) == is_empty);

        if constexpr (sentinel_for<sentinel_t<const R>, iterator_t<R>>) {
            assert((r.begin() == cs) == is_empty);
        }

        STATIC_ASSERT(common_range<const R> == common_range<const V>);
        if constexpr (common_range<const V> && bidirectional_range<const V>) {
            if (!is_empty) {
                assert(*ranges::prev(cs) == *ranges::prev(end(expected_rng)));
            }

            if constexpr (copy_constructible<V>) {
                const auto r2 = r;
                if (!is_empty) {
                    assert(*ranges::prev(r2.end()) == *ranges::prev(end(expected_rng)));
                }
            }
        }
    }

    // Check view_interface::cbegin
    STATIC_ASSERT(CanMemberCBegin<R>);
    STATIC_ASSERT(
        CanMemberCBegin<const R&>
        == (ranges::range<const V> && regular_invocable_with_repeated_type<const Fn&, range_reference_t<const V>, N>) );
    {
        const same_as<const_iterator_t<R>> auto i = r.cbegin();
        if (!is_empty) {
            assert(*i == *cbegin(expected_rng));
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
    STATIC_ASSERT(CanMemberCEnd<R>);
    STATIC_ASSERT(
        CanMemberCEnd<const R&>
        == (ranges::range<const V> && regular_invocable_with_repeated_type<const Fn&, range_reference_t<const V>, N>) );
    if (!is_empty) {
        same_as<const_sentinel_t<R>> auto i = r.cend();
        if constexpr (common_range<V> && sized_range<V> && bidirectional_range<V>) {
            assert(*ranges::prev(i) == *ranges::prev(cend(expected_rng)));
        }

        if constexpr (CanCEnd<const R&>) {
            same_as<const_sentinel_t<const R>> auto i2 = as_const(r).cend();
            if constexpr (common_range<const V> && sized_range<const V> && bidirectional_range<const V>) {
                assert(*ranges::prev(i2) == *ranges::prev(cend(expected_rng)));
            }
        }
    }

    // Check view_interface::data
    STATIC_ASSERT(!CanData<R>);
    STATIC_ASSERT(!CanData<const R>);

    if (is_empty) {
        return true;
    }

    // Check view_interface::operator[]
    STATIC_ASSERT(CanIndex<R> == random_access_range<V>);
    if constexpr (CanIndex<R>) {
        assert(r[0] == expected_rng[0]);
    }

    // Check view_interface::operator[] (const)
    STATIC_ASSERT(CanIndex<const R> == random_access_range<const V>);
    if constexpr (CanIndex<const R>) {
        assert(as_const(r)[0] == expected_rng[0]);
    }

    // Check view_interface::front
    STATIC_ASSERT(CanMemberFront<R>);
    assert(r.front() == *begin(expected_rng));

    // Check view_interface::front (const)
    STATIC_ASSERT(CanMemberFront<const R>);
    assert(as_const(r).front() == *begin(expected_rng));

    // Check view_interface::back
    STATIC_ASSERT(CanMemberBack<R> == (bidirectional_range<V> && common_range<V>) );
    if constexpr (CanMemberBack<R>) {
        assert(r.back() == *prev(end(expected_rng)));
    }

    // Check view_interface::back (const)
    STATIC_ASSERT(CanMemberBack<const R> == (bidirectional_range<const V> && common_range<const V>) );
    if constexpr (CanMemberBack<const R>) {
        assert(as_const(r).back() == *prev(end(expected_rng)));
    }

    { // Check adjacent_transform_view::iterator<not const>
        using BI = iterator_t<V>;
        using I  = iterator_t<R>;
        STATIC_ASSERT(forward_iterator<I>);

        // Check iterator_category
        using IterCat = typename I::iterator_category;
        if constexpr (!is_reference_v<invoke_result_with_repeated_type<Fn, range_reference_t<V>, N>>) {
            STATIC_ASSERT(same_as<IterCat, input_iterator_tag>);
        } else {
            using BaseCat = typename iterator_traits<BI>::iterator_category;
            if constexpr (derived_from<BaseCat, random_access_iterator_tag>) {
                STATIC_ASSERT(same_as<IterCat, random_access_iterator_tag>);
            } else if constexpr (derived_from<BaseCat, bidirectional_iterator_tag>) {
                STATIC_ASSERT(same_as<IterCat, bidirectional_iterator_tag>);
            } else if constexpr (derived_from<BaseCat, forward_iterator_tag>) {
                STATIC_ASSERT(same_as<IterCat, forward_iterator_tag>);
            } else {
                STATIC_ASSERT(same_as<IterCat, input_iterator_tag>);
            }
        }

        // Check iterator_concept
        using IterConcept = typename I::iterator_concept;
        STATIC_ASSERT(same_as<IterConcept, typename iterator_t<adjacent_view<V, N>>::iterator_concept>);

        // Check value_type
        STATIC_ASSERT(same_as<typename I::value_type,
            remove_cvref_t<invoke_result_with_repeated_type<Fn&, range_reference_t<V>, N>>>);

        // Check default-initializability
        STATIC_ASSERT(default_initializable<I>);

        auto i = r.begin();

        { // Check dereference
            same_as<invoke_result_with_repeated_type<Fn&, range_reference_t<V>, N>> decltype(auto) v = *as_const(i);
            assert(v == expected_rng[0]);
        }

        { // Check pre-incrementation
            same_as<I&> decltype(auto) i2 = ++i;
            assert(&i2 == &i);
            if (i != r.end()) {
#pragma warning(suppress : 28020) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call
                assert(*i == expected_rng[1]);
            }
            i = r.begin();
        }

        { // Check post-incrementation
            same_as<I> decltype(auto) i2 = i++;
            assert(*i2 == expected_rng[0]);
            if (i != r.end()) {
#pragma warning(suppress : 28020) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call
                assert(*i == expected_rng[1]);
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
                assert(*i2 == expected_rng[0]);
            }

            { // Check post-decrementation
                i = ranges::next(r.begin());

                same_as<I> decltype(auto) i2 = i--;
                if (i2 != r.end()) {
#pragma warning(suppress : 28020) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call
                    assert(*i2 == expected_rng[1]);
                }
                assert(*i == expected_rng[0]);
            }
        }

        if constexpr (random_access_range<V>) {
            { // Check advancement operators
                same_as<I&> decltype(auto) i2 = (i += 1);
                assert(&i2 == &i);
                if (i != r.end()) {
#pragma warning(suppress : 28020) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call
                    assert(*i == expected_rng[1]);
                }

                same_as<I&> decltype(auto) i3 = (i -= 1);
                assert(&i3 == &i);
                assert(*i == expected_rng[0]);
            }

            { // Check subscript operator
                same_as<invoke_result_with_repeated_type<Fn&, range_reference_t<V>, N>> decltype(auto) v = i[0];
                assert(v == expected_rng[0]);
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
#pragma warning(suppress : 28020) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call
                    assert(*i2 == expected_rng[1]);
                }

                same_as<I> auto i3 = 1 + i;
                if (i3 != r.end()) {
#pragma warning(suppress : 28020) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call
                    assert(*i3 == expected_rng[1]);
                }
            }

            { // Check operator-(Iter, Diff)
                same_as<I> auto i2 = ranges::next(i) - 1;
                assert(*i2 == expected_rng[0]);
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
            const auto size                                 = ranges::ssize(expected_rng);
            const same_as<range_difference_t<V>> auto diff1 = i - s;
            assert(diff1 == -size);
            const same_as<range_difference_t<V>> auto diff2 = s - i;
            assert(diff2 == size);
        }

        if constexpr (sized_sentinel_for<sentinel_t<const R>, I>) { // Check differencing with sentinel<const>
            const auto s                                    = as_const(r).end();
            const auto size                                 = ranges::ssize(expected_rng);
            const same_as<range_difference_t<V>> auto diff1 = i - s;
            assert(diff1 == -size);
            const same_as<range_difference_t<V>> auto diff2 = s - i;
            assert(diff2 == size);
        }
    }

    // Check adjacent_transform_view::iterator<const>
    if constexpr (CanMemberBegin<const R>) {
        using CBI = iterator_t<const V>;
        using CI  = iterator_t<const R>;
        STATIC_ASSERT(forward_iterator<CI>);

        // Check iterator_category
        using IterCat = typename CI::iterator_category;
        if constexpr (!is_reference_v<invoke_result_with_repeated_type<Fn, range_reference_t<const V>, N>>) {
            STATIC_ASSERT(same_as<IterCat, input_iterator_tag>);
        } else {
            using BaseCat = typename iterator_traits<CBI>::iterator_category;
            if constexpr (derived_from<BaseCat, random_access_iterator_tag>) {
                STATIC_ASSERT(same_as<IterCat, random_access_iterator_tag>);
            } else if constexpr (derived_from<BaseCat, bidirectional_iterator_tag>) {
                STATIC_ASSERT(same_as<IterCat, bidirectional_iterator_tag>);
            } else if constexpr (derived_from<BaseCat, forward_iterator_tag>) {
                STATIC_ASSERT(same_as<IterCat, forward_iterator_tag>);
            } else {
                STATIC_ASSERT(same_as<IterCat, input_iterator_tag>);
            }
        }

        // Check iterator_concept
        using IterConcept = typename CI::iterator_concept;
        STATIC_ASSERT(same_as<IterConcept, typename iterator_t<const adjacent_view<V, N>>::iterator_concept>);

        // Check value_type
        STATIC_ASSERT(same_as<typename CI::value_type,
            remove_cvref_t<invoke_result_with_repeated_type<const Fn&, range_reference_t<const V>, N>>>);

        // Check default-initializability
        STATIC_ASSERT(default_initializable<CI>);

        iterator_t<R> i = r.begin();

        // Check construction from non-const iterator
        CI ci = i;

        { // Check dereference
            same_as<invoke_result_with_repeated_type<const Fn&, range_reference_t<const V>, N>> decltype(auto) v =
                *as_const(ci);
            assert(v == expected_rng[0]);
        }

        { // Check pre-incrementation
            same_as<CI&> decltype(auto) ci2 = ++ci;
            assert(&ci2 == &ci);
            if (ci != r.end()) {
#pragma warning(suppress : 28020) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call
                assert(*ci == expected_rng[1]);
            }
            ci = r.begin();
        }

        { // Check post-incrementation
            same_as<CI> decltype(auto) ci2 = ci++;
            assert(*ci2 == expected_rng[0]);
            if (ci != r.end()) {
#pragma warning(suppress : 28020) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call
                assert(*ci == expected_rng[1]);
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
                assert(*ci2 == expected_rng[0]);
            }

            { // Check post-decrementation
                ci = ranges::next(r.begin());

                same_as<CI> decltype(auto) ci2 = ci--;
                if (ci2 != r.end()) {
#pragma warning(suppress : 28020) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call
                    assert(*ci2 == expected_rng[1]);
                }
                assert(*ci == expected_rng[0]);
            }
        }

        if constexpr (random_access_range<const V>) {
            { // Check advancement operators
                same_as<CI&> decltype(auto) ci2 = (ci += 1);
                assert(&ci2 == &ci);
                if (ci != r.end()) {
#pragma warning(suppress : 28020) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call
                    assert(*ci == expected_rng[1]);
                }

                same_as<CI&> decltype(auto) ci3 = (ci -= 1);
                assert(&ci3 == &ci);
                assert(*ci == expected_rng[0]);
            }

            { // Check subscript operator
                same_as<invoke_result_with_repeated_type<const Fn&, range_reference_t<const V>, N>> decltype(auto) v =
                    ci[0];
                assert(v == expected_rng[0]);
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
#pragma warning(suppress : 28020) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call
                    assert(*ci2 == expected_rng[1]);
                }

                same_as<CI> auto ci3 = 1 + ci;
                if (ci3 != r.end()) {
#pragma warning(suppress : 28020) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call
                    assert(*ci3 == expected_rng[1]);
                }
            }

            { // Check operator-(Iter, Diff)
                same_as<CI> auto ci2 = ranges::next(ci) - 1;
                assert(*ci2 == expected_rng[0]);
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
            const auto size                                       = ranges::ssize(expected_rng);
            const same_as<range_difference_t<const V>> auto diff1 = ci - s;
            assert(diff1 == -size);
            const same_as<range_difference_t<const V>> auto diff2 = s - ci;
            assert(diff2 == size);
        }

        if constexpr (sized_sentinel_for<sentinel_t<const R>, CI>) { // Check differencing with sentinel<const>
            const auto s                                          = as_const(r).end();
            const auto size                                       = ranges::ssize(expected_rng);
            const same_as<range_difference_t<const V>> auto diff1 = ci - s;
            assert(diff1 == -size);
            const same_as<range_difference_t<const V>> auto diff2 = s - ci;
            assert(diff2 == size);
        }
    }

    // Check adjacent_transform_view::base() const&
    STATIC_ASSERT(CanMemberBase<const R&> == copy_constructible<V>);
    if constexpr (copy_constructible<V>) {
        [[maybe_unused]] same_as<V> auto b1 = as_const(r).base();
        STATIC_ASSERT(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>); // strengthened
    }

    // Check adjacent_transform_view::base() &&
    [[maybe_unused]] same_as<V> auto b2 = move(r).base();
    STATIC_ASSERT(noexcept(move(r).base()) == is_nothrow_move_constructible_v<V>); // strengthened

    return true;
}

template <ranges::input_range Rng>
constexpr void test_adjacent0(Rng&& rng) {
    auto func = [] { return 602; };
    using Fn  = decltype(func);

    using V = views::all_t<Rng>;
    using E = ranges::empty_view<int>;

    // Check range closure object
    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;
    const auto closure     = views::adjacent_transform<0>(func);

    // ... with lvalue argument
    STATIC_ASSERT(CanViewAdjacentTransform<Rng&, Fn, 0> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewAdjacentTransform<Rng&, Fn, 0>) {
        STATIC_ASSERT(same_as<decltype(views::adjacent_transform<0>(rng, func)), E>);
        STATIC_ASSERT(noexcept(views::adjacent_transform<0>(rng, func)) == is_nothrow_copy_constructible_v<Fn>);

        STATIC_ASSERT(same_as<decltype(closure(rng)), E>);
        STATIC_ASSERT(noexcept(closure(rng)) == is_nothrow_copy_constructible_v<Fn>);

        STATIC_ASSERT(same_as<decltype(rng | closure), E>);
        STATIC_ASSERT(noexcept(rng | closure) == is_nothrow_copy_constructible_v<Fn>);
    }

    // ... with const lvalue argument
    STATIC_ASSERT(
        CanViewAdjacentTransform<const remove_reference_t<Rng>&, Fn, 0> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewAdjacentTransform<const remove_reference_t<Rng>&, Fn, 0>) {
        STATIC_ASSERT(same_as<decltype(views::adjacent_transform<0>(as_const(rng), func)), E>);
        STATIC_ASSERT(
            noexcept(views::adjacent_transform<0>(as_const(rng), func)) == is_nothrow_copy_constructible_v<Fn>);

        STATIC_ASSERT(same_as<decltype(closure(as_const(rng))), E>);
        STATIC_ASSERT(noexcept(closure(as_const(rng))) == is_nothrow_copy_constructible_v<Fn>);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | closure), E>);
        STATIC_ASSERT(noexcept(as_const(rng) | closure) == is_nothrow_copy_constructible_v<Fn>);
    }

    // ... with rvalue argument
    STATIC_ASSERT(
        CanViewAdjacentTransform<remove_reference_t<Rng>, Fn, 0> == (is_view || movable<remove_reference_t<Rng>>) );
    if constexpr (CanViewAdjacentTransform<remove_reference_t<Rng>, Fn, 0>) {
        STATIC_ASSERT(same_as<decltype(views::adjacent_transform<0>(move(rng), func)), E>);
        STATIC_ASSERT(noexcept(views::adjacent_transform<0>(move(rng), func)) == is_nothrow_copy_constructible_v<Fn>);

        STATIC_ASSERT(same_as<decltype(closure(move(rng))), E>);
        STATIC_ASSERT(noexcept(closure(move(rng))) == is_nothrow_copy_constructible_v<Fn>);

        STATIC_ASSERT(same_as<decltype(move(rng) | closure), E>);
        STATIC_ASSERT(noexcept(move(rng) | closure) == is_nothrow_copy_constructible_v<Fn>);
    }

    // ... with const rvalue argument
    STATIC_ASSERT(
        CanViewAdjacentTransform<const remove_reference_t<Rng>, Fn, 0> == (is_view && copy_constructible<V>) );
    if constexpr (CanViewAdjacentTransform<const remove_reference_t<Rng>, Fn, 0>) {
        STATIC_ASSERT(same_as<decltype(views::adjacent_transform<0>(move(as_const(rng)), func)), E>);
        STATIC_ASSERT(
            noexcept(views::adjacent_transform<0>(move(as_const(rng)), func)) == is_nothrow_copy_constructible_v<Fn>);

        STATIC_ASSERT(same_as<decltype(closure(move(as_const(rng)))), E>);
        STATIC_ASSERT(noexcept(closure(move(as_const(rng)))) == is_nothrow_copy_constructible_v<Fn>);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | closure), E>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | closure) == is_nothrow_copy_constructible_v<Fn>);
    }
}

// We have to use std::array instead of C-array, see LLVM-61025
constexpr auto some_ints        = array{1, 3, 5, 7, 11, 13};
constexpr auto adjacent1_fn     = [](auto&& val) -> const int& { return val; };
constexpr auto adjacent1_result = array{1, 3, 5, 7, 11, 13};
constexpr auto pairwise_fn      = plus<int>{};
constexpr auto pairwise_result  = array{4, 8, 12, 18, 24};
constexpr auto adjacent3_fn     = [](auto... vals) { return (vals * ...); };
constexpr auto adjacent3_result = array{15, 105, 385, 1001};
constexpr auto adjacent4_fn     = [](auto... vals) { return (vals | ...); };
constexpr auto adjacent4_result = array{7, 15, 15};
constexpr auto adjacent5_fn     = [](auto... vals) { return (vals - ...); };
constexpr auto adjacent5_result = array{7, 7};
constexpr auto adjacent6_fn     = [](auto... vals) { return (... - vals); };
constexpr auto adjacent6_result = array{-38};
constexpr auto adjacent7_fn     = [](auto...) { return 0; };
constexpr array<int, 0> adjacent7_result{};

template <class Category, test::Common IsCommon, test::Sized IsSized>
using test_range =
    test::range<Category, const int, IsSized, test::CanDifference{derived_from<Category, random_access_iterator_tag>},
        IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
        test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}>;

struct instantiator {
    template <ranges::input_range R>
    static constexpr void call() {
        R r{some_ints};
        test_one<1>(r, adjacent1_fn, adjacent1_result);
        test_one<2>(r, pairwise_fn, pairwise_result);
#ifndef _M_IX86 // fatal error C1060: compiler is out of heap space
        test_one<4>(r, adjacent4_fn, adjacent4_result);
#endif // !defined(_M_IX86)
        test_one<7>(r, adjacent7_fn, adjacent7_result);
        test_adjacent0(r);
    }
};

constexpr void instantiation_test() {
#ifdef TEST_EVERYTHING
    test_fwd<instantiator, const int>();
#else // ^^^ test all forward range permutations / test only "interesting" permutations vvv
    using test::Common, test::Sized;

    // The view is sensitive to category, commonality, and size, but oblivious to proxyness and differencing
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
#endif // TEST_EVERYTHING
}

template <class Category, test::Common IsCommon, bool IsRandom = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{IsRandom}, test::CanDifference{IsRandom}, IsCommon,
    test::CanCompare::yes, test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes,
    test::Copyability::move_only>;

int main() {
    { // Check views
        // ... copyable
        constexpr span<const int> s{some_ints};
        STATIC_ASSERT(test_one<4>(s, adjacent4_fn, adjacent4_result));
        test_one<4>(s, adjacent4_fn, adjacent4_result);
    }

    { // ... move-only
        test_one<1>(move_only_view<forward_iterator_tag, test::Common::no>{some_ints}, adjacent1_fn, adjacent1_result);
        test_one<2>(move_only_view<forward_iterator_tag, test::Common::yes>{some_ints}, pairwise_fn, pairwise_result);
        test_one<3>(
            move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints}, adjacent3_fn, adjacent3_result);
        test_one<4>(
            move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints}, adjacent4_fn, adjacent4_result);
        test_one<5>(
            move_only_view<random_access_iterator_tag, test::Common::no>{some_ints}, adjacent5_fn, adjacent5_result);
        test_one<6>(
            move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints}, adjacent6_fn, adjacent6_result);
    }

    { // Check non-views
        STATIC_ASSERT(test_one<2>(some_ints, pairwise_fn, pairwise_result));
        test_one<2>(some_ints, pairwise_fn, pairwise_result);

        auto vec = some_ints | ranges::to<vector>();
        test_one<3>(vec, adjacent3_fn, adjacent3_result);

        auto lst = some_ints | ranges::to<forward_list>();
        test_one<4>(lst, adjacent4_fn, adjacent4_result);
    }

    { // Check empty range
        constexpr auto to_float = [](int x, int y, int z) { return static_cast<float>(x + y + z); };
        STATIC_ASSERT(test_one<3>(span<const int>{}, to_float, span<float>{}));
        test_one<3>(span<const int>{}, to_float, span<float>{});
    }

    STATIC_ASSERT((instantiation_test(), true));
    instantiation_test();
}
