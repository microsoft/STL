// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <ranges>

using namespace std;

struct is_less_than_ten {
    constexpr bool operator()(const int value) const noexcept {
        return value < 10;
    }
};

// Use a non-common underlying range so take_while_view has a distinct
// iterator and sentinel type.
using Base = ranges::subrange<int*, unreachable_sentinel_t>;
using View = ranges::take_while_view<Base, is_less_than_ten>;

using BaseSentinel = ranges::sentinel_t<Base>;
using Iterator     = ranges::iterator_t<View>;
using Sentinel     = ranges::sentinel_t<View>;

static_assert(!same_as<Iterator, Sentinel>);

// P3059R2: users must not be able to construct take_while_view::sentinel
// directly from the underlying sentinel and a pointer to the predicate.
static_assert(!constructible_from<Sentinel, BaseSentinel, const is_less_than_ten*>);

// The sentinel must remain default-initializable, copyable, and movable
// through its public interface.
static_assert(default_initializable<Sentinel>);
static_assert(copy_constructible<Sentinel>);
static_assert(move_constructible<Sentinel>);

// Verify the same requirements for the const sentinel specialization.
using ConstBaseSentinel = ranges::sentinel_t<const Base>;
using ConstIterator     = ranges::iterator_t<const View>;
using ConstSentinel     = ranges::sentinel_t<const View>;

static_assert(!same_as<ConstIterator, ConstSentinel>);

static_assert(!constructible_from<ConstSentinel, ConstBaseSentinel, const is_less_than_ten*>);

static_assert(default_initializable<ConstSentinel>);
static_assert(copy_constructible<ConstSentinel>);
static_assert(move_constructible<ConstSentinel>);

// The public converting constructor from sentinel<false> to sentinel<true>
// must remain available.
static_assert(constructible_from<ConstSentinel, Sentinel>);
