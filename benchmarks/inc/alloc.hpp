// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <cstddef>
#include <cstdlib>
#include <stdexcept>

template <class T, size_t Alignment, size_t Skew>
struct skewed_allocator {
    using value_type = T;
    static_assert(
        Alignment % alignof(T) == 0 && Skew % alignof(T) == 0, "Chosen parameters will produce unaligned T objects");

    template <class U>
    struct rebind {
        using type = skewed_allocator<U, Alignment, Skew>;
    };

    skewed_allocator() = default;
    template <class U>
    skewed_allocator(const skewed_allocator<U, Alignment, Skew>&) {}

    template <class U>
    bool operator==(const skewed_allocator<U, Alignment, Skew>&) const {
        return true;
    }
    T* allocate(size_t n) {
        const auto p = static_cast<unsigned char*>(_aligned_malloc(n * sizeof(T) + Skew, Alignment));
        if (!p) {
            throw std::bad_alloc{};
        }
        return reinterpret_cast<T*>(p + Skew);
    }

    void deallocate(T* p, size_t) {
        if (p) {
            _aligned_free(reinterpret_cast<unsigned char*>(p) - Skew);
        }
    }
};

// The purpose is to provide consistent behavior for benchmarks.
// 64 seems to be reasonable alignment for practical perf uses,
// as it is both cache line size and maximum vector instruction size (on x64).
// However to provide even more consistency, aligning to page,
// to make sure the same number of page boundaries is crossed each time.
constexpr size_t page_size = 4096;

// A realistic skew relative to allocation granularity, when a variable is placed
// next to a pointer in a structure or on stack. Also corresponds to the default packing.
constexpr size_t skew = 8;

template <class T>
struct highly_aligned_allocator : skewed_allocator<T, page_size, 0> {};

template <class T>
struct not_highly_aligned_allocator : skewed_allocator<T, page_size, skew> {};

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier

template <class T>
struct alignas(page_size) highly_aligned {
    T value;
};

template <class T>
struct alignas(page_size) not_highly_aligned {
    char pad[skew];
    T value;
};

#pragma warning(pop)
