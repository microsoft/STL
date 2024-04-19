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

#pragma warning(disable : 6011) // Dereferencing NULL pointer '%s'

struct evil_convertible_to_difference {
    evil_convertible_to_difference() = default;
    evil_convertible_to_difference(const evil_convertible_to_difference&) {
        throw 42;
    }
    evil_convertible_to_difference(evil_convertible_to_difference&&) = default;

    evil_convertible_to_difference& operator=(const evil_convertible_to_difference&) {
        throw 42;
        return *this;
    }
    evil_convertible_to_difference& operator=(evil_convertible_to_difference&&) = default;

    constexpr operator int() const noexcept {
        return 4;
    }
};

// Test a silly precomposed range adaptor pipeline
constexpr auto pipeline = views::drop(1) | views::drop(1) | views::drop(1) | views::drop(1);

template <class>
constexpr bool is_subrange = false;
template <class I, class S, ranges::subrange_kind K>
constexpr bool is_subrange<ranges::subrange<I, S, K>> = true;

template <class>
struct mapped {
    template <class Rng>
    using apply = ranges::drop_view<views::all_t<Rng>>;
};
template <class T>
struct mapped<ranges::empty_view<T>> {
    template <class>
    using apply = ranges::empty_view<T>;
};
template <class T, size_t N>
struct mapped<span<T, N>> {
    template <class>
    using apply = span<T>;
};
template <class CharT, class Traits>
struct mapped<basic_string_view<CharT, Traits>> {
    template <class>
    using apply = basic_string_view<CharT, Traits>;
};
template <class W, class B>
    requires ranges::random_access_range<ranges::iota_view<W, B>> && ranges::sized_range<ranges::iota_view<W, B>>
struct mapped<ranges::iota_view<W, B>> {
    template <class>
    using apply = ranges::iota_view<W, B>;
};
template <class I, class S>
    requires random_access_iterator<I>
struct mapped<ranges::subrange<I, S, ranges::subrange_kind::sized>> {
    template <class>
    using apply = ranges::subrange<I, S, ranges::subrange_kind::sized>;
};

template <ranges::viewable_range Rng>
using mapped_t = mapped<remove_cvref_t<Rng>>::template apply<Rng>;

template <ranges::viewable_range Rng>
using pipeline_t = mapped_t<mapped_t<mapped_t<mapped_t<Rng>>>>;

template <class Rng>
concept CanViewDrop = requires(Rng&& r) { views::drop(forward<Rng>(r), 42); };

