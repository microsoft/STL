// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <type_traits> header, part 4
#define TEST_NAME "<type_traits>, part 4"

#include "tdefs.h"
#include "typetr.h"
#include <limits>
#include <type_traits>

// TESTS
static void t_is_nothrow_constructible() { // test is_nothrow_constructible<T, Args...> for various types
    T_UNSPEC_CV(is_nothrow_constructible, void, false);
    T_UNSPEC_CV(is_nothrow_constructible, char, true);
    T_UNSPEC_CV(is_nothrow_constructible, signed char, true);
    T_UNSPEC_CV(is_nothrow_constructible, unsigned char, true);
    T_UNSPEC_CV(is_nothrow_constructible, wchar_t, true);
    T_UNSPEC_CV(is_nothrow_constructible, short, true);
    T_UNSPEC_CV(is_nothrow_constructible, unsigned short, true);
    T_UNSPEC_CV(is_nothrow_constructible, int, true);
    T_UNSPEC_CV(is_nothrow_constructible, unsigned int, true);
    T_UNSPEC_CV(is_nothrow_constructible, long, true);
    T_UNSPEC_CV(is_nothrow_constructible, unsigned long, true);

    T_UNSPEC_CV(is_nothrow_constructible, long long, true);
    T_UNSPEC_CV(is_nothrow_constructible, unsigned long long, true);

    T_UNSPEC_CV(is_nothrow_constructible, float, true);
    T_UNSPEC_CV(is_nothrow_constructible, double, true);
    T_UNSPEC_CV(is_nothrow_constructible, long double, true);
    T_UNSPEC_CV(is_nothrow_constructible, int*, true);
    T_UNSPEC(is_nothrow_constructible, int&, false);

    T_UNSPEC_CV(is_nothrow_constructible, B, true);
    T_UNSPEC_CV(is_nothrow_constructible, C, true);

    T_UNSPEC_CV(is_nothrow_constructible, D, true);
    T_UNSPEC_CV(is_nothrow_constructible, pmo, true);
    T_UNSPEC_CV(is_nothrow_constructible, pmoc, true);
    T_UNSPEC_CV(is_nothrow_constructible, pmov, true);
    T_UNSPEC_CV(is_nothrow_constructible, pmocv, true);
    T_UNSPEC_CV(is_nothrow_constructible, pmf, true);
    T_UNSPEC_CV(is_nothrow_constructible, pmfc, true);
    T_UNSPEC_CV(is_nothrow_constructible, pmfv, true);
    T_UNSPEC_CV(is_nothrow_constructible, pmfcv, true);
    T_UNSPEC_CV(is_nothrow_constructible, arr_t, true);
    T_UNSPEC_CV(is_nothrow_constructible, enum_t, true);
    T_UNSPEC_CV(is_nothrow_constructible, pf, true);
    T_UNSPEC(is_nothrow_constructible, func, false);
    T_UNSPEC_CV(is_nothrow_constructible, void*, true);

    bool ans = STD is_nothrow_constructible<int>::value;
    CHECK(ans);
    ans = STD is_nothrow_constructible<int, int>::value;
    CHECK(ans);
    ans = STD is_nothrow_constructible<int, int, int>::value;
    CHECK(!ans);
    ans = STD is_nothrow_constructible<int, int, int, int>::value;
    CHECK(!ans);
    ans = STD is_nothrow_constructible<int, int, int, int, int>::value;
    CHECK(!ans);
}

static void t_is_nothrow_default_constructible() { // test is_nothrow_default_constructible<T> for various types
                                                   // (minimal tests in absence of compiler support)
    T_TRIVIAL_CV(is_nothrow_default_constructible, void, false);
    T_TRIVIAL_CV(is_nothrow_default_constructible, char, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, signed char, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, unsigned char, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, wchar_t, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, short, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, unsigned short, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, int, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, unsigned int, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, long, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, unsigned long, true);

    T_TRIVIAL_CV(is_nothrow_default_constructible, long long, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, unsigned long long, true);

    T_TRIVIAL_CV(is_nothrow_default_constructible, float, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, double, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, long double, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, int*, true);
    T_TRIVIAL(is_nothrow_default_constructible, int&, false);

    T_TRIVIAL_CV(is_nothrow_default_constructible, B, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, C, true);

    T_TRIVIAL_CV(is_nothrow_default_constructible, D, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, pmo, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, pmoc, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, pmov, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, pmocv, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, pmf, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, pmfc, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, pmfv, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, pmfcv, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, arr_t, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, enum_t, true);
    T_TRIVIAL_CV(is_nothrow_default_constructible, pf, true);
    T_TRIVIAL(is_nothrow_default_constructible, func, false);
    T_TRIVIAL_CV(is_nothrow_default_constructible, void*, true);
}

