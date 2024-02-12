// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <type_traits> header, part 1
#define TEST_NAME "<type_traits>, part 1"

#define _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING
#define _SILENCE_CXX23_ALIGNED_UNION_DEPRECATION_WARNING

#include "tdefs.h"
#include "typetr.h"
#include <type_traits>

// TESTS
static void t_wrappers() { // test integral_constant, true_type, false_type
    typedef STD integral_constant<int, 3> XX;
    T_INTEGRAL_CONSTANT(XX, int, 3);
    T_INTEGRAL_CONSTANT(STD true_type, bool, true);
    T_INTEGRAL_CONSTANT(STD false_type, bool, false);
}

static void t_is_void() { // test is_void<T> for various types
    T_TEST_CV(is_void, void, true);
    T_TEST_CV(is_void, char, false);
    T_TEST_CV(is_void, signed char, false);
    T_TEST_CV(is_void, unsigned char, false);
    T_TEST_CV(is_void, wchar_t, false);
    T_TEST_CV(is_void, short, false);
    T_TEST_CV(is_void, unsigned short, false);
    T_TEST_CV(is_void, int, false);
    T_TEST_CV(is_void, unsigned int, false);
    T_TEST_CV(is_void, long, false);
    T_TEST_CV(is_void, unsigned long, false);

    T_TEST_CV(is_void, long long, false);
    T_TEST_CV(is_void, unsigned long long, false);

    T_TEST_CV(is_void, float, false);
    T_TEST_CV(is_void, double, false);
    T_TEST_CV(is_void, long double, false);
    T_TEST_CV(is_void, int*, false);
    T_TEST(is_void, int&, false);
    T_TEST_CV(is_void, A, false);
    T_TEST_CV(is_void, B, false);
    T_TEST_CV(is_void, C, false);
    T_TEST_CV(is_void, D, false);
    T_TEST_CV(is_void, U, false);
    T_TEST_CV(is_void, pmo, false);
    T_TEST_CV(is_void, pmoc, false);
    T_TEST_CV(is_void, pmov, false);
    T_TEST_CV(is_void, pmocv, false);
    T_TEST_CV(is_void, pmf, false);
    T_TEST_CV(is_void, pmfc, false);
    T_TEST_CV(is_void, pmfv, false);
    T_TEST_CV(is_void, pmfcv, false);
    T_TEST_CV(is_void, arr_t, false);
    T_TEST_CV(is_void, enum_t, false);
    T_TEST_CV(is_void, pf, false);
    T_TEST(is_void, func, false);
    T_TEST_CV(is_void, void*, false);
}

static void t_is_integral() { // test is_integral<T> for various types
    T_TEST_CV(is_integral, void, false);
    T_TEST_CV(is_integral, char, true);
    T_TEST_CV(is_integral, signed char, true);
    T_TEST_CV(is_integral, unsigned char, true);
    T_TEST_CV(is_integral, wchar_t, true);
    T_TEST_CV(is_integral, short, true);
    T_TEST_CV(is_integral, unsigned short, true);
    T_TEST_CV(is_integral, int, true);
    T_TEST_CV(is_integral, unsigned int, true);
    T_TEST_CV(is_integral, long, true);
    T_TEST_CV(is_integral, unsigned long, true);

    T_TEST_CV(is_integral, long long, true);
    T_TEST_CV(is_integral, unsigned long long, true);

    T_TEST_CV(is_integral, float, false);
    T_TEST_CV(is_integral, double, false);
    T_TEST_CV(is_integral, long double, false);
    T_TEST_CV(is_integral, int*, false);
    T_TEST(is_integral, int&, false);
    T_TEST_CV(is_integral, A, false);
    T_TEST_CV(is_integral, B, false);
    T_TEST_CV(is_integral, C, false);
    T_TEST_CV(is_integral, D, false);
    T_TEST_CV(is_integral, U, false);
    T_TEST_CV(is_integral, pmo, false);
    T_TEST_CV(is_integral, pmoc, false);
    T_TEST_CV(is_integral, pmov, false);
    T_TEST_CV(is_integral, pmocv, false);
    T_TEST_CV(is_integral, pmf, false);
    T_TEST_CV(is_integral, pmfc, false);
    T_TEST_CV(is_integral, pmfv, false);
    T_TEST_CV(is_integral, pmfcv, false);
    T_TEST_CV(is_integral, arr_t, false);
    T_TEST_CV(is_integral, enum_t, false);
    T_TEST_CV(is_integral, pf, false);
    T_TEST(is_integral, func, false);
    T_TEST_CV(is_integral, void*, false);
}

