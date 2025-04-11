// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <deque>
#include <filesystem>
#include <forward_list>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <ranges>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <vector>

#include <range_algorithm_support.hpp>

// Note that many tests herein assume:
static_assert(std::same_as<std::make_unsigned_t<std::ptrdiff_t>, std::size_t>);

// GH-2358: <filesystem>: path's comparison operators are IF-NDR
static_assert(ranges::range<std::filesystem::path>);
static_assert(ranges::range<const std::filesystem::path>);

template <class T>
concept Decayed = std::same_as<std::decay_t<T>, T>;

template <class R>
concept CanSizeType = requires { typename ranges::range_size_t<R>; };

struct invalid_type {};

template <class T>
constexpr bool is_valid = !std::same_as<T, invalid_type>;

// Validate properties common to all Customization Point Objects (CPOs)
template <class T>
constexpr bool test_cpo(T const& obj) {
    static_assert(std::semiregular<T>);

    // Not required, but likely portable nonetheless:
    static_assert(std::is_empty_v<T>);
    static_assert(std::is_trivially_copyable_v<T>);
    static_assert(std::is_nothrow_destructible_v<T>);
    static_assert(std::is_nothrow_default_constructible_v<T>);
    static_assert(std::is_trivially_default_constructible_v<T>);
    static_assert(std::is_nothrow_copy_constructible_v<T>);
    static_assert(std::is_trivially_copy_constructible_v<T>);
    static_assert(std::is_nothrow_move_constructible_v<T>);
    static_assert(std::is_trivially_move_constructible_v<T>);
    static_assert(std::is_nothrow_copy_assignable_v<T>);
    static_assert(std::is_trivially_copy_assignable_v<T>);
    static_assert(std::is_nothrow_move_assignable_v<T>);
    static_assert(std::is_trivially_move_assignable_v<T>);

    // Not required to be constant expressions, but likely portable nonetheless:
    [[maybe_unused]] T value_initialized{};
    T copy_constructed = obj;
    T move_constructed = std::move(copy_constructed);
    copy_constructed   = std::move(move_constructed);
    move_constructed   = copy_constructed;

    return true;
}

static_assert(test_cpo(std::strong_order));
static_assert(test_cpo(std::weak_order));
static_assert(test_cpo(std::partial_order));
static_assert(test_cpo(std::compare_strong_order_fallback));
static_assert(test_cpo(std::compare_weak_order_fallback));
static_assert(test_cpo(std::compare_partial_order_fallback));

static_assert(test_cpo(ranges::swap));
static_assert(test_cpo(ranges::iter_swap));
static_assert(test_cpo(ranges::iter_move));
static_assert(test_cpo(ranges::begin));
static_assert(test_cpo(ranges::end));
static_assert(test_cpo(ranges::cbegin));
static_assert(test_cpo(ranges::cend));
static_assert(test_cpo(ranges::rbegin));
static_assert(test_cpo(ranges::rend));
static_assert(test_cpo(ranges::crbegin));
static_assert(test_cpo(ranges::crend));
static_assert(test_cpo(ranges::size));
static_assert(test_cpo(ranges::ssize));
static_assert(test_cpo(ranges::empty));
static_assert(test_cpo(ranges::data));
static_assert(test_cpo(ranges::cdata));

static_assert(test_cpo(ranges::views::all));
static_assert(test_cpo(ranges::views::common));
static_assert(test_cpo(ranges::views::counted));
static_assert(test_cpo(ranges::views::drop));
static_assert(test_cpo(ranges::views::drop_while));
static_assert(test_cpo(ranges::views::elements<42>));
static_assert(test_cpo(ranges::views::filter));
static_assert(test_cpo(ranges::views::iota));
static_assert(test_cpo(ranges::views::istream<int>));
static_assert(test_cpo(ranges::views::istream<double>));
static_assert(test_cpo(ranges::views::join));
static_assert(test_cpo(ranges::views::keys));
static_assert(test_cpo(ranges::views::lazy_split));
static_assert(test_cpo(ranges::views::reverse));
static_assert(test_cpo(ranges::views::single));
static_assert(test_cpo(ranges::views::split));
static_assert(test_cpo(ranges::views::take));
static_assert(test_cpo(ranges::views::take_while));
static_assert(test_cpo(ranges::views::transform));
static_assert(test_cpo(ranges::views::values));

#if _HAS_CXX23
static_assert(test_cpo(ranges::views::adjacent<3>));
static_assert(test_cpo(ranges::views::adjacent_transform<3>));
static_assert(test_cpo(ranges::views::as_const));
static_assert(test_cpo(ranges::views::as_rvalue));
static_assert(test_cpo(ranges::views::cartesian_product));
static_assert(test_cpo(ranges::views::chunk));
static_assert(test_cpo(ranges::views::chunk_by));
static_assert(test_cpo(ranges::views::enumerate));
static_assert(test_cpo(ranges::views::join_with));
static_assert(test_cpo(ranges::views::pairwise));
static_assert(test_cpo(ranges::views::pairwise_transform));
static_assert(test_cpo(ranges::views::repeat));
static_assert(test_cpo(ranges::views::slide));
static_assert(test_cpo(ranges::views::stride));
static_assert(test_cpo(ranges::views::zip));
static_assert(test_cpo(ranges::views::zip_transform));
#endif // _HAS_CXX23

void test_cpo_ambiguity() {
    using namespace std::ranges;

    // This type is *really* associated with namespace std:
    std::vector<std::reverse_iterator<int*>> vri{};

    // Verify that unqualified lookup is unambiguous:
    (void) begin(vri);
    (void) end(vri);
    (void) cbegin(vri);
    (void) cend(vri);
    (void) rbegin(vri);
    (void) rend(vri);
    (void) crbegin(vri);
    (void) crend(vri);
    (void) size(vri);
    (void) ssize(vri);
    (void) empty(vri);
    (void) data(vri);
    (void) cdata(vri);
}

template <class Range, class Iterator = invalid_type>
constexpr bool test_begin() {
    // Validate ranges::begin(std::declval<Range>()) and ranges::iterator_t<Range>
    static_assert(Decayed<Iterator>);
    static_assert(!is_valid<Iterator> || std::input_or_output_iterator<Iterator>);

    static_assert(CanBegin<Range> == is_valid<Iterator>);
    if constexpr (is_valid<Iterator>) {
        static_assert(std::same_as<decltype(ranges::begin(std::declval<Range>())), Iterator>);
        static_assert(std::same_as<ranges::iterator_t<Range>, Iterator>);
        static_assert(std::same_as<ranges::iterator_t<Range&>, Iterator>);
        static_assert(std::same_as<ranges::iterator_t<Range&&>, Iterator>);
    } else {
        static_assert(!CanCBegin<Range>);
    }

    return true;
}

template <class Range, class Iterator = invalid_type>
constexpr bool test_cbegin() {
    // Validate ranges::cbegin
    static_assert(Decayed<Iterator>);
    static_assert(!is_valid<Iterator> || std::input_or_output_iterator<Iterator>);

    static_assert(CanCBegin<Range> == is_valid<Iterator>);
    if constexpr (is_valid<Iterator>) {
        static_assert(std::same_as<decltype(ranges::cbegin(std::declval<Range>())), Iterator>);

        static_assert(CanBegin<Range>);
    }

    return true;
}

template <class Range, class Iterator = invalid_type>
constexpr bool test_rbegin() {
    // Validate ranges::rbegin
    static_assert(Decayed<Iterator>);
    static_assert(!is_valid<Iterator> || std::input_or_output_iterator<Iterator>);

    static_assert(CanRBegin<Range> == is_valid<Iterator>);
    if constexpr (is_valid<Iterator>) {
        static_assert(std::same_as<decltype(ranges::rbegin(std::declval<Range>())), Iterator>);
    } else {
        static_assert(!CanCRBegin<Range>);
    }

    return true;
}

template <class Range, class Iterator = invalid_type>
constexpr bool test_crbegin() {
    // Validate ranges::crbegin
    static_assert(Decayed<Iterator>);
    static_assert(!is_valid<Iterator> || std::input_or_output_iterator<Iterator>);

    static_assert(CanCRBegin<Range> == is_valid<Iterator>);
    if constexpr (is_valid<Iterator>) {
        static_assert(std::same_as<decltype(ranges::crbegin(std::declval<Range>())), Iterator>);

        static_assert(CanRBegin<Range>);
    }

    return true;
}

template <class Range, class Sentinel = invalid_type>
constexpr bool test_end() {
    // Validate ranges::end and ranges::sentinel_t
    static_assert(Decayed<Sentinel>);
    static_assert(std::semiregular<Sentinel>);

    static_assert(CanEnd<Range> == is_valid<Sentinel>);
    if constexpr (is_valid<Sentinel>) {
        static_assert(std::same_as<decltype(ranges::end(std::declval<Range>())), Sentinel>);
        static_assert(std::same_as<ranges::sentinel_t<Range>, Sentinel>);
        static_assert(std::same_as<ranges::sentinel_t<Range&>, Sentinel>);
        static_assert(std::same_as<ranges::sentinel_t<Range&&>, Sentinel>);

        using Iterator = decltype(ranges::begin(std::declval<Range>()));
        static_assert(std::sentinel_for<Sentinel, Iterator>);

        // Validate ranges::range and ranges::common_range
        static_assert(ranges::range<Range>);
        static_assert(ranges::common_range<Range> == std::same_as<Iterator, Sentinel>);

        // Validate ranges::range_difference_t and ranges::range_reference_t
        using Difference = std::iter_difference_t<Iterator>;
        static_assert(std::same_as<ranges::range_difference_t<Range>, Difference>);
        static_assert(std::same_as<ranges::range_difference_t<Range&>, Difference>);
        static_assert(std::same_as<ranges::range_difference_t<Range&&>, Difference>);

        using Reference = std::iter_reference_t<Iterator>;
        static_assert(std::same_as<ranges::range_reference_t<Range>, Reference>);
        static_assert(std::same_as<ranges::range_reference_t<Range&>, Reference>);
        static_assert(std::same_as<ranges::range_reference_t<Range&&>, Reference>);
    } else {
        static_assert(!CanCEnd<Range>);
    }

    return true;
}

template <class Range, class Sentinel = invalid_type>
constexpr bool test_cend() {
    // Validate ranges::cend
    static_assert(Decayed<Sentinel>);
    static_assert(std::semiregular<Sentinel>);

    static_assert(CanCEnd<Range> == is_valid<Sentinel>);
    if constexpr (is_valid<Sentinel>) {
        static_assert(std::same_as<decltype(ranges::cend(std::declval<Range>())), Sentinel>);

        using Iterator = decltype(ranges::cbegin(std::declval<Range>()));
        static_assert(std::sentinel_for<Sentinel, Iterator>);

        static_assert(CanEnd<Range>);
    }

    return true;
}

template <class Range, class Sentinel = invalid_type>
constexpr bool test_rend() {
    // Validate ranges::rend
    static_assert(Decayed<Sentinel>);
    static_assert(std::semiregular<Sentinel>);

    static_assert(CanREnd<Range> == is_valid<Sentinel>);
    if constexpr (is_valid<Sentinel>) {
        static_assert(std::same_as<decltype(ranges::rend(std::declval<Range>())), Sentinel>);

        using Iterator = decltype(ranges::rbegin(std::declval<Range>()));
        static_assert(std::sentinel_for<Sentinel, Iterator>);
    } else {
        static_assert(!CanCREnd<Range>);
    }

    return true;
}

template <class Range, class Sentinel = invalid_type>
constexpr bool test_crend() {
    // Validate ranges::crend
    static_assert(Decayed<Sentinel>);
    static_assert(std::semiregular<Sentinel>);

    static_assert(CanCREnd<Range> == is_valid<Sentinel>);
    if constexpr (is_valid<Sentinel>) {
        static_assert(std::same_as<decltype(ranges::crend(std::declval<Range>())), Sentinel>);

        using Iterator = decltype(ranges::crbegin(std::declval<Range>()));
        static_assert(std::sentinel_for<Sentinel, Iterator>);

        static_assert(CanREnd<Range>);
    }

    return true;
}

template <class Range, bool Emptyable>
constexpr bool test_empty() {
    // Validate ranges::empty
    static_assert(CanEmpty<Range> == Emptyable);
    if constexpr (Emptyable) {
        static_assert(std::same_as<decltype(ranges::empty(std::declval<Range>())), bool>);
    }

    return true;
}

template <class Range, class Size = invalid_type>
constexpr bool test_size() {
    // Validate ranges::size, ranges::sized_range, and ranges::ssize
    static_assert(!is_valid<Size> || std::integral<Size>);

    static_assert(CanSize<Range> == is_valid<Size>);
    static_assert(CanSSize<Range> == is_valid<Size>);
    static_assert(CanSizeType<Range> == is_valid<Size>);
    static_assert(ranges::sized_range<Range> == is_valid<Size>);
    if constexpr (is_valid<Size>) {
        static_assert(std::same_as<decltype(ranges::size(std::declval<Range>())), Size>);
        static_assert(std::same_as<ranges::range_size_t<Range>, Size>);

        using SignedSize = std::common_type_t<std::ptrdiff_t, std::make_signed_t<Size>>;
        static_assert(std::same_as<decltype(ranges::ssize(std::declval<Range>())), SignedSize>);

        static_assert(CanEmpty<Range>);
    }

    return true;
}

