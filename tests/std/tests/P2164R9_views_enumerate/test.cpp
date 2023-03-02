// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
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

template <class Rng>
concept CanViewEnumerate = requires(Rng&& r) { views::enumerate(forward<Rng>(r)); };

struct ImmovableReference {
    ImmovableReference(ImmovableReference&&) = delete;
    operator int(); // not defined
};

struct IteratorWithImmovableReference {
    using value_type      = int;
    using difference_type = ptrdiff_t;

    ImmovableReference operator*() const; // not defined
    IteratorWithImmovableReference& operator++(); // not defined
    void operator++(int); // not defined
    bool operator==(default_sentinel_t) const; // not defined
};

STATIC_ASSERT(input_iterator<IteratorWithImmovableReference>);

using RangeWithImmovableReference = ranges::subrange<IteratorWithImmovableReference, default_sentinel_t>;
STATIC_ASSERT(ranges::input_range<RangeWithImmovableReference>);

STATIC_ASSERT(!CanViewEnumerate<RangeWithImmovableReference>);

template <ranges::input_range Rng, class Expected>
constexpr bool test_one(Rng&& rng, Expected&& expected) {
    using ranges::enumerate_view, ranges::forward_range, ranges::bidirectional_range, ranges::random_access_range,
        ranges::sized_range, ranges::common_range, ranges::iterator_t, ranges::sentinel_t, ranges::const_iterator_t,
        ranges::const_sentinel_t, ranges::range_difference_t, ranges::range_rvalue_reference_t, ranges::cbegin,
        ranges::cend, ranges::begin, ranges::end;

    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;

    using V = views::all_t<Rng>;
    using R = enumerate_view<V>;

    STATIC_ASSERT(ranges::view<R>);
    STATIC_ASSERT(ranges::input_range<R>);
    STATIC_ASSERT(forward_range<R> == forward_range<Rng>);
    STATIC_ASSERT(bidirectional_range<R> == bidirectional_range<Rng>);
    STATIC_ASSERT(random_access_range<R> == random_access_range<Rng>);
    STATIC_ASSERT(!ranges::contiguous_range<R>);

    // Validate default-initializability
    STATIC_ASSERT(default_initializable<R> == default_initializable<V>);

    // Validate borrowed_range
    STATIC_ASSERT(ranges::borrowed_range<R> == ranges::borrowed_range<V>);

    // Validate range closure object
    // ... with lvalue argument
    STATIC_ASSERT(CanViewEnumerate<Rng&> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewEnumerate<Rng&>) {
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::enumerate(rng)), R>);
        STATIC_ASSERT(noexcept(views::enumerate(rng)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(rng | views::enumerate), R>);
        STATIC_ASSERT(noexcept(rng | views::enumerate) == is_noexcept);
    }

    // ... with const lvalue argument
    STATIC_ASSERT(CanViewEnumerate<const remove_reference_t<Rng>&> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewEnumerate<const remove_reference_t<Rng>&>) {
        using RC                   = enumerate_view<views::all_t<const remove_reference_t<Rng>&>>;
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::enumerate(as_const(rng))), RC>);
        STATIC_ASSERT(noexcept(views::enumerate(as_const(rng))) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | views::enumerate), RC>);
        STATIC_ASSERT(noexcept(as_const(rng) | views::enumerate) == is_noexcept);
    }

    // ... with rvalue argument
    STATIC_ASSERT(CanViewEnumerate<remove_reference_t<Rng>> == (is_view || movable<remove_reference_t<Rng>>) );
    if constexpr (CanViewEnumerate<remove_reference_t<Rng>>) {
        using RS                   = enumerate_view<views::all_t<remove_reference_t<Rng>>>;
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::enumerate(move(rng))), RS>);
        STATIC_ASSERT(noexcept(views::enumerate(move(rng))) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | views::enumerate), RS>);
        STATIC_ASSERT(noexcept(move(rng) | views::enumerate) == is_noexcept);
    }

    // ... with const rvalue argument
    STATIC_ASSERT(CanViewEnumerate<const remove_reference_t<Rng>> == (is_view && copy_constructible<V>) );
    if constexpr (CanViewEnumerate<const remove_reference_t<Rng>>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        STATIC_ASSERT(same_as<decltype(views::enumerate(move(as_const(rng)))), R>);
        STATIC_ASSERT(noexcept(views::enumerate(move(as_const(rng)))) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | views::enumerate), R>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | views::enumerate) == is_noexcept);
    }

    // Validate deduction guide
    same_as<R> auto r = enumerate_view{forward<Rng>(rng)};

    // Validate enumerate_view::size
    STATIC_ASSERT(CanMemberSize<R> == sized_range<V>);
    if constexpr (CanMemberSize<R>) {
        same_as<ranges::range_size_t<V>> auto s = r.size();
        assert(_To_unsigned_like(s) == ranges::size(expected));
        STATIC_ASSERT(noexcept(r.size()) == noexcept(ranges::size(rng))); // strengthened
    }

    // Validate enumerate_view::size (const)
    STATIC_ASSERT(CanMemberSize<const R> == sized_range<const V>);
    if constexpr (CanMemberSize<const R>) {
        same_as<ranges::range_size_t<const V>> auto s = as_const(r).size();
        assert(_To_unsigned_like(s) == ranges::size(expected));
        STATIC_ASSERT(noexcept(as_const(r).size()) == noexcept(ranges::size(as_const(rng)))); // strengthened
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
    STATIC_ASSERT(CanMemberEmpty<const R> == (forward_range<const V> || sized_range<const V>) );
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

    // Validate enumerate_view::begin
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

    // Validate enumerate_view::begin (const)
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

    // Validate enumerate_view::end
    STATIC_ASSERT(CanMemberEnd<R>);
    {
        const same_as<sentinel_t<R>> auto s = r.end();
        assert((r.begin() == s) == is_empty);

        if constexpr (sentinel_for<sentinel_t<R>, iterator_t<const R>>) {
            assert((as_const(r).begin() == s) == is_empty);
        }

        STATIC_ASSERT(common_range<R> == (common_range<V> && sized_range<V>) );
        if constexpr (common_range<V> && sized_range<V> && bidirectional_range<V>) {
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

    // Validate enumerate_view::end (const)
    STATIC_ASSERT(CanMemberEnd<const R> == ranges::range<const V>);
    if constexpr (CanMemberEnd<const R>) {
        const same_as<sentinel_t<const R>> auto cs = as_const(r).end();
        assert((as_const(r).begin() == cs) == is_empty);

        if constexpr (sentinel_for<sentinel_t<const R>, iterator_t<R>>) {
            assert((r.begin() == cs) == is_empty);
        }

        STATIC_ASSERT(common_range<const R> == (common_range<const V> && sized_range<const V>) );
        if constexpr (common_range<const V> && sized_range<const V> && bidirectional_range<const V>) {
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

    // Validate view_interface::cbegin
    STATIC_ASSERT(CanMemberCBegin<R>);
    STATIC_ASSERT(CanMemberCBegin<const R&> == ranges::range<const V>);
    {
        const same_as<const_iterator_t<R>> auto i = r.cbegin();
        if (!is_empty) {
            assert(*i == *cbegin(expected));
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
    STATIC_ASSERT(CanMemberCEnd<R>);
    STATIC_ASSERT(CanMemberCEnd<const R&> == ranges::range<const V>);
    if (!is_empty) {
        same_as<const_sentinel_t<R>> auto i = r.cend();
        if constexpr (common_range<V> && sized_range<V> && bidirectional_range<V>) {
            assert(*prev(i) == *prev(cend(expected)));
        }

        if constexpr (CanCEnd<const R&>) {
            same_as<const_sentinel_t<const R>> auto i2 = as_const(r).cend();
            if constexpr (common_range<const V> && sized_range<const V> && bidirectional_range<const V>) {
                assert(*prev(i2) == *prev(cend(expected)));
            }
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

    { // Validate enumerate_view::iterator<not const>
        // Check iterator_category
        STATIC_ASSERT(same_as<typename iterator_t<R>::iterator_category, input_iterator_tag>);

        if constexpr (forward_range<R>) {
            [[maybe_unused]] const iterator_t<R> defaulted;
        }

        same_as<iterator_t<R>> auto i = r.begin();
        assert(i.index() == 0);

        { // Check comparisons
            assert(i == i);
            assert(!(i != i));
            assert(!(i < i));
            assert(i <= i);
            assert(!(i > i));
            assert(i >= i);
            assert(i <=> i == strong_ordering::equal);

            if constexpr (forward_range<R>) {
                auto i2 = ranges::next(i, 1);
                assert(!(i == i2));
                assert(i != i2);
                assert(i < i2);
                assert(i <= i2);
                assert(i2 > i);
                assert(i2 >= i);
                assert(i <=> i2 == strong_ordering::less);
                assert(i2 <=> i == strong_ordering::greater);
            }
        }

        if constexpr (forward_range<R>) {
            assert(*i++ == expected[0]);
        } else {
            STATIC_ASSERT(is_void_v<decltype(i++)>);
        }
        assert(*++i == expected[2]);
        assert(i.index() == 2);

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

            const same_as<range_difference_t<V>> auto diff1 = i2 - i;
            assert(diff1 == 2);

            const same_as<range_difference_t<V>> auto diff2 = i - i2;
            assert(diff2 == -2);
        }

        if constexpr (sized_sentinel_for<sentinel_t<V>, iterator_t<V>>) {
            const auto i2   = r.begin();
            const auto sen  = r.end();
            const auto size = ranges::ssize(expected);

            const same_as<range_difference_t<V>> auto diff3 = i2 - sen;
            assert(diff3 == -size);

            const same_as<range_difference_t<V>> auto diff4 = sen - i2;
            assert(diff4 == size);
        }

        if constexpr (sized_sentinel_for<sentinel_t<const V>, iterator_t<V>>) {
            const auto i2   = r.begin();
            const auto csen = as_const(r).end();
            const auto size = ranges::ssize(expected);

            const same_as<range_difference_t<V>> auto diff3 = i2 - csen;
            assert(diff3 == -size);

            const same_as<range_difference_t<V>> auto diff4 = csen - i2;
            assert(diff4 == size);
        }

        using IterMoveResult = tuple<range_difference_t<const Rng>, range_rvalue_reference_t<const Rng>>;
        [[maybe_unused]] same_as<IterMoveResult> decltype(auto) moved = ranges::iter_move(i);
        STATIC_ASSERT(noexcept(ranges::iter_move(i))
                      == (noexcept(ranges::iter_move(i.base()))
                          && is_nothrow_move_constructible_v<range_rvalue_reference_t<const Rng>>) );

        [[maybe_unused]] same_as<const iterator_t<V>&> decltype(auto) i_base = as_const(i).base();
        STATIC_ASSERT(noexcept(as_const(i).base()));

        [[maybe_unused]] same_as<iterator_t<V>> decltype(auto) i_base2 = move(i).base();
        STATIC_ASSERT(noexcept(move(i).base()) == is_nothrow_move_constructible_v<iterator_t<Rng>>); // strengthened
    }

    if constexpr (CanMemberBegin<const R>) { // Validate enumerate_view::iterator<const>
        // Check iterator_category
        STATIC_ASSERT(same_as<typename iterator_t<const R>::iterator_category, input_iterator_tag>);

        constexpr bool constructible_from_nonconst = convertible_to<iterator_t<V>, iterator_t<const V>> //
                                                  && convertible_to<sentinel_t<V>, sentinel_t<const V>>;
        if constexpr (forward_range<const R>) {
            [[maybe_unused]] const iterator_t<const R> const_defaulted;
        }

        same_as<iterator_t<R>> auto i        = r.begin();
        same_as<iterator_t<const R>> auto ci = as_const(r).begin();
        assert(ci.index() == 0);

        { // Check comparisons
            assert(ci == ci);
            assert(!(ci != ci));
            assert(!(ci < ci));
            assert(ci <= ci);
            assert(!(ci > ci));
            assert(ci >= ci);
            assert(ci <=> ci == strong_ordering::equal);

            if constexpr (forward_range<R>) {
                auto ci2 = ranges::next(ci, 1);
                assert(!(ci == ci2));
                assert(ci != ci2);
                assert(ci < ci2);
                assert(ci <= ci2);
                assert(ci2 > ci);
                assert(ci2 >= ci);
                assert(ci <=> ci2 == strong_ordering::less);
                assert(ci2 <=> ci == strong_ordering::greater);
            }

            if constexpr (constructible_from_nonconst) { // Check cross comparisons
                assert(ci == i);
                assert(!(ci < i));
                assert(ci <= i);
                assert(!(ci > i));
                assert(ci >= i);
                assert(ci <=> i == strong_ordering::equal);

                if constexpr (forward_range<const R>) {
                    auto i2 = ranges::next(i, 1);
                    assert(ci != i2);
                    assert(ci < i2);
                    assert(ci <= i2);
                    assert(i2 > ci);
                    assert(i2 >= ci);
                    assert(ci <=> i2 == strong_ordering::less);
                    assert(i2 <=> ci == strong_ordering::greater);
                }
            }
        }

        if constexpr (forward_range<const R>) {
            assert(*ci++ == expected[0]);
        } else {
            STATIC_ASSERT(is_void_v<decltype(ci++)>);
        }
        assert(*++ci == expected[2]);
        assert(ci.index() == 2);

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

            const same_as<range_difference_t<V>> auto diff1 = ci2 - ci;
            assert(diff1 == 2);

            const same_as<range_difference_t<V>> auto diff2 = ci - ci2;
            assert(diff2 == -2);
        }

        if constexpr (sized_sentinel_for<sentinel_t<V>, iterator_t<const V>>) {
            const auto ci2  = as_const(r).begin();
            const auto sen  = r.end();
            const auto size = ranges::ssize(expected);

            const same_as<range_difference_t<const V>> auto diff3 = ci2 - sen;
            assert(diff3 == -size);

            const same_as<range_difference_t<const V>> auto diff4 = sen - ci2;
            assert(diff4 == size);
        }

        if constexpr (sized_sentinel_for<sentinel_t<const V>, iterator_t<const V>>) {
            const auto ci2  = as_const(r).begin();
            const auto csen = as_const(r).end();
            const auto size = ranges::ssize(expected);

            const same_as<range_difference_t<const V>> auto diff3 = ci2 - csen;
            assert(diff3 == -size);

            const same_as<range_difference_t<const V>> auto diff4 = csen - ci2;
            assert(diff4 == size);
        }

        using IterMoveResult = tuple<range_difference_t<const Rng>, range_rvalue_reference_t<const Rng>>;
        [[maybe_unused]] same_as<IterMoveResult> decltype(auto) moved = ranges::iter_move(ci);
        STATIC_ASSERT(noexcept(ranges::iter_move(ci))
                      == (noexcept(ranges::iter_move(ci.base()))
                          && is_nothrow_move_constructible_v<range_rvalue_reference_t<const Rng>>) );

        [[maybe_unused]] same_as<const iterator_t<const V>&> decltype(auto) ci_base = as_const(ci).base();
        STATIC_ASSERT(noexcept(as_const(ci).base()));

        [[maybe_unused]] same_as<iterator_t<const V>> decltype(auto) ci_base2 = move(ci).base();
        STATIC_ASSERT(noexcept(move(ci).base()) == is_nothrow_move_constructible_v<iterator_t<Rng>>);
    }

    // Validate enumerate_view::base() const&
    STATIC_ASSERT(CanMemberBase<const R&> == copy_constructible<V>);
    if constexpr (copy_constructible<V>) {
        same_as<V> auto b1 = as_const(r).base();
        STATIC_ASSERT(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>); // strengthened
        assert(*b1.begin() == get<1>(*begin(expected)));
    }

    // Validate enumerate_view::base() &&
    [[maybe_unused]] same_as<V> auto b2 = move(r).base();
    STATIC_ASSERT(noexcept(move(r).base()) == is_nothrow_move_constructible_v<V>); // strengthened
    assert(*b2.begin() == get<1>(*begin(expected)));

    return true;
}

static constexpr int some_ints[]                          = {1, 22, 333, 4444, 55555, 666666};
static constexpr tuple<ptrdiff_t, int> enumerate_result[] = {
    {0, 1}, {1, 22}, {2, 333}, {3, 4444}, {4, 55555}, {5, 666666}};

struct instantiator {
    template <ranges::input_range R>
    static constexpr void call() {
        R r{some_ints};
        test_one(r, enumerate_result);
    }
};

template <test::CanDifference Difference>
using test_input_range = test::range<input_iterator_tag, const int, test::Sized::yes, Difference, test::Common::yes,
    test::CanCompare::yes, test::ProxyRef::yes>;

template <class Category, test::Common IsCommon, test::Sized IsSized>
using test_range =
    test::range<Category, const int, IsSized, test::CanDifference{derived_from<Category, random_access_iterator_tag>},
        IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
        test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}>;

constexpr void instantiation_test() {
#ifdef TEST_EVERYTHING
    test_in<instantiator, const int>();
#else // ^^^ test all input range permutations / test only "interesting" permutations vvv
    using test::Common, test::Sized;

    // When the base range is an input range, the view is sensitive to differencing
    instantiator::call<test_input_range<test::CanDifference::yes>>();
    instantiator::call<test_input_range<test::CanDifference::no>>();

    // The view is sensitive to category, commonality, and size, but oblivious to proxyness and differencing
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
        STATIC_ASSERT(test_one(s, enumerate_result));
        test_one(s, enumerate_result);
    }

    { // ... move-only
        test_one(move_only_view<input_iterator_tag, test::Common::no>{some_ints}, enumerate_result);
        test_one(move_only_view<input_iterator_tag, test::Common::yes>{some_ints}, enumerate_result);
        test_one(move_only_view<forward_iterator_tag, test::Common::no>{some_ints}, enumerate_result);
        test_one(move_only_view<forward_iterator_tag, test::Common::yes>{some_ints}, enumerate_result);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints}, enumerate_result);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints}, enumerate_result);
        test_one(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints}, enumerate_result);
        test_one(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints}, enumerate_result);
    }

    { // Validate non-views
        STATIC_ASSERT(test_one(some_ints, enumerate_result));
        test_one(some_ints, enumerate_result);

        auto vec = some_ints | ranges::to<vector>();
        test_one(vec, enumerate_result);

        auto lst = some_ints | ranges::to<forward_list>();
        test_one(lst, enumerate_result);
    }

    { // Validate empty range
        STATIC_ASSERT(test_one(span<const int>{}, span<tuple<ptrdiff_t, int>>{}));
        test_one(span<const int>{}, span<tuple<ptrdiff_t, int>>{});
    }

    STATIC_ASSERT((instantiation_test(), true));
    instantiation_test();
}