static void t_is_floating_point() { // test is_floating_point<T> for various types
    T_TEST_CV(is_floating_point, void, false);
    T_TEST_CV(is_floating_point, char, false);
    T_TEST_CV(is_floating_point, signed char, false);
    T_TEST_CV(is_floating_point, unsigned char, false);
    T_TEST_CV(is_floating_point, wchar_t, false);
    T_TEST_CV(is_floating_point, short, false);
    T_TEST_CV(is_floating_point, unsigned short, false);
    T_TEST_CV(is_floating_point, int, false);
    T_TEST_CV(is_floating_point, unsigned int, false);
    T_TEST_CV(is_floating_point, long, false);
    T_TEST_CV(is_floating_point, unsigned long, false);

    T_TEST_CV(is_floating_point, long long, false);
    T_TEST_CV(is_floating_point, unsigned long long, false);

    T_TEST_CV(is_floating_point, float, true);
    T_TEST_CV(is_floating_point, double, true);
    T_TEST_CV(is_floating_point, long double, true);
    T_TEST_CV(is_floating_point, int*, false);
    T_TEST(is_floating_point, int&, false);
    T_TEST_CV(is_floating_point, A, false);
    T_TEST_CV(is_floating_point, B, false);
    T_TEST_CV(is_floating_point, C, false);
    T_TEST_CV(is_floating_point, D, false);
    T_TEST_CV(is_floating_point, U, false);
    T_TEST_CV(is_floating_point, pmo, false);
    T_TEST_CV(is_floating_point, pmoc, false);
    T_TEST_CV(is_floating_point, pmov, false);
    T_TEST_CV(is_floating_point, pmocv, false);
    T_TEST_CV(is_floating_point, pmf, false);
    T_TEST_CV(is_floating_point, pmfc, false);
    T_TEST_CV(is_floating_point, pmfv, false);
    T_TEST_CV(is_floating_point, pmfcv, false);
    T_TEST_CV(is_floating_point, arr_t, false);
    T_TEST_CV(is_floating_point, enum_t, false);
    T_TEST_CV(is_floating_point, pf, false);
    T_TEST(is_floating_point, func, false);
    T_TEST_CV(is_floating_point, void*, false);
}

