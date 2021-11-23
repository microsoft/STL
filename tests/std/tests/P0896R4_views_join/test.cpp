// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <forward_list>
#include <list>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>
using namespace std;

template <class Rng>
concept CanViewJoin = requires(Rng&& r) {
    views::join(forward<Rng>(r));
};

template <ranges::input_range Outer, ranges::random_access_range Expected>
constexpr bool test_one(Outer&& rng, Expected&& expected) {
    using ranges::join_view, ranges::begin, ranges::end, ranges::next, ranges::prev, ranges::input_range,
        ranges::forward_range, ranges::bidirectional_range, ranges::common_range, ranges::borrowed_range,
        ranges::iterator_t, ranges::range_value_t, ranges::range_reference_t;

    using Inner                     = range_value_t<Outer>;
    constexpr bool deref_is_glvalue = is_reference_v<range_reference_t<Outer>>;

    constexpr bool can_test = ranges::viewable_range<Outer> && input_range<range_reference_t<Outer>>;

    if constexpr (can_test) {
        constexpr bool is_view = ranges::view<remove_cvref_t<Outer>>;

        // Validate range adapter object
        // ...with lvalue argument
        static_assert(CanViewJoin<Outer&> == (!is_view || copy_constructible<remove_cvref_t<Outer>>) );
        if constexpr (CanViewJoin<Outer&>) {
            using JV                   = join_view<views::all_t<Outer&>>;
            constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<remove_cvref_t<Outer>>;

            static_assert(same_as<decltype(views::join(rng)), JV>);
            static_assert(noexcept(views::join(rng)) == is_noexcept);

            static_assert(same_as<decltype(rng | views::join), JV>);
            static_assert(noexcept(rng | views::join) == is_noexcept);
        }

        // ... with const lvalue argument
        static_assert(
            CanViewJoin<const remove_reference_t<Outer>&> == (!is_view || copy_constructible<remove_cvref_t<Outer>>) );
        if constexpr (is_view && copy_constructible<remove_cvref_t<Outer>>) {
            using JV                   = join_view<views::all_t<const remove_reference_t<Outer>&>>;
            constexpr bool is_noexcept = is_nothrow_copy_constructible_v<remove_cvref_t<Outer>>;

            static_assert(same_as<decltype(views::join(as_const(rng))), JV>);
            static_assert(noexcept(views::join(as_const(rng))) == is_noexcept);

            static_assert(same_as<decltype(as_const(rng) | views::join), JV>);
            static_assert(noexcept(as_const(rng) | views::join) == is_noexcept);
        } else if constexpr (!is_view) {
            using JV                   = join_view<ranges::ref_view<const remove_reference_t<Outer>>>;
            constexpr bool is_noexcept = true;

            static_assert(same_as<decltype(views::join(as_const(rng))), JV>);
            static_assert(noexcept(views::join(as_const(rng))) == is_noexcept);

            static_assert(same_as<decltype(as_const(rng) | views::join), JV>);
            static_assert(noexcept(as_const(rng) | views::join) == is_noexcept);
        }

        // ... with rvalue argument
        static_assert(CanViewJoin<remove_reference_t<Outer>> == (is_view || movable<remove_reference_t<Outer>>) );
        if constexpr (is_view) {
            using JV                   = join_view<views::all_t<remove_reference_t<Outer>>>;
            constexpr bool is_noexcept = is_nothrow_move_constructible_v<remove_reference_t<Outer>>;

            static_assert(same_as<decltype(views::join(move(rng))), JV>);
            static_assert(noexcept(views::join(move(rng))) == is_noexcept);

            static_assert(same_as<decltype(move(rng) | views::join), JV>);
            static_assert(noexcept(move(rng) | views::join) == is_noexcept);
        } else if constexpr (movable<remove_reference_t<Outer>>) {
            using JV                   = join_view<ranges::owning_view<remove_reference_t<Outer>>>;
            constexpr bool is_noexcept = is_nothrow_move_constructible_v<remove_reference_t<Outer>>;

            static_assert(same_as<decltype(views::join(move(rng))), JV>);
            static_assert(noexcept(views::join(move(rng))) == is_noexcept);

            static_assert(same_as<decltype(move(rng) | views::join), JV>);
            static_assert(noexcept(move(rng) | views::join) == is_noexcept);
        }

        // ... with const rvalue argument
        static_assert(
            CanViewJoin<const remove_reference_t<Outer>> == (is_view && copy_constructible<remove_cvref_t<Outer>>) );
        if constexpr (is_view && copy_constructible<remove_cvref_t<Outer>>) {
            using JV                   = join_view<remove_cvref_t<Outer>>;
            constexpr bool is_noexcept = is_nothrow_copy_constructible_v<remove_cvref_t<Outer>>;

            static_assert(same_as<decltype(views::join(move(as_const(rng)))), JV>);
            static_assert(noexcept(views::join(move(as_const(rng)))) == is_noexcept);

            static_assert(same_as<decltype(move(as_const(rng)) | views::join), JV>);
            static_assert(noexcept(move(as_const(rng)) | views::join) == is_noexcept);
        }

        using V = views::all_t<Outer>;
        using R = join_view<V>;
        static_assert(ranges::view<R>);
        static_assert(input_range<R> == input_range<Inner>);
        static_assert(forward_range<R> == (deref_is_glvalue && forward_range<Outer> && forward_range<Inner>) );
        static_assert(
            bidirectional_range<R> == //
            (deref_is_glvalue && bidirectional_range<Outer> && bidirectional_range<Inner> && common_range<Inner>) );
        static_assert(!ranges::random_access_range<R>);
        static_assert(!ranges::contiguous_range<R>);

        // Validate deduction guide
        same_as<R> auto r = join_view{forward<Outer>(rng)};
        assert(ranges::equal(r, expected));
        const bool is_empty = ranges::empty(expected);

        // Validate lack of size
        static_assert(!CanSize<R>);

        // Validate view_interface::empty and operator bool
        static_assert(CanEmpty<R> == forward_range<R>);
        static_assert(CanMemberEmpty<R> == CanEmpty<R>);
        if constexpr (CanMemberEmpty<R>) {
            assert(r.empty() == is_empty);
            assert(static_cast<bool>(r) == !is_empty);

            static_assert(CanEmpty<const R> == forward_range<const R>);
            static_assert(CanMemberEmpty<const R> == CanEmpty<const R>);
            if constexpr (CanMemberEmpty<const R>) {
                assert(as_const(r).empty() == is_empty);
                assert(static_cast<bool>(as_const(r)) == !is_empty);
            }
        }

        // Validate join_view::begin
        static_assert(CanMemberBegin<R>);
        static_assert(CanMemberBegin<const R> == (input_range<const V> && is_reference_v<range_reference_t<const V>>) );
        if (forward_range<R>) {
            const iterator_t<R> i = r.begin();
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

            static_assert(CanMemberBegin<const R> == CanBegin<const R&>);
            if constexpr (CanMemberBegin<const R>) {
                const iterator_t<const R> ci = as_const(r).begin();
                if (!is_empty) {
                    assert(*ci == *i);
                }

                if constexpr (copyable<V>) {
                    const auto r2                               = r;
                    const same_as<iterator_t<const R>> auto ci2 = r2.begin();
                    if (!is_empty) {
                        assert(*ci2 == *i);
                    }
                }
            }
        }

        // Validate join_view::end
        static_assert(CanMemberEnd<R>);
        static_assert(CanMemberEnd<const R> == (input_range<const V> && is_reference_v<range_reference_t<const V>>) );
        // clang-format off
        static_assert(common_range<R> == (forward_range<V> && is_reference_v<range_reference_t<V>> && common_range<V>
                                          && forward_range<Inner> && common_range<Inner>) );
        static_assert(common_range<const R> == (forward_range<const V> && is_reference_v<range_reference_t<const V>>
                                                && common_range<const V> && forward_range<range_reference_t<const V>>
                                                && common_range<range_reference_t<const V>>) );
        // clang-format on
        const ranges::sentinel_t<R> s = r.end();
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
                const ranges::sentinel_t<const R> cs = as_const(r).end();
                if constexpr (bidirectional_range<R> && common_range<R>) {
                    assert(*prev(cs) == *prev(end(expected)));

                    if constexpr (copyable<V>) {
                        const auto r2                   = r;
                        const ranges::sentinel_t<R> cs2 = r2.end();
                        assert(*prev(cs2) == *prev(end(expected)));
                    }
                }
            }
        }

        // Validate view_interface::data
        static_assert(!CanData<R>);
        static_assert(!CanData<const R>);

        // Validate view_interface::operator[]
        static_assert(!CanIndex<R>);
        static_assert(!CanIndex<const R>);

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
        // clang-format off
        static_assert(CanMemberBack<const R> == (bidirectional_range<const R> && common_range<const R>
                                                 && is_reference_v<range_reference_t<const V>>) );
        // clang-format on
        if (!is_empty) {
            if constexpr (CanMemberBack<R>) {
                assert(r.back() == *prev(end(expected)));
            }

            if constexpr (CanMemberBack<const R>) {
                assert(as_const(r).back() == *prev(end(expected)));
            }
        }

        // Validate join_view::base() const&
        static_assert(CanMemberBase<const R&> == copy_constructible<V>);
        if constexpr (copy_constructible<V> && forward_range<V>) {
            same_as<V> auto b1 = as_const(r).base();
            static_assert(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>);
            if (!is_empty) {
                auto bi1 = b1.begin();
                while (ranges::empty(*bi1)) {
                    ++bi1;
                }
                auto&& inner_first = *bi1;
                assert(*begin(inner_first) == *begin(expected));

                if constexpr (bidirectional_range<R> && common_range<R>) {
                    auto ei1 = prev(b1.end());
                    while (ranges::empty(*ei1)) {
                        --ei1;
                    }
                    auto&& inner_last = *ei1;
                    assert(*prev(end(inner_last)) == *prev(end(expected)));
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
                    while (ranges::empty(*bi2)) {
                        ++bi2;
                    }
                    auto&& inner_first = *bi2;
                    assert(*begin(inner_first) == *begin(expected));

                    if constexpr (bidirectional_range<R> && common_range<R>) {
                        auto ei2 = prev(b2.end());
                        while (ranges::empty(*ei2)) {
                            --ei2;
                        }
                        auto&& inner_last = *ei2;
                        assert(*prev(end(inner_last)) == *prev(end(expected)));
                    }
                }
            }
        }
    }
    return true;
}