template <class Range, class Pointer = invalid_type>
constexpr bool test_data() {
    // Validate ranges::data
    static_assert(!is_valid<Pointer> || std::is_pointer_v<Pointer>);

    static_assert(CanData<Range> == is_valid<Pointer>);
    if constexpr (is_valid<Pointer>) {
        static_assert(std::same_as<decltype(ranges::data(std::declval<Range>())), Pointer>);
        static_assert(std::same_as<ranges::range_value_t<Range>, std::remove_cv_t<std::remove_pointer_t<Pointer>>>);
    } else {
        static_assert(!CanCData<Range>);
    }

    return true;
}

template <class Range, class Pointer = invalid_type>
constexpr bool test_cdata() {
    // Validate ranges::cdata
    static_assert(!is_valid<Pointer> || std::is_pointer_v<Pointer>);

    static_assert(CanCData<Range> == is_valid<Pointer>);
    if constexpr (is_valid<Pointer>) {
        static_assert(std::same_as<decltype(ranges::cdata(std::declval<Range>())), Pointer>);
        static_assert(std::same_as<ranges::range_value_t<Range>, std::remove_cv_t<std::remove_pointer_t<Pointer>>>);

        static_assert(CanData<Range>);
    }

    return true;
}

template <class NonRange>
constexpr bool test_non_range() {
    // Validate ranges::range, ranges::sized_range, and ranges::view
    static_assert(!ranges::range<NonRange>);
    static_assert(!CanEnd<NonRange>);
    static_assert(!CanCEnd<NonRange>);
    static_assert(!ranges::sized_range<NonRange>);
    static_assert(!ranges::view<NonRange>);
    return true;
}

template <class Range>
constexpr bool test_input_range() {
    // Validate ranges::range and ranges::input_range
    static_assert(ranges::range<Range>);
    using Iterator = ranges::iterator_t<Range>;
    static_assert(std::input_iterator<Iterator>);
    static_assert(ranges::input_range<Range>);

    // Validate ranges::range_value_t and ranges::range_rvalue_reference_t
    using Value = std::iter_value_t<Iterator>;
    static_assert(std::same_as<ranges::range_value_t<Range>, Value>);
    static_assert(std::same_as<ranges::range_value_t<Range&>, Value>);
    static_assert(std::same_as<ranges::range_value_t<Range&&>, Value>);

    using RvalueReference = std::iter_rvalue_reference_t<Iterator>;
    static_assert(std::same_as<ranges::range_rvalue_reference_t<Range>, RvalueReference>);
    static_assert(std::same_as<ranges::range_rvalue_reference_t<Range&>, RvalueReference>);
    static_assert(std::same_as<ranges::range_rvalue_reference_t<Range&&>, RvalueReference>);

    // LWG-3860, validate ranges::range_common_reference_t
    using CommonReference = std::iter_common_reference_t<Iterator>;
    static_assert(std::same_as<ranges::range_common_reference_t<Range>, CommonReference>);
    static_assert(std::same_as<ranges::range_common_reference_t<Range&>, CommonReference>);
    static_assert(std::same_as<ranges::range_common_reference_t<Range&&>, CommonReference>);

    return true;
}

template <class Range>
constexpr bool test_forward_range() {
    // Validate ranges::forward_range
    static_assert(test_input_range<Range>());
    static_assert(std::forward_iterator<ranges::iterator_t<Range>>);
    static_assert(ranges::forward_range<Range>);
    static_assert(CanEmpty<Range>);

    return true;
}

template <class Range>
constexpr bool test_bidirectional_range() {
    // Validate ranges::bidirectional_range
    static_assert(test_forward_range<Range>());
    static_assert(std::bidirectional_iterator<ranges::iterator_t<Range>>);
    static_assert(ranges::bidirectional_range<Range>);
    if constexpr (ranges::common_range<Range>) {
        static_assert(CanRBegin<Range&>);
        static_assert(CanREnd<Range&>);
    }

    return true;
}

template <class Range>
constexpr bool test_random_access_range() {
    // Validate ranges::random_access_range
    static_assert(test_bidirectional_range<Range>());
    static_assert(std::random_access_iterator<ranges::iterator_t<Range>>);
    static_assert(ranges::random_access_range<Range>);
    static_assert(ranges::sized_range<Range> || !ranges::common_range<Range>);

    return true;
}

template <class Range>
constexpr bool test_contiguous_range() {
    // Validate ranges::contiguous_range
    static_assert(test_random_access_range<Range>());
    static_assert(std::contiguous_iterator<ranges::iterator_t<Range>>);
    static_assert(ranges::contiguous_range<Range>);
    static_assert(CanData<Range&>);

    return true;
}

// Validate types that are unequivocally not ranges
template <class Nope>
constexpr bool totally_not_a_range() {
    static_assert(test_begin<Nope>());
    static_assert(test_end<Nope>());
    static_assert(test_cbegin<Nope>());
    static_assert(test_cend<Nope>());
    static_assert(test_rbegin<Nope>());
    static_assert(test_rend<Nope>());
    static_assert(test_crbegin<Nope>());
    static_assert(test_crend<Nope>());
    static_assert(test_empty<Nope, false>());
    static_assert(test_size<Nope>());
    static_assert(test_data<Nope>());
    static_assert(test_cdata<Nope>());
    static_assert(test_non_range<Nope>());
    return true;
}
static_assert(totally_not_a_range<void>());
static_assert(totally_not_a_range<int>());
static_assert(totally_not_a_range<int&>());

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
static_assert(totally_not_a_range<void()>());
static_assert(totally_not_a_range<void (*)()>());
static_assert(totally_not_a_range<void (&)()>());
static_assert(totally_not_a_range<void() const>());
#pragma warning(pop)

// Validate C arrays, which have special-case range behaviors
static_assert(test_begin<int[42]>());
static_assert(test_end<int[42]>());
static_assert(test_cbegin<int[42]>());
static_assert(test_cend<int[42]>());
static_assert(test_rbegin<int[42]>());
static_assert(test_rend<int[42]>());
static_assert(test_crbegin<int[42]>());
static_assert(test_crend<int[42]>());
static_assert(test_size<int[42], std::size_t>());
static_assert(test_empty<int[42], true>());
static_assert(test_data<int[42]>());
static_assert(test_cdata<int[42]>());
static_assert(test_contiguous_range<int[42]>());
static_assert(!ranges::view<int[42]>);

static_assert(test_begin<int const[42]>());
static_assert(test_end<int const[42]>());
static_assert(test_cbegin<int const[42]>());
static_assert(test_cend<int const[42]>());
static_assert(test_rbegin<int const[42]>());
static_assert(test_rend<int const[42]>());
static_assert(test_crbegin<int const[42]>());
static_assert(test_crend<int const[42]>());
static_assert(test_size<int const[42], std::size_t>());
static_assert(test_empty<int const[42], true>());
static_assert(test_data<int const[42]>());
static_assert(test_cdata<int const[42]>());
static_assert(test_contiguous_range<int const[42]>());
static_assert(!ranges::view<int const[42]>);

static_assert(test_begin<int (&)[42], int*>());
static_assert(test_end<int (&)[42], int*>());
static_assert(test_cbegin<int (&)[42], int const*>());
static_assert(test_cend<int (&)[42], int const*>());
static_assert(test_rbegin<int (&)[42], std::reverse_iterator<int*>>());
static_assert(test_rend<int (&)[42], std::reverse_iterator<int*>>());
static_assert(test_crbegin<int (&)[42], std::reverse_iterator<int const*>>());
static_assert(test_crend<int (&)[42], std::reverse_iterator<int const*>>());
static_assert(test_size<int (&)[42], std::size_t>());
static_assert(test_empty<int (&)[42], true>());
static_assert(test_data<int (&)[42], int*>());
static_assert(test_cdata<int (&)[42], int const*>());
static_assert(test_contiguous_range<int (&)[42]>());
static_assert(!ranges::view<int (&)[42]>);

static_assert(test_begin<int const (&)[42], int const*>());
static_assert(test_end<int const (&)[42], int const*>());
static_assert(test_cbegin<int const (&)[42], int const*>());
static_assert(test_cend<int const (&)[42], int const*>());
static_assert(test_rbegin<int const (&)[42], std::reverse_iterator<int const*>>());
static_assert(test_rend<int const (&)[42], std::reverse_iterator<int const*>>());
static_assert(test_crbegin<int const (&)[42], std::reverse_iterator<int const*>>());
static_assert(test_crend<int const (&)[42], std::reverse_iterator<int const*>>());
static_assert(test_size<int const (&)[42], std::size_t>());
static_assert(test_empty<int const (&)[42], true>());
static_assert(test_data<int const (&)[42], int const*>());
static_assert(test_cdata<int const (&)[42], int const*>());
static_assert(test_contiguous_range<int const (&)[42]>());
static_assert(!ranges::view<int const (&)[42]>);

// Validate arrays of unknown bound
static_assert(test_begin<int[]>());
static_assert(test_end<int[]>());
static_assert(test_cbegin<int[]>());
static_assert(test_cend<int[]>());
static_assert(test_rbegin<int[]>());
static_assert(test_rend<int[]>());
static_assert(test_crbegin<int[]>());
static_assert(test_crend<int[]>());
static_assert(test_size<int[]>());
static_assert(test_empty<int[], false>());
static_assert(test_data<int[]>());
static_assert(test_cdata<int[]>());
static_assert(!ranges::range<int[]>);
static_assert(!ranges::view<int[]>);

static_assert(test_begin<int const[]>());
static_assert(test_end<int const[]>());
static_assert(test_cbegin<int const[]>());
static_assert(test_cend<int const[]>());
static_assert(test_rbegin<int const[]>());
static_assert(test_rend<int const[]>());
static_assert(test_crbegin<int const[]>());
static_assert(test_crend<int const[]>());
static_assert(test_size<int const[]>());
static_assert(test_empty<int const[], false>());
static_assert(test_data<int const[]>());
static_assert(test_cdata<int const[]>());
static_assert(!ranges::range<int const[]>);
static_assert(!ranges::view<int const[]>);

static_assert(test_begin<int (&)[], int*>());
static_assert(test_end<int (&)[]>());
static_assert(test_cend<int (&)[]>());
static_assert(test_rbegin<int (&)[]>());
static_assert(test_rend<int (&)[]>());
static_assert(test_crbegin<int (&)[]>());
static_assert(test_crend<int (&)[]>());
static_assert(test_size<int (&)[]>());
static_assert(test_empty<int (&)[], false>());
// Can't use test_data here because it uses range_value_t and this isn't a range
static_assert(std::same_as<decltype(ranges::data(std::declval<int (&)[]>())), int*>);
static_assert(!ranges::range<int (&)[]>);
static_assert(!ranges::view<int (&)[]>);

#if _HAS_CXX23 // ranges::cbegin and ranges::cdata behavior differs in C++20 and C++23 modes
static_assert(test_cbegin<int (&)[]>());
static_assert(test_cdata<int (&)[]>());
#else // ^^^ _HAS_CXX23 / !_HAS_CXX23 vvv
static_assert(test_cbegin<int (&)[], int const*>());
// Can't use test_cdata here because it uses range_value_t and this isn't a range
static_assert(std::same_as<decltype(ranges::cdata(std::declval<int (&)[]>())), int const*>);
#endif // ^^^ !_HAS_CXX23 ^^^

static_assert(test_begin<int const (&)[], int const*>());
static_assert(test_end<int const (&)[]>());
static_assert(test_cend<int const (&)[]>());
static_assert(test_rbegin<int const (&)[]>());
static_assert(test_rend<int const (&)[]>());
static_assert(test_crbegin<int const (&)[]>());
static_assert(test_crend<int const (&)[]>());
static_assert(test_size<int const (&)[]>());
static_assert(test_empty<int const (&)[], false>());
// Can't use test_data here because it uses range_value_t and this isn't a range
static_assert(std::same_as<decltype(ranges::data(std::declval<int const (&)[]>())), int const*>);
static_assert(!ranges::range<int const (&)[]>);
static_assert(!ranges::view<int const (&)[]>);

#if _HAS_CXX23 // ranges::cbegin and ranges::cdata behavior differs in C++20 and C++23 modes
static_assert(test_cbegin<int const (&)[]>());
static_assert(test_cdata<int const (&)[]>());
#else // ^^^ _HAS_CXX23 / !_HAS_CXX23 vvv
static_assert(test_cbegin<int const (&)[], int const*>());
// Can't use test_cdata here because it uses range_value_t and this isn't a range
static_assert(std::same_as<decltype(ranges::cdata(std::declval<int const (&)[]>())), int const*>);
#endif // ^^^ !_HAS_CXX23 ^^^