template <ranges::input_range Rng, ranges::random_access_range Expected>
constexpr bool test_one(Rng&& rng, Expected&& expected) {
    using ranges::drop_view, ranges::common_range, ranges::enable_borrowed_range, ranges::iterator_t, ranges::prev,
        ranges::range, ranges::sentinel_t, ranges::sized_range, ranges::borrowed_range;
    using ranges::input_range, ranges::forward_range, ranges::bidirectional_range, ranges::random_access_range,
        ranges::contiguous_range;

    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;

    using V = views::all_t<Rng>;
    using M = mapped_t<Rng>;
    static_assert(ranges::view<M>);
    static_assert(common_range<M> == common_range<Rng>);
    static_assert(input_range<M> == input_range<Rng>);
    static_assert(forward_range<M> == forward_range<Rng>);
    static_assert(bidirectional_range<M> == bidirectional_range<Rng>);
    static_assert(random_access_range<M> == random_access_range<Rng>);
    static_assert(contiguous_range<M> == contiguous_range<Rng>);

    // Validate range adaptor object and range adaptor closure
    constexpr auto closure = views::drop(4);

    // ... with lvalue argument
    static_assert(CanViewDrop<Rng&> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewDrop<Rng&>) { // Validate lvalue
        constexpr bool is_noexcept = !is_view || (is_nothrow_copy_constructible_v<V> && !is_subrange<V>);

        static_assert(same_as<decltype(views::drop(rng, 4)), M>);
        static_assert(noexcept(views::drop(rng, 4)) == is_noexcept);

        static_assert(same_as<decltype(rng | closure), M>);
        static_assert(noexcept(rng | closure) == is_noexcept);

        static_assert(same_as<decltype(rng | pipeline), pipeline_t<Rng&>>);
        static_assert(noexcept(rng | pipeline) == is_noexcept);
    }

    // ... with const lvalue argument
    static_assert(CanViewDrop<const remove_reference_t<Rng>&> == (!is_view || copy_constructible<V>) );
    if constexpr (is_view && copy_constructible<V>) {
        constexpr bool is_noexcept = (is_nothrow_copy_constructible_v<V> && !is_subrange<V>);

        static_assert(same_as<decltype(views::drop(as_const(rng), 4)), M>);
        static_assert(noexcept(views::drop(as_const(rng), 4)) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | closure), M>);
        static_assert(noexcept(as_const(rng) | closure) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | pipeline), pipeline_t<const remove_reference_t<Rng>&>>);
        static_assert(noexcept(as_const(rng) | pipeline) == is_noexcept);
    } else if constexpr (!is_view) {
        using RC                   = mapped_t<const remove_reference_t<Rng>&>;
        constexpr bool is_noexcept = is_nothrow_constructible_v<RC, const remove_reference_t<Rng>&, int>;

        static_assert(same_as<decltype(views::drop(as_const(rng), 4)), RC>);
        static_assert(noexcept(views::drop(as_const(rng), 4)) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | closure), RC>);
        static_assert(noexcept(as_const(rng) | closure) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | pipeline), pipeline_t<const remove_reference_t<Rng>&>>);
        static_assert(noexcept(as_const(rng) | pipeline) == is_noexcept);
    }

    // ... with rvalue argument
    static_assert(CanViewDrop<remove_reference_t<Rng>> == (is_view || movable<remove_reference_t<Rng>>) );
    if constexpr (is_view) {
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V> && !is_subrange<V>;
        static_assert(same_as<decltype(views::drop(move(rng), 4)), M>);
        static_assert(noexcept(views::drop(move(rng), 4)) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | closure), M>);
        static_assert(noexcept(move(rng) | closure) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | pipeline), pipeline_t<remove_reference_t<Rng>>>);
        static_assert(noexcept(move(rng) | pipeline) == is_noexcept);
    } else if constexpr (movable<remove_reference_t<Rng>>) {
        using S                    = ranges::owning_view<remove_reference_t<Rng>>;
        using RS                   = drop_view<S>;
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<remove_reference_t<Rng>>;

        static_assert(same_as<decltype(views::drop(move(rng), 4)), RS>);
        static_assert(noexcept(views::drop(move(rng), 4)) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | closure), RS>);
        static_assert(noexcept(move(rng) | closure) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | pipeline), mapped_t<mapped_t<mapped_t<RS>>>>);
        static_assert(noexcept(move(rng) | pipeline) == is_noexcept);
    }

    // ... with const rvalue argument
    static_assert(CanViewDrop<const remove_reference_t<Rng>> == (is_view && copy_constructible<V>) );
    if constexpr (is_view && copy_constructible<V>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V> && !is_subrange<V>;

        static_assert(same_as<decltype(views::drop(move(as_const(rng)), 4)), M>);
        static_assert(noexcept(views::drop(move(as_const(rng)), 4)) == is_noexcept);

        static_assert(same_as<decltype(move(as_const(rng)) | closure), M>);
        static_assert(noexcept(move(as_const(rng)) | closure) == is_noexcept);

        static_assert(same_as<decltype(move(as_const(rng)) | pipeline), pipeline_t<const remove_reference_t<Rng>>>);
        static_assert(noexcept(move(as_const(rng)) | pipeline) == is_noexcept);
    }

    const bool is_empty = ranges::empty(expected);

    // Validate deduction guide
    same_as<drop_view<V>> auto r = drop_view{forward<Rng>(rng), 4};
    using R                      = decltype(r);
    static_assert(ranges::view<R>);
    static_assert(input_range<R> == input_range<Rng>);
    static_assert(forward_range<R> == forward_range<Rng>);
    static_assert(bidirectional_range<R> == bidirectional_range<Rng>);
    static_assert(random_access_range<R> == random_access_range<Rng>);
    static_assert(contiguous_range<R> == contiguous_range<Rng>);
    static_assert(borrowed_range<R> == borrowed_range<V>);

    // Validate drop_view::size
    static_assert(CanMemberSize<R> == CanSize<Rng>);
    if constexpr (CanMemberSize<R>) {
        assert(r.size() == static_cast<decltype(r.size())>(ranges::size(expected)));
    } else {
        static_assert(!CanSize<R>);
    }

    // Validate view_interface::empty and operator bool
    static_assert(CanMemberEmpty<R> == (sized_range<Rng> || forward_range<Rng>) );
    static_assert(CanBool<R> == CanEmpty<R>);
    if constexpr (CanMemberEmpty<R>) {
        assert(r.empty() == is_empty);
        assert(static_cast<bool>(r) == !is_empty);
    } else {
        static_assert(CanEmpty<R> == CanSize<R>);
        if constexpr (CanEmpty<R>) {
            assert(ranges::empty(r) == is_empty);
            assert(static_cast<bool>(r) == !is_empty);
        }
    }

    static_assert(CanMemberEmpty<const R> == (random_access_range<const Rng> && sized_range<const Rng>) );
    static_assert(CanBool<const R> == CanEmpty<const R>);
    if constexpr (CanMemberEmpty<const R>) {
        assert(as_const(r).empty() == is_empty);
        assert(static_cast<bool>(as_const(r)) == !is_empty);
    } else {
        static_assert(CanEmpty<const R> == CanSize<const R>);
        if constexpr (CanEmpty<const R>) {
            assert(ranges::empty(as_const(r)) == is_empty);
            assert(static_cast<bool>(as_const(r)) == !is_empty);
        }
    }

    // Validate content
    assert(ranges::equal(r, expected));

    // Validate drop_view::begin
    static_assert(CanMemberBegin<R>);
    static_assert(same_as<iterator_t<R>, iterator_t<V>>);
    static_assert(CanBegin<const R&> == (random_access_range<const V> && sized_range<const V>) );
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

        if constexpr (random_access_range<const V> && sized_range<const V>) {
            static_assert(same_as<iterator_t<const R>, iterator_t<const V>>);
            const same_as<iterator_t<const R>> auto i3 = as_const(r).begin();
            if (!is_empty) {
                assert(*i3 == *i);
            }
        }
    }

    // Validate drop_view::end
    static_assert(CanMemberEnd<R>);
    static_assert(same_as<sentinel_t<R>, sentinel_t<V>>);
    static_assert(CanEnd<const R&> == (random_access_range<const V> && sized_range<const V>) );
    if (!is_empty) {
        same_as<sentinel_t<R>> auto i = r.end();
        if constexpr (bidirectional_range<R> && common_range<R>) {
            assert(*prev(i) == *prev(end(expected)));
        }

        if constexpr (random_access_range<const V> && sized_range<const V>) {
            same_as<sentinel_t<const R>> auto i2 = as_const(r).end();
            if constexpr (bidirectional_range<const R> && common_range<const R>) {
                assert(*prev(i2) == *prev(end(expected)));
            }
        }
    }

