// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstring>

struct X0 {
    void operator&() const = delete;
};


struct X1 {
    char x : 6;

    void operator&() const = delete;

    void set(char v) {
        x = v;
    }
};

struct X2 {
    short x : 9;

    void operator&() const = delete;

    void set(char v) {
        x = v;
    }
};

#pragma pack(push, 1)
struct X3 {
    char x : 4;
    char : 2;
    char y : 1;
    short z;

    void operator&() const = delete;

    void set(char v) {
        x = v;
        y = 0;
        z = 0;
    }
};
#pragma pack(pop)

#pragma warning(push)
#pragma warning(disable : 4324) // '%s': structure was padded due to alignment specifier
struct alignas(4) X4 {
    char x;

    void operator&() const = delete;

    void set(char v) {
        x = v;
    }
};
#pragma warning(pop)

struct X8 {
    char x;
    long y;

    void operator&() const = delete;

    void set(char v) {
        x = v;
        y = 0;
    }
};

struct X16 {
    long x;
    char y;
    long long z;

    void operator&() const = delete;

    void set(char v) {
        x = v;
        y = 0;
        z = 0;
    }
};

struct X20 {
    long x;
    long y[3];
    char z;

    void operator&() const = delete;

    void set(char v) {
        x = v;
        std::memset(&y, 0, sizeof(y));
        z = 0;
    }
};


template <class X, std::size_t S>
void test() {
    static_assert(sizeof(X) == S, "Unexpected size");
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
    assert(v.load().x == 5);

    v.store(x1);
    for (int retry = 0; retry != 10; ++retry) {
        X xw;
        std::memcpy(std::addressof(xw), std::addressof(x3), sizeof(x));
        assert(!v.compare_exchange_weak(xw, x4));
        assert(v.load().x == 5);
    }

    v.store(x1);
    std::memcpy(std::addressof(x), std::addressof(x2), sizeof(x));
    assert(v.compare_exchange_strong(x, x3));
    assert(v.load().x == 6);

    v.store(x1);
    for (;;) {
        X xw;
        std::memcpy(std::addressof(xw), std::addressof(x2), sizeof(x));
        if (v.compare_exchange_weak(xw, x3)) {
            break;
        }
    }
    assert(v.load().x == 6);
}


template <class X>
void test0() {
    X x1;
    X x2;
    std::memset(std::addressof(x1), 0xaa, sizeof(x1));
    std::memset(std::addressof(x2), 0x55, sizeof(x2));

    std::atomic<X> v;
    v.store(x1);
    X x;

    assert(v.compare_exchange_strong(x, x2));
}

int main() {
#ifndef __clang__ // TRANSITION, LLVM-46685
    test0<X0>();
    test<X1, 1>();
    test<X2, 2>();
    test<X3, 3>();
    test<X4, 4>();
    test<X8, 8>();
    test<X16, 16>();
    test<X20, 20>();
#endif // !__clang__, TRANSITION, LLVM-46685
    return 0;
}