constexpr int expected_ints[]         = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
constexpr span<const int> intervals[] = {
    {expected_ints + 0, expected_ints + 3},
    {expected_ints + 3, expected_ints + 7},
    {expected_ints + 7, expected_ints + 7},
    {expected_ints + 7, expected_ints + 10},
    {expected_ints + 10, expected_ints + 10},
};

struct instantiator {
    template <ranges::range Inner, ranges::input_range Outer>
    static constexpr void call() {
        static_assert(ranges::size(intervals) == 5);
        Inner inner_ranges[] = {
            Inner{intervals[0]}, Inner{intervals[1]}, Inner{intervals[2]}, Inner{intervals[3]}, Inner{intervals[4]}};
        test_one(Outer{inner_ranges}, expected_ints);
    }
};

enum class RefOrView { reference, view };

template <class Category, test::CanView IsView, test::Common IsCommon,
    bool is_random = derived_from<Category, random_access_iterator_tag>>
using inner_test_range = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
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
    // * inner and outer range common category (input, forward, bidi)
    // * outer range's reference type referenceness vs. value type viewness
    // * if the inner range models common_range
    // * if the outer range models common_range
    // * if both inner and outer iterators are equality_comparable (the defaults for input-non-common and forward
    // suffice to get coverage here)
    // * if the inner range has -> (Ditto defaults)
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

