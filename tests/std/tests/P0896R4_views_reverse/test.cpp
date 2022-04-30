// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
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
concept CanViewReverse = requires(Rng&& r) {
    views::reverse(forward<Rng>(r));
};

// Test a silly precomposed range adaptor pipeline
constexpr auto pipeline = views::all | views::reverse | views::all | views::reverse | views::all | views::reverse;

template <ranges::bidirectional_range Rng, class Expected>
constexpr bool test_one(Rng&& rng, Expected&& expected) {
    using ranges::common_range, ranges::reverse_view, ranges::sized_range, ranges::begin, ranges::end, ranges::size,
        ranges::iterator_t, ranges::range_size_t, ranges::random_access_range, ranges::prev,
        ranges::enable_borrowed_range, ranges::borrowed_range;
    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;

    using V = views::all_t<Rng>;
    using R = reverse_view<V>;
    static_assert(ranges::view<R>);
    static_assert(ranges::bidirectional_range<R>);
    static_assert(random_access_range<R> == random_access_range<Rng>);
    static_assert(!ranges::contiguous_range<R>);
    static_assert(borrowed_range<R> == borrowed_range<V>);

    // Validate range adapter object
    // ...with lvalue argument
    static_assert(CanViewReverse<Rng&> == (!is_view || copy_constructible<V>) );
    if constexpr (CanViewReverse<Rng&>) {
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<decltype(views::reverse(rng)), R>);
        static_assert(noexcept(views::reverse(rng)) == is_noexcept);

        static_assert(same_as<decltype(rng | views::reverse), R>);
        static_assert(noexcept(rng | views::reverse) == is_noexcept);

        static_assert(same_as<decltype(views::reverse(views::reverse(rng))), V>);
        static_assert(noexcept(views::reverse(views::reverse(rng))) == is_noexcept);

        static_assert(same_as<decltype(rng | views::reverse | views::reverse | views::reverse), R>);
        static_assert(noexcept(rng | views::reverse | views::reverse | views::reverse) == is_noexcept);

        static_assert(same_as<decltype(rng | pipeline), R>);
        static_assert(noexcept(rng | pipeline) == is_noexcept);
    }

    // ... with const lvalue argument
    static_assert(CanViewReverse<const remove_reference_t<Rng>&> == (!is_view || copy_constructible<V>) );
    if constexpr (is_view && copy_constructible<V>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<decltype(views::reverse(as_const(rng))), R>);
        static_assert(noexcept(views::reverse(as_const(rng))) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | views::reverse), R>);
        static_assert(noexcept(as_const(rng) | views::reverse) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | views::reverse | views::reverse | views::reverse), R>);
        static_assert(noexcept(as_const(rng) | views::reverse | views::reverse | views::reverse) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | pipeline), R>);
        static_assert(noexcept(as_const(rng) | pipeline) == is_noexcept);
    } else if constexpr (!is_view) {
        using RC                   = reverse_view<ranges::ref_view<const remove_reference_t<Rng>>>;
        constexpr bool is_noexcept = true;

        static_assert(same_as<decltype(views::reverse(as_const(rng))), RC>);
        static_assert(noexcept(views::reverse(as_const(rng))) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | views::reverse), RC>);
        static_assert(noexcept(as_const(rng) | views::reverse) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | views::reverse | views::reverse | views::reverse), RC>);
        static_assert(noexcept(as_const(rng) | views::reverse | views::reverse | views::reverse) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | pipeline), RC>);
        static_assert(noexcept(as_const(rng) | pipeline) == is_noexcept);
    }

    // ... with rvalue argument
    static_assert(CanViewReverse<remove_reference_t<Rng>> == (is_view || movable<remove_reference_t<Rng>>) );
    if constexpr (is_view) {
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;
        static_assert(same_as<decltype(views::reverse(move(rng))), R>);
        static_assert(noexcept(views::reverse(move(rng))) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | views::reverse), R>);
        static_assert(noexcept(move(rng) | views::reverse) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | views::reverse | views::reverse | views::reverse), R>);
        static_assert(noexcept(move(rng) | views::reverse | views::reverse | views::reverse) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | pipeline), R>);
        static_assert(noexcept(move(rng) | pipeline) == is_noexcept);
    } else if constexpr (movable<remove_reference_t<Rng>>) {
        using RS                   = reverse_view<ranges::owning_view<remove_reference_t<Rng>>>;
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<remove_reference_t<Rng>>;

        static_assert(same_as<decltype(views::reverse(move(rng))), RS>);
        static_assert(noexcept(views::reverse(move(rng))) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | views::reverse), RS>);
        static_assert(noexcept(move(rng) | views::reverse) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | views::reverse | views::reverse | views::reverse), RS>);
        static_assert(noexcept(move(rng) | views::reverse | views::reverse | views::reverse) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | pipeline), RS>);
        static_assert(noexcept(move(rng) | pipeline) == is_noexcept);
    }

    // ... with const rvalue argument
    static_assert(CanViewReverse<const remove_reference_t<Rng>> == (is_view && copy_constructible<V>) );
    if constexpr (is_view && copy_constructible<V>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<decltype(views::reverse(move(as_const(rng)))), R>);
        static_assert(noexcept(views::reverse(move(as_const(rng)))) == is_nothrow_copy_constructible_v<R>);

        static_assert(same_as<decltype(move(as_const(rng)) | views::reverse), R>);
        static_assert(noexcept(move(as_const(rng)) | views::reverse) == is_nothrow_copy_constructible_v<R>);

        static_assert(same_as<decltype(move(as_const(rng)) | views::reverse | views::reverse | views::reverse), R>);
        static_assert(noexcept(move(as_const(rng)) | views::reverse | views::reverse | views::reverse) == is_noexcept);

        static_assert(same_as<decltype(move(as_const(rng)) | pipeline), R>);
        static_assert(noexcept(move(as_const(rng)) | pipeline) == is_noexcept);
    }

    // Validate deduction guide
    same_as<R> auto r = reverse_view{forward<Rng>(rng)};
    assert(ranges::equal(r, expected));

    // Validate reverse_view::size
    static_assert(CanMemberSize<R> == sized_range<Rng>);
    if constexpr (sized_range<Rng>) {
        assert(r.size() == static_cast<range_size_t<R>>(size(expected)));
        static_assert(noexcept(r.size()) == noexcept(size(rng)));
    }

    static_assert(CanMemberSize<const R> == sized_range<const Rng>);
    if constexpr (sized_range<const Rng>) {
        assert(as_const(r).size() == static_cast<range_size_t<R>>(size(expected)));
        static_assert(noexcept(r.size()) == noexcept(size(as_const(rng))));
    }

    // Validate view_interface::empty and operator bool
    const bool is_empty = ranges::empty(expected);
    assert(r.empty() == is_empty);
    assert(static_cast<bool>(r) == !is_empty);
    static_assert(CanMemberEmpty<const R> == common_range<Rng>);
    if constexpr (common_range<Rng>) {
        assert(as_const(r).empty() == is_empty);
        assert(static_cast<bool>(as_const(r)) == !is_empty);
    }

    // Validate reverse_view::begin
    static_assert(CanMemberBegin<R>);
    {
        // reverse_view sometimes caches begin, so let's make several extra calls
        const same_as<reverse_iterator<iterator_t<V>>> auto i = r.begin();
        if (!is_empty) {
            assert(*i == *begin(expected));
        }
        assert(r.begin() == i);
        assert(r.begin() == i);
        // NB: non-const begin is unconditionally noexcept(false) due to caching
        static_assert(!noexcept(r.begin()));

        if constexpr (copyable<V>) {
            auto r2                                                = r;
            const same_as<reverse_iterator<iterator_t<V>>> auto i2 = r2.begin();
            assert(r2.begin() == i2);
            assert(r2.begin() == i2);
            if (!is_empty) {
                assert(*i2 == *i);
            }
        }

        static_assert(CanMemberBegin<const R> == common_range<Rng>);
        if constexpr (common_range<Rng>) {
            const same_as<reverse_iterator<iterator_t<const V>>> auto ci = as_const(r).begin();
            assert(as_const(r).begin() == ci);
            assert(as_const(r).begin() == ci);
            if (!is_empty) {
                assert(*ci == *i);
            }
            static_assert(noexcept(as_const(r).begin()) == noexcept(reverse_iterator{end(as_const(rng))}));

            if constexpr (copyable<V>) {
                const auto r2                                                 = r;
                const same_as<reverse_iterator<iterator_t<const V>>> auto ci2 = r2.begin();
                assert(r2.begin() == ci2);
                assert(r2.begin() == ci2);
                if (!is_empty) {
                    assert(*ci2 == *i);
                }
            }
        }
    }

    // Validate reverse_view::end
    static_assert(CanMemberEnd<R>);
    if (!is_empty) {
        assert(*prev(r.end()) == *prev(end(expected)));

        if constexpr (copyable<V>) {
            auto r2 = r;
            assert(*prev(r2.end()) == *prev(end(expected)));
        }
        static_assert(noexcept(r.end()) == noexcept(reverse_iterator{begin(rng)}));

        static_assert(CanMemberEnd<const R> == common_range<Rng>);
        if constexpr (common_range<Rng>) {
            assert(*prev(as_const(r).end()) == *prev(end(expected)));
            static_assert(noexcept(as_const(r).end()) == noexcept(reverse_iterator{begin(as_const(rng))}));
        }
    }

    // Validate view_interface::data
    static_assert(!CanData<R>);
    static_assert(!CanData<const R>);

    if (!is_empty) {
        // Validate view_interface::operator[]
        static_assert(CanIndex<R> == random_access_range<Rng>);
        static_assert(CanIndex<const R> == (random_access_range<Rng> && common_range<Rng>) );
        if constexpr (random_access_range<Rng>) {
            assert(r[0] == *begin(expected));

            if constexpr (common_range<Rng>) {
                assert(as_const(r)[0] == *begin(expected));
            }
        }

        // Validate view_interface::front and back
        assert(r.front() == *begin(expected));
        assert(r.back() == *prev(end(expected)));

        static_assert(CanMemberFront<const R> == common_range<Rng>);
        static_assert(CanMemberBack<const R> == common_range<Rng>);
        if constexpr (common_range<Rng>) {
            assert(as_const(r).front() == *begin(expected));
            assert(as_const(r).back() == *prev(end(expected)));
        }
    }

    // Validate reverse_view::base() const&
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

    // Validate reverse_view::base() && (NB: do this last since it leaves r moved-from)
    same_as<V> auto b2 = move(r).base();
    static_assert(noexcept(move(r).base()) == is_nothrow_move_constructible_v<V>);
    if (!is_empty) {
        assert(*b2.begin() == *prev(end(expected)));
        if constexpr (common_range<V>) {
            assert(*prev(b2.end()) == *begin(expected));
        }
    }

    return true;
}

