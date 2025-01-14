// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <ranges>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;

template <class Rng, class Delimiter>
concept CanViewLazySplit =
    requires(Rng&& r, Delimiter&& d) { views::lazy_split(forward<Rng>(r), forward<Delimiter>(d)); };

constexpr auto equal_ranges    = [](auto&& left, auto&& right) { return ranges::equal(left, right); };
constexpr auto text            = "This is a test, this is only a test."sv;
constexpr auto trailing_empty  = "test "sv;
constexpr auto lwg3505_pattern = "xxyxxyx"sv;

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
    delimiter_view_impl<is_convertible_v<Delimiter, ranges::range_value_t<Base>>>::template apply<Base, Delimiter>;

template <ranges::input_range Base, class Delimiter, ranges::input_range Expected>
constexpr void test_one(Base&& base, Delimiter&& delimiter, Expected&& expected) {
    static_assert(CanViewLazySplit<Base, Delimiter>);
    using R = decltype(views::lazy_split(forward<Base>(base), forward<Delimiter>(delimiter)));

    // Validate type properties
    static_assert(ranges::view<R>);
    static_assert(ranges::input_range<R>);
    static_assert(ranges::forward_range<R> == ranges::forward_range<Base>);
    static_assert(!ranges::bidirectional_range<R>);

    // Validate range adaptor object and range adaptor closure
    using DV           = delimiter_view_t<Base, Delimiter>;
    const auto closure = views::lazy_split(delimiter);

    constexpr bool is_view = ranges::view<remove_cvref_t<Base>>;

    // ... with lvalue argument
    static_assert(CanViewLazySplit<Base&, Delimiter&> == (!is_view || copy_constructible<remove_cvref_t<Base>>) );
    if constexpr (CanViewLazySplit<Base&, Delimiter&>) { // Validate lvalue
        constexpr bool is_noexcept =
            (!is_view || is_nothrow_copy_constructible_v<views::all_t<Base&>>) && is_nothrow_copy_constructible_v<DV>;

        static_assert(same_as<decltype(views::lazy_split(base, delimiter)), R>);
        static_assert(noexcept(views::lazy_split(base, delimiter)) == is_noexcept);

        static_assert(same_as<decltype(base | closure), R>);
        static_assert(noexcept(base | closure) == is_noexcept);

        using outer_iterator = decltype(views::lazy_split(base, delimiter).begin());
        static_assert(!is_default_constructible_v<iter_value_t<outer_iterator>>); // LWG-4013
        static_assert(!is_constructible_v<iter_value_t<outer_iterator>, outer_iterator>); // LWG-4013
    }

    // ... with const lvalue argument
    static_assert(CanViewLazySplit<const remove_reference_t<Base>&, Delimiter&>
                  == (!is_view || copy_constructible<remove_cvref_t<Base>>) );
    if constexpr (is_view && copy_constructible<remove_cvref_t<Base>>) {
        constexpr bool is_noexcept =
            is_nothrow_copy_constructible_v<remove_cvref_t<Base>> && is_nothrow_copy_constructible_v<DV>;

        static_assert(same_as<decltype(views::lazy_split(as_const(base), delimiter)), R>);
        static_assert(noexcept(views::lazy_split(as_const(base), delimiter)) == is_noexcept);

        static_assert(same_as<decltype(as_const(base) | closure), R>);
        static_assert(noexcept(as_const(base) | closure) == is_noexcept);

        using outer_iterator = decltype(views::lazy_split(as_const(base), delimiter).begin());
        static_assert(!is_default_constructible_v<iter_value_t<outer_iterator>>); // LWG-4013
        static_assert(!is_constructible_v<iter_value_t<outer_iterator>, outer_iterator>); // LWG-4013
    } else if constexpr (!is_view) {
        using LSV                  = ranges::lazy_split_view<ranges::ref_view<const remove_reference_t<Base>>, DV>;
        constexpr bool is_noexcept = is_nothrow_constructible_v<LSV, const remove_reference_t<Base>&, Delimiter&>;

        static_assert(same_as<decltype(views::lazy_split(as_const(base), delimiter)), LSV>);
        static_assert(noexcept(views::lazy_split(as_const(base), delimiter)) == is_noexcept);

        static_assert(same_as<decltype(as_const(base) | closure), LSV>);
        static_assert(noexcept(as_const(base) | closure) == is_noexcept);
    }

    // ... with rvalue argument
    static_assert(
        CanViewLazySplit<remove_reference_t<Base>, Delimiter&> == (is_view || movable<remove_reference_t<Base>>) );
    if constexpr (is_view) {
        constexpr bool is_noexcept =
            is_nothrow_move_constructible_v<remove_reference_t<Base>> && is_nothrow_copy_constructible_v<DV>;
        static_assert(same_as<decltype(views::lazy_split(move(base), delimiter)), R>);
        static_assert(noexcept(views::lazy_split(move(base), delimiter)) == is_noexcept);

        static_assert(same_as<decltype(move(base) | closure), R>);
        static_assert(noexcept(move(base) | closure) == is_noexcept);

        using outer_iterator = decltype(views::lazy_split(move(base), delimiter).begin());
        static_assert(!is_default_constructible_v<iter_value_t<outer_iterator>>); // LWG-4013
        static_assert(!is_constructible_v<iter_value_t<outer_iterator>, outer_iterator>); // LWG-4013
    } else if constexpr (movable<remove_reference_t<Base>>) {
        using RS = ranges::lazy_split_view<ranges::owning_view<remove_reference_t<Base>>, DV>;
        constexpr bool is_noexcept =
            is_nothrow_move_constructible_v<remove_reference_t<Base>> && is_nothrow_copy_constructible_v<DV>;

        static_assert(same_as<decltype(views::lazy_split(move(base), delimiter)), RS>);
        static_assert(noexcept(views::lazy_split(move(base), delimiter)) == is_noexcept);

        static_assert(same_as<decltype(move(base) | closure), RS>);
        static_assert(noexcept(move(base) | closure) == is_noexcept);
    }

    // ... with const rvalue argument
    static_assert(CanViewLazySplit<const remove_reference_t<Base>, Delimiter&>
                  == (is_view && copy_constructible<remove_cvref_t<Base>>) );
    if constexpr (is_view && copy_constructible<remove_cvref_t<Base>>) {
        constexpr bool is_noexcept =
            is_nothrow_copy_constructible_v<remove_cvref_t<Base>> && is_nothrow_copy_constructible_v<DV>;

        static_assert(same_as<decltype(views::lazy_split(move(as_const(base)), delimiter)), R>);
        static_assert(noexcept(views::lazy_split(move(as_const(base)), delimiter)) == is_noexcept);

        static_assert(same_as<decltype(move(as_const(base)) | closure), R>);
        static_assert(noexcept(move(as_const(base)) | closure) == is_noexcept);

        using outer_iterator = decltype(views::lazy_split(move(as_const(base)), delimiter).begin());
        static_assert(!is_default_constructible_v<iter_value_t<outer_iterator>>); // LWG-4013
        static_assert(!is_constructible_v<iter_value_t<outer_iterator>, outer_iterator>); // LWG-4013
    }

    // Validate deduction guide
    same_as<R> auto r = ranges::lazy_split_view{forward<Base>(base), forward<Delimiter>(delimiter)};
    assert(ranges::equal(r, expected, equal_ranges));

    // Validate view_interface::empty and operator bool
    const bool is_empty = ranges::empty(expected);
    static_assert(CanMemberEmpty<R> == ranges::forward_range<Base>);
    static_assert(CanBool<R> == ranges::forward_range<Base>);
    if constexpr (ranges::forward_range<Base>) {
        assert(r.empty() == is_empty);
        assert(static_cast<bool>(r) == !is_empty);
    }

    // Validate lazy_split_view::begin
    static_assert(CanMemberBegin<R>);
    if (ranges::forward_range<Base>) { // intentionally not if constexpr
        const auto i = r.begin();
        if (!is_empty) {
            assert(ranges::equal(*i, *ranges::begin(expected)));
        }

        if constexpr (copyable<R>) {
            auto r2       = r;
            const auto i2 = r2.begin();
            if (!is_empty) {
                assert(ranges::equal(*i2, *i));
            }
        }

        static_assert(CanMemberBegin<const R> == ranges::forward_range<const remove_reference_t<Base>>);
        static_assert(CanBegin<const R&> == CanMemberBegin<const R>);
        if constexpr (CanBegin<const R&>) {
            const auto ic = as_const(r).begin();
            if (!is_empty) {
                assert(ranges::equal(*ic, *ranges::begin(expected)));
            }

            if constexpr (copyable<remove_reference_t<Base>>) {
                auto r2       = r;
                const auto i2 = as_const(r2).begin();
                if (!is_empty) {
                    assert(ranges::equal(*i2, *ic));
                }
            }
        }
    }

    // Validate lazy_split_view::end
    static_assert(CanMemberEnd<R>);
    [[maybe_unused]] same_as<ranges::sentinel_t<R>> auto s = r.end();
    if (ranges::forward_range<Base>) {
        assert((r.begin() == s) == is_empty);
    }
    static_assert(ranges::common_range<R> == (ranges::forward_range<Base> && ranges::common_range<Base>) );
    if constexpr (!ranges::common_range<R>) {
        static_assert(same_as<ranges::sentinel_t<R>, default_sentinel_t>);
    }

    static_assert(CanMemberEnd<const R>);
    constexpr bool should_be_const_common = ranges::forward_range<Base>
                                         && ranges::forward_range<const remove_cvref_t<Base>>
                                         && ranges::common_range<const remove_cvref_t<Base>>;
    static_assert(ranges::common_range<const R> == should_be_const_common);
    const auto sc = as_const(r).end();
    if constexpr (ranges::forward_range<Base> && ranges::forward_range<const remove_cvref_t<Base>>) {
        static_assert(same_as<decltype(sc), const ranges::sentinel_t<const R>>);
        assert((as_const(r).begin() == sc) == is_empty);
    }

    if constexpr (!ranges::common_range<const R>) {
        static_assert(same_as<ranges::sentinel_t<R>, default_sentinel_t>);
    }

    // Validate view_interface::data
    static_assert(!CanData<R>);
    static_assert(!CanData<const R>);

    // Validate view_interface::size
    static_assert(!CanSize<R>);
    static_assert(!CanSize<const R>);

    // Validate view_interface::operator[]
    static_assert(!CanIndex<R>);
    static_assert(!CanIndex<const R>);

    if (!is_empty) {
        // Validate view_interface::front and back
        static_assert(CanMemberFront<R> == ranges::forward_range<Base>);
        if constexpr (ranges::forward_range<Base>) {
            assert(ranges::equal(r.front(), *ranges::begin(expected)));
        }

        static_assert(CanMemberFront<const R> == ranges::forward_range<const remove_cvref_t<Base>>);
        if constexpr (CanMemberFront<const R>) {
            assert(ranges::equal(as_const(r).front(), *ranges::begin(expected)));
        }

        static_assert(!CanMemberBack<R>);
        static_assert(!CanMemberBack<const R>);
    }

    // Validate lazy_split_view::base() const&
    static_assert(CanMemberBase<const R&> == copy_constructible<views::all_t<Base>>);
    if constexpr (CanMemberBase<const R&> && ranges::forward_range<Base>) {
        same_as<views::all_t<Base>> auto b1 = as_const(r).base();
        static_assert(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<views::all_t<Base>>);
        if (!is_empty) {
            assert(*b1.begin() == *ranges::begin(*ranges::begin(expected)));
        }
    }

    // Validate lazy_split_view::base() && (NB: do this last since it leaves r moved-from)
    if (ranges::forward_range<views::all_t<Base>>) { // intentionally not if constexpr
        same_as<views::all_t<Base>> auto b2 = move(r).base();
        static_assert(noexcept(move(r).base()) == is_nothrow_move_constructible_v<views::all_t<Base>>);
        if (!is_empty) {
            assert(*b2.begin() == *ranges::begin(*ranges::begin(expected)));
        }
    }
}

