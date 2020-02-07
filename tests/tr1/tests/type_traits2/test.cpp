// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <type_traits> header, part 2
#define TEST_NAME "<type_traits>, part 2"

#include "tdefs.h"
#include "typetr.h"
#include <type_traits>

// TESTS
static void t_is_enum() { // test is_enum<T> for various types
    T_TEST_CV(is_enum, void, false);
    T_TEST_CV(is_enum, char, false);
    T_TEST_CV(is_enum, signed char, false);
    T_TEST_CV(is_enum, unsigned char, false);
    T_TEST_CV(is_enum, wchar_t, false);
    T_TEST_CV(is_enum, short, false);
    T_TEST_CV(is_enum, unsigned short, false);
    T_TEST_CV(is_enum, int, false);
    T_TEST_CV(is_enum, unsigned int, false);
    T_TEST_CV(is_enum, long, false);
    T_TEST_CV(is_enum, unsigned long, false);

    T_TEST_CV(is_enum, long long, false);
    T_TEST_CV(is_enum, unsigned long long, false);

    T_TEST_CV(is_enum, float, false);
    T_TEST_CV(is_enum, double, false);
    T_TEST_CV(is_enum, long double, false);
    T_TEST_CV(is_enum, int*, false);
    T_TEST(is_enum, int&, false);

    T_TEST_CV(is_enum, A, false); // unspecified whether A is instantiated

    T_TEST_CV(is_enum, B, false);
    T_TEST_CV(is_enum, C, false);
    T_TEST_CV(is_enum, D, false);
    T_TEST_CV(is_enum, U, false);
    T_TEST_CV(is_enum, pmo, false);
    T_TEST_CV(is_enum, pmoc, false);
    T_TEST_CV(is_enum, pmov, false);
    T_TEST_CV(is_enum, pmocv, false);
    T_TEST_CV(is_enum, pmf, false);
    T_TEST_CV(is_enum, pmfc, false);
    T_TEST_CV(is_enum, pmfv, false);
    T_TEST_CV(is_enum, pmfcv, false);
    T_TEST_CV(is_enum, arr_t, false);
    T_TEST_CV(is_enum, enum_t, true);
    T_TEST_CV(is_enum, pf, false);
    T_TEST(is_enum, func, false);
    T_TEST_CV(is_enum, void*, false);
}

static void t_is_union() { // test is_union<T> for various types
    T_NOBASE_CV(is_union, void, false);
    T_NOBASE_CV(is_union, char, false);
    T_NOBASE_CV(is_union, signed char, false);
    T_NOBASE_CV(is_union, unsigned char, false);
    T_NOBASE_CV(is_union, wchar_t, false);
    T_NOBASE_CV(is_union, short, false);
    T_NOBASE_CV(is_union, unsigned short, false);
    T_NOBASE_CV(is_union, int, false);
    T_NOBASE_CV(is_union, unsigned int, false);
    T_NOBASE_CV(is_union, long, false);
    T_NOBASE_CV(is_union, unsigned long, false);

    T_NOBASE_CV(is_union, long long, false);
    T_NOBASE_CV(is_union, unsigned long long, false);

    T_NOBASE_CV(is_union, float, false);
    T_NOBASE_CV(is_union, double, false);
    T_NOBASE_CV(is_union, long double, false);
    T_NOBASE_CV(is_union, int*, false);
    T_NOBASE(is_union, int&, false);
    T_NOBASE_CV(is_union, A, false);
    T_NOBASE_CV(is_union, B, false);
    T_NOBASE_CV(is_union, C, false);
    T_NOBASE_CV(is_union, D, false);
    T_NOBASE_CV(is_union, U, true);
    T_NOBASE_CV(is_union, pmo, false);
    T_NOBASE_CV(is_union, pmoc, false);
    T_NOBASE_CV(is_union, pmov, false);
    T_NOBASE_CV(is_union, pmocv, false);
    T_NOBASE_CV(is_union, pmf, false);
    T_NOBASE_CV(is_union, pmfc, false);
    T_NOBASE_CV(is_union, pmfv, false);
    T_NOBASE_CV(is_union, pmfcv, false);
    T_NOBASE_CV(is_union, arr_t, false);
    T_NOBASE_CV(is_union, enum_t, false);
    T_NOBASE_CV(is_union, pf, false);
    T_NOBASE(is_union, func, false);
    T_NOBASE_CV(is_union, void*, false);
}