// Validate behavior before/after completing the bound of an array
extern int initially_unbounded[];
static_assert(ranges::begin(initially_unbounded) == initially_unbounded);
static_assert(!CanEnd<decltype((initially_unbounded))>);
static_assert(!CanCEnd<decltype((initially_unbounded))>);
static_assert(!CanRBegin<decltype((initially_unbounded))>);
static_assert(!CanREnd<decltype((initially_unbounded))>);
static_assert(!CanCRBegin<decltype((initially_unbounded))>);
static_assert(!CanCREnd<decltype((initially_unbounded))>);
static_assert(!CanSize<decltype((initially_unbounded))>);
static_assert(!CanEmpty<decltype((initially_unbounded))>);
static_assert(ranges::data(initially_unbounded) == initially_unbounded);

#if _HAS_CXX23 // ranges::cbegin and ranges::cdata behavior differs in C++20 and C++23 modes
static_assert(!CanCBegin<decltype((initially_unbounded))>);
static_assert(!CanCData<decltype((initially_unbounded))>);
#else // ^^^ _HAS_CXX23 / !_HAS_CXX23 vvv
static_assert(ranges::cbegin(initially_unbounded) == initially_unbounded);
static_assert(ranges::cdata(initially_unbounded) == initially_unbounded);
#endif // ^^^ !_HAS_CXX23 ^^^

int initially_unbounded[42];
static_assert(ranges::begin(initially_unbounded) == initially_unbounded);
static_assert(ranges::end(initially_unbounded) == initially_unbounded + ranges::size(initially_unbounded));
static_assert(ranges::cbegin(initially_unbounded) == initially_unbounded);
static_assert(ranges::cend(initially_unbounded) == initially_unbounded + ranges::size(initially_unbounded));
static_assert(ranges::rbegin(initially_unbounded).base() == initially_unbounded + ranges::size(initially_unbounded));
static_assert(ranges::rend(initially_unbounded).base() == initially_unbounded);
static_assert(ranges::crbegin(initially_unbounded).base() == initially_unbounded + ranges::size(initially_unbounded));
static_assert(ranges::crend(initially_unbounded).base() == initially_unbounded);
static_assert(ranges::size(initially_unbounded) == std::size(initially_unbounded));
static_assert(!ranges::empty(initially_unbounded));
static_assert(ranges::data(initially_unbounded) == initially_unbounded);
static_assert(ranges::cdata(initially_unbounded) == initially_unbounded);


// Validate the few behaviors that we can for arrays of incomplete element type. begin, end, rbegin, rend, and data
// (and their c variations) are ill-formed with no diagnostic required; we can only pass an array of incomplete elements
// to size and empty.
struct initially_incomplete;
extern initially_incomplete array_of_incomplete[42];
static_assert(ranges::size(array_of_incomplete) == 42);
static_assert(!ranges::empty(array_of_incomplete));

// begin, end, rbegin, rend, and data (and their c variations) should reject rvalues of array of incomplete elements
// with substitution failures
static_assert(!CanBegin<initially_incomplete (&&)[42]>);
static_assert(!CanCBegin<initially_incomplete (&&)[42]>);
static_assert(!CanEnd<initially_incomplete (&&)[42]>);
static_assert(!CanCEnd<initially_incomplete (&&)[42]>);
static_assert(!CanRBegin<initially_incomplete (&&)[42]>);
static_assert(!CanCRBegin<initially_incomplete (&&)[42]>);
static_assert(!CanREnd<initially_incomplete (&&)[42]>);
static_assert(!CanCREnd<initially_incomplete (&&)[42]>);
static_assert(!CanData<initially_incomplete (&&)[42]>);
static_assert(!CanCData<initially_incomplete (&&)[42]>);

struct initially_incomplete {};
initially_incomplete array_of_incomplete[42];


// Validate initializer_lists, which have special-case range behaviors
static_assert(test_begin<std::initializer_list<int>>());
static_assert(test_end<std::initializer_list<int>>());
static_assert(test_cbegin<std::initializer_list<int>>());
static_assert(test_cend<std::initializer_list<int>>());
static_assert(test_rbegin<std::initializer_list<int>>());
static_assert(test_rend<std::initializer_list<int>>());
static_assert(test_crbegin<std::initializer_list<int>>());
static_assert(test_crend<std::initializer_list<int>>());
static_assert(test_size<std::initializer_list<int>, std::size_t>());
static_assert(test_empty<std::initializer_list<int>, true>());
static_assert(test_data<std::initializer_list<int>>());
static_assert(test_cdata<std::initializer_list<int>>());
static_assert(test_contiguous_range<std::initializer_list<int>>());
static_assert(!ranges::view<std::initializer_list<int>>);

static_assert(test_begin<std::initializer_list<int> const>());
static_assert(test_end<std::initializer_list<int> const>());
static_assert(test_cbegin<std::initializer_list<int> const>());
static_assert(test_cend<std::initializer_list<int> const>());
static_assert(test_rbegin<std::initializer_list<int> const>());
static_assert(test_rend<std::initializer_list<int> const>());
static_assert(test_crbegin<std::initializer_list<int> const>());
static_assert(test_crend<std::initializer_list<int> const>());
static_assert(test_size<std::initializer_list<int> const, std::size_t>());
static_assert(test_empty<std::initializer_list<int> const, true>());
static_assert(test_data<std::initializer_list<int> const>());
static_assert(test_cdata<std::initializer_list<int> const>());
static_assert(test_contiguous_range<std::initializer_list<int> const>());
static_assert(!ranges::view<std::initializer_list<int> const>);

static_assert(test_begin<std::initializer_list<int>&, int const*>());
static_assert(test_end<std::initializer_list<int>&, int const*>());
static_assert(test_cbegin<std::initializer_list<int>&, int const*>());
static_assert(test_cend<std::initializer_list<int>&, int const*>());
static_assert(test_rbegin<std::initializer_list<int>&, std::reverse_iterator<int const*>>());
static_assert(test_rend<std::initializer_list<int>&, std::reverse_iterator<int const*>>());
static_assert(test_crbegin<std::initializer_list<int>&, std::reverse_iterator<int const*>>());
static_assert(test_crend<std::initializer_list<int>&, std::reverse_iterator<int const*>>());
static_assert(test_size<std::initializer_list<int>&, std::size_t>());
static_assert(test_empty<std::initializer_list<int>&, true>());
static_assert(test_data<std::initializer_list<int>&, int const*>());
static_assert(test_cdata<std::initializer_list<int>&, int const*>());
static_assert(test_contiguous_range<std::initializer_list<int>&>());
static_assert(!ranges::view<std::initializer_list<int>&>);

static_assert(test_begin<std::initializer_list<int> const&, int const*>());
static_assert(test_end<std::initializer_list<int> const&, int const*>());
static_assert(test_cbegin<std::initializer_list<int> const&, int const*>());
static_assert(test_cend<std::initializer_list<int> const&, int const*>());
static_assert(test_rbegin<std::initializer_list<int> const&, std::reverse_iterator<int const*>>());
static_assert(test_rend<std::initializer_list<int> const&, std::reverse_iterator<int const*>>());
static_assert(test_crbegin<std::initializer_list<int> const&, std::reverse_iterator<int const*>>());
static_assert(test_crend<std::initializer_list<int> const&, std::reverse_iterator<int const*>>());
static_assert(test_size<std::initializer_list<int> const&, std::size_t>());
static_assert(test_empty<std::initializer_list<int> const&, true>());
static_assert(test_data<std::initializer_list<int> const&, int const*>());
static_assert(test_cdata<std::initializer_list<int> const&, int const*>());
static_assert(test_contiguous_range<std::initializer_list<int> const&>());
static_assert(!ranges::view<std::initializer_list<int> const&>);


// Validate containers
template <class>
constexpr bool is_forward_list = false;
template <class T, class A>
constexpr bool is_forward_list<std::forward_list<T, A>> = true;

template <class T, class IterConcept>
constexpr bool test_std_container() {
    using I  = T::iterator;
    using D  = std::iter_difference_t<I>;
    using CI = T::const_iterator;
    static_assert(std::same_as<std::iter_difference_t<CI>, D>);

    using Category = std::iterator_traits<I>::iterator_category;
    static_assert(std::derived_from<IterConcept, Category>);

    using RI  = std::conditional_t<std::bidirectional_iterator<I>, std::reverse_iterator<I>, invalid_type>;
    using RCI = std::conditional_t<std::bidirectional_iterator<I>, std::reverse_iterator<CI>, invalid_type>;

    using V = T::value_type;

    static_assert(test_begin<T>());
    static_assert(test_end<T>());
    static_assert(test_cbegin<T>());
    static_assert(test_cend<T>());
    static_assert(test_rbegin<T>());
    static_assert(test_rend<T>());
    static_assert(test_crbegin<T>());
    static_assert(test_crend<T>());
    if constexpr (!is_forward_list<T>) {
        static_assert(test_size<T, std::make_unsigned_t<D>>());
    }
    static_assert(test_empty<T, true>());
    if constexpr (std::contiguous_iterator<I>) {
        static_assert(test_data<T>());
        static_assert(test_cdata<T>());
    }
    static_assert(!ranges::view<T>);

    static_assert(test_begin<T const>());
    static_assert(test_end<T const>());
    static_assert(test_cbegin<T const>());
    static_assert(test_cend<T const>());
    static_assert(test_rbegin<T const>());
    static_assert(test_rend<T const>());
    static_assert(test_crbegin<T const>());
    static_assert(test_crend<T const>());
    if constexpr (!is_forward_list<T>) {
        static_assert(test_size<T const, std::make_unsigned_t<D>>());
    }
    static_assert(test_empty<T const, true>());
    if constexpr (std::contiguous_iterator<I>) {
        static_assert(test_data<T const>());
        static_assert(test_cdata<T const>());
    }
    static_assert(!ranges::view<T const>);

    static_assert(test_begin<T&, I>());
    static_assert(test_end<T&, I>());
    static_assert(test_cbegin<T&, CI>());
    static_assert(test_cend<T&, CI>());
    static_assert(test_rbegin<T&, RI>());
    static_assert(test_rend<T&, RI>());
    static_assert(test_crbegin<T&, RCI>());
    static_assert(test_crend<T&, RCI>());
    if constexpr (!is_forward_list<T>) {
        static_assert(test_size<T&, std::make_unsigned_t<D>>());
    }
    static_assert(test_empty<T&, true>());
    if constexpr (std::contiguous_iterator<I>) {
        static_assert(test_data<T&, V*>());
        static_assert(test_cdata<T&, V const*>());
    }
    static_assert(!ranges::view<T&>);

    static_assert(test_begin<T const&, CI>());
    static_assert(test_end<T const&, CI>());
    static_assert(test_cbegin<T const&, CI>());
    static_assert(test_cend<T const&, CI>());
    static_assert(test_rbegin<T const&, RCI>());
    static_assert(test_rend<T const&, RCI>());
    static_assert(test_crbegin<T const&, RCI>());
    static_assert(test_crend<T const&, RCI>());
    if constexpr (!is_forward_list<T>) {
        static_assert(test_size<T const&, std::make_unsigned_t<D>>());
    }
    static_assert(test_empty<T const&, true>());
    if constexpr (std::contiguous_iterator<I>) {
        static_assert(test_data<T const&, V const*>());
        static_assert(test_cdata<T const&, V const*>());
    }
    static_assert(!ranges::view<T const&>);

    static_assert(std::contiguous_iterator<I> == std::derived_from<IterConcept, std::contiguous_iterator_tag>);
    static_assert(std::random_access_iterator<I> == std::derived_from<IterConcept, std::random_access_iterator_tag>);
    static_assert(std::bidirectional_iterator<I> == std::derived_from<IterConcept, std::bidirectional_iterator_tag>);
    static_assert(std::forward_iterator<I>);
    static_assert(std::derived_from<IterConcept, std::forward_iterator_tag>);

    if constexpr (std::contiguous_iterator<I>) {
        static_assert(test_contiguous_range<T>());
        static_assert(test_contiguous_range<T const>());
        static_assert(test_contiguous_range<T&>());
        static_assert(test_contiguous_range<T const&>());
    } else if constexpr (std::random_access_iterator<I>) {
        static_assert(test_random_access_range<T>());
        static_assert(test_random_access_range<T const>());
        static_assert(test_random_access_range<T&>());
        static_assert(test_random_access_range<T const&>());
    } else if constexpr (std::bidirectional_iterator<I>) {
        static_assert(test_bidirectional_range<T>());
        static_assert(test_bidirectional_range<T const>());
        static_assert(test_bidirectional_range<T&>());
        static_assert(test_bidirectional_range<T const&>());
    } else {
        static_assert(test_forward_range<T>());
        static_assert(test_forward_range<T const>());
        static_assert(test_forward_range<T&>());
        static_assert(test_forward_range<T const&>());
    }

    return true;
}

