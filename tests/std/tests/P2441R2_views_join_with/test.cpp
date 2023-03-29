// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <ranges>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <range_algorithm_support.hpp>
using namespace std;

template <class Rng, class Delimiter>
concept CanViewJoinWith =
    requires(Rng&& r, Delimiter&& d) { views::join_with(forward<Rng>(r), forward<Delimiter>(d)); };

template <bool IsElement>
struct delimiter_view_impl {
    template <class Base, class>
    using apply = ranges::single_view<ranges::range_value_t<Base>>;
};
template <>
struct delimiter_view_impl<false> {
    template <class, class Delimiter>
    using apply = views::all_t<Delimiter>;
};
template <class Base, class Delimiter>
using delimiter_view_t =
    typename delimiter_view_impl<is_convertible_v<Delimiter, ranges::range_value_t<Base>>>::template apply<Base,
        Delimiter>;

template <ranges::input_range Outer, class Delimiter, ranges::forward_range Expected>
constexpr void test_one(Outer&& rng, Delimiter&& delimiter, Expected&& expected) {
    using ranges::join_with_view, ranges::begin, ranges::end, ranges::prev, ranges::input_range, ranges::forward_range,
        ranges::bidirectional_range, ranges::common_range, ranges::iterator_t, ranges::sentinel_t,
        ranges::range_value_t, ranges::range_reference_t;

    using Inner                     = range_value_t<Outer>;
    constexpr bool deref_is_glvalue = is_reference_v<range_reference_t<Outer>>;

    using V  = views::all_t<Outer>;
    using DV = delimiter_view_t<Inner, Delimiter>;
    using R  = join_with_view<V, DV>;

    // Validate type properties
    STATIC_ASSERT(ranges::view<R>);
    STATIC_ASSERT(input_range<R>);
    STATIC_ASSERT(forward_range<R> == (deref_is_glvalue && forward_range<Outer> && forward_range<Inner>) );
    STATIC_ASSERT(bidirectional_range<R>
                  == (deref_is_glvalue && bidirectional_range<Outer> && bidirectional_range<Inner>
                      && common_range<Inner> && bidirectional_range<DV> && common_range<DV>) );
    STATIC_ASSERT(!ranges::random_access_range<R>);

    // Validate iterator_category
    if constexpr (forward_range<R>) {
        using OuterIter   = iterator_t<Outer>;
        using InnerIter   = iterator_t<range_reference_t<Outer>>;
        using PatternIter = iterator_t<DV>;
        using OuterCat    = typename iterator_traits<OuterIter>::iterator_category;
        using InnerCat    = typename iterator_traits<InnerIter>::iterator_category;
        using PatternCat  = typename iterator_traits<PatternIter>::iterator_category;

        if constexpr (!is_reference_v<common_reference_t<iter_reference_t<InnerIter>, iter_reference_t<PatternIter>>>) {
            STATIC_ASSERT(same_as<typename iterator_t<R>::iterator_category, input_iterator_tag>);
        } else if constexpr (derived_from<OuterCat, bidirectional_iterator_tag>
                             && derived_from<InnerCat, bidirectional_iterator_tag>
                             && derived_from<PatternCat, bidirectional_iterator_tag>
                             && common_range<range_reference_t<Outer>> && common_range<DV>) {
            STATIC_ASSERT(same_as<typename iterator_t<R>::iterator_category, bidirectional_iterator_tag>);
        } else if constexpr (derived_from<OuterCat, forward_iterator_tag>
                             && derived_from<InnerCat, forward_iterator_tag>
                             && derived_from<PatternCat, forward_iterator_tag>) {
            STATIC_ASSERT(same_as<typename iterator_t<R>::iterator_category, forward_iterator_tag>);
        } else {
            STATIC_ASSERT(same_as<typename iterator_t<R>::iterator_category, input_iterator_tag>);
        }
    }

    if constexpr (forward_range<const R>) {
        using OuterIter   = iterator_t<const Outer>;
        using InnerIter   = iterator_t<range_reference_t<const Outer>>;
        using PatternIter = iterator_t<const DV>;
        using OuterCat    = typename iterator_traits<OuterIter>::iterator_category;
        using InnerCat    = typename iterator_traits<InnerIter>::iterator_category;
        using PatternCat  = typename iterator_traits<PatternIter>::iterator_category;

        if constexpr (!is_reference_v<common_reference_t<iter_reference_t<InnerIter>, iter_reference_t<PatternIter>>>) {
            STATIC_ASSERT(same_as<typename iterator_t<const R>::iterator_category, input_iterator_tag>);
        } else if constexpr (derived_from<OuterCat, bidirectional_iterator_tag>
                             && derived_from<InnerCat, bidirectional_iterator_tag>
                             && derived_from<PatternCat, bidirectional_iterator_tag>
                             && common_range<range_reference_t<const Outer>> && common_range<const DV>) {
            STATIC_ASSERT(same_as<typename iterator_t<const R>::iterator_category, bidirectional_iterator_tag>);
        } else if constexpr (derived_from<OuterCat, forward_iterator_tag>
                             && derived_from<InnerCat, forward_iterator_tag>
                             && derived_from<PatternCat, forward_iterator_tag>) {
            STATIC_ASSERT(same_as<typename iterator_t<const R>::iterator_category, forward_iterator_tag>);
        } else {
            STATIC_ASSERT(same_as<typename iterator_t<const R>::iterator_category, input_iterator_tag>);
        }
    }

    // Validate range adaptor object and range adaptor closure
    constexpr bool is_view = ranges::view<remove_cvref_t<Outer>>;
    const auto closure     = views::join_with(delimiter);

    // ... with lvalue argument
    STATIC_ASSERT(CanViewJoinWith<Outer&, Delimiter&> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewJoinWith<Outer&, Delimiter&>) {
        constexpr bool is_noexcept =
            (!is_view || is_nothrow_copy_constructible_v<V>) &&is_nothrow_copy_constructible_v<DV>;

        STATIC_ASSERT(same_as<decltype(views::join_with(rng, delimiter)), R>);
        STATIC_ASSERT(noexcept(views::join_with(rng, delimiter)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(rng | closure), R>);
        STATIC_ASSERT(noexcept(rng | closure) == is_noexcept);
    }

    // ... with const lvalue argument
    STATIC_ASSERT(
        CanViewJoinWith<const remove_reference_t<Outer>&, Delimiter&> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewJoinWith<const remove_reference_t<Outer>&, Delimiter&>) {
        using RC = join_with_view<views::all_t<const remove_reference_t<Outer>&>, DV>;
        constexpr bool is_noexcept =
            (!is_view || is_nothrow_copy_constructible_v<V>) &&is_nothrow_copy_constructible_v<DV>;

        STATIC_ASSERT(same_as<decltype(views::join_with(as_const(rng), delimiter)), RC>);
        STATIC_ASSERT(noexcept(views::join_with(as_const(rng), delimiter)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(rng) | closure), RC>);
        STATIC_ASSERT(noexcept(as_const(rng) | closure) == is_noexcept);
    }

    // ... with rvalue argument
    STATIC_ASSERT(
        CanViewJoinWith<remove_reference_t<Outer>, Delimiter&> == (is_view || movable<remove_reference<Outer>>) );
    if constexpr (CanViewJoinWith<remove_reference_t<Outer>, Delimiter&>) {
        using RS                   = join_with_view<views::all_t<remove_reference_t<Outer>>, DV>;
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V> && is_nothrow_copy_constructible_v<DV>;

        STATIC_ASSERT(same_as<decltype(views::join_with(move(rng), delimiter)), RS>);
        STATIC_ASSERT(noexcept(views::join_with(move(rng), delimiter)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(rng) | closure), RS>);
        STATIC_ASSERT(noexcept(move(rng) | closure) == is_noexcept);
    }

    // ... with const rvalue argument
    STATIC_ASSERT(CanViewJoinWith<const remove_reference_t<Outer>, Delimiter&> == (is_view && copy_constructible<V>) );
    if constexpr (CanViewJoinWith<const remove_reference_t<Outer>, Delimiter&>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V> && is_nothrow_copy_constructible_v<DV>;

        STATIC_ASSERT(same_as<decltype(views::join_with(move(as_const(rng)), delimiter)), R>);
        STATIC_ASSERT(noexcept(views::join_with(move(as_const(rng)), delimiter)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(rng)) | closure), R>);
        STATIC_ASSERT(noexcept(move(as_const(rng)) | closure) == is_noexcept);
    }

    // Validate deduction guide
    same_as<R> auto r = join_with_view{forward<Outer>(rng), forward<Delimiter>(delimiter)};
    assert(ranges::equal(r, expected));

    // Validate view_interface::empty and operator bool
    const bool is_empty = ranges::empty(expected);
    STATIC_ASSERT(CanEmpty<R> == forward_range<R>);
    STATIC_ASSERT(CanMemberEmpty<R> == CanEmpty<R>);
    STATIC_ASSERT(CanBool<R> == CanEmpty<R>);
    if constexpr (CanMemberEmpty<R>) {
        assert(r.empty() == is_empty);
        assert(static_cast<bool>(r) == !is_empty);

        STATIC_ASSERT(CanEmpty<const R> == forward_range<const R>);
        STATIC_ASSERT(CanMemberEmpty<const R> == CanEmpty<const R>);
        STATIC_ASSERT(CanBool<const R> == CanEmpty<const R>);
        if constexpr (CanMemberEmpty<const R>) {
            assert(as_const(r).empty() == is_empty);
            assert(static_cast<bool>(as_const(r)) == !is_empty);
        }
    }

    // Validate join_with_view::begin
    STATIC_ASSERT(CanMemberBegin<R>);
    STATIC_ASSERT(CanMemberBegin<const R&>
                  == (forward_range<const V> && forward_range<const DV> && is_reference_v<range_reference_t<const V>>
                      && input_range<range_reference_t<const V>>) );
    if (forward_range<R>) { // intentionally not if constexpr
        const auto i = r.begin();
        if (!is_empty) {
            assert(*i == *begin(expected));
        }

        if constexpr (copy_constructible<R>) {
            auto r2       = r;
            const auto i2 = r2.begin();
            if (!is_empty) {
                assert(*i2 == *i);
            }
        }

        static_assert(CanMemberBegin<const R> == CanBegin<const R&>);
        if constexpr (CanMemberBegin<const R>) {
            const same_as<iterator_t<const R>> auto ci = as_const(r).begin();
            if (!is_empty) {
                assert(*ci == *i);
            }

            if constexpr (copy_constructible<V>) {
                const auto r2                               = r;
                const same_as<iterator_t<const R>> auto ci2 = r2.begin();
                if (!is_empty) {
                    assert(*ci2 == *i);
                }
            }
        }
    }

    // Validate join_with_view::end
    static_assert(CanMemberEnd<R>);
    static_assert(CanMemberEnd<const R>
                  == (forward_range<const V> && forward_range<const DV> && is_reference_v<range_reference_t<const V>>
                      && input_range<range_reference_t<const V>>) );
    static_assert(common_range<R>
                  == (forward_range<V> && is_reference_v<range_reference_t<V>> && common_range<V>
                      && forward_range<Inner> && common_range<Inner>) );
    static_assert(common_range<const R>
                  == (forward_range<const V> && forward_range<const DV> && is_reference_v<range_reference_t<const V>>
                      && common_range<const V> && forward_range<range_reference_t<const V>>
                      && common_range<range_reference_t<const V>>) );
    const same_as<sentinel_t<R>> auto s = r.end();
    if (!is_empty) {
        if constexpr (bidirectional_range<R> && common_range<R>) {
            assert(*prev(s) == *prev(end(expected)));

            if constexpr (copyable<V>) {
                auto r2 = r;
                assert(*prev(r2.end()) == *prev(end(expected)));
            }
        }

        static_assert(CanMemberEnd<const R> == CanEnd<const R&>);
        if constexpr (CanMemberEnd<const R>) {
            const same_as<sentinel_t<const R>> auto cs = as_const(r).end();
            if constexpr (bidirectional_range<R> && common_range<R>) {
                assert(*prev(cs) == *prev(end(expected)));

                if constexpr (copyable<V>) {
                    const auto r2                               = r;
                    const same_as<sentinel_t<const R>> auto cs2 = r2.end();
                    assert(*prev(cs2) == *prev(end(expected)));
                }
            }
        }
    }

    // Validate view_interface::data
    STATIC_ASSERT(!CanData<R>);
    STATIC_ASSERT(!CanData<const R>);

    // Validate view_interface::size
    STATIC_ASSERT(!CanSize<R>);
    STATIC_ASSERT(!CanSize<const R>);

    // Validate view_interface::operator[]
    STATIC_ASSERT(!CanIndex<R>);
    STATIC_ASSERT(!CanIndex<const R>);

    // Validate view_interface::front and back
    static_assert(CanMemberFront<R> == forward_range<R>);
    static_assert(CanMemberFront<const R> == forward_range<const R>);
    if (!is_empty) {
        if constexpr (CanMemberFront<R>) {
            assert(r.front() == *begin(expected));
        }

        if constexpr (CanMemberFront<const R>) {
            assert(as_const(r).front() == *begin(expected));
        }
    }

    static_assert(CanMemberBack<R> == (bidirectional_range<R> && common_range<R>) );
    static_assert(CanMemberBack<const R> == (bidirectional_range<const R> && common_range<const R>) );
    if (!is_empty) {
        if constexpr (CanMemberBack<R>) {
            assert(r.back() == *prev(end(expected)));
        }

        if constexpr (CanMemberBack<const R>) {
            assert(as_const(r).back() == *prev(end(expected)));
        }
    }

    // Validate join_with_view::base() const&
    static_assert(CanMemberBase<const R&> == copy_constructible<V>);
    if constexpr (copy_constructible<V> && forward_range<V>) {
        same_as<V> auto b1 = as_const(r).base();
        static_assert(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>);
        if (!is_empty) {
            auto bi1 = b1.begin();
            if (!ranges::empty(*bi1)) {
                auto&& inner_first = *bi1;
                assert(*begin(inner_first) == *begin(expected));
            }
        }
    }

    // Validate join_view::base() && (NB: do this last since it leaves r moved-from)
    if (forward_range<V>) { // intentionally not if constexpr
        same_as<V> auto b2 = move(r).base();
        static_assert(noexcept(move(r).base()) == is_nothrow_move_constructible_v<V>);
        if constexpr (CanEmpty<Inner>) {
            if (!is_empty) {
                auto bi2 = b2.begin();
                if (!ranges::empty(*bi2)) {
                    auto&& inner_first = *bi2;
                    assert(*begin(inner_first) == *begin(expected));
                }
            }
        }
    }
}