static void t_is_nothrow_copy_constructible() { // test is_nothrow_copy_constructible<T> for various types
    T_UNSPEC_CV(is_nothrow_copy_constructible, void, false);
    T_UNSPEC_CV(is_nothrow_copy_constructible, char, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, signed char, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, unsigned char, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, wchar_t, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, short, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, unsigned short, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, int, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, unsigned int, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, long, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, unsigned long, true);

    T_UNSPEC_CV(is_nothrow_copy_constructible, long long, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, unsigned long long, true);

    T_UNSPEC_CV(is_nothrow_copy_constructible, float, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, double, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, long double, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, int*, true);
    T_UNSPEC(is_nothrow_copy_constructible, int&, true);
    T_UNSPEC_C(is_nothrow_copy_constructible, B, true);
    T_UNSPEC_C(is_nothrow_copy_constructible, C, true);
    T_UNSPEC_C(is_nothrow_copy_constructible, D, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, pmo, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, pmoc, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, pmov, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, pmocv, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, pmf, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, pmfc, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, pmfv, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, pmfcv, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, arr_t, false);
    T_UNSPEC_CV(is_nothrow_copy_constructible, enum_t, true);
    T_UNSPEC_CV(is_nothrow_copy_constructible, pf, true);
    T_UNSPEC(is_nothrow_copy_constructible, func, false);
    T_UNSPEC_CV(is_nothrow_copy_constructible, void*, true);
}

static void t_is_nothrow_move_constructible() { // test is_nothrow_move_constructible<T> for various types
                                                // (minimal tests in absence of compiler support)
    T_TRIVIAL_CV(is_nothrow_move_constructible, void, false);
    T_TRIVIAL_CV(is_nothrow_move_constructible, char, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, signed char, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, unsigned char, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, wchar_t, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, short, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, unsigned short, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, int, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, unsigned int, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, long, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, unsigned long, true);

    T_TRIVIAL_CV(is_nothrow_move_constructible, long long, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, unsigned long long, true);

    T_TRIVIAL_CV(is_nothrow_move_constructible, float, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, double, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, long double, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, int*, true);
    T_TRIVIAL(is_nothrow_move_constructible, int&, true);
    T_TRIVIAL_C(is_nothrow_move_constructible, B, true);
    T_TRIVIAL_C(is_nothrow_move_constructible, C, true);
    T_TRIVIAL_C(is_nothrow_move_constructible, D, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, pmo, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, pmoc, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, pmov, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, pmocv, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, pmf, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, pmfc, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, pmfv, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, pmfcv, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, arr_t, false);
    T_TRIVIAL_CV(is_nothrow_move_constructible, enum_t, true);
    T_TRIVIAL_CV(is_nothrow_move_constructible, pf, true);
    T_TRIVIAL(is_nothrow_move_constructible, func, false);
    T_TRIVIAL_CV(is_nothrow_move_constructible, void*, true);
}

static void t_is_nothrow_assignable() { // test is_nothrow_assignable<T> for various types
                                        // (minimal tests in absence of compiler support)
    bool ans = STD is_nothrow_assignable<B, B>::value;
    CHECK(ans);
    ans = STD is_nothrow_assignable<int&, int>::value;
    CHECK(ans);
    ans = STD is_nothrow_assignable<int&, B>::value;
    CHECK(!ans);
    ans = STD is_nothrow_assignable<void, void>::value;
    CHECK(!ans);
}