static_assert(test_std_container<std::array<int, 42>, std::contiguous_iterator_tag>());
static_assert(test_std_container<std::deque<int>, std::random_access_iterator_tag>());
static_assert(test_std_container<std::forward_list<int>, std::forward_iterator_tag>());
static_assert(test_std_container<std::list<int>, std::bidirectional_iterator_tag>());
static_assert(test_std_container<std::map<int, int>, std::bidirectional_iterator_tag>());
static_assert(test_std_container<std::multimap<int, int>, std::bidirectional_iterator_tag>());
static_assert(test_std_container<std::multiset<int>, std::bidirectional_iterator_tag>());
static_assert(test_std_container<std::set<int>, std::bidirectional_iterator_tag>());
static_assert(test_std_container<std::string, std::contiguous_iterator_tag>());
static_assert(test_std_container<std::unordered_map<int, int>, std::bidirectional_iterator_tag>());
static_assert(test_std_container<std::unordered_multimap<int, int>, std::bidirectional_iterator_tag>());
static_assert(test_std_container<std::unordered_multiset<int>, std::bidirectional_iterator_tag>());
static_assert(test_std_container<std::unordered_set<int>, std::bidirectional_iterator_tag>());
static_assert(test_std_container<std::vector<int>, std::contiguous_iterator_tag>());
static_assert(test_std_container<std::vector<bool>, std::random_access_iterator_tag>());
static_assert(test_std_container<std::wstring, std::contiguous_iterator_tag>());

// Validate that "old" fancy pointers that fail to model contiguous_iterator don't break contiguity of containers.
template <class T>
struct fancy_pointer {
    using element_type    = T;
    using difference_type = int;

    fancy_pointer() = default;
    fancy_pointer(std::nullptr_t);
    template <class U>
        requires std::convertible_to<U*, T*>
    fancy_pointer(fancy_pointer<U>);

    element_type& operator*() const;
    element_type& operator[](difference_type) const;
    element_type* operator->() const;

    fancy_pointer& operator++();
    fancy_pointer operator++(int);
    fancy_pointer& operator--();
    fancy_pointer operator--(int);

    fancy_pointer& operator+=(difference_type);
    fancy_pointer operator+(difference_type) const;
    friend fancy_pointer operator+(difference_type, fancy_pointer);

    fancy_pointer& operator-=(difference_type);
    fancy_pointer operator-(difference_type) const;
    difference_type operator-(fancy_pointer) const;

    explicit operator bool() const;

    bool operator==(fancy_pointer const&) const = default;
    bool operator==(std::nullptr_t) const;

    auto operator<=>(fancy_pointer const&) const = default;

    static fancy_pointer pointer_to(element_type&);
};
static_assert(std::random_access_iterator<fancy_pointer<int>>);
static_assert(!std::contiguous_iterator<fancy_pointer<int>>);

template <class T>
struct fancy_allocator {
    using value_type = T;
    using pointer    = fancy_pointer<T>;

    fancy_allocator() = default;
    template <class U>
    fancy_allocator(fancy_allocator<U> const&) {}

    pointer allocate(std::size_t);
    void deallocate(pointer, std::size_t);

    template <class U>
    bool operator==(fancy_allocator<U>) const {
        return true;
    }
};

static_assert(test_std_container<std::basic_string<char, std::char_traits<char>, fancy_allocator<char>>,
    std::contiguous_iterator_tag>());
static_assert(test_std_container<std::vector<int, fancy_allocator<int>>, std::contiguous_iterator_tag>());

// Validate some non-containers
static_assert(test_begin<std::string_view, std::string_view::iterator>());
static_assert(test_end<std::string_view, std::string_view::iterator>());
static_assert(test_cbegin<std::string_view, std::string_view::iterator>());
static_assert(test_cend<std::string_view, std::string_view::iterator>());
static_assert(test_rbegin<std::string_view, std::reverse_iterator<std::string_view::iterator>>());
static_assert(test_rend<std::string_view, std::reverse_iterator<std::string_view::iterator>>());
static_assert(test_crbegin<std::string_view, std::reverse_iterator<std::string_view::iterator>>());
static_assert(test_crend<std::string_view, std::reverse_iterator<std::string_view::iterator>>());
static_assert(test_size<std::string_view, std::size_t>());
static_assert(test_empty<std::string_view, true>());
static_assert(test_data<std::string_view, char const*>());
static_assert(test_cdata<std::string_view, char const*>());
static_assert(test_contiguous_range<std::string_view>());
static_assert(ranges::view<std::string_view>);

static_assert(test_begin<std::string_view const, std::string_view::iterator>());
static_assert(test_end<std::string_view const, std::string_view::iterator>());
static_assert(test_cbegin<std::string_view const, std::string_view::iterator>());
static_assert(test_cend<std::string_view const, std::string_view::iterator>());
static_assert(test_rbegin<std::string_view const, std::reverse_iterator<std::string_view::iterator>>());
static_assert(test_rend<std::string_view const, std::reverse_iterator<std::string_view::iterator>>());
static_assert(test_crbegin<std::string_view const, std::reverse_iterator<std::string_view::iterator>>());
static_assert(test_crend<std::string_view const, std::reverse_iterator<std::string_view::iterator>>());
static_assert(test_size<std::string_view const, std::size_t>());
static_assert(test_empty<std::string_view const, true>());
static_assert(test_data<std::string_view const, char const*>());
static_assert(test_cdata<std::string_view const, char const*>());
static_assert(test_contiguous_range<std::string_view const>());
static_assert(!ranges::view<std::string_view const>);

static_assert(test_begin<std::string_view&, std::string_view::iterator>());
static_assert(test_end<std::string_view&, std::string_view::iterator>());
static_assert(test_cbegin<std::string_view&, std::string_view::iterator>());
static_assert(test_cend<std::string_view&, std::string_view::iterator>());
static_assert(test_rbegin<std::string_view&, std::reverse_iterator<std::string_view::iterator>>());
static_assert(test_rend<std::string_view&, std::reverse_iterator<std::string_view::iterator>>());
static_assert(test_crbegin<std::string_view&, std::reverse_iterator<std::string_view::iterator>>());
static_assert(test_crend<std::string_view&, std::reverse_iterator<std::string_view::iterator>>());
static_assert(test_size<std::string_view&, std::size_t>());
static_assert(test_empty<std::string_view&, true>());
static_assert(test_data<std::string_view&, char const*>());
static_assert(test_cdata<std::string_view&, char const*>());
static_assert(test_contiguous_range<std::string_view&>());
static_assert(!ranges::view<std::string_view&>);

static_assert(test_begin<std::string_view const&, std::string_view::iterator>());
static_assert(test_end<std::string_view const&, std::string_view::iterator>());
static_assert(test_cbegin<std::string_view const&, std::string_view::iterator>());
static_assert(test_cend<std::string_view const&, std::string_view::iterator>());
static_assert(test_rbegin<std::string_view const&, std::reverse_iterator<std::string_view::iterator>>());
static_assert(test_rend<std::string_view const&, std::reverse_iterator<std::string_view::iterator>>());
static_assert(test_crbegin<std::string_view const&, std::reverse_iterator<std::string_view::iterator>>());
static_assert(test_crend<std::string_view const&, std::reverse_iterator<std::string_view::iterator>>());
static_assert(test_size<std::string_view const&, std::size_t>());
static_assert(test_empty<std::string_view const&, true>());
static_assert(test_data<std::string_view const&, char const*>());
static_assert(test_cdata<std::string_view const&, char const*>());
static_assert(test_contiguous_range<std::string_view const&>());
static_assert(!ranges::view<std::string_view const&>);

static_assert(test_begin<std::span<int>, std::span<int>::iterator>());
static_assert(test_end<std::span<int>, std::span<int>::iterator>());
static_assert(test_rbegin<std::span<int>, std::reverse_iterator<std::span<int>::iterator>>());
static_assert(test_rend<std::span<int>, std::reverse_iterator<std::span<int>::iterator>>());
static_assert(test_size<std::span<int>, std::size_t>());
static_assert(test_empty<std::span<int>, true>());
static_assert(test_data<std::span<int>, int*>());
static_assert(test_contiguous_range<std::span<int>>());
static_assert(ranges::view<std::span<int>>);

#if _HAS_CXX23 // behavior of span members differs in C++20 and C++23 modes
static_assert(test_cbegin<std::span<int>, std::span<int>::const_iterator>());
static_assert(test_cend<std::span<int>, std::span<int>::const_iterator>());
static_assert(test_crbegin<std::span<int>, std::span<int>::const_reverse_iterator>());
static_assert(test_crend<std::span<int>, std::span<int>::const_reverse_iterator>());
static_assert(test_cdata<std::span<int>, const int*>());
#else // ^^^ _HAS_CXX23 / !_HAS_CXX23 vvv
static_assert(test_cbegin<std::span<int>, std::span<int>::iterator>());
static_assert(test_cend<std::span<int>, std::span<int>::iterator>());
static_assert(test_crbegin<std::span<int>, std::reverse_iterator<std::span<int>::iterator>>());
static_assert(test_crend<std::span<int>, std::reverse_iterator<std::span<int>::iterator>>());
static_assert(test_cdata<std::span<int>, int*>());
#endif // ^^^ !_HAS_CXX23 ^^^

static_assert(test_begin<std::span<int> const, std::span<int>::iterator>());
static_assert(test_end<std::span<int> const, std::span<int>::iterator>());
static_assert(test_rbegin<std::span<int> const, std::reverse_iterator<std::span<int>::iterator>>());
static_assert(test_rend<std::span<int> const, std::reverse_iterator<std::span<int>::iterator>>());
static_assert(test_size<std::span<int> const, std::size_t>());
static_assert(test_empty<std::span<int> const, true>());
static_assert(test_data<std::span<int> const, int*>());
static_assert(test_contiguous_range<std::span<int> const>());
static_assert(!ranges::view<std::span<int> const>);

#if _HAS_CXX23 // behavior of const span members differs in C++20 and C++23 modes
static_assert(test_cbegin<std::span<int> const, std::span<int>::const_iterator>());
static_assert(test_cend<std::span<int> const, std::span<int>::const_iterator>());
static_assert(test_crbegin<std::span<int> const, std::span<int>::const_reverse_iterator>());
static_assert(test_crend<std::span<int> const, std::span<int>::const_reverse_iterator>());
static_assert(test_cdata<std::span<int> const, const int*>());
#else // ^^^ _HAS_CXX23 / !_HAS_CXX23 vvv
static_assert(test_cbegin<std::span<int> const, std::span<int>::iterator>());
static_assert(test_cend<std::span<int> const, std::span<int>::iterator>());
static_assert(test_crbegin<std::span<int> const, std::reverse_iterator<std::span<int>::iterator>>());
static_assert(test_crend<std::span<int> const, std::reverse_iterator<std::span<int>::iterator>>());
static_assert(test_cdata<std::span<int> const, int*>());
#endif // ^^^ !_HAS_CXX23 ^^^

static_assert(test_begin<std::span<int>&, std::span<int>::iterator>());
static_assert(test_end<std::span<int>&, std::span<int>::iterator>());
static_assert(test_rbegin<std::span<int>&, std::reverse_iterator<std::span<int>::iterator>>());
static_assert(test_rend<std::span<int>&, std::reverse_iterator<std::span<int>::iterator>>());
static_assert(test_size<std::span<int>&, std::size_t>());
static_assert(test_empty<std::span<int>&, true>());
static_assert(test_data<std::span<int>&, int*>());
static_assert(test_contiguous_range<std::span<int>&>());
static_assert(!ranges::view<std::span<int>&>);

#if _HAS_CXX23 // behavior of span& members differs in C++20 and C++23 modes
static_assert(test_cbegin<std::span<int>&, std::span<int>::const_iterator>());
static_assert(test_cend<std::span<int>&, std::span<int>::const_iterator>());
static_assert(test_crbegin<std::span<int>&, std::span<int>::const_reverse_iterator>());
static_assert(test_crend<std::span<int>&, std::span<int>::const_reverse_iterator>());
static_assert(test_cdata<std::span<int>&, const int*>());
#else // ^^^ _HAS_CXX23 / !_HAS_CXX23 vvv
static_assert(test_cbegin<std::span<int>&, std::span<int>::iterator>());
static_assert(test_cend<std::span<int>&, std::span<int>::iterator>());
static_assert(test_crbegin<std::span<int>&, std::reverse_iterator<std::span<int>::iterator>>());
static_assert(test_crend<std::span<int>&, std::reverse_iterator<std::span<int>::iterator>>());
static_assert(test_cdata<std::span<int>&, int*>());
#endif // ^^^ !_HAS_CXX23 ^^^