constexpr string_view input[]         = {{}, "This"sv, "is"sv, {}, "a"sv, "test"sv, {}, {}};
constexpr string_view expected_single = "*This*is**a*test**";
constexpr string_view expected_range  = "*#This*#is*#*#a*#test*#*#";
constexpr string_view expected_empty  = "Thisisatest";

struct instantiator {
    template <ranges::range Inner, ranges::input_range Outer>
    static constexpr void call() {
        static_assert(ranges::size(input) == 8);

        { // Single-element delimiter
            Inner inner_ranges[] = {Inner{span{input[0]}}, Inner{span{input[1]}}, Inner{span{input[2]}},
                Inner{span{input[3]}}, Inner{span{input[4]}}, Inner{span{input[5]}}, Inner{span{input[6]}},
                Inner{span{input[7]}}};
            Outer r{inner_ranges};
            test_one(r, '*', expected_single);

            Outer empty{span<Inner, 0>{}};
            test_one(empty, '*', views::empty<char>);
        }
        { // Empty delimiter
            Inner inner_ranges[] = {Inner{span{input[0]}}, Inner{span{input[1]}}, Inner{span{input[2]}},
                Inner{span{input[3]}}, Inner{span{input[4]}}, Inner{span{input[5]}}, Inner{span{input[6]}},
                Inner{span{input[7]}}};
            Outer r{inner_ranges};
            test_one(r, views::empty<char>, expected_empty);

            Outer empty{span<Inner, 0>{}};
            test_one(empty, views::empty<char>, views::empty<char>);
        }
        { // Range delimiter
            Inner inner_ranges[] = {Inner{span{input[0]}}, Inner{span{input[1]}}, Inner{span{input[2]}},
                Inner{span{input[3]}}, Inner{span{input[4]}}, Inner{span{input[5]}}, Inner{span{input[6]}},
                Inner{span{input[7]}}};
            Outer r{inner_ranges};
            test_one(r, "*#"sv, expected_range);

            Outer empty{span<Inner, 0>{}};
            test_one(empty, "*#"sv, views::empty<char>);
        }
#ifdef __clang__ // TRANSITION, LLVM-60293
        if constexpr (ranges::forward_range<Outer> || ranges::common_range<Outer>)
#endif // __clang__
        { // Range-of-rvalue delimiter
            Inner inner_ranges[] = {Inner{span{input[0]}}, Inner{span{input[1]}}, Inner{span{input[2]}},
                Inner{span{input[3]}}, Inner{span{input[4]}}, Inner{span{input[5]}}, Inner{span{input[6]}},
                Inner{span{input[7]}}};
            Outer r{inner_ranges};
            test_one(r | views::as_rvalue, "*#"sv | views::as_rvalue, expected_range);

            Outer empty{span<Inner, 0>{}};
            test_one(empty | views::as_rvalue, "*#"sv | views::as_rvalue, views::empty<char>);
        }
    }
};