static void t_is_array() { // test is_array<T> for various types
    T_TEST_CV(is_array, void, false);
    T_TEST_CV(is_array, char, false);
    T_TEST_CV(is_array, signed char, false);
    T_TEST_CV(is_array, unsigned char, false);
    T_TEST_CV(is_array, wchar_t, false);
    T_TEST_CV(is_array, short, false);
    T_TEST_CV(is_array, unsigned short, false);
    T_TEST_CV(is_array, int, false);
    T_TEST_CV(is_array, unsigned int, false);
    T_TEST_CV(is_array, long, false);
    T_TEST_CV(is_array, unsigned long, false);

    T_TEST_CV(is_array, long long, false);
    T_TEST_CV(is_array, unsigned long long, false);

    T_TEST_CV(is_array, float, false);
    T_TEST_CV(is_array, double, false);
    T_TEST_CV(is_array, long double, false);
    T_TEST_CV(is_array, int*, false);
    T_TEST(is_array, int&, false);
    T_TEST_CV(is_array, A, false);
    T_TEST_CV(is_array, B, false);
    T_TEST_CV(is_array, B, false);
    T_TEST_CV(is_array, C, false);
    T_TEST_CV(is_array, D, false);
    T_TEST_CV(is_array, U, false);
    T_TEST_CV(is_array, pmo, false);
    T_TEST_CV(is_array, pmoc, false);
    T_TEST_CV(is_array, pmov, false);
    T_TEST_CV(is_array, pmocv, false);
    T_TEST_CV(is_array, pmf, false);
    T_TEST_CV(is_array, pmfc, false);
    T_TEST_CV(is_array, pmfv, false);
    T_TEST_CV(is_array, pmfcv, false);
    T_TEST_CV(is_array, arr_t, true);
    T_TEST_CV(is_array, enum_t, false);
    T_TEST_CV(is_array, pf, false);
    T_TEST(is_array, func, false);
    T_TEST_CV(is_array, void*, false);
}

static void t_is_pointer() { // test is_pointer<T> for various types
    T_TEST_CV(is_pointer, void, false);
    T_TEST_CV(is_pointer, char, false);
    T_TEST_CV(is_pointer, signed char, false);
    T_TEST_CV(is_pointer, unsigned char, false);
    T_TEST_CV(is_pointer, wchar_t, false);
    T_TEST_CV(is_pointer, short, false);
    T_TEST_CV(is_pointer, unsigned short, false);
    T_TEST_CV(is_pointer, int, false);
    T_TEST_CV(is_pointer, unsigned int, false);
    T_TEST_CV(is_pointer, long, false);
    T_TEST_CV(is_pointer, unsigned long, false);

    T_TEST_CV(is_pointer, long long, false);
    T_TEST_CV(is_pointer, unsigned long long, false);

    T_TEST_CV(is_pointer, float, false);
    T_TEST_CV(is_pointer, double, false);
    T_TEST_CV(is_pointer, long double, false);
    T_TEST_CV(is_pointer, int*, true);
    T_TEST(is_pointer, int&, false);
    T_TEST_CV(is_pointer, A, false);
    T_TEST_CV(is_pointer, B, false);
    T_TEST_CV(is_pointer, C, false);
    T_TEST_CV(is_pointer, D, false);
    T_TEST_CV(is_pointer, U, false);
    T_TEST_CV(is_pointer, pmo, false);
    T_TEST_CV(is_pointer, pmoc, false);
    T_TEST_CV(is_pointer, pmov, false);
    T_TEST_CV(is_pointer, pmocv, false);
    T_TEST_CV(is_pointer, pmf, false);
    T_TEST_CV(is_pointer, pmfc, false);
    T_TEST_CV(is_pointer, pmfv, false);
    T_TEST_CV(is_pointer, pmfcv, false);
    T_TEST_CV(is_pointer, arr_t, false);
    T_TEST_CV(is_pointer, enum_t, false);
    T_TEST_CV(is_pointer, pf, true);
    T_TEST(is_pointer, func, false);
    T_TEST_CV(is_pointer, void*, true);
}