static_assert(test_begin<std::span<int> const&, std::span<int>::iterator>());
static_assert(test_end<std::span<int> const&, std::span<int>::iterator>());
static_assert(test_rbegin<std::span<int> const&, std::reverse_iterator<std::span<int>::iterator>>());
static_assert(test_rend<std::span<int> const&, std::reverse_iterator<std::span<int>::iterator>>());
static_assert(test_size<std::span<int> const&, std::size_t>());
static_assert(test_empty<std::span<int> const&, true>());
static_assert(test_data<std::span<int> const&, int*>());
static_assert(test_contiguous_range<std::span<int> const&>());
static_assert(!ranges::view<std::span<int> const&>);

#if _HAS_CXX23 // behavior of const span& members differs in C++20 and C++23 modes
static_assert(test_cbegin<std::span<int> const&, std::span<int>::const_iterator>());
static_assert(test_cend<std::span<int> const&, std::span<int>::const_iterator>());
static_assert(test_crbegin<std::span<int> const&, std::span<int>::const_reverse_iterator>());
static_assert(test_crend<std::span<int> const&, std::span<int>::const_reverse_iterator>());
static_assert(test_cdata<std::span<int> const&, const int*>());
#else // ^^^ _HAS_CXX23 / !_HAS_CXX23 vvv
static_assert(test_cbegin<std::span<int> const&, std::span<int>::iterator>());
static_assert(test_cend<std::span<int> const&, std::span<int>::iterator>());
static_assert(test_crbegin<std::span<int> const&, std::reverse_iterator<std::span<int>::iterator>>());
static_assert(test_crend<std::span<int> const&, std::reverse_iterator<std::span<int>::iterator>>());
static_assert(test_cdata<std::span<int> const&, int*>());
#endif // ^^^ !_HAS_CXX23 ^^^

using valarray_int_iterator       = decltype(std::begin(std::declval<std::valarray<int>&>()));
using const_valarray_int_iterator = decltype(std::begin(std::declval<const std::valarray<int>&>()));
static_assert(test_begin<std::valarray<int>>());
static_assert(test_end<std::valarray<int>>());
static_assert(test_cbegin<std::valarray<int>>());
static_assert(test_cend<std::valarray<int>>());
static_assert(test_rbegin<std::valarray<int>>());
static_assert(test_rend<std::valarray<int>>());
static_assert(test_crbegin<std::valarray<int>>());
static_assert(test_crend<std::valarray<int>>());
static_assert(test_size<std::valarray<int>, std::size_t>());
static_assert(test_empty<std::valarray<int>, true>());
static_assert(test_data<std::valarray<int>>());
static_assert(test_cdata<std::valarray<int>>());
static_assert(test_contiguous_range<std::valarray<int>>());
static_assert(!ranges::view<std::valarray<int>>);

static_assert(test_begin<std::valarray<int> const>());
static_assert(test_end<std::valarray<int> const>());
static_assert(test_cbegin<std::valarray<int> const>());
static_assert(test_cend<std::valarray<int> const>());
static_assert(test_rbegin<std::valarray<int> const>());
static_assert(test_rend<std::valarray<int> const>());
static_assert(test_crbegin<std::valarray<int> const>());
static_assert(test_crend<std::valarray<int> const>());
static_assert(test_size<std::valarray<int> const, std::size_t>());
static_assert(test_empty<std::valarray<int> const, true>());
static_assert(test_data<std::valarray<int> const>());
static_assert(test_cdata<std::valarray<int> const>());
static_assert(test_contiguous_range<std::valarray<int> const>());
static_assert(!ranges::view<std::valarray<int> const>);

static_assert(test_begin<std::valarray<int>&, valarray_int_iterator>());
static_assert(test_end<std::valarray<int>&, valarray_int_iterator>());
static_assert(test_cbegin<std::valarray<int>&, const_valarray_int_iterator>());
static_assert(test_cend<std::valarray<int>&, const_valarray_int_iterator>());
static_assert(test_rbegin<std::valarray<int>&, std::reverse_iterator<valarray_int_iterator>>());
static_assert(test_rend<std::valarray<int>&, std::reverse_iterator<valarray_int_iterator>>());
static_assert(test_crbegin<std::valarray<int>&, std::reverse_iterator<const_valarray_int_iterator>>());
static_assert(test_crend<std::valarray<int>&, std::reverse_iterator<const_valarray_int_iterator>>());
static_assert(test_size<std::valarray<int>&, std::size_t>());
static_assert(test_empty<std::valarray<int>&, true>());
static_assert(test_data<std::valarray<int>&, int*>());
static_assert(test_cdata<std::valarray<int>&, int const*>());
static_assert(test_contiguous_range<std::valarray<int>&>());
static_assert(!ranges::view<std::valarray<int>&>);

static_assert(test_begin<std::valarray<int> const&, const_valarray_int_iterator>());
static_assert(test_end<std::valarray<int> const&, const_valarray_int_iterator>());
static_assert(test_cbegin<std::valarray<int> const&, const_valarray_int_iterator>());
static_assert(test_cend<std::valarray<int> const&, const_valarray_int_iterator>());
static_assert(test_rbegin<std::valarray<int> const&, std::reverse_iterator<const_valarray_int_iterator>>());
static_assert(test_rend<std::valarray<int> const&, std::reverse_iterator<const_valarray_int_iterator>>());
static_assert(test_crbegin<std::valarray<int> const&, std::reverse_iterator<const_valarray_int_iterator>>());
static_assert(test_crend<std::valarray<int> const&, std::reverse_iterator<const_valarray_int_iterator>>());
static_assert(test_size<std::valarray<int> const&, std::size_t>());
static_assert(test_empty<std::valarray<int> const&, true>());
static_assert(test_data<std::valarray<int> const&, int const*>());
static_assert(test_cdata<std::valarray<int> const&, int const*>());
static_assert(test_contiguous_range<std::valarray<int> const&>());
static_assert(!ranges::view<std::valarray<int> const&>);

namespace adl_block { // Validate some range concept corner cases
    struct members_only { // baseline
        constexpr int* begin() {
            return nullptr;
        }
        constexpr int* end() {
            return nullptr;
        }
        constexpr auto rbegin() {
            return std::reverse_iterator<int*>{};
        }
        constexpr auto rend() {
            return std::reverse_iterator<int*>{};
        }
        constexpr int const* begin() const {
            return nullptr;
        }
        constexpr int const* end() const {
            return nullptr;
        }
        constexpr auto rbegin() const {
            return std::reverse_iterator<int const*>{};
        }
        constexpr auto rend() const {
            return std::reverse_iterator<int const*>{};
        }
    };
    static_assert(test_begin<members_only&, int*>());
    static_assert(test_end<members_only&, int*>());
    static_assert(test_cbegin<members_only&, int const*>());
    static_assert(test_cend<members_only&, int const*>());
    static_assert(test_rbegin<members_only&, std::reverse_iterator<int*>>());
    static_assert(test_rend<members_only&, std::reverse_iterator<int*>>());
    static_assert(test_crbegin<members_only&, std::reverse_iterator<int const*>>());
    static_assert(test_crend<members_only&, std::reverse_iterator<int const*>>());
    static_assert(test_size<members_only&, std::size_t>());
    static_assert(test_empty<members_only&, true>());
    static_assert(test_data<members_only&, int*>());
    static_assert(test_cdata<members_only&, int const*>());
    static_assert(test_contiguous_range<members_only&>());

    // Verify that non-members with unacceptable return types are correctly ignored
    struct bogus_non_member : members_only {};
    void* begin(bogus_non_member); // returns non-input_or_output_iterator
    int end(bogus_non_member); // return type doesn't model sentinel_for
    auto rbegin(bogus_non_member) -> void (*)(); // returns non-input_or_output_iterator
    auto rend(bogus_non_member) -> int members_only::*; // return type doesn't model sentinel_for
    char* data(bogus_non_member); // doesn't return pointer to cv-qualified-value_type
    static_assert(test_begin<bogus_non_member&, int*>());
    static_assert(test_end<bogus_non_member&, int*>());
    static_assert(test_cbegin<bogus_non_member&, int const*>());
    static_assert(test_cend<bogus_non_member&, int const*>());
    static_assert(test_rbegin<bogus_non_member&, std::reverse_iterator<int*>>());
    static_assert(test_rend<bogus_non_member&, std::reverse_iterator<int*>>());
    static_assert(test_crbegin<bogus_non_member&, std::reverse_iterator<int const*>>());
    static_assert(test_crend<bogus_non_member&, std::reverse_iterator<int const*>>());
    static_assert(test_size<bogus_non_member&, std::size_t>());
    static_assert(test_empty<bogus_non_member&, true>());
    static_assert(test_data<bogus_non_member&, int*>());
    static_assert(test_cdata<bogus_non_member&, int const*>());
    static_assert(test_contiguous_range<bogus_non_member&>());

    // Verify that members are preferred to perfectly reasonable non-members
    template <class>
    struct prefer_member : members_only {};
    template <class T>
    int* begin(prefer_member<T>&) {
        static_assert(false);
    }
    template <class T>
    int* end(prefer_member<T>&) {
        static_assert(false);
    }
    template <class T>
    std::reverse_iterator<int*> rbegin(prefer_member<T>&) {
        static_assert(false);
    }
    template <class T>
    std::reverse_iterator<int*> rend(prefer_member<T>&) {
        static_assert(false);
    }
    template <class T>
    int* data(prefer_member<T>&) {
        static_assert(false);
    }

    constexpr bool test() {
        static_assert(test_begin<prefer_member<void>&, int*>());
        static_assert(test_end<prefer_member<void>&, int*>());
        static_assert(test_cbegin<prefer_member<void>&, int const*>());
        static_assert(test_cend<prefer_member<void>&, int const*>());
        static_assert(test_rbegin<prefer_member<void>&, std::reverse_iterator<int*>>());
        static_assert(test_rend<prefer_member<void>&, std::reverse_iterator<int*>>());
        static_assert(test_crbegin<prefer_member<void>&, std::reverse_iterator<int const*>>());
        static_assert(test_crend<prefer_member<void>&, std::reverse_iterator<int const*>>());
        static_assert(test_size<prefer_member<void>&, std::size_t>());
        static_assert(test_empty<prefer_member<void>&, true>());
        static_assert(test_data<prefer_member<void>&, int*>());
        static_assert(test_cdata<prefer_member<void>&, int const*>());
        static_assert(test_contiguous_range<prefer_member<void>&>());

        prefer_member<void> r;

        (void) ranges::begin(r);
        (void) ranges::end(r);
        (void) ranges::cbegin(r);
        (void) ranges::cend(r);
        (void) ranges::rbegin(r);
        (void) ranges::rend(r);
        (void) ranges::crbegin(r);
        (void) ranges::crend(r);
        (void) ranges::empty(r);
        (void) ranges::size(r);
        (void) ranges::data(r);
        (void) ranges::cdata(r);

        return true;
    }
    static_assert(test());

    // Verify that ranges::c?r?end reject non-ranges
    struct no_begin {
        int* end() const;
    };
    static_assert(test_begin<no_begin&>());
    static_assert(test_end<no_begin&>());
    static_assert(test_cbegin<no_begin&>());
    static_assert(test_cend<no_begin&>());
    static_assert(test_rbegin<no_begin&>());
    static_assert(test_rend<no_begin&>());
    static_assert(test_crbegin<no_begin&>());
    static_assert(test_crend<no_begin&>());
    static_assert(test_size<no_begin&>());
    static_assert(test_empty<no_begin&, false>());
    static_assert(test_data<no_begin&>());
    static_assert(test_cdata<no_begin&>());
    static_assert(test_non_range<no_begin&>());
} // namespace adl_block

namespace disable_sized_range_testing {
    struct program_defined_type {};

    struct weird_range {
        int* begin() const;
        int* end() const;
    };
} // namespace disable_sized_range_testing

template <>
constexpr bool ranges::disable_sized_range<disable_sized_range_testing::program_defined_type[42]> = true;
template <>
constexpr bool ranges::disable_sized_range<disable_sized_range_testing::weird_range> = true;

// Overriding disable_sized_range for an array type has no effect:
static_assert(ranges::sized_range<disable_sized_range_testing::program_defined_type (&)[42]>);
// Overriding disable_sized_range for a range whose sentinel and iterator model sized_sentinel_for has no effect:
static_assert(ranges::sized_range<disable_sized_range_testing::weird_range>);

template <class T>
constexpr bool test_array_ish() { // An actual runtime test!
    T array_ish = {0, 1, 2};

    assert(ranges::empty(array_ish) == false);

    int const* const first = &*std::begin(array_ish);
    assert(ranges::data(array_ish) == first);
    assert(&*ranges::begin(array_ish) == first);
    assert(&*ranges::cbegin(array_ish) == first);

    std::size_t const n = std::size(array_ish);
    assert(ranges::size(array_ish) == n);

    int const* const last = first + n;
    assert(1 + &*(ranges::end(array_ish) - 1) == last);
    assert(1 + &*(ranges::cend(array_ish) - 1) == last);

    int count = 0;
    for (auto p = ranges::begin(array_ish), e = ranges::end(array_ish); p != e; ++p) {
        assert(*p == count++);
    }
    assert(count == 3);

    for (auto p = ranges::rbegin(array_ish), e = ranges::rend(array_ish); p != e; ++p) {
        assert(*p == --count);
    }
    assert(count == 0);

    for (auto p = ranges::cbegin(array_ish), e = ranges::cend(array_ish); p != e; ++p) {
        assert(*p == count++);
    }
    assert(count == 3);

    for (auto p = ranges::crbegin(array_ish), e = ranges::crend(array_ish); p != e; ++p) {
        assert(*p == --count);
    }
    assert(count == 0);

    return true;
}

