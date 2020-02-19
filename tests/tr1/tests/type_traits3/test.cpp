// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <type_traits> header, part 3
#define TEST_NAME "<type_traits>, part 3"

#include "tdefs.h"
#include "typetr.h"
#include <type_traits>

// TESTS
static void t_is_trivially_constructible() { // test is_trivially_constructible<T, Args...> for various types
    T_UNSPEC_CV(is_trivially_constructible, void, false);
    T_UNSPEC_CV(is_trivially_constructible, char, true);
    T_UNSPEC_CV(is_trivially_constructible, signed char, true);
    T_UNSPEC_CV(is_trivially_constructible, unsigned char, true);
    T_UNSPEC_CV(is_trivially_constructible, wchar_t, true);
    T_UNSPEC_CV(is_trivially_constructible, short, true);
    T_UNSPEC_CV(is_trivially_constructible, unsigned short, true);
    T_UNSPEC_CV(is_trivially_constructible, int, true);
    T_UNSPEC_CV(is_trivially_constructible, unsigned int, true);
    T_UNSPEC_CV(is_trivially_constructible, long, true);
    T_UNSPEC_CV(is_trivially_constructible, unsigned long, true);

    T_UNSPEC_CV(is_trivially_constructible, long long, true);
    T_UNSPEC_CV(is_trivially_constructible, unsigned long long, true);

    T_UNSPEC_CV(is_trivially_constructible, float, true);
    T_UNSPEC_CV(is_trivially_constructible, double, true);
    T_UNSPEC_CV(is_trivially_constructible, long double, true);
    T_UNSPEC_CV(is_trivially_constructible, int*, true);
    T_UNSPEC(is_trivially_constructible, int&, false);
    T_UNSPEC_CV(is_trivially_constructible, B, false);
    T_UNSPEC_CV(is_trivially_constructible, C, false);
    T_UNSPEC_CV(is_trivially_constructible, D, true);
    T_UNSPEC_CV(is_trivially_constructible, pmo, true);
    T_UNSPEC_CV(is_trivially_constructible, pmoc, true);
    T_UNSPEC_CV(is_trivially_constructible, pmov, true);
    T_UNSPEC_CV(is_trivially_constructible, pmocv, true);
    T_UNSPEC_CV(is_trivially_constructible, pmf, true);
    T_UNSPEC_CV(is_trivially_constructible, pmfc, true);
    T_UNSPEC_CV(is_trivially_constructible, pmfv, true);
    T_UNSPEC_CV(is_trivially_constructible, pmfcv, true);
    T_UNSPEC_CV(is_trivially_constructible, arr_t, true);
    T_UNSPEC_CV(is_trivially_constructible, enum_t, true);
    T_UNSPEC_CV(is_trivially_constructible, pf, true);
    T_UNSPEC(is_trivially_constructible, func, false);
    T_UNSPEC_CV(is_trivially_constructible, void*, true);

    bool ans = STD is_trivially_constructible<int>::value;
    CHECK(ans);
    ans = STD is_trivially_constructible<int, int>::value;
    CHECK(ans);
    ans = STD is_trivially_constructible<int, int, int>::value;
    CHECK(!ans);
    ans = STD is_trivially_constructible<int, int, int, int>::value;
    CHECK(!ans);
    ans = STD is_trivially_constructible<int, int, int, int, int>::value;
    CHECK(!ans);
}

static void t_is_trivially_default_constructible() { // test is_trivially_default_constructible<T> for various types
                                                     // (minimal tests in absence of compiler support)
    T_TRIVIAL_CV(is_trivially_default_constructible, void, false);
    T_TRIVIAL_CV(is_trivially_default_constructible, char, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, signed char, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, unsigned char, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, wchar_t, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, short, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, unsigned short, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, int, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, unsigned int, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, long, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, unsigned long, true);

    T_TRIVIAL_CV(is_trivially_default_constructible, long long, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, unsigned long long, true);

    T_TRIVIAL_CV(is_trivially_default_constructible, float, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, double, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, long double, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, int*, true);
    T_TRIVIAL(is_trivially_default_constructible, int&, false);
    T_TRIVIAL_CV(is_trivially_default_constructible, B, false);
    T_TRIVIAL_CV(is_trivially_default_constructible, C, false);
    T_TRIVIAL_CV(is_trivially_default_constructible, D, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, pmo, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, pmoc, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, pmov, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, pmocv, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, pmf, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, pmfc, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, pmfv, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, pmfcv, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, arr_t, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, enum_t, true);
    T_TRIVIAL_CV(is_trivially_default_constructible, pf, true);
    T_TRIVIAL(is_trivially_default_constructible, func, false);
    T_TRIVIAL_CV(is_trivially_default_constructible, void*, true);
}

