// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <concepts>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

// Use a non-common forward range so split_view::end() returns the nested
// sentinel type instead of the iterator type.
using Base    = ranges::subrange<int*, unreachable_sentinel_t>;
using Pattern = ranges::single_view<int>;
using View    = ranges::split_view<Base, Pattern>;

using BaseIterator = ranges::iterator_t<Base>;
using NextSubrange = ranges::subrange<BaseIterator>;

using Iterator = ranges::iterator_t<View>;
using Sentinel = ranges::sentinel_t<View>;

static_assert(!same_as<Iterator, Sentinel>);

// P3059R2: users must not be able to construct split_view::iterator directly
// from its parent, current underlying iterator, and next delimiter subrange.
static_assert(!constructible_from<Iterator, View&, BaseIterator, NextSubrange>);

// The iterator's public default constructor and special member functions
// must remain available.
static_assert(default_initializable<Iterator>);
static_assert(copy_constructible<Iterator>);
static_assert(move_constructible<Iterator>);

// P3059R2: users must not be able to construct split_view::sentinel directly
// from its parent split_view.
static_assert(!constructible_from<Sentinel, View&>);

// The sentinel's public default constructor and special member functions
// must remain available.
static_assert(default_initializable<Sentinel>);
static_assert(copy_constructible<Sentinel>);
static_assert(move_constructible<Sentinel>);

// Verify that the iterator and sentinel still satisfy their required public
// iterator interface.
static_assert(forward_iterator<Iterator>);
static_assert(sentinel_for<Sentinel, Iterator>);


 // Verify that normal iteration through split_view compiles.
bool test_split_view() {
    const string_view input = "alpha,beta,gamma";
    auto view               = views::split(input, ',');

    vector<string> result;
    for (const auto part : view) {
        result.emplace_back(part.begin(), part.end());
    }

    return result == vector<string>{"alpha", "beta", "gamma"};
}

int main() {
    assert(test_split_view());
}
