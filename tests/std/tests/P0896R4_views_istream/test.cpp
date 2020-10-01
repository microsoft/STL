// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <ranges>
#include <sstream>

#include <range_algorithm_support.hpp>

using namespace std;

static constexpr int expected_empty[]  = {-1, -1, -1, -1, -1};
static constexpr int expected_copied[] = {0, 1, 2, 3, -1};

struct streamable {
    streamable() = default;
    streamable(const int input) : _val(input) {}

    friend basic_istream<char>& operator>>(basic_istream<char>& is, streamable& right) noexcept {
        is >> right._val;
        return is;
    }

    friend bool operator==(const streamable& left, const streamable& right) noexcept = default;

    int _val = 0;
};

template <class T>
bool test_one_type() {
    using ranges::basic_istream_view;

    // validate type properties
    using R = basic_istream_view<T, char>;
    static_assert(ranges::view<R>);
    static_assert(ranges::input_range<R>);
    static_assert(!ranges::forward_range<R>);

    static_assert(!ranges::sized_range<R>);
    static_assert(!ranges::common_range<R>);

    // validate constructor
    auto nonempty_stream                = istringstream{"0"};
    auto empty_intstream                = istringstream{};
    same_as<R> auto default_constructed = ranges::basic_istream_view<T, char>{};
    same_as<R> auto empty_constructed   = ranges::basic_istream_view<T, char>{empty_intstream};
    same_as<R> auto value_constructed   = ranges::basic_istream_view<T, char>{nonempty_stream};

    // validate member begin
    // NOTE: This moves the stream one element int front
    assert(default_constructed.begin() == default_sentinel);
    assert(empty_constructed.begin() == default_sentinel);
    assert(value_constructed.begin() != default_sentinel);

    // validate member end
    static_assert(same_as<decltype(default_constructed.end()), default_sentinel_t>);
    static_assert(noexcept(default_constructed.end()));
    static_assert(noexcept(ranges::end(default_constructed)));

    // Non existing member functions
    static_assert(!CanMemberSize<R>);
    static_assert(!CanMemberData<R>);
    static_assert(!CanMemberEmpty<R>);
    static_assert(!CanMemberFront<R>);
    static_assert(!CanMemberBack<R>);

    // Some basic test
    T input_default[] = {-1, -1, -1, -1, -1};
    ranges::copy(default_constructed, input_default);
    assert(ranges::equal(input_default, expected_empty));

    T input_empty[] = {-1, -1, -1, -1, -1};
    ranges::copy(empty_constructed, input_empty);
    assert(ranges::equal(input_empty, expected_empty));

    auto intstream  = istringstream{"0 1  2   3"};
    T input_value[] = {-1, -1, -1, -1, -1};
    ranges::copy(ranges::basic_istream_view<T, char>{intstream}, input_value);
    assert(ranges::equal(input_value, expected_copied));

    return true;
}

int main() {
    test_one_type<int>();
    test_one_type<streamable>();
}
