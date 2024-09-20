// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Tests MSVC STL specific behavior on ABI.

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <memory_resource>
#include <ranges>
#include <type_traits>

using namespace std;

static_assert(sizeof(ranges::in_value_result<ranges::dangling, int>) == sizeof(int));
static_assert(sizeof(ranges::in_value_result<int, ranges::dangling>) == sizeof(int));
#ifndef __EDG__ // TRANSITION, DevCom-10747012
static_assert(is_empty_v<ranges::in_value_result<ranges::dangling, ranges::dangling>>);
#endif // ^^^ no workaround ^^^

static_assert(sizeof(ranges::out_value_result<ranges::dangling, int>) == sizeof(int));
static_assert(sizeof(ranges::out_value_result<int, ranges::dangling>) == sizeof(int));
#ifndef __EDG__ // TRANSITION, DevCom-10747012
static_assert(is_empty_v<ranges::out_value_result<ranges::dangling, ranges::dangling>>);
#endif // ^^^ no workaround ^^^

struct stateless_input_it {
    using value_type      = int;
    using difference_type = ptrdiff_t;

    int operator*() const noexcept {
        return 42;
    }

    stateless_input_it& operator++() noexcept {
        return *this;
    }

    stateless_input_it operator++(int) noexcept {
        return {};
    }
};

struct stateless_forward_it {
    using value_type      = int;
    using difference_type = ptrdiff_t;

    int operator*() const noexcept {
        return 42;
    }

    stateless_forward_it& operator++() noexcept {
        return *this;
    }

    stateless_forward_it operator++(int) noexcept {
        return {};
    }

    friend bool operator==(stateless_forward_it, stateless_forward_it) = default;
};

static_assert(input_iterator<stateless_input_it>);
static_assert(forward_iterator<stateless_forward_it>);

using test_range                 = ranges::subrange<int*, unreachable_sentinel_t>;
using test_outer_range           = ranges::subrange<const test_range*, unreachable_sentinel_t>;
using stateless_input_range      = ranges::subrange<stateless_input_it, unreachable_sentinel_t>;
using stateless_fwd_range        = ranges::subrange<stateless_forward_it, unreachable_sentinel_t>;
using stateless_fwd_common_range = ranges::subrange<stateless_forward_it, stateless_forward_it>;

#ifndef __EDG__ // TRANSITION, DevCom-10747012
static_assert(is_empty_v<decltype((test_outer_range{} | views::join_with(test_range{})).end())>);
#endif // ^^^ no workaround ^^^

static_assert(sizeof((stateless_input_range{} | views::enumerate).begin()) == sizeof(ptrdiff_t));
#ifndef __EDG__ // TRANSITION, DevCom-10747012
static_assert(is_empty_v<decltype((stateless_input_range{} | views::enumerate).end())>);
#endif // ^^^ no workaround ^^^

static_assert(sizeof((stateless_fwd_range{} | views::chunk(42)).begin()) == sizeof(ptrdiff_t) * 2);
#ifndef __EDG__ // TRANSITION, DevCom-10747012
static_assert(sizeof((stateless_fwd_common_range{} | views::chunk(42)).begin()) == sizeof(ptrdiff_t) * 2);
#endif // ^^^ no workaround ^^^

static_assert(sizeof((stateless_fwd_range{} | views::chunk_by(ranges::less{})).begin()) == sizeof(void*) * 2);
static_assert(sizeof((stateless_fwd_common_range{} | views::chunk_by(ranges::less{})).begin()) == sizeof(void*) * 2);

#ifndef __EDG__ // TRANSITION, DevCom-10747012
static_assert(is_empty_v<decltype((test_range{} | views::slide(42)).end())>);
#endif // ^^^ no workaround ^^^

static_assert(sizeof(ranges::elements_of<int[1], allocator<int>>) == sizeof(int[1]));
static_assert(sizeof(ranges::elements_of<int (&)[1], allocator<int>>) == sizeof(int (*)[1]));
static_assert(sizeof(ranges::elements_of<ranges::empty_view<int>, pmr::polymorphic_allocator<int>>)
              == sizeof(pmr::polymorphic_allocator<int>));
static_assert(sizeof(ranges::elements_of<ranges::empty_view<int>, allocator<int>>) == 1);

#ifndef __EDG__ // TRANSITION, DevCom-10747012
static_assert(is_empty_v<ranges::elements_of<ranges::empty_view<int>, allocator<int>>>);
#endif // ^^^ no workaround ^^^