using mo_inner = test::range<input_iterator_tag, const int, test::Sized::no, test::CanDifference::no, test::Common::no,
    test::CanCompare::no, test::ProxyRef::yes, test::CanView::yes, test::Copyability::copyable>;

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, mo_inner, test::Sized{is_random}, test::CanDifference{is_random}, IsCommon,
    test::CanCompare{derived_from<Category, forward_iterator_tag>}, test::ProxyRef::no, test::CanView::yes,
    test::Copyability::move_only>;
void test_move_only_views() {
    const auto gen = [] {
        return array{mo_inner{intervals[0]}, mo_inner{intervals[1]}, mo_inner{intervals[2]}, mo_inner{intervals[3]}};
    };

    auto input = gen();
    test_one(move_only_view<input_iterator_tag, test::Common::no>{input}, expected_ints);

    input = gen();
    test_one(move_only_view<forward_iterator_tag, test::Common::no>{input}, expected_ints);

    input = gen();
    test_one(move_only_view<forward_iterator_tag, test::Common::yes>{input}, expected_ints);

    input = gen();
    test_one(move_only_view<bidirectional_iterator_tag, test::Common::no>{input}, expected_ints);

    input = gen();
    test_one(move_only_view<bidirectional_iterator_tag, test::Common::yes>{input}, expected_ints);
}

constexpr array<string_view, 5> prvalue_input = {{{}, "Hello "sv, {}, "World!"sv, {}}};

constexpr auto ToVector(const int val) {
    return vector{val + 1};
}

constexpr auto ToString(const size_t val) {
    return string{prvalue_input[val]};
}

struct Immovable {
    Immovable()                 = default;
    Immovable(const Immovable&) = delete;
    Immovable(Immovable&&)      = delete;
    Immovable& operator=(const Immovable&) = delete;
    Immovable& operator=(Immovable&&) = delete;
};

