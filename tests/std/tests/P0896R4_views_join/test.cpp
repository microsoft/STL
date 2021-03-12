// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <list>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;

template <class Rng>
concept CanViewJoin = requires(Rng&& r) {
    views::join(static_cast<Rng&&>(r));
};

template <ranges::input_range Outer, ranges::random_access_range Expected>
constexpr bool test_one(Outer&& rng, Expected&& expected) {
    using ranges::begin, ranges::bidirectional_range, ranges::common_range, ranges::enable_borrowed_range, ranges::end,
        ranges::forward_range, ranges::input_range, ranges::iterator_t, ranges::join_view, ranges::random_access_range,
        ranges::range_value_t, ranges::range_reference_t;

    using Inner                     = range_value_t<Outer>;
    constexpr bool deref_is_glvalue = is_reference_v<range_reference_t<Outer>>;

    // clang-format off
    constexpr bool can_test = ranges::viewable_range<Outer>
        && ranges::input_range<range_reference_t<Outer>>
        && (deref_is_glvalue || ranges::view<Inner>);
    // clang-format on

    if constexpr (can_test) {
        using V = views::all_t<Outer>;
        using R = join_view<V>;
        static_assert(ranges::view<R>);
        static_assert(input_range<R> == input_range<Inner>);
        static_assert(forward_range<R> == (forward_range<Outer> && forward_range<Inner>) );
        static_assert(bidirectional_range<R> == (bidirectional_range<Outer> && bidirectional_range<Inner>) );
        static_assert(!ranges::random_access_range<R>);
        static_assert(!ranges::contiguous_range<R>);

        constexpr bool is_view = ranges::view<Outer>;

        // Validate range adapter object
        // ...with lvalue argument
        static_assert(CanViewJoin<Outer&> == (!is_view || copyable<V>) );
        if constexpr (CanViewJoin<Outer&>) {
            constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

            static_assert(same_as<decltype(views::join(rng)), R>);
            static_assert(noexcept(views::join(rng)) == is_noexcept);

            static_assert(same_as<decltype(rng | views::join), R>);
            static_assert(noexcept(rng | views::join) == is_noexcept);
        }

        // ... with const lvalue argument
        static_assert(CanViewJoin<const remove_reference_t<Outer>&> == (!is_view || copyable<V>) );
        if constexpr (is_view && copyable<V>) {
            constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

            static_assert(same_as<decltype(views::join(as_const(rng))), R>);
            static_assert(noexcept(views::join(as_const(rng))) == is_noexcept);

            static_assert(same_as<decltype(as_const(rng) | views::join), R>);
            static_assert(noexcept(as_const(rng) | views::join) == is_noexcept);
        } else if constexpr (!is_view) {
            using RC                   = join_view<views::all_t<const remove_reference_t<Outer>&>>;
            constexpr bool is_noexcept = is_nothrow_constructible_v<RC, const remove_reference_t<Outer>&>;

            static_assert(same_as<decltype(views::join(as_const(rng))), RC>);
            static_assert(noexcept(views::join(as_const(rng))) == is_noexcept);

            static_assert(same_as<decltype(as_const(rng) | views::join), RC>);
            static_assert(noexcept(as_const(rng) | views::join) == is_noexcept);
        }

        // ... with rvalue argument
        static_assert(
            CanViewJoin<remove_reference_t<Outer>> == is_view || enable_borrowed_range<remove_cvref_t<Outer>>);
        if constexpr (is_view) {
            constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;
            static_assert(same_as<decltype(views::join(move(rng))), R>);
            static_assert(noexcept(views::join(move(rng))) == is_noexcept);

            static_assert(same_as<decltype(move(rng) | views::join), R>);
            static_assert(noexcept(move(rng) | views::join) == is_noexcept);
        } else if constexpr (enable_borrowed_range<Outer>) {
            using S                    = decltype(ranges::subrange{move(rng)});
            using RS                   = join_view<S>;
            constexpr bool is_noexcept = noexcept(S{move(rng)});

            static_assert(same_as<decltype(views::join(move(rng))), RS>);
            static_assert(noexcept(views::join(move(rng))) == is_noexcept);

            static_assert(same_as<decltype(move(rng) | views::join), RS>);
            static_assert(noexcept(move(rng) | views::join) == is_noexcept);
        }

        // ... with const rvalue argument
        static_assert(CanViewJoin<const remove_reference_t<Outer>> == (is_view && copyable<V>)
                      || (!is_view && enable_borrowed_range<remove_cvref_t<Outer>>) );
        if constexpr (is_view && copyable<V>) {
            constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

            static_assert(same_as<decltype(views::join(move(as_const(rng)))), R>);
            static_assert(noexcept(views::join(move(as_const(rng)))) == is_noexcept);

            static_assert(same_as<decltype(move(as_const(rng)) | views::join), R>);
            static_assert(noexcept(move(as_const(rng)) | views::join) == is_noexcept);
        } else if constexpr (!is_view && enable_borrowed_range<const remove_cvref_t<Outer>>) {
            using S                    = decltype(ranges::subrange{as_const(rng)});
            using RS                   = join_view<S>;
            constexpr bool is_noexcept = noexcept(S{as_const(rng)});

            static_assert(same_as<decltype(views::join(move(as_const(rng)))), RS>);
            static_assert(noexcept(views::join(move(as_const(rng)))) == is_noexcept);

            static_assert(same_as<decltype(move(as_const(rng)) | views::join), RS>);
            static_assert(noexcept(move(as_const(rng)) | views::join) == is_noexcept);
        }

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
        if constexpr (forward_range<R>) {
            const iterator_t<R> i = r.begin();
            assert(r.begin() == i);
            if (!is_empty) {
                assert(*i == *begin(expected));
            }

            if constexpr (copyable<V>) {
                auto r2                              = r;
                const same_as<iterator_t<R>> auto i2 = r2.begin();
                assert(r2.begin() == i2);
                if (!is_empty) {
                    assert(*i2 == *i);
                }
            }

            if constexpr (CanMemberBegin<const R> && !CanBegin<const R>) {
                static_assert(input_iterator<decltype(std::declval<const R>().begin())>);
                static_assert(
                    sentinel_for<decltype(std::declval<const R>().end()), decltype(std::declval<const R>().begin())>);
            }

            if constexpr (CanBegin<const R> && forward_range<R>) {
                const iterator_t<const R> ci = as_const(r).begin();
                assert(as_const(r).begin() == ci);
                if (!is_empty) {
                    assert(*ci == *i);
                }

                if constexpr (copyable<V>) {
                    const auto r2                               = r;
                    const same_as<iterator_t<const V>> auto ci2 = r2.begin();
                    assert(r2.begin() == ci2);
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
        if (!is_empty) {
            const ranges::sentinel_t<R> s = r.end();
            if constexpr (bidirectional_range<R> && common_range<R>) {
                assert(*prev(s) == *prev(end(expected)));
            }

            if constexpr (bidirectional_range<R> && common_range<V> && copyable<V>) {
                auto r2 = r;
                assert(*prev(r2.end()) == *prev(end(expected)));
            }

            if constexpr (CanMemberEnd<const R>) {
                const ranges::sentinel_t<const R> cs = as_const(r).end();
                if constexpr (bidirectional_range<R>) {
                    assert(*prev(cs) == *prev(end(expected)));
                }

                if constexpr (copyable<V>) {
                    const auto r2                   = r;
                    const ranges::sentinel_t<R> cs2 = r2.end();
                    if constexpr (bidirectional_range<R>) {
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
                assert(*ranges::begin(inner_first) == *begin(expected));

                if constexpr (bidirectional_range<V> && common_range<V>) {
                    auto ei1 = prev(b1.end());
                    while (ranges::empty(*ei1)) {
                        --ei1;
                    }
                    auto&& inner_last = *ei1;
                    assert(*prev(ranges::end(inner_last)) == *prev(end(expected)));
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
                    assert(*ranges::begin(inner_first) == *begin(expected));

                    if constexpr (bidirectional_range<V> && common_range<V>) {
                        auto ei2 = prev(b2.end());
                        while (ranges::empty(*ei2)) {
                            --ei2;
                        }
                        auto&& inner_last = *ei2;
                        assert(*prev(ranges::end(inner_last)) == *prev(end(expected)));
                    }
                }
            }
        }
    }
    return true;
}

template <class Continuation>
struct with_dependent_input_ranges {
    template <class Inner>
    static constexpr void call() {
        using namespace test;
        using test::range;

        // For all ranges, IsCommon implies Eq.
        // For single-pass ranges, Eq is uninteresting without IsCommon (there's only one valid iterator
        // value at a time, and no reason to compare it with itself for equality).
        Continuation::template call<Inner,
            range<input, Inner, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::no>>();
        Continuation::template call<Inner,
            range<input, Inner, Sized::no, CanDifference::no, Common::no, CanCompare::no, ProxyRef::yes>>();
        Continuation::template call<Inner,
            range<input, Inner, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Inner,
            range<input, Inner, Sized::no, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::yes>>();

        Continuation::template call<Inner,
            range<input, Inner, Sized::no, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::no>>();
        Continuation::template call<Inner,
            range<input, Inner, Sized::no, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::yes>>();
        Continuation::template call<Inner,
            range<input, Inner, Sized::no, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Inner,
            range<input, Inner, Sized::no, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::yes>>();

        Continuation::template call<Inner,
            range<input, Inner, Sized::yes, CanDifference::no, Common::no, CanCompare::no, ProxyRef::no>>();
        Continuation::template call<Inner,
            range<input, Inner, Sized::yes, CanDifference::no, Common::no, CanCompare::no, ProxyRef::yes>>();
        Continuation::template call<Inner,
            range<input, Inner, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Inner,
            range<input, Inner, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes, ProxyRef::yes>>();

        Continuation::template call<Inner,
            range<input, Inner, Sized::yes, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::no>>();
        Continuation::template call<Inner,
            range<input, Inner, Sized::yes, CanDifference::yes, Common::no, CanCompare::no, ProxyRef::yes>>();
        Continuation::template call<Inner,
            range<input, Inner, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::no>>();
        Continuation::template call<Inner,
            range<input, Inner, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes, ProxyRef::yes>>();

        with_forward_ranges<Continuation, Inner>::template call<Inner>();
    }
};

template <class Instantiator, class Element>
constexpr void test_nested_inout() {
    with_input_or_output_ranges<with_dependent_input_ranges<Instantiator>, Element>::call();
}

constexpr int expected_ints[]         = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
constexpr span<const int> intervals[] = {
    {expected_ints + 0, expected_ints + 3},
    {expected_ints + 3, expected_ints + 7},
    {expected_ints + 7, expected_ints + 7},
    {expected_ints + 7, expected_ints + 10},
};

struct instantiator {
    template <ranges::range Inner, ranges::input_range Outer>
    static constexpr void call() {
        static_assert(ranges::size(intervals) == 4);
        array inner_ranges = {Inner{intervals[0]}, Inner{intervals[1]}, Inner{intervals[2]}, Inner{intervals[3]}};
        test_one(Outer{inner_ranges}, expected_ints);
    }
};

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

    auto data = gen();
    test_one(move_only_view<input_iterator_tag, test::Common::no>{data}, expected_ints);

    data = gen();
    test_one(move_only_view<forward_iterator_tag, test::Common::no>{data}, expected_ints);

    data = gen();
    test_one(move_only_view<forward_iterator_tag, test::Common::yes>{data}, expected_ints);

    data = gen();
    test_one(move_only_view<bidirectional_iterator_tag, test::Common::no>{data}, expected_ints);

    data = gen();
    test_one(move_only_view<bidirectional_iterator_tag, test::Common::yes>{data}, expected_ints);
}

int main() {
    // Validate views
    { // ...copyable
        static constexpr array<string_view, 5> data = {{{}, "Hello "sv, {}, "World!"sv, {}}};
        constexpr span<const string_view, 5> input{data};
        constexpr string_view expected = "Hello World!"sv;
        static_assert(test_one(input, expected));
        test_one(input, expected);
    }
    // ... move-only
    test_move_only_views();

    // Validate non-views
#if defined(__clang__) || defined(__EDG__) // TRANSITION, FIXME
    { // ... C array
        static constexpr int join_me[5][2] = {{0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}};
        static_assert(test_one(join_me, expected_ints));
        test_one(join_me, expected_ints);
    }
#endif // TRANSITION, FIXME
#if 0 // FIXME
    { // ... contiguous container
        string str{"Hello, World!"};
        constexpr auto expected = "!dlroW ,olleH"sv;
        test_one(str, expected);
    }
    { // ... bidi container
        list<int> lst{3, 4, 5};
        static constexpr int joined[] = {5, 4, 3};
        test_one(lst, joined);

        static constexpr int joined_prefix[] = {4, 3};
        assert(ranges::equal(
            views::join(ranges::subrange{counted_iterator{lst.begin(), 2}, default_sentinel}), joined_prefix));
    }
#endif // FIXME

#if defined(__clang__) || defined(__EDG__) // FIXME: C1060 "out of heap space"
    // Get full instantiation coverage
    static_assert((test_nested_inout<instantiator, const int>(), true));
    test_nested_inout<instantiator, const int>();
#endif // FIXME
}
