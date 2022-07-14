// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <charconv>
#include <ranges>
#include <span>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>
using namespace std;

template <class Rng, class Delimiter>
concept CanViewSplit = requires(Rng&& r, Delimiter&& d) {
    views::split(forward<Rng>(r), forward<Delimiter>(d));
};

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

template <ranges::forward_range Base, class Delimiter, ranges::forward_range Expected>
constexpr void test_one(Base&& base, Delimiter&& delimiter, Expected&& expected) {
    STATIC_ASSERT(CanViewSplit<Base, Delimiter>);
    using R = decltype(views::split(forward<Base>(base), forward<Delimiter>(delimiter)));

    // Validate type properties
    STATIC_ASSERT(ranges::view<R>);
    STATIC_ASSERT(ranges::input_range<R>);
    STATIC_ASSERT(ranges::forward_range<R> == ranges::forward_range<Base>);
    STATIC_ASSERT(!ranges::bidirectional_range<R>);

    // Validate range adaptor object and range adaptor closure
    using DV           = delimiter_view_t<Base, Delimiter>;
    const auto closure = views::split(delimiter);

    constexpr bool is_view = ranges::view<remove_cvref_t<Base>>;

    // ... with lvalue argument
    STATIC_ASSERT(CanViewSplit<Base&, Delimiter&> == (!is_view || copy_constructible<remove_cvref_t<Base>>) );
    if constexpr (CanViewSplit<Base&, Delimiter&>) { // Validate lvalue
        constexpr bool is_noexcept =
            (!is_view || is_nothrow_copy_constructible_v<views::all_t<Base&>>) &&is_nothrow_copy_constructible_v<DV>;

        STATIC_ASSERT(same_as<decltype(views::split(base, delimiter)), R>);
        STATIC_ASSERT(noexcept(views::split(base, delimiter)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(base | closure), R>);
        STATIC_ASSERT(noexcept(base | closure) == is_noexcept);
    }

    // ... with const lvalue argument
    STATIC_ASSERT(CanViewSplit<const remove_reference_t<Base>&,
                      Delimiter&> == (!is_view || copy_constructible<remove_cvref_t<Base>>) );
    if constexpr (is_view && copy_constructible<remove_cvref_t<Base>>) {
        constexpr bool is_noexcept =
            is_nothrow_copy_constructible_v<remove_cvref_t<Base>> && is_nothrow_copy_constructible_v<DV>;

        STATIC_ASSERT(same_as<decltype(views::split(as_const(base), delimiter)), R>);
        STATIC_ASSERT(noexcept(views::split(as_const(base), delimiter)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(base) | closure), R>);
        STATIC_ASSERT(noexcept(as_const(base) | closure) == is_noexcept);
    } else if constexpr (!is_view) {
        using RC                   = ranges::split_view<ranges::ref_view<const remove_reference_t<Base>>, DV>;
        constexpr bool is_noexcept = is_nothrow_constructible_v<RC, const remove_reference_t<Base>&, Delimiter&>;

        STATIC_ASSERT(same_as<decltype(views::split(as_const(base), delimiter)), RC>);
        STATIC_ASSERT(noexcept(views::split(as_const(base), delimiter)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(as_const(base) | closure), RC>);
        STATIC_ASSERT(noexcept(as_const(base) | closure) == is_noexcept);
    }

    // ... with rvalue argument
    STATIC_ASSERT(
        CanViewSplit<remove_reference_t<Base>, Delimiter&> == (is_view || movable<remove_reference_t<Base>>) );
    if constexpr (is_view) {
        constexpr bool is_noexcept =
            is_nothrow_move_constructible_v<remove_reference_t<Base>> && is_nothrow_copy_constructible_v<DV>;
        STATIC_ASSERT(same_as<decltype(views::split(move(base), delimiter)), R>);
        STATIC_ASSERT(noexcept(views::split(move(base), delimiter)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(base) | closure), R>);
        STATIC_ASSERT(noexcept(move(base) | closure) == is_noexcept);
    } else if constexpr (movable<remove_cvref_t<Base>>) {
        using RS = ranges::split_view<ranges::owning_view<remove_reference_t<Base>>, DV>;
        constexpr bool is_noexcept =
            is_nothrow_move_constructible_v<remove_reference_t<Base>> && is_nothrow_copy_constructible_v<DV>;

        STATIC_ASSERT(same_as<decltype(views::split(move(base), delimiter)), RS>);
        STATIC_ASSERT(noexcept(views::split(move(base), delimiter)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(base) | closure), RS>);
        STATIC_ASSERT(noexcept(move(base) | closure) == is_noexcept);
    }

    // ... with const rvalue argument
    STATIC_ASSERT(CanViewSplit<const remove_reference_t<Base>,
                      Delimiter&> == (is_view && copy_constructible<remove_cvref_t<Base>>) );
    if constexpr (is_view && copy_constructible<remove_cvref_t<Base>>) {
        constexpr bool is_noexcept =
            is_nothrow_copy_constructible_v<remove_cvref_t<Base>> && is_nothrow_copy_constructible_v<DV>;

        STATIC_ASSERT(same_as<decltype(views::split(move(as_const(base)), delimiter)), R>);
        STATIC_ASSERT(noexcept(views::split(move(as_const(base)), delimiter)) == is_noexcept);

        STATIC_ASSERT(same_as<decltype(move(as_const(base)) | closure), R>);
        STATIC_ASSERT(noexcept(move(as_const(base)) | closure) == is_noexcept);
    }

    // Validate deduction guide
    same_as<R> auto r = ranges::split_view{forward<Base>(base), forward<Delimiter>(delimiter)};
    assert(ranges::equal(r, expected, equal_ranges));

    // Validate view_interface::empty and operator bool
    const bool is_empty = ranges::empty(expected);
    STATIC_ASSERT(CanMemberEmpty<R> == ranges::forward_range<Base>);
    STATIC_ASSERT(CanBool<R> == ranges::forward_range<Base>);
    if constexpr (ranges::forward_range<Base>) {
        assert(r.empty() == is_empty);
        assert(static_cast<bool>(r) == !is_empty);
    }

    // Validate split_view::begin
    STATIC_ASSERT(CanMemberBegin<R>);
    STATIC_ASSERT(!CanBegin<const R&>);
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

    // Validate split_view::end
    STATIC_ASSERT(CanMemberEnd<R>);
    STATIC_ASSERT(!CanEnd<const R&>);
    STATIC_ASSERT(ranges::common_range<R> == ranges::common_range<Base>);
    same_as<ranges::sentinel_t<R>> auto s = r.end();
    assert((r.begin() == s) == is_empty);

    // Validate view_interface::data
    STATIC_ASSERT(!CanData<R>);
    STATIC_ASSERT(!CanData<const R>);

    // Validate view_interface::size
    STATIC_ASSERT(!CanSize<R>);
    STATIC_ASSERT(!CanSize<const R>);

    // Validate view_interface::operator[]
    STATIC_ASSERT(!CanIndex<R>);
    STATIC_ASSERT(!CanIndex<const R>);

    // Validate view_interface::front
    STATIC_ASSERT(CanMemberFront<R>);
    STATIC_ASSERT(!CanMemberFront<const R>);
    if (!is_empty) {
        assert(ranges::equal(r.front(), *ranges::begin(expected)));
    }

    // Validate view_interface::back
    STATIC_ASSERT(!CanMemberBack<R>);
    STATIC_ASSERT(!CanMemberBack<const R>);

    // Validate split_view::base() const&
    STATIC_ASSERT(CanMemberBase<const R&> == copy_constructible<views::all_t<Base>>);
    if constexpr (CanMemberBase<const R&>) {
        same_as<views::all_t<Base>> auto b1 = as_const(r).base();
        STATIC_ASSERT(noexcept(as_const(r).base()) == is_nothrow_copy_constructible_v<views::all_t<Base>>);
        if (!is_empty) {
            assert(*b1.begin() == *ranges::begin(*ranges::begin(expected)));
        }
    }

    // Validate split_view::base() && (NB: do this last since it leaves r moved-from)
    same_as<views::all_t<Base>> auto b2 = move(r).base();
    STATIC_ASSERT(noexcept(move(r).base()) == is_nothrow_move_constructible_v<views::all_t<Base>>);
    if (!is_empty) {
        assert(*b2.begin() == *ranges::begin(*ranges::begin(expected)));
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

    template <ranges::forward_range Read>
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
        { // Range delimiter
            Read read{span{text}};
            test_one(move_if_needed(read), "is"sv, expected_range);

            Read empty{span<const char, 0>{}};
            test_one(move_if_needed(empty), "is"sv, views::empty<string_view>);
        }

        { // LWG-3478, trailing empty range
            Read read{span{trailing_empty}};
            test_one(move_if_needed(read), ' ', expected_trailing);
        }

        { // LWG-3505, multichar pattern
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
    // 1. Copyability
    // 2. Commonality
    // 3. Length of delimiter pattern (0/static 1/dynamic) [covered in instantiator::call]

    instantiator::call<test_range<forward_iterator_tag, Common::no, CanView::no, Copyability::immobile>>();
    instantiator::call<test_range<forward_iterator_tag, Common::no, CanView::yes, Copyability::move_only>>();
    instantiator::call<test_range<forward_iterator_tag, Common::no, CanView::yes, Copyability::copyable>>();

    instantiator::call<test_range<forward_iterator_tag, Common::yes, CanView::no, Copyability::immobile>>();
    instantiator::call<test_range<forward_iterator_tag, Common::yes, CanView::yes, Copyability::move_only>>();
    instantiator::call<test_range<forward_iterator_tag, Common::yes, CanView::yes, Copyability::copyable>>();

    { // ensure we get something contiguous
        for (ranges::contiguous_range auto sv : "127..0..0..1"sv | views::split(".."sv)) {
            assert(!sv.empty());
        }
    }

    if (!is_constant_evaluated()) { // test the from_chars example
        auto ip    = "1.2.3.4"sv;
        auto parts = ip | views::split('.') | views::transform([](span<const char> s) {
            int i             = 0;
            const auto result = from_chars(s.data(), s.data() + s.size(), i);
            assert(result.ec == errc{});
            return i;
        });

        int expected = 1;
        for (auto value : parts) {
            assert(value == expected++);
        }
    }

    return true;
}

constexpr bool test_devcom_1559808() {
    // Regression test for DevCom-1559808, an interaction between vector and the
    // use of structured bindings in the constexpr evaluator.

    vector<char> letters{'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 't', 'e', 's', 't'};
    auto r = views::split(letters, ' ');
    auto i = r.begin();
    assert(*(*i).begin() == 'T');
    ++i;
    assert(*(*i).begin() == 'i');
    ++i;
    assert(*(*i).begin() == 'a');
    ++i;
    assert(*(*i).begin() == 't');
    ++i;
    assert(i == r.end());

    return true;
}

constexpr bool test_LWG_3590() {
    // LWG-3590: "split_view::base() const & is overconstrained"
    struct weird_view : ranges::view_interface<weird_view> {
        weird_view()                        = default;
        weird_view(const weird_view&)       = default;
        weird_view& operator=(weird_view&&) = default;

        constexpr const int* begin() const {
            return &inner;
        }
        constexpr const int* end() const {
            return &inner + 1;
        }

        int inner = 42;
    };
    STATIC_ASSERT(ranges::view<weird_view>);
    STATIC_ASSERT(copy_constructible<weird_view>);
    STATIC_ASSERT(!copyable<weird_view>);

    const auto r                                         = views::split(weird_view{}, 0);
    [[maybe_unused]] const same_as<weird_view> auto copy = r.base();

    return true;
}

int main() {
    STATIC_ASSERT(instantiation_test());
    instantiation_test();

#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-1516290
    STATIC_ASSERT(test_devcom_1559808());
#endif // TRANSITION, DevCom-1516290
    test_devcom_1559808();

    STATIC_ASSERT(test_LWG_3590());
    assert(test_LWG_3590());
}
