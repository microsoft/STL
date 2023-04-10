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

// Test a silly precomposed range adaptor pipeline
constexpr auto is_even = [](const auto& x) { return x % 2 == 0; };
using Pred             = remove_const_t<decltype(is_even)>;
STATIC_ASSERT(is_nothrow_copy_constructible_v<Pred>&& is_nothrow_move_constructible_v<Pred>);

constexpr auto pipeline =
    views::filter(is_even) | views::filter(is_even) | views::filter(is_even) | views::filter(is_even);

template <class Rng, class V = views::all_t<Rng>>
using pipeline_t =
    ranges::filter_view<ranges::filter_view<ranges::filter_view<ranges::filter_view<V, Pred>, Pred>, Pred>, Pred>;

template <class Rng>
concept CanViewFilter = requires(Rng&& r) { views::filter(forward<Rng>(r), is_even); };

template <ranges::input_range Rng, ranges::random_access_range Expected>
constexpr bool test_one(Rng&& rng, Expected&& expected) {
    using ranges::filter_view, ranges::common_range, ranges::bidirectional_range, ranges::enable_borrowed_range,
        ranges::forward_range, ranges::iterator_t, ranges::prev;

    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;

    using V = views::all_t<Rng>;
    using F = filter_view<V, Pred>;
    STATIC_ASSERT(ranges::view<F>);
    STATIC_ASSERT(ranges::input_range<F>);
    STATIC_ASSERT(forward_range<F> == forward_range<Rng>);
    STATIC_ASSERT(bidirectional_range<F> == bidirectional_range<Rng>);
    STATIC_ASSERT(!ranges::random_access_range<F>);
    STATIC_ASSERT(!ranges::contiguous_range<F>);

    // Validate range adaptor object and range adaptor closure
    constexpr auto filter_even = views::filter(is_even);

    // ... with lvalue argument
    STATIC_ASSERT(CanViewFilter<Rng&> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewFilter<Rng&>) {
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::filter(rng, is_even)), F>);
        STATIC_ASSERT(noexcept(views::filter(rng, is_even)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(rng | filter_even), F>);
        STATIC_ASSERT(noexcept(rng | filter_even) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(rng | pipeline), pipeline_t<Rng&>>);
        STATIC_ASSERT(noexcept(rng | pipeline) == is_noexcept);
    }

    // ... with const lvalue argument
    STATIC_ASSERT(CanViewFilter<const remove_reference_t<Rng>&> == (!is_view || copy_constructible<V>) );
    if constexpr (is_view && copy_constructible<V>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::filter(as_const(rng), is_even)), F>);
        STATIC_ASSERT(noexcept(views::filter(as_const(rng), is_even)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | filter_even), F>);
        STATIC_ASSERT(noexcept(as_const(rng) | filter_even) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | pipeline), pipeline_t<const remove_reference_t<Rng>&>>);
        STATIC_ASSERT(noexcept(as_const(rng) | pipeline) == is_noexcept);
    } else if constexpr (!is_view) {
        using RC                   = filter_view<ranges::ref_view<const remove_reference_t<Rng>>, Pred>;
        constexpr bool is_noexcept = true;

        STATIC_ASSERT(same_as<decltype(views::filter(as_const(rng), is_even)), RC>);
        STATIC_ASSERT(noexcept(views::filter(as_const(rng), is_even)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | filter_even), RC>);
        STATIC_ASSERT(noexcept(as_const(rng) | filter_even) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | pipeline), pipeline_t<const remove_reference_t<Rng>&>>);
        STATIC_ASSERT(noexcept(as_const(rng) | pipeline) == is_noexcept);
    }

    // ... with rvalue argument
    STATIC_ASSERT(CanViewFilter<remove_reference_t<Rng>> == (is_view || movable<remove_reference_t<Rng>>) );
    if constexpr (is_view) {
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;
        STATIC_ASSERT(same_as<decltype(views::filter(move(rng), is_even)), F>);
        STATIC_ASSERT(noexcept(views::filter(move(rng), is_even)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | filter_even), F>);
        STATIC_ASSERT(noexcept(move(rng) | filter_even) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | pipeline), pipeline_t<remove_reference_t<Rng>>>);
        STATIC_ASSERT(noexcept(move(rng) | pipeline) == is_noexcept);
    } else if constexpr (movable<remove_reference_t<Rng>>) {
        using S                    = ranges::owning_view<remove_reference_t<Rng>>;
        using RS                   = filter_view<S, Pred>;
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<remove_reference_t<Rng>>;

        STATIC_ASSERT(same_as<decltype(views::filter(move(rng), is_even)), RS>);
        STATIC_ASSERT(noexcept(views::filter(move(rng), is_even)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | filter_even), RS>);
        STATIC_ASSERT(noexcept(move(rng) | filter_even) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | pipeline), pipeline_t<remove_reference_t<Rng>>>);
        STATIC_ASSERT(noexcept(move(rng) | pipeline) == is_noexcept);
    }

    // ... with const rvalue argument
    STATIC_ASSERT(CanViewFilter<const remove_reference_t<Rng>> == (is_view && copy_constructible<V>) );
    if constexpr (is_view && copy_constructible<V>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::filter(move(as_const(rng)), is_even)), F>);
        STATIC_ASSERT(noexcept(views::filter(move(as_const(rng)), is_even)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | filter_even), F>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | filter_even) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | pipeline), pipeline_t<const remove_reference_t<Rng>>>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | pipeline) == is_noexcept);
    }

    // Validate deduction guide
    same_as<F> auto r = filter_view{forward<Rng>(rng), is_even};
    assert(ranges::equal(r, expected));
    if constexpr (forward_range<V>) {
        // filter_view memoizes the first iterator, let's repeat a few times for coverage.
        assert(ranges::equal(r, expected));
        assert(ranges::equal(r, expected));
        assert(ranges::equal(r, expected));
    }

    { // Validate filter_view::pred
        [[maybe_unused]] same_as<Pred> auto pred_copy = as_const(r).pred();
        STATIC_ASSERT(noexcept(as_const(r).pred()));
    }

    // Validate view_interface::empty and operator bool
    const bool is_empty = ranges::empty(expected);
    STATIC_ASSERT(CanMemberEmpty<F> == forward_range<V>);
    STATIC_ASSERT(CanBool<F> == CanMemberEmpty<F>);
    if constexpr (CanMemberEmpty<F>) {
        assert(r.empty() == is_empty);
        assert(static_cast<bool>(r) == !is_empty);
    }
    STATIC_ASSERT(!CanMemberEmpty<const F>);
    STATIC_ASSERT(!CanBool<const F>);

    // Validate filter_view::begin
    STATIC_ASSERT(CanMemberBegin<F>);
    if (forward_range<V>) { // intentionally not if constexpr
        // Ditto "let's make some extra calls because memoization"
        const same_as<iterator_t<F>> auto i = r.begin();
        if (!is_empty) {
            assert(*i == *begin(expected));
        }
        assert(*r.begin() == *begin(expected));
        assert(*r.begin() == *begin(expected));

        if constexpr (copy_constructible<V>) {
            auto r2                              = r;
            const same_as<iterator_t<F>> auto i2 = r2.begin();
            assert(*r2.begin() == *i2);
            assert(*r2.begin() == *i2);
            if (!is_empty) {
                assert(*i2 == *i);
            }
        }

        STATIC_ASSERT(!CanBegin<const F>);
    }

    // Validate filter_view::end
    STATIC_ASSERT(CanMemberEnd<F>);
    if (!is_empty) {
        if constexpr (common_range<V>) {
            same_as<iterator_t<F>> auto i = r.end();
            if constexpr (bidirectional_range<V>) {
                assert(*prev(i) == *prev(end(expected)));
            }
        } else {
            [[maybe_unused]] same_as<ranges::sentinel_t<F>> auto s = r.end();
        }

        if constexpr (bidirectional_range<V> && common_range<V> && copy_constructible<V>) {
            auto r2 = r;
            assert(*prev(r2.end()) == *prev(end(expected)));
        }

        STATIC_ASSERT(!CanEnd<const F>);
    }

