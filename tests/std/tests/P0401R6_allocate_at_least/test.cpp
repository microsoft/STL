// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#ifdef __cpp_lib_concepts // TRANSITION, GH-395
#include <concepts>
#endif // __cpp_lib_concepts
#include <cstdint>
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

struct small_allocator {
    using value_type = int;
    using size_type  = uint32_t;

    [[nodiscard]] constexpr int* allocate(const size_type count) {
        return allocator<int>{}.allocate(count);
    }

    constexpr void deallocate(int* ptr, const size_type count) {
        allocator<int>{}.deallocate(ptr, count);
    }
};

struct huge_allocator {
    using value_type = int;
    using size_type  = uint64_t;

    [[nodiscard]] constexpr int* allocate(const size_type count) {
        return allocator<int>{}.allocate(static_cast<size_t>(count));
    }

    constexpr void deallocate(int* ptr, const size_type count) {
        allocator<int>{}.deallocate(ptr, static_cast<size_t>(count));
    }
};

constexpr bool test() {
    {
        allocator<int> al;

#ifdef __cpp_lib_concepts // TRANSITION, GH-395
        same_as<allocation_result<int*>> auto result = al.allocate_at_least(5);
#else // ^^^ defined(__cpp_lib_concepts) / !defined(__cpp_lib_concepts) vvv
        auto result = al.allocate_at_least(5);
        static_assert(is_same_v<decltype(result), allocation_result<int*>>);
#endif // ^^^ !defined(__cpp_lib_concepts) ^^^
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

#ifdef __cpp_lib_concepts // TRANSITION, GH-395
        same_as<allocation_result<int*>> auto result = allocator_traits<generous_allocator>::allocate_at_least(al, 4);
#else // ^^^ defined(__cpp_lib_concepts) / !defined(__cpp_lib_concepts) vvv
        auto result = allocator_traits<generous_allocator>::allocate_at_least(al, 4);
        static_assert(is_same_v<decltype(result), allocation_result<int*>>);
#endif // ^^^ !defined(__cpp_lib_concepts) ^^^
        assert(result.ptr);
        assert(result.count == 16);
        al.deallocate(result.ptr, result.count);
    }

    {
        strict_allocator al;

#ifdef __cpp_lib_concepts // TRANSITION, GH-395
        same_as<allocation_result<int*>> auto result = allocator_traits<strict_allocator>::allocate_at_least(al, 4);
#else // ^^^ defined(__cpp_lib_concepts) / !defined(__cpp_lib_concepts) vvv
        auto result = allocator_traits<strict_allocator>::allocate_at_least(al, 4);
        static_assert(is_same_v<decltype(result), allocation_result<int*>>);
#endif // ^^^ !defined(__cpp_lib_concepts) ^^^
        assert(result.ptr);
        assert(result.count == 4);
        al.deallocate(result.ptr, result.count);
    }

    {
        small_allocator al;

#ifdef __cpp_lib_concepts // TRANSITION, GH-395
        same_as<allocation_result<int*, uint32_t>> auto result =
            allocator_traits<small_allocator>::allocate_at_least(al, 4);
#else // ^^^ defined(__cpp_lib_concepts) / !defined(__cpp_lib_concepts) vvv
        auto result = allocator_traits<small_allocator>::allocate_at_least(al, 4);
        static_assert(is_same_v<decltype(result), allocation_result<int*, uint32_t>>);
#endif // ^^^ !defined(__cpp_lib_concepts) ^^^
        assert(result.ptr);
        assert(result.count == 4);
        al.deallocate(result.ptr, result.count);
    }

    {
        huge_allocator al;

#ifdef __cpp_lib_concepts // TRANSITION, GH-395
        same_as<allocation_result<int*, uint64_t>> auto result =
            allocator_traits<huge_allocator>::allocate_at_least(al, 4);
#else // ^^^ defined(__cpp_lib_concepts) / !defined(__cpp_lib_concepts) vvv
        auto result = allocator_traits<huge_allocator>::allocate_at_least(al, 4);
        static_assert(is_same_v<decltype(result), allocation_result<int*, uint64_t>>);
#endif // ^^^ !defined(__cpp_lib_concepts) ^^^
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
