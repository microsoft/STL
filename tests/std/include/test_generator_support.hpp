// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <concepts>
#include <cstddef>
#include <memory>
#include <memory_resource>
#include <new>
#include <type_traits>

template <class T, class AlwaysEqual = std::true_type, std::signed_integral DifferenceType = std::ptrdiff_t>
class TestAllocator : public std::allocator<T> {
public:
    using value_type      = T;
    using is_always_equal = AlwaysEqual;
    using difference_type = DifferenceType;
    using size_type       = std::make_unsigned_t<difference_type>;

    TestAllocator() = default;

    template <class U>
    TestAllocator(const TestAllocator<U, AlwaysEqual, DifferenceType>&) {}

    T* allocate(const size_type s) {
        return static_cast<T*>(::operator new(static_cast<size_t>(s * sizeof(T)), std::align_val_t{alignof(T)}));
    }

    void deallocate(T* const p, size_type s) {
        ::operator delete(p, s * sizeof(T), std::align_val_t{alignof(T)});
    }

    operator std::pmr::polymorphic_allocator<void>() const {
        return {};
    }

    bool operator==(const TestAllocator&) const = default;
};

struct MoveOnly {
    MoveOnly()                           = default;
    MoveOnly(const MoveOnly&)            = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&&)                 = default;
    MoveOnly& operator=(MoveOnly&&)      = default;
};

static_assert(std::movable<MoveOnly>);
static_assert(!std::copyable<MoveOnly>);

struct Immovable {
    Immovable()                       = default;
    Immovable(Immovable&&)            = delete;
    Immovable& operator=(Immovable&&) = delete;
};

static_assert(!std::movable<Immovable>);

template <class T>
struct Proxy {
    Proxy(const T&) {}
};