static void t_is_lvalue_reference() { // test is_lvalue_reference<T> for various types
    T_TEST_CV(is_lvalue_reference, void, false);
    T_TEST_CV(is_lvalue_reference, char, false);
    T_TEST_CV(is_lvalue_reference, signed char, false);
    T_TEST_CV(is_lvalue_reference, unsigned char, false);
    T_TEST_CV(is_lvalue_reference, wchar_t, false);
    T_TEST_CV(is_lvalue_reference, short, false);
    T_TEST_CV(is_lvalue_reference, unsigned short, false);
    T_TEST_CV(is_lvalue_reference, int, false);
    T_TEST_CV(is_lvalue_reference, unsigned int, false);
    T_TEST_CV(is_lvalue_reference, long, false);
    T_TEST_CV(is_lvalue_reference, unsigned long, false);

    T_TEST_CV(is_lvalue_reference, long long, false);
    T_TEST_CV(is_lvalue_reference, unsigned long long, false);

    T_TEST_CV(is_lvalue_reference, float, false);
    T_TEST_CV(is_lvalue_reference, double, false);
    T_TEST_CV(is_lvalue_reference, long double, false);
    T_TEST_CV(is_lvalue_reference, int*, false);
    T_TEST(is_lvalue_reference, int&, true);

    T_TEST(is_lvalue_reference, int&&, false);

    T_TEST_CV(is_lvalue_reference, A, false);
    T_TEST_CV(is_lvalue_reference, C, false);
    T_TEST_CV(is_lvalue_reference, D, false);
    T_TEST_CV(is_lvalue_reference, U, false);
    T_TEST_CV(is_lvalue_reference, pmo, false);
    T_TEST_CV(is_lvalue_reference, pmoc, false);
    T_TEST_CV(is_lvalue_reference, pmov, false);
    T_TEST_CV(is_lvalue_reference, pmocv, false);
    T_TEST_CV(is_lvalue_reference, pmf, false);
    T_TEST_CV(is_lvalue_reference, pmfc, false);
    T_TEST_CV(is_lvalue_reference, pmfv, false);
    T_TEST_CV(is_lvalue_reference, pmfcv, false);
    T_TEST_CV(is_lvalue_reference, arr_t, false);
    T_TEST_CV(is_lvalue_reference, enum_t, false);
    T_TEST_CV(is_lvalue_reference, pf, false);
    T_TEST(is_lvalue_reference, func, false);
    T_TEST_CV(is_lvalue_reference, void*, false);
}

static void t_is_rvalue_reference() { // test is_rvalue_reference<T> for various types
    T_TEST_CV(is_rvalue_reference, void, false);
    T_TEST_CV(is_rvalue_reference, char, false);
    T_TEST_CV(is_rvalue_reference, signed char, false);
    T_TEST_CV(is_rvalue_reference, unsigned char, false);
    T_TEST_CV(is_rvalue_reference, wchar_t, false);
    T_TEST_CV(is_rvalue_reference, short, false);
    T_TEST_CV(is_rvalue_reference, unsigned short, false);
    T_TEST_CV(is_rvalue_reference, int, false);
    T_TEST_CV(is_rvalue_reference, unsigned int, false);
    T_TEST_CV(is_rvalue_reference, long, false);
    T_TEST_CV(is_rvalue_reference, unsigned long, false);

    T_TEST_CV(is_rvalue_reference, long long, false);
    T_TEST_CV(is_rvalue_reference, unsigned long long, false);

    T_TEST_CV(is_rvalue_reference, float, false);
    T_TEST_CV(is_rvalue_reference, double, false);
    T_TEST_CV(is_rvalue_reference, long double, false);
    T_TEST_CV(is_rvalue_reference, int*, false);
    T_TEST(is_rvalue_reference, int&, false);

    T_TEST(is_rvalue_reference, int&&, true);

    T_TEST_CV(is_rvalue_reference, A, false);
    T_TEST_CV(is_rvalue_reference, C, false);
    T_TEST_CV(is_rvalue_reference, D, false);
    T_TEST_CV(is_rvalue_reference, U, false);
    T_TEST_CV(is_rvalue_reference, pmo, false);
    T_TEST_CV(is_rvalue_reference, pmoc, false);
    T_TEST_CV(is_rvalue_reference, pmov, false);
    T_TEST_CV(is_rvalue_reference, pmocv, false);
    T_TEST_CV(is_rvalue_reference, pmf, false);
    T_TEST_CV(is_rvalue_reference, pmfc, false);
    T_TEST_CV(is_rvalue_reference, pmfv, false);
    T_TEST_CV(is_rvalue_reference, pmfcv, false);
    T_TEST_CV(is_rvalue_reference, arr_t, false);
    T_TEST_CV(is_rvalue_reference, enum_t, false);
    T_TEST_CV(is_rvalue_reference, pf, false);
    T_TEST(is_rvalue_reference, func, false);
    T_TEST_CV(is_rvalue_reference, void*, false);
}