enum class RefOrView { reference, view };

template <class Category, test::CanView IsView, test::Common IsCommon,
    bool is_random = derived_from<Category, random_access_iterator_tag>>
using inner_test_range = test::range<Category, const char, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
    test::ProxyRef::no, IsView, test::Copyability::copyable>;

template <class Category, class Element, RefOrView RV, test::Common IsCommon,
    bool is_random = derived_from<Category, random_access_iterator_tag>>
using outer_test_range = test::range<Category, Element, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
    (RV == RefOrView::view ? test::ProxyRef::prvalue : test::ProxyRef::no), test::CanView::yes,
    test::Copyability::copyable>;

constexpr bool instantiation_test() {
    // The adaptor is sensitive to:
    // * inner, outer, and pattern range common category (input, forward, bidi)
    // * outer range's reference type referenceness vs. value type viewness
    // * if the inner range models common_range
    // * if the outer range models common_range
    // * if the pattern range models common_range
    // * if both inner and outer iterators are equality_comparable (the defaults for input-non-common and forward
    // suffice to get coverage here)
    using test::CanView, test::Common;

    instantiator::call<inner_test_range<input_iterator_tag, CanView::no, Common::no>,
        outer_test_range<input_iterator_tag, inner_test_range<input_iterator_tag, CanView::no, Common::no>,
            RefOrView::reference, Common::no>>();
    instantiator::call<inner_test_range<input_iterator_tag, CanView::no, Common::no>,
        outer_test_range<input_iterator_tag, inner_test_range<input_iterator_tag, CanView::no, Common::no>,
            RefOrView::reference, Common::yes>>();
    instantiator::call<inner_test_range<input_iterator_tag, CanView::no, Common::yes>,
        outer_test_range<input_iterator_tag, inner_test_range<input_iterator_tag, CanView::no, Common::yes>,
            RefOrView::reference, Common::no>>();
    instantiator::call<inner_test_range<input_iterator_tag, CanView::no, Common::yes>,
        outer_test_range<input_iterator_tag, inner_test_range<input_iterator_tag, CanView::no, Common::yes>,
            RefOrView::reference, Common::yes>>();
    instantiator::call<inner_test_range<input_iterator_tag, CanView::yes, Common::no>,
        outer_test_range<input_iterator_tag, inner_test_range<input_iterator_tag, CanView::yes, Common::no>,
            RefOrView::view, Common::no>>();
    instantiator::call<inner_test_range<input_iterator_tag, CanView::yes, Common::no>,
        outer_test_range<input_iterator_tag, inner_test_range<input_iterator_tag, CanView::yes, Common::no>,
            RefOrView::view, Common::yes>>();
    instantiator::call<inner_test_range<input_iterator_tag, CanView::yes, Common::yes>,
        outer_test_range<input_iterator_tag, inner_test_range<input_iterator_tag, CanView::yes, Common::yes>,
            RefOrView::view, Common::no>>();
    instantiator::call<inner_test_range<input_iterator_tag, CanView::yes, Common::yes>,
        outer_test_range<input_iterator_tag, inner_test_range<input_iterator_tag, CanView::yes, Common::yes>,
            RefOrView::view, Common::yes>>();
    instantiator::call<inner_test_range<forward_iterator_tag, CanView::no, Common::no>,
        outer_test_range<forward_iterator_tag, inner_test_range<forward_iterator_tag, CanView::no, Common::no>,
            RefOrView::reference, Common::no>>();
    instantiator::call<inner_test_range<forward_iterator_tag, CanView::no, Common::no>,
        outer_test_range<forward_iterator_tag, inner_test_range<forward_iterator_tag, CanView::no, Common::no>,
            RefOrView::reference, Common::yes>>();
    instantiator::call<inner_test_range<forward_iterator_tag, CanView::no, Common::yes>,
        outer_test_range<forward_iterator_tag, inner_test_range<forward_iterator_tag, CanView::no, Common::yes>,
            RefOrView::reference, Common::no>>();
    instantiator::call<inner_test_range<forward_iterator_tag, CanView::no, Common::yes>,
        outer_test_range<forward_iterator_tag, inner_test_range<forward_iterator_tag, CanView::no, Common::yes>,
            RefOrView::reference, Common::yes>>();
    instantiator::call<inner_test_range<forward_iterator_tag, CanView::yes, Common::no>,
        outer_test_range<forward_iterator_tag, inner_test_range<forward_iterator_tag, CanView::yes, Common::no>,
            RefOrView::view, Common::no>>();
    instantiator::call<inner_test_range<forward_iterator_tag, CanView::yes, Common::no>,
        outer_test_range<forward_iterator_tag, inner_test_range<forward_iterator_tag, CanView::yes, Common::no>,
            RefOrView::view, Common::yes>>();
    instantiator::call<inner_test_range<forward_iterator_tag, CanView::yes, Common::yes>,
        outer_test_range<forward_iterator_tag, inner_test_range<forward_iterator_tag, CanView::yes, Common::yes>,
            RefOrView::view, Common::no>>();
    instantiator::call<inner_test_range<forward_iterator_tag, CanView::yes, Common::yes>,
        outer_test_range<forward_iterator_tag, inner_test_range<forward_iterator_tag, CanView::yes, Common::yes>,
            RefOrView::view, Common::yes>>();
    instantiator::call<inner_test_range<bidirectional_iterator_tag, CanView::no, Common::no>,
        outer_test_range<bidirectional_iterator_tag,
            inner_test_range<bidirectional_iterator_tag, CanView::no, Common::no>, RefOrView::reference, Common::no>>();
    instantiator::call<inner_test_range<bidirectional_iterator_tag, CanView::no, Common::no>,
        outer_test_range<bidirectional_iterator_tag,
            inner_test_range<bidirectional_iterator_tag, CanView::no, Common::no>, RefOrView::reference,
            Common::yes>>();
    instantiator::call<inner_test_range<bidirectional_iterator_tag, CanView::no, Common::yes>,
        outer_test_range<bidirectional_iterator_tag,
            inner_test_range<bidirectional_iterator_tag, CanView::no, Common::yes>, RefOrView::reference,
            Common::no>>();
    instantiator::call<inner_test_range<bidirectional_iterator_tag, CanView::no, Common::yes>,
        outer_test_range<bidirectional_iterator_tag,
            inner_test_range<bidirectional_iterator_tag, CanView::no, Common::yes>, RefOrView::reference,
            Common::yes>>();
    instantiator::call<inner_test_range<bidirectional_iterator_tag, CanView::yes, Common::no>,
        outer_test_range<bidirectional_iterator_tag,
            inner_test_range<bidirectional_iterator_tag, CanView::yes, Common::no>, RefOrView::view, Common::no>>();
    instantiator::call<inner_test_range<bidirectional_iterator_tag, CanView::yes, Common::no>,
        outer_test_range<bidirectional_iterator_tag,
            inner_test_range<bidirectional_iterator_tag, CanView::yes, Common::no>, RefOrView::view, Common::yes>>();
    instantiator::call<inner_test_range<bidirectional_iterator_tag, CanView::yes, Common::yes>,
        outer_test_range<bidirectional_iterator_tag,
            inner_test_range<bidirectional_iterator_tag, CanView::yes, Common::yes>, RefOrView::view, Common::no>>();
    instantiator::call<inner_test_range<bidirectional_iterator_tag, CanView::yes, Common::yes>,
        outer_test_range<bidirectional_iterator_tag,
            inner_test_range<bidirectional_iterator_tag, CanView::yes, Common::yes>, RefOrView::view, Common::yes>>();

    return true;
}

