// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <forward_list>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>
using namespace std;
using P = pair<int, int>;

constexpr auto pipeline = views::elements<0>() | views::all;

template <class Rng, class V = views::all_t<Rng>>
using pipeline_t = ranges::elements_view<V, 0>;

template <class Rng>
concept CanViewElements = requires(Rng&& r) {
    views::elements<0>(static_cast<Rng&&>(r));
};

template <ranges::input_range Rng, ranges::random_access_range Expected>
constexpr bool test_one(Rng&& rng, Expected&& expected_keys, Expected&& expected_values) {
    using ranges::elements_view, ranges::bidirectional_range, ranges::common_range, ranges::contiguous_range,
        ranges::enable_borrowed_range, ranges::forward_range, ranges::input_range, ranges::iterator_t, ranges::prev,
        ranges::random_access_range, ranges::range, ranges::range_reference_t, ranges::sentinel_t;

    using V  = views::all_t<Rng>;
    using EV = elements_view<V, 0>;
    STATIC_ASSERT(ranges::view<EV>);
    STATIC_ASSERT(input_range<EV>);
    STATIC_ASSERT(forward_range<EV> == forward_range<Rng>);
    STATIC_ASSERT(bidirectional_range<EV> == bidirectional_range<Rng>);
    STATIC_ASSERT(random_access_range<EV> == random_access_range<Rng>);
    STATIC_ASSERT(!contiguous_range<EV>);

    // Validate range adaptor object and range adaptor closure
    constexpr auto elements_incr = views::elements<0>();


    // ... with lvalue argument
    STATIC_ASSERT(CanViewElements<Rng&>);
    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;
    if constexpr (CanViewElements<Rng&>) { // Validate lvalue
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::elements<0>(rng)), EV>);
        STATIC_ASSERT(noexcept(views::elements<0>(rng)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(rng | elements_incr), EV>);
        STATIC_ASSERT(noexcept(rng | elements_incr) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(rng | pipeline), pipeline_t<Rng&>>);
        STATIC_ASSERT(noexcept(rng | pipeline) == is_noexcept);
    }

    // ... with const lvalue argument
    STATIC_ASSERT(CanViewElements<const remove_reference_t<Rng>&> == (!is_view || copyable<V>) );
    if constexpr (is_view && copyable<V>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::elements<0>(as_const(rng))), EV>);
        STATIC_ASSERT(noexcept(views::elements<0>(as_const(rng))) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | elements_incr), EV>);
        STATIC_ASSERT(noexcept(as_const(rng) | elements_incr) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | pipeline), pipeline_t<const remove_reference_t<Rng>&>>);
        STATIC_ASSERT(noexcept(as_const(rng) | pipeline) == is_noexcept);
    } else if constexpr (!is_view) {
        using RC                   = elements_view<views::all_t<const remove_reference_t<Rng>&>, 0>;
        constexpr bool is_noexcept = is_nothrow_constructible_v<RC, const remove_reference_t<Rng>&>;

        STATIC_ASSERT(same_as<decltype(views::elements<0>(as_const(rng))), RC>);
        STATIC_ASSERT(noexcept(views::elements<0>(as_const(rng))) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | elements_incr), RC>);
        STATIC_ASSERT(noexcept(as_const(rng) | elements_incr) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | pipeline), pipeline_t<const remove_reference_t<Rng>&>>);
        STATIC_ASSERT(noexcept(as_const(rng) | pipeline) == is_noexcept);
    }

    // ... with rvalue argument
    STATIC_ASSERT(CanViewElements<remove_reference_t<Rng>> == is_view || enable_borrowed_range<remove_cvref_t<Rng>>);
    if constexpr (is_view) {
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;
        STATIC_ASSERT(same_as<decltype(views::elements<0>(move(rng))), EV>);
        STATIC_ASSERT(noexcept(views::elements<0>(move(rng))) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | elements_incr), EV>);
        STATIC_ASSERT(noexcept(move(rng) | elements_incr) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | pipeline), pipeline_t<remove_reference_t<Rng>>>);
        STATIC_ASSERT(noexcept(move(rng) | pipeline) == is_noexcept);
    } else if constexpr (enable_borrowed_range<remove_cvref_t<Rng>>) {
        using S                    = decltype(ranges::subrange{move(rng)});
        using RS                   = elements_view<S, 0>;
        constexpr bool is_noexcept = noexcept(S{move(rng)});

        STATIC_ASSERT(same_as<decltype(views::elements<0>(move(rng))), RS>);
        STATIC_ASSERT(noexcept(views::elements<0>(move(rng))) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | elements_incr), RS>);
        STATIC_ASSERT(noexcept(move(rng) | elements_incr) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | pipeline), pipeline_t<remove_reference_t<Rng>>>);
        STATIC_ASSERT(noexcept(move(rng) | pipeline) == is_noexcept);
    }

    // ... with const rvalue argument
    STATIC_ASSERT(CanViewElements<const remove_reference_t<Rng>> == (is_view && copyable<V>)
                  || (!is_view && enable_borrowed_range<remove_cvref_t<Rng>>) );
    if constexpr (is_view && copyable<V>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::elements<0>(move(as_const(rng)))), EV>);
        STATIC_ASSERT(noexcept(views::elements<0>(move(as_const(rng)))) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | elements_incr), EV>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | elements_incr) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | pipeline), pipeline_t<const remove_reference_t<Rng>>>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | pipeline) == is_noexcept);
    } else if constexpr (!is_view && enable_borrowed_range<remove_cvref_t<Rng>>) {
        using S                    = decltype(ranges::subrange{move(as_const(rng))});
        using RS                   = elements_view<S, 0>;
        constexpr bool is_noexcept = noexcept(S{move(as_const(rng))});

        STATIC_ASSERT(same_as<decltype(views::elements<0>(move(as_const(rng)))), RS>);
        STATIC_ASSERT(noexcept(views::elements<0>(move(as_const(rng)))) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | elements_incr), RS>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | elements_incr) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | pipeline), pipeline_t<const remove_reference_t<Rng>>>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | pipeline) == is_noexcept);
    }

    // Validate deduction guide
    same_as<EV> auto r = elements_view<views::all_t<Rng>, 0>{forward<Rng>(rng)};
    using R            = decltype(r);
    STATIC_ASSERT(ranges::view<R>);
    STATIC_ASSERT(input_range<R>);
    STATIC_ASSERT(forward_range<R> == forward_range<Rng>);
    STATIC_ASSERT(bidirectional_range<R> == bidirectional_range<Rng>);
    STATIC_ASSERT(random_access_range<R> == random_access_range<Rng>);
    STATIC_ASSERT(!contiguous_range<R>);

    // Validate views::keys and views::values
    same_as<EV> auto kr = ranges::keys_view<Rng>{forward<Rng>(rng)};
    using KR            = decltype(kr);
    STATIC_ASSERT(ranges::view<KR>);
    STATIC_ASSERT(input_range<KR>);
    STATIC_ASSERT(forward_range<KR> == forward_range<Rng>);
    STATIC_ASSERT(bidirectional_range<KR> == bidirectional_range<Rng>);
    STATIC_ASSERT(random_access_range<KR> == random_access_range<Rng>);
    STATIC_ASSERT(!contiguous_range<KR>);

    using EV2            = elements_view<views::all_t<Rng>, 1>;
    same_as<EV2> auto vr = ranges::values_view<Rng>{forward<Rng>(rng)};
    using VR             = decltype(vr);
    STATIC_ASSERT(ranges::view<VR>);
    STATIC_ASSERT(input_range<VR>);
    STATIC_ASSERT(forward_range<VR> == forward_range<Rng>);
    STATIC_ASSERT(bidirectional_range<VR> == bidirectional_range<Rng>);
    STATIC_ASSERT(random_access_range<VR> == random_access_range<Rng>);
    STATIC_ASSERT(!contiguous_range<VR>);

    // Validate elements_view::size
    STATIC_ASSERT(CanMemberSize<R> == CanSize<V>);
    STATIC_ASSERT(CanSize<R> == CanMemberSize<R>);
    STATIC_ASSERT(CanMemberSize<const R> == CanSize<const V>);
    STATIC_ASSERT(CanSize<const R> == CanMemberSize<const R>);
    if constexpr (CanMemberSize<R>) {
        assert(r.size() == static_cast<decltype(r.size())>(ranges::size(expected_keys)));
        if constexpr (CanMemberSize<const R>) {
            assert(as_const(r).size() == static_cast<decltype(r.size())>(ranges::size(expected_keys)));
        }
    }

    const bool is_empty = ranges::empty(expected_keys);

    // Validate view_interface::empty and operator bool
    STATIC_ASSERT(CanMemberEmpty<R> == forward_range<Rng>);
    STATIC_ASSERT(CanBool<R> == CanEmpty<R>);
    if constexpr (CanMemberEmpty<R>) {
        assert(r.empty() == is_empty);
        assert(static_cast<bool>(r) == !is_empty);
    } else {
        STATIC_ASSERT(CanEmpty<R> == CanSize<R>);
        if constexpr (CanEmpty<R>) {
            assert(ranges::empty(r) == is_empty);
            assert(static_cast<bool>(r) == !is_empty);
        }
    }

    STATIC_ASSERT(CanMemberEmpty<const R> == (forward_range<const Rng>) );
    STATIC_ASSERT(CanBool<const R> == CanEmpty<const R>);
    if constexpr (CanMemberEmpty<const R>) {
        assert(as_const(r).empty() == is_empty);
        assert(static_cast<bool>(as_const(r)) == !is_empty);
    } else {
        STATIC_ASSERT(CanEmpty<const R> == CanSize<const R>);
        if constexpr (CanEmpty<const R>) {
            assert(ranges::empty(as_const(r)) == is_empty);
            assert(static_cast<bool>(as_const(r)) == !is_empty);
        }
    }

    // Validate content
    assert(ranges::equal(r, expected_keys));
    assert(ranges::equal(vr, expected_values));

    // Validate elements_view::begin
    STATIC_ASSERT(CanMemberBegin<R>);
    STATIC_ASSERT(CanBegin<const R&> == (range<const V>) );
    if (forward_range<V>) { // intentionally not if constexpr
        const same_as<iterator_t<R>> auto i = r.begin();
        if (!is_empty) {
            assert(*i == *begin(expected_keys));
        }

        if constexpr (copyable<V>) {
            auto r2                              = r;
            const same_as<iterator_t<R>> auto i2 = r2.begin();
            if (!is_empty) {
                assert(*i2 == *i);
            }
        }

        if constexpr (CanBegin<const R&>) {
            const same_as<iterator_t<const R>> auto i3 = as_const(r).begin();
            if (!is_empty) {
                assert(*i3 == *i);
            }
        }
    }

    // Validate elements_view::end
    STATIC_ASSERT(CanMemberEnd<R>);
    STATIC_ASSERT(CanEnd<const R&> == (range<const V>) );
    if (!is_empty) {
        same_as<sentinel_t<R>> auto i = r.end();
        // static_assert(is_same_v<sentinel_t<R>, iterator_t<R>> == common_range<V>);
        if constexpr (bidirectional_range<R> && common_range<R>) {
            assert(*prev(i) == *prev(end(expected_keys)));
        }

        if constexpr (CanEnd<const R&>) {
            same_as<sentinel_t<const R>> auto i2 = as_const(r).end();
            static_assert(is_same_v<sentinel_t<const R>, iterator_t<const R>> == common_range<const V>);
            if constexpr (bidirectional_range<const R> && common_range<const R>) {
                assert(*prev(i2) == *prev(end(expected_keys)));
            }
        }
    }

    // Validate view_interface::data
    STATIC_ASSERT(!CanData<EV>);
    STATIC_ASSERT(!CanData<const EV>);

    // Validate view_interface::front and back
    if (!is_empty) {
        STATIC_ASSERT(CanMemberFront<EV> == forward_range<V>);
        if constexpr (CanMemberFront<EV>) {
            assert(r.front() == *begin(expected_keys));
        }

        STATIC_ASSERT(CanMemberBack<EV> == (bidirectional_range<V> && common_range<V>) );
        if constexpr (CanMemberBack<EV>) {
            assert(r.back() == *prev(end(expected_keys)));
        }

        STATIC_ASSERT(CanMemberFront<const EV> == (forward_range<const V>) );
        if constexpr (CanMemberFront<const EV>) {
            assert(as_const(r).front() == *begin(expected_keys));
        }

        STATIC_ASSERT(CanMemberBack<const EV> == (bidirectional_range<const V> && common_range<const V>) );
        if constexpr (CanMemberBack<const EV>) {
            assert(as_const(r).back() == *prev(end(expected_keys)));
        }
    }

    // Validate view_interface::operator[]
    if (!is_empty) {
        STATIC_ASSERT(CanIndex<R> == random_access_range<V>);
        if constexpr (CanIndex<R>) {
            assert(r[0] == *r.begin());
        }

        STATIC_ASSERT(CanIndex<const R> == (random_access_range<const V>) );
        if constexpr (CanIndex<const R>) {
            assert(as_const(r)[0] == *as_const(r).begin());
        }
    }

    // Validate elements_view::base() const&
    STATIC_ASSERT(CanMemberBase<const EV&> == copy_constructible<V>);
    if constexpr (copy_constructible<V> && forward_range<V>) {
        same_as<V> auto b1 = as_const(r).base();
        STATIC_ASSERT(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>);
        if (!is_empty) {
            assert(*b1.begin() == pair(0, -1)); // NB: depends on the test data
            if constexpr (bidirectional_range<V> && common_range<V>) {
                assert(*prev(b1.end()) == pair(7, -8)); // NB: depends on the test data
            }
        }
    }

    // Validate elements_view::base() && (NB: do this last since it leaves r moved-from)