#if _HAS_CXX23
    using ranges::const_iterator_t, ranges::const_sentinel_t;

    // Validate view_interface::cbegin
    STATIC_ASSERT(CanMemberCBegin<F>);
    if (forward_range<V>) { // intentionally not if constexpr
        // Ditto "let's make some extra calls because memoization"
        const same_as<const_iterator_t<F>> auto ci = r.cbegin();
        if (!is_empty) {
            assert(*ci == *begin(expected));
        }
        assert(*r.cbegin() == *begin(expected));
        assert(*r.cbegin() == *begin(expected));

        if constexpr (copy_constructible<V>) {
            auto r2                                     = r;
            const same_as<const_iterator_t<F>> auto ci2 = r2.cbegin();
            assert(*r2.cbegin() == *ci2);
            assert(*r2.cbegin() == *ci2);
            if (!is_empty) {
                assert(*ci2 == *ci);
            }
        }

        STATIC_ASSERT(!CanMemberCBegin<const F>);
    }

    // Validate view_interface::cend
    STATIC_ASSERT(CanMemberCEnd<F>);
    if (!is_empty) {
        if constexpr (common_range<V>) {
            same_as<const_iterator_t<F>> auto ci = r.cend();
            if constexpr (bidirectional_range<V>) {
                assert(*prev(ci) == *prev(end(expected)));
            }
        } else {
            [[maybe_unused]] same_as<const_sentinel_t<F>> auto cs = r.cend();
        }

        if constexpr (bidirectional_range<V> && common_range<V> && copy_constructible<V>) {
            auto r2 = r;
            assert(*prev(r2.cend()) == *prev(end(expected)));
        }

        STATIC_ASSERT(!CanMemberCEnd<const F>);
    }
