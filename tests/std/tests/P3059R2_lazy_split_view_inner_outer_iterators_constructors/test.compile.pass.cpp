// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <istream>
#include <ranges>
#include <string_view>
#include <utility>

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
