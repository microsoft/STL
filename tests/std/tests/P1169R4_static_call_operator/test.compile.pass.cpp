// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef __cpp_static_call_operator

#include <functional>
#include <future>
#include <type_traits>
using namespace std;

struct F0 {
    static char operator()();
};

struct F1 {
    static short operator()(float);
};

struct F2 {
    static int operator()(double*, double&);
};

struct F3 {
    static void operator()(const long&, long&&, const long&&);
};

struct Base {
    static bool operator()(unsigned int);
};

struct Derived : Base {};

struct Nothrow {
    static char16_t operator()(char32_t) noexcept;
};

template <template <class> class Temp>
void test_ctad() {
    static_assert(is_same_v<decltype(Temp{F0{}}), Temp<char()>>);
    static_assert(is_same_v<decltype(Temp{F1{}}), Temp<short(float)>>);
    static_assert(is_same_v<decltype(Temp{F2{}}), Temp<int(double*, double&)>>);
    static_assert(is_same_v<decltype(Temp{F3{}}), Temp<void(const long&, long&&, const long&&)>>);
    static_assert(is_same_v<decltype(Temp{Derived{}}), Temp<bool(unsigned int)>>);
    static_assert(is_same_v<decltype(Temp{Nothrow{}}), Temp<char16_t(char32_t)>>);

#if !(defined(__clang__) && defined(_M_IX86)) // TRANSITION, LLVM-62594, fixed in Clang 18
    auto lambda = [](int* p, int** q) static { return *p + **q; };
    static_assert(is_same_v<decltype(Temp{lambda}), Temp<int(int*, int**)>>);
#endif // ^^^ no workaround ^^^
}

void all_tests() {
    test_ctad<function>();
    test_ctad<packaged_task>();
}

#endif // ^^^ defined(__cpp_static_call_operator) ^^^
