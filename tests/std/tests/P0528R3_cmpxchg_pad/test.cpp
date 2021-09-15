// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <type_traits>

struct X0 {
    void operator&() const = delete;
};


struct X1 {
    char x : 6;

    void operator&() const = delete;

    void set(const char v) {
        x = v;
    }

    bool check(const char v) const {
        return x == v;
    }
};

struct X2 {
    short x : 9;

    void operator&() const = delete;

    void set(const char v) {
        x = v;
    }

    bool check(const char v) const {
        return x == v;
    }
};

#pragma pack(push, 1)
struct X3 {
    char x : 4;
    char : 2;
    char y : 1;
    short z;

    void operator&() const = delete;

    void set(const char v) {
        x = v;
        y = 0;
        z = ~v;
    }

    bool check(const char v) const {
        return x == v && z == ~v;
    }
};
#pragma pack(pop)

#pragma warning(push)
#pragma warning(disable : 4324) // '%s': structure was padded due to alignment specifier
struct alignas(4) X4 {
    char x;

    void operator&() const = delete;

    void set(const char v) {
        x = v;
    }

    bool check(const char v) const {
        return x == v;
    }
};
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4324) // '%s': structure was padded due to alignment specifier
struct X6 {
    char x;
    alignas(2) char y[2];
    char z;

    void operator&() const = delete;

    void set(const char v) {
        x = v;
        std::memset(&y, 0, sizeof(y));
        z = ~v;
    }

    bool check(const char v) const {
        return x == v && z == ~v;
    }
};
#pragma warning(pop)

struct X8 {
    char x;
    long y;

    void operator&() const = delete;

    void set(const char v) {
        x = v;
        y = 0;
    }

    bool check(const char v) const {
        return x == v;
    }
};

#pragma pack(push, 1)
struct X9 {
    X8 x;
    char z;

    void operator&() const = delete;

    void set(const char v) {
        x.set(v);
        z = ~v;
    }

    bool check(const char v) const {
        return x.check(v) && z == ~v;
    }
};
#pragma pack(pop)

struct X16 {
    long x;
    char y;
    long long z;

    void operator&() const = delete;

    void set(const char v) {
        x = v;
        y = 0;
        z = ~v;
    }

    bool check(const char v) const {
        return x == v && z == ~v;
    }
};

struct X20 {
    long x;
    long y[3];
    char z;

    void operator&() const = delete;

    void set(const char v) {
        x = v;
        std::memset(&y, 0, sizeof(y));
        z = ~v;
    }

    bool check(const char v) const {
        return x == v && z == ~v;
    }
};


template <class X>
void test_atomic() {
    X x1;
    X x2;
    X x3;
    X x4;
    std::memset(std::addressof(x1), 0xaa, sizeof(x1));
    std::memset(std::addressof(x2), 0x55, sizeof(x2));
    std::memset(std::addressof(x3), 0x55, sizeof(x3));
    std::memset(std::addressof(x4), 0x55, sizeof(x4));
    x1.set(5);
    x2.set(5);
    x3.set(6);
    x4.set(7);

    std::atomic<X> v;
    v.store(x1);
    X x;
    std::memcpy(std::addressof(x), std::addressof(x3), sizeof(x));
    assert(!v.compare_exchange_strong(x, x4));
    assert(v.load().check(5));

    v.store(x1);
    for (int retry = 0; retry != 10; ++retry) {
        X xw;
        std::memcpy(std::addressof(xw), std::addressof(x3), sizeof(x));
        assert(!v.compare_exchange_weak(xw, x4));
        assert(v.load().check(5));
    }

    v.store(x1);
    std::memcpy(std::addressof(x), std::addressof(x2), sizeof(x));
    assert(v.compare_exchange_strong(x, x3));
    assert(v.load().check(6));

    v.store(x1);
    for (;;) {
        X xw;
        std::memcpy(std::addressof(xw), std::addressof(x2), sizeof(x));
        if (v.compare_exchange_weak(xw, x3)) {
            break;
        }
    }
    assert(v.load().check(6));
}