namespace nothrow_testing {
    template <unsigned int I, bool NoThrow>
    struct range {
        int elements_[3];

        // begin/end are members for I == 0, and non-members otherwise
        int* begin() noexcept(NoThrow)
            requires (I == 0)
        {
            return elements_;
        }
        int* end() noexcept(NoThrow)
            requires (I == 0)
        {
            return elements_ + 3;
        }
        int const* begin() const noexcept(NoThrow)
            requires (I == 0)
        {
            return elements_;
        }
        int const* end() const noexcept(NoThrow)
            requires (I == 0)
        {
            return elements_ + 3;
        }

        // rbegin/rend are members for I == 0, not provided for I == 1, and non-members otherwise
        // (Not providing operations allows us to test the library-provided fallback behavior)
        int* rbegin() noexcept(NoThrow)
            requires (I == 0)
        {
            return elements_;
        }
        int* rend() noexcept(NoThrow)
            requires (I == 0)
        {
            return elements_ + 3;
        }
        int const* rbegin() const noexcept(NoThrow)
            requires (I == 0)
        {
            return elements_;
        }
        int const* rend() const noexcept(NoThrow)
            requires (I == 0)
        {
            return elements_ + 3;
        }

        // empty is not provided when I == 1
        bool empty() const noexcept(NoThrow)
            requires (I != 1)
        {
            return false;
        }

        // data is not provided when I == 2
        int* data() noexcept(NoThrow)
            requires (I != 2)
        {
            return elements_;
        }
        int const* data() const noexcept(NoThrow)
            requires (I != 2)
        {
            return elements_;
        }

        // size is not provided when I == 3
        std::size_t size() const noexcept(NoThrow)
            requires (I != 3)
        {
            return 3;
        }
    };

    template <unsigned int I, bool NoThrow>
    int* begin(range<I, NoThrow>& a) noexcept(NoThrow)
        requires (I != 0)
    {
        return a.elements_;
    }
    template <unsigned int I, bool NoThrow>
    int* end(range<I, NoThrow>& a) noexcept(NoThrow)
        requires (I != 0)
    {
        return a.elements_ + 3;
    }
    template <unsigned int I, bool NoThrow>
    int const* begin(range<I, NoThrow> const& a) noexcept(NoThrow)
        requires (I != 0)
    {
        return a.elements_;
    }
    template <unsigned int I, bool NoThrow>
    int const* end(range<I, NoThrow> const& a) noexcept(NoThrow)
        requires (I != 0)
    {
        return a.elements_ + 3;
    }

    template <unsigned int I, bool NoThrow>
    int* rbegin(range<I, NoThrow>& a) noexcept(NoThrow)
        requires (I > 2)
    {
        return a.elements_;
    }
    template <unsigned int I, bool NoThrow>
    int* rend(range<I, NoThrow>& a) noexcept(NoThrow)
        requires (I > 2)
    {
        return a.elements_ + 3;
    }
    template <unsigned int I, bool NoThrow>
    int const* rbegin(range<I, NoThrow> const& a) noexcept(NoThrow)
        requires (I > 2)
    {
        return a.elements_;
    }
    template <unsigned int I, bool NoThrow>
    int const* rend(range<I, NoThrow> const& a) noexcept(NoThrow)
        requires (I > 2)
    {
        return a.elements_ + 3;
    }

    template <class T, bool Nothrow>
    constexpr bool test() {
        T t = {{0, 1, 2}};

        static_assert(noexcept(ranges::begin(t)) == Nothrow);
        static_assert(noexcept(ranges::end(t)) == Nothrow);
        static_assert(noexcept(ranges::cbegin(t)) == Nothrow);
        static_assert(noexcept(ranges::cend(t)) == Nothrow);
        static_assert(noexcept(ranges::rbegin(t)) == Nothrow);
        static_assert(noexcept(ranges::rend(t)) == Nothrow);
        static_assert(noexcept(ranges::crbegin(t)) == Nothrow);
        static_assert(noexcept(ranges::crend(t)) == Nothrow);
        static_assert(noexcept(ranges::empty(t)) == Nothrow);
        static_assert(noexcept(ranges::size(t)) == Nothrow);
        static_assert(noexcept(ranges::ssize(t)) == Nothrow);
        static_assert(noexcept(ranges::data(t)) == Nothrow);
        static_assert(noexcept(ranges::cdata(t)) == Nothrow);

        return true;
    }

    // Validate conditional noexcept
    static_assert(nothrow_testing::test<range<0, true>, true>());
    static_assert(nothrow_testing::test<range<1, true>, true>());
    static_assert(nothrow_testing::test<range<2, true>, true>());
    static_assert(nothrow_testing::test<range<3, true>, true>());
    static_assert(nothrow_testing::test<range<4, true>, true>());
    static_assert(nothrow_testing::test<range<0, false>, false>());
    static_assert(nothrow_testing::test<range<1, false>, false>());
    static_assert(nothrow_testing::test<range<2, false>, false>());
    static_assert(nothrow_testing::test<range<3, false>, false>());
    static_assert(nothrow_testing::test<range<4, false>, false>());
} // namespace nothrow_testing

namespace subsumption_testing {
    // Validate that the "indirectly_readable" range concepts properly subsume (doubly important given that we spell
    // them differently than does the working draft)
    enum range_type {
        exactly_range,
        exactly_input_range,
        exactly_forward_range,
        exactly_bidirectional_range,
        exactly_random_access_range,
        exactly_contiguous_range
    };

    template <ranges::range R>
    constexpr range_type f() {
        return exactly_range;
    }
    template <ranges::input_range R>
    constexpr range_type f() {
        return exactly_input_range;
    }
    template <ranges::forward_range R>
    constexpr range_type f() {
        return exactly_forward_range;
    }
    template <ranges::bidirectional_range R>
    constexpr range_type f() {
        return exactly_bidirectional_range;
    }
    template <ranges::random_access_range R>
    constexpr range_type f() {
        return exactly_random_access_range;
    }
    template <ranges::contiguous_range R>
    constexpr range_type f() {
        return exactly_contiguous_range;
    }

    struct simple_input_range {
        struct sentinel {
            bool operator==(std::istream_iterator<int>) const;
        };
        std::istream_iterator<int> begin() const;
        static sentinel end();
    };

    static_assert(f<simple_input_range>() == exactly_input_range);
    static_assert(f<std::forward_list<int>>() == exactly_forward_range);
    static_assert(f<std::list<int>>() == exactly_bidirectional_range);
    static_assert(f<std::deque<int>>() == exactly_random_access_range);
    static_assert(f<int (&)[42]>() == exactly_contiguous_range);
} // namespace subsumption_testing

namespace borrowed_range_testing {
    template <class Rng, class Iterator, class Sentinel = Iterator, class RIterator = std::reverse_iterator<Iterator>,
        class RSentinel = RIterator>
    constexpr bool test_borrowed_range() {
        // Validate that rvalue/lvalue const/non-const Rng models borrowed_range
        static_assert(Decayed<Rng>);

        static_assert(test_begin<Rng, Iterator>());
        static_assert(test_end<Rng, Sentinel>());
        static_assert(test_rbegin<Rng, RIterator>());
        static_assert(test_rend<Rng, RSentinel>());
        static_assert(ranges::borrowed_range<Rng>);

        static_assert(test_begin<Rng&, Iterator>());
        static_assert(test_end<Rng&, Sentinel>());
        static_assert(test_rbegin<Rng&, RIterator>());
        static_assert(test_rend<Rng&, RSentinel>());
        static_assert(ranges::borrowed_range<Rng&>);

        static_assert(test_begin<Rng const, Iterator>());
        static_assert(test_end<Rng const, Sentinel>());
        static_assert(test_rbegin<Rng const, RIterator>());
        static_assert(test_rend<Rng const, RSentinel>());
        static_assert(ranges::borrowed_range<Rng const>);

        static_assert(test_begin<Rng const&, Iterator>());
        static_assert(test_end<Rng const&, Sentinel>());
        static_assert(test_rbegin<Rng const&, RIterator>());
        static_assert(test_rend<Rng const&, RSentinel>());
        static_assert(ranges::borrowed_range<Rng const&>);

        return true;
    }

    static_assert(test_borrowed_range<std::string_view, std::string_view::iterator>());
    static_assert(test_borrowed_range<std::wstring_view, std::wstring_view::iterator>());
    static_assert(test_borrowed_range<std::span<int>, std::span<int>::iterator>());
    static_assert(test_borrowed_range<std::span<int, 42>, std::span<int, 42>::iterator>());
    static_assert(test_borrowed_range<ranges::subrange<int*, int*>, int*>());
    static_assert(test_borrowed_range<ranges::ref_view<int[42]>, int*>());
    static_assert(test_borrowed_range<ranges::iota_view<int, int>, ranges::iterator_t<ranges::iota_view<int, int>>>());

    struct simple_borrowed_range {
        int* begin() const {
            return nullptr;
        }
        int* end() const {
            return nullptr;
        }
    };

    struct less_simple_borrowed_range {
        friend int* begin(less_simple_borrowed_range) {
            return nullptr;
        }
        friend int* end(less_simple_borrowed_range) {
            return nullptr;
        }
    };
} // namespace borrowed_range_testing

template <>
inline constexpr bool std::ranges::enable_borrowed_range<borrowed_range_testing::simple_borrowed_range> = true;
template <>
inline constexpr bool std::ranges::enable_borrowed_range<borrowed_range_testing::less_simple_borrowed_range> = true;

namespace borrowed_range_testing {
    static_assert(test_borrowed_range<simple_borrowed_range, int*>());
    static_assert(test_borrowed_range<less_simple_borrowed_range, int*>());
} // namespace borrowed_range_testing

template <bool AllowNonConst, bool AllowConst, bool AllowSize>
struct arbitrary_range {
    arbitrary_range()                             = default;
    arbitrary_range(arbitrary_range&&)            = default;
    arbitrary_range& operator=(arbitrary_range&&) = default;

    int* begin()
        requires AllowNonConst;
    int* end()
        requires AllowNonConst;

    int const* begin() const
        requires AllowConst;
    int const* end() const
        requires AllowConst;

    unsigned char size() const
        requires AllowSize;
};

using mutable_unsized_range      = arbitrary_range<true, true, false>;
using mutable_only_no_size_range = arbitrary_range<true, false, false>;
using immutable_unsized_range    = arbitrary_range<false, true, false>;
using mutable_sized_range        = arbitrary_range<true, true, true>;
using mutable_only_sized_range   = arbitrary_range<true, false, true>;
using immutable_sized_range      = arbitrary_range<false, true, true>;

template <class Base>
struct badsized_range : Base { // size() launches the missiles.
    badsized_range()                            = default;
    badsized_range(badsized_range&&)            = default;
    badsized_range& operator=(badsized_range&&) = default;

    [[noreturn]] int size() const {
        static_assert(false);
    }

    [[noreturn]] friend int size(const badsized_range&) {
        static_assert(false);
    }
};

using mutable_badsized_range      = badsized_range<mutable_sized_range>;
using mutable_only_badsized_range = badsized_range<mutable_only_sized_range>;
using immutable_badsized_range    = badsized_range<immutable_sized_range>;

template <class T>
constexpr bool ranges::disable_sized_range<badsized_range<T>> = true;

// "strange" in that const-ness affects the iterator type
struct strange_view {
    strange_view()                          = default;
    strange_view(strange_view&&)            = default;
    strange_view& operator=(strange_view&&) = default;

    int* begin();
    int const* begin() const;

    int* end();
    int const* end() const;
};

struct strange_view2 : strange_view, ranges::view_base {};
struct strange_view3 : strange_view2 {}; // not truly a view since enable_view<strange_view3> is false (see below)
struct strange_view4 : strange_view, ranges::view_interface<strange_view4> {};
struct strange_view5 : strange_view4, ranges::view_interface<strange_view5> {
    // not truly a view since enable_view<strange_view5> is false due to multiple inheritance from view_interface
};

// Verify that specializations of view_interface do not inherit from view_base
static_assert(!std::is_base_of_v<std::ranges::view_base, ranges::view_interface<strange_view4>>);
static_assert(!std::is_base_of_v<std::ranges::view_base, ranges::view_interface<strange_view5>>);

// Verify that enable_view<T&> or enable_view<T&&> is never true
static_assert(ranges::enable_view<strange_view4>);
static_assert(!ranges::enable_view<strange_view4&>);
static_assert(!ranges::enable_view<const strange_view4&>);
static_assert(!ranges::enable_view<strange_view4&&>);
static_assert(!ranges::enable_view<const strange_view4&&>);