static void t_is_trivially_copy_constructible() { // test is_trivially_copy_constructible<T> for various types
    T_UNSPEC_CV(is_trivially_copy_constructible, void, false);
    T_UNSPEC_CV(is_trivially_copy_constructible, char, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, signed char, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, unsigned char, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, wchar_t, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, short, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, unsigned short, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, int, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, unsigned int, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, long, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, unsigned long, true);

    T_UNSPEC_CV(is_trivially_copy_constructible, long long, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, unsigned long long, true);

    T_UNSPEC_CV(is_trivially_copy_constructible, float, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, double, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, long double, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, int*, true);
    T_UNSPEC(is_trivially_copy_constructible, int&, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, B, false);
    T_UNSPEC_CV(is_trivially_copy_constructible, C, false);
    T_UNSPEC_C(is_trivially_copy_constructible, D, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, pmo, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, pmoc, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, pmov, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, pmocv, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, pmf, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, pmfc, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, pmfv, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, pmfcv, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, arr_t, false);
    T_UNSPEC_CV(is_trivially_copy_constructible, enum_t, true);
    T_UNSPEC_CV(is_trivially_copy_constructible, pf, true);
    T_UNSPEC(is_trivially_copy_constructible, func, false);
    T_UNSPEC_CV(is_trivially_copy_constructible, void*, true);
}

static void t_is_trivially_copyable() { // test is_trivially_copyable<T> for various types
    T_UNSPEC_CV(is_trivially_copyable, void, false);
    T_UNSPEC_C(is_trivially_copyable, char, true);
    T_UNSPEC_C(is_trivially_copyable, signed char, true);
    T_UNSPEC_C(is_trivially_copyable, unsigned char, true);
    T_UNSPEC_C(is_trivially_copyable, wchar_t, true);
    T_UNSPEC_C(is_trivially_copyable, short, true);
    T_UNSPEC_C(is_trivially_copyable, unsigned short, true);
    T_UNSPEC_C(is_trivially_copyable, int, true);
    T_UNSPEC_C(is_trivially_copyable, unsigned int, true);
    T_UNSPEC_C(is_trivially_copyable, long, true);
    T_UNSPEC_C(is_trivially_copyable, unsigned long, true);

    T_UNSPEC_C(is_trivially_copyable, long long, true);
    T_UNSPEC_C(is_trivially_copyable, unsigned long long, true);

    T_UNSPEC_C(is_trivially_copyable, float, true);
    T_UNSPEC_C(is_trivially_copyable, double, true);
    T_UNSPEC_C(is_trivially_copyable, long double, true);
    T_UNSPEC_C(is_trivially_copyable, int*, true);
    T_UNSPEC(is_trivially_copyable, int&, false);
    T_UNSPEC_CV(is_trivially_copyable, B, false);
    T_UNSPEC_CV(is_trivially_copyable, C, false);
    T_UNSPEC_C(is_trivially_copyable, D, true);
    T_UNSPEC_C(is_trivially_copyable, pmo, true);
    T_UNSPEC_C(is_trivially_copyable, pmoc, true);
    T_UNSPEC_C(is_trivially_copyable, pmov, true);
    T_UNSPEC_C(is_trivially_copyable, pmocv, true);
    T_UNSPEC_C(is_trivially_copyable, pmf, true);
    T_UNSPEC_C(is_trivially_copyable, pmfc, true);
    T_UNSPEC_C(is_trivially_copyable, pmfv, true);
    T_UNSPEC_C(is_trivially_copyable, pmfcv, true);
    T_UNSPEC_C(is_trivially_copyable, arr_t, true);
    T_UNSPEC_C(is_trivially_copyable, enum_t, true);
    T_UNSPEC_C(is_trivially_copyable, pf, true);
    T_UNSPEC(is_trivially_copyable, func, false);
    T_UNSPEC_C(is_trivially_copyable, void*, true);
}

