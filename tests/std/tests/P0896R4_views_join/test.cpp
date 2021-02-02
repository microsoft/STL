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
        ranges::forward_range, ranges::input_range, ranges::iterator_t, ranges::join_view, ranges::range_value_t;

    using Inner = range_value_t<Outer>;
    static_assert(ranges::range<Inner>);

    constexpr bool deref_is_glvalue = is_reference_v<ranges::range_reference_t<Outer>>;

    if constexpr (deref_is_glvalue || ranges::view<Inner>) {
        using V = views::all_t<Outer>;
        using R = join_view<V>;
        static_assert(ranges::view<R>);
        static_assert(input_range<R> == input_range<Inner>);
        static_assert(forward_range<R> == forward_range<Outer> && forward_range<Inner>);
        static_assert(bidirectional_range<R> == bidirectional_range<Outer> && bidirectional_range<Inner>);
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

        // Validate join_view::size
        static_assert(!CanSize<R>);

        // Validate view_interface::empty and operator bool
        static_assert(CanEmpty<R> == forward_range<Outer>);
        static_assert(CanMemberEmpty<R> == CanEmpty<R>);
        if (forward_range<Outer>) {
            const bool is_empty = ranges::empty(expected);
            assert(r.empty() == is_empty);
            assert(static_cast<bool>(r) == !is_empty);

            static_assert(CanEmpty<const R> == forward_range<const Outer>);
            static_assert(CanMemberEmpty<const R> == CanEmpty<const R>);
            if constexpr (forward_range<const Outer>) {
                assert(as_const(r).empty() == is_empty);
                assert(static_cast<bool>(as_const(r)) == !is_empty);
            }
        }

#if 0 // FIXME
      // Validate join_view::begin
        static_assert(CanMemberBegin<R>);
        {
            // join_view sometimes caches begin, so let's make several extra calls // FIXME: lies
            const same_as<join_iterator<iterator_t<V>>> auto i = r.begin();
            if (!is_empty) {
                assert(*i == *begin(expected));
            }
            assert(r.begin() == i);
            assert(r.begin() == i);
            // NB: non-const begin is unconditionally noexcept(false) due to caching // FIXME: lies
            static_assert(!noexcept(r.begin()));

            if constexpr (copyable<V>) {
                auto r2                                             = r;
                const same_as<join_iterator<iterator_t<V>>> auto i2 = r2.begin();
                assert(r2.begin() == i2);
                assert(r2.begin() == i2);
                if (!is_empty) {
                    assert(*i2 == *i);
                }
            }

            static_assert(CanMemberBegin<const R> == common_range<Outer>);
            if constexpr (common_range<Outer>) {
                const same_as<join_iterator<iterator_t<const V>>> auto ci = as_const(r).begin();
                assert(as_const(r).begin() == ci);
                assert(as_const(r).begin() == ci);
                if (!is_empty) {
                    assert(*ci == *i);
                }
                static_assert(noexcept(as_const(r).begin()) == noexcept(join_iterator{end(as_const(rng))}));

                if constexpr (copyable<V>) {
                    const auto r2                                              = r;
                    const same_as<join_iterator<iterator_t<const V>>> auto ci2 = r2.begin();
                    assert(r2.begin() == ci2);
                    assert(r2.begin() == ci2);
                    if (!is_empty) {
                        assert(*ci2 == *i);
                    }
                }
            }
        }

        // Validate join_view::end
        static_assert(CanMemberEnd<R>);
        if (!is_empty) {
            assert(*prev(r.end()) == *prev(end(expected)));

            if constexpr (copyable<V>) {
                auto r2 = r;
                assert(*prev(r2.end()) == *prev(end(expected)));
            }
            static_assert(noexcept(r.end()) == noexcept(join_iterator{begin(rng)}));

            static_assert(CanMemberEnd<const R> == common_range<Outer>);
            if constexpr (common_range<Outer>) {
                assert(*prev(as_const(r).end()) == *prev(end(expected)));
                static_assert(noexcept(as_const(r).end()) == noexcept(join_iterator{begin(as_const(rng))}));
            }
        }

        // Validate view_interface::data
        static_assert(!CanData<R>);
        static_assert(!CanData<const R>);

        if (!is_empty) {
            // Validate view_interface::operator[]
            static_assert(CanIndex<R> == random_access_range<Outer>);
            static_assert(CanIndex<const R> == (random_access_range<Outer> && common_range<Outer>) );
            if constexpr (random_access_range<Outer>) {
                assert(r[0] == *begin(expected));

                if constexpr (common_range<Outer>) {
                    assert(as_const(r)[0] == *begin(expected));
                }
            }

            // Validate view_interface::front and back
            assert(r.front() == *begin(expected));
            assert(r.back() == *prev(end(expected)));

            static_assert(CanMemberFront<const R> == common_range<Outer>);
            static_assert(CanMemberBack<const R> == common_range<Outer>);
            if constexpr (common_range<Outer>) {
                assert(as_const(r).front() == *begin(expected));
                assert(as_const(r).back() == *prev(end(expected)));
            }
        }

        // Validate join_view::base() const&
        static_assert(CanMemberBase<const R&> == copy_constructible<V>);
        if constexpr (copy_constructible<V>) {
            same_as<V> auto b1 = as_const(r).base();
            static_assert(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>);
            if (!is_empty) {
                assert(*b1.begin() == *prev(end(expected)));
                if constexpr (common_range<V>) {
                    assert(*prev(b1.end()) == *begin(expected));
                }
            }
        }

        // Validate join_view::base() && (NB: do this last since it leaves r moved-from)
        same_as<V> auto b2 = move(r).base();
        static_assert(noexcept(move(r).base()) == is_nothrow_move_constructible_v<V>);
        if (!is_empty) {
            assert(*b2.begin() == *prev(end(expected)));
            if constexpr (common_range<V>) {
                assert(*prev(b2.end()) == *begin(expected));
            }
        }
#endif // FIXME
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

static constexpr int some_ints[]             = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
static constexpr span<const int> intervals[] = {
    {some_ints + 0, some_ints + 3},
    {some_ints + 3, some_ints + 7},
    {some_ints + 7, some_ints + 7},
    {some_ints + 7, some_ints + 10},
};

struct instantiator {
    template <ranges::range Inner, ranges::input_range Outer>
    static constexpr void call() {
        static_assert(ranges::size(intervals) == 4);
        array inner_ranges = {Inner{intervals[0]}, Inner{intervals[1]}, Inner{intervals[2]}, Inner{intervals[3]}};
        test_one(Outer{inner_ranges}, some_ints);
    }
};

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

int main() {
    // Validate views
    { // ...copyable
        static constexpr array<string_view, 5> data = {{{}, "Hello "sv, {}, "World!"sv, {}}};
        constexpr span<const string_view, 5> input{data};
        constexpr string_view expected = "Hello World!"sv;
        static_assert(test_one(input, expected));
        test_one(input, expected);
    }
#if 0 // FIXME
    { // ... move-only
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints}, joind_ints);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints}, joind_ints);
        test_one(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints}, joind_ints);
        test_one(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints}, joind_ints);
        test_one(move_only_view<contiguous_iterator_tag, test::Common::no>{some_ints}, joind_ints);
        test_one(move_only_view<contiguous_iterator_tag, test::Common::yes>{some_ints}, joind_ints);
    }

    // Validate non-views
    { // ... C array
        static_assert(test_one(some_ints, joind_ints));
        test_one(some_ints, joind_ints);
    }
    { // ... contiguous container
        string str{"Hello, World!"};
        constexpr auto expected = "!dlroW ,olleH"sv;
        test_one(str, expected);
    }
    { // ... bidi container
        list<int> lst{3, 4, 5};
        static constexpr int joind[] = {5, 4, 3};
        test_one(lst, joind);

        static constexpr int joind_prefix[] = {4, 3};
        assert(ranges::equal(
            views::join(ranges::subrange{counted_iterator{lst.begin(), 2}, default_sentinel}), joind_prefix));
    }

    // Validate a non-view borrowed range
    {
        constexpr span s{some_ints};
        static_assert(test_one(s, joind_ints));
        test_one(s, joind_ints);
    }
    // Get full instantiation coverage
    static_assert((test_nested_inout<instantiator, const int>(), true));
    test_nested_inout<instantiator, const int>();
#endif // FIXME
}
