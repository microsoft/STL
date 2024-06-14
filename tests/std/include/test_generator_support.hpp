// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <memory_resource>
#include <new>
#include <type_traits>

template <class T, class AlwaysEqual = std::true_type, std::signed_integral DifferenceType = std::ptrdiff_t>
class StatelessAlloc : public std::allocator<T> {
public:
    using value_type      = T;
    using is_always_equal = AlwaysEqual;
    using difference_type = DifferenceType;
    using size_type       = std::make_unsigned_t<difference_type>;

    StatelessAlloc() = default;

    template <class U>
    StatelessAlloc(const StatelessAlloc<U, AlwaysEqual, DifferenceType>&) {}

    T* allocate(const size_type s) {
        void* vp;
        if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            vp = ::_aligned_malloc(s * sizeof(T), alignof(T));
        } else {
            vp = std::malloc(s * sizeof(T));
        }

        if (vp) {
            return static_cast<T*>(vp);
        }

        throw std::bad_alloc{};
    }

    void deallocate(T* const p, size_type) {
        if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            ::_aligned_free(p);
        } else {
            std::free(p);
        }
    }

    operator std::pmr::polymorphic_allocator<void>() const {
        return {};
    }

    bool operator==(const StatelessAlloc&) const = default;
};

static_assert(std::default_initializable<StatelessAlloc<int>>);

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
class Proxy {
public:
    Proxy(const T&) {}
};

template <std::equality_comparable T>
class Proxy<T> {
public:
    Proxy(const T& _value_) : value(_value_) {}

    bool operator==(const Proxy& other) const {
        return value == other.value;
    }

    bool operator==(const T& x) const {
        return value == x;
    }

private:
    const T& value;
};
