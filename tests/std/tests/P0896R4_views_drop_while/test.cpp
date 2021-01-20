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
template <int X>
constexpr auto is_less_than = [](const auto& x) { return x < X; };

using Pred = remove_const_t<decltype(is_less_than<3>)>;
STATIC_ASSERT(is_nothrow_copy_constructible_v<Pred>&& is_nothrow_move_constructible_v<Pred>);

constexpr auto pipeline = views::drop_while(is_less_than<3>) | views::drop_while(is_less_than<3>)
                        | views::drop_while(is_less_than<3>) | views::drop_while(is_less_than<3>);

template <class Rng, class V = views::all_t<Rng>>
using pipeline_t = ranges::drop_while_view<
    ranges::drop_while_view<ranges::drop_while_view<ranges::drop_while_view<V, Pred>, Pred>, Pred>, Pred>;

template <class Rng>
concept CanViewDropWhile = requires(Rng&& r) {
    views::drop_while(static_cast<Rng&&>(r), is_less_than<3>);
};

template <ranges::input_range Rng, ranges::random_access_range Expected>
constexpr bool test_one(Rng&& rng, Expected&& expected) {
    using ranges::drop_while_view, ranges::bidirectional_range, ranges::common_range, ranges::contiguous_range,
        ranges::enable_borrowed_range, ranges::forward_range, ranges::iterator_t, ranges::prev,
        ranges::random_access_range;

    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;

    using V = views::all_t<Rng>;
    using R = drop_while_view<V, Pred>;
    STATIC_ASSERT(ranges::view<R>);
    STATIC_ASSERT(ranges::input_range<R>);
    STATIC_ASSERT(forward_range<R> == forward_range<Rng>);
    STATIC_ASSERT(bidirectional_range<R> == bidirectional_range<Rng>);
    STATIC_ASSERT(random_access_range<R> == random_access_range<Rng>);
    STATIC_ASSERT(contiguous_range<R> == contiguous_range<Rng>);

    // Validate range adaptor object and range adaptor closure
    constexpr auto closure = views::drop_while(is_less_than<3>);

    // ... with lvalue argument
    STATIC_ASSERT(CanViewDropWhile<Rng&> == (!is_view || copyable<V>) );
    if constexpr (CanViewDropWhile<Rng&>) { // Validate lvalue
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::drop_while(rng, is_less_than<3>)), R>);
        STATIC_ASSERT(noexcept(views::drop_while(rng, is_less_than<3>)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(rng | closure), R>);
        STATIC_ASSERT(noexcept(rng | closure) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(rng | pipeline), pipeline_t<Rng&>>);
        STATIC_ASSERT(noexcept(rng | pipeline) == is_noexcept);
    }

    // ... with const lvalue argument
    STATIC_ASSERT(CanViewDropWhile<const remove_reference_t<Rng>&> == (!is_view || copyable<V>) );
    if constexpr (is_view && copyable<V>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::drop_while(as_const(rng), is_less_than<3>)), R>);
        STATIC_ASSERT(noexcept(views::drop_while(as_const(rng), is_less_than<3>)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | closure), R>);
        STATIC_ASSERT(noexcept(as_const(rng) | closure) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | pipeline), pipeline_t<const remove_reference_t<Rng>&>>);
        STATIC_ASSERT(noexcept(as_const(rng) | pipeline) == is_noexcept);
    } else if constexpr (!is_view) {
        using RC = drop_while_view<views::all_t<const remove_reference_t<Rng>&>, Pred>;
        constexpr bool is_noexcept =
            is_nothrow_constructible_v<RC, const remove_reference_t<Rng>&, decltype((is_less_than<3>) )>;

        STATIC_ASSERT(same_as<decltype(views::drop_while(as_const(rng), is_less_than<3>)), RC>);
        STATIC_ASSERT(noexcept(views::drop_while(as_const(rng), is_less_than<3>)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | closure), RC>);
        STATIC_ASSERT(noexcept(as_const(rng) | closure) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | pipeline), pipeline_t<const remove_reference_t<Rng>&>>);
        STATIC_ASSERT(noexcept(as_const(rng) | pipeline) == is_noexcept);
    }

    // ... with rvalue argument
    STATIC_ASSERT(CanViewDropWhile<remove_reference_t<Rng>> == is_view || enable_borrowed_range<remove_cvref_t<Rng>>);
    if constexpr (is_view) {
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;
        STATIC_ASSERT(same_as<decltype(views::drop_while(move(rng), is_less_than<3>)), R>);
        STATIC_ASSERT(noexcept(views::drop_while(move(rng), is_less_than<3>)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | closure), R>);
        STATIC_ASSERT(noexcept(move(rng) | closure) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | pipeline), pipeline_t<remove_reference_t<Rng>>>);
        STATIC_ASSERT(noexcept(move(rng) | pipeline) == is_noexcept);
    } else if constexpr (enable_borrowed_range<remove_cvref_t<Rng>>) {
        using S                    = decltype(ranges::subrange{move(rng)});
        using RS                   = drop_while_view<S, Pred>;
        constexpr bool is_noexcept = noexcept(S{move(rng)});

        STATIC_ASSERT(same_as<decltype(views::drop_while(move(rng), is_less_than<3>)), RS>);
        STATIC_ASSERT(noexcept(views::drop_while(move(rng), is_less_than<3>)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | closure), RS>);
        STATIC_ASSERT(noexcept(move(rng) | closure) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | pipeline), pipeline_t<remove_reference_t<Rng>>>);
        STATIC_ASSERT(noexcept(move(rng) | pipeline) == is_noexcept);
    }

    // ... with const rvalue argument
    STATIC_ASSERT(CanViewDropWhile<const remove_reference_t<Rng>> == (is_view && copyable<V>)
                  || (!is_view && enable_borrowed_range<remove_cvref_t<Rng>>) );
    if constexpr (is_view && copyable<V>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::drop_while(move(as_const(rng)), is_less_than<3>)), R>);
        STATIC_ASSERT(noexcept(views::drop_while(move(as_const(rng)), is_less_than<3>)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | closure), R>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | closure) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | pipeline), pipeline_t<const remove_reference_t<Rng>>>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | pipeline) == is_noexcept);
    } else if constexpr (!is_view && enable_borrowed_range<remove_cvref_t<Rng>>) {
        using S                    = decltype(ranges::subrange{move(as_const(rng))});
        using RS                   = drop_while_view<S, Pred>;
        constexpr bool is_noexcept = noexcept(S{move(as_const(rng))});

        STATIC_ASSERT(same_as<decltype(views::drop_while(move(as_const(rng)), is_less_than<3>)), RS>);
        STATIC_ASSERT(noexcept(views::drop_while(move(as_const(rng)), is_less_than<3>)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | closure), RS>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | closure) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | pipeline), pipeline_t<const remove_reference_t<Rng>>>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | pipeline) == is_noexcept);
    }

    // Validate deduction guide
    same_as<R> auto r = drop_while_view{forward<Rng>(rng), is_less_than<3>};
    assert(ranges::equal(r, expected));
    if constexpr (forward_range<V>) {
        // drop_while_view memoizes the first iterator, let's repeat a few times for coverage.
        assert(ranges::equal(r, expected));
        assert(ranges::equal(r, expected));
        assert(ranges::equal(r, expected));
    }

    { // Validate drop_while_view::pred
        [[maybe_unused]] same_as<Pred> auto pred_copy = as_const(r).pred();
        STATIC_ASSERT(noexcept(as_const(r).pred()));
    }

    const bool is_empty = ranges::empty(expected);

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

    STATIC_ASSERT(!CanEmpty<const R>);
    STATIC_ASSERT(!CanBool<const R>);

    // Validate drop_while_view::begin
    STATIC_ASSERT(CanMemberBegin<R>);
    if (forward_range<V>) { // intentionally not if constexpr
        // Ditto "let's make some extra calls because memoization"
        const same_as<iterator_t<R>> auto i = r.begin();
        if (!is_empty) {
            assert(*i == *begin(expected));
            assert(*r.begin() == *begin(expected));
            assert(*r.begin() == *begin(expected));
        }

        if constexpr (copyable<V>) {
            auto r2                              = r;
            const same_as<iterator_t<R>> auto i2 = r2.begin();
            if (!is_empty) {
                assert(*i2 == *i);
                assert(*r2.begin() == *i2);
                assert(*r2.begin() == *i2);
            }
        }

        STATIC_ASSERT(!CanBegin<const R>);
    }

    // Validate drop_while_view::end
    STATIC_ASSERT(CanMemberEnd<R>);
    if (!is_empty) {
        if constexpr (common_range<V>) {
            same_as<iterator_t<R>> auto i = r.end();
            if constexpr (bidirectional_range<V>) {
                assert(*prev(i) == *prev(end(expected)));
            }
        } else {
            [[maybe_unused]] same_as<ranges::sentinel_t<R>> auto s = r.end();
        }

        if constexpr (bidirectional_range<V> && common_range<V> && copyable<V>) {
            auto r2 = r;
            assert(*prev(r2.end()) == *prev(end(expected)));
        }

        STATIC_ASSERT(!CanEnd<const R>);
    }

    // Validate view_interface::data
    STATIC_ASSERT(CanMemberData<R> == contiguous_range<V>);
    STATIC_ASSERT(CanData<R&> == contiguous_range<V>);
    if constexpr (contiguous_range<V>) {
        const same_as<remove_reference_t<ranges::range_reference_t<V>>*> auto ptr1 = r.data();
        assert(ptr1 == to_address(r.begin()));
    }
    STATIC_ASSERT(!CanData<const R&>);

    // Validate view_interface::size
    STATIC_ASSERT(CanMemberSize<R> == CanSize<Rng>);
    if constexpr (CanMemberSize<R>) {
        assert(r.size() == static_cast<decltype(r.size())>(ranges::size(expected)));
    } else {
        STATIC_ASSERT(!CanSize<R>);
    }

    // Validate view_interface::operator[]
    STATIC_ASSERT(CanIndex<R> == random_access_range<V>);
    if constexpr (CanIndex<R>) {
        if (!is_empty) {
            assert(r[0] == expected[0]);
        }
    }
    STATIC_ASSERT(!CanIndex<const R>);

    // Validate view_interface::front and back
    STATIC_ASSERT(CanMemberFront<R> == forward_range<V>);
    if constexpr (CanMemberFront<R>) {
        if (!is_empty) {
            assert(r.front() == *begin(expected));
        }
    }
    STATIC_ASSERT(!CanMemberFront<const R>);

    STATIC_ASSERT(CanMemberBack<R> == (bidirectional_range<R> && common_range<R>) );
    if constexpr (CanMemberBack<R>) {
        if (!is_empty) {
            assert(r.back() == *prev(end(expected)));
        }
    }
    STATIC_ASSERT(!CanMemberBack<const R>);

    // Validate drop_while_view::base() const&
    STATIC_ASSERT(CanMemberBase<const R&> == copy_constructible<V>);
    if constexpr (copy_constructible<V> && forward_range<V>) {
        same_as<V> auto b1 = as_const(r).base();
        STATIC_ASSERT(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>);
        if (!is_empty) {
            assert(*b1.begin() == 0); // NB: depends on the test data
            if constexpr (bidirectional_range<V> && common_range<V>) {
                assert(*prev(b1.end()) == *prev(end(expected)));
            }
        }
    }

    if (forward_range<V>) { // intentionally not if constexpr
        same_as<V> auto b2 = move(r).base();
        STATIC_ASSERT(noexcept(move(r).base()) == is_nothrow_move_constructible_v<V>);
        if (!is_empty) {
            assert(*b2.begin() == 0); // NB: depends on the test data
            if constexpr (bidirectional_range<V> && common_range<V>) {
                assert(*prev(b2.end()) == *prev(end(expected)));
            }
        }
    }
    return true;
}

