// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <type_traits> header, part 7
#define TEST_NAME "<type_traits>, part 7"

#define _HAS_DEPRECATED_IS_LITERAL_TYPE 1
#define _SILENCE_CXX17_IS_LITERAL_TYPE_DEPRECATION_WARNING

#include "tdefs.h"
#include "typetr.h"
#include <type_traits>

// TESTS
static void t_is_constructible() { // test is_constructible<T, Args...> for various types
    T_UNSPEC_CV(is_constructible, void, false);
    T_UNSPEC_CV(is_constructible, char, true);
    T_UNSPEC_CV(is_constructible, signed char, true);
    T_UNSPEC_CV(is_constructible, unsigned char, true);
    T_UNSPEC_CV(is_constructible, wchar_t, true);
    T_UNSPEC_CV(is_constructible, short, true);
    T_UNSPEC_CV(is_constructible, unsigned short, true);
    T_UNSPEC_CV(is_constructible, int, true);
    T_UNSPEC_CV(is_constructible, unsigned int, true);
    T_UNSPEC_CV(is_constructible, long, true);
    T_UNSPEC_CV(is_constructible, unsigned long, true);

    T_UNSPEC_CV(is_constructible, long long, true);
    T_UNSPEC_CV(is_constructible, unsigned long long, true);

    T_UNSPEC_CV(is_constructible, float, true);
    T_UNSPEC_CV(is_constructible, double, true);
    T_UNSPEC_CV(is_constructible, long double, true);
    T_UNSPEC_CV(is_constructible, int*, true);
    T_UNSPEC(is_constructible, int&, false);
    T_UNSPEC_CV(is_constructible, B, true);
    T_UNSPEC_CV(is_constructible, C, true);
    T_UNSPEC_CV(is_constructible, D, true);
    T_UNSPEC_CV(is_constructible, pmo, true);
    T_UNSPEC_CV(is_constructible, pmoc, true);
    T_UNSPEC_CV(is_constructible, pmov, true);
    T_UNSPEC_CV(is_constructible, pmocv, true);
    T_UNSPEC_CV(is_constructible, pmf, true);
    T_UNSPEC_CV(is_constructible, pmfc, true);
    T_UNSPEC_CV(is_constructible, pmfv, true);
    T_UNSPEC_CV(is_constructible, pmfcv, true);
    T_UNSPEC_CV(is_constructible, arr_t, true);
    T_UNSPEC_CV(is_constructible, enum_t, true);
    T_UNSPEC_CV(is_constructible, pf, true);
    T_UNSPEC(is_constructible, func, false);
    T_UNSPEC_CV(is_constructible, void*, true);

    bool ans = STD is_constructible<int>::value;
    CHECK(ans);
    ans = STD is_constructible<int, int>::value;
    CHECK(ans);
    ans = STD is_constructible<int, int, int>::value;
    CHECK(!ans);
    ans = STD is_constructible<int, int, int, int>::value;
    CHECK(!ans);
    ans = STD is_constructible<int, int, int, int, int>::value;
    CHECK(!ans);
}

