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
concept CanViewStride = requires(Rng&& r) { views::stride(forward<Rng>(r), 3); };

template <ranges::input_range Rng, class Expected>
constexpr bool test_one(Rng&& rng, Expected&& expected) {
    using ranges::forward_range, ranges::bidirectional_range, ranges::random_access_range, ranges::common_range,
        ranges::sized_range;
    using ranges::stride_view, ranges::begin, ranges::end, ranges::iterator_t, ranges::sentinel_t, ranges::prev;

    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;

    using V = views::all_t<Rng>;
    using R = stride_view<V>;

    STATIC_ASSERT(ranges::view<R>);
    STATIC_ASSERT(ranges::input_range<R>);
    STATIC_ASSERT(forward_range<R> == forward_range<Rng>);
    STATIC_ASSERT(bidirectional_range<R> == bidirectional_range<Rng>);
    STATIC_ASSERT(random_access_range<R> == random_access_range<Rng>);
    STATIC_ASSERT(!ranges::contiguous_range<R>);

    // Validate non-default-initializability
    STATIC_ASSERT(!is_default_constructible_v<R>);

    // Validate borrowed_range
    STATIC_ASSERT(ranges::borrowed_range<R> == ranges::borrowed_range<V>);

    // Validate range adaptor object and range adaptor closure
    constexpr auto closure = views::stride(3);

    // ... with lvalue argument
    STATIC_ASSERT(CanViewStride<Rng&> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewStride<Rng&>) {
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::stride(rng, 3)), R>);
        STATIC_ASSERT(noexcept(views::stride(rng, 3)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(rng | closure), R>);
        STATIC_ASSERT(noexcept(rng | closure) == is_noexcept);
    }

    // ... with const lvalue argument
    STATIC_ASSERT(CanViewStride<const remove_reference_t<Rng>&> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewStride<const remove_reference_t<Rng>&>) {
        using RC                   = stride_view<views::all_t<const remove_reference_t<Rng>&>>;
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::stride(as_const(rng), 3)), RC>);
        STATIC_ASSERT(noexcept(views::stride(as_const(rng), 3)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | closure), RC>);
        STATIC_ASSERT(noexcept(as_const(rng) | closure) == is_noexcept);
    }

    // ... with rvalue argument
    STATIC_ASSERT(CanViewStride<remove_reference_t<Rng>> == (is_view || movable<remove_reference_t<Rng>>) );
    if constexpr (CanViewStride<remove_reference_t<Rng>>) {
        using RS                   = stride_view<views::all_t<remove_reference_t<Rng>>>;
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::stride(std::move(rng), 3)), RS>);
        STATIC_ASSERT(noexcept(views::stride(std::move(rng), 3)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(std::move(rng) | closure), RS>);
        STATIC_ASSERT(noexcept(std::move(rng) | closure) == is_noexcept);
    }

    // ... with const rvalue argument
    STATIC_ASSERT(CanViewStride<const remove_reference_t<Rng>> == (is_view && copy_constructible<V>) );
    if constexpr (CanViewStride<const remove_reference_t<Rng>>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::stride(std::move(as_const(rng)), 3)), R>);
        STATIC_ASSERT(noexcept(views::stride(std::move(as_const(rng)), 3)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(std::move(as_const(rng)) | closure), R>);
        STATIC_ASSERT(noexcept(std::move(as_const(rng)) | closure) == is_noexcept);
    }

    // Validate deduction guide
    same_as<R> auto r = stride_view{std::forward<Rng>(rng), 3};

    // Validate stride_view::stride
    {
        const same_as<ranges::range_difference_t<V>> auto s = as_const(r).stride();
        assert(s == 3);
        STATIC_ASSERT(noexcept(as_const(r).stride()));
    }

    // Validate stride_view::size
    STATIC_ASSERT(CanMemberSize<R> == sized_range<V>);
    if constexpr (CanMemberSize<R>) {
        same_as<_Make_unsigned_like_t<ranges::range_difference_t<V>>> auto s = r.size();
        assert(s == ranges::size(expected));
        STATIC_ASSERT(noexcept(r.size()) == noexcept(ranges::distance(rng)));
    }

    // Validate stride_view::size (const)
    STATIC_ASSERT(CanMemberSize<const R> == sized_range<const V>);
    if constexpr (CanMemberSize<const R>) {
        same_as<_Make_unsigned_like_t<ranges::range_difference_t<const V>>> auto s = as_const(r).size();
        assert(s == ranges::size(expected));
        STATIC_ASSERT(noexcept(as_const(r).size()) == noexcept(ranges::distance(rng)));
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

    // Validate content
    assert(ranges::equal(r, expected));
    if (!forward_range<V>) { // intentionally not if constexpr
        return true;
    }

    // Validate stride_view::begin
    STATIC_ASSERT(CanMemberBegin<R>);
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

    // Validate stride_view::begin (const)
    STATIC_ASSERT(CanMemberBegin<const R> == ranges::range<const V>);
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

    // Validate stride_view::end
    STATIC_ASSERT(CanMemberEnd<R>);
    {
        const same_as<sentinel_t<R>> auto s = r.end();
        assert((r.begin() == s) == is_empty);
        STATIC_ASSERT(common_range<R> == (common_range<V> && (sized_range<V> || !bidirectional_range<V>) ));
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

        if constexpr (!common_range<R>) {
            STATIC_ASSERT(same_as<sentinel_t<R>, default_sentinel_t>);
        }
    }

    // Validate stride_view::end (const)
    STATIC_ASSERT(CanMemberEnd<const R> == ranges::range<const V>);
    if constexpr (CanMemberEnd<const R>) {
        const same_as<sentinel_t<const R>> auto cs = as_const(r).end();
        assert((as_const(r).begin() == cs) == is_empty);
        STATIC_ASSERT(common_range<const R> == //
                      (common_range<const V> && (sized_range<const V> || !bidirectional_range<const V>) ));
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

        if constexpr (!common_range<const R>) {
            STATIC_ASSERT(same_as<sentinel_t<const R>, default_sentinel_t>);
        }
    }

    // Validate view_interface::data
    STATIC_ASSERT(!CanData<R>);
    STATIC_ASSERT(!CanData<const R>);

    if (is_empty) {
        return true;
    }

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
    STATIC_ASSERT(CanMemberBack<R> == (bidirectional_range<V> && common_range<V> && sized_range<V>) );
    if constexpr (CanMemberBack<R>) {
        assert(r.back() == *prev(end(expected)));
    }

    // Validate view_interface::back (const)
    STATIC_ASSERT(
        CanMemberBack<const R> == (bidirectional_range<const V> && common_range<const V> && sized_range<const V>) );
    if constexpr (CanMemberBack<const R>) {
        assert(as_const(r).back() == *prev(end(expected)));
    }

    // Validate stride_view::iterator<not const>
    {
        // Check iterator_category
        if constexpr (forward_range<R>) {
            using IterCat = typename iterator_t<R>::iterator_category;
            using C       = typename iterator_traits<iterator_t<V>>::iterator_category;
            STATIC_ASSERT((derived_from<C, random_access_iterator_tag> && same_as<IterCat, random_access_iterator_tag>)
                          || same_as<IterCat, C>);
        }

        if constexpr (forward_range<R>) {
            [[maybe_unused]] const iterator_t<R> defaulted;
        }
        same_as<iterator_t<R>> auto i = r.begin();

        // Comparisons with std::default_sentinel_t
        assert(i != default_sentinel);
        if constexpr (forward_range<R>) {
            assert(ranges::next(i, r.end()) == default_sentinel);
        }

        // Equality operators
        if constexpr (equality_comparable<iterator_t<V>>) {
            assert(i == i);
            if constexpr (forward_range<R>) {
                assert(i != ranges::next(i, 1));
            }
        }

        if constexpr (forward_range<R>) {
            assert(*i++ == expected[0]);
        } else {
            STATIC_ASSERT(is_void_v<decltype(i++)>);
        }
        assert(*++i == expected[2]);

        if constexpr (bidirectional_range<R>) {
            assert(*i-- == expected[2]);
            assert(*--i == expected[0]);
        }

        if constexpr (random_access_range<R>) {
            i += 2;
            assert(*i == expected[2]);

            i -= 2;
            assert(*i == expected[0]);

            assert(i[2] == expected[2]);

            const auto i2 = i + 2;
            assert(*i2 == expected[2]);

            const auto i3 = 2 + i;
            assert(*i3 == expected[2]);

            const auto i4 = i3 - 2;
            assert(*i4 == expected[0]);

            const same_as<ranges::range_difference_t<V>> auto diff1 = i2 - i;
            assert(diff1 == 2);

            const same_as<ranges::range_difference_t<V>> auto diff2 = i - i2;
            assert(diff2 == -2);

            // Comparisons
            assert(i < i2);
            assert(i2 <= i3);
            assert(i3 > i4);
            assert(i4 >= i);
            if constexpr (three_way_comparable<iterator_t<V>>) {
                assert(i <=> i2 == strong_ordering::less);
                assert(i <=> i4 == strong_ordering::equal);
                assert(i2 <=> i == strong_ordering::greater);
            }
        }

        if constexpr (sized_sentinel_for<sentinel_t<V>, iterator_t<V>>) {
            const auto i2   = r.begin();
            const auto sen  = r.end();
            const auto size = ranges::ssize(expected);

            const same_as<ranges::range_difference_t<V>> auto diff3 = i2 - sen;
            assert(diff3 == -size);

            const same_as<ranges::range_difference_t<V>> auto diff4 = sen - i2;
            assert(diff4 == size);
        }

        [[maybe_unused]] same_as<const iterator_t<V>&> decltype(auto) base_ref = as_const(i).base();
        STATIC_ASSERT(noexcept(i.base()));
        [[maybe_unused]] same_as<iterator_t<V>> decltype(auto) base_ref2 = std::move(i).base();
    }

    // Validate stride_view::iterator<const>
    if constexpr (CanMemberBegin<const R>) {
        // Check iterator_category
        if constexpr (forward_range<const R>) {
            using IterCat = typename iterator_t<const R>::iterator_category;
            using C       = typename iterator_traits<iterator_t<const V>>::iterator_category;
            STATIC_ASSERT((derived_from<C, random_access_iterator_tag> && same_as<IterCat, random_access_iterator_tag>)
                          || same_as<IterCat, C>);
        }

        constexpr bool constructible_from_nonconst = convertible_to<iterator_t<V>, iterator_t<const V>> //
                                                  && convertible_to<sentinel_t<V>, sentinel_t<const V>>;

        if constexpr (forward_range<const R>) {
            [[maybe_unused]] const iterator_t<const R> const_defaulted;
        }
        auto i                               = r.begin();
        same_as<iterator_t<const R>> auto ci = as_const(r).begin();

        // Comparisons with std::default_sentinel_t
        assert(ci != default_sentinel);
        if constexpr (forward_range<const R>) {
            assert(ranges::next(ci, r.end()) == default_sentinel);
        }

        // Equality operators
        if constexpr (equality_comparable<iterator_t<const V>>) {
            assert(ci == ci);
            if constexpr (forward_range<const R>) {
                assert(ci != ranges::next(ci, 1));
            }

            if constexpr (constructible_from_nonconst) {
                assert(ci == i);
                if constexpr (forward_range<const R>) {
                    assert(ci != ranges::next(i, 1));
                }
            }
        }

        if constexpr (forward_range<const R>) {
            assert(*ci++ == expected[0]);
        } else {
            STATIC_ASSERT(is_void_v<decltype(i++)>);
        }
        assert(*++ci == expected[2]);

        if constexpr (bidirectional_range<const R>) {
            assert(*ci-- == expected[2]);
            assert(*--ci == expected[0]);
        }

        if constexpr (random_access_range<const R>) {
            ci += 2;
            assert(*ci == expected[2]);

            ci -= 2;
            assert(*ci == expected[0]);

            assert(ci[2] == expected[2]);

            const auto ci2 = ci + 2;
            assert(*ci2 == expected[2]);

            const auto ci3 = 2 + ci;
            assert(*ci3 == expected[2]);

            const auto ci4 = ci3 - 2;
            assert(*ci4 == expected[0]);

            const same_as<ranges::range_difference_t<V>> auto diff1 = ci2 - ci;
            assert(diff1 == 2);

            const same_as<ranges::range_difference_t<V>> auto diff2 = ci - ci2;
            assert(diff2 == -2);

            // Comparisons
            assert(ci < ci2);
            assert(ci <= ci2);
            assert(ci2 > ci);
            assert(ci2 >= ci);
            if constexpr (three_way_comparable<iterator_t<const V>>) {
                assert(ci <=> ci2 == strong_ordering::less);
                assert(ci <=> ci4 == strong_ordering::equal);
                assert(ci2 <=> ci == strong_ordering::greater);
            }

            // Cross comparisons
            if constexpr (constructible_from_nonconst) {
                assert(i < ci2);
                assert(i <= ci2);
                assert(ci2 > i);
                assert(ci2 >= i);
                if constexpr (three_way_comparable<iterator_t<const V>>) {
                    assert(i <=> ci2 == strong_ordering::less);
                    assert(i <=> ci4 == strong_ordering::equal);
                    assert(ci2 <=> i == strong_ordering::greater);
                }
            }
        }

        if constexpr (sized_sentinel_for<sentinel_t<const V>, iterator_t<const V>>) {
            const auto i2   = as_const(r).begin();
            const auto sen  = as_const(r).end();
            const auto size = ranges::ssize(expected);

            const same_as<ranges::range_difference_t<const V>> auto diff3 = i2 - sen;
            assert(diff3 == -size);

            const same_as<ranges::range_difference_t<const V>> auto diff4 = sen - i2;
            assert(diff4 == size);
        }

        [[maybe_unused]] same_as<const iterator_t<const V>&> decltype(auto) base_ref = as_const(ci).base();
        STATIC_ASSERT(noexcept(ci.base()));
        [[maybe_unused]] same_as<iterator_t<const V>> decltype(auto) base_ref2 = std::move(ci).base();
    }

    // Validate stride_view::base() const&
    STATIC_ASSERT(CanMemberBase<const R&> == copy_constructible<V>);
    if constexpr (copy_constructible<V>) {
        same_as<V> auto b1 = as_const(r).base();
        STATIC_ASSERT(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>);
        assert(*b1.begin() == *begin(expected));
    }

    // Validate stride_view::base() &&
    same_as<V> auto b2 = std::move(r).base();
    STATIC_ASSERT(noexcept(std::move(r).base()) == is_nothrow_move_constructible_v<V>);
    if (!is_empty) {
        assert(*b2.begin() == *begin(expected));
    }

    return true;
}