static void t_is_class() { // test is_class<T> for various types
    T_NOBASE_CV(is_class, void, false);
    T_NOBASE_CV(is_class, char, false);
    T_NOBASE_CV(is_class, signed char, false);
    T_NOBASE_CV(is_class, unsigned char, false);
    T_NOBASE_CV(is_class, wchar_t, false);
    T_NOBASE_CV(is_class, short, false);
    T_NOBASE_CV(is_class, unsigned short, false);
    T_NOBASE_CV(is_class, int, false);
    T_NOBASE_CV(is_class, unsigned int, false);
    T_NOBASE_CV(is_class, long, false);
    T_NOBASE_CV(is_class, unsigned long, false);

    T_NOBASE_CV(is_class, long long, false);
    T_NOBASE_CV(is_class, unsigned long long, false);

    T_NOBASE_CV(is_class, float, false);
    T_NOBASE_CV(is_class, double, false);
    T_NOBASE_CV(is_class, long double, false);
    T_NOBASE_CV(is_class, int*, false);
    T_NOBASE(is_class, int&, false);
    T_NOBASE_CV(is_class, A, true);
    T_NOBASE_CV(is_class, B, true);
    T_NOBASE_CV(is_class, C, true);
    T_NOBASE_CV(is_class, D, true);
    T_NOBASE_CV(is_class, U, false);
    T_NOBASE_CV(is_class, pmo, false);
    T_NOBASE_CV(is_class, pmoc, false);
    T_NOBASE_CV(is_class, pmov, false);
    T_NOBASE_CV(is_class, pmocv, false);
    T_NOBASE_CV(is_class, pmf, false);
    T_NOBASE_CV(is_class, pmfc, false);
    T_NOBASE_CV(is_class, pmfv, false);
    T_NOBASE_CV(is_class, pmfcv, false);
    T_NOBASE_CV(is_class, arr_t, false);
    T_NOBASE_CV(is_class, enum_t, false);
    T_NOBASE_CV(is_class, pf, false);
    T_NOBASE(is_class, func, false);
    T_NOBASE_CV(is_class, void*, false);
}

static void t_is_function() { // test is_function<T> for various types
    T_TEST_CV(is_function, void, false);
    T_TEST_CV(is_function, char, false);
    T_TEST_CV(is_function, signed char, false);
    T_TEST_CV(is_function, unsigned char, false);
    T_TEST_CV(is_function, wchar_t, false);
    T_TEST_CV(is_function, short, false);
    T_TEST_CV(is_function, unsigned short, false);
    T_TEST_CV(is_function, int, false);
    T_TEST_CV(is_function, unsigned int, false);
    T_TEST_CV(is_function, long, false);
    T_TEST_CV(is_function, unsigned long, false);

    T_TEST_CV(is_function, long long, false);
    T_TEST_CV(is_function, unsigned long long, false);

    T_TEST_CV(is_function, float, false);
    T_TEST_CV(is_function, double, false);
    T_TEST_CV(is_function, long double, false);
    T_TEST_CV(is_function, int*, false);
    T_TEST(is_function, int&, false);
    T_TEST_CV(is_function, A, false);
    T_TEST_CV(is_function, B, false);
    T_TEST_CV(is_function, C, false);
    T_TEST_CV(is_function, D, false);
    T_TEST_CV(is_function, pmo, false);
    T_TEST_CV(is_function, pmoc, false);
    T_TEST_CV(is_function, pmov, false);
    T_TEST_CV(is_function, pmocv, false);
    T_TEST_CV(is_function, pmf, false);
    T_TEST_CV(is_function, pmfc, false);
    T_TEST_CV(is_function, pmfv, false);
    T_TEST_CV(is_function, pmfcv, false);
    T_TEST_CV(is_function, arr_t, false);
    T_TEST_CV(is_function, enum_t, false);
    T_TEST_CV(is_function, pf, false);
    T_TEST(is_function, func, true);
    T_TEST_CV(is_function, void*, false);

    typedef void(pf0)();
    typedef void(pf1)(int);
    typedef void(pf2)(int, int);
    typedef void(pf3)(int, int, int);
    typedef void(pf4)(int, int, int, int);
    typedef void(pf5)(int, int, int, int, int);
    CHECK(STD is_function<pf0>::value);
    CHECK(STD is_function<pf1>::value);
    CHECK(STD is_function<pf2>::value);
    CHECK(STD is_function<pf3>::value);
    CHECK(STD is_function<pf4>::value);
    CHECK(STD is_function<pf5>::value);
}