static constexpr int some_ints[]               = {0, 1, 2, 3, 4, 3, 2, 1};
static constexpr int expected_output[]         = {3, 4, 3, 2, 1};
static constexpr int expected_output_reverse[] = {0, 1, 2, 3, 4, 3};

struct instantiator {
    template <ranges::input_range R>
    static constexpr void call() {
        R r{some_ints};
        test_one(r, expected_output);
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
        STATIC_ASSERT(test_one(s, expected_output));
        test_one(s, expected_output);
    }
    { // ... move-only
        test_one(move_only_view<input_iterator_tag, test::Common::no>{some_ints}, expected_output);
        test_one(move_only_view<forward_iterator_tag, test::Common::no>{some_ints}, expected_output);
        test_one(move_only_view<forward_iterator_tag, test::Common::yes>{some_ints}, expected_output);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints}, expected_output);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints}, expected_output);
        test_one(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints}, expected_output);
        test_one(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints}, expected_output);
    }

    // Validate non-views
    {
        STATIC_ASSERT(test_one(some_ints, expected_output));
        test_one(some_ints, expected_output);
    }
    {
        vector vec(ranges::begin(some_ints), ranges::end(some_ints));
        test_one(vec, expected_output);
    }
    {
        forward_list lst(ranges::begin(some_ints), ranges::end(some_ints));
        test_one(lst, expected_output);
    }

    // Validate a non-view borrowed range
    {
        constexpr span s{some_ints};
        STATIC_ASSERT(test_one(s, expected_output));
        test_one(s, expected_output);
    }

    // drop_while/reverse interaction test
    {
        auto dwr_pipe = views::drop_while(is_less_than<3>) | views::reverse;
        auto rdw_pipe = views::reverse | views::drop_while(is_less_than<3>);

        auto r0  = some_ints | dwr_pipe;
        using R0 = decltype(r0);
        STATIC_ASSERT(ranges::bidirectional_range<R0> && ranges::view<R0>);
        assert(ranges::equal(r0, views::reverse(expected_output)));

        auto r1  = some_ints | rdw_pipe;
        using R1 = decltype(r1);
        STATIC_ASSERT(ranges::bidirectional_range<R1> && ranges::view<R1>);
        assert(ranges::equal(r1, views::reverse(expected_output_reverse)));
    }

    { // empty range
        STATIC_ASSERT(test_one(span<const int, 0>{}, span<const int, 0>{}));
        test_one(span<const int, 0>{}, span<const int, 0>{});
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

        (void) views::drop_while(Fn{})(span<int>{});
    }
}