static void t_is_nothrow_copy_assignable() { // test is_nothrow_copy_assignable<T> for various types
                                             // (minimal tests in absence of compiler support)
    T_TRIVIAL_V(is_nothrow_copy_assignable, void, false);
    T_TRIVIAL_V(is_nothrow_copy_assignable, char, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, signed char, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, unsigned char, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, wchar_t, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, short, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, unsigned short, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, int, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, unsigned int, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, long, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, unsigned long, true);

    T_TRIVIAL_V(is_nothrow_copy_assignable, long long, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, unsigned long long, true);

    T_TRIVIAL_V(is_nothrow_copy_assignable, float, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, double, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, long double, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, int*, true);
    T_TRIVIAL(is_nothrow_copy_assignable, int&, true);
    T_TRIVIAL(is_nothrow_copy_assignable, B, true);
    T_TRIVIAL(is_nothrow_copy_assignable, C, true);
    T_TRIVIAL(is_nothrow_copy_assignable, D, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, pmo, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, pmoc, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, pmov, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, pmocv, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, pmf, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, pmfc, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, pmfv, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, pmfcv, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, arr_t, false);
    T_TRIVIAL_V(is_nothrow_copy_assignable, enum_t, true);
    T_TRIVIAL_V(is_nothrow_copy_assignable, pf, true);
    T_TRIVIAL(is_nothrow_copy_assignable, func, false);
    T_TRIVIAL_V(is_nothrow_copy_assignable, void*, true);
}

static void t_is_nothrow_move_assignable() { // test is_nothrow_move_assignable<T> for various types
                                             // (minimal tests in absence of compiler support)
    T_TRIVIAL_V(is_nothrow_move_assignable, void, false);
    T_TRIVIAL_V(is_nothrow_move_assignable, char, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, signed char, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, unsigned char, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, wchar_t, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, short, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, unsigned short, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, int, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, unsigned int, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, long, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, unsigned long, true);

    T_TRIVIAL_V(is_nothrow_move_assignable, long long, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, unsigned long long, true);

    T_TRIVIAL_V(is_nothrow_move_assignable, float, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, double, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, long double, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, int*, true);
    T_TRIVIAL(is_nothrow_move_assignable, int&, true);
    T_TRIVIAL(is_nothrow_move_assignable, B, true);
    T_TRIVIAL(is_nothrow_move_assignable, C, true);
    T_TRIVIAL(is_nothrow_move_assignable, D, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, pmo, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, pmoc, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, pmov, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, pmocv, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, pmf, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, pmfc, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, pmfv, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, pmfcv, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, arr_t, false);
    T_TRIVIAL_V(is_nothrow_move_assignable, enum_t, true);
    T_TRIVIAL_V(is_nothrow_move_assignable, pf, true);
    T_TRIVIAL(is_nothrow_move_assignable, func, false);
    T_TRIVIAL_V(is_nothrow_move_assignable, void*, true);
}

static void t_is_nothrow_destructible() { // test is_nothrow_destructible<T> for various types
                                          // (minimal tests in absence of compiler support)
    T_TRIVIAL_CV(is_nothrow_destructible, void, false);
    T_TRIVIAL_CV(is_nothrow_destructible, char, true);
    T_TRIVIAL_CV(is_nothrow_destructible, signed char, true);
    T_TRIVIAL_CV(is_nothrow_destructible, unsigned char, true);
    T_TRIVIAL_CV(is_nothrow_destructible, wchar_t, true);
    T_TRIVIAL_CV(is_nothrow_destructible, short, true);
    T_TRIVIAL_CV(is_nothrow_destructible, unsigned short, true);
    T_TRIVIAL_CV(is_nothrow_destructible, int, true);
    T_TRIVIAL_CV(is_nothrow_destructible, unsigned int, true);
    T_TRIVIAL_CV(is_nothrow_destructible, long, true);
    T_TRIVIAL_CV(is_nothrow_destructible, unsigned long, true);

    T_TRIVIAL_CV(is_nothrow_destructible, long long, true);
    T_TRIVIAL_CV(is_nothrow_destructible, unsigned long long, true);

    T_TRIVIAL_CV(is_nothrow_destructible, float, true);
    T_TRIVIAL_CV(is_nothrow_destructible, double, true);
    T_TRIVIAL_CV(is_nothrow_destructible, long double, true);
    T_TRIVIAL_CV(is_nothrow_destructible, int*, true);
    T_TRIVIAL(is_nothrow_destructible, int&, true);
    T_TRIVIAL_CV(is_nothrow_destructible, B, true);
    T_TRIVIAL_CV(is_nothrow_destructible, C, true);
    T_TRIVIAL_CV(is_nothrow_destructible, D, true);
    T_TRIVIAL_CV(is_nothrow_destructible, pmo, true);
    T_TRIVIAL_CV(is_nothrow_destructible, pmoc, true);
    T_TRIVIAL_CV(is_nothrow_destructible, pmov, true);
    T_TRIVIAL_CV(is_nothrow_destructible, pmocv, true);
    T_TRIVIAL_CV(is_nothrow_destructible, pmf, true);
    T_TRIVIAL_CV(is_nothrow_destructible, pmfc, true);
    T_TRIVIAL_CV(is_nothrow_destructible, pmfv, true);
    T_TRIVIAL_CV(is_nothrow_destructible, pmfcv, true);
    T_TRIVIAL_CV(is_nothrow_destructible, arr_t, true);
    T_TRIVIAL_CV(is_nothrow_destructible, enum_t, true);
    T_TRIVIAL_CV(is_nothrow_destructible, pf, true);
    T_TRIVIAL(is_nothrow_destructible, func, false);
    T_TRIVIAL_CV(is_nothrow_destructible, void*, true);
}