struct throwing_iterator {
    struct tag {};

    throwing_iterator() = default;
    throwing_iterator(int j) : i(j) {}
    throwing_iterator(const throwing_iterator& other) : i(other.i) {
        if (i == 1) {
            throw tag{};
        }
    }
    throwing_iterator& operator=(const throwing_iterator&) = default;

    using difference_type = ptrdiff_t;
    using value_type      = int;

    throwing_iterator& operator++() {
        ++i;
        return *this;
    }
    throwing_iterator operator++(int) {
        return throwing_iterator{i++};
    }
    throwing_iterator& operator--() {
        --i;
        return *this;
    }
    throwing_iterator operator--(int) {
        return throwing_iterator{i--};
    }
    int operator*() const {
        return i;
    }
    bool operator==(const throwing_iterator& other) const {
        return i == other.i;
    }
    int i;
};

void test_valueless_iterator() {
    auto r = vector{"0"sv, ""sv} | views::join_with(ranges::subrange{throwing_iterator{0}, throwing_iterator{2}});

    auto it = r.begin();
    ++it;
    ++it;
    assert(*it == 1);
    auto it2 = r.begin();
    try {
        it2 = it;
        assert(false);
    } catch (throwing_iterator::tag&) {
    }

    try {
        (void) *it2;
        assert(false);
    } catch (bad_variant_access&) {
    }
    try {
        (void) ++it2;
        assert(false);
    } catch (bad_variant_access&) {
    }
    try {
        (void) --it2;
        assert(false);
    } catch (bad_variant_access&) {
    }
    try {
        (void) ranges::iter_move(it2);
        assert(false);
    } catch (bad_variant_access&) {
    }
}