#if !defined(__clang__) && !defined(__EDG__) // TRANSITION, DevCom-1159442
    (void) 42;
#endif // TRANSITION, DevCom-1159442
    if (forward_range<V>) { // intentionally not if constexpr
        same_as<V> auto b2 = move(r).base();
        STATIC_ASSERT(noexcept(move(r).base()) == is_nothrow_move_constructible_v<V>);
        if (!is_empty) {
            assert(*b2.begin() == pair(0, -1)); // NB: depends on the test data
            if constexpr (bidirectional_range<V> && common_range<V>) {
                assert(*prev(b2.end()) == pair(7, -8)); // NB: depends on the test data
            }
        }
    }
    return true;
}

static constexpr P some_pairs[]        = {{0, -1}, {1, -2}, {2, -3}, {3, -4}, {4, -5}, {5, -6}, {6, -7}, {7, -8}};
static constexpr int expected_keys[]   = {0, 1, 2, 3, 4, 5, 6, 7};
static constexpr int expected_values[] = {-1, -2, -3, -4, -5, -6, -7, -8};

struct instantiator {
    template <ranges::input_range R>
    static constexpr void call() {
        R r{some_pairs};
        test_one(r, expected_keys, expected_values);
    }
};

template <class Category, test::Sized IsSized, test::Common IsCommon>
using test_range =
    test::range<Category, const P, IsSized, test::CanDifference{derived_from<Category, random_access_iterator_tag>},
        IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
        test::ProxyRef::no>;

