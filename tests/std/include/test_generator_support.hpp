// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <concepts>
#include <cstddef>
#include <generator>
#include <memory>
#include <type_traits>

template <class R, class V = void, class A = void>
struct gen_traits;

template <class R>
struct gen_traits<R> {
    using generator = std::generator<R>;
    using value     = std::remove_cvref_t<R>;
    using reference = R&&;
    using yielded   = reference;

    // Verify the default template arguments
    static_assert(std::same_as<std::generator<R>, std::generator<R, void>>);
    static_assert(std::same_as<std::generator<R, void>, std::generator<R, void, void>>);
};
template <class R, class V>
struct gen_traits<R, V> {
    using generator = std::generator<R, V>;
    using value     = V;
    using reference = R;
    using yielded   = std::conditional_t<std::is_reference_v<R>, R, const R&>;

    // Ditto verify default template arguments
    static_assert(std::same_as<std::generator<R, V>, std::generator<R, V, void>>);
};
template <class R, class V, class A>
struct gen_traits : gen_traits<R, V> {
    using generator = std::generator<R, V, A>;
};

template <class Ref, class V>
using gen_value_t = gen_traits<Ref, V>::value;

template <class Ref, class V>
using gen_reference_t = gen_traits<Ref, V>::reference;

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

    template <class U>
    bool operator==(const StatelessAlloc<U, AlwaysEqual, DifferenceType>&) const noexcept {
        return true;
    }
};

static_assert(std::default_initializable<StatelessAlloc<int>>);

template <class T>
class StatefulAlloc {
public:
    using value_type = T;

    explicit StatefulAlloc(int dom) noexcept : domain{dom} {}

    template <class U>
    StatefulAlloc(const StatefulAlloc<U>& that) noexcept : domain{that.domain} {}

    T* allocate(const size_t n) {
        return std::allocator<T>{}.allocate(n);
    }

    void deallocate(T* const p, const size_t n) noexcept {
        return std::allocator<T>{}.deallocate(p, n);
    }

    template <class U>
    bool operator==(const StatefulAlloc<U>& that) noexcept {
        return domain == that.domain;
    }

private:
    int domain;

    template <class U>
    friend class StatefulAlloc;
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