static void t_is_arithmetic() { // test is_arithmetic<T> for various types
    T_TEST_CV(is_arithmetic, void, false);
    T_TEST_CV(is_arithmetic, char, true);
    T_TEST_CV(is_arithmetic, signed char, true);
    T_TEST_CV(is_arithmetic, unsigned char, true);
    T_TEST_CV(is_arithmetic, wchar_t, true);
    T_TEST_CV(is_arithmetic, short, true);
    T_TEST_CV(is_arithmetic, unsigned short, true);
    T_TEST_CV(is_arithmetic, int, true);
    T_TEST_CV(is_arithmetic, unsigned int, true);
    T_TEST_CV(is_arithmetic, long, true);
    T_TEST_CV(is_arithmetic, unsigned long, true);

    T_TEST_CV(is_arithmetic, long long, true);
    T_TEST_CV(is_arithmetic, unsigned long long, true);

    T_TEST_CV(is_arithmetic, float, true);
    T_TEST_CV(is_arithmetic, double, true);
    T_TEST_CV(is_arithmetic, long double, true);
    T_TEST_CV(is_arithmetic, int*, false);
    T_TEST(is_arithmetic, int&, false);
    T_TEST_CV(is_arithmetic, A, false);
    T_TEST_CV(is_arithmetic, B, false);
    T_TEST_CV(is_arithmetic, C, false);
    T_TEST_CV(is_arithmetic, D, false);
    T_TEST_CV(is_arithmetic, pmo, false);
    T_TEST_CV(is_arithmetic, pmoc, false);
    T_TEST_CV(is_arithmetic, pmov, false);
    T_TEST_CV(is_arithmetic, pmocv, false);
    T_TEST_CV(is_arithmetic, pmf, false);
    T_TEST_CV(is_arithmetic, pmfc, false);
    T_TEST_CV(is_arithmetic, pmfv, false);
    T_TEST_CV(is_arithmetic, pmfcv, false);
    T_TEST_CV(is_arithmetic, arr_t, false);
    T_TEST_CV(is_arithmetic, enum_t, false);
    T_TEST_CV(is_arithmetic, pf, false);
    T_TEST(is_arithmetic, func, false);
    T_TEST_CV(is_arithmetic, void*, false);
}

static void t_is_fundamental() { // test is_fundamental<T> for various types
    T_TEST_CV(is_fundamental, void, true);
    T_TEST_CV(is_fundamental, char, true);
    T_TEST_CV(is_fundamental, signed char, true);
    T_TEST_CV(is_fundamental, unsigned char, true);
    T_TEST_CV(is_fundamental, wchar_t, true);
    T_TEST_CV(is_fundamental, short, true);
    T_TEST_CV(is_fundamental, unsigned short, true);
    T_TEST_CV(is_fundamental, int, true);
    T_TEST_CV(is_fundamental, unsigned int, true);
    T_TEST_CV(is_fundamental, long, true);
    T_TEST_CV(is_fundamental, unsigned long, true);

    T_TEST_CV(is_fundamental, long long, true);
    T_TEST_CV(is_fundamental, unsigned long long, true);

    T_TEST_CV(is_fundamental, float, true);
    T_TEST_CV(is_fundamental, double, true);
    T_TEST_CV(is_fundamental, long double, true);
    T_TEST_CV(is_fundamental, int*, false);
    T_TEST(is_fundamental, int&, false);
    T_TEST_CV(is_fundamental, A, false);
    T_TEST_CV(is_fundamental, B, false);
    T_TEST_CV(is_fundamental, C, false);
    T_TEST_CV(is_fundamental, D, false);
    T_TEST_CV(is_fundamental, pmo, false);
    T_TEST_CV(is_fundamental, pmoc, false);
    T_TEST_CV(is_fundamental, pmov, false);
    T_TEST_CV(is_fundamental, pmocv, false);
    T_TEST_CV(is_fundamental, pmf, false);
    T_TEST_CV(is_fundamental, pmfc, false);
    T_TEST_CV(is_fundamental, pmfv, false);
    T_TEST_CV(is_fundamental, pmfcv, false);
    T_TEST_CV(is_fundamental, arr_t, false);
    T_TEST_CV(is_fundamental, enum_t, false);
    T_TEST_CV(is_fundamental, pf, false);
    T_TEST(is_fundamental, func, false);
    T_TEST_CV(is_fundamental, void*, false);
}