static void t_is_default_constructible() { // test is_default_constructible<T> for various types
                                           // (minimal tests in absence of compiler support)
    T_TRIVIAL_CV(is_default_constructible, void, false);
    T_TRIVIAL_CV(is_default_constructible, char, true);
    T_TRIVIAL_CV(is_default_constructible, signed char, true);
    T_TRIVIAL_CV(is_default_constructible, unsigned char, true);
    T_TRIVIAL_CV(is_default_constructible, wchar_t, true);
    T_TRIVIAL_CV(is_default_constructible, short, true);
    T_TRIVIAL_CV(is_default_constructible, unsigned short, true);
    T_TRIVIAL_CV(is_default_constructible, int, true);
    T_TRIVIAL_CV(is_default_constructible, unsigned int, true);
    T_TRIVIAL_CV(is_default_constructible, long, true);
    T_TRIVIAL_CV(is_default_constructible, unsigned long, true);

    T_TRIVIAL_CV(is_default_constructible, long long, true);
    T_TRIVIAL_CV(is_default_constructible, unsigned long long, true);

    T_TRIVIAL_CV(is_default_constructible, float, true);
    T_TRIVIAL_CV(is_default_constructible, double, true);
    T_TRIVIAL_CV(is_default_constructible, long double, true);
    T_TRIVIAL_CV(is_default_constructible, int*, true);
    T_TRIVIAL(is_default_constructible, int&, false);
    T_TRIVIAL_CV(is_default_constructible, B, true);
    T_TRIVIAL_CV(is_default_constructible, C, true);
    T_TRIVIAL_CV(is_default_constructible, D, true);
    T_TRIVIAL_CV(is_default_constructible, pmo, true);
    T_TRIVIAL_CV(is_default_constructible, pmoc, true);
    T_TRIVIAL_CV(is_default_constructible, pmov, true);
    T_TRIVIAL_CV(is_default_constructible, pmocv, true);
    T_TRIVIAL_CV(is_default_constructible, pmf, true);
    T_TRIVIAL_CV(is_default_constructible, pmfc, true);
    T_TRIVIAL_CV(is_default_constructible, pmfv, true);
    T_TRIVIAL_CV(is_default_constructible, pmfcv, true);
    T_TRIVIAL_CV(is_default_constructible, arr_t, true);
    T_TRIVIAL_CV(is_default_constructible, enum_t, true);
    T_TRIVIAL_CV(is_default_constructible, pf, true);
    T_TRIVIAL(is_default_constructible, func, false);
    T_TRIVIAL_CV(is_default_constructible, void*, true);
}

static void t_is_copy_constructible() { // test is_copy_constructible<T> for various types
    T_UNSPEC_CV(is_copy_constructible, void, false);
    T_UNSPEC_CV(is_copy_constructible, char, true);
    T_UNSPEC_CV(is_copy_constructible, signed char, true);
    T_UNSPEC_CV(is_copy_constructible, unsigned char, true);
    T_UNSPEC_CV(is_copy_constructible, wchar_t, true);
    T_UNSPEC_CV(is_copy_constructible, short, true);
    T_UNSPEC_CV(is_copy_constructible, unsigned short, true);
    T_UNSPEC_CV(is_copy_constructible, int, true);
    T_UNSPEC_CV(is_copy_constructible, unsigned int, true);
    T_UNSPEC_CV(is_copy_constructible, long, true);
    T_UNSPEC_CV(is_copy_constructible, unsigned long, true);

    T_UNSPEC_CV(is_copy_constructible, long long, true);
    T_UNSPEC_CV(is_copy_constructible, unsigned long long, true);

    T_UNSPEC_CV(is_copy_constructible, float, true);
    T_UNSPEC_CV(is_copy_constructible, double, true);
    T_UNSPEC_CV(is_copy_constructible, long double, true);
    T_UNSPEC_CV(is_copy_constructible, int*, true);
    T_UNSPEC(is_copy_constructible, int&, true);
    T_UNSPEC_C(is_copy_constructible, B, true);
    T_UNSPEC_C(is_copy_constructible, C, true);
    T_UNSPEC_C(is_copy_constructible, D, true);
    T_UNSPEC_CV(is_copy_constructible, pmo, true);
    T_UNSPEC_CV(is_copy_constructible, pmoc, true);
    T_UNSPEC_CV(is_copy_constructible, pmov, true);
    T_UNSPEC_CV(is_copy_constructible, pmocv, true);
    T_UNSPEC_CV(is_copy_constructible, pmf, true);
    T_UNSPEC_CV(is_copy_constructible, pmfc, true);
    T_UNSPEC_CV(is_copy_constructible, pmfv, true);
    T_UNSPEC_CV(is_copy_constructible, pmfcv, true);
    T_UNSPEC_CV(is_copy_constructible, arr_t, false);
    T_UNSPEC_CV(is_copy_constructible, enum_t, true);
    T_UNSPEC_CV(is_copy_constructible, pf, true);
    T_UNSPEC(is_copy_constructible, func, false);
    T_UNSPEC_CV(is_copy_constructible, void*, true);
}

