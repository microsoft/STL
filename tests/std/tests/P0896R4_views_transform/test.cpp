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

#if _ITERATOR_DEBUG_LEVEL == 0
#define NOEXCEPT_IDL0(...) noexcept(__VA_ARGS__)
#else
#define NOEXCEPT_IDL0(...) true
#endif // _ITERATOR_DEBUG_LEVEL == 0

// Test a silly precomposed range adaptor pipeline
constexpr auto add8 = [](const auto& x) noexcept { return x + 8; };

using Fun = remove_const_t<decltype(add8)>;
STATIC_ASSERT(is_nothrow_copy_constructible_v<Fun>&& is_nothrow_move_constructible_v<Fun>);

constexpr auto pipeline =
    views::transform(add8) | views::transform(add8) | views::transform(add8) | views::transform(add8);

template <class Rng, class V = views::all_t<Rng>>
using pipeline_t =
    ranges::transform_view<ranges::transform_view<ranges::transform_view<ranges::transform_view<V, Fun>, Fun>, Fun>,
        Fun>;

template <class Rng>
concept CanViewTransform = requires(Rng&& r) {
    views::transform(static_cast<Rng&&>(r), add8);
};

template <ranges::input_range Rng, ranges::random_access_range Expected>
constexpr bool test_one(Rng&& rng, Expected&& expected) {
    using ranges::transform_view, ranges::bidirectional_range, ranges::common_range, ranges::contiguous_range,
        ranges::enable_borrowed_range, ranges::forward_range, ranges::input_range, ranges::iterator_t, ranges::prev,
        ranges::random_access_range, ranges::range, ranges::range_reference_t, ranges::sentinel_t;

    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;

    using V  = views::all_t<Rng>;
    using TV = transform_view<V, Fun>;
    STATIC_ASSERT(ranges::view<TV>);
    STATIC_ASSERT(input_range<TV>);
    STATIC_ASSERT(forward_range<TV> == forward_range<Rng>);
    STATIC_ASSERT(bidirectional_range<TV> == bidirectional_range<Rng>);
    STATIC_ASSERT(random_access_range<TV> == random_access_range<Rng>);
    STATIC_ASSERT(!contiguous_range<TV>);

    // Validate range adaptor object and range adaptor closure
    constexpr auto transform_incr = views::transform(add8);

    // ... with lvalue argument
    STATIC_ASSERT(CanViewTransform<Rng&> == (!is_view || copyable<V>) );
    if constexpr (CanViewTransform<Rng&>) { // Validate lvalue
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::transform(rng, add8)), TV>);
        STATIC_ASSERT(noexcept(views::transform(rng, add8)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(rng | transform_incr), TV>);
        STATIC_ASSERT(noexcept(rng | transform_incr) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(rng | pipeline), pipeline_t<Rng&>>);
        STATIC_ASSERT(noexcept(rng | pipeline) == is_noexcept);
    }

    // ... with const lvalue argument
    STATIC_ASSERT(CanViewTransform<const remove_reference_t<Rng>&> == (!is_view || copyable<V>) );
    if constexpr (is_view && copyable<V>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::transform(as_const(rng), add8)), TV>);
        STATIC_ASSERT(noexcept(views::transform(as_const(rng), add8)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | transform_incr), TV>);
        STATIC_ASSERT(noexcept(as_const(rng) | transform_incr) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | pipeline), pipeline_t<const remove_reference_t<Rng>&>>);
        STATIC_ASSERT(noexcept(as_const(rng) | pipeline) == is_noexcept);
    } else if constexpr (!is_view) {
        using RC                   = transform_view<views::all_t<const remove_reference_t<Rng>&>, Fun>;
        constexpr bool is_noexcept = is_nothrow_constructible_v<RC, const remove_reference_t<Rng>&, decltype((add8))>;

        STATIC_ASSERT(same_as<decltype(views::transform(as_const(rng), add8)), RC>);
        STATIC_ASSERT(noexcept(views::transform(as_const(rng), add8)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | transform_incr), RC>);
        STATIC_ASSERT(noexcept(as_const(rng) | transform_incr) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | pipeline), pipeline_t<const remove_reference_t<Rng>&>>);
        STATIC_ASSERT(noexcept(as_const(rng) | pipeline) == is_noexcept);
    }

    // ... with rvalue argument
    STATIC_ASSERT(CanViewTransform<remove_reference_t<Rng>> == is_view || enable_borrowed_range<remove_cvref_t<Rng>>);
    if constexpr (is_view) {
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;
        STATIC_ASSERT(same_as<decltype(views::transform(move(rng), add8)), TV>);
        STATIC_ASSERT(noexcept(views::transform(move(rng), add8)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | transform_incr), TV>);
        STATIC_ASSERT(noexcept(move(rng) | transform_incr) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | pipeline), pipeline_t<remove_reference_t<Rng>>>);
        STATIC_ASSERT(noexcept(move(rng) | pipeline) == is_noexcept);
    } else if constexpr (enable_borrowed_range<remove_cvref_t<Rng>>) {
        using S                    = decltype(ranges::subrange{declval<remove_reference_t<Rng>>()});
        using RS                   = transform_view<S, Fun>;
        constexpr bool is_noexcept = noexcept(S{declval<remove_reference_t<Rng>>()});

        STATIC_ASSERT(same_as<decltype(views::transform(move(rng), add8)), RS>);
        STATIC_ASSERT(noexcept(views::transform(move(rng), add8)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | transform_incr), RS>);
        STATIC_ASSERT(noexcept(move(rng) | transform_incr) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | pipeline), pipeline_t<remove_reference_t<Rng>>>);
        STATIC_ASSERT(noexcept(move(rng) | pipeline) == is_noexcept);
    }

    // ... with const rvalue argument
    STATIC_ASSERT(CanViewTransform<const remove_reference_t<Rng>> == (is_view && copyable<V>)
                  || (!is_view && enable_borrowed_range<remove_cvref_t<Rng>>) );
    if constexpr (is_view && copyable<V>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::transform(move(as_const(rng)), add8)), TV>);
        STATIC_ASSERT(noexcept(views::transform(as_const(rng), add8)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | transform_incr), TV>);
        STATIC_ASSERT(noexcept(as_const(rng) | transform_incr) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | pipeline), pipeline_t<const remove_reference_t<Rng>>>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | pipeline) == is_noexcept);
    } else if constexpr (!is_view && enable_borrowed_range<remove_cvref_t<Rng>>) {
        using S                    = decltype(ranges::subrange{declval<const remove_reference_t<Rng>>()});
        using RS                   = transform_view<S, Fun>;
        constexpr bool is_noexcept = noexcept(S{declval<const remove_reference_t<Rng>>()});

        STATIC_ASSERT(same_as<decltype(views::transform(move(as_const(rng)), add8)), RS>);
        STATIC_ASSERT(noexcept(views::transform(move(as_const(rng)), add8)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | transform_incr), RS>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | transform_incr) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | pipeline), pipeline_t<const remove_reference_t<Rng>>>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | pipeline) == is_noexcept);
    }

    const bool is_empty = ranges::empty(expected);

    // Validate deduction guide
    same_as<TV> auto r = transform_view{forward<Rng>(rng), add8};
    using R            = decltype(r);
    STATIC_ASSERT(ranges::view<R>);
    STATIC_ASSERT(input_range<R>);
    STATIC_ASSERT(forward_range<R> == forward_range<Rng>);
    STATIC_ASSERT(bidirectional_range<R> == bidirectional_range<Rng>);
    STATIC_ASSERT(random_access_range<R> == random_access_range<Rng>);
    STATIC_ASSERT(!contiguous_range<R>);

    // Validate transform_view::size
    STATIC_ASSERT(CanMemberSize<R> == CanSize<V>);
    STATIC_ASSERT(CanSize<R> == CanMemberSize<R>);
    STATIC_ASSERT(CanMemberSize<const R> == CanSize<const V>);
    STATIC_ASSERT(CanSize<const R> == CanMemberSize<const R>);
    if constexpr (CanMemberSize<R>) {
        assert(r.size() == static_cast<decltype(r.size())>(ranges::size(expected)));
        if constexpr (CanMemberSize<const R>) {
            assert(as_const(r).size() == static_cast<decltype(r.size())>(ranges::size(expected)));
        }
    }

    constexpr bool const_invocable = regular_invocable<const Fun&, range_reference_t<const V>>;

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

    STATIC_ASSERT(CanMemberEmpty<const R> == (forward_range<const Rng> && const_invocable));
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
    assert(ranges::equal(r, expected));

    // Validate transform_view::begin
    STATIC_ASSERT(CanMemberBegin<R>);
    STATIC_ASSERT(CanBegin<const R&> == (range<const V> && const_invocable));
    if (forward_range<V>) { // intentionally not if constexpr
        const same_as<iterator_t<R>> auto i = r.begin();
        if (!is_empty) {
            assert(*i == *begin(expected));
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

    // Validate transform_view::end
    STATIC_ASSERT(CanMemberEnd<R>);
    STATIC_ASSERT(CanEnd<const R&> == (range<const V> && const_invocable));
    if (!is_empty) {
        same_as<sentinel_t<R>> auto i = r.end();
        static_assert(is_same_v<sentinel_t<R>, iterator_t<R>> == common_range<V>);
        if constexpr (bidirectional_range<R> && common_range<R>) {
            assert(*prev(i) == *prev(end(expected)));
        }

        if constexpr (CanEnd<const R&>) {
            same_as<sentinel_t<const R>> auto i2 = as_const(r).end();
            static_assert(is_same_v<sentinel_t<const R>, iterator_t<const R>> == common_range<const V>);
            if constexpr (bidirectional_range<const R> && common_range<const R>) {
                assert(*prev(i2) == *prev(end(expected)));
            }
        }
    }

    // Validate view_interface::data
    STATIC_ASSERT(!CanData<TV>);
    STATIC_ASSERT(!CanData<const TV>);

    // Validate view_interface::front and back
    if (!is_empty) {
        STATIC_ASSERT(CanMemberFront<TV> == forward_range<V>);
        if constexpr (CanMemberFront<TV>) {
            assert(r.front() == *begin(expected));
        }

        STATIC_ASSERT(CanMemberBack<TV> == (bidirectional_range<V> && common_range<V>) );
        if constexpr (CanMemberBack<TV>) {
            assert(r.back() == *prev(end(expected)));
        }

        STATIC_ASSERT(CanMemberFront<const TV> == (forward_range<const V> && const_invocable));
        if constexpr (CanMemberFront<const TV>) {
            assert(as_const(r).front() == *begin(expected));
        }

        STATIC_ASSERT(
            CanMemberBack<const TV> == (bidirectional_range<const V> && common_range<const V> && const_invocable));
        if constexpr (CanMemberBack<const TV>) {
            assert(as_const(r).back() == *prev(end(expected)));
        }
    }

    // Validate view_interface::operator[]
    if (!is_empty) {
        STATIC_ASSERT(CanIndex<R> == random_access_range<V>);
        if constexpr (CanIndex<R>) {
            assert(r[0] == *r.begin());
        }

        STATIC_ASSERT(CanIndex<const R> == (random_access_range<const V> && const_invocable));
        if constexpr (CanIndex<const R>) {
            assert(as_const(r)[0] == *as_const(r).begin());
        }
    }

    // Validate transform_view::base() const&
    STATIC_ASSERT(CanMemberBase<const TV&> == copy_constructible<V>);
    if constexpr (copy_constructible<V> && forward_range<V>) {
        same_as<V> auto b1 = as_const(r).base();
        STATIC_ASSERT(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>);
        if (!is_empty) {
            assert(*b1.begin() == 0); // NB: depends on the test data
            if constexpr (bidirectional_range<V> && common_range<V>) {
                assert(*prev(b1.end()) == 7); // NB: depends on the test data
            }
        }
    }

    // Validate transform_view::base() && (NB: do this last since it leaves r moved-from)
#if !defined(__clang__) && !defined(__EDG__) // TRANSITION, DevCom-1159442
    (void) 42;
#endif // TRANSITION, DevCom-1159442
    if (forward_range<V>) { // intentionally not if constexpr
        same_as<V> auto b2 = move(r).base();
        STATIC_ASSERT(noexcept(move(r).base()) == is_nothrow_move_constructible_v<V>);
        if (!is_empty) {
            assert(*b2.begin() == 0); // NB: depends on the test data
            if constexpr (bidirectional_range<V> && common_range<V>) {
                assert(*prev(b2.end()) == 7); // NB: depends on the test data
            }
        }
    }

    return true;
}

static constexpr int some_ints[]        = {0, 1, 2, 3, 4, 5, 6, 7};
static constexpr int transformed_ints[] = {8, 9, 10, 11, 12, 13, 14, 15};

struct instantiator {
    template <ranges::input_range R>
    static constexpr void call() {
        R r{some_ints};
        test_one(r, transformed_ints);
    }
};

template <class Category, test::Sized IsSized, test::Common IsCommon>
using test_range =
    test::range<Category, const int, IsSized, test::CanDifference{derived_from<Category, random_access_iterator_tag>},
        IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
        test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}>;

