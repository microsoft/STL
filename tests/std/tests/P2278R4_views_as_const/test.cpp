// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <forward_list>
#include <iterator>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

template <class Rng>
concept CanViewAsConst = requires(Rng&& r) { views::as_const(forward<Rng>(r)); };

template <class>
struct RefViewUnderlyingType {};

template <class T>
struct RefViewUnderlyingType<ranges::ref_view<T>> {
    using type = T;
};

template <class>
constexpr bool CanReconstructRefView = false;

template <class T>
constexpr bool CanReconstructRefView<ranges::ref_view<T>> = ranges::constant_range<const T>;

template <ranges::input_range Rng, class Expected>
constexpr bool test_one(Rng&& rng, Expected&& expected) {
    using ranges::ref_view, ranges::as_const_view, ranges::begin, ranges::end, ranges::iterator_t, ranges::sentinel_t,
        ranges::prev, ranges::forward_range, ranges::bidirectional_range, ranges::random_access_range,
        ranges::contiguous_range, ranges::common_range, ranges::sized_range, ranges::constant_range;

    using V = views::all_t<Rng>;
    using R = as_const_view<V>;

    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;

    STATIC_ASSERT(ranges::view<R>);
    STATIC_ASSERT(ranges::input_range<R> == ranges::input_range<V>);
    STATIC_ASSERT(forward_range<R> == forward_range<V>);
    STATIC_ASSERT(bidirectional_range<R> == bidirectional_range<V>);
    STATIC_ASSERT(random_access_range<R> == random_access_range<V>);
    STATIC_ASSERT(contiguous_range<R>
                  == (contiguous_range<V> && is_lvalue_reference_v<iter_const_reference_t<ranges::iterator_t<V>>>) );
    STATIC_ASSERT(constant_range<R>);

    STATIC_ASSERT(!indirectly_writable<iterator_t<R>, ranges::range_value_t<Rng>>);
    STATIC_ASSERT(!indirectly_writable<iterator_t<R>, ranges::range_reference_t<Rng>>);

    // Validate default-initializability
    STATIC_ASSERT(default_initializable<R> == default_initializable<V>);

    // Validate borrowed_range
    STATIC_ASSERT(ranges::borrowed_range<R> == ranges::borrowed_range<V>);

    // Validate range adaptor object
    if constexpr (constant_range<Rng>) { // range adaptor results in views::all_t
        // ... with lvalue argument
        STATIC_ASSERT(CanViewAsConst<Rng&> == (!is_view || copy_constructible<V>) );
        if constexpr (CanViewAsConst<Rng&>) {
            constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

            STATIC_ASSERT(same_as<decltype(views::as_const(forward<Rng>(rng))), V>);
            STATIC_ASSERT(noexcept(views::as_const(forward<Rng>(rng))) == is_noexcept);

            STATIC_ASSERT(same_as<decltype(forward<Rng>(rng) | views::as_const), V>);
            STATIC_ASSERT(noexcept(forward<Rng>(rng) | views::as_const) == is_noexcept);
        }

        // ... with const lvalue argument
        STATIC_ASSERT(CanViewAsConst<const remove_reference_t<Rng>&> == (!is_view || copy_constructible<V>) );
        if constexpr (CanViewAsConst<const remove_reference_t<Rng>&>) {
            using VC                   = views::all_t<const remove_reference_t<Rng>&>;
            constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

            STATIC_ASSERT(same_as<decltype(views::as_const(as_const(rng))), VC>);
            STATIC_ASSERT(noexcept(views::as_const(as_const(rng))) == is_noexcept);

            STATIC_ASSERT(same_as<decltype(as_const(rng) | views::as_const), VC>);
            STATIC_ASSERT(noexcept(as_const(rng) | views::as_const) == is_noexcept);
        }

        // ... with rvalue argument
        STATIC_ASSERT(CanViewAsConst<remove_reference_t<Rng>> == (is_view || movable<remove_reference_t<Rng>>) );
        if constexpr (CanViewAsConst<remove_reference_t<Rng>>) {
            using VS                   = views::all_t<remove_reference_t<Rng>>;
            constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;

            STATIC_ASSERT(same_as<decltype(views::as_const(move(rng))), VS>);
            STATIC_ASSERT(noexcept(views::as_const(move(rng))) == is_noexcept);

            STATIC_ASSERT(same_as<decltype(move(rng) | views::as_const), VS>);
            STATIC_ASSERT(noexcept(move(rng) | views::as_const) == is_noexcept);
        }

        // ... with const rvalue argument
        STATIC_ASSERT(CanViewAsConst<const remove_reference_t<Rng>> == (is_view && copy_constructible<V>) );
        if constexpr (CanViewAsConst<const remove_reference_t<Rng>>) {
            constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

            STATIC_ASSERT(same_as<decltype(views::as_const(move(as_const(rng)))), V>);
            STATIC_ASSERT(noexcept(views::as_const(move(as_const(rng)))) == is_noexcept);

            STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | views::as_const), V>);
            STATIC_ASSERT(noexcept(move(as_const(rng)) | views::as_const) == is_noexcept);
        }
    } else if constexpr (_Is_specialization_v<remove_cvref_t<Rng>, ranges::empty_view>) {
        // range adaptor results in empty_view<const X> reconstructed from empty_view<X>
        using ConstEmpty = ranges::empty_view<const remove_reference_t<ranges::range_reference_t<Rng>>>;

        { // ... with lvalue argument
            STATIC_ASSERT(same_as<decltype(views::as_const(forward<Rng>(rng))), ConstEmpty>);
            STATIC_ASSERT(noexcept(views::as_const(forward<Rng>(rng))));

            STATIC_ASSERT(same_as<decltype(forward<Rng>(rng) | views::as_const), ConstEmpty>);
            STATIC_ASSERT(noexcept(forward<Rng>(rng) | views::as_const));
        }

        { // ... with const lvalue argument
            STATIC_ASSERT(same_as<decltype(views::as_const(as_const(rng))), ConstEmpty>);
            STATIC_ASSERT(noexcept(views::as_const(as_const(rng))));

            STATIC_ASSERT(same_as<decltype(as_const(rng) | views::as_const), ConstEmpty>);
            STATIC_ASSERT(noexcept(as_const(rng) | views::as_const));
        }

        { // ... with rvalue argument
            STATIC_ASSERT(same_as<decltype(views::as_const(move(rng))), ConstEmpty>);
            STATIC_ASSERT(noexcept(views::as_const(move(rng))));

            STATIC_ASSERT(same_as<decltype(move(rng) | views::as_const), ConstEmpty>);
            STATIC_ASSERT(noexcept(move(rng) | views::as_const));
        }

        { // ... with const rvalue argument
            STATIC_ASSERT(same_as<decltype(views::as_const(move(as_const(rng)))), ConstEmpty>);
            STATIC_ASSERT(noexcept(views::as_const(move(as_const(rng)))));

            STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | views::as_const), ConstEmpty>);
            STATIC_ASSERT(noexcept(move(as_const(rng)) | views::as_const));
        }
    } else if constexpr (_Is_span_v<Rng>) { // range adaptor results in span<const X, E> reconstructed from span<X, E>
        using ConstSpan = span<const typename V::element_type, V::extent>;

        { // ... with lvalue argument
            STATIC_ASSERT(same_as<decltype(views::as_const(forward<Rng>(rng))), ConstSpan>);
            STATIC_ASSERT(noexcept(views::as_const(forward<Rng>(rng))));

            STATIC_ASSERT(same_as<decltype(forward<Rng>(rng) | views::as_const), ConstSpan>);
            STATIC_ASSERT(noexcept(forward<Rng>(rng) | views::as_const));
        }

        { // ... with const lvalue argument
            STATIC_ASSERT(same_as<decltype(views::as_const(as_const(rng))), ConstSpan>);
            STATIC_ASSERT(noexcept(views::as_const(as_const(rng))));

            STATIC_ASSERT(same_as<decltype(as_const(rng) | views::as_const), ConstSpan>);
            STATIC_ASSERT(noexcept(as_const(rng) | views::as_const));
        }

        { // ... with rvalue argument
            STATIC_ASSERT(same_as<decltype(views::as_const(move(rng))), ConstSpan>);
            STATIC_ASSERT(noexcept(views::as_const(move(rng))));

            STATIC_ASSERT(same_as<decltype(move(rng) | views::as_const), ConstSpan>);
            STATIC_ASSERT(noexcept(move(rng) | views::as_const));
        }

        { // ... with const rvalue argument
            STATIC_ASSERT(same_as<decltype(views::as_const(move(as_const(rng)))), ConstSpan>);
            STATIC_ASSERT(noexcept(views::as_const(move(as_const(rng)))));

            STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | views::as_const), ConstSpan>);
            STATIC_ASSERT(noexcept(move(as_const(rng)) | views::as_const));
        }
    } else if constexpr (CanReconstructRefView<Rng>) {
        // range adaptor results in ref_view<const X> reconstructed from ref_view<X>
        using ReconstructedRefView = ref_view<const typename RefViewUnderlyingType<V>::type>;

        { // ... with lvalue argument
            STATIC_ASSERT(same_as<decltype(views::as_const(forward<Rng>(rng))), ReconstructedRefView>);
            STATIC_ASSERT(noexcept(views::as_const(forward<Rng>(rng))));

            STATIC_ASSERT(same_as<decltype(forward<Rng>(rng) | views::as_const), ReconstructedRefView>);
            STATIC_ASSERT(noexcept(forward<Rng>(rng) | views::as_const));
        }

        { // ... with const lvalue argument
            STATIC_ASSERT(same_as<decltype(views::as_const(as_const(rng))), ReconstructedRefView>);
            STATIC_ASSERT(noexcept(views::as_const(as_const(rng))));

            STATIC_ASSERT(same_as<decltype(as_const(rng) | views::as_const), ReconstructedRefView>);
            STATIC_ASSERT(noexcept(as_const(rng) | views::as_const));
        }

        { //... with rvalue argument
            STATIC_ASSERT(same_as<decltype(views::as_const(move(rng))), ReconstructedRefView>);
            STATIC_ASSERT(noexcept(views::as_const(move(rng))));

            STATIC_ASSERT(same_as<decltype(move(rng) | views::as_const), ReconstructedRefView>);
            STATIC_ASSERT(noexcept(move(rng) | views::as_const));
        }

        { // ... with const rvalue argument
            STATIC_ASSERT(same_as<decltype(views::as_const(move(as_const(rng)))), ReconstructedRefView>);
            STATIC_ASSERT(noexcept(views::as_const(move(as_const(rng)))));

            STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | views::as_const), ReconstructedRefView>);
            STATIC_ASSERT(noexcept(move(as_const(rng)) | views::as_const));
        }
    } else if constexpr (is_lvalue_reference_v<Rng> && constant_range<const remove_cvref_t<Rng>>
                         && !is_view) { // range adaptor results in ref_view<const X>
        using ConstRefView = ranges::ref_view<const remove_cvref_t<Rng>>;

        // ... with lvalue argument
        STATIC_ASSERT(CanViewAsConst<Rng&> == copy_constructible<V>);
        if constexpr (CanViewAsConst<Rng&>) {
            STATIC_ASSERT(same_as<decltype(views::as_const(forward<Rng>(rng))), ConstRefView>);
            STATIC_ASSERT(noexcept(views::as_const(forward<Rng>(rng))));

            STATIC_ASSERT(same_as<decltype(forward<Rng>(rng) | views::as_const), ConstRefView>);
            STATIC_ASSERT(noexcept(forward<Rng>(rng) | views::as_const));
        }

        // ... with const lvalue argument
        STATIC_ASSERT(CanViewAsConst<const remove_reference_t<Rng>&> == copy_constructible<V>);
        if constexpr (CanViewAsConst<const remove_reference_t<Rng>&>) {
            STATIC_ASSERT(same_as<decltype(views::as_const(as_const(rng))), ConstRefView>);
            STATIC_ASSERT(noexcept(views::as_const(as_const(rng))));

            STATIC_ASSERT(same_as<decltype(as_const(rng) | views::as_const), ConstRefView>);
            STATIC_ASSERT(noexcept(as_const(rng) | views::as_const));
        }
    } else { // range adaptor results in as_const_view
        // ... with lvalue argument
        STATIC_ASSERT(CanViewAsConst<Rng&> == (!is_view || copy_constructible<V>) );
        if constexpr (CanViewAsConst<Rng&>) {
            constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

            STATIC_ASSERT(same_as<decltype(views::as_const(forward<Rng>(rng))), R>);
            STATIC_ASSERT(noexcept(views::as_const(forward<Rng>(rng))) == is_noexcept);

            STATIC_ASSERT(same_as<decltype(forward<Rng>(rng) | views::as_const), R>);
            STATIC_ASSERT(noexcept(forward<Rng>(rng) | views::as_const) == is_noexcept);
        }

        // ... with const lvalue argument
        STATIC_ASSERT(CanViewAsConst<const remove_reference_t<Rng>&> == (!is_view || copy_constructible<V>) );
        if constexpr (CanViewAsConst<const remove_reference_t<Rng>&>
                      && !constant_range<const remove_reference_t<Rng>&>) {
            using RC                   = as_const_view<views::all_t<const remove_reference_t<Rng>&>>;
            constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

            STATIC_ASSERT(same_as<decltype(views::as_const(as_const(rng))), RC>);
            STATIC_ASSERT(noexcept(views::as_const(as_const(rng))) == is_noexcept);

            STATIC_ASSERT(same_as<decltype(as_const(rng) | views::as_const), RC>);
            STATIC_ASSERT(noexcept(as_const(rng) | views::as_const) == is_noexcept);
        }

        // ... with rvalue argument
        STATIC_ASSERT(CanViewAsConst<remove_reference_t<Rng>> == (is_view || movable<remove_reference_t<Rng>>) );
        if constexpr (CanViewAsConst<remove_reference_t<Rng>>) {
            using RS                   = as_const_view<views::all_t<remove_reference_t<Rng>>>;
            constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;

            STATIC_ASSERT(same_as<decltype(views::as_const(move(rng))), RS>);
            STATIC_ASSERT(noexcept(views::as_const(move(rng))) == is_noexcept);

            STATIC_ASSERT(same_as<decltype(move(rng) | views::as_const), RS>);
            STATIC_ASSERT(noexcept(move(rng) | views::as_const) == is_noexcept);
        }

        // ... with const rvalue argument
        STATIC_ASSERT(CanViewAsConst<const remove_reference_t<Rng>> == (is_view && copy_constructible<V>) );
        if constexpr (CanViewAsConst<const remove_reference_t<Rng>>) {
            constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

            STATIC_ASSERT(same_as<decltype(views::as_const(move(as_const(rng)))), R>);
            STATIC_ASSERT(noexcept(views::as_const(move(as_const(rng)))) == is_noexcept);

            STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | views::as_const), R>);
            STATIC_ASSERT(noexcept(move(as_const(rng)) | views::as_const) == is_noexcept);
        }
    }

    // Validate deduction guide
    same_as<R> auto r = as_const_view{forward<Rng>(rng)};

    // Validate as_const_view::size
    STATIC_ASSERT(CanMemberSize<R> == sized_range<V>);
    if constexpr (CanMemberSize<R>) {
        same_as<ranges::range_size_t<V>> auto s = r.size();
        assert(_To_unsigned_like(s) == ranges::size(expected));
        STATIC_ASSERT(noexcept(r.size()) == noexcept(ranges::size(as_const(rng))));
    }

    // Validate as_const_view::size (const)
    STATIC_ASSERT(CanMemberSize<const R> == sized_range<const V>);
    if constexpr (CanMemberSize<const R>) {
        same_as<ranges::range_size_t<const V>> auto s = as_const(r).size();
        assert(_To_unsigned_like(s) == ranges::size(expected));
        STATIC_ASSERT(noexcept(as_const(r).size()) == noexcept(ranges::size(rng)));
    }

    const bool is_empty = ranges::empty(expected);

    // Validate view_interface::empty and operator bool
    STATIC_ASSERT(CanMemberEmpty<R> == (forward_range<V> || sized_range<V>) );
    STATIC_ASSERT(CanBool<R> == CanEmpty<R>);
    if constexpr (CanMemberEmpty<R>) {
        assert(r.empty() == is_empty);
        assert(static_cast<bool>(r) == !is_empty);
    }

    // Validate view_interface::empty and operator bool (const)
    STATIC_ASSERT(CanMemberEmpty<const R> == (forward_range<const Rng> || sized_range<const V>) );
    STATIC_ASSERT(CanBool<const R> == CanEmpty<const R>);
    if constexpr (CanMemberEmpty<const R>) {
        assert(as_const(r).empty() == is_empty);
        assert(static_cast<bool>(as_const(r)) == !is_empty);
    }

    assert(ranges::equal(r, expected));
    if (!forward_range<V>) { // intentionally not if constexpr
        return true;
    }

    // Validate as_const_view::begin
    STATIC_ASSERT(CanMemberBegin<R>);
    {
        const same_as<iterator_t<R>> auto i = r.begin();
        if (!is_empty) {
            // (static analyzer doesn't realize that `i == nullptr` implies `is_empty`)
#pragma warning(suppress : 6011) // Dereferencing NULL pointer 'i'
            assert(*i == *begin(expected));
        }

        if constexpr (copy_constructible<V>) {
            auto r2                              = r;
            const same_as<iterator_t<R>> auto i2 = r2.begin();
            if (!is_empty) {
#pragma warning(suppress : 6011) // Dereferencing NULL pointer 'i'
                assert(*i2 == *i);
            }
        }
    }

    // Validate as_const_view::begin (const)
    STATIC_ASSERT(CanMemberBegin<const R> == ranges::range<const V>);
    if constexpr (CanMemberBegin<const R>) {
        const same_as<iterator_t<const R>> auto ci = as_const(r).begin();
        if (!is_empty) {
#pragma warning(suppress : 6011) // Dereferencing NULL pointer 'i'
            assert(*ci == *begin(expected));
        }

        if constexpr (copy_constructible<V>) {
            const auto cr2                              = r;
            const same_as<iterator_t<const R>> auto ci2 = cr2.begin();
            if (!is_empty) {
#pragma warning(suppress : 6011) // Dereferencing NULL pointer 'i'
                assert(*ci2 == *ci);
            }
        }
    }

    // Validate as_const_view::end
    STATIC_ASSERT(CanMemberEnd<R>);
    {
        const same_as<sentinel_t<R>> auto s = r.end();
        assert((r.begin() == s) == is_empty);
        STATIC_ASSERT(common_range<R> == common_range<V>);
        if constexpr (common_range<R> && bidirectional_range<V>) {
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

    // Validate as_const_view::end (const)
    STATIC_ASSERT(CanMemberEnd<const R> == ranges::range<const V>);
    if constexpr (CanMemberEnd<const R>) {
        const same_as<sentinel_t<const R>> auto cs = as_const(r).end();
        assert((as_const(r).begin() == cs) == is_empty);
        STATIC_ASSERT(common_range<const R> == common_range<const V>);
        if constexpr (common_range<const R> && bidirectional_range<const V>) {
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

    // Validate view_interface::data
    STATIC_ASSERT(CanMemberData<R> == contiguous_range<R>);
    STATIC_ASSERT(CanData<R&> == contiguous_range<R>);
    if constexpr (contiguous_range<R>) {
        const same_as<const remove_reference_t<ranges::range_reference_t<V>>*> auto ptr = r.data();
        assert(ptr == to_address(r.begin()));
    }

    // Validate view_interface::data (const)
    STATIC_ASSERT(CanMemberData<const R> == contiguous_range<const R>);
    STATIC_ASSERT(CanData<const R&> == contiguous_range<const R>);
    if constexpr (contiguous_range<const R>) {
        const same_as<const remove_reference_t<ranges::range_reference_t<const V>>*> auto ptr = as_const(r).data();
        assert(ptr == to_address(as_const(r).begin()));
    }

    if (!is_empty) {
        // Validate view_interface::operator[]
        STATIC_ASSERT(CanIndex<R> == random_access_range<V>);
        if constexpr (CanIndex<R>) {
            assert(r[0] == expected[0]);
        }

        // Validate view_interface::operator[] (const)
        STATIC_ASSERT(CanIndex<const R> == random_access_range<const V>);
        if constexpr (CanIndex<const R>) {
            assert(as_const(r)[0] == expected[0]);
        }

        // Validate view_interface::front
        STATIC_ASSERT(CanMemberFront<R> == forward_range<V>);
        if constexpr (CanMemberFront<R>) {
            assert(r.front() == *begin(expected));
        }

        // Validate view_interface::front (const)
        STATIC_ASSERT(CanMemberFront<const R> == forward_range<const V>);
        if constexpr (CanMemberFront<const R>) {
            assert(as_const(r).front() == *begin(expected));
        }

        // Validate view_interface::back
        STATIC_ASSERT(CanMemberBack<R> == (bidirectional_range<V> && common_range<V>) );
        if constexpr (CanMemberBack<R>) {
            assert(r.back() == *prev(end(expected)));
        }

        // Validate view_interface::back (const)
        STATIC_ASSERT(CanMemberBack<const R> == (bidirectional_range<const V> && common_range<const V>) );
        if constexpr (CanMemberBack<const R>) {
            assert(as_const(r).back() == *prev(end(expected)));
        }
    }

    // Validate as_const_view::base() const&
    STATIC_ASSERT(CanMemberBase<const R&> == copy_constructible<V>);
    if constexpr (copy_constructible<V>) {
        same_as<V> auto b1 = as_const(r).base();
        STATIC_ASSERT(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>);
        if (!is_empty) {
            assert(*b1.begin() == *begin(expected));
        }
    }

    // Validate as_const_view::base() &&
    same_as<V> auto b2 = move(r).base();
    STATIC_ASSERT(noexcept(move(r).base()) == is_nothrow_move_constructible_v<V>);
    if (!is_empty) {
        assert(*b2.begin() == *begin(expected));
    }

    return true;
}

constexpr int some_ints[] = {0, 3, 6, 9, 12, 15};

struct instantiator {
    template <ranges::input_range R>
    static constexpr void call() {
        R r{some_ints};
        test_one(r, span{some_ints});
    }
};

template <class Category, test::Common IsCommon, test::Sized IsSized>
using test_range =
    test::range<Category, const int, IsSized, test::CanDifference{derived_from<Category, random_access_iterator_tag>},
        IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
        test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}>;

constexpr bool instantiation_test() {
#ifdef TEST_EVERYTHING
    test_in<instantiator, const int>();
#else // ^^^ test all input permutations / test only "interesting" permutations vvv
    using test::Common, test::Sized;

    // The view is sensitive to category, commonality, and size, but oblivious to differencing and proxyness
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
#endif // TEST_EVERYTHING
    return true;
}

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare::yes, test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>},
    test::CanView::yes, test::Copyability::move_only>;

int main() {
#ifndef __clang__ // TRANSITION, LLVM-44833
    { // Validate views
        // ... copyable
        constexpr span<const int> s{some_ints};
        STATIC_ASSERT(test_one(s, some_ints));
        test_one(s, some_ints);
    }
#endif // TRANSITION, LLVM-44833

    { // ... move-only
        test_one(move_only_view<input_iterator_tag, test::Common::no>{some_ints}, some_ints);
        test_one(move_only_view<input_iterator_tag, test::Common::yes>{some_ints}, some_ints);
        test_one(move_only_view<forward_iterator_tag, test::Common::no>{some_ints}, some_ints);
        test_one(move_only_view<forward_iterator_tag, test::Common::yes>{some_ints}, some_ints);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints}, some_ints);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints}, some_ints);
        test_one(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints}, some_ints);
        test_one(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints}, some_ints);
    }

    { // Validate non-views
        STATIC_ASSERT(test_one(some_ints, some_ints));
        test_one(some_ints, some_ints);

        // Test with lvalue, rvalue, and wrapped in ref_view non-views
        auto vec = some_ints | ranges::to<vector>();
        test_one(vec, some_ints);
        test_one(ranges::ref_view{vec}, some_ints);
        test_one(some_ints | ranges::to<vector>(), some_ints);

        auto lst = some_ints | ranges::to<forward_list>();
        test_one(lst, some_ints);
        test_one(ranges::ref_view{lst}, some_ints);
        test_one(some_ints | ranges::to<forward_list>(), some_ints);
    }

    { // Validate single_view
        static constexpr int one_int[1] = {333};
        STATIC_ASSERT(test_one(views::single(333), one_int));
        test_one(views::single(333), one_int);
    }

    { // Validate empty_view
        array<int, 0> empty_arr;

        STATIC_ASSERT(test_one(views::empty<int>, empty_arr));
        test_one(views::empty<int>, empty_arr);
        STATIC_ASSERT(test_one(as_const(views::empty<int>), empty_arr));
        test_one(as_const(views::empty<int>), empty_arr);

        STATIC_ASSERT(test_one(views::empty<const int>, empty_arr));
        test_one(views::empty<const int>, empty_arr);
        STATIC_ASSERT(test_one(as_const(views::empty<const int>), empty_arr));
        test_one(as_const(views::empty<const int>), empty_arr);

        STATIC_ASSERT(test_one(views::empty<volatile int>, empty_arr));
        test_one(views::empty<volatile int>, empty_arr);
        STATIC_ASSERT(test_one(as_const(views::empty<volatile int>), empty_arr));
        test_one(as_const(views::empty<volatile int>), empty_arr);

        STATIC_ASSERT(test_one(views::empty<const volatile int>, empty_arr));
        test_one(views::empty<const volatile int>, empty_arr);
        STATIC_ASSERT(test_one(as_const(views::empty<const volatile int>), empty_arr));
        test_one(as_const(views::empty<const volatile int>), empty_arr);
    }

#ifndef __clang__ // TRANSITION, LLVM-44833
    { // empty range
        using Span = span<int>;
        STATIC_ASSERT(test_one(Span{}, Span{}));
        test_one(Span{}, Span{});
    }
#endif // TRANSITION, LLVM-44833

    STATIC_ASSERT(instantiation_test());
    instantiation_test();
}