static void t_is_move_constructible() { // test is_move_constructible<T> for various types
                                        // (minimal tests in absence of compiler support)
    T_TRIVIAL_CV(is_move_constructible, void, false);
    T_TRIVIAL_CV(is_move_constructible, char, true);
    T_TRIVIAL_CV(is_move_constructible, signed char, true);
    T_TRIVIAL_CV(is_move_constructible, unsigned char, true);
    T_TRIVIAL_CV(is_move_constructible, wchar_t, true);
    T_TRIVIAL_CV(is_move_constructible, short, true);
    T_TRIVIAL_CV(is_move_constructible, unsigned short, true);
    T_TRIVIAL_CV(is_move_constructible, int, true);
    T_TRIVIAL_CV(is_move_constructible, unsigned int, true);
    T_TRIVIAL_CV(is_move_constructible, long, true);
    T_TRIVIAL_CV(is_move_constructible, unsigned long, true);

    T_TRIVIAL_CV(is_move_constructible, long long, true);
    T_TRIVIAL_CV(is_move_constructible, unsigned long long, true);

    T_TRIVIAL_CV(is_move_constructible, float, true);
    T_TRIVIAL_CV(is_move_constructible, double, true);
    T_TRIVIAL_CV(is_move_constructible, long double, true);
    T_TRIVIAL_CV(is_move_constructible, int*, true);
    T_TRIVIAL(is_move_constructible, int&, true);
    T_TRIVIAL_C(is_move_constructible, B, true);
    T_TRIVIAL_C(is_move_constructible, C, true);
    T_TRIVIAL_C(is_move_constructible, D, true);
    T_TRIVIAL_CV(is_move_constructible, pmo, true);
    T_TRIVIAL_CV(is_move_constructible, pmoc, true);
    T_TRIVIAL_CV(is_move_constructible, pmov, true);
    T_TRIVIAL_CV(is_move_constructible, pmocv, true);
    T_TRIVIAL_CV(is_move_constructible, pmf, true);
    T_TRIVIAL_CV(is_move_constructible, pmfc, true);
    T_TRIVIAL_CV(is_move_constructible, pmfv, true);
    T_TRIVIAL_CV(is_move_constructible, pmfcv, true);
    T_TRIVIAL_CV(is_move_constructible, arr_t, false);
    T_TRIVIAL_CV(is_move_constructible, enum_t, true);
    T_TRIVIAL_CV(is_move_constructible, pf, true);
    T_TRIVIAL(is_move_constructible, func, false);
    T_TRIVIAL_CV(is_move_constructible, void*, true);
}

static void t_is_assignable() { // test is_assignable<T> for various types
                                // (minimal tests in absence of compiler support)
    bool ans = STD is_assignable<B&, B>::value;
    CHECK(ans);
    ans = STD is_assignable<int&, int>::value;
    CHECK(ans);
    ans = STD is_assignable<int&, B>::value;
    CHECK(!ans);
    ans = STD is_assignable<void, void>::value;
    CHECK(!ans);
}

static void t_is_copy_assignable() { // test is_copy_assignable<T> for various types
                                     // (minimal tests in absence of compiler support)
    T_TRIVIAL_V(is_copy_assignable, void, false);
    T_TRIVIAL_V(is_copy_assignable, char, true);
    T_TRIVIAL_V(is_copy_assignable, signed char, true);
    T_TRIVIAL_V(is_copy_assignable, unsigned char, true);
    T_TRIVIAL_V(is_copy_assignable, wchar_t, true);
    T_TRIVIAL_V(is_copy_assignable, short, true);
    T_TRIVIAL_V(is_copy_assignable, unsigned short, true);
    T_TRIVIAL_V(is_copy_assignable, int, true);
    T_TRIVIAL_V(is_copy_assignable, unsigned int, true);
    T_TRIVIAL_V(is_copy_assignable, long, true);
    T_TRIVIAL_V(is_copy_assignable, unsigned long, true);

    T_TRIVIAL_V(is_copy_assignable, long long, true);
    T_TRIVIAL_V(is_copy_assignable, unsigned long long, true);

    T_TRIVIAL_V(is_copy_assignable, float, true);
    T_TRIVIAL_V(is_copy_assignable, double, true);
    T_TRIVIAL_V(is_copy_assignable, long double, true);
    T_TRIVIAL_V(is_copy_assignable, int*, true);
    T_TRIVIAL(is_copy_assignable, int&, true);
    T_TRIVIAL(is_copy_assignable, B, true);
    T_TRIVIAL(is_copy_assignable, C, true);
    T_TRIVIAL(is_copy_assignable, D, true);
    T_TRIVIAL_V(is_copy_assignable, pmo, true);
    T_TRIVIAL_V(is_copy_assignable, pmoc, true);
    T_TRIVIAL_V(is_copy_assignable, pmov, true);
    T_TRIVIAL_V(is_copy_assignable, pmocv, true);
    T_TRIVIAL_V(is_copy_assignable, pmf, true);
    T_TRIVIAL_V(is_copy_assignable, pmfc, true);
    T_TRIVIAL_V(is_copy_assignable, pmfv, true);
    T_TRIVIAL_V(is_copy_assignable, pmfcv, true);
    T_TRIVIAL_V(is_copy_assignable, arr_t, false);
    T_TRIVIAL_V(is_copy_assignable, enum_t, true);
    T_TRIVIAL_V(is_copy_assignable, pf, true);
    T_TRIVIAL(is_copy_assignable, func, false);
    T_TRIVIAL_V(is_copy_assignable, void*, true);
}