#if _HAS_CXX23
    using ranges::const_iterator_t, ranges::const_sentinel_t, ranges::cbegin, ranges::cend;

    // Validate view_interface::cbegin
    static_assert(CanMemberCBegin<R> == input_range<V>);
    static_assert(same_as<const_iterator_t<R>, const_iterator_t<V>>);
    static_assert(CanMemberCBegin<const R&> == (random_access_range<const V> && sized_range<const V>) );
    if (forward_range<V>) { // intentionally not if constexpr
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

        if constexpr (random_access_range<const V> && sized_range<const V>) {
            static_assert(same_as<const_iterator_t<const R>, const_iterator_t<const V>>);
            const same_as<const_iterator_t<const R>> auto i3 = as_const(r).cbegin();
            if (!is_empty) {
                assert(*i3 == *i);
            }
        }
    }

    // Validate view_interface::cend
    static_assert(CanMemberCEnd<R> == input_range<V>);
    static_assert(same_as<const_sentinel_t<R>, const_sentinel_t<V>>);
    static_assert(CanMemberCEnd<const R&> == (random_access_range<const V> && sized_range<const V>) );
    if (!is_empty) {
        same_as<const_sentinel_t<R>> auto i = r.cend();
        if constexpr (bidirectional_range<R> && common_range<R>) {
            assert(*prev(i) == *prev(cend(expected)));
        }

        if constexpr (random_access_range<const V> && sized_range<const V>) {
            same_as<const_sentinel_t<const R>> auto i2 = as_const(r).cend();
            if constexpr (bidirectional_range<const R> && common_range<const R>) {
                assert(*prev(i2) == *prev(cend(expected)));
            }
        }
    }