static void t_is_reference() { // test is_reference<T> for various types
    T_TEST_CV(is_reference, void, false);
    T_TEST_CV(is_reference, char, false);
    T_TEST_CV(is_reference, signed char, false);
    T_TEST_CV(is_reference, unsigned char, false);
    T_TEST_CV(is_reference, wchar_t, false);
    T_TEST_CV(is_reference, short, false);
    T_TEST_CV(is_reference, unsigned short, false);
    T_TEST_CV(is_reference, int, false);
    T_TEST_CV(is_reference, unsigned int, false);
    T_TEST_CV(is_reference, long, false);
    T_TEST_CV(is_reference, unsigned long, false);

    T_TEST_CV(is_reference, long long, false);
    T_TEST_CV(is_reference, unsigned long long, false);

    T_TEST_CV(is_reference, float, false);
    T_TEST_CV(is_reference, double, false);
    T_TEST_CV(is_reference, long double, false);
    T_TEST_CV(is_reference, int*, false);
    T_TEST(is_reference, int&, true);

    T_TEST(is_reference, int&&, true);

    T_TEST_CV(is_reference, A, false);
    T_TEST_CV(is_reference, C, false);
    T_TEST_CV(is_reference, D, false);
    T_TEST_CV(is_reference, U, false);
    T_TEST_CV(is_reference, pmo, false);
    T_TEST_CV(is_reference, pmoc, false);
    T_TEST_CV(is_reference, pmov, false);
    T_TEST_CV(is_reference, pmocv, false);
    T_TEST_CV(is_reference, pmf, false);
    T_TEST_CV(is_reference, pmfc, false);
    T_TEST_CV(is_reference, pmfv, false);
    T_TEST_CV(is_reference, pmfcv, false);
    T_TEST_CV(is_reference, arr_t, false);
    T_TEST_CV(is_reference, enum_t, false);
    T_TEST_CV(is_reference, pf, false);
    T_TEST(is_reference, func, false);
    T_TEST_CV(is_reference, void*, false);
}

static void t_is_member_object_pointer() { // test is_member_object_pointer<T> for various types
    T_TEST_CV(is_member_object_pointer, void, false);
    T_TEST_CV(is_member_object_pointer, char, false);
    T_TEST_CV(is_member_object_pointer, signed char, false);
    T_TEST_CV(is_member_object_pointer, unsigned char, false);
    T_TEST_CV(is_member_object_pointer, wchar_t, false);
    T_TEST_CV(is_member_object_pointer, short, false);
    T_TEST_CV(is_member_object_pointer, unsigned short, false);
    T_TEST_CV(is_member_object_pointer, int, false);
    T_TEST_CV(is_member_object_pointer, unsigned int, false);
    T_TEST_CV(is_member_object_pointer, long, false);
    T_TEST_CV(is_member_object_pointer, unsigned long, false);

    T_TEST_CV(is_member_object_pointer, long long, false);
    T_TEST_CV(is_member_object_pointer, unsigned long long, false);

    T_TEST_CV(is_member_object_pointer, float, false);
    T_TEST_CV(is_member_object_pointer, double, false);
    T_TEST_CV(is_member_object_pointer, long double, false);
    T_TEST_CV(is_member_object_pointer, int*, false);
    T_TEST(is_member_object_pointer, int&, false);
    T_TEST_CV(is_member_object_pointer, A, false);
    T_TEST_CV(is_member_object_pointer, B, false);
    T_TEST_CV(is_member_object_pointer, C, false);
    T_TEST_CV(is_member_object_pointer, D, false);
    T_TEST_CV(is_member_object_pointer, U, false);
    T_TEST_CV(is_member_object_pointer, pmo, true);
    T_TEST_CV(is_member_object_pointer, pmoc, true);
    T_TEST_CV(is_member_object_pointer, pmov, true);
    T_TEST_CV(is_member_object_pointer, pmocv, true);
    T_TEST_CV(is_member_object_pointer, pmf, false);
    T_TEST_CV(is_member_object_pointer, pmfc, false);
    T_TEST_CV(is_member_object_pointer, pmfv, false);
    T_TEST_CV(is_member_object_pointer, pmfcv, false);
    T_TEST_CV(is_member_object_pointer, arr_t, false);
    T_TEST_CV(is_member_object_pointer, enum_t, false);
    T_TEST_CV(is_member_object_pointer, pf, false);
    T_TEST(is_member_object_pointer, func, false);
    T_TEST_CV(is_member_object_pointer, void*, false);
}