static void t_is_move_assignable() { // test is_move_assignable<T> for various types
                                     // (minimal tests in absence of compiler support)
    T_TRIVIAL_V(is_move_assignable, void, false);
    T_TRIVIAL_V(is_move_assignable, char, true);
    T_TRIVIAL_V(is_move_assignable, signed char, true);
    T_TRIVIAL_V(is_move_assignable, unsigned char, true);
    T_TRIVIAL_V(is_move_assignable, wchar_t, true);
    T_TRIVIAL_V(is_move_assignable, short, true);
    T_TRIVIAL_V(is_move_assignable, unsigned short, true);
    T_TRIVIAL_V(is_move_assignable, int, true);
    T_TRIVIAL_V(is_move_assignable, unsigned int, true);
    T_TRIVIAL_V(is_move_assignable, long, true);
    T_TRIVIAL_V(is_move_assignable, unsigned long, true);

    T_TRIVIAL_V(is_move_assignable, long long, true);
    T_TRIVIAL_V(is_move_assignable, unsigned long long, true);

    T_TRIVIAL_V(is_move_assignable, float, true);
    T_TRIVIAL_V(is_move_assignable, double, true);
    T_TRIVIAL_V(is_move_assignable, long double, true);
    T_TRIVIAL_V(is_move_assignable, int*, true);
    T_TRIVIAL(is_move_assignable, int&, true);
    T_TRIVIAL(is_move_assignable, B, true);
    T_TRIVIAL(is_move_assignable, C, true);
    T_TRIVIAL(is_move_assignable, D, true);
    T_TRIVIAL_V(is_move_assignable, pmo, true);
    T_TRIVIAL_V(is_move_assignable, pmoc, true);
    T_TRIVIAL_V(is_move_assignable, pmov, true);
    T_TRIVIAL_V(is_move_assignable, pmocv, true);
    T_TRIVIAL_V(is_move_assignable, pmf, true);
    T_TRIVIAL_V(is_move_assignable, pmfc, true);
    T_TRIVIAL_V(is_move_assignable, pmfv, true);
    T_TRIVIAL_V(is_move_assignable, pmfcv, true);
    T_TRIVIAL_V(is_move_assignable, arr_t, false);
    T_TRIVIAL_V(is_move_assignable, enum_t, true);
    T_TRIVIAL_V(is_move_assignable, pf, true);
    T_TRIVIAL(is_move_assignable, func, false);
    T_TRIVIAL_V(is_move_assignable, void*, true);
}