static void t_is_object() { // test is_object<T> for various types
    T_TEST_CV(is_object, void, false);
    T_TEST_CV(is_object, char, true);
    T_TEST_CV(is_object, signed char, true);
    T_TEST_CV(is_object, unsigned char, true);
    T_TEST_CV(is_object, wchar_t, true);
    T_TEST_CV(is_object, short, true);
    T_TEST_CV(is_object, unsigned short, true);
    T_TEST_CV(is_object, int, true);
    T_TEST_CV(is_object, unsigned int, true);
    T_TEST_CV(is_object, long, true);
    T_TEST_CV(is_object, unsigned long, true);

    T_TEST_CV(is_object, long long, true);
    T_TEST_CV(is_object, unsigned long long, true);

    T_TEST_CV(is_object, float, true);
    T_TEST_CV(is_object, double, true);
    T_TEST_CV(is_object, long double, true);
    T_TEST_CV(is_object, int*, true);
    T_TEST(is_object, int&, false);
    T_TEST_CV(is_object, A, true);
    T_TEST_CV(is_object, B, true);
    T_TEST_CV(is_object, C, true);
    T_TEST_CV(is_object, D, true);
    T_TEST_CV(is_object, pmo, true);
    T_TEST_CV(is_object, pmoc, true);
    T_TEST_CV(is_object, pmov, true);
    T_TEST_CV(is_object, pmocv, true);
    T_TEST_CV(is_object, pmf, true);
    T_TEST_CV(is_object, pmfc, true);
    T_TEST_CV(is_object, pmfv, true);
    T_TEST_CV(is_object, pmfcv, true);
    T_TEST_CV(is_object, arr_t, true);
    T_TEST_CV(is_object, enum_t, true);
    T_TEST_CV(is_object, pf, true);
    T_TEST(is_object, func, false);
    T_TEST_CV(is_object, void*, true);
}

static void t_is_scalar() { // test is_scalar<T> for various types
    T_TEST_CV(is_scalar, void, false);
    T_TEST_CV(is_scalar, char, true);
    T_TEST_CV(is_scalar, signed char, true);
    T_TEST_CV(is_scalar, unsigned char, true);
    T_TEST_CV(is_scalar, wchar_t, true);
    T_TEST_CV(is_scalar, short, true);
    T_TEST_CV(is_scalar, unsigned short, true);
    T_TEST_CV(is_scalar, int, true);
    T_TEST_CV(is_scalar, unsigned int, true);
    T_TEST_CV(is_scalar, long, true);
    T_TEST_CV(is_scalar, unsigned long, true);

    T_TEST_CV(is_scalar, long long, true);
    T_TEST_CV(is_scalar, unsigned long long, true);

    T_TEST_CV(is_scalar, float, true);
    T_TEST_CV(is_scalar, double, true);
    T_TEST_CV(is_scalar, long double, true);
    T_TEST_CV(is_scalar, int*, true);
    T_TEST(is_scalar, int&, false);

    T_TEST_CV(is_scalar, A, false); // unspecified whether A is instantiated

    T_TEST_CV(is_scalar, B, false);
    T_TEST_CV(is_scalar, C, false);
    T_TEST_CV(is_scalar, D, false);
    T_TEST_CV(is_scalar, pmo, true);
    T_TEST_CV(is_scalar, pmoc, true);
    T_TEST_CV(is_scalar, pmov, true);
    T_TEST_CV(is_scalar, pmocv, true);
    T_TEST_CV(is_scalar, pmf, true);
    T_TEST_CV(is_scalar, pmfc, true);
    T_TEST_CV(is_scalar, pmfv, true);
    T_TEST_CV(is_scalar, pmfcv, true);
    T_TEST_CV(is_scalar, arr_t, false);
    T_TEST_CV(is_scalar, enum_t, true);
    T_TEST_CV(is_scalar, pf, true);
    T_TEST(is_scalar, func, false);
    T_TEST_CV(is_scalar, void*, true);
}