struct instantiator {
    template <ranges::bidirectional_range R>
    static constexpr void call() {
        R r{span<const int, 0>{}};
        test_one(r, span<const int, 0>{});
    }
};

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

void test_gh_2312() { // COMPILE-ONLY
    // GH-2312 <ranges>: using views::reverse on ranges::reverse_view lvalue is broken
    using X = ranges::iota_view<int, int>;
    ranges::reverse_view<X> view;
    static_assert(same_as<decltype(view | views::reverse), X>);
}

int main() {
    static constexpr int some_ints[]     = {0, 1, 2};
    static constexpr int reversed_ints[] = {2, 1, 0};

    // Validate views
    { // ...copyable
        constexpr string_view str{"Hello, World!"};
        constexpr auto expected = "!dlroW ,olleH"sv;
        static_assert(test_one(str, expected));
        test_one(str, expected);
    }
    { // ... move-only
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints}, reversed_ints);
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints}, reversed_ints);
        test_one(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints}, reversed_ints);
        test_one(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints}, reversed_ints);
        test_one(move_only_view<contiguous_iterator_tag, test::Common::no>{some_ints}, reversed_ints);
        test_one(move_only_view<contiguous_iterator_tag, test::Common::yes>{some_ints}, reversed_ints);
    }

    // Validate non-views
    { // ... C array
        static_assert(test_one(some_ints, reversed_ints));
        test_one(some_ints, reversed_ints);
    }
    { // ... contiguous container
        string str{"Hello, World!"};
        constexpr auto expected = "!dlroW ,olleH"sv;
        test_one(str, expected);
    }
    { // ... bidi container
        list<int> lst{3, 4, 5};
        static constexpr int reversed[] = {5, 4, 3};
        test_one(lst, reversed);

        static constexpr int reversed_prefix[] = {4, 3};
        assert(ranges::equal(
            views::reverse(ranges::subrange{counted_iterator{lst.begin(), 2}, default_sentinel}), reversed_prefix));
    }

    // Get full instantiation coverage
    static_assert((test_bidi<instantiator, const int>(), true));
    test_bidi<instantiator, const int>();
}
