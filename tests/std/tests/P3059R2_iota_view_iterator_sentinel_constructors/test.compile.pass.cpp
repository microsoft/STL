// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <concepts>
#include <ranges>
#include <vector>

using namespace std;

// Verify an unbounded iota_view. Its end type is unreachable_sentinel_t.
using UnboundedView     = ranges::iota_view<int>;
using UnboundedIterator = ranges::iterator_t<UnboundedView>;
using UnboundedSentinel = ranges::sentinel_t<UnboundedView>;

static_assert(same_as<UnboundedSentinel, unreachable_sentinel_t>);

// P3059R2: users must not be able to construct iota_view::iterator
// directly from an iota value.
static_assert(!constructible_from<UnboundedIterator, int>);

// The iterator's public default constructor and special member functions
// must remain available.
static_assert(default_initializable<UnboundedIterator>);
static_assert(copy_constructible<UnboundedIterator>);
static_assert(move_constructible<UnboundedIterator>);

static_assert(random_access_iterator<UnboundedIterator>);
static_assert(sentinel_for<UnboundedSentinel, UnboundedIterator>);

// The public iota_view constructor taking its iterator and sentinel must
// remain available.
static_assert(constructible_from<UnboundedView, UnboundedIterator, UnboundedSentinel>);

// Verify a bounded common iota_view. Its end type is the iterator type.
using CommonView     = ranges::iota_view<int, int>;
using CommonIterator = ranges::iterator_t<CommonView>;
using CommonSentinel = ranges::sentinel_t<CommonView>;

static_assert(same_as<CommonIterator, CommonSentinel>);

// P3059R2: users must not be able to construct iota_view::iterator
// directly from an iota value.
static_assert(!constructible_from<CommonIterator, int>);

static_assert(default_initializable<CommonIterator>);
static_assert(copy_constructible<CommonIterator>);
static_assert(move_constructible<CommonIterator>);

static_assert(random_access_iterator<CommonIterator>);
static_assert(sentinel_for<CommonSentinel, CommonIterator>);

// The public iota_view constructor taking two iterators must remain
// available for a common iota_view.
static_assert(constructible_from<CommonView, CommonIterator, CommonSentinel>);

// Verify a bounded non-common iota_view. Different value and bound types
// cause end() to return the implementation sentinel type.
using NonCommonView     = ranges::iota_view<int, long>;
using NonCommonIterator = ranges::iterator_t<NonCommonView>;
using NonCommonSentinel = ranges::sentinel_t<NonCommonView>;

static_assert(!same_as<NonCommonIterator, NonCommonSentinel>);

// P3059R2: users must not be able to construct iota_view::iterator
// directly from an iota value.
static_assert(!constructible_from<NonCommonIterator, int>);

// P3059R2: users must not be able to construct iota_view::sentinel
// directly from an iota bound.
static_assert(!constructible_from<NonCommonSentinel, long>);

// The iterator's and sentinel's public default constructors and special
// member functions must remain available.
static_assert(default_initializable<NonCommonIterator>);
static_assert(copy_constructible<NonCommonIterator>);
static_assert(move_constructible<NonCommonIterator>);

static_assert(default_initializable<NonCommonSentinel>);
static_assert(copy_constructible<NonCommonSentinel>);
static_assert(move_constructible<NonCommonSentinel>);

static_assert(random_access_iterator<NonCommonIterator>);
static_assert(sentinel_for<NonCommonSentinel, NonCommonIterator>);

// The public iota_view constructor taking its iterator and sentinel must
// remain available.
static_assert(constructible_from<NonCommonView, NonCommonIterator, NonCommonSentinel>);


// Verify that users can still obtain and use the iterator and sentinel through
// begin() and end().
bool test_iota_view() {
    const auto view = views::iota(1, 6);

    auto first = view.begin();
    auto last  = view.end();

    vector<int> result;

    for (; first != last; ++first) {
        result.push_back(*first);
    }

    return result == vector{1, 2, 3, 4, 5};
}

int main() {
    assert(test_iota_view());
}
