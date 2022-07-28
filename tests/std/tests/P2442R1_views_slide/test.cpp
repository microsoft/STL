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

template <class Rng>
concept CanViewSlide = requires(Rng&& r) {
    views::slide(forward<Rng>(r), 4);
};

constexpr auto equal_ranges = [](auto&& left, auto&& right) { return ranges::equal(left, right); };

template <ranges::forward_range Rng, class Expected>
constexpr bool test_one(Rng&& rng, Expected&& expected) {
    using ranges::bidirectional_range, ranges::common_range, ranges::random_access_range, ranges::sized_range;
    using ranges::slide_view, ranges::begin, ranges::end, ranges::equal, ranges::iterator_t, ranges::sentinel_t,
        ranges::prev;
    constexpr bool is_view              = ranges::view<remove_cvref_t<Rng>>;
    constexpr bool caches_nothing       = random_access_range<Rng> && sized_range<Rng>;
    constexpr bool caches_last          = !caches_nothing && bidirectional_range<Rng> && common_range<Rng>;
    constexpr bool caches_first         = !caches_nothing && !caches_last;
    constexpr bool caches_nothing_const = random_access_range<const Rng> && sized_range<const Rng>;

    using V = views::all_t<Rng>;
    using R = slide_view<V>;

    STATIC_ASSERT(ranges::view<R>);
    STATIC_ASSERT(ranges::forward_range<R>);
    STATIC_ASSERT(bidirectional_range<R> == bidirectional_range<Rng>);
    STATIC_ASSERT(random_access_range<R> == random_access_range<Rng>);

    // Validate non-default-initializability
    STATIC_ASSERT(!is_default_constructible_v<R>);

    // Validate borrowed_range
    static_assert(ranges::borrowed_range<R> == ranges::borrowed_range<V>);

    // Validate range adaptor object and range adaptor closure
    constexpr auto closure = views::slide(4);

    // ... with lvalue argument
    STATIC_ASSERT(CanViewSlide<Rng&> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewSlide<Rng&>) {
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::slide(rng, 4)), R>);
        STATIC_ASSERT(noexcept(views::slide(rng, 4)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(rng | closure), R>);
        STATIC_ASSERT(noexcept(rng | closure) == is_noexcept);
    }

    // ... with const lvalue argument
    STATIC_ASSERT(CanViewSlide<const remove_reference_t<Rng>&> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewSlide<const remove_reference_t<Rng>&>) {
        using RC                   = slide_view<views::all_t<const remove_reference_t<Rng>&>>;
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(!is_default_constructible_v<RC>);

        STATIC_ASSERT(same_as<decltype(views::slide(as_const(rng), 4)), RC>);
        STATIC_ASSERT(noexcept(views::slide(as_const(rng), 4)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | closure), RC>);
        STATIC_ASSERT(noexcept(as_const(rng) | closure) == is_noexcept);
    }

    // ... with rvalue argument
    STATIC_ASSERT(CanViewSlide<remove_reference_t<Rng>> == (is_view || movable<remove_reference_t<Rng>>) );
    if constexpr (CanViewSlide<remove_reference_t<Rng>>) {
        using RS                   = slide_view<views::all_t<remove_reference_t<Rng>>>;
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;

        STATIC_ASSERT(!is_default_constructible_v<RS>);

        STATIC_ASSERT(same_as<decltype(views::slide(move(rng), 4)), RS>);
        STATIC_ASSERT(noexcept(views::slide(move(rng), 4)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | closure), RS>);
        STATIC_ASSERT(noexcept(move(rng) | closure) == is_noexcept);
    }

    // ... with const rvalue argument
    STATIC_ASSERT(CanViewSlide<const remove_reference_t<Rng>> == (is_view && copy_constructible<V>) );
    if constexpr (CanViewSlide<const remove_reference_t<Rng>>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::slide(move(as_const(rng)), 4)), R>);
        STATIC_ASSERT(noexcept(views::slide(move(as_const(rng)), 4)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | closure), R>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | closure) == is_noexcept);
    }

    // Validate deduction guide
    same_as<R> auto r = slide_view{forward<Rng>(rng), 4};
    assert(equal(r, expected, equal_ranges));
    const bool is_empty = ranges::empty(expected);

    // Validate view_interface::empty and operator bool
    STATIC_ASSERT(CanMemberEmpty<R>);
    STATIC_ASSERT(CanBool<R>);
    assert(r.empty() == is_empty);
    assert(static_cast<bool>(r) == !is_empty);

    STATIC_ASSERT(CanMemberEmpty<const R> == caches_nothing_const);
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

    // Validate slide_view::begin
    STATIC_ASSERT(CanMemberBegin<R>);
    {
        const same_as<iterator_t<R>> auto i = r.begin();
        if (!is_empty) {
            assert(equal(*i, *begin(expected)));
        }

        if constexpr (copy_constructible<V>) {
            auto r2                              = r;
            const same_as<iterator_t<R>> auto i2 = r2.begin();
            if (!is_empty) {
                assert(equal(*i2, *i));
            }
        }
    }

    STATIC_ASSERT(CanMemberBegin<const R> == (random_access_range<const V> && sized_range<const V>) );
    if constexpr (CanMemberBegin<const R>) {
        const same_as<iterator_t<const R>> auto ci = as_const(r).begin();
        if (!is_empty) {
            assert(equal(*ci, *begin(expected)));
        }

        if constexpr (copy_constructible<V>) {
            const auto r2                               = r;
            const same_as<iterator_t<const R>> auto ci2 = r2.begin();
            if (!is_empty) {
                assert(equal(*ci2, *ci));
            }
        }
    }

    // Validate slide_view::end
    STATIC_ASSERT(CanMemberEnd<R>);
    {
        const same_as<sentinel_t<R>> auto s = r.end();
        assert((r.begin() == s) == is_empty);
        STATIC_ASSERT(common_range<R> == (!caches_first || common_range<V>) );
        if constexpr (common_range<R> && bidirectional_range<V>) {
            if (!is_empty) {
                assert(equal(*prev(s), *prev(end(expected))));
            }

            if constexpr (copy_constructible<V>) {
                auto r2 = r;
                if (!is_empty) {
                    assert(equal(*prev(r2.end()), *prev(end(expected))));
                }
            }
        }
    }

    STATIC_ASSERT(CanMemberEnd<const R> == caches_nothing_const);
    if constexpr (CanMemberEnd<const R>) {
        const same_as<sentinel_t<const R>> auto cs = as_const(r).end();
        assert((r.begin() == cs) == is_empty);
        STATIC_ASSERT(common_range<const R> == (!caches_first || common_range<const V>) );
        if constexpr (common_range<const R> && bidirectional_range<V>) {
            if (!is_empty) {
                assert(equal(*prev(cs), *prev(end(expected))));
            }

            if constexpr (copy_constructible<V>) {
                const auto r2 = r;
                if (!is_empty) {
                    assert(equal(*prev(r2.end()), *prev(end(expected))));
                }
            }
        }
    }

    // Validate slide_view::size
    STATIC_ASSERT(CanMemberSize<R> == sized_range<V>);
    if constexpr (CanMemberSize<R>) {
        same_as<_Make_unsigned_like_t<ranges::range_difference_t<V>>> auto s = r.size();
        assert(s == ranges::size(expected));
        STATIC_ASSERT(noexcept(r.size()));
    }

    STATIC_ASSERT(CanMemberSize<const R> == sized_range<const V>);
    if constexpr (CanMemberSize<const R>) {
        same_as<_Make_unsigned_like_t<ranges::range_difference_t<const V>>> auto s = as_const(r).size();
        assert(s == ranges::size(expected));
        STATIC_ASSERT(noexcept(as_const(r).size()));
    }

    if (is_empty) {
        return true;
    }

    // Validate view_interface::data
    STATIC_ASSERT(!CanData<R>);
    STATIC_ASSERT(!CanData<const R>);

    // Validate view_interface::operator[]
    STATIC_ASSERT(CanIndex<R> == random_access_range<V>);
    if constexpr (CanIndex<R>) {
        assert(equal(r[0], expected[0]));
    }

    STATIC_ASSERT(CanIndex<const R> == random_access_range<const V>);
    if constexpr (CanIndex<const R>) {
        assert(equal(as_const(r)[0], expected[0]));
    }

    // Validate view_interface::front
    STATIC_ASSERT(CanMemberFront<R>);
    assert(equal(r.front(), *begin(expected)));

    STATIC_ASSERT(CanMemberFront<const R> == caches_nothing_const);
    if constexpr (CanMemberFront<const R>) {
        assert(equal(as_const(r).front(), *begin(expected)));
    }

    // Validate view_interface::back
    STATIC_ASSERT(CanMemberBack<R> == (bidirectional_range<V> && (!caches_first || common_range<V>) ));
    if constexpr (CanMemberBack<R>) {
        assert(equal(r.back(), *prev(end(expected))));
    }

    STATIC_ASSERT(CanMemberBack<const R> == caches_nothing_const);
    if constexpr (CanMemberBack<const R>) {
        assert(equal(as_const(r).back(), *prev(end(expected))));
    }

    // validate iterators
    {
        [[maybe_unused]] const iterator_t<R> defaulted;
        same_as<iterator_t<R>> auto i = r.begin();

        assert(equal(*i++, expected[0]));
        assert(equal(*++i, expected[2]));

        if constexpr (bidirectional_range<R>) {
            assert(equal(*i--, expected[2]));
            assert(equal(*--i, expected[0]));
        }

        if constexpr (random_access_range<R>) {
            i += 2;
            assert(equal(*i, expected[2]));

            i -= 2;
            assert(equal(*i, expected[0]));

            assert(equal(i[2], expected[2]));

            const same_as<iterator_t<R>> auto i2 = i + 2;
            assert(equal(*i2, expected[2]));

            const same_as<iterator_t<R>> auto i3 = 2 + i;
            assert(equal(*i3, expected[2]));

            const same_as<iterator_t<R>> auto i4 = i3 - 2;
            assert(equal(*i4, expected[0]));

            const same_as<ranges::range_difference_t<V>> auto diff1 = i2 - i;
            assert(diff1 == 2);

            const same_as<ranges::range_difference_t<V>> auto diff2 = i - i2;
            assert(diff2 == -2);

            // comparisons
            assert(i == i4);
            assert(i != i2);

            assert(i < i2);
            assert(i <= i2);
            assert(i2 > i);
            assert(i2 >= i);

            if constexpr (three_way_comparable<iterator_t<V>>) {
                assert(i <=> i4 == strong_ordering::equal);
                assert(i <=> i2 == strong_ordering::less);
                assert(i2 <=> i == strong_ordering::greater);
            }
        }

        if constexpr (!common_range<R>) {
            [[maybe_unused]] const sentinel_t<R> sentinel_defaulted;

            const auto i2   = r.begin();
            const auto sen  = r.end();
            const auto size = ranges::ssize(expected);

            assert(next(i2, size) == sen);
            assert(i2 != sen);

            if constexpr (sized_sentinel_for<sentinel_t<V>, iterator_t<V>>) {
                const same_as<ranges::range_difference_t<V>> auto diff1 = i2 - sen;
                assert(diff1 == -size);

                const same_as<ranges::range_difference_t<V>> auto diff2 = sen - i2;
                assert(diff2 == size);
            }
        }
    }

    if constexpr (CanMemberBegin<const R>) {
        [[maybe_unused]] const iterator_t<const R> const_defaulted;
        iterator_t<R> i = r.begin();
        iterator_t<const R> ci{i};

        assert(equal(*ci++, expected[0]));
        assert(equal(*++ci, expected[2]));

        if constexpr (bidirectional_range<const R>) {
            assert(equal(*ci--, expected[2]));
            assert(equal(*--ci, expected[0]));
        }

        if constexpr (random_access_range<const R>) {
            ci += 2;
            assert(equal(*ci, expected[2]));

            ci -= 2;
            assert(equal(*ci, expected[0]));

            assert(equal(ci[2], expected[2]));

            const same_as<iterator_t<const R>> auto ci2 = ci + 2;
            assert(equal(*ci2, expected[2]));

            const same_as<iterator_t<const R>> auto ci3 = 2 + ci;
            assert(equal(*ci3, expected[2]));

            const same_as<iterator_t<const R>> auto ci4 = ci3 - 2;
            assert(equal(*ci4, expected[0]));

            const same_as<ranges::range_difference_t<V>> auto diff1 = ci2 - ci;
            assert(diff1 == 2);

            const same_as<ranges::range_difference_t<V>> auto diff2 = ci - ci2;
            assert(diff2 == -2);

            // comparisons
            assert(ci == ci4);
            assert(ci != ci2);

            assert(ci < ci2);
            assert(ci <= ci2);
            assert(ci2 > ci);
            assert(ci2 >= ci);

            // cross comparisons
            assert(ci == i);
            assert(ci2 != i);

            assert(i < ci2);
            assert(i <= ci2);
            assert(ci2 > i);
            assert(ci2 >= i);

            if constexpr (three_way_comparable<iterator_t<const V>>) {
                assert(ci <=> ci4 == strong_ordering::equal);
                assert(ci <=> ci2 == strong_ordering::less);
                assert(ci2 <=> ci == strong_ordering::greater);
            }
        }

        if constexpr (!common_range<const R>) {
            [[maybe_unused]] const sentinel_t<const R> defaulted;

            static_assert(!equality_comparable_with<sentinel_t<const R>, iterator_t<const R>>);
        }
    }

    return true;
}

