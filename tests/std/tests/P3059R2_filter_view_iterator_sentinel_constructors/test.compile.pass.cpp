// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <concepts>
#include <iterator>
#include <ranges>
#include <utility>
#include <vector>

using namespace std;

struct is_even {
    constexpr bool operator()(const int value) const noexcept {
        return value % 2 == 0;
    }
};

// Verify filter_view with a common underlying range.
using CommonStorage = array<int, 6>;
using CommonBase    = ranges::ref_view<CommonStorage>;
using CommonView    = ranges::filter_view<CommonBase, is_even>;

using CommonBaseIterator = ranges::iterator_t<CommonBase>;
using CommonIterator     = ranges::iterator_t<CommonView>;

// P3059R2: users must not be able to construct filter_view::iterator
// directly from the parent filter_view and the underlying iterator.
static_assert(!constructible_from<CommonIterator, CommonView&, CommonBaseIterator>);

// The iterator must remain copyable and movable through its public interface.
static_assert(copy_constructible<CommonIterator>);
static_assert(move_constructible<CommonIterator>);

// A sentinel type for constructing a non-common underlying range.
struct int_sentinel {
    int* last = nullptr;

    friend constexpr bool operator==(int* const iterator, const int_sentinel sentinel) noexcept {
        return iterator == sentinel.last;
    }

    friend constexpr bool operator==(const int_sentinel sentinel, int* const iterator) noexcept {
        return iterator == sentinel.last;
    }
};

static_assert(sentinel_for<int_sentinel, int*>);

// filter_view has a distinct sentinel only when its underlying range is
// not a common_range.
using NonCommonBase = ranges::subrange<int*, int_sentinel>;
using NonCommonView = ranges::filter_view<NonCommonBase, is_even>;

using NonCommonBaseIterator = ranges::iterator_t<NonCommonBase>;
using NonCommonIterator     = ranges::iterator_t<NonCommonView>;
using NonCommonSentinel     = ranges::sentinel_t<NonCommonView>;

static_assert(!ranges::common_range<NonCommonBase>);
static_assert(!same_as<NonCommonIterator, NonCommonSentinel>);

// P3059R2: the iterator constructor must not be publicly accessible.
static_assert(!constructible_from<NonCommonIterator, NonCommonView&, NonCommonBaseIterator>);

// P3059R2: the sentinel constructor taking the parent filter_view must
// not be publicly accessible.
static_assert(!constructible_from<NonCommonSentinel, NonCommonView&>);

// Public iterator and sentinel operations must continue to work after the
// implementation-only constructors are made private.
constexpr bool test_common_filter_view() {
    CommonStorage values{1, 2, 3, 4, 5, 6};
    CommonView view{ranges::ref_view{values}, is_even{}};

    auto iterator   = view.begin();
    const auto last = view.end();

    if (iterator == last || *iterator != 2) {
        return false;
    }

    ++iterator;

    if (iterator == last || *iterator != 4) {
        return false;
    }

    ++iterator;

    if (iterator == last || *iterator != 6) {
        return false;
    }

    ++iterator;

    return iterator == last;
}

constexpr bool test_non_common_filter_view() {
    int values[]{1, 2, 3, 4, 5, 6};

    NonCommonBase base{values, int_sentinel{values + size(values)}};

    NonCommonView view{base, is_even{}};

    auto iterator   = view.begin();
    const auto last = view.end();

    if (iterator == last || *iterator != 2) {
        return false;
    }

    ++iterator;

    if (iterator == last || *iterator != 4) {
        return false;
    }

    ++iterator;

    if (iterator == last || *iterator != 6) {
        return false;
    }

    ++iterator;

    return iterator == last;
}

static_assert(test_common_filter_view());
static_assert(test_non_common_filter_view());


// Verify that normal iteration through filter_view compiles.
bool test_filter_view() {
    vector values{1, 2, 3, 4, 5, 6};

    auto view = values | views::filter([](const int value) { return value % 2 == 0; });

    vector<int> result;
    auto last = view.end();
    for (auto first = view.begin(); first != last; ++first) {
        result.push_back(*first);
    }

    return result == vector{2, 4, 6};
}

int main() {
    assert(test_common_filter_view());
}
