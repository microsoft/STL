// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;

template <class Rng>
concept CanViewAll = requires(Rng&& r) {
    views::all(static_cast<Rng&&>(r));
};

// Test a silly precomposed range adaptor pipeline
constexpr auto pipeline = views::all | views::all | views::all | views::all | views::all | views::all;

template <class Rng>
constexpr bool test_one(Rng&& rng) {
    constexpr bool is_view = ranges::view<remove_cvref_t<Rng>>;
    static_assert(is_view || is_lvalue_reference_v<Rng>);

    using V = conditional_t<is_view, remove_cvref_t<Rng>, ranges::ref_view<remove_reference_t<Rng>>>;

    static_assert(CanViewAll<Rng&> == (!is_view || copyable<V>) );
    if constexpr (!is_view || copyable<V>) { // Validate lvalue
        constexpr bool is_noexcept = !is_view || is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<views::all_t<Rng>, V>);
        static_assert(same_as<decltype(views::all(rng)), V>);
        static_assert(noexcept(views::all(rng)) == is_noexcept);

        static_assert(same_as<decltype(rng | views::all), V>);
        static_assert(noexcept(rng | views::all) == is_noexcept);

        static_assert(same_as<decltype(views::all(rng)), decltype(rng | views::all | views::all | views::all)>);
        static_assert(noexcept(rng | views::all | views::all | views::all) == is_noexcept);

        static_assert(same_as<decltype(views::all(rng)), decltype(rng | pipeline)>);
        static_assert(noexcept(rng | pipeline) == is_noexcept);
    }

    static_assert(CanViewAll<const remove_cvref_t<Rng>&> == (!is_view || copyable<V>) );
    if constexpr (is_view && copyable<V>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<views::all_t<const remove_cvref_t<Rng>&>, V>);
        static_assert(same_as<decltype(views::all(as_const(rng))), V>);
        static_assert(noexcept(views::all(as_const(rng))) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | views::all), V>);
        static_assert(noexcept(as_const(rng) | views::all) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | views::all | views::all | views::all), V>);
        static_assert(noexcept(as_const(rng) | views::all | views::all | views::all) == is_noexcept);

        static_assert(same_as<decltype(as_const(rng) | pipeline), V>);
        static_assert(noexcept(as_const(rng) | pipeline) == is_noexcept);
    } else if constexpr (!is_view) {
        using RV = ranges::ref_view<const remove_cvref_t<Rng>>;

        static_assert(same_as<views::all_t<const remove_cvref_t<Rng>&>, RV>);
        static_assert(same_as<decltype(views::all(as_const(rng))), RV>);
        static_assert(noexcept(views::all(as_const(rng))));

        static_assert(same_as<decltype(as_const(rng) | views::all), RV>);
        static_assert(noexcept(as_const(rng) | views::all));

        static_assert(same_as<decltype(as_const(rng) | views::all | views::all | views::all), RV>);
        static_assert(noexcept(as_const(rng) | views::all | views::all | views::all));

        static_assert(same_as<decltype(as_const(rng) | pipeline), RV>);
        static_assert(noexcept(as_const(rng) | pipeline));
    }

    // Validate rvalue
    static_assert(CanViewAll<remove_cvref_t<Rng>> == is_view || ranges::enable_borrowed_range<remove_cvref_t<Rng>>);
    if constexpr (is_view) {
        constexpr bool is_noexcept = is_nothrow_move_constructible_v<V>;
        static_assert(same_as<views::all_t<remove_cvref_t<Rng>>, V>);
        static_assert(same_as<decltype(views::all(move(rng))), V>);
        static_assert(noexcept(views::all(move(rng))) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | views::all), V>);
        static_assert(noexcept(move(rng) | views::all) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | views::all | views::all | views::all), V>);
        static_assert(noexcept(move(rng) | views::all | views::all | views::all) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | pipeline), V>);
        static_assert(noexcept(move(rng) | pipeline) == is_noexcept);
    } else if constexpr (ranges::enable_borrowed_range<Rng>) {
        using S                    = decltype(ranges::subrange{declval<Rng>()});
        constexpr bool is_noexcept = noexcept(S{declval<Rng>()});

        static_assert(same_as<views::all_t<remove_cvref_t<Rng>>, S>);
        static_assert(same_as<decltype(views::all(move(rng))), S>);
        static_assert(noexcept(views::all(move(rng))) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | views::all), S>);
        static_assert(noexcept(move(rng) | views::all) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | views::all | views::all | views::all), S>);
        static_assert(noexcept(move(rng) | views::all | views::all | views::all) == is_noexcept);

        static_assert(same_as<decltype(move(rng) | pipeline), S>);
        static_assert(noexcept(move(rng) | pipeline) == is_noexcept);
    }

    // Validate const rvalue
    static_assert(CanViewAll<const remove_cvref_t<Rng>> == (is_view && copyable<V>)
                  || (!is_view && ranges::enable_borrowed_range<remove_cvref_t<Rng>>) );
    if constexpr (is_view && copyable<V>) {
        constexpr bool is_noexcept = is_nothrow_copy_constructible_v<V>;

        static_assert(same_as<views::all_t<const remove_cvref_t<Rng>>, V>);
        static_assert(same_as<decltype(views::all(move(as_const(rng)))), V>);
        static_assert(noexcept(views::all(as_const(rng))) == is_noexcept);

        static_assert(same_as<decltype(move(as_const(rng)) | views::all), V>);
        static_assert(noexcept(as_const(rng) | views::all) == is_noexcept);

        static_assert(same_as<decltype(move(as_const(rng)) | views::all | views::all | views::all), V>);
        static_assert(noexcept(move(as_const(rng)) | views::all | views::all | views::all) == is_noexcept);

        static_assert(same_as<decltype(move(as_const(rng)) | pipeline), V>);
        static_assert(noexcept(move(as_const(rng)) | pipeline) == is_noexcept);
    } else if constexpr (!is_view && ranges::enable_borrowed_range<const remove_cvref_t<Rng>>) {
        using S                    = decltype(ranges::subrange{declval<const remove_cvref_t<Rng>>()});
        constexpr bool is_noexcept = noexcept(S{declval<const remove_cvref_t<Rng>>()});

        static_assert(same_as<views::all_t<const remove_cvref_t<Rng>>, S>);
        static_assert(same_as<decltype(views::all(move(as_const(rng)))), S>);
        static_assert(noexcept(views::all(move(as_const(rng)))) == is_noexcept);

        static_assert(same_as<decltype(move(as_const(rng)) | views::all), S>);
        static_assert(noexcept(move(as_const(rng)) | views::all) == is_noexcept);

        static_assert(same_as<decltype(move(as_const(rng)) | views::all | views::all | views::all), S>);
        static_assert(noexcept(move(as_const(rng)) | views::all | views::all | views::all) == is_noexcept);

        static_assert(same_as<decltype(move(as_const(rng)) | pipeline), S>);
        static_assert(noexcept(move(as_const(rng)) | pipeline) == is_noexcept);
    }

    return true;
}