template <class X>
void test_atomic_0() {
    X x1;
    X x2;
    X x3;
    std::memset(std::addressof(x1), 0xaa, sizeof(x1));
    std::memset(std::addressof(x2), 0x55, sizeof(x2));
    std::memset(std::addressof(x3), 0x55, sizeof(x3));

    std::atomic<X> v;
    v.store(x1);
    X x;
    std::memcpy(std::addressof(x), std::addressof(x3), sizeof(x));
    assert(v.compare_exchange_strong(x1, x2));
}

template <class X>
void test_atomic_ref() {
    X x1;
    X x2;
    X x3;
    X x4;
    std::memset(std::addressof(x1), 0xaa, sizeof(x1));
    std::memset(std::addressof(x2), 0x55, sizeof(x2));
    std::memset(std::addressof(x3), 0x55, sizeof(x3));
    std::memset(std::addressof(x4), 0x55, sizeof(x4));
    x1.set(5);
    x2.set(5);
    x3.set(6);
    x4.set(7);

    alignas(std::atomic_ref<X>::required_alignment) X v = x1;
    X x;
    std::memcpy(std::addressof(x), std::addressof(x3), sizeof(x));
    assert(!std::atomic_ref<X>(v).compare_exchange_strong(x, x4));
    assert(std::atomic_ref<X>(v).load().check(5));

    std::atomic_ref<X>(v).store(x1);
    for (int retry = 0; retry != 10; ++retry) {
        X xw;
        std::memcpy(std::addressof(xw), std::addressof(x3), sizeof(x));
        assert(!std::atomic_ref<X>(v).compare_exchange_weak(xw, x4));
        assert(std::atomic_ref<X>(v).load().check(5));
    }

    std::atomic_ref<X>(v).store(x1);
    std::memcpy(std::addressof(x), std::addressof(x2), sizeof(x));
    assert(std::atomic_ref<X>(v).compare_exchange_strong(x, x3));
    assert(std::atomic_ref<X>(v).load().check(6));

    std::atomic_ref<X>(v).store(x1);
    for (;;) {
        X xw;
        std::memcpy(std::addressof(xw), std::addressof(x2), sizeof(x));
        if (std::atomic_ref<X>(v).compare_exchange_weak(xw, x3)) {
            break;
        }
    }
    assert(std::atomic_ref<X>(v).load().check(6));
}

template <class X>
void test_atomic_ref_0() {
    X x1;
    X x2;
    X x3;
    std::memset(std::addressof(x1), 0xaa, sizeof(x1));
    std::memset(std::addressof(x2), 0x55, sizeof(x2));
    std::memset(std::addressof(x3), 0x55, sizeof(x3));

    alignas(std::atomic_ref<X>::required_alignment) X v = x1;
    X x;
    std::memcpy(std::addressof(x), std::addressof(x3), sizeof(x));
    assert(std::atomic_ref<X>(v).compare_exchange_strong(x1, x2));
}

template <class X, std::size_t S>
void test() {
    static_assert(sizeof(X) == S, "Unexpected size");
    static_assert(
        !std::has_unique_object_representations_v<X>, "Type without padding is not useful for testing P0528.");
    test_atomic<X>();
    test_atomic_ref<X>();
}

template <class X>
void test0() {
    static_assert(
        !std::has_unique_object_representations_v<X>, "Type without padding is not useful for testing P0528.");
    test_atomic_0<X>();
    test_atomic_ref_0<X>();
}

int main() {
#ifndef __clang__ // TRANSITION, LLVM-46685
    test0<X0>();
    test<X1, 1>();
    test<X2, 2>();
    test<X3, 3>();
    test<X4, 4>();
    test<X6, 6>();
    test<X8, 8>();
    test<X9, 9>();
    test<X16, 16>();
    test<X20, 20>();
#endif // !__clang__, TRANSITION, LLVM-46685
    return 0;
}