static void t_is_member_function_pointer() { // test is_member_function_pointer<T> for various types
    T_TEST_CV(is_member_function_pointer, void, false);
    T_TEST_CV(is_member_function_pointer, char, false);
    T_TEST_CV(is_member_function_pointer, signed char, false);
    T_TEST_CV(is_member_function_pointer, unsigned char, false);
    T_TEST_CV(is_member_function_pointer, wchar_t, false);
    T_TEST_CV(is_member_function_pointer, short, false);
    T_TEST_CV(is_member_function_pointer, unsigned short, false);
    T_TEST_CV(is_member_function_pointer, int, false);
    T_TEST_CV(is_member_function_pointer, unsigned int, false);
    T_TEST_CV(is_member_function_pointer, long, false);
    T_TEST_CV(is_member_function_pointer, unsigned long, false);

    T_TEST_CV(is_member_function_pointer, long long, false);
    T_TEST_CV(is_member_function_pointer, unsigned long long, false);

    T_TEST_CV(is_member_function_pointer, float, false);
    T_TEST_CV(is_member_function_pointer, double, false);
    T_TEST_CV(is_member_function_pointer, long double, false);
    T_TEST_CV(is_member_function_pointer, int*, false);
    T_TEST(is_member_function_pointer, int&, false);
    T_TEST_CV(is_member_function_pointer, A, false);
    T_TEST_CV(is_member_function_pointer, B, false);
    T_TEST_CV(is_member_function_pointer, C, false);
    T_TEST_CV(is_member_function_pointer, D, false);
    T_TEST_CV(is_member_function_pointer, U, false);
    T_TEST_CV(is_member_function_pointer, pmo, false);
    T_TEST_CV(is_member_function_pointer, pmoc, false);
    T_TEST_CV(is_member_function_pointer, pmov, false);
    T_TEST_CV(is_member_function_pointer, pmocv, false);
    T_TEST_CV(is_member_function_pointer, pmf, true);
    T_TEST_CV(is_member_function_pointer, pmfc, true);
    T_TEST_CV(is_member_function_pointer, pmfv, true);
    T_TEST_CV(is_member_function_pointer, pmfcv, true);
    T_TEST_CV(is_member_function_pointer, arr_t, false);
    T_TEST_CV(is_member_function_pointer, enum_t, false);
    T_TEST_CV(is_member_function_pointer, pf, false);
    T_TEST(is_member_function_pointer, func, false);
    T_TEST_CV(is_member_function_pointer, void*, false);

    typedef void (B::*pf0)();
    typedef void (B::*pf1)(int);
    typedef void (B::*pf2)(int, int);
    typedef void (B::*pf3)(int, int, int);
    typedef void (B::*pf4)(int, int, int, int);
    typedef void (B::*pf5)(int, int, int, int, int);
    CHECK(STD is_member_function_pointer<pf0>::value);
    CHECK(STD is_member_function_pointer<pf1>::value);
    CHECK(STD is_member_function_pointer<pf2>::value);
    CHECK(STD is_member_function_pointer<pf3>::value);
    CHECK(STD is_member_function_pointer<pf4>::value);
    CHECK(STD is_member_function_pointer<pf5>::value);
}

