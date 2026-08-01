// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <concepts>
#include <iterator>
#include <ranges>
#include <vector>

using namespace std;

// Use a non-common, non-sized underlying range so take_view uses its nested
// sentinel type instead of returning an iterator or default_sentinel_t.
using Base = ranges::subrange<int*, unreachable_sentinel_t>;
using View = ranges::take_view<Base>;

using BaseSentinel = ranges::sentinel_t<Base>;
using Iterator     = ranges::iterator_t<View>;
using Sentinel     = ranges::sentinel_t<View>;

static_assert(!same_as<Iterator, Sentinel>);
static_assert(!same_as<Sentinel, default_sentinel_t>);

// P3059R2: users must not be able to construct take_view::sentinel directly
// from the sentinel of the underlying range.
static_assert(!constructible_from<Sentinel, BaseSentinel>);

// The sentinel must remain default-initializable, copyable, and movable
// through its public interface.
static_assert(default_initializable<Sentinel>);
static_assert(copy_constructible<Sentinel>);
static_assert(move_constructible<Sentinel>);

// Verify the same requirements for the const specialization.
using ConstBaseSentinel = ranges::sentinel_t<const Base>;
using ConstIterator     = ranges::iterator_t<const View>;
using ConstSentinel     = ranges::sentinel_t<const View>;

static_assert(!same_as<ConstIterator, ConstSentinel>);
static_assert(!same_as<ConstSentinel, default_sentinel_t>);

static_assert(!constructible_from<ConstSentinel, ConstBaseSentinel>);

static_assert(default_initializable<ConstSentinel>);
static_assert(copy_constructible<ConstSentinel>);
static_assert(move_constructible<ConstSentinel>);

// The public conversion from the non-const sentinel specialization to the
// const sentinel specialization must remain available.
static_assert(constructible_from<ConstSentinel, Sentinel>);

// Verify that normal iteration through take_view compiles.
bool test_take_view() {
    int values[] = {1, 2, 3, 4, 5};

    auto base = ranges::subrange{
        counted_iterator{values, 5},
        default_sentinel,
    };

    auto view = base | views::take(3);

    vector<int> result;

    for (auto first = view.begin(), last = view.end(); first != last; ++first) {
        result.push_back(*first);
    }

    return result == vector{1, 2, 3};
}

int main() {
    assert(test_take_view());
}