#endif // _HAS_CXX23

    // Validate view_interface::data
    static_assert(CanMemberData<R> == contiguous_range<V>);
    static_assert(CanData<R&> == contiguous_range<V>);
    static_assert(CanData<const R&> == (contiguous_range<const V> && sized_range<const V>) );
    if constexpr (contiguous_range<V>) {
        const same_as<remove_reference_t<ranges::range_reference_t<V>>*> auto ptr1 = r.data();
        assert(to_address(ptr1) == to_address(r.begin()));

        if constexpr (CanData<const R&>) {
            const same_as<remove_reference_t<ranges::range_reference_t<const V>>*> auto ptr2 = as_const(r).data();
            assert(to_address(ptr2) == to_address(as_const(r).begin()));
        }
    }

    // Validate view_interface::front and back
    if (!is_empty) {
        static_assert(CanMemberFront<R> == forward_range<V>);
        if constexpr (forward_range<V>) {
            assert(r.front() == *begin(expected));
        }

        static_assert(CanMemberFront<const R> == (random_access_range<const V> && sized_range<const V>) );
        if constexpr (CanMemberFront<const R>) {
            assert(as_const(r).front() == *begin(expected));
        }

        static_assert(CanMemberBack<R> == (bidirectional_range<R> && common_range<R>) );
        if constexpr (CanMemberBack<R>) {
            assert(r.back() == *prev(end(expected)));
        }

        static_assert(
            CanMemberBack<const R> == (common_range<const V> && random_access_range<const V> && sized_range<const V>) );
        if constexpr (CanMemberBack<const R>) {
            assert(as_const(r).back() == *prev(end(expected)));
        }
    }

    // Validate view_interface::operator[]
    static_assert(CanIndex<R> == random_access_range<V>);
    static_assert(CanIndex<const R> == (random_access_range<const V> && sized_range<const V>) );
    if (!is_empty) {
        if constexpr (CanIndex<R>) {
            assert(r[0] == *r.begin());
        }

        if constexpr (CanIndex<const R>) {
            assert(as_const(r)[0] == *as_const(r).begin());
        }
    }

    // Validate drop_view::base() const&
    static_assert(CanMemberBase<const R&> == copy_constructible<V>);
    if constexpr (CanMemberBase<const R&> && forward_range<V>) {
        same_as<V> auto b1 = as_const(r).base();
        static_assert(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<V>);
        if (!is_empty) {
            assert(*b1.begin() == 0); // NB: depends on the test data
            if constexpr (bidirectional_range<V> && common_range<V>) {
                assert(*prev(b1.end()) == 7); // NB: depends on the test data
            }
        }
    }

    // Validate drop_view::base() && (NB: do this last since it leaves r moved-from)
    if (forward_range<V>) { // intentionally not if constexpr
        same_as<V> auto b2 = move(r).base();
        static_assert(noexcept(move(r).base()) == is_nothrow_move_constructible_v<V>);
        if (!is_empty) {
            assert(*b2.begin() == 0); // NB: depends on the test data
            if constexpr (bidirectional_range<V> && common_range<V>) {
                assert(*prev(b2.end()) == 7); // NB: depends on the test data
            }
        }
    }

    return true;
}