static void t_is_destructible() { // test is_destructible<T> for various types
                                  // (minimal tests in absence of compiler support)
    T_TRIVIAL_CV(is_destructible, void, false);
    T_TRIVIAL_CV(is_destructible, char, true);
    T_TRIVIAL_CV(is_destructible, signed char, true);
    T_TRIVIAL_CV(is_destructible, unsigned char, true);
    T_TRIVIAL_CV(is_destructible, wchar_t, true);
    T_TRIVIAL_CV(is_destructible, short, true);
    T_TRIVIAL_CV(is_destructible, unsigned short, true);
    T_TRIVIAL_CV(is_destructible, int, true);
    T_TRIVIAL_CV(is_destructible, unsigned int, true);
    T_TRIVIAL_CV(is_destructible, long, true);
    T_TRIVIAL_CV(is_destructible, unsigned long, true);

    T_TRIVIAL_CV(is_destructible, long long, true);
    T_TRIVIAL_CV(is_destructible, unsigned long long, true);

    T_TRIVIAL_CV(is_destructible, float, true);
    T_TRIVIAL_CV(is_destructible, double, true);
    T_TRIVIAL_CV(is_destructible, long double, true);
    T_TRIVIAL_CV(is_destructible, int*, true);
    T_TRIVIAL(is_destructible, int&, true);
    T_TRIVIAL_CV(is_destructible, B, true);
    T_TRIVIAL_CV(is_destructible, C, true);
    T_TRIVIAL_CV(is_destructible, D, true);
    T_TRIVIAL_CV(is_destructible, pmo, true);
    T_TRIVIAL_CV(is_destructible, pmoc, true);
    T_TRIVIAL_CV(is_destructible, pmov, true);
    T_TRIVIAL_CV(is_destructible, pmocv, true);
    T_TRIVIAL_CV(is_destructible, pmf, true);
    T_TRIVIAL_CV(is_destructible, pmfc, true);
    T_TRIVIAL_CV(is_destructible, pmfv, true);
    T_TRIVIAL_CV(is_destructible, pmfcv, true);
    T_TRIVIAL_CV(is_destructible, arr_t, true);
    T_TRIVIAL_CV(is_destructible, enum_t, true);
    T_TRIVIAL_CV(is_destructible, pf, true);
    T_TRIVIAL(is_destructible, func, false);
    T_TRIVIAL_CV(is_destructible, void*, true);
}

static void t_enable_if() { // test enable_if<Test, Ty> for various types
    typedef STD enable_if<false, void> ty1;
    CHECK_TYPE(STD enable_if<false>, ty1);
    CHECK_TYPE(STD enable_if<true>::type, void);

    typedef STD enable_if<true, double> ty2;
    CHECK_TYPE(ty2::type, double);
}

static void t_is_trivial() { // test is_trivial<T> for various types
    T_UNSPEC_CV(is_trivial, void, false);
    T_UNSPEC_CV(is_trivial, char, true);
    T_UNSPEC_CV(is_trivial, signed char, true);
    T_UNSPEC_CV(is_trivial, unsigned char, true);
    T_UNSPEC_CV(is_trivial, wchar_t, true);
    T_UNSPEC_CV(is_trivial, short, true);
    T_UNSPEC_CV(is_trivial, unsigned short, true);
    T_UNSPEC_CV(is_trivial, int, true);
    T_UNSPEC_CV(is_trivial, unsigned int, true);
    T_UNSPEC_CV(is_trivial, long, true);
    T_UNSPEC_CV(is_trivial, unsigned long, true);

    T_UNSPEC_CV(is_trivial, long long, true);
    T_UNSPEC_CV(is_trivial, unsigned long long, true);

    T_UNSPEC_CV(is_trivial, float, true);
    T_UNSPEC_CV(is_trivial, double, true);
    T_UNSPEC_CV(is_trivial, long double, true);
    T_UNSPEC_CV(is_trivial, int*, true);
    T_UNSPEC(is_trivial, int&, false);
    T_UNSPEC_CV(is_trivial, B, false);
    T_UNSPEC_CV(is_trivial, C, false);
    T_UNSPEC_CV(is_trivial, D, true);
    T_UNSPEC_CV(is_trivial, pmo, true);
    T_UNSPEC_CV(is_trivial, pmoc, true);
    T_UNSPEC_CV(is_trivial, pmov, true);
    T_UNSPEC_CV(is_trivial, pmocv, true);
    T_UNSPEC_CV(is_trivial, pmf, true);
    T_UNSPEC_CV(is_trivial, pmfc, true);
    T_UNSPEC_CV(is_trivial, pmfv, true);
    T_UNSPEC_CV(is_trivial, pmfcv, true);
    T_UNSPEC_CV(is_trivial, arr_t, true);
    T_UNSPEC_CV(is_trivial, enum_t, true);
    T_UNSPEC_CV(is_trivial, pf, true);
    T_UNSPEC(is_trivial, func, false);
    T_UNSPEC_CV(is_trivial, void*, true);
}