static void t_is_compound() { // test is_compound<T> for various types
    T_TEST_CV(is_compound, void, false);
    T_TEST_CV(is_compound, char, false);
    T_TEST_CV(is_compound, signed char, false);
    T_TEST_CV(is_compound, unsigned char, false);
    T_TEST_CV(is_compound, wchar_t, false);
    T_TEST_CV(is_compound, short, false);
    T_TEST_CV(is_compound, unsigned short, false);
    T_TEST_CV(is_compound, int, false);
    T_TEST_CV(is_compound, unsigned int, false);
    T_TEST_CV(is_compound, long, false);
    T_TEST_CV(is_compound, unsigned long, false);

    T_TEST_CV(is_compound, long long, false);
    T_TEST_CV(is_compound, unsigned long long, false);

    T_TEST_CV(is_compound, float, false);
    T_TEST_CV(is_compound, double, false);
    T_TEST_CV(is_compound, long double, false);
    T_TEST_CV(is_compound, int*, true);
    T_TEST(is_compound, int&, true);
    T_TEST_CV(is_compound, A, true);
    T_TEST_CV(is_compound, B, true);
    T_TEST_CV(is_compound, C, true);
    T_TEST_CV(is_compound, D, true);
    T_TEST_CV(is_compound, pmo, true);
    T_TEST_CV(is_compound, pmoc, true);
    T_TEST_CV(is_compound, pmov, true);
    T_TEST_CV(is_compound, pmocv, true);
    T_TEST_CV(is_compound, pmf, true);
    T_TEST_CV(is_compound, pmfc, true);
    T_TEST_CV(is_compound, pmfv, true);
    T_TEST_CV(is_compound, pmfcv, true);
    T_TEST_CV(is_compound, arr_t, true);
    T_TEST_CV(is_compound, enum_t, true);
    T_TEST_CV(is_compound, pf, true);
    T_TEST(is_compound, func, true);
    T_TEST_CV(is_compound, void*, true);
}

static void t_is_member_pointer() { // test is_member_pointer<T> for various types
    T_TEST_CV(is_member_pointer, void, false);
    T_TEST_CV(is_member_pointer, char, false);
    T_TEST_CV(is_member_pointer, signed char, false);
    T_TEST_CV(is_member_pointer, unsigned char, false);
    T_TEST_CV(is_member_pointer, wchar_t, false);
    T_TEST_CV(is_member_pointer, short, false);
    T_TEST_CV(is_member_pointer, unsigned short, false);
    T_TEST_CV(is_member_pointer, int, false);
    T_TEST_CV(is_member_pointer, unsigned int, false);
    T_TEST_CV(is_member_pointer, long, false);
    T_TEST_CV(is_member_pointer, unsigned long, false);

    T_TEST_CV(is_member_pointer, long long, false);
    T_TEST_CV(is_member_pointer, unsigned long long, false);

    T_TEST_CV(is_member_pointer, float, false);
    T_TEST_CV(is_member_pointer, double, false);
    T_TEST_CV(is_member_pointer, long double, false);
    T_TEST_CV(is_member_pointer, int*, false);
    T_TEST(is_member_pointer, int&, false);
    T_TEST_CV(is_member_pointer, A, false);
    T_TEST_CV(is_member_pointer, B, false);
    T_TEST_CV(is_member_pointer, C, false);
    T_TEST_CV(is_member_pointer, D, false);
    T_TEST_CV(is_member_pointer, pmo, true);
    T_TEST_CV(is_member_pointer, pmoc, true);
    T_TEST_CV(is_member_pointer, pmov, true);
    T_TEST_CV(is_member_pointer, pmocv, true);
    T_TEST_CV(is_member_pointer, pmf, true);
    T_TEST_CV(is_member_pointer, pmfc, true);
    T_TEST_CV(is_member_pointer, pmfv, true);
    T_TEST_CV(is_member_pointer, pmfcv, true);
    T_TEST_CV(is_member_pointer, arr_t, false);
    T_TEST_CV(is_member_pointer, enum_t, false);
    T_TEST_CV(is_member_pointer, pf, false);
    T_TEST(is_member_pointer, func, false);
    T_TEST_CV(is_member_pointer, void*, false);
}

void test_main() { // test type traits
    t_is_enum();
    t_is_union();
    t_is_class();
    t_is_function();
    t_is_arithmetic();
    t_is_fundamental();
    t_is_object();
    t_is_scalar();
    t_is_compound();
    t_is_member_pointer();
}