static void t_is_trivially_move_constructible() { // test is_trivially_move_constructible<T> for various types
                                                  // (minimal tests in absence of compiler support)
    T_TRIVIAL_CV(is_trivially_move_constructible, void, false);
    T_TRIVIAL_CV(is_trivially_move_constructible, char, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, signed char, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, unsigned char, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, wchar_t, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, short, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, unsigned short, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, int, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, unsigned int, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, long, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, unsigned long, true);

    T_TRIVIAL_CV(is_trivially_move_constructible, long long, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, unsigned long long, true);

    T_TRIVIAL_CV(is_trivially_move_constructible, float, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, double, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, long double, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, int*, true);
    T_TRIVIAL(is_trivially_move_constructible, int&, true);

    T_TRIVIAL_CV(is_trivially_move_constructible, B, false);
    T_TRIVIAL_CV(is_trivially_move_constructible, C, false);

    T_TRIVIAL_C(is_trivially_move_constructible, D, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, pmo, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, pmoc, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, pmov, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, pmocv, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, pmf, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, pmfc, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, pmfv, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, pmfcv, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, arr_t, false);
    T_TRIVIAL_CV(is_trivially_move_constructible, enum_t, true);
    T_TRIVIAL_CV(is_trivially_move_constructible, pf, true);
    T_TRIVIAL(is_trivially_move_constructible, func, false);
    T_TRIVIAL_CV(is_trivially_move_constructible, void*, true);
}

static void t_is_trivially_assignable() { // test is_trivially_assignable<T> for various types
                                          // (minimal tests in absence of compiler support)
    bool ans = STD is_trivially_assignable<B, B>::value;
    CHECK(!ans);
    ans = STD is_trivially_assignable<int&, int>::value;
    CHECK(ans);
    ans = STD is_trivially_assignable<int&, B>::value;
    CHECK(!ans);
    // ans = STD is_trivially_assignable<void, void>::value;
    CHECK(!ans);
}

static void t_is_trivially_copy_assignable() { // test is_trivially_copy_assignable<T> for various types
                                               // (minimal tests in absence of compiler support)
    T_TRIVIAL_V(is_trivially_copy_assignable, void, false);
    T_TRIVIAL_V(is_trivially_copy_assignable, char, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, signed char, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, unsigned char, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, wchar_t, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, short, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, unsigned short, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, int, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, unsigned int, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, long, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, unsigned long, true);

    T_TRIVIAL_V(is_trivially_copy_assignable, long long, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, unsigned long long, true);

    T_TRIVIAL_V(is_trivially_copy_assignable, float, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, double, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, long double, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, int*, true);
    T_TRIVIAL(is_trivially_copy_assignable, int&, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, B, false);
    T_TRIVIAL_V(is_trivially_copy_assignable, C, false);
    T_TRIVIAL(is_trivially_copy_assignable, D, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, pmo, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, pmoc, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, pmov, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, pmocv, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, pmf, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, pmfc, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, pmfv, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, pmfcv, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, arr_t, false);
    T_TRIVIAL_V(is_trivially_copy_assignable, enum_t, true);
    T_TRIVIAL_V(is_trivially_copy_assignable, pf, true);
    T_TRIVIAL(is_trivially_copy_assignable, func, false);
    T_TRIVIAL_V(is_trivially_copy_assignable, void*, true);
}

static void t_is_trivially_move_assignable() { // test is_trivially_move_assignable<T> for various types
                                               // (minimal tests in absence of compiler support)
    // T_TRIVIAL_V(is_trivially_move_assignable, void, false);
    T_TRIVIAL_V(is_trivially_move_assignable, char, true);
    T_TRIVIAL_V(is_trivially_move_assignable, signed char, true);
    T_TRIVIAL_V(is_trivially_move_assignable, unsigned char, true);
    T_TRIVIAL_V(is_trivially_move_assignable, wchar_t, true);
    T_TRIVIAL_V(is_trivially_move_assignable, short, true);
    T_TRIVIAL_V(is_trivially_move_assignable, unsigned short, true);
    T_TRIVIAL_V(is_trivially_move_assignable, int, true);
    T_TRIVIAL_V(is_trivially_move_assignable, unsigned int, true);
    T_TRIVIAL_V(is_trivially_move_assignable, long, true);
    T_TRIVIAL_V(is_trivially_move_assignable, unsigned long, true);

    T_TRIVIAL_V(is_trivially_move_assignable, long long, true);
    T_TRIVIAL_V(is_trivially_move_assignable, unsigned long long, true);

    T_TRIVIAL_V(is_trivially_move_assignable, float, true);
    T_TRIVIAL_V(is_trivially_move_assignable, double, true);
    T_TRIVIAL_V(is_trivially_move_assignable, long double, true);
    T_TRIVIAL_V(is_trivially_move_assignable, int*, true);
    T_TRIVIAL(is_trivially_move_assignable, int&, true);

    T_TRIVIAL_V(is_trivially_move_assignable, B, false);
    T_TRIVIAL_V(is_trivially_move_assignable, C, false);

    T_TRIVIAL(is_trivially_move_assignable, D, true);
    T_TRIVIAL_V(is_trivially_move_assignable, pmo, true);
    T_TRIVIAL_V(is_trivially_move_assignable, pmoc, true);
    T_TRIVIAL_V(is_trivially_move_assignable, pmov, true);
    T_TRIVIAL_V(is_trivially_move_assignable, pmocv, true);
    T_TRIVIAL_V(is_trivially_move_assignable, pmf, true);
    T_TRIVIAL_V(is_trivially_move_assignable, pmfc, true);
    T_TRIVIAL_V(is_trivially_move_assignable, pmfv, true);
    T_TRIVIAL_V(is_trivially_move_assignable, pmfcv, true);
    T_TRIVIAL_V(is_trivially_move_assignable, arr_t, false);
    T_TRIVIAL_V(is_trivially_move_assignable, enum_t, true);
    T_TRIVIAL_V(is_trivially_move_assignable, pf, true);
    T_TRIVIAL(is_trivially_move_assignable, func, false);
    T_TRIVIAL_V(is_trivially_move_assignable, void*, true);
}

