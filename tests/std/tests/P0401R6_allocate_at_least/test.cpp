// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <memory>
#include <numeric>
#include <type_traits>

using namespace std;

static_assert(is_standard_layout_v<allocation_result<int*>>);
static_assert(is_trivially_copyable_v<allocation_result<int*>>);

struct generous_allocator {
    using value_type = int;

    allocator<int> inner;

    [[nodiscard]] constexpr int* allocate(const size_t count) {
        return inner.allocate(max(count, static_cast<size_t>(16)));
    }

    [[nodiscard]] constexpr allocation_result<int*> allocate_at_least(const size_t count) {
        return {inner.allocate(max(count, static_cast<size_t>(16))), max(count, static_cast<size_t>(16))};
    }

    constexpr void deallocate(int* ptr, const size_t count) {
        inner.deallocate(ptr, max(count, static_cast<size_t>(16)));
    }
};

struct strict_allocator {
    using value_type = int;

    allocator<int> inner;

    [[nodiscard]] constexpr int* allocate(const size_t count) {
        return inner.allocate(count);
    }

    constexpr void deallocate(int* ptr, const size_t count) {
        inner.deallocate(ptr, count);
    }
};

constexpr bool test() {
    {
        allocator<int> al;

        same_as<allocation_result<int*>> auto result = al.allocate_at_least(5);
        assert(result.ptr);
        assert(result.count >= 5);
        al.deallocate(result.ptr, 5);

        result = al.allocate_at_least(7);
        assert(result.ptr);
        assert(result.count >= 7);
        al.deallocate(result.ptr, result.count);

        result = al.allocate_at_least(9);
        assert(result.ptr);
        assert(result.count >= 9);
        al.deallocate(result.ptr, midpoint(static_cast<size_t>(9), result.count));
    }

    {
        generous_allocator al;

        same_as<allocation_result<int*>> auto result = allocate_at_least(al, 4);
        assert(result.ptr);
        assert(result.count == 16);
        al.deallocate(result.ptr, result.count);
    }

    {
        strict_allocator al;

        same_as<allocation_result<int*>> auto result = allocate_at_least(al, 4);
        assert(result.ptr);
        assert(result.count == 4);
        al.deallocate(result.ptr, result.count);
    }

    return true;
}

int main() {
    test();
    static_assert(test());
}