struct instantiator {
    static constexpr string_view expected_single[] = {
        "This"sv, "is"sv, "a"sv, "test,"sv, "this"sv, "is"sv, "only"sv, "a"sv, "test."sv};
    static constexpr string_view expected_range[]    = {"Th"sv, " "sv, " a test, th"sv, " "sv, " only a test."sv};
    static constexpr string_view expected_empty[]    = {"T"sv, "h"sv, "i"sv, "s"sv, " "sv, "i"sv, "s"sv, " "sv, "a"sv,
           " "sv, "t"sv, "e"sv, "s"sv, "t"sv, ","sv, " "sv, "t"sv, "h"sv, "i"sv, "s"sv, " "sv, "i"sv, "s"sv, " "sv, //
           "o"sv, "n"sv, "l"sv, "y"sv, " "sv, "a"sv, " "sv, "t"sv, "e"sv, "s"sv, "t"sv, "."sv};
    static constexpr string_view expected_trailing[] = {"test"sv, ""sv};
    static constexpr string_view expected_lwg3505[]  = {"x"sv, "x"sv, "x"sv};

    template <class T>
    static constexpr decltype(auto) move_if_needed(T& t) noexcept {
        if constexpr (ranges::view<T> && !copyable<T>) {
            return move(t);
        } else {
            return t;
        }
    }