static constexpr int some_ints[] = {0, 1, 2, 3, 4, 5, 6, 7};
static constexpr span<const int> wrapped{some_ints};
static constexpr span<const int> slides_of_four[] = {
    wrapped.subspan(0, 4), wrapped.subspan(1, 4), wrapped.subspan(2, 4), wrapped.subspan(3, 4), wrapped.subspan(4, 4)};

struct instantiator {
    template <ranges::forward_range R>
    static constexpr void call() {
        R r{some_ints};
        test_one(r, slides_of_four);
    }
};

template <class Category, test::Common IsCommon, test::Sized IsSized>
using test_range = test::range<Category, const int, IsSized,
    test::CanDifference{derived_from<Category, random_access_iterator_tag> || IsSized == test::Sized::yes}, IsCommon,
    test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}>;

constexpr void instantiation_test() {
#ifdef TEST_EVERYTHING
    test_fwd<instantiator, const int>();
#else // ^^^ test all forward range permutations / test only "interesting" permutations vvv
    // The view is sensitive to category, commonality, and size, but oblivious to differencing and proxyness.
    using test::Common, test::Sized;

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

template <class Category, test::Common IsCommon, test::Sized IsSized,
    bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, IsSized, test::CanDifference{is_random}, IsCommon,
    test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

int main() {
    // Validate views
    { // ... copyable
        constexpr span<const int> s{some_ints};
        STATIC_ASSERT(test_one(s, slides_of_four));
        test_one(s, slides_of_four);
    }

    { // ... move-only
        using test::Common, test::Sized;
        test_one(move_only_view<forward_iterator_tag, Common::no, Sized::yes>{some_ints}, slides_of_four);
        test_one(move_only_view<forward_iterator_tag, Common::no, Sized::no>{some_ints}, slides_of_four);
        test_one(move_only_view<forward_iterator_tag, Common::yes, Sized::yes>{some_ints}, slides_of_four);
        test_one(move_only_view<forward_iterator_tag, Common::yes, Sized::no>{some_ints}, slides_of_four);
        test_one(move_only_view<bidirectional_iterator_tag, Common::no, Sized::yes>{some_ints}, slides_of_four);
        test_one(move_only_view<bidirectional_iterator_tag, Common::no, Sized::no>{some_ints}, slides_of_four);
        test_one(move_only_view<bidirectional_iterator_tag, Common::yes, Sized::yes>{some_ints}, slides_of_four);
        test_one(move_only_view<bidirectional_iterator_tag, Common::yes, Sized::no>{some_ints}, slides_of_four);
        test_one(move_only_view<random_access_iterator_tag, Common::no, Sized::yes>{some_ints}, slides_of_four);
        test_one(move_only_view<random_access_iterator_tag, Common::no, Sized::no>{some_ints}, slides_of_four);
        test_one(move_only_view<random_access_iterator_tag, Common::yes, Sized::yes>{some_ints}, slides_of_four);
        test_one(move_only_view<random_access_iterator_tag, Common::yes, Sized::no>{some_ints}, slides_of_four);
    }

    // Validate non-views
    {
        STATIC_ASSERT(test_one(some_ints, slides_of_four));
        test_one(some_ints, slides_of_four);
    }
    {
        vector vec(ranges::begin(some_ints), ranges::end(some_ints));
        test_one(vec, slides_of_four);
    }
    {
        forward_list lst(ranges::begin(some_ints), ranges::end(some_ints));
        test_one(lst, slides_of_four);
    }

    { // empty range
        STATIC_ASSERT(test_one(span<const int, 0>{}, span<span<const int>>{}));
        test_one(span<const int, 0>{}, span<span<const int>>{});
    }

    { // too short range
        STATIC_ASSERT(test_one(span<const int, 3>{some_ints, some_ints + 3}, span<span<const int>>{}));
        test_one(span<const int, 3>{some_ints, some_ints + 3}, span<span<const int>>{});
    }

    STATIC_ASSERT((instantiation_test(), true));
    instantiation_test();
}