struct non_view_borrowed_range {
    int* begin() const;
    int* end() const;
};

template <>
inline constexpr bool ranges::enable_borrowed_range<non_view_borrowed_range> = true;

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const int, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

int main() {
    static constexpr int some_ints[] = {0, 1, 2};

    // Validate views
    { // ... copyable
        constexpr string_view str{"Hello, World!"};
        static_assert(test_one(str));
        test_one(str);
        assert(ranges::equal(views::all(str), str));
    }
    { // ... move-only
        test_one(move_only_view<input_iterator_tag, test::Common::no>{some_ints});
        assert(ranges::equal(views::all(move_only_view<input_iterator_tag, test::Common::no>{some_ints}), some_ints));
        test_one(move_only_view<forward_iterator_tag, test::Common::no>{some_ints});
        assert(ranges::equal(views::all(move_only_view<forward_iterator_tag, test::Common::no>{some_ints}), some_ints));
        test_one(move_only_view<forward_iterator_tag, test::Common::yes>{some_ints});
        assert(
            ranges::equal(views::all(move_only_view<forward_iterator_tag, test::Common::yes>{some_ints}), some_ints));
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints});
        assert(ranges::equal(
            views::all(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_ints}), some_ints));
        test_one(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints});
        assert(ranges::equal(
            views::all(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_ints}), some_ints));
        test_one(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints});
        assert(ranges::equal(
            views::all(move_only_view<random_access_iterator_tag, test::Common::no>{some_ints}), some_ints));
        test_one(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints});
        assert(ranges::equal(
            views::all(move_only_view<random_access_iterator_tag, test::Common::yes>{some_ints}), some_ints));
    }

    // Validate non-views
    {
        static_assert(test_one(some_ints));
        test_one(some_ints);
        assert(ranges::equal(views::all(some_ints), some_ints));
    }
    {
        string str{"Hello, World!"};
        test_one(str);
        assert(ranges::equal(views::all(str), str));
    }

    // Validate a non-view borrowed range
    {
        constexpr span s{some_ints};
        static_assert(test_one(s));
        test_one(s);
    }
}