static void t_is_trivially_destructible() { // test is_trivially_destructible<T> for various types
                                            // (minimal tests in absence of compiler support)
    T_TRIVIAL_CV(is_trivially_destructible, void, false);
    T_TRIVIAL_CV(is_trivially_destructible, char, true);
    T_TRIVIAL_CV(is_trivially_destructible, signed char, true);
    T_TRIVIAL_CV(is_trivially_destructible, unsigned char, true);
    T_TRIVIAL_CV(is_trivially_destructible, wchar_t, true);
    T_TRIVIAL_CV(is_trivially_destructible, short, true);
    T_TRIVIAL_CV(is_trivially_destructible, unsigned short, true);
    T_TRIVIAL_CV(is_trivially_destructible, int, true);
    T_TRIVIAL_CV(is_trivially_destructible, unsigned int, true);
    T_TRIVIAL_CV(is_trivially_destructible, long, true);
    T_TRIVIAL_CV(is_trivially_destructible, unsigned long, true);

    T_TRIVIAL_CV(is_trivially_destructible, long long, true);
    T_TRIVIAL_CV(is_trivially_destructible, unsigned long long, true);

    T_TRIVIAL_CV(is_trivially_destructible, float, true);
    T_TRIVIAL_CV(is_trivially_destructible, double, true);
    T_TRIVIAL_CV(is_trivially_destructible, long double, true);
    T_TRIVIAL_CV(is_trivially_destructible, int*, true);
    T_TRIVIAL(is_trivially_destructible, int&, true);
    T_TRIVIAL_CV(is_trivially_destructible, B, false);
    T_TRIVIAL_CV(is_trivially_destructible, C, false);
    T_TRIVIAL_CV(is_trivially_destructible, D, true);
    T_TRIVIAL_CV(is_trivially_destructible, pmo, true);
    T_TRIVIAL_CV(is_trivially_destructible, pmoc, true);
    T_TRIVIAL_CV(is_trivially_destructible, pmov, true);
    T_TRIVIAL_CV(is_trivially_destructible, pmocv, true);
    T_TRIVIAL_CV(is_trivially_destructible, pmf, true);
    T_TRIVIAL_CV(is_trivially_destructible, pmfc, true);
    T_TRIVIAL_CV(is_trivially_destructible, pmfv, true);
    T_TRIVIAL_CV(is_trivially_destructible, pmfcv, true);
    T_TRIVIAL_CV(is_trivially_destructible, arr_t, true);
    T_TRIVIAL_CV(is_trivially_destructible, enum_t, true);
    T_TRIVIAL_CV(is_trivially_destructible, pf, true);
    T_TRIVIAL(is_trivially_destructible, func, false);
    T_TRIVIAL_CV(is_trivially_destructible, void*, true);
}

void test_main() { // test type traits
    t_is_trivially_constructible();
    t_is_trivially_default_constructible();
    t_is_trivially_copy_constructible();
    t_is_trivially_copyable();
    t_is_trivially_move_constructible();

    t_is_trivially_assignable();
    t_is_trivially_copy_assignable();
    t_is_trivially_move_assignable();

    t_is_trivially_destructible();
}
