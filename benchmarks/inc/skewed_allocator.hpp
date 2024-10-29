// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <cstddef>
#include <cstdlib>
#include <new>

template <class T, size_t Alignment, size_t Skew>
struct skewed_allocator {
    using value_type = T;
    static_assert(Alignment % alignof(T) == 0, "Chosen Alignment will produce unaligned T objects");
    static_assert(Skew % alignof(T) == 0, "Chosen Skew will produce unaligned T objects");

    template <class U>
    struct rebind {
        using other = skewed_allocator<U, Alignment, Skew>;
    };

    skewed_allocator() = default;
    template <class U>
    skewed_allocator(const skewed_allocator<U, Alignment, Skew>&) {}

    template <class U>
    bool operator==(const skewed_allocator<U, Alignment, Skew>&) const {
        return true;
    }

    T* allocate(const size_t n) {
        const auto p = static_cast<unsigned char*>(_aligned_malloc(n * sizeof(T) + Skew, Alignment));
        if (!p) {
            throw std::bad_alloc{};
        }
        return reinterpret_cast<T*>(p + Skew);
    }

    void deallocate(T* const p, size_t) {
        if (p) {
            _aligned_free(reinterpret_cast<unsigned char*>(p) - Skew);
        }
    }
};

// The purpose is to provide consistent behavior for benchmarks.
// 64 would be a reasonable alignment for practical perf uses,
// as it is both the cache line size and the maximum vector instruction size (on x64).
// However, aligning to the page size will provide even more consistency
// by ensuring that the same number of page boundaries is crossed each time.
inline constexpr size_t page_size = 4096;

// A realistic skew relative to allocation granularity, when a variable is placed
// next to a pointer in a structure or on the stack. Also corresponds to the default packing.
inline constexpr size_t realistic_skew = 8;

template <class T>
using highly_aligned_allocator = skewed_allocator<T, page_size, 0>;

template <class T>
using not_highly_aligned_allocator = skewed_allocator<T, page_size, realistic_skew>;

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier

template <class T>
struct alignas(page_size) highly_aligned {
    T value;
};

template <class T>
struct alignas(page_size) not_highly_aligned {
    char pad[realistic_skew];
    T value;
};

#pragma warning(pop)