// Verify that enable_view<view_interface<T>> is false, i.e., strict derivation is required
static_assert(!ranges::enable_view<ranges::view_interface<strange_view4>>);
static_assert(!ranges::enable_view<ranges::view_interface<strange_view5>>);

// Verify that enable_view ignores cv-qualification by default
template <class T>
void test_enable_view_cv() { // COMPILE-ONLY
    static_assert(ranges::enable_view<const T> == ranges::enable_view<T>);
    static_assert(ranges::enable_view<volatile T> == ranges::enable_view<T>);
    static_assert(ranges::enable_view<const volatile T> == ranges::enable_view<T>);
}

void test_enable_view_cv_all() { // COMPILE-ONLY
    test_enable_view_cv<strange_view4>();
    test_enable_view_cv<strange_view5>();
    test_enable_view_cv<ranges::view_interface<strange_view4>>();
    test_enable_view_cv<ranges::view_interface<strange_view5>>();
}

// Verify that the derived-from-view_interface mechanism can handle uses of incomplete types whenever possible
struct incomplet;

template <class T>
struct value_holder {
    T t;
};

static_assert(!ranges::enable_view<value_holder<incomplet>*>);

template <>
inline constexpr bool ranges::enable_view<strange_view> = true;
template <>
inline constexpr bool ranges::enable_view<strange_view3> = false;

// Test types that satisfy `view` only when being cv-qualified.

struct view_when_const1 : ranges::view_base {
    view_when_const1(const view_when_const1&&);
    const view_when_const1& operator=(const view_when_const1&&) const;

    friend void swap(const view_when_const1&, const view_when_const1&);

    int* begin() const;
    int* end() const;
};

struct view_when_const2 : ranges::view_interface<view_when_const2> {
    view_when_const2(const view_when_const2&&);
    const view_when_const2& operator=(const view_when_const2&&) const;

    friend void swap(const view_when_const2&, const view_when_const2&);

    int* begin() const;
    int* end() const;

    // avoid checking ranges::view_interface<view_when_const2>::size()
    void size()       = delete;
    void size() const = delete;
};

struct view_when_volatile1 : ranges::view_base {
    view_when_volatile1(volatile view_when_volatile1&&);
    volatile view_when_volatile1& operator=(volatile view_when_volatile1&&) volatile;

    friend void swap(volatile view_when_volatile1&, volatile view_when_volatile1&);

    int* begin() volatile;
    int* end() volatile;
};

struct view_when_volatile2 : ranges::view_interface<view_when_volatile2> {
    view_when_volatile2(volatile view_when_volatile2&&);
    volatile view_when_volatile2& operator=(volatile view_when_volatile2&&) volatile;

    friend void swap(volatile view_when_volatile2&, volatile view_when_volatile2&);

    int* begin() volatile;
    int* end() volatile;

    // avoid checking ranges::view_interface<view_when_volatile2>::size()
    void size()       = delete;
    void size() const = delete;
};

struct view_when_const_volatile1 : ranges::view_base {
    view_when_const_volatile1(const volatile view_when_const_volatile1&&);
    const volatile view_when_const_volatile1& operator=(const volatile view_when_const_volatile1&&) const volatile;

    friend void swap(const volatile view_when_const_volatile1&, const volatile view_when_const_volatile1&);

    int* begin() const volatile;
    int* end() const volatile;
};

struct view_when_const_volatile2 : ranges::view_interface<view_when_const_volatile2> {
    view_when_const_volatile2(const volatile view_when_const_volatile2&&);
    const volatile view_when_const_volatile2& operator=(const volatile view_when_const_volatile2&&) const volatile;

    friend void swap(const volatile view_when_const_volatile2&, const volatile view_when_const_volatile2&);

    int* begin() const volatile;
    int* end() const volatile;

    // avoid checking ranges::view_interface<view_when_const_volatile2>::size()
    void size()       = delete;
    void size() const = delete;
};

namespace exhaustive_size_and_view_test {
    template <class Rng, bool IsView = false, class Iterator = invalid_type, class Size = invalid_type>
    constexpr bool test() {
        static_assert(ranges::range<Rng> == is_valid<Iterator>);
        if constexpr (is_valid<Iterator>) {
            static_assert(std::same_as<ranges::iterator_t<Rng>, Iterator>);
        }

        static_assert(ranges::sized_range<Rng> == is_valid<Size>);
        if constexpr (is_valid<Size>) {
            static_assert(std::same_as<decltype(ranges::size(std::declval<Rng>())), Size>);

            using SignedSize = std::common_type_t<std::ptrdiff_t, std::make_signed_t<Size>>;
            static_assert(std::same_as<decltype(ranges::ssize(std::declval<Rng>())), SignedSize>);
        }

        static_assert(ranges::view<Rng> == IsView);
        return true;
    }

    using I  = int*;
    using CI = int const*;
    using S  = std::size_t;
    using UC = unsigned char;

    static_assert(test<mutable_unsized_range, false, I, S>());
    static_assert(test<mutable_unsized_range&, false, I, S>());
    static_assert(test<mutable_unsized_range const, false, CI, S>());
    static_assert(test<mutable_unsized_range const&, false, CI, S>());

    static_assert(test<mutable_only_no_size_range, false, I, S>());
    static_assert(test<mutable_only_no_size_range&, false, I, S>());
    static_assert(test<mutable_only_no_size_range const>());
    static_assert(test<mutable_only_no_size_range const&>());

    static_assert(test<immutable_unsized_range, false, CI, S>());
    static_assert(test<immutable_unsized_range&, false, CI, S>());
    static_assert(test<immutable_unsized_range const, false, CI, S>());
    static_assert(test<immutable_unsized_range const&, false, CI, S>());

    static_assert(test<mutable_sized_range, false, I, UC>());
    static_assert(test<mutable_sized_range&, false, I, UC>());
    static_assert(test<mutable_sized_range const, false, CI, UC>());
    static_assert(test<mutable_sized_range const&, false, CI, UC>());

    static_assert(test<mutable_only_sized_range, false, I, UC>());
    static_assert(test<mutable_only_sized_range&, false, I, UC>());
    static_assert(test<mutable_only_sized_range const>());
    static_assert(test<mutable_only_sized_range const&>());

    static_assert(test<immutable_sized_range, false, CI, UC>());
    static_assert(test<immutable_sized_range&, false, CI, UC>());
    static_assert(test<immutable_sized_range const, false, CI, UC>());
    static_assert(test<immutable_sized_range const&, false, CI, UC>());

    static_assert(test<mutable_badsized_range, false, I, S>());
    static_assert(test<mutable_badsized_range&, false, I, S>());
    static_assert(test<mutable_badsized_range const, false, CI, S>());
    static_assert(test<mutable_badsized_range const&, false, CI, S>());

    static_assert(test<mutable_only_badsized_range, false, I, S>());
    static_assert(test<mutable_only_badsized_range&, false, I, S>());
    static_assert(test<mutable_only_badsized_range const>());
    static_assert(test<mutable_only_badsized_range const&>());

    static_assert(test<immutable_badsized_range, false, CI, S>());
    static_assert(test<immutable_badsized_range&, false, CI, S>());
    static_assert(test<immutable_badsized_range const, false, CI, S>());
    static_assert(test<immutable_badsized_range const&, false, CI, S>());

    static_assert(test<strange_view, true, I, S>());
    static_assert(test<strange_view&, false, I, S>());
    static_assert(test<strange_view const, false, CI, S>());
    static_assert(test<strange_view const&, false, CI, S>());

    static_assert(test<strange_view2, true, I, S>());
    static_assert(test<strange_view2&, false, I, S>());
    static_assert(test<strange_view2 const, false, CI, S>());
    static_assert(test<strange_view2 const&, false, CI, S>());

    static_assert(test<strange_view3, false, I, S>());
    static_assert(test<strange_view3&, false, I, S>());
    static_assert(test<strange_view3 const, false, CI, S>());
    static_assert(test<strange_view3 const&, false, CI, S>());

    static_assert(test<strange_view4, true, I, S>());
    static_assert(test<strange_view4&, false, I, S>());
    static_assert(test<strange_view4 const, false, CI, S>());
    static_assert(test<strange_view4 const&, false, CI, S>());

    template <class = void>
    constexpr bool strict_test_case() {
        if constexpr (!is_permissive) {
            static_assert(test<strange_view5, false, I, S>());
        }
        return true;
    }
    static_assert(strict_test_case());
    static_assert(test<strange_view5&, false, I, S>());
    static_assert(test<strange_view5 const, false, CI, S>());
    static_assert(test<strange_view5 const&, false, CI, S>());

    static_assert(test<view_when_const1, false, I, S>());
    static_assert(test<view_when_const1 const, true, I, S>());
    static_assert(test<view_when_const1&, false, I, S>());
    static_assert(test<view_when_const1 const&, false, I, S>());

    static_assert(test<view_when_const2, false, I, S>());
    static_assert(test<view_when_const2 const, true, I, S>());
    static_assert(test<view_when_const2&, false, I, S>());
    static_assert(test<view_when_const2 const&, false, I, S>());

    static_assert(test<view_when_volatile1, false, I, S>());
    static_assert(test<view_when_volatile1 const, false>());
    static_assert(test<view_when_volatile1&, false, I, S>());
    static_assert(test<view_when_volatile1 const&, false>());

    static_assert(test<view_when_volatile2, false, I, S>());
    static_assert(test<view_when_volatile2 const, false>());
    static_assert(test<view_when_volatile2&, false, I, S>());
    static_assert(test<view_when_volatile2 const&, false>());

    static_assert(test<view_when_volatile1 volatile, true, I, S>());
    static_assert(test<view_when_volatile1 const volatile, false>());
    static_assert(test<view_when_volatile1 volatile&, false, I, S>());
    static_assert(test<view_when_volatile1 const volatile&, false>());

    static_assert(test<view_when_volatile2 volatile, true, I, S>());
    static_assert(test<view_when_volatile2 const volatile, false>());
    static_assert(test<view_when_volatile2 volatile&, false, I, S>());
    static_assert(test<view_when_volatile2 const volatile&, false>());

    static_assert(test<view_when_const_volatile1, false, I, S>());
    static_assert(test<view_when_const_volatile1 const, false, I, S>());
    static_assert(test<view_when_const_volatile1&, false, I, S>());
    static_assert(test<view_when_const_volatile1 const&, false, I, S>());

    static_assert(test<view_when_const_volatile2, false, I, S>());
    static_assert(test<view_when_const_volatile2 const, false, I, S>());
    static_assert(test<view_when_const_volatile2&, false, I, S>());
    static_assert(test<view_when_const_volatile2 const&, false, I, S>());

    static_assert(test<view_when_const_volatile1 volatile, false, I, S>());
    static_assert(test<view_when_const_volatile1 const volatile, true, I, S>());
    static_assert(test<view_when_const_volatile1 volatile&, false, I, S>());
    static_assert(test<view_when_const_volatile1 const volatile&, false, I, S>());

    static_assert(test<view_when_const_volatile2 volatile, false, I, S>());
    static_assert(test<view_when_const_volatile2 const volatile, true, I, S>());
    static_assert(test<view_when_const_volatile2 volatile&, false, I, S>());
    static_assert(test<view_when_const_volatile2 const volatile&, false, I, S>());
} // namespace exhaustive_size_and_view_test

// Validate output_range
static_assert(ranges::output_range<int[42], int>);
static_assert(ranges::output_range<int[42], int&>);
static_assert(ranges::output_range<int[42], int const>);
static_assert(ranges::output_range<int[42], int const&>);

static_assert(ranges::output_range<int[42], short>);
static_assert(ranges::output_range<int[42], short&>);
static_assert(ranges::output_range<int[42], short const>);
static_assert(ranges::output_range<int[42], short const&>);

static_assert(!ranges::output_range<int[42], char*>);
static_assert(!ranges::output_range<int[42], char*&>);
static_assert(!ranges::output_range<int[42], char* const>);
static_assert(!ranges::output_range<int[42], char* const&>);

template <std::input_or_output_iterator I, std::sentinel_for<I> S>
constexpr I complicated_algorithm(I i, S const s) {
    while (i != s) {
        ++i;
    }
    return i;
}

template <ranges::range R>
constexpr ranges::iterator_t<R> complicated_algorithm(R&& r) {
    return complicated_algorithm(ranges::begin(r), ranges::end(r));
}

template <class T>
struct array_view : ranges::view_base {
    T* first_;
    std::size_t n_;

    array_view() = default;
    template <std::size_t N>
    constexpr array_view(T (&a)[N]) : first_{a}, n_{N} {}

    constexpr auto begin() const {
        return first_;
    }
    constexpr auto end() const {
        return first_ + n_;
    }
    constexpr auto size() const {
        return n_;
    }
};