#endif // _HAS_CXX23

    // Validate view_interface::data
    STATIC_ASSERT(!CanData<F>);
    STATIC_ASSERT(!CanData<const F>);

    // Validate view_interface::size
    STATIC_ASSERT(!CanSize<F>);
    STATIC_ASSERT(!CanSize<const F>);

    // Validate view_interface::operator[]
    STATIC_ASSERT(!CanIndex<F>);
    STATIC_ASSERT(!CanIndex<const F>);

    if (!is_empty) {
        // Validate view_interface::front and back
        STATIC_ASSERT(CanMemberFront<F> == forward_range<V>);
        if constexpr (forward_range<V>) {
            assert(r.front() == *begin(expected));
        }

        STATIC_ASSERT(CanMemberBack<F> == (bidirectional_range<V> && common_range<V>) );
        if constexpr (CanMemberBack<F>) {
            assert(r.back() == *prev(end(expected)));
        }

        STATIC_ASSERT(!CanMemberFront<const F>);
        STATIC_ASSERT(!CanMemberBack<const F>);
    }

    // Validate filter_view::base() const&
    STATIC_ASSERT(CanMemberBase<const F&> == copy_constructible<V>);
    if constexpr (copy_constructible<V> && forward_range<V>) {
        same_as<V> auto b1 = as_const(r).base();
        STATIC_ASSERT(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>);
        if (!is_empty) {
            assert(*b1.begin() == *begin(expected));
            if constexpr (bidirectional_range<V> && common_range<V>) {
                assert(*prev(b1.end(), 2) == *prev(end(expected))); // NB: depends on the test data
            }
        }
    }

    // Validate filter_view::base() && (NB: do this last since it leaves r moved-from)
    if (forward_range<V>) { // intentionally not if constexpr
        same_as<V> auto b2 = move(r).base();
        STATIC_ASSERT(noexcept(move(r).base()) == is_nothrow_move_constructible_v<V>);
        if (!is_empty) {
            assert(*b2.begin() == *begin(expected));
            if constexpr (bidirectional_range<V> && common_range<V>) {
                assert(*prev(b2.end(), 2) == *prev(end(expected))); // NB: depends on the test data
            }
        }
    }

    return true;
}

