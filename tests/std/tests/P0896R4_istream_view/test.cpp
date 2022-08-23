// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <ranges>
#include <sstream>
#include <type_traits>

#include <range_algorithm_support.hpp>

using namespace std;

constexpr int expected_empty[] = {-1, -1, -1, -1, -1};
constexpr int expected_vals[]  = {0, 1, 2, 3, -1};

struct streamable {
    streamable() = default;
    streamable(const int input) : _val(input) {}

    template <class CharT, class Traits>
    friend basic_istream<CharT, Traits>& operator>>(basic_istream<CharT, Traits>& is, streamable& right) noexcept {
        is >> right._val;
        return is;
    }

    friend bool operator==(const streamable&, const streamable&) noexcept = default;

    int _val = 0;
};

template <class T>
void test_one_type() {
    using ranges::basic_istream_view;

    // validate type properties
    using R = basic_istream_view<T, char>;
    static_assert(ranges::view<R>);
    static_assert(ranges::input_range<R>);
    static_assert(!ranges::forward_range<R>);

    static_assert(!ranges::sized_range<R>);
    static_assert(!ranges::common_range<R>);

    // validate constructors
    istringstream nonempty_stream{"0"};
    istringstream empty_intstream{};
    R empty_constructed{empty_intstream};
    R non_empty_constructed{nonempty_stream};

    static_assert(is_nothrow_constructible_v<R, istream&> == is_nothrow_default_constructible_v<T>);

    // validate member begin
    // NOTE: begin() consumes the first token
    assert(empty_constructed.begin() == default_sentinel);
    assert(non_empty_constructed.begin() != default_sentinel);

    // validate member end
    static_assert(same_as<decltype(empty_constructed.end()), default_sentinel_t>);
    static_assert(noexcept(empty_constructed.end()));
    static_assert(noexcept(ranges::end(empty_constructed)));

    // Nonexistent member functions
    static_assert(!CanMemberSize<R>);
    static_assert(!CanMemberData<R>);
    static_assert(!CanMemberEmpty<R>);
    static_assert(!CanMemberFront<R>);
    static_assert(!CanMemberBack<R>);

    // Some basic tests
    T input_empty[] = {-1, -1, -1, -1, -1};
    ranges::copy(empty_constructed, input_empty);
    assert(ranges::equal(input_empty, expected_empty));

    { // using ranges::basic_istream_view with wide stream
        wistringstream wintstream{L"0 1 2 3"};
        T input_value[] = {-1, -1, -1, -1, -1};
        ranges::copy(basic_istream_view<T, wchar_t>{wintstream}, input_value);
        assert(ranges::equal(input_value, expected_vals));
    }

    { // using ranges::basic_istream_view with narrow stream
        istringstream intstream{"0 1 2 3"};
        T input_value[] = {-1, -1, -1, -1, -1};
        ranges::copy(basic_istream_view<T, char>{intstream}, input_value);
        assert(ranges::equal(input_value, expected_vals));
    }

    { // Using ranges::istream_view
        istringstream intstream{"0 1 2 3"};
        T input[] = {-1, -1, -1, -1, -1};
        ranges::copy(ranges::istream_view<T>(intstream), input);
        static_assert(noexcept(ranges::istream_view<T>(intstream)));
        assert(ranges::equal(input, expected_vals));
    }

    { // Using ranges::wistream_view
        wistringstream wintstream{L"0 1 2 3"};
        T input[] = {-1, -1, -1, -1, -1};
        ranges::copy(ranges::wistream_view<T>(wintstream), input);
        static_assert(noexcept(ranges::wistream_view<T>(wintstream)));
        assert(ranges::equal(input, expected_vals));
    }

    { // Using views::istream with narrow stream
        istringstream intstream{"0 1 2 3"};
        T input[] = {-1, -1, -1, -1, -1};
        ranges::copy(views::istream<T>(intstream), input);
        static_assert(noexcept(views::istream<T>(intstream)));
        assert(ranges::equal(input, expected_vals));
    }

    { // Using views::istream with wide stream
        wistringstream wintstream{L"0 1 2 3"};
        T input[] = {-1, -1, -1, -1, -1};
        ranges::copy(views::istream<T>(wintstream), input);
        static_assert(noexcept(views::istream<T>(wintstream)));
        assert(ranges::equal(input, expected_vals));
    }
}

istringstream some_stream{"42"};
constexpr bool test_constexpr() {
    // stream constructor is constexpr
    ranges::basic_istream_view<int, char> meow{some_stream};

    // begin is constexpr, but realistically unusable in a constant expression

    // end is constexpr
    (void) meow.end();

    return true;
}

int main() {
    test_one_type<int>();
    test_one_type<streamable>();

    static_assert(test_constexpr());
}