static void t_has_virtual_destructor() { // test has_virtual_destructor<T>
    T_TEST_CV(has_virtual_destructor, void, false);
    T_TEST_CV(has_virtual_destructor, char, false);
    T_TEST_CV(has_virtual_destructor, signed char, false);
    T_TEST_CV(has_virtual_destructor, unsigned char, false);
    T_TEST_CV(has_virtual_destructor, wchar_t, false);
    T_TEST_CV(has_virtual_destructor, short, false);
    T_TEST_CV(has_virtual_destructor, unsigned short, false);
    T_TEST_CV(has_virtual_destructor, int, false);
    T_TEST_CV(has_virtual_destructor, unsigned int, false);
    T_TEST_CV(has_virtual_destructor, long, false);
    T_TEST_CV(has_virtual_destructor, unsigned long, false);

    T_TEST_CV(has_virtual_destructor, long long, false);
    T_TEST_CV(has_virtual_destructor, unsigned long long, false);

    T_TEST_CV(has_virtual_destructor, float, false);
    T_TEST_CV(has_virtual_destructor, double, false);
    T_TEST_CV(has_virtual_destructor, long double, false);
    T_TEST_CV(has_virtual_destructor, int*, false);
    T_TEST(has_virtual_destructor, int&, false);

    T_TEST_CV(has_virtual_destructor, B, true);
    T_TEST_CV(has_virtual_destructor, C, true);

    T_TEST_CV(has_virtual_destructor, D, false);
    T_TEST_CV(has_virtual_destructor, pmo, false);
    T_TEST_CV(has_virtual_destructor, pmoc, false);
    T_TEST_CV(has_virtual_destructor, pmov, false);
    T_TEST_CV(has_virtual_destructor, pmocv, false);
    T_TEST_CV(has_virtual_destructor, pmf, false);
    T_TEST_CV(has_virtual_destructor, pmfc, false);
    T_TEST_CV(has_virtual_destructor, pmfv, false);
    T_TEST_CV(has_virtual_destructor, pmfcv, false);
    T_TEST_CV(has_virtual_destructor, arr_t, false);
    T_TEST_CV(has_virtual_destructor, enum_t, false);
    T_TEST_CV(has_virtual_destructor, pf, false);
    T_TEST(has_virtual_destructor, func, false);
    T_TEST_CV(has_virtual_destructor, void*, false);
}

void test_main() { // test type traits
    t_is_nothrow_constructible();
    t_is_nothrow_default_constructible();
    t_is_nothrow_copy_constructible();
    t_is_nothrow_move_constructible();

    t_is_nothrow_assignable();
    t_is_nothrow_copy_assignable();
    t_is_nothrow_move_assignable();

    t_is_nothrow_destructible();

    t_has_virtual_destructor();
}