// GH-3014 "<ranges>: list-initialization is misused"
struct FakeStr {
    const char* begin() {
        return nullptr;
    }

    unreachable_sentinel_t end() {
        return {};
    }
};

void test_gh_3014() { // COMPILE-ONLY
    struct FwdRange {
        FakeStr* begin() {
            return nullptr;
        }

        test::init_list_not_constructible_iterator<FakeStr> begin() const {
            return nullptr;
        }

        FakeStr* end() {
            return nullptr;
        }

        test::init_list_not_constructible_sentinel<FakeStr> end() const {
            return nullptr;
        }
    };

    auto r                                           = FwdRange{} | views::join_with('-');
    [[maybe_unused]] decltype(as_const(r).begin()) i = r.begin(); // Check 'iterator(iterator<!Const> i)'
    [[maybe_unused]] decltype(as_const(r).end()) s   = r.end(); // Check 'sentinel(sentinel<!Const> s)'
}

constexpr bool test_lwg3698() {
    // LWG-3698 "regex_iterator and join_view don't work together very well"
    struct stashing_iterator {
        using difference_type = int;
        using value_type      = span<const int>;

        int x = 1;

        constexpr stashing_iterator& operator++() {
            ++x;
            return *this;
        }
        constexpr void operator++(int) {
            ++x;
        }
        constexpr value_type operator*() const {
            return {&x, &x + 1};
        }
        constexpr bool operator==(default_sentinel_t) const {
            return x > 3;
        }
    };

    auto r   = ranges::subrange{stashing_iterator{}, default_sentinel} | views::join_with(views::empty<int>);
    auto r2  = r;
    auto it  = r.begin();
    auto it2 = r2.begin();

    auto itcopy = it;
    it          = ++it2;
    assert(*itcopy == 1);

    constexpr int expected_ints[] = {1, 2, 3, 5, 7};
    span<const int> intervals[2]  = {{expected_ints + 0, expected_ints + 3}, {expected_ints + 3, expected_ints + 5}};

    struct intricate_range {
        span<const int>* p;

        constexpr stashing_iterator begin() {
            return {};
        }
        constexpr default_sentinel_t end() {
            return {};
        }
        constexpr const span<const int>* begin() const {
            return p;
        }
        constexpr const span<const int>* end() const {
            return p + 2;
        }
    };

    auto jwv = intricate_range{.p = intervals} | views::join_with(views::empty<int>);
    auto cit = as_const(jwv).begin();
    assert(*++cit == 2);
    assert(*--cit == 1);
    assert(ranges::equal(as_const(jwv), expected_ints));

    return true;
}

void test_lwg3700() { // COMPILE-ONLY
    // LWG-3700 "The const begin of the join_view family does not require InnerRng to be a range"
    auto r  = views::iota(0, 5) | views::filter([](auto) { return true; });
    auto j  = views::single(r) | views::join_with(-1);
    using J = decltype(j);
    STATIC_ASSERT(!CanMemberBegin<const J>);
    STATIC_ASSERT(!CanMemberEnd<const J>);
}

int main() {
    {
        auto filtered_and_joined =
            vector<vector<int>>{} | views::filter([](auto) { return true; }) | views::join_with(0);
        assert(ranges::empty(filtered_and_joined));
    }

    STATIC_ASSERT(instantiation_test());
    instantiation_test();

    test_valueless_iterator();

    STATIC_ASSERT(test_lwg3698());
    assert(test_lwg3698());
}
