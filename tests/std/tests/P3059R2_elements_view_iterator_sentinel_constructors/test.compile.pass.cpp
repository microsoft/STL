// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <concepts>
#include <iterator>
#include <ranges>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using namespace std;

// Use a non-common range so elements_view::end() returns its nested sentinel
// instead of an iterator.
using Element      = pair<int, long>;
using Base         = ranges::subrange<Element*, unreachable_sentinel_t>;
using View         = ranges::elements_view<Base, 0>;
using BaseIterator = ranges::iterator_t<Base>;
using BaseSentinel = ranges::sentinel_t<Base>;

using Iterator = ranges::iterator_t<View>;
using Sentinel = ranges::sentinel_t<View>;

static_assert(!same_as<Iterator, Sentinel>);

// P3059R2: users must not be able to construct elements_view::iterator
// directly from the underlying range iterator.
static_assert(!constructible_from<Iterator, BaseIterator>);

// P3059R2: users must not be able to construct elements_view::sentinel
// directly from the underlying range sentinel.
static_assert(!constructible_from<Sentinel, BaseSentinel>);

// The public special member functions must remain available.
static_assert(default_initializable<Iterator>);
static_assert(copy_constructible<Iterator>);
static_assert(move_constructible<Iterator>);

static_assert(default_initializable<Sentinel>);
static_assert(copy_constructible<Sentinel>);
static_assert(move_constructible<Sentinel>);

// Verify the iterator and sentinel still satisfy their public interfaces.
static_assert(random_access_iterator<Iterator>);
static_assert(sentinel_for<Sentinel, Iterator>);

// Test the const specializations.
using ConstBaseIterator = ranges::iterator_t<const Base>;
using ConstBaseSentinel = ranges::sentinel_t<const Base>;

using ConstIterator = ranges::iterator_t<const View>;
using ConstSentinel = ranges::sentinel_t<const View>;

static_assert(!same_as<ConstIterator, ConstSentinel>);

// The implementation-only constructors for the const specializations must
// also be inaccessible.
static_assert(!constructible_from<ConstIterator, ConstBaseIterator>);
static_assert(!constructible_from<ConstSentinel, ConstBaseSentinel>);

static_assert(default_initializable<ConstIterator>);
static_assert(copy_constructible<ConstIterator>);
static_assert(move_constructible<ConstIterator>);

static_assert(default_initializable<ConstSentinel>);
static_assert(copy_constructible<ConstSentinel>);
static_assert(move_constructible<ConstSentinel>);

static_assert(random_access_iterator<ConstIterator>);
static_assert(sentinel_for<ConstSentinel, ConstIterator>);

// The public non-const-to-const converting constructors must remain
// available.
static_assert(constructible_from<ConstIterator, Iterator>);
static_assert(constructible_from<ConstSentinel, Sentinel>);


// Use a non-common underlying range so elements_view has a distinct sentinel.
bool test_elements_view() {
    using element_type = tuple<int, string>;

    element_type values[] = {
        {1, "one"},
        {2, "two"},
        {3, "three"},
    };

    auto base = ranges::subrange{
        counted_iterator{values, 3},
        default_sentinel,
    };

    auto view = base | views::elements<1>;

    vector<string> result;
    auto last = view.end();
    for (auto first = view.begin(); first != last; ++first) {
        result.push_back(*first);
    }

    return result == vector<string>{"one", "two", "three"};
}

int main() {
    assert(test_elements_view());
}
