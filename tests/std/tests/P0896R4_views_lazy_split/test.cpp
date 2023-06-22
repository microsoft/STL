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
    typename delimiter_view_impl<is_convertible_v<Delimiter, ranges::range_value_t<Base>>>::template apply<Base,
        Delimiter>;

template <ranges::input_range Base, class Delimiter, ranges::input_range Expected>
constexpr void test_one(Base&& base, Delimiter&& delimiter, Expected&& expected) {
    STATIC_ASSERT(CanViewLazySplit<Base, Delimiter>);
    using R = decltype(views::lazy_split(forward<Base>(base), forward<Delimiter>(delimiter)));

    // Validate type properties
    STATIC_ASSERT(ranges::view<R>);
    STATIC_ASSERT(ranges::input_range<R>);
    STATIC_ASSERT(ranges::forward_range<R> == ranges::forward_range<Base>);
    STATIC_ASSERT(!ranges::bidirectional_range<R>);

    // Validate range adaptor object and range adaptor closure
    using DV           = delimiter_view_t<Base, Delimiter>;
    const auto closure = views::lazy_split(delimiter);

    constexpr bool is_view = ranges::view<remove_cvref_t<Base>>;

    // ... with lvalue argument
    STATIC_ASSERT(CanViewLazySplit<Base&, Delimiter&> == (!is_view || copy_constructible<remove_cvref_t<Base>>) );
    if constexpr (CanViewLazySplit<Base&, Delimiter&>) { // Validate lvalue
        constexpr bool is_noexcept =
            (!is_view || is_nothrow_copy_constructible_v<views::all_t<Base&>>) &&is_nothrow_copy_constructible_v<DV>;

        STATIC_ASSERT(same_as<decltype(views::lazy_split(base, delimiter)), R>);
        STATIC_ASSERT(noexcept(views::lazy_split(base, delimiter)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(base | closure), R>);
        STATIC_ASSERT(noexcept(base | closure) == is_noexcept);
    }

    // ... with const lvalue argument
    STATIC_ASSERT(CanViewLazySplit<const remove_reference_t<Base>&, Delimiter&>
                  == (!is_view || copy_constructible<remove_cvref_t<Base>>) );
    if constexpr (is_view && copy_constructible<remove_cvref_t<Base>>) {
        constexpr bool is_noexcept =
            is_nothrow_copy_constructible_v<remove_cvref_t<Base>> && is_nothrow_copy_constructible_v<DV>;

        STATIC_ASSERT(same_as<decltype(views::lazy_split(as_const(base), delimiter)), R>);
        STATIC_ASSERT(noexcept(views::lazy_split(as_const(base), delimiter)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(base) | closure), R>);
        STATIC_ASSERT(noexcept(as_const(base) | closure) == is_noexcept);
    } else if constexpr (!is_view) {
        using LSV                  = ranges::lazy_split_view<ranges::ref_view<const remove_reference_t<Base>>, DV>;
        constexpr bool is_noexcept = is_nothrow_constructible_v<LSV, const remove_reference_t<Base>&, Delimiter&>;

        STATIC_ASSERT(same_as<decltype(views::lazy_split(as_const(base), delimiter)), LSV>);
        STATIC_ASSERT(noexcept(views::lazy_split(as_const(base), delimiter)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(base) | closure), LSV>);
        STATIC_ASSERT(noexcept(as_const(base) | closure) == is_noexcept);
    }

    // ... with rvalue argument
    STATIC_ASSERT(
        CanViewLazySplit<remove_reference_t<Base>, Delimiter&> == (is_view || movable<remove_reference_t<Base>>) );
    if constexpr (is_view) {
        constexpr bool is_noexcept =
            is_nothrow_move_constructible_v<remove_reference_t<Base>> && is_nothrow_copy_constructible_v<DV>;
        STATIC_ASSERT(same_as<decltype(views::lazy_split(move(base), delimiter)), R>);
        STATIC_ASSERT(noexcept(views::lazy_split(move(base), delimiter)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(base) | closure), R>);
        STATIC_ASSERT(noexcept(move(base) | closure) == is_noexcept);
    } else if constexpr (movable<remove_reference_t<Base>>) {
        using RS = ranges::lazy_split_view<ranges::owning_view<remove_reference_t<Base>>, DV>;
        constexpr bool is_noexcept =
            is_nothrow_move_constructible_v<remove_reference_t<Base>> && is_nothrow_copy_constructible_v<DV>;

        STATIC_ASSERT(same_as<decltype(views::lazy_split(move(base), delimiter)), RS>);
        STATIC_ASSERT(noexcept(views::lazy_split(move(base), delimiter)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(base) | closure), RS>);
        STATIC_ASSERT(noexcept(move(base) | closure) == is_noexcept);
    }

    // ... with const rvalue argument
    STATIC_ASSERT(CanViewLazySplit<const remove_reference_t<Base>, Delimiter&>
                  == (is_view && copy_constructible<remove_cvref_t<Base>>) );
    if constexpr (is_view && copy_constructible<remove_cvref_t<Base>>) {
        constexpr bool is_noexcept =
            is_nothrow_copy_constructible_v<remove_cvref_t<Base>> && is_nothrow_copy_constructible_v<DV>;

        STATIC_ASSERT(same_as<decltype(views::lazy_split(move(as_const(base)), delimiter)), R>);
        STATIC_ASSERT(noexcept(views::lazy_split(move(as_const(base)), delimiter)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(base)) | closure), R>);
        STATIC_ASSERT(noexcept(move(as_const(base)) | closure) == is_noexcept);
    }

    // Validate deduction guide
    same_as<R> auto r = ranges::lazy_split_view{forward<Base>(base), forward<Delimiter>(delimiter)};
    assert(ranges::equal(r, expected, equal_ranges));

    // Validate view_interface::empty and operator bool
    const bool is_empty = ranges::empty(expected);
    STATIC_ASSERT(CanMemberEmpty<R> == ranges::forward_range<Base>);
    STATIC_ASSERT(CanBool<R> == ranges::forward_range<Base>);
    if constexpr (ranges::forward_range<Base>) {
        assert(r.empty() == is_empty);
        assert(static_cast<bool>(r) == !is_empty);
    }

    // Validate lazy_split_view::begin
    STATIC_ASSERT(CanMemberBegin<R>);
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

        STATIC_ASSERT(CanMemberBegin<const R> == ranges::forward_range<const remove_reference_t<Base>>);
        STATIC_ASSERT(CanBegin<const R&> == CanMemberBegin<const R>);
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
    STATIC_ASSERT(CanMemberEnd<R>);
    [[maybe_unused]] same_as<ranges::sentinel_t<R>> auto s = r.end();
    if (ranges::forward_range<Base>) {
        assert((r.begin() == s) == is_empty);
    }
    STATIC_ASSERT(ranges::common_range<R> == (ranges::forward_range<Base> && ranges::common_range<Base>) );
    if constexpr (!ranges::common_range<R>) {
        STATIC_ASSERT(same_as<ranges::sentinel_t<R>, default_sentinel_t>);
    }

    STATIC_ASSERT(CanMemberEnd<const R>);
    // clang-format off
    constexpr bool should_be_const_common = ranges::forward_range<Base>
        && ranges::forward_range<const remove_cvref_t<Base>> && ranges::common_range<const remove_cvref_t<Base>>;
    // clang-format on
    STATIC_ASSERT(ranges::common_range<const R> == should_be_const_common);
    const auto sc = as_const(r).end();
    if constexpr (ranges::forward_range<Base> && ranges::forward_range<const remove_cvref_t<Base>>) {
        STATIC_ASSERT(same_as<decltype(sc), const ranges::sentinel_t<const R>>);
        assert((as_const(r).begin() == sc) == is_empty);
    }

    if constexpr (!ranges::common_range<const R>) {
        STATIC_ASSERT(same_as<ranges::sentinel_t<R>, default_sentinel_t>);
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

    if (!is_empty) {
        // Validate view_interface::front and back
        STATIC_ASSERT(CanMemberFront<R> == ranges::forward_range<Base>);
        if constexpr (ranges::forward_range<Base>) {
            assert(ranges::equal(r.front(), *ranges::begin(expected)));
        }

        STATIC_ASSERT(CanMemberFront<const R> == ranges::forward_range<const remove_cvref_t<Base>>);
        if constexpr (CanMemberFront<const R>) {
            assert(ranges::equal(as_const(r).front(), *ranges::begin(expected)));
        }

        STATIC_ASSERT(!CanMemberBack<R>);
        STATIC_ASSERT(!CanMemberBack<const R>);
    }

    // Validate lazy_split_view::base() const&
    STATIC_ASSERT(CanMemberBase<const R&> == copy_constructible<views::all_t<Base>>);
    if constexpr (CanMemberBase<const R&> && ranges::forward_range<Base>) {
        same_as<views::all_t<Base>> auto b1 = as_const(r).base();
        STATIC_ASSERT(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<views::all_t<Base>>);
        if (!is_empty) {
            assert(*b1.begin() == *ranges::begin(*ranges::begin(expected)));
        }
    }

    // Validate lazy_split_view::base() && (NB: do this last since it leaves r moved-from)
    if (ranges::forward_range<views::all_t<Base>>) { // intentionally not if constexpr
        same_as<views::all_t<Base>> auto b2 = move(r).base();
        STATIC_ASSERT(noexcept(move(r).base()) == is_nothrow_move_constructible_v<views::all_t<Base>>);
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

int main() {
    STATIC_ASSERT(instantiation_test());
    instantiation_test();

    STATIC_ASSERT(test_lwg_3904());
    assert(test_lwg_3904());
}