constexpr void instantiation_test() {
#ifdef TEST_EVERYTHING
    test_in<instantiator, const int>();
#else // ^^^ test all input range permutations / test only "interesting" permutations vvv
    // The view is sensitive to category, commonality, size, and differencing, but oblivious to proxyness.
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

template <class Category, test::Sized IsSized, test::Common IsCommon>
using move_only_view = test::range<Category, const int, IsSized,
    test::CanDifference{derived_from<Category, random_access_iterator_tag>}, IsCommon,
    test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

constexpr void move_only_view_tests() {
    using test::Common, test::Sized;

    test_one(move_only_view<input_iterator_tag, Sized::no, Common::no>{some_ints}, transformed_ints);
    test_one(move_only_view<input_iterator_tag, Sized::yes, Common::no>{some_ints}, transformed_ints);

    test_one(move_only_view<forward_iterator_tag, Sized::no, Common::no>{some_ints}, transformed_ints);
    test_one(move_only_view<forward_iterator_tag, Sized::no, Common::yes>{some_ints}, transformed_ints);
    test_one(move_only_view<forward_iterator_tag, Sized::yes, Common::no>{some_ints}, transformed_ints);
    test_one(move_only_view<forward_iterator_tag, Sized::yes, Common::yes>{some_ints}, transformed_ints);

    test_one(move_only_view<bidirectional_iterator_tag, Sized::no, Common::no>{some_ints}, transformed_ints);
    test_one(move_only_view<bidirectional_iterator_tag, Sized::no, Common::yes>{some_ints}, transformed_ints);
    test_one(move_only_view<bidirectional_iterator_tag, Sized::yes, Common::no>{some_ints}, transformed_ints);
    test_one(move_only_view<bidirectional_iterator_tag, Sized::yes, Common::yes>{some_ints}, transformed_ints);

    test_one(move_only_view<random_access_iterator_tag, Sized::no, Common::no>{some_ints}, transformed_ints);
    test_one(move_only_view<random_access_iterator_tag, Sized::no, Common::yes>{some_ints}, transformed_ints);
    test_one(move_only_view<random_access_iterator_tag, Sized::yes, Common::no>{some_ints}, transformed_ints);
    test_one(move_only_view<random_access_iterator_tag, Sized::yes, Common::yes>{some_ints}, transformed_ints);
}

struct iterator_instantiator {
    template <input_iterator Iter>
    static constexpr void call() {
        // Pre: Iter is a specialization of test::iterator whose element type is const int
        int mutable_ints[]   = {0, 1, 2, 3, 4, 5, 6, 7};
        const auto make_view = [&] {
            return views::transform(
                ranges::subrange{Iter{mutable_ints}, test::sentinel<int>{ranges::end(mutable_ints)}}, add8);
        };
        using R = decltype(make_view());
        using I = ranges::iterator_t<R>;
        using S = ranges::sentinel_t<R>;

        // Validate nested types
        STATIC_ASSERT(is_same_v<typename I::iterator_concept,
            conditional_t<random_access_iterator<Iter>, random_access_iterator_tag,
                conditional_t<bidirectional_iterator<Iter>, bidirectional_iterator_tag,
                    conditional_t<forward_iterator<Iter>, forward_iterator_tag, input_iterator_tag>>>>);

        using C = typename iterator_traits<Iter>::iterator_category;
        STATIC_ASSERT(is_same_v<typename I::iterator_category,
            conditional_t<is_lvalue_reference_v<invoke_result_t<decltype((add8)), iter_reference_t<Iter>>>,
                conditional_t<derived_from<C, contiguous_iterator_tag>, random_access_iterator_tag, C>,
                input_iterator_tag>>);

        { // Validate iterator special member functions and base
            I defaultConstructed{};
            assert(std::move(defaultConstructed).base().peek() == nullptr);
            STATIC_ASSERT(is_nothrow_default_constructible_v<I>);

            auto r0 = make_view();
            I valueConstructed{r0, Iter{mutable_ints}};
            STATIC_ASSERT(is_nothrow_constructible_v<I, R&, Iter>);

            if constexpr (copyable<Iter>) {
                I copyConstructed{valueConstructed};
                assert(copyConstructed == valueConstructed);
                STATIC_ASSERT(is_nothrow_copy_constructible_v<I>);

                defaultConstructed = copyConstructed;
                assert(defaultConstructed == valueConstructed);
                STATIC_ASSERT(is_nothrow_copy_assignable_v<I>);
            }
            assert(std::move(valueConstructed).base().peek() == mutable_ints);

            if constexpr (forward_iterator<Iter>) {
                auto r1      = make_view();
                const auto i = r1.begin();
                using CI     = ranges::iterator_t<const R>;
                CI conversionConstructed{i};
                assert(conversionConstructed.base().peek() == mutable_ints);
                STATIC_ASSERT(is_nothrow_constructible_v<CI, const I&>);
            }
        }

        { // Validate sentinel constructors and base
            S defaultConstructed{};
            assert(defaultConstructed.base().peek() == nullptr);
            STATIC_ASSERT(is_nothrow_default_constructible_v<S>);

            const test::sentinel<int> s{mutable_ints + 2};
            S valueConstructed{s};
            assert(valueConstructed.base().peek() == s.peek());
            STATIC_ASSERT(is_nothrow_constructible_v<S, const test::sentinel<int>&>);

            S copyConstructed{valueConstructed};
            assert(copyConstructed.base().peek() == valueConstructed.base().peek());
            STATIC_ASSERT(is_nothrow_copy_constructible_v<S>);

            defaultConstructed = copyConstructed;
            assert(defaultConstructed.base().peek() == valueConstructed.base().peek());
            STATIC_ASSERT(is_nothrow_copy_assignable_v<S>);

            if constexpr (forward_iterator<Iter> && indirectly_swappable<Iter>) {
                auto r       = make_view();
                using CS     = ranges::sentinel_t<const R>;
                const S last = r.end();
                CS conversionConstructed{last};
                assert(conversionConstructed.base().peek() == ranges::end(mutable_ints));
                STATIC_ASSERT(is_nothrow_constructible_v<CS, const S&>);
            }
        }

        { // Validate dereference ops
            auto r0 = make_view();
            auto i0 = r0.begin();
            assert(*i0 == add8(mutable_ints[0]));
            STATIC_ASSERT(NOEXCEPT_IDL0(*i0));

            assert(ranges::iter_move(i0) == add8(mutable_ints[0])); // NB: moving from int leaves it unchanged
            STATIC_ASSERT(NOEXCEPT_IDL0(ranges::iter_move(i0)));

            if constexpr (forward_iterator<Iter>) {
                auto i1 = ranges::next(i0);
                ranges::iter_swap(i0, i1);
                assert(mutable_ints[0] == 1);
                assert(mutable_ints[1] == 0);
                ranges::iter_swap(i1, i0);
                assert(mutable_ints[0] == 0);
                assert(mutable_ints[1] == 1);
                STATIC_ASSERT(NOEXCEPT_IDL0(ranges::iter_swap(i0, i1)));
            }
        }

        { // Validate increments
            auto r0 = make_view();
            auto i0 = r0.begin();
            assert(&++i0 == &i0);
            assert(std::move(i0).base().peek() == mutable_ints + 1);
            STATIC_ASSERT(noexcept(++i0));

            auto r1 = make_view();
            auto i1 = r1.begin();
            if constexpr (forward_iterator<Iter>) {
                assert(i1++ == r1.begin());
            } else {
                i1++;
            }
            assert(std::move(i1).base().peek() == mutable_ints + 1);
            STATIC_ASSERT(noexcept(i0++));
        }

        if constexpr (bidirectional_iterator<Iter>) { // Validate decrements
            auto r            = make_view();
            const auto second = ranges::next(r.begin());
            auto i            = second;
            assert(&--i == &i);
            assert(i.base().peek() == mutable_ints);
            STATIC_ASSERT(noexcept(--i));

            i = second;
            assert(i-- == second);
            assert(i.base().peek() == mutable_ints);
            STATIC_ASSERT(noexcept(i--));
        }

        if constexpr (random_access_iterator<Iter>) { // Validate seek operations and []
            auto r = make_view();
            auto i = r.begin();
            assert((i + 2).base().peek() == mutable_ints + 2);
            assert((I{} + 0) == I{});
            STATIC_ASSERT(NOEXCEPT_IDL0(i + 2));

            assert((2 + i).base().peek() == mutable_ints + 2);
            assert((0 + I{}).base().peek() == nullptr);
            STATIC_ASSERT(NOEXCEPT_IDL0(2 + i));

            auto vi = I{};
            assert(&(i += 5) == &i);
            assert(i.base().peek() == mutable_ints + 5);
            assert(&(vi += 0) == &vi);
            assert(vi.base().peek() == nullptr);
            STATIC_ASSERT(NOEXCEPT_IDL0(i += 5));

            assert((i - 2).base().peek() == mutable_ints + 3);
            assert((I{} - 0).base().peek() == nullptr);
            STATIC_ASSERT(NOEXCEPT_IDL0(i - 2));

            assert(&(i -= 3) == &i);
            assert(i.base().peek() == mutable_ints + 2);
            assert(&(vi -= 0) == &vi);
            assert(vi.base().peek() == nullptr);
            STATIC_ASSERT(NOEXCEPT_IDL0(i -= 3));

            assert(i[4] == add8(mutable_ints[6]));
            STATIC_ASSERT(NOEXCEPT_IDL0(i[4]));
        }

        if constexpr (equality_comparable<Iter>) {
            // Validate == and !=
            auto r           = make_view();
            const auto first = r.begin();
            const auto last  = r.end();

            assert(first == first);
            assert(I{} == I{});
            STATIC_ASSERT(noexcept(first == first));

            assert(!(first == last));
            STATIC_ASSERT(noexcept(first == last));
            assert(!(last == first));
            STATIC_ASSERT(noexcept(last == first));

            assert(!(first != first));
            assert(!(I{} != I{}));
            STATIC_ASSERT(noexcept(first != first));

            if constexpr (forward_iterator<Iter>) {
                const auto final = ranges::next(first, last);
                assert(!(first == final));
                assert(first != final);

                assert(last == final);
                assert(final == last);

                assert(!(last != final));
                assert(!(final != last));

                if constexpr (sized_sentinel_for<Iter, Iter>) { // Validate difference
                    assert(first - first == 0);
                    assert(final - first == ranges::ssize(mutable_ints));
                    assert(first - final == -ranges::ssize(mutable_ints));
                    assert(I{} - I{} == 0);
                    STATIC_ASSERT(noexcept(first - first));
                }

                if constexpr (sized_sentinel_for<test::sentinel<int>, Iter>) {
                    assert(last - first == ranges::ssize(mutable_ints));
                    assert(first - last == -ranges::ssize(mutable_ints));
                    STATIC_ASSERT(noexcept(last - first));
                    STATIC_ASSERT(noexcept(first - last));
                }

                if constexpr (random_access_iterator<Iter>) { // Validate relational operators
                    assert(!(first < first));
                    assert(first < final);
                    assert(!(final < first));
                    assert(!(I{} < I{}));
                    STATIC_ASSERT(noexcept(first < final));

                    assert(!(first > first));
                    assert(!(first > final));
                    assert(final > first);
                    assert(!(I{} > I{}));
                    STATIC_ASSERT(noexcept(first > final));

                    assert(first <= first);
                    assert(first <= final);
                    assert(!(final <= first));
                    assert(I{} <= I{});
                    STATIC_ASSERT(noexcept(first <= final));

                    assert(first >= first);
                    assert(!(first >= final));
                    assert(final >= first);
                    assert(I{} >= I{});
                    STATIC_ASSERT(noexcept(first >= final));

                    if constexpr (three_way_comparable<Iter>) { // Validate spaceship
                        assert((first <=> first) == strong_ordering::equal);
                        assert((first <=> final) == strong_ordering::less);
                        assert((final <=> first) == strong_ordering::greater);
                        assert((I{} <=> I{}) == strong_ordering::equal);
                        STATIC_ASSERT(noexcept(first <=> final));
                    }
                }
            }
        }
    }
};

template <class Category, test::CanDifference Diff>
using test_iterator =
    test::iterator<Category, int, Diff, test::CanCompare{derived_from<Category, forward_iterator_tag>},
        test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}>;