    template <ranges::input_range Read>
    static constexpr void call() {
        { // Single-element delimiter
            Read read{span{text}};
            test_one(move_if_needed(read), ' ', expected_single);

            Read empty{span<const char, 0>{}};
            test_one(move_if_needed(empty), ' ', views::empty<string_view>);
        }
        { // Empty delimiter
            Read read{span{text}};
            test_one(move_if_needed(read), views::empty<char>, expected_empty);

            Read empty{span<const char, 0>{}};
            test_one(move_if_needed(empty), views::empty<char>, views::empty<string_view>);
        }
        if constexpr (ranges::forward_range<Read>) { // Range delimiter
            Read read{span{text}};
            test_one(move_if_needed(read), "is"sv, expected_range);

            Read empty{span<const char, 0>{}};
            test_one(move_if_needed(empty), "is"sv, views::empty<string_view>);
        }

        { // LWG-3478, trailing empty range
            Read read{span{trailing_empty}};
            test_one(move_if_needed(read), ' ', expected_trailing);
        }

        if constexpr (ranges::forward_range<Read>) { // LWG-3505, multichar pattern
            Read read{span{lwg3505_pattern}};
            test_one(move_if_needed(read), "xy"sv, expected_lwg3505);
        }
    }
};

template <class Category, test::Common IsCommon, test::CanView IsView, test::Copyability CanCopy>
using test_range = test::range<Category, const char, test::Sized::no,
    test::CanDifference{derived_from<Category, random_access_iterator_tag>}, IsCommon,
    test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == test::Common::yes},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, IsView, CanCopy>;

