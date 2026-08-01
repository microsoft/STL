// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <concepts>
#include <iterator>
#include <ranges>
#include <vector>

using namespace std;

using InnerRange = array<int, 3>;

// Use a non-common outer range so join_view::end() returns its nested sentinel
// instead of an iterator.
using Base = ranges::subrange<InnerRange*, unreachable_sentinel_t>;
using View = ranges::join_view<Base>;

using Iterator = ranges::iterator_t<View>;
using Sentinel = ranges::sentinel_t<View>;

static_assert(!same_as<Iterator, Sentinel>);

// P3059R2: users must not be able to construct join_view::sentinel directly
// from its parent join_view.
static_assert(!constructible_from<Sentinel, View&>);

// The sentinel must remain usable through its public special member functions.
static_assert(default_initializable<Sentinel>);
static_assert(copy_constructible<Sentinel>);
static_assert(move_constructible<Sentinel>);

// Verify the const sentinel specialization.
using ConstIterator = ranges::iterator_t<const View>;
using ConstSentinel = ranges::sentinel_t<const View>;

static_assert(!same_as<ConstIterator, ConstSentinel>);

// P3059R2: the implementation-only constructor of sentinel<true> must also
// be inaccessible.
static_assert(!constructible_from<ConstSentinel, const View&>);

static_assert(default_initializable<ConstSentinel>);
static_assert(copy_constructible<ConstSentinel>);
static_assert(move_constructible<ConstSentinel>);

// The public conversion from sentinel<false> to sentinel<true> must remain
// available.
static_assert(constructible_from<ConstSentinel, Sentinel>);


// Verify that normal iteration through join_view compiles.
bool test_join_view() {
    array<array<int, 2>, 3> values{{
        {1, 2},
        {3, 4},
        {5, 6},
    }};

    auto outer = values | views::take(2);
    auto view  = outer | views::join;

    vector<int> result;

    const auto last = view.end();

    for (auto first = view.begin(); first != last; ++first) {
        result.push_back(*first);
    }

    return result == vector{1, 2, 3, 4};
}

int main() {
    assert(test_join_view());
}
