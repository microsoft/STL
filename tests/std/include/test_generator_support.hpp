// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <memory_resource>
#include <type_traits>

template <class T, class AlwaysEqual = std::true_type, std::signed_integral DifferenceType = std::ptrdiff_t>
class StatelessAlloc {
public:
    using value_type      = T;
    using is_always_equal = AlwaysEqual;
    using difference_type = DifferenceType;
    using size_type       = std::make_unsigned_t<difference_type>;

    StatelessAlloc() = default;

    template <class U>
    StatelessAlloc(const StatelessAlloc<U, AlwaysEqual, DifferenceType>&) {}

    T* allocate(const size_type s) {
        return std::allocator<T>{}.allocate(s);
    }

    void deallocate(T* const p, const size_type n) noexcept {
        std::allocator<T>{}.deallocate(p, n);
    }

    operator std::pmr::polymorphic_allocator<void>() const {
        return {};
    }

    template <class U>
    bool operator==(const StatelessAlloc<U, AlwaysEqual, DifferenceType>&) const noexcept {
        return true;
    }
};

static_assert(std::default_initializable<StatelessAlloc<int>>);

template <class T>
struct StatefulAlloc {
    using value_type = T;

    int domain;

    explicit StatefulAlloc(int dom) noexcept : domain{dom} {}

    template <class U>
    constexpr StatefulAlloc(const StatefulAlloc<U>& that) noexcept : domain{that.domain} {}

    T* allocate(const size_t n) {
        return std::allocator<T>{}.allocate(n);
    }

    void deallocate(T* const p, const size_t n) noexcept {
        return std::allocator<T>{}.deallocate(p, n);
    }

    template <class U>
    constexpr bool operator==(const StatefulAlloc<U>& that) noexcept {
        return domain == that.domain;
    }
};

static_assert(!std::default_initializable<StatefulAlloc<int>>);

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
