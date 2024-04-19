// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <forward_list>
#include <ranges>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>
using namespace std;

constexpr auto equal_ranges = [](auto&& left, auto&& right) { return ranges::equal(left, right); };

using Pred = less<>;
constexpr Pred pred{};

template <class Rng>
concept CanViewChunkBy = requires(Rng&& r) { views::chunk_by(forward<Rng>(r), pred); };

template <ranges::forward_range Rng, class Expected>
constexpr bool test_one(Rng&& rng, Expected&& expected) {
    using ranges::chunk_by_view, ranges::begin, ranges::end, ranges::iterator_t, ranges::sentinel_t,
        ranges::common_range, ranges::bidirectional_range, ranges::prev;
    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;

    using V = views::all_t<Rng>;
    using R = chunk_by_view<V, Pred>;
    static_assert(ranges::view<R>);
    static_assert(ranges::forward_range<R>);
    static_assert(bidirectional_range<R> == bidirectional_range<Rng>);
    static_assert(!ranges::random_access_range<R>);

    // Validate range adaptor object and range adaptor closure
    constexpr auto closure = views::chunk_by(pred);

    // ... with lvalue argument
    static_assert(CanViewChunkBy<Rng&> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewChunkBy<Rng&>) {
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<decltype(views::chunk_by(rng, pred)), R>);
        static_assert(noexcept(views::chunk_by(rng, pred)) == is_noexcept);

        static_assert(same_as<decltype(rng | closure), R>);
        static_assert(noexcept(rng | closure) == is_noexcept);
    }

    // ... with const lvalue argument
    static_assert(CanViewChunkBy<const remove_reference_t<Rng>&> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewChunkBy<const remove_reference_t<Rng>&>) {
        using RC                   = chunk_by_view<views::all_t<const remove_reference_t<Rng>&>, Pred>;
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<decltype(views::chunk_by(as_const(rng), pred)), RC>);
        static_assert(noexcept(views::chunk_by(as_const(rng), pred)) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | closure), RC>);
        static_assert(noexcept(as_const(rng) | closure) == is_noexcept);
    }

    // ... with rvalue argument
    static_assert(CanViewChunkBy<remove_reference_t<Rng>> == (is_view || movable<remove_reference_t<Rng>>) );
    if constexpr (CanViewChunkBy<remove_reference_t<Rng>>) {
        using RS                   = chunk_by_view<views::all_t<remove_reference_t<Rng>>, Pred>;
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;

        static_assert(same_as<decltype(views::chunk_by(move(rng), pred)), RS>);
        static_assert(noexcept(views::chunk_by(move(rng), pred)) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | closure), RS>);
        static_assert(noexcept(move(rng) | closure) == is_noexcept);
    }

    // ... with const rvalue argument
    static_assert(CanViewChunkBy<const remove_reference_t<Rng>> == (is_view && copy_constructible<V>) );
    if constexpr (CanViewChunkBy<const remove_reference_t<Rng>>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<decltype(views::chunk_by(move(as_const(rng)), pred)), R>);
        static_assert(noexcept(views::chunk_by(move(as_const(rng)), pred)) == is_noexcept);

        static_assert(same_as<decltype(move(as_const(rng)) | closure), R>);
        static_assert(noexcept(move(as_const(rng)) | closure) == is_noexcept);
    }

    // Validate deduction guide
    same_as<R> auto r = chunk_by_view{forward<Rng>(rng), pred};
    assert(ranges::equal(r, expected, equal_ranges));

    { // Validate chunk_by_view::pred
        [[maybe_unused]] same_as<Pred> auto pred_copy = as_const(r).pred();
        static_assert(noexcept(as_const(r).pred()));
    }

    // Validate view_interface::empty and operator bool
    const bool is_empty = ranges::empty(expected);
    static_assert(CanMemberEmpty<R>);
    static_assert(CanBool<R>);
    assert(r.empty() == is_empty);
    assert(static_cast<bool>(r) == !is_empty);
    static_assert(!CanMemberEmpty<const R>);
    static_assert(!CanBool<const R>);

    // Validate chunk_by_view::begin
    static_assert(CanMemberBegin<R>);
    const same_as<iterator_t<R>> auto i = r.begin();
    if (!is_empty) {
        assert(ranges::equal(*i, *begin(expected)));
    }

    if constexpr (copy_constructible<V>) {
        auto r2                              = r;
        const same_as<iterator_t<R>> auto i2 = r2.begin();
        if (!is_empty) {
            assert(ranges::equal(*i2, *i));
        }
    }

    static_assert(!CanBegin<const R>);

    // Validate chunk_by_view::end
    static_assert(CanMemberEnd<R>);
    same_as<sentinel_t<R>> auto s = r.end();
    assert((r.begin() == s) == is_empty);
    if constexpr (common_range<V>) {
        static_assert(same_as<sentinel_t<R>, iterator_t<R>>);
        if constexpr (bidirectional_range<V>) {
            if (!is_empty) {
                assert(ranges::equal(*prev(s), *prev(end(expected))));
            }
        }
    } else {
        static_assert(same_as<sentinel_t<R>, default_sentinel_t>);
    }

    if constexpr (bidirectional_range<V> && common_range<V> && copy_constructible<V>) {
        auto r2 = r;
        if (!is_empty) {
            assert(ranges::equal(*prev(r2.end()), *prev(end(expected))));
        }
    }

    static_assert(!CanEnd<const R>);

    // Validate view_interface::data
    static_assert(!CanData<R>);
    static_assert(!CanData<const R>);

    // Validate view_interface::size
    static_assert(!CanSize<R>);
    static_assert(!CanSize<const R>);

    // Validate view_interface::operator[]
    static_assert(!CanIndex<R>);
    static_assert(!CanIndex<const R>);

    if (!is_empty) {
        // Validate view_interface::front and back
        static_assert(CanMemberFront<R>);
        assert(ranges::equal(r.front(), *begin(expected)));

        static_assert(CanMemberBack<R> == (bidirectional_range<V> && common_range<V>) );
        if constexpr (CanMemberBack<R>) {
            assert(ranges::equal(r.back(), *prev(end(expected))));
        }

        static_assert(!CanMemberFront<const R>);
        static_assert(!CanMemberBack<const R>);
    }

    // Validate chunk_by_view::base() const&
    static_assert(CanMemberBase<const R&> == copy_constructible<V>);
    if constexpr (copy_constructible<V>) {
        same_as<V> auto b1 = as_const(r).base();
        static_assert(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>);
        if (!is_empty) {
            assert(*b1.begin() == *begin(*begin(expected)));
        }
    }

    // Validate chunk_by_view::base() && (NB: do this last since it leaves r moved-from)
    same_as<V> auto b2 = move(r).base();
    static_assert(noexcept(move(r).base()) == is_nothrow_move_constructible_v<V>);
    if (!is_empty) {
        assert(*b2.begin() == *begin(*begin(expected)));
    }

    return true;
}