constexpr bool instantiation_test() {
    using test::CanView, test::Common, test::Copyability;

    // The view is sensitive to:
    // 1. Category of the range to be lazy_split (input, forward)
    // 2. Copyability
    // 3. Commonality
    // 4. Length of delimiter pattern (0/static 1/dynamic) [covered in instantiator::call]

    instantiator::call<test_range<input_iterator_tag, Common::no, CanView::no, Copyability::immobile>>();
    instantiator::call<test_range<input_iterator_tag, Common::no, CanView::yes, Copyability::move_only>>();
    instantiator::call<test_range<input_iterator_tag, Common::no, CanView::yes, Copyability::copyable>>();

    instantiator::call<test_range<input_iterator_tag, Common::yes, CanView::no, Copyability::immobile>>();
    instantiator::call<test_range<input_iterator_tag, Common::yes, CanView::yes, Copyability::move_only>>();
    instantiator::call<test_range<input_iterator_tag, Common::yes, CanView::yes, Copyability::copyable>>();

    instantiator::call<test_range<forward_iterator_tag, Common::no, CanView::no, Copyability::immobile>>();
    instantiator::call<test_range<forward_iterator_tag, Common::no, CanView::yes, Copyability::move_only>>();
    instantiator::call<test_range<forward_iterator_tag, Common::no, CanView::yes, Copyability::copyable>>();

    instantiator::call<test_range<forward_iterator_tag, Common::yes, CanView::no, Copyability::immobile>>();
    instantiator::call<test_range<forward_iterator_tag, Common::yes, CanView::yes, Copyability::move_only>>();
    instantiator::call<test_range<forward_iterator_tag, Common::yes, CanView::yes, Copyability::copyable>>();

    return true;
}

constexpr bool test_lwg_3904() {
    auto r = views::single(0) | views::lazy_split(0);
    auto i = r.begin();
    ++i;
    decltype(as_const(r).begin()) j = i;
    return j != r.end();
}

void test_lwg_4027() { // COMPILE-ONLY
    auto r   = views::single(0) | views::lazy_split(0);
    using R1 = decltype((*ranges::cbegin(r)).front());
    using R2 = decltype((*cbegin(r)).front());
    static_assert(same_as<R1, R2>);
    static_assert(is_const_v<remove_reference_t<R1>>);
}

int main() {
    static_assert(instantiation_test());
    instantiation_test();

    static_assert(test_lwg_3904());
    assert(test_lwg_3904());
}
