// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <cassert>
#include <ranges>
#include <iterator>
#include <vector>

using namespace std;

struct transform_fn {
    constexpr int operator()(const int value) const noexcept {
        return value * 2;
    }
};

// Use a non-common range so transform_view has distinct iterator and sentinel
// types. unreachable_sentinel_t also makes the underlying sentinel type easy
// to name without defining additional test machinery.
using Base = ranges::subrange<int*, unreachable_sentinel_t>;
using View = ranges::transform_view<Base, transform_fn>;

using BaseIterator = ranges::iterator_t<Base>;
using BaseSentinel = ranges::sentinel_t<Base>;

using Iterator = ranges::iterator_t<View>;
using Sentinel = ranges::sentinel_t<View>;

// P3059R2: users must not be able to construct transform_view::iterator
// directly from the parent transform_view and the underlying iterator.
static_assert(!constructible_from<Iterator, View&, BaseIterator>);

// P3059R2: users must not be able to construct transform_view::sentinel
// directly from the underlying sentinel.
static_assert(!constructible_from<Sentinel, BaseSentinel>);

// The iterator and sentinel must remain copyable and movable through their
// public interfaces.
static_assert(copy_constructible<Iterator>);
static_assert(move_constructible<Iterator>);
static_assert(copy_constructible<Sentinel>);
static_assert(move_constructible<Sentinel>);

// Verify the corresponding constructors are also inaccessible for the const
// iterator and sentinel specializations.
using ConstBaseIterator = ranges::iterator_t<const Base>;
using ConstBaseSentinel = ranges::sentinel_t<const Base>;

using ConstIterator = ranges::iterator_t<const View>;
using ConstSentinel = ranges::sentinel_t<const View>;

static_assert(!constructible_from<ConstIterator, const View&, ConstBaseIterator>);
static_assert(!constructible_from<ConstSentinel, ConstBaseSentinel>);

// P3059R2 does not remove the public converting constructors from non-const
// iterator/sentinel specializations to their const counterparts.
static_assert(constructible_from<ConstIterator, Iterator>);
static_assert(constructible_from<ConstSentinel, Sentinel>);


// Use a non-common underlying range so transform_view has a distinct sentinel.
bool test_transform_view() {
    int values[] = {1, 2, 3, 4};

    auto base = ranges::subrange{
        counted_iterator{values, 4},
        default_sentinel,
    };

    auto view = base | views::transform([](const int value) { return value * value; });

    vector<int> result;
    auto last = view.end();
    for (auto first = view.begin(); first != last; ++first) {
        result.push_back(*first);
    }

    return result == vector{1, 4, 9, 16};
}

int main()
{
    assert(test_transform_view());
}
