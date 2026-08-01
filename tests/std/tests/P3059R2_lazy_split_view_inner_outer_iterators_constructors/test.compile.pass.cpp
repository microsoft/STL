// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <concepts>
#include <istream>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace std;

// Test the forward_range form of lazy_split_view::outer-iterator.
using ForwardBase = string_view;
using Pattern     = ranges::single_view<char>;
using ForwardView = ranges::lazy_split_view<ForwardBase, Pattern>;

using ForwardBaseIterator = ranges::iterator_t<ForwardBase>;
using ForwardOuter        = ranges::iterator_t<ForwardView>;
using ForwardOuterValue   = iter_value_t<ForwardOuter>;
using ForwardInner        = decltype(declval<ForwardOuterValue>().begin());

// P3059R2: the outer iterator must not be publicly constructible from its
// parent and the current iterator of the underlying forward range.
static_assert(!constructible_from<ForwardOuter, ForwardView&, ForwardBaseIterator>);

// P3059R2: the inner iterator must not be publicly constructible from an
// outer iterator.
static_assert(!constructible_from<ForwardInner, ForwardOuter>);

// The outer iterator's public special member functions remain available.
static_assert(copy_constructible<ForwardOuter>);
static_assert(move_constructible<ForwardOuter>);

// The inner iterator remains copyable and movable through its public
// special member functions.
static_assert(copy_constructible<ForwardInner>);
static_assert(move_constructible<ForwardInner>);

// Test the const forward-range specializations.
using ConstForwardBaseIterator = ranges::iterator_t<const ForwardBase>;
using ConstForwardOuter        = ranges::iterator_t<const ForwardView>;
using ConstForwardOuterValue   = iter_value_t<ConstForwardOuter>;
using ConstForwardInner        = decltype(declval<ConstForwardOuterValue>().begin());

static_assert(!constructible_from<ConstForwardOuter, const ForwardView&, ConstForwardBaseIterator>);

static_assert(!constructible_from<ConstForwardInner, ConstForwardOuter>);

static_assert(copy_constructible<ConstForwardOuter>);
static_assert(move_constructible<ConstForwardOuter>);
static_assert(copy_constructible<ConstForwardInner>);
static_assert(move_constructible<ConstForwardInner>);

// The public conversion from outer-iterator<false> to outer-iterator<true>
// remains available.
static_assert(constructible_from<ConstForwardOuter, ForwardOuter>);

// Test the input_range form of lazy_split_view::outer-iterator.
using InputBase = ranges::istream_view<char>;
using InputView = ranges::lazy_split_view<InputBase, Pattern>;

using InputOuter      = ranges::iterator_t<InputView>;
using InputOuterValue = iter_value_t<InputOuter>;
using InputInner      = decltype(declval<InputOuterValue>().begin());

// P3059R2: for a non-forward range, the outer iterator must not be publicly
// constructible directly from its parent.
static_assert(!constructible_from<InputOuter, InputView&>);

// The corresponding inner-iterator constructor must also be inaccessible.
static_assert(!constructible_from<InputInner, InputOuter>);

static_assert(move_constructible<InputOuter>);
static_assert(move_constructible<InputInner>);

constexpr bool test_lazy_split_view() {
    array input{1, 2, 0, 3, 4};

    auto view = input | views::lazy_split(0);

    auto outer = view.begin();

    const auto first_part = *outer;
    auto first_inner      = first_part.begin();

    if (*first_inner != 1) {
        return false;
    }

    ++first_inner;
    if (*first_inner != 2) {
        return false;
    }

    ++outer;

    const auto second_part = *outer;
    auto second_inner      = second_part.begin();

    if (*second_inner != 3) {
        return false;
    }

    ++second_inner;
    return *second_inner == 4;
}

static_assert(test_lazy_split_view());

// This exercises the forward-range outer-iterator constructor and the
// inner-iterator created from the outer iterator.
bool test_lazy_split_view_forward_range() {
    const string input = "one,two,three";
    auto view          = input | views::lazy_split(',');

    vector<string> result;
    auto last = view.end();
    for (auto outer = view.begin(); outer != last; ++outer) {
        string part;

        for (const char character : *outer) {
            part.push_back(character);
        }

        result.push_back(move(part));
    }

    return result == vector<string>{"one", "two", "three"};
}


// This exercises the constructor used when the underlying range is an input
// range rather than a forward range.
bool test_lazy_split_view_input_range() {
    istringstream input{"1 0 2 0 3"};

    auto source  = ranges::istream_view<int>(input);
    auto pattern = views::single(0);
    auto view    = views::lazy_split(source, pattern);

    vector<vector<int>> result;
    auto last = view.end();
    for (auto outer = view.begin(); outer != last; ++outer) {
        vector<int> part;

        for (const int value : *outer) {
            part.push_back(value);
        }

        result.push_back(move(part));
    }

    return result == vector<vector<int>>{{1}, {2}, {3}};
}

int main() {
    assert(test_lazy_split_view_forward_range());
    assert(test_lazy_split_view_input_range());
}