constexpr void instantiation_test() {
#ifdef TEST_EVERYTHING
    test_in<instantiator, const P>();
#else // ^^^ test all input range permutations / test only "interesting" permutations vvv
    // The view is sensitive to category, commonality, size, and differencing, but cannot handle proxies.
    using test::Common, test::Sized;

    instantiator::call<test_range<input_iterator_tag, Sized::no, Common::no>>();
    instantiator::call<test_range<input_iterator_tag, Sized::no, Common::yes>>();
    instantiator::call<test_range<input_iterator_tag, Sized::yes, Common::no>>();
    instantiator::call<test_range<input_iterator_tag, Sized::yes, Common::yes>>();
    instantiator::call<test_range<forward_iterator_tag, Sized::no, Common::no>>();
    instantiator::call<test_range<forward_iterator_tag, Sized::no, Common::yes>>();
    instantiator::call<test_range<forward_iterator_tag, Sized::yes, Common::no>>();
    instantiator::call<test_range<forward_iterator_tag, Sized::yes, Common::yes>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Sized::no, Common::no>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Sized::no, Common::yes>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Sized::yes, Common::no>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Sized::yes, Common::yes>>();
    instantiator::call<test_range<random_access_iterator_tag, Sized::no, Common::no>>();
    instantiator::call<test_range<random_access_iterator_tag, Sized::no, Common::yes>>();
    instantiator::call<test_range<random_access_iterator_tag, Sized::yes, Common::no>>();
    instantiator::call<test_range<random_access_iterator_tag, Sized::yes, Common::yes>>();
    instantiator::call<test_range<contiguous_iterator_tag, Sized::no, Common::no>>();
    instantiator::call<test_range<contiguous_iterator_tag, Sized::no, Common::yes>>();
    instantiator::call<test_range<contiguous_iterator_tag, Sized::yes, Common::no>>();
    instantiator::call<test_range<contiguous_iterator_tag, Sized::yes, Common::yes>>();
#endif // TEST_EVERYTHING
}

int main() {
    { // Validate copyable views
        constexpr span<const P> s{some_pairs};
        STATIC_ASSERT(test_one(s, expected_keys, expected_values));
        test_one(s, expected_keys, expected_values);
    }

    { // Validate non-views
        STATIC_ASSERT(test_one(some_pairs, expected_keys, expected_values));
        test_one(some_pairs, expected_keys, expected_values);

        {
            vector vec(ranges::begin(some_pairs), ranges::end(some_pairs));
            test_one(vec, expected_keys, expected_values);
        }
        {
            forward_list lst(ranges::begin(some_pairs), ranges::end(some_pairs));
            test_one(lst, expected_keys, expected_values);
        }

        STATIC_ASSERT((instantiation_test(), true));
        instantiation_test();
    }

    { // Validate a non-view borrowed range
        constexpr span s{some_pairs};
        STATIC_ASSERT(test_one(s, expected_keys, expected_values));
        test_one(s, expected_keys, expected_values);
    }
}