static constexpr int some_ints[]     = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
static constexpr int stride_result[] = {0, 3, 6, 9};

struct instantiator {
    template <ranges::input_range R>
    static constexpr void call() {
        R r{some_ints};
        test_one(r, stride_result);
    }
};

template <test::CanDifference Difference>
using test_input_range = test::range<input_iterator_tag, const int, test::Sized::yes, Difference, test::Common::yes,
    test::CanCompare::yes, test::ProxyRef::no>;

template <class Category, test::Common IsCommon, test::Sized IsSized>
using test_range =
    test::range<Category, const int, IsSized, test::CanDifference{derived_from<Category, random_access_iterator_tag>},
        IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
        test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}>;

constexpr void instantiation_test() {
#ifdef TEST_EVERYTHING
    test_in<instantiator, const int>();
#else // ^^^ test all input and forward range permutations / test only "interesting" permutations vvv
    using test::Common, test::Sized;

    // When the base range is an input range, the view is sensitive to differencing
    instantiator::call<test_input_range<test::CanDifference::yes>>();
    instantiator::call<test_input_range<test::CanDifference::no>>();

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
}

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare::yes, test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>},
    test::CanView::yes, test::Copyability::move_only>;

int main() {
    { // Validate views
        // ... copyable
        constexpr span<const int> s{some_ints};
        STATIC_ASSERT(test_one(s, stride_result));
        test_one(s, stride_result);
    }

    { // ... move-only
        test_one(move_only_view<input_iterator_tag, test::Common::no>{some_ints}, stride_result);
        test_one(move_only_view<input_iterator_tag, test::Common::yes>{some_ints}, stride_result);
        test_one(move_only_view<forward_iterator_tag, test::Common::no>{some_ints}, stride_result);
        test_one(move_only_view<forward_iterator_tag, test::Common::yes>{some_ints}, stride_result);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints}, stride_result);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints}, stride_result);
        test_one(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints}, stride_result);
        test_one(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints}, stride_result);
    }

    { // Validate non-views
        STATIC_ASSERT(test_one(some_ints, stride_result));
        test_one(some_ints, stride_result);
    }
    {
        vector vec(ranges::begin(some_ints), ranges::end(some_ints));
        test_one(vec, stride_result);
    }
    {
        forward_list lst(ranges::begin(some_ints), ranges::end(some_ints));
        test_one(lst, stride_result);
    }

    { // empty range
        using Span = span<const int>;
        STATIC_ASSERT(test_one(Span{}, Span{}));
        test_one(Span{}, Span{});
    }

    STATIC_ASSERT((instantiation_test(), true));
    instantiation_test();
}