constexpr void iterator_instantiation_test() {
    using test::CanDifference;

    iterator_instantiator::call<test_iterator<input_iterator_tag, CanDifference::no>>();

    iterator_instantiator::call<test_iterator<forward_iterator_tag, CanDifference::no>>();
    iterator_instantiator::call<test_iterator<forward_iterator_tag, CanDifference::yes>>();

    iterator_instantiator::call<test_iterator<bidirectional_iterator_tag, CanDifference::no>>();
    iterator_instantiator::call<test_iterator<bidirectional_iterator_tag, CanDifference::yes>>();

    iterator_instantiator::call<test_iterator<random_access_iterator_tag, CanDifference::yes>>();
    iterator_instantiator::call<test_iterator<contiguous_iterator_tag, CanDifference::yes>>();
}

int main() {
    { // Validate copyable views
        constexpr span<const int> s{some_ints};
        STATIC_ASSERT(test_one(s, transformed_ints));
        test_one(s, transformed_ints);
    }

    // Validate move-only views
    STATIC_ASSERT((move_only_view_tests(), true));
    move_only_view_tests();

    { // Validate non-views
        STATIC_ASSERT(test_one(some_ints, transformed_ints));
        test_one(some_ints, transformed_ints);

        {
            vector vec(ranges::begin(some_ints), ranges::end(some_ints));
            test_one(vec, transformed_ints);
        }
        {
            forward_list lst(ranges::begin(some_ints), ranges::end(some_ints));
            test_one(lst, transformed_ints);
        }

        STATIC_ASSERT((instantiation_test(), true));
        instantiation_test();
    }

    { // Validate a non-view borrowed range
        constexpr span s{some_ints};
        STATIC_ASSERT(test_one(s, transformed_ints));
        test_one(s, transformed_ints);
    }

    STATIC_ASSERT((iterator_instantiation_test(), true));
    iterator_instantiation_test();
}