constexpr bool complicated_algorithm_test() {
    int const some_ints[] = {2, 3, 5, 7};
    assert(complicated_algorithm(some_ints) == ranges::end(some_ints));
    assert(complicated_algorithm(array_view{some_ints}) == ranges::end(some_ints));
    static_assert(ranges::view<decltype(array_view{some_ints})>);
    return true;
}

// Regression test for DevCom-739010 (aka VSO-985597)
// https://developercommunity.visualstudio.com/content/problem/739010/meow.html
// which allows overload resolution to prefer a hidden friend `const T&` overload of `begin`
// for an rvalue `T` over the deleted `begin(T&&)` instantiated from the poison pill.
template <class T>
struct bad_string_view {
    friend T const* begin(bad_string_view const&) {
        return nullptr;
    }
};
static_assert(!CanBegin<bad_string_view<char>>);

// Validate viewable_range
static_assert(!ranges::viewable_range<void>);
static_assert(!ranges::viewable_range<int>);
static_assert(!ranges::viewable_range<int&>);
static_assert(!ranges::viewable_range<int*>);
static_assert(!ranges::viewable_range<int()>);
static_assert(!ranges::viewable_range<int (&)()>);
static_assert(!ranges::viewable_range<int (*)()>);
static_assert(!ranges::viewable_range<int() const>);

static_assert(ranges::viewable_range<std::vector<int>&>);
static_assert(ranges::viewable_range<std::vector<int> const&>);
static_assert(ranges::viewable_range<std::vector<int>>);
static_assert(!ranges::viewable_range<std::vector<int> const>);
static_assert(ranges::viewable_range<std::string_view&>);
static_assert(ranges::viewable_range<std::string_view const&>);
static_assert(ranges::viewable_range<std::string_view>);
static_assert(ranges::viewable_range<std::string_view const>);
static_assert(ranges::viewable_range<std::span<int>&>);
static_assert(ranges::viewable_range<std::span<int> const&>);
static_assert(ranges::viewable_range<std::span<int>>);
static_assert(ranges::viewable_range<std::span<int> const>);

namespace poison_pill_test {
    template <class T>
    int* begin(T&);
    template <class T>
    int const* begin(T const&);
    template <class T>
    int* end(T&);
    template <class T>
    int const* end(T const&);
    template <class T>
    std::reverse_iterator<int*> rbegin(T&);
    template <class T>
    std::reverse_iterator<int const*> rbegin(T const&);
    template <class T>
    std::reverse_iterator<int*> rend(T&);
    template <class T>
    std::reverse_iterator<int const*> rend(T const&);
    template <class T>
    std::size_t size(T&);
    template <class T>
    std::size_t size(T const&);

    struct some_type {};

    // The above underconstrained templates were blocked by the poison pills for the ranges CPOs
    // until P2602R2 removed them.

    static_assert(CanBegin<some_type&>);
    static_assert(CanBegin<some_type const&>);
    static_assert(CanEnd<some_type&>);
    static_assert(CanEnd<some_type const&>);
    static_assert(CanRBegin<some_type&>);
    static_assert(CanRBegin<some_type const&>);
    static_assert(CanREnd<some_type&>);
    static_assert(CanREnd<some_type const&>);
    static_assert(CanSize<some_type&>);
    static_assert(CanSize<some_type const&>);
} // namespace poison_pill_test

#ifndef _M_CEE // TRANSITION, VSO-1659496
// N.B. reverse_iterator<value_holder<incomplet>*> can't be made ADL-proof and doesn't model bidirectional_iterator,
// so the rbegin()/rend() member functions return the same iterators as begin()/end().
namespace adl_proof_test {
    struct validating_member_range {
        value_holder<incomplet>* elems_[1];

        constexpr value_holder<incomplet>** begin() noexcept {
            return elems_;
        }
        constexpr value_holder<incomplet>* const* begin() const noexcept {
            return elems_;
        }

        constexpr value_holder<incomplet>** end() noexcept {
            return elems_ + 1;
        }
        constexpr value_holder<incomplet>* const* end() const noexcept {
            return elems_ + 1;
        }

        constexpr value_holder<incomplet>** rbegin() noexcept {
            return elems_;
        }
        constexpr value_holder<incomplet>* const* rbegin() const noexcept {
            return elems_;
        }

        constexpr value_holder<incomplet>** rend() noexcept {
            return elems_ + 1;
        }
        constexpr value_holder<incomplet>* const* rend() const noexcept {
            return elems_ + 1;
        }

        constexpr value_holder<incomplet>** data() noexcept {
            return elems_;
        }
        constexpr value_holder<incomplet>* const* data() const noexcept {
            return elems_;
        }
    };

    static_assert(CanBegin<validating_member_range&>);
    static_assert(CanBegin<const validating_member_range&>);
    static_assert(CanCBegin<validating_member_range&>);
    static_assert(CanCBegin<const validating_member_range&>);

    static_assert(CanEnd<validating_member_range&>);
    static_assert(CanEnd<const validating_member_range&>);
    static_assert(CanCEnd<validating_member_range&>);
    static_assert(CanCEnd<const validating_member_range&>);

    static_assert(CanRBegin<validating_member_range&>);
    static_assert(CanRBegin<const validating_member_range&>);
    static_assert(CanCRBegin<validating_member_range&>);
    static_assert(CanCRBegin<const validating_member_range&>);

    static_assert(CanREnd<validating_member_range&>);
    static_assert(CanREnd<const validating_member_range&>);
    static_assert(CanCREnd<validating_member_range&>);
    static_assert(CanCREnd<const validating_member_range&>);

    static_assert(CanData<validating_member_range&>);
    static_assert(CanData<const validating_member_range&>);
    static_assert(CanCData<validating_member_range&>);
    static_assert(CanCData<const validating_member_range&>);

    struct validating_nonmember_range {
        value_holder<incomplet>* elems_[1];

        friend constexpr value_holder<incomplet>** begin(validating_nonmember_range& r) noexcept {
            return r.elems_;
        }
        friend constexpr value_holder<incomplet>* const* begin(const validating_nonmember_range& r) noexcept {
            return r.elems_;
        }

        friend constexpr value_holder<incomplet>** end(validating_nonmember_range& r) noexcept {
            return r.elems_ + 1;
        }
        friend constexpr value_holder<incomplet>* const* end(const validating_nonmember_range& r) noexcept {
            return r.elems_ + 1;
        }

        friend constexpr value_holder<incomplet>** rbegin(validating_nonmember_range& r) noexcept {
            return r.elems_;
        }
        friend constexpr value_holder<incomplet>* const* rbegin(const validating_nonmember_range& r) noexcept {
            return r.elems_;
        }

        friend constexpr value_holder<incomplet>** rend(validating_nonmember_range& r) noexcept {
            return r.elems_ + 1;
        }
        friend constexpr value_holder<incomplet>* const* rend(const validating_nonmember_range& r) noexcept {
            return r.elems_ + 1;
        }
    };

    static_assert(CanBegin<validating_nonmember_range&>);
    static_assert(CanBegin<const validating_nonmember_range&>);
    static_assert(CanCBegin<validating_nonmember_range&>);
    static_assert(CanCBegin<const validating_nonmember_range&>);

    static_assert(CanEnd<validating_nonmember_range&>);
    static_assert(CanEnd<const validating_nonmember_range&>);
    static_assert(CanCEnd<validating_nonmember_range&>);
    static_assert(CanCEnd<const validating_nonmember_range&>);

    static_assert(CanRBegin<validating_nonmember_range&>);
    static_assert(CanRBegin<const validating_nonmember_range&>);
    static_assert(CanCRBegin<validating_nonmember_range&>);
    static_assert(CanCRBegin<const validating_nonmember_range&>);

    static_assert(CanREnd<validating_nonmember_range&>);
    static_assert(CanREnd<const validating_nonmember_range&>);
    static_assert(CanCREnd<validating_nonmember_range&>);
    static_assert(CanCREnd<const validating_nonmember_range&>);

    struct nonsizable_type {
        constexpr value_holder<incomplet>* size() const noexcept {
            return nullptr;
        }

        friend constexpr value_holder<incomplet>* size(nonsizable_type) noexcept {
            return nullptr;
        }
    };

    static_assert(CanSize<validating_member_range>);
    static_assert(CanSize<validating_nonmember_range>);
    static_assert(!CanSize<nonsizable_type>);
} // namespace adl_proof_test
#endif // ^^^ no workaround ^^^

namespace unwrapped_begin_end {
    // Validate the iterator-unwrapping range access CPOs ranges::_Ubegin and ranges::_Uend
    using test::CanCompare, test::CanDifference, test::Common, test::ProxyRef, test::Sized, test::WrappedState;

    template <WrappedState IterWrapped, WrappedState SentWrapped = IterWrapped>
    struct range {
        using I = test::iterator<std::forward_iterator_tag, int, CanDifference::no, CanCompare::yes, ProxyRef::yes,
            IterWrapped>;
        using S = test::sentinel<int, SentWrapped>;

        I begin() const;
        S end() const;
    };

    struct with_unchecked : range<WrappedState::wrapped> {
        bool begin_called_ = false;
        bool end_called_   = false;

        [[nodiscard]] constexpr range<WrappedState::unwrapped>::I _Unchecked_begin() {
            begin_called_ = true;
            return {};
        }
        [[nodiscard]] constexpr range<WrappedState::unwrapped>::S _Unchecked_end() {
            end_called_ = true;
            return {};
        }
    };

    constexpr bool test() {
        using std::same_as, ranges::_Ubegin, ranges::_Uend;

        range<WrappedState::unwrapped> unwrapped;
        static_assert(same_as<decltype(_Ubegin(unwrapped)), range<WrappedState::unwrapped>::I>);
        static_assert(same_as<decltype(_Uend(unwrapped)), range<WrappedState::unwrapped>::S>);

        range<WrappedState::wrapped> wrapped;
        static_assert(same_as<decltype(_Ubegin(wrapped)), range<WrappedState::unwrapped>::I>);
        static_assert(same_as<decltype(_Uend(wrapped)), range<WrappedState::unwrapped>::S>);

        range<WrappedState::wrapped, WrappedState::ignorant> it_wrapped_se_ignorant;
        static_assert(same_as<decltype(_Ubegin(it_wrapped_se_ignorant)), range<WrappedState::wrapped>::I>);
        static_assert(same_as<decltype(_Uend(it_wrapped_se_ignorant)), range<WrappedState::ignorant>::S>);

        range<WrappedState::ignorant, WrappedState::wrapped> it_ignorant_se_wrapped;
        static_assert(same_as<decltype(_Ubegin(it_ignorant_se_wrapped)), range<WrappedState::ignorant>::I>);
        static_assert(same_as<decltype(_Uend(it_ignorant_se_wrapped)), range<WrappedState::wrapped>::S>);

        with_unchecked uncheckable;
        static_assert(same_as<decltype(_Ubegin(uncheckable)), range<WrappedState::unwrapped>::I>);
        static_assert(same_as<decltype(_Uend(uncheckable)), range<WrappedState::unwrapped>::S>);
        (void) _Ubegin(uncheckable);
        assert(uncheckable.begin_called_);
        (void) _Uend(uncheckable);
        assert(uncheckable.end_called_);

        return true;
    }
} // namespace unwrapped_begin_end

namespace closure {
    // Verify that range adaptor closures capture with the proper value category

    enum class GLValueKind { lvalue, const_lvalue, xvalue, const_xvalue };

    template <GLValueKind Allowed>
    struct arg {
        constexpr arg(arg&) {
            static_assert(Allowed == GLValueKind::lvalue);
        }
        constexpr arg(const arg&) {
            static_assert(Allowed == GLValueKind::const_lvalue);
        }
        constexpr arg(arg&&) {
            static_assert(Allowed == GLValueKind::xvalue);
        }
        constexpr arg(const arg&&) {
            static_assert(Allowed == GLValueKind::const_xvalue);
        }

    private:
        friend void test();
        arg() = default;
    };

    void test() {
        using std::as_const, std::move, std::views::filter;

        arg<GLValueKind::lvalue> l;
        (void) filter(l);

        arg<GLValueKind::const_lvalue> cl;
        (void) filter(as_const(cl));

        arg<GLValueKind::xvalue> r;
        (void) filter(move(r));

        arg<GLValueKind::const_xvalue> cr;
        (void) filter(move(as_const(cr)));
    }
} // namespace closure

int main() {
    // Validate conditional constexpr
    static_assert(test_array_ish<std::initializer_list<int>>());
    static_assert(test_array_ish<std::initializer_list<int const>>());
    static_assert(test_array_ish<int[3]>());
    static_assert(test_array_ish<int const[3]>());

    test_array_ish<std::initializer_list<int>>();
    test_array_ish<std::initializer_list<int const>>();
    test_array_ish<int[3]>();
    test_array_ish<int const[3]>();

    static_assert(complicated_algorithm_test());
    complicated_algorithm_test();

    static_assert(unwrapped_begin_end::test());
    unwrapped_begin_end::test();

    closure::test();
}
