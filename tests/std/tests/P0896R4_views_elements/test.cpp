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

// Validate views::keys and views::values
static_assert(same_as<remove_const_t<decltype(views::keys)>, remove_const_t<decltype(views::elements<0>)>>);
static_assert(same_as<remove_const_t<decltype(views::values)>, remove_const_t<decltype(views::elements<1>)>>);

constexpr auto pipeline = views::elements<0> | views::all;

template <class Rng, class V = views::all_t<Rng>>
using pipeline_t = ranges::elements_view<V, 0>;

template <class Rng>
concept CanViewElements = requires(Rng&& r) { views::elements<0>(forward<Rng>(r)); };

constexpr P some_pairs[]        = {{0, -1}, {1, -2}, {2, -3}, {3, -4}, {4, -5}, {5, -6}, {6, -7}, {7, -8}};
constexpr int expected_keys[]   = {0, 1, 2, 3, 4, 5, 6, 7};
constexpr int expected_values[] = {-1, -2, -3, -4, -5, -6, -7, -8};

template <ranges::input_range Rng>
constexpr bool test_one(Rng&& rng) {
    using ranges::elements_view, ranges::bidirectional_range, ranges::common_range, ranges::contiguous_range,
        ranges::enable_borrowed_range, ranges::forward_range, ranges::input_range, ranges::iterator_t, ranges::prev,
        ranges::random_access_range, ranges::range, ranges::range_reference_t, ranges::sentinel_t, ranges::sized_range,
        ranges::borrowed_range;

    using V = views::all_t<Rng>;
    using R = elements_view<V, 0>;
    static_assert(ranges::view<R>);
    static_assert(input_range<R>);
    static_assert(forward_range<R> == forward_range<Rng>);
    static_assert(bidirectional_range<R> == bidirectional_range<Rng>);
    static_assert(random_access_range<R> == random_access_range<Rng>);
    static_assert(!contiguous_range<R>);
    static_assert(borrowed_range<R> == borrowed_range<V>);

    // ... with lvalue argument
    static_assert(CanViewElements<Rng&>);
    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;
    { // Validate lvalue
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<decltype(views::elements<0>(rng)), R>);
        static_assert(noexcept(views::elements<0>(rng)) == is_noexcept);

        static_assert(same_as<decltype(rng | views::elements<0>), R>);
        static_assert(noexcept(rng | views::elements<0>) == is_noexcept);

        static_assert(same_as<decltype(rng | pipeline), pipeline_t<Rng&>>);
        static_assert(noexcept(rng | pipeline) == is_noexcept);
    }

    // ... with const lvalue argument
    static_assert(CanViewElements<const remove_reference_t<Rng>&> == (!is_view || copy_constructible<V>) );
    if constexpr (is_view && copy_constructible<V>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<decltype(views::elements<0>(as_const(rng))), R>);
        static_assert(noexcept(views::elements<0>(as_const(rng))) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | views::elements<0>), R>);
        static_assert(noexcept(as_const(rng) | views::elements<0>) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | pipeline), pipeline_t<const remove_reference_t<Rng>&>>);
        static_assert(noexcept(as_const(rng) | pipeline) == is_noexcept);
    } else if constexpr (!is_view) {
        using RC                   = elements_view<ranges::ref_view<const remove_reference_t<Rng>>, 0>;
        constexpr bool is_noexcept = true;

        static_assert(same_as<decltype(views::elements<0>(as_const(rng))), RC>);
        static_assert(noexcept(views::elements<0>(as_const(rng))) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | views::elements<0>), RC>);
        static_assert(noexcept(as_const(rng) | views::elements<0>) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | pipeline), pipeline_t<const remove_reference_t<Rng>&>>);
        static_assert(noexcept(as_const(rng) | pipeline) == is_noexcept);
    }

    // ... with rvalue argument
    static_assert(CanViewElements<remove_reference_t<Rng>> == (is_view || movable<remove_reference_t<Rng>>) );
    if constexpr (is_view) {
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;
        static_assert(same_as<decltype(views::elements<0>(move(rng))), R>);
        static_assert(noexcept(views::elements<0>(move(rng))) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | views::elements<0>), R>);
        static_assert(noexcept(move(rng) | views::elements<0>) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | pipeline), pipeline_t<remove_reference_t<Rng>>>);
        static_assert(noexcept(move(rng) | pipeline) == is_noexcept);
    } else if constexpr (movable<remove_reference_t<Rng>>) {
        using RS                   = elements_view<ranges::owning_view<remove_reference_t<Rng>>, 0>;
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;

        static_assert(same_as<decltype(views::elements<0>(move(rng))), RS>);
        static_assert(noexcept(views::elements<0>(move(rng))) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | views::elements<0>), RS>);
        static_assert(noexcept(move(rng) | views::elements<0>) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | pipeline), pipeline_t<remove_reference_t<Rng>>>);
        static_assert(noexcept(move(rng) | pipeline) == is_noexcept);
    }

    // ... with const rvalue argument
    static_assert(CanViewElements<const remove_reference_t<Rng>> == (is_view && copy_constructible<V>) );
    if constexpr (is_view && copy_constructible<V>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<decltype(views::elements<0>(move(as_const(rng)))), R>);
        static_assert(noexcept(views::elements<0>(move(as_const(rng)))) == is_noexcept);

        static_assert(same_as<decltype(move(as_const(rng)) | views::elements<0>), R>);
        static_assert(noexcept(move(as_const(rng)) | views::elements<0>) == is_noexcept);

        static_assert(same_as<decltype(move(as_const(rng)) | pipeline), pipeline_t<const remove_reference_t<Rng>>>);
        static_assert(noexcept(move(as_const(rng)) | pipeline) == is_noexcept);
    }

    // Validate concepts are properly modeled
    R r{forward<Rng>(rng)};
    static_assert(ranges::view<R>);
    static_assert(input_range<R>);
    static_assert(forward_range<R> == forward_range<Rng>);
    static_assert(bidirectional_range<R> == bidirectional_range<Rng>);
    static_assert(random_access_range<R> == random_access_range<Rng>);
    static_assert(!contiguous_range<R>);

    // Validate elements_view::size
    static_assert(CanMemberSize<R> == CanSize<V>);
    static_assert(CanSize<R> == CanMemberSize<R>);
    static_assert(CanMemberSize<const R> == CanSize<const V>);
    static_assert(CanSize<const R> == CanMemberSize<const R>);
    if constexpr (CanMemberSize<R>) {
        assert(r.size() == static_cast<decltype(r.size())>(ranges::size(expected_keys)));
        if constexpr (CanMemberSize<const R>) {
            assert(as_const(r).size() == static_cast<decltype(r.size())>(ranges::size(expected_keys)));
        }
    }

    const bool is_empty = ranges::empty(expected_keys);

    // Validate view_interface::empty and operator bool
    static_assert(CanMemberEmpty<R> == (sized_range<Rng> || forward_range<Rng>) );
    static_assert(CanBool<R> == CanEmpty<R>);
    if constexpr (CanMemberEmpty<R>) {
        assert(r.empty() == is_empty);
        assert(static_cast<bool>(r) == !is_empty);
    } else {
        static_assert(CanEmpty<R> == CanSize<R>);
        if constexpr (CanEmpty<R>) {
            assert(ranges::empty(r) == is_empty);
            assert(static_cast<bool>(r) == !is_empty);
        }
    }

    static_assert(CanMemberEmpty<const R> == (sized_range<const Rng> || forward_range<const Rng>) );
    static_assert(CanBool<const R> == CanEmpty<const R>);
    if constexpr (CanMemberEmpty<const R>) {
        assert(as_const(r).empty() == is_empty);
        assert(static_cast<bool>(as_const(r)) == !is_empty);
    } else {
        static_assert(CanEmpty<const R> == CanSize<const R>);
        if constexpr (CanEmpty<const R>) {
            assert(ranges::empty(as_const(r)) == is_empty);
            assert(static_cast<bool>(as_const(r)) == !is_empty);
        }
    }

    // Validate content
    assert(ranges::equal(r, expected_keys));

    // Validate views::keys and views::values
    static_assert(same_as<decltype(views::keys(rng)), R>);
    static_assert(same_as<decltype(views::values(rng)), elements_view<V, 1>>);
    if constexpr (forward_range<Rng> && is_lvalue_reference_v<Rng>) {
        assert(ranges::equal(views::values(rng), expected_values));
    }

    // Validate elements_view::begin
    static_assert(CanMemberBegin<R>);
    static_assert(CanBegin<const R&> == (range<const V>) );
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
    static_assert(CanMemberEnd<R>);
    static_assert(CanEnd<const R&> == range<const V>);
    if (!is_empty) {
        same_as<sentinel_t<R>> auto i = r.end();
        static_assert(common_range<R> == common_range<V>);
        if constexpr (bidirectional_range<R> && common_range<R>) {
            assert(*prev(i) == *prev(end(expected_keys)));
        }

        if constexpr (CanEnd<const R&>) {
            same_as<sentinel_t<const R>> auto i2 = as_const(r).end();
            static_assert(common_range<const R> == common_range<const V>);
            if constexpr (bidirectional_range<const R> && common_range<const R>) {
                assert(*prev(i2) == *prev(end(expected_keys)));
            }
        }
    }