static void t_is_literal_type() { // test is_literal_type<T> for various types
    T_UNSPEC_CV(is_literal_type, void, true);
    T_UNSPEC_CV(is_literal_type, char, true);
    T_UNSPEC_CV(is_literal_type, signed char, true);
    T_UNSPEC_CV(is_literal_type, unsigned char, true);
    T_UNSPEC_CV(is_literal_type, wchar_t, true);
    T_UNSPEC_CV(is_literal_type, short, true);
    T_UNSPEC_CV(is_literal_type, unsigned short, true);
    T_UNSPEC_CV(is_literal_type, int, true);
    T_UNSPEC_CV(is_literal_type, unsigned int, true);
    T_UNSPEC_CV(is_literal_type, long, true);
    T_UNSPEC_CV(is_literal_type, unsigned long, true);

    T_UNSPEC_CV(is_literal_type, long long, true);
    T_UNSPEC_CV(is_literal_type, unsigned long long, true);

    T_UNSPEC_CV(is_literal_type, float, true);
    T_UNSPEC_CV(is_literal_type, double, true);
    T_UNSPEC_CV(is_literal_type, long double, true);
    T_UNSPEC_CV(is_literal_type, int*, true);
    T_UNSPEC(is_literal_type, int&, true);
    T_UNSPEC_CV(is_literal_type, B, false);
    T_UNSPEC_CV(is_literal_type, C, false);
    T_UNSPEC_CV(is_literal_type, D, true);
    T_UNSPEC_CV(is_literal_type, pmo, true);
    T_UNSPEC_CV(is_literal_type, pmoc, true);
    T_UNSPEC_CV(is_literal_type, pmov, true);
    T_UNSPEC_CV(is_literal_type, pmocv, true);
    T_UNSPEC_CV(is_literal_type, pmf, true);
    T_UNSPEC_CV(is_literal_type, pmfc, true);
    T_UNSPEC_CV(is_literal_type, pmfv, true);
    T_UNSPEC_CV(is_literal_type, pmfcv, true);
    T_UNSPEC_CV(is_literal_type, arr_t, true);
    T_UNSPEC_CV(is_literal_type, enum_t, true);
    T_UNSPEC_CV(is_literal_type, pf, true);
    T_UNSPEC(is_literal_type, func, false);
    T_UNSPEC_CV(is_literal_type, void*, true);
}

static void t_is_same() { // test is_same<T1, T2> for various types
    bool ans = STD is_same<int, void>::value;
    CHECK(!ans);
    ans = STD is_same<int, int>::value;
    CHECK(ans);
}

static void t_is_base_of() { // test is_base_of<Base, Derived> for various types
    bool ans = STD is_base_of<int, int>::value;
    CHECK(!ans);
    ans = STD is_base_of<B, C>::value;
    CHECK(ans);
}

static void t_is_convertible() { // test is_convertible<From, To> for various types
    bool ans = STD is_convertible<void, int>::value;
    CHECK(!ans);
    ans = STD is_convertible<int, int>::value;
    CHECK(ans);
}

static void t_void_t() { // test void_t
    CHECK_TYPE(STD void_t<int>, void);
    typedef STD void_t<int, float, double> void3;
    CHECK_TYPE(void3, void);
}

void test_main() { // test type traits
    t_is_constructible();
    t_is_default_constructible();
    t_is_copy_constructible();
    t_is_move_constructible();

    t_is_assignable();
    t_is_copy_assignable();
    t_is_move_assignable();

    t_is_destructible();

    t_enable_if();
    t_is_trivial();
    t_is_literal_type();

    t_is_same();
    t_is_base_of();
    t_is_convertible();

    t_void_t();
}