static constexpr int some_ints[]      = {0, 1, 2, 3, 4, 5, 6, 7};
static constexpr int only_even_ints[] = {0, 2, 4, 6};

struct instantiator {
    template <ranges::input_range R>
    static constexpr void call() {
        R r{some_ints};
        test_one(r, only_even_ints);
    }
};

template <class Category, test::Common IsCommon>
using test_range = test::range<Category, const int, test::Sized::no,
    test::CanDifference{derived_from<Category, random_access_iterator_tag>}, IsCommon,
    test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}>;

constexpr void instantiation_test() {
#ifdef TEST_EVERYTHING
    test_in<instantiator, const int>();
#else // ^^^ test all input range permutations / test only "interesting" permutations vvv
    // The view is sensitive to category and commonality, but oblivious to size, differencing, and proxyness.
    using test::Common;

    instantiator::call<test_range<input_iterator_tag, Common::no>>();
    instantiator::call<test_range<input_iterator_tag, Common::yes>>();
    instantiator::call<test_range<forward_iterator_tag, Common::no>>();
    instantiator::call<test_range<forward_iterator_tag, Common::yes>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Common::no>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Common::yes>>();
    instantiator::call<test_range<random_access_iterator_tag, Common::no>>();
    instantiator::call<test_range<random_access_iterator_tag, Common::yes>>();
    instantiator::call<test_range<contiguous_iterator_tag, Common::no>>();
    instantiator::call<test_range<contiguous_iterator_tag, Common::yes>>();
#endif // TEST_EVERYTHING
}

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

int main() {
    // Validate views
    { // ... copyable
        constexpr span<const int> s{some_ints};
        STATIC_ASSERT(test_one(s, only_even_ints));
        test_one(s, only_even_ints);
    }
    { // ... move-only
        test_one(move_only_view<input_iterator_tag, test::Common::no>{some_ints}, only_even_ints);
        test_one(move_only_view<forward_iterator_tag, test::Common::no>{some_ints}, only_even_ints);
        test_one(move_only_view<forward_iterator_tag, test::Common::yes>{some_ints}, only_even_ints);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints}, only_even_ints);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints}, only_even_ints);
        test_one(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints}, only_even_ints);
        test_one(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints}, only_even_ints);
    }

    // Validate non-views
    {
        STATIC_ASSERT(test_one(some_ints, only_even_ints));
        test_one(some_ints, only_even_ints);
    }
    {
        vector vec(ranges::begin(some_ints), ranges::end(some_ints));
        test_one(vec, only_even_ints);
    }
    {
        forward_list lst(ranges::begin(some_ints), ranges::end(some_ints));
        test_one(lst, only_even_ints);
    }

    // filter/reverse interaction test
    {
        auto fr_pipe = views::filter(is_even) | views::reverse;
        auto rf_pipe = views::reverse | views::filter(is_even);

        auto r0  = some_ints | fr_pipe;
        using R0 = decltype(r0);
        STATIC_ASSERT(ranges::bidirectional_range<R0> && ranges::view<R0>);
        assert(ranges::equal(r0, views::reverse(only_even_ints)));

        auto r1  = some_ints | rf_pipe;
        using R1 = decltype(r1);
        STATIC_ASSERT(ranges::bidirectional_range<R1> && ranges::view<R1>);
        assert(ranges::equal(r1, views::reverse(only_even_ints)));

        assert(ranges::equal(r0, r1));
    }

    STATIC_ASSERT((instantiation_test(), true));
    instantiation_test();

    { // Validate **non-standard guarantee** that predicates are moved into the range adaptor closure, and into the view
      // object from an rvalue closure
        struct Fn {
            Fn()     = default;
            Fn(Fn&&) = default;
            Fn(const Fn&) {
                assert(false);
            }
            Fn& operator=(Fn&&) = default;

            Fn& operator=(const Fn&) {
                assert(false);
                return *this;
            }

            bool operator()(int) const {
                return true;
            }
        };

        (void) views::filter(Fn{})(span<int>{});
    }
}