static constexpr int some_ints[]      = {0, 1, 2, 3, 4, 5, 6, 7};
static constexpr int only_four_ints[] = {4, 5, 6, 7};

struct instantiator {
    template <ranges::input_range R>
    static constexpr void call() {
        R r{some_ints};
        test_one(r, only_four_ints);

        R empty_range{span<const int, 0>{}};
        test_one(empty_range, span<const int, 0>{});
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
    // The view is sensitive to category, size, and commonality, but oblivious to differencing and proxyness.
    using test::Common, test::Sized;

    instantiator::call<test_range<input_iterator_tag, Sized::no, Common::no>>();
    instantiator::call<test_range<input_iterator_tag, Sized::yes, Common::no>>();
    instantiator::call<test_range<input_iterator_tag, Sized::no, Common::yes>>();
    instantiator::call<test_range<input_iterator_tag, Sized::yes, Common::yes>>();
    instantiator::call<test_range<forward_iterator_tag, Sized::no, Common::no>>();
    instantiator::call<test_range<forward_iterator_tag, Sized::yes, Common::no>>();
    instantiator::call<test_range<forward_iterator_tag, Sized::no, Common::yes>>();
    instantiator::call<test_range<forward_iterator_tag, Sized::yes, Common::yes>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Sized::no, Common::no>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Sized::yes, Common::no>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Sized::no, Common::yes>>();
    instantiator::call<test_range<bidirectional_iterator_tag, Sized::yes, Common::yes>>();
    instantiator::call<test_range<random_access_iterator_tag, Sized::no, Common::no>>();
    instantiator::call<test_range<random_access_iterator_tag, Sized::yes, Common::no>>();
    instantiator::call<test_range<random_access_iterator_tag, Sized::no, Common::yes>>();
    instantiator::call<test_range<random_access_iterator_tag, Sized::yes, Common::yes>>();
    instantiator::call<test_range<contiguous_iterator_tag, Sized::no, Common::no>>();
    instantiator::call<test_range<contiguous_iterator_tag, Sized::yes, Common::no>>();
    instantiator::call<test_range<contiguous_iterator_tag, Sized::no, Common::yes>>();
    instantiator::call<test_range<contiguous_iterator_tag, Sized::yes, Common::yes>>();
#endif // TEST_EVERYTHING
}

template <class Category, test::Sized IsSized, test::Common IsCommon>
using move_only_view = test::range<Category, const int, IsSized,
    test::CanDifference{derived_from<Category, random_access_iterator_tag>}, IsCommon,
    test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

constexpr void move_only_test() {
    using test::Common, test::Sized;
    using input      = input_iterator_tag;
    using fwd        = forward_iterator_tag;
    using bidi       = bidirectional_iterator_tag;
    using random     = random_access_iterator_tag;
    using contiguous = contiguous_iterator_tag;

    test_one(move_only_view<input, Sized::no, Common::no>{some_ints}, only_four_ints);
    test_one(move_only_view<input, Sized::yes, Common::no>{some_ints}, only_four_ints);
    test_one(move_only_view<fwd, Sized::no, Common::no>{some_ints}, only_four_ints);
    test_one(move_only_view<fwd, Sized::yes, Common::no>{some_ints}, only_four_ints);
    test_one(move_only_view<fwd, Sized::no, Common::yes>{some_ints}, only_four_ints);
    test_one(move_only_view<fwd, Sized::yes, Common::yes>{some_ints}, only_four_ints);
    test_one(move_only_view<bidi, Sized::no, Common::no>{some_ints}, only_four_ints);
    test_one(move_only_view<bidi, Sized::yes, Common::no>{some_ints}, only_four_ints);
    test_one(move_only_view<bidi, Sized::no, Common::yes>{some_ints}, only_four_ints);
    test_one(move_only_view<bidi, Sized::yes, Common::yes>{some_ints}, only_four_ints);
    test_one(move_only_view<random, Sized::no, Common::no>{some_ints}, only_four_ints);
    test_one(move_only_view<random, Sized::yes, Common::no>{some_ints}, only_four_ints);
    test_one(move_only_view<random, Sized::no, Common::yes>{some_ints}, only_four_ints);
    test_one(move_only_view<random, Sized::yes, Common::yes>{some_ints}, only_four_ints);
    test_one(move_only_view<contiguous, Sized::no, Common::no>{some_ints}, only_four_ints);
    test_one(move_only_view<contiguous, Sized::yes, Common::no>{some_ints}, only_four_ints);
    test_one(move_only_view<contiguous, Sized::no, Common::yes>{some_ints}, only_four_ints);
    test_one(move_only_view<contiguous, Sized::yes, Common::yes>{some_ints}, only_four_ints);
}

constexpr void output_range_test() {
    using R = test::range<output_iterator_tag, int, test::Sized::no, test::CanDifference::no, test::Common::no,
        test::CanCompare::no, test::ProxyRef::yes, test::CanView::yes, test::Copyability::move_only>;
    int some_writable_ints[] = {0, 1, 2, 3};
    static_assert(same_as<decltype(views::drop(R{some_writable_ints}, 2)), ranges::drop_view<R>>);
    ranges::fill(R{some_writable_ints} | views::drop(2), 42);
    assert(ranges::equal(some_writable_ints, initializer_list<int>{0, 1, 42, 42}));
}

int main() {
    // Validate views
    { // ... copyable
        // Test all of the "reconstructible range" types: span, empty_view, subrange, basic_string_view, iota_view
        constexpr span s0{some_ints};
        static_assert(test_one(s0, only_four_ints));
        test_one(s0, only_four_ints);

        constexpr span<const int> s1{some_ints};
        static_assert(test_one(s1, only_four_ints));
        test_one(s1, only_four_ints);

        static_assert(test_one(ranges::subrange{some_ints}, only_four_ints));
        test_one(ranges::subrange{some_ints}, only_four_ints);

        static_assert(test_one(views::empty<int>, span<const int, 0>{}));
        test_one(views::empty<int>, span<const int, 0>{});

        static_assert(test_one(basic_string_view{ranges::begin(some_ints), ranges::end(some_ints)}, only_four_ints));
        test_one(basic_string_view{ranges::begin(some_ints), ranges::end(some_ints)}, only_four_ints);

        static_assert(test_one(ranges::iota_view{0, 8}, only_four_ints));
        test_one(ranges::iota_view{0, 8}, only_four_ints);
    }
    // ... move-only
    static_assert((move_only_test(), true));
    move_only_test();

    // Validate non-views
    {
        static_assert(test_one(some_ints, only_four_ints));
        test_one(some_ints, only_four_ints);
    }
    {
        vector vec(ranges::begin(some_ints), ranges::end(some_ints));
        test_one(vec, only_four_ints);
    }
    {
        forward_list lst(ranges::begin(some_ints), ranges::end(some_ints));
        test_one(lst, only_four_ints);
    }

    // Validate an output range
    static_assert((output_range_test(), true));
    output_range_test();

    static_assert((instantiation_test(), true));
    instantiation_test();

    {
        // Validate a view borrowed range
        constexpr auto v =
            views::iota(0ull, ranges::size(some_ints)) | views::transform([](auto i) { return some_ints[i]; });
        static_assert(test_one(v, only_four_ints));
        test_one(v, only_four_ints);
    }

    { // Validate that we can use something that is convertible to integral (GH-1957)
        constexpr span s{some_ints};
        auto r1 = s | views::drop(integral_constant<int, 4>{});
        assert(ranges::equal(r1, only_four_ints));

        auto r2 = s | views::drop(evil_convertible_to_difference{});
        assert(ranges::equal(r2, only_four_ints));
    }
}