void t_aliases() { // test template aliases
    typedef int Ty;

    CHECK_TYPE(STD remove_const_t<Ty>, STD remove_const<Ty>::type);
    CHECK_TYPE(STD remove_volatile_t<Ty>, STD remove_volatile<Ty>::type);
    CHECK_TYPE(STD remove_cv_t<Ty>, STD remove_cv<Ty>::type);
    CHECK_TYPE(STD add_const_t<Ty>, STD add_const<Ty>::type);
    CHECK_TYPE(STD add_volatile_t<Ty>, STD add_volatile<Ty>::type);
    CHECK_TYPE(STD add_cv_t<Ty>, STD add_cv<Ty>::type);
    CHECK_TYPE(STD remove_reference_t<Ty>, STD remove_reference<Ty>::type);
    CHECK_TYPE(STD add_lvalue_reference_t<Ty>, STD add_lvalue_reference<Ty>::type);
    CHECK_TYPE(STD add_rvalue_reference_t<Ty>, STD add_rvalue_reference<Ty>::type);
    CHECK_TYPE(STD make_signed_t<Ty>, STD make_signed<Ty>::type);
    CHECK_TYPE(STD make_unsigned_t<Ty>, STD make_unsigned<Ty>::type);
    CHECK_TYPE(STD remove_extent_t<Ty>, STD remove_extent<Ty>::type);
    CHECK_TYPE(STD remove_all_extents_t<Ty>, STD remove_all_extents<Ty>::type);
    CHECK_TYPE(STD remove_pointer_t<Ty>, STD remove_pointer<Ty>::type);
    CHECK_TYPE(STD add_pointer_t<Ty>, STD add_pointer<Ty>::type);
    {
        typedef STD aligned_storage_t<10, 4> Ty1;
        typedef STD aligned_storage<10, 4>::type Ty2;
        CHECK_TYPE(Ty1, Ty2);
    }
    {
        typedef STD aligned_union_t<10, int> Ty1;
        typedef STD aligned_union<10, int>::type Ty2;
        CHECK_TYPE(Ty1, Ty2);
    }
    CHECK_TYPE(STD decay_t<Ty>, STD decay<Ty>::type);
    {
        typedef STD enable_if_t<true, Ty> Ty1;
        typedef STD enable_if<true, Ty>::type Ty2;
        CHECK_TYPE(Ty1, Ty2);
    }
    {
        typedef STD conditional_t<true, Ty, Ty> Ty1;
        typedef STD conditional<true, Ty, Ty>::type Ty2;
        CHECK_TYPE(Ty1, Ty2);
    }
    {
        typedef STD common_type_t<Ty, Ty> Ty1;
        typedef STD common_type<Ty, Ty>::type Ty2;
        CHECK_TYPE(Ty1, Ty2);
    }
    CHECK_TYPE(STD underlying_type_t<enum_t>, STD underlying_type<enum_t>::type);
    {
#if _HAS_CXX17
        CHECK_TYPE(STD invoke_result_t<pf>, STD invoke_result<pf>::type);
#else
        CHECK_TYPE(STD result_of_t<pf()>, STD result_of<pf()>::type);
#endif // _HAS_CXX17
    }
}

void test_main() { // test type traits
    t_wrappers();
    t_is_void();
    t_is_integral();
    t_is_floating_point();
    t_is_array();
    t_is_pointer();

    t_is_lvalue_reference();
    t_is_rvalue_reference();

    t_is_reference();
    t_is_member_object_pointer();
    t_is_member_function_pointer();

    t_aliases();
}