static constexpr string_view some_chars                = "01234321"sv;
static constexpr string_view expected_output[]         = {"01234"sv, "3"sv, "2"sv, "1"sv};
static constexpr string_view expected_output_reverse[] = {"1234"sv, "3"sv, "2"sv, "1"sv, "0"sv};

struct instantiator {
    template <ranges::forward_range R>
    static constexpr void call() {
        R r{some_chars};
        test_one(r, expected_output);
    }
};

template <class Category, test::Common IsCommon>
using test_range = test::range<Category, const char, test::Sized::no,
    test::CanDifference{derived_from<Category, random_access_iterator_tag>}, IsCommon,
    test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}>;

constexpr void instantiation_test() {
#ifdef TEST_EVERYTHING
    test_fwd<instantiator, const char>();
#else // ^^^ test all forward range permutations / test only "interesting" permutations vvv
    // The view is sensitive to category and commonality, but oblivious to size, differencing, and proxyness.
    using test::Common;

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

void test_gh_2889() { // COMPILE-ONLY
    // GH-2889 <ranges>: chunk_by_view's helper lambda does not specify return type
    struct Bool { // NB: poor model of boolean-testable; don't use in runtime code.
        Bool()            = default;
        Bool(const Bool&) = delete;
        Bool& operator!() {
            return *this;
        }
        operator bool() {
            return true;
        }
    };

    Bool x[3];
    auto r = x | views::chunk_by([](Bool& b, Bool&) -> Bool& { return b; });
    (void) r.begin();
}

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const char, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

int main() {
    // Validate views
    { // ... copyable
        constexpr span<const char> s{some_chars};
        static_assert(test_one(s, expected_output));
        test_one(s, expected_output);
    }
    { // ... move-only
        test_one(move_only_view<forward_iterator_tag, test::Common::no>{some_chars}, expected_output);
        test_one(move_only_view<forward_iterator_tag, test::Common::yes>{some_chars}, expected_output);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_chars}, expected_output);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_chars}, expected_output);
        test_one(move_only_view<random_access_iterator_tag, test::Common::no>{some_chars}, expected_output);
        test_one(move_only_view<random_access_iterator_tag, test::Common::yes>{some_chars}, expected_output);
    }

    // Validate non-views
    {
        static_assert(test_one(some_chars, expected_output));
        test_one(some_chars, expected_output);
    }
    {
        vector vec(ranges::begin(some_chars), ranges::end(some_chars));
        test_one(vec, expected_output);
    }
    {
        forward_list lst(ranges::begin(some_chars), ranges::end(some_chars));
        test_one(lst, expected_output);
    }

    // chunk_by/reverse interaction test
    {
        auto cbr_pipe = views::chunk_by(pred) | views::reverse;
        auto rcb_pipe = views::reverse | views::chunk_by(pred);

        auto r0  = some_chars | cbr_pipe;
        using R0 = decltype(r0);
        static_assert(ranges::bidirectional_range<R0> && ranges::view<R0>);
        assert(ranges::equal(r0, views::reverse(expected_output), equal_ranges));

        auto r1  = some_chars | rcb_pipe;
        using R1 = decltype(r1);
        static_assert(ranges::bidirectional_range<R1> && ranges::view<R1>);
        assert(ranges::equal(r1, expected_output_reverse, equal_ranges));
    }

    { // empty range
        static_assert(test_one(span<const int, 0>{}, span<string_view, 0>{}));
        test_one(span<const int, 0>{}, span<string_view, 0>{});
    }

    static_assert((instantiation_test(), true));
    instantiation_test();

    { // Validate that predicates are moved into the range adaptor closure, and into the view object from an rvalue
      // closure
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

            bool operator()(int, int) const {
                return true;
            }
        };

        (void) views::chunk_by(Fn{})(span<int>{});
    }
}