int main() {
    // Validate views
    constexpr string_view expected = "Hello World!"sv;

    { // ...copyable
        static constexpr array<string_view, 5> input = {{{}, "Hello "sv, {}, "World!"sv, {}}};
        constexpr span<const string_view, 5> sp{input};
        static_assert(test_one(sp, expected));
        test_one(sp, expected);
    }
    { // ...copyable rvalue
        static_assert(test_one(array<string_view, 5>{{{}, "Hello "sv, {}, "World!"sv, {}}}, expected));
        test_one(array<string_view, 5>{{{}, "Hello "sv, {}, "World!"sv, {}}}, expected);
    }
    // ... move-only
    test_move_only_views();

    // Validate non-views
    { // ... C array
        static constexpr int join_me[5][2] = {{0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}};
#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-934264
        static_assert(test_one(join_me, expected_ints));
#endif // TRANSITION, VSO-934264
        test_one(join_me, expected_ints);
    }
    { // ... fwd container
        forward_list<string_view> lst = {{}, "Hello "sv, {}, "World!"sv, {}};
        test_one(lst, expected);
    }
    { // ... bidi container
        list<string_view> lst = {{}, "Hello "sv, {}, "World!"sv, {}};
        test_one(lst, expected);
    }
    { // ... random container
        vector<string_view> lst = {{}, "Hello "sv, {}, "World!"sv, {}};
        test_one(lst, expected);
    }

    { // From example in LWG-3474
        vector<vector<vector<int>>> nested_vectors = {{{1, 2, 3}, {4, 5}, {6}}, {{7}, {8, 9}, {10, 11, 12}}, {{13}}};
        auto joined                                = nested_vectors | views::join | views::join;
        static constexpr int result[]              = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
        assert(ranges::equal(joined, result));
    }

    { // P2328 range of prvalue array
        static constexpr int result[] = {1, 2, 3, 4, 5};
        constexpr auto ToArray        = [](const int i) { return array<int, 1>{i + 1}; };
        assert(ranges::equal(views::iota(0, 5) | views::transform(ToArray) | views::join, result));
        static_assert(ranges::equal(views::iota(0, 5) | views::transform(ToArray) | views::join, result));
    }

    { // P2328 range of prvalue vector using global function
        static constexpr int result[] = {1, 2, 3, 4, 5};
        assert(ranges::equal(views::iota(0, 5) | views::transform(ToVector) | views::join, result));
#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-934264
        static_assert(ranges::equal(views::iota(0, 5) | views::transform(ToVector) | views::join, result));
#endif // not MSVC
    }

    { // P2328 range of prvalue vector using lambda
        static constexpr int result[] = {1, 2, 3, 4, 5};
        auto ToVectorLambda           = [](const int i) { return vector{i + 1}; };
        assert(ranges::equal(views::iota(0, 5) | views::transform(ToVectorLambda) | views::join, result));
#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-934264
        static_assert(ranges::equal(views::iota(0, 5) | views::transform(ToVectorLambda) | views::join, result));
#endif // not MSVC
    }

    { // P2328 range of prvalue string using global function
        assert(ranges::equal(views::iota(0u, 5u) | views::transform(ToString) | views::join, expected));
#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-934264
        static_assert(ranges::equal(views::iota(0u, 5u) | views::transform(ToString) | views::join, expected));
#endif // not MSVC
    }

    { // P2328 range of prvalue string using lambda
        auto ToStringLambda = [](const size_t i) { return string{prvalue_input[i]}; };
        assert(ranges::equal(views::iota(0u, 5u) | views::transform(ToStringLambda) | views::join, expected));
#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-934264
        static_assert(ranges::equal(views::iota(0u, 5u) | views::transform(ToStringLambda) | views::join, expected));
#endif // not MSVC
    }

    { // Immovable type
        constexpr auto ToArrayOfImmovable = [](int) { return array<Immovable, 3>{}; };
        assert(ranges::distance(views::iota(0, 2) | views::transform(ToArrayOfImmovable) | views::join) == 6);
        static_assert(ranges::distance(views::iota(0, 2) | views::transform(ToArrayOfImmovable) | views::join) == 6);
    }

    { // Joining a non-const view without const qualified begin and end methods
        vector<vector<int>> nested_vectors = {{0}, {1, 2, 3}, {99}, {4, 5, 6, 7}, {}, {8, 9, 10}};
        auto RemoveSmallVectors            = [](const vector<int>& inner_vector) { return inner_vector.size() > 2; };
        auto filtered_and_joined           = nested_vectors | views::filter(RemoveSmallVectors) | views::join;
        static constexpr int result[]      = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        assert(ranges::equal(filtered_and_joined, result));
    }

    STATIC_ASSERT(instantiation_test());
    instantiation_test();
}