#if _HAS_CXX23
    using ranges::const_iterator_t, ranges::const_sentinel_t, ranges::cbegin, ranges::cend;

    // Validate view_interface::cbegin
    static_assert(CanMemberCBegin<R>);
    static_assert(CanMemberCBegin<const R&> == range<const V>);
    if (forward_range<V>) { // intentionally not if constexpr
        const same_as<const_iterator_t<R>> auto i = r.cbegin();
        if (!is_empty) {
            assert(*i == *cbegin(expected_keys));
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

    // Validate view_interface::cend
    static_assert(CanMemberCEnd<R>);
    static_assert(CanMemberCEnd<const R&> == range<const V>);
    if (!is_empty) {
        same_as<const_sentinel_t<R>> auto i = r.cend();
        static_assert(common_range<R> == common_range<V>);
        if constexpr (bidirectional_range<R> && common_range<R>) {
            assert(*prev(i) == *prev(cend(expected_keys)));
        }

        if constexpr (CanCEnd<const R&>) {
            same_as<const_sentinel_t<const R>> auto i2 = as_const(r).cend();
            static_assert(common_range<const R> == common_range<const V>);
            if constexpr (bidirectional_range<const R> && common_range<const R>) {
                assert(*prev(i2) == *prev(cend(expected_keys)));
            }
        }
    }
#endif // _HAS_CXX23

    // Validate view_interface::data
    static_assert(!CanData<R>);
    static_assert(!CanData<const R>);

    // Validate view_interface::front and back
    if (!is_empty) {
        static_assert(CanMemberFront<R> == forward_range<V>);
        if constexpr (CanMemberFront<R>) {
            assert(r.front() == *begin(expected_keys));
        }

        static_assert(CanMemberBack<R> == (bidirectional_range<V> && common_range<V>) );
        if constexpr (CanMemberBack<R>) {
            assert(r.back() == *prev(end(expected_keys)));
        }

        static_assert(CanMemberFront<const R> == (forward_range<const V>) );
        if constexpr (CanMemberFront<const R>) {
            assert(as_const(r).front() == *begin(expected_keys));
        }

        static_assert(CanMemberBack<const R> == (bidirectional_range<const V> && common_range<const V>) );
        if constexpr (CanMemberBack<const R>) {
            assert(as_const(r).back() == *prev(end(expected_keys)));
        }
    }

    // Validate view_interface::operator[]
    if (!is_empty) {
        static_assert(CanIndex<R> == random_access_range<V>);
        if constexpr (CanIndex<R>) {
            assert(r[0] == *r.begin());
        }

        static_assert(CanIndex<const R> == random_access_range<const V>);
        if constexpr (CanIndex<const R>) {
            assert(as_const(r)[0] == *as_const(r).begin());
        }
    }

    // Validate elements_view::base() const&
    static_assert(CanMemberBase<const R&> == copy_constructible<V>);
    if constexpr (copy_constructible<V> && forward_range<V>) {
        same_as<V> auto b1 = as_const(r).base();
        static_assert(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>);
        if (!is_empty) {
            assert((*b1.begin() == pair{0, -1})); // NB: depends on the test data
            if constexpr (bidirectional_range<V> && common_range<V>) {
                assert((*prev(b1.end()) == pair{7, -8})); // NB: depends on the test data
            }
        }
    }

    // Validate elements_view::base() && (NB: do this last since it leaves r moved-from)
    if (forward_range<V>) { // intentionally not if constexpr
        same_as<V> auto b2 = move(r).base();
        static_assert(noexcept(move(r).base()) == is_nothrow_move_constructible_v<V>);
        if (!is_empty) {
            assert((*b2.begin() == pair{0, -1})); // NB: depends on the test data
            if constexpr (bidirectional_range<V> && common_range<V>) {
                assert((*prev(b2.end()) == pair{7, -8})); // NB: depends on the test data
            }
        }
    }
    return true;
}

struct instantiator {
    template <ranges::input_range R>
    static constexpr void call() {
        R r{some_pairs};
        test_one(r);
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

// GH-3014 "<ranges>: list-initialization is misused"
void test_gh_3014() { // COMPILE-ONLY
    struct FwdRange {
        P* begin() {
            return nullptr;
        }

        test::init_list_not_constructible_iterator<P> begin() const {
            return nullptr;
        }

        unreachable_sentinel_t end() const {
            return {};
        }
    };

    auto r                                           = FwdRange{} | views::elements<0>;
    [[maybe_unused]] decltype(as_const(r).begin()) i = r.begin(); // Check 'iterator(iterator<!Const> i)'
}

int main() {
    { // Validate copyable views
        constexpr span<const P> s{some_pairs};
        static_assert(test_one(s));
        test_one(s);
    }

    { // Validate non-views
        static_assert(test_one(some_pairs));
        test_one(some_pairs);

        {
            vector vec(ranges::begin(some_pairs), ranges::end(some_pairs));
            test_one(vec);
        }
        {
            forward_list lst(ranges::begin(some_pairs), ranges::end(some_pairs));
            test_one(lst);
        }

        static_assert((instantiation_test(), true));
        instantiation_test();
    }

    { // Validate a view borrowed range
        constexpr auto v = views::iota(0ull, ranges::size(expected_keys))
                         | views::transform([](auto i) { return make_pair(expected_keys[i], expected_values[i]); });
        static_assert(test_one(v));
        test_one(v);
    }
}
