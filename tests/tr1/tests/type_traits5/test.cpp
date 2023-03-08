// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <type_traits> header, part 5
#define TEST_NAME "<type_traits>, part 5"

#define _DISABLE_EXTENDED_ALIGNED_STORAGE 1
#define _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING
#define _SILENCE_CXX23_ALIGNED_UNION_DEPRECATION_WARNING

#include "tdefs.h"
#include "typetr.h"
#include <limits.h>
#include <limits>
#include <type_traits>

#define CHAR_ALIGN    1
#define PTR_ALIGN     4
#define INT_ALIGN     4
#define LONG_ALIGN    4
#define FLOAT_ALIGN   4
#define DOUBLE_ALIGN  4
#define LDOUBLE_ALIGN 4

#define CHECK_TYPEX(...) static_assert(STD is_same_v<__VA_ARGS__>, "!is_same_v<" #__VA_ARGS__ ">")

// TESTS
static void t_remove_const() { // test remove_const<T> for various types
    CHECK_TYPEX(STD remove_const<I>::type, I);
    CHECK_TYPEX(STD remove_const<CI>::type, I);
    CHECK_TYPEX(STD remove_const<VI>::type, VI);
    CHECK_TYPEX(STD remove_const<CVI>::type, VI);
    CHECK_TYPEX(STD remove_const<IP>::type, IP);
    CHECK_TYPEX(STD remove_const<ICP>::type, IP);
    CHECK_TYPEX(STD remove_const<IVP>::type, IVP);
    CHECK_TYPEX(STD remove_const<ICVP>::type, IVP);
    CHECK_TYPEX(STD remove_const<CIP>::type, CIP);
    CHECK_TYPEX(STD remove_const<CICP>::type, CIP);
    CHECK_TYPEX(STD remove_const<CIVP>::type, CIVP);
    CHECK_TYPEX(STD remove_const<CICVP>::type, CIVP);
    CHECK_TYPEX(STD remove_const<VIP>::type, VIP);
    CHECK_TYPEX(STD remove_const<VICP>::type, VIP);
    CHECK_TYPEX(STD remove_const<VIVP>::type, VIVP);
    CHECK_TYPEX(STD remove_const<VICVP>::type, VIVP);
    CHECK_TYPEX(STD remove_const<CVIP>::type, CVIP);
    CHECK_TYPEX(STD remove_const<CVICP>::type, CVIP);
    CHECK_TYPEX(STD remove_const<CVIVP>::type, CVIVP);
    CHECK_TYPEX(STD remove_const<CVICVP>::type, CVIVP);
}

static void t_remove_volatile() { // test remove_volatile<T> for various types
    CHECK_TYPEX(STD remove_volatile<I>::type, I);
    CHECK_TYPEX(STD remove_volatile<CI>::type, CI);
    CHECK_TYPEX(STD remove_volatile<VI>::type, I);
    CHECK_TYPEX(STD remove_volatile<CVI>::type, CI);
    CHECK_TYPEX(STD remove_volatile<IP>::type, IP);
    CHECK_TYPEX(STD remove_volatile<ICP>::type, ICP);
    CHECK_TYPEX(STD remove_volatile<IVP>::type, IP);
    CHECK_TYPEX(STD remove_volatile<ICVP>::type, ICP);
    CHECK_TYPEX(STD remove_volatile<CIP>::type, CIP);
    CHECK_TYPEX(STD remove_volatile<CICP>::type, CICP);
    CHECK_TYPEX(STD remove_volatile<CIVP>::type, CIP);
    CHECK_TYPEX(STD remove_volatile<CICVP>::type, CICP);
    CHECK_TYPEX(STD remove_volatile<VIP>::type, VIP);
    CHECK_TYPEX(STD remove_volatile<VICP>::type, VICP);
    CHECK_TYPEX(STD remove_volatile<VIVP>::type, VIP);
    CHECK_TYPEX(STD remove_volatile<VICVP>::type, VICP);
    CHECK_TYPEX(STD remove_volatile<CVIP>::type, CVIP);
    CHECK_TYPEX(STD remove_volatile<CVICP>::type, CVICP);
    CHECK_TYPEX(STD remove_volatile<CVIVP>::type, CVIP);
    CHECK_TYPEX(STD remove_volatile<CVICVP>::type, CVICP);
}

static void t_remove_cv() { // test remove_cv<T> for various types
    CHECK_TYPEX(STD remove_cv<I>::type, I);
    CHECK_TYPEX(STD remove_cv<CI>::type, I);
    CHECK_TYPEX(STD remove_cv<VI>::type, I);
    CHECK_TYPEX(STD remove_cv<CVI>::type, I);
    CHECK_TYPEX(STD remove_cv<IP>::type, IP);
    CHECK_TYPEX(STD remove_cv<ICP>::type, IP);
    CHECK_TYPEX(STD remove_cv<IVP>::type, IP);
    CHECK_TYPEX(STD remove_cv<ICVP>::type, IP);
    CHECK_TYPEX(STD remove_cv<CIP>::type, CIP);
    CHECK_TYPEX(STD remove_cv<CICP>::type, CIP);
    CHECK_TYPEX(STD remove_cv<CIVP>::type, CIP);
    CHECK_TYPEX(STD remove_cv<CICVP>::type, CIP);
    CHECK_TYPEX(STD remove_cv<VIP>::type, VIP);
    CHECK_TYPEX(STD remove_cv<VICP>::type, VIP);
    CHECK_TYPEX(STD remove_cv<VIVP>::type, VIP);
    CHECK_TYPEX(STD remove_cv<VICVP>::type, VIP);
    CHECK_TYPEX(STD remove_cv<CVIP>::type, CVIP);
    CHECK_TYPEX(STD remove_cv<CVICP>::type, CVIP);
    CHECK_TYPEX(STD remove_cv<CVIVP>::type, CVIP);
    CHECK_TYPEX(STD remove_cv<CVICVP>::type, CVIP);
}

static void t_add_const() { // test add_const<T> for various types
    CHECK_TYPEX(STD add_const<I>::type, CI);
    CHECK_TYPEX(STD add_const<CI>::type, CI);
    CHECK_TYPEX(STD add_const<VI>::type, CVI);
    CHECK_TYPEX(STD add_const<CVI>::type, CVI);
    CHECK_TYPEX(STD add_const<IP>::type, ICP);
    CHECK_TYPEX(STD add_const<ICP>::type, ICP);
    CHECK_TYPEX(STD add_const<IVP>::type, ICVP);
    CHECK_TYPEX(STD add_const<ICVP>::type, ICVP);
    CHECK_TYPEX(STD add_const<CIP>::type, CICP);
    CHECK_TYPEX(STD add_const<CICP>::type, CICP);
    CHECK_TYPEX(STD add_const<CIVP>::type, CICVP);
    CHECK_TYPEX(STD add_const<CICVP>::type, CICVP);
    CHECK_TYPEX(STD add_const<VIP>::type, VICP);
    CHECK_TYPEX(STD add_const<VICP>::type, VICP);
    CHECK_TYPEX(STD add_const<VIVP>::type, VICVP);
    CHECK_TYPEX(STD add_const<VICVP>::type, VICVP);
    CHECK_TYPEX(STD add_const<CVIP>::type, CVICP);
    CHECK_TYPEX(STD add_const<CVICP>::type, CVICP);
    CHECK_TYPEX(STD add_const<CVIVP>::type, CVICVP);
    CHECK_TYPEX(STD add_const<CVICVP>::type, CVICVP);
}

static void t_add_volatile() { // test add_volatile<T> for various types
    CHECK_TYPEX(STD add_volatile<I>::type, VI);
    CHECK_TYPEX(STD add_volatile<CI>::type, CVI);
    CHECK_TYPEX(STD add_volatile<VI>::type, VI);
    CHECK_TYPEX(STD add_volatile<CVI>::type, CVI);
    CHECK_TYPEX(STD add_volatile<IP>::type, IVP);
    CHECK_TYPEX(STD add_volatile<ICP>::type, ICVP);
    CHECK_TYPEX(STD add_volatile<IVP>::type, IVP);
    CHECK_TYPEX(STD add_volatile<ICVP>::type, ICVP);
    CHECK_TYPEX(STD add_volatile<CIP>::type, CIVP);
    CHECK_TYPEX(STD add_volatile<CICP>::type, CICVP);
    CHECK_TYPEX(STD add_volatile<CIVP>::type, CIVP);
    CHECK_TYPEX(STD add_volatile<CICVP>::type, CICVP);
    CHECK_TYPEX(STD add_volatile<VIP>::type, VIVP);
    CHECK_TYPEX(STD add_volatile<VICP>::type, VICVP);
    CHECK_TYPEX(STD add_volatile<VIVP>::type, VIVP);
    CHECK_TYPEX(STD add_volatile<VICVP>::type, VICVP);
    CHECK_TYPEX(STD add_volatile<CVIP>::type, CVIVP);
    CHECK_TYPEX(STD add_volatile<CVICP>::type, CVICVP);
    CHECK_TYPEX(STD add_volatile<CVIVP>::type, CVIVP);
    CHECK_TYPEX(STD add_volatile<CVICVP>::type, CVICVP);
}

static void t_add_cv() { // test add_cv<T> for various types
    CHECK_TYPEX(STD add_cv<I>::type, CVI);
    CHECK_TYPEX(STD add_cv<CI>::type, CVI);
    CHECK_TYPEX(STD add_cv<VI>::type, CVI);
    CHECK_TYPEX(STD add_cv<CVI>::type, CVI);
    CHECK_TYPEX(STD add_cv<IP>::type, ICVP);
    CHECK_TYPEX(STD add_cv<ICP>::type, ICVP);
    CHECK_TYPEX(STD add_cv<IVP>::type, ICVP);
    CHECK_TYPEX(STD add_cv<ICVP>::type, ICVP);
    CHECK_TYPEX(STD add_cv<CIP>::type, CICVP);
    CHECK_TYPEX(STD add_cv<CICP>::type, CICVP);
    CHECK_TYPEX(STD add_cv<CIVP>::type, CICVP);
    CHECK_TYPEX(STD add_cv<CICVP>::type, CICVP);
    CHECK_TYPEX(STD add_cv<VIP>::type, VICVP);
    CHECK_TYPEX(STD add_cv<VICP>::type, VICVP);
    CHECK_TYPEX(STD add_cv<VIVP>::type, VICVP);
    CHECK_TYPEX(STD add_cv<VICVP>::type, VICVP);
    CHECK_TYPEX(STD add_cv<CVIP>::type, CVICVP);
    CHECK_TYPEX(STD add_cv<CVICP>::type, CVICVP);
    CHECK_TYPEX(STD add_cv<CVIVP>::type, CVICVP);
    CHECK_TYPEX(STD add_cv<CVICVP>::type, CVICVP);
}

static void t_remove_reference() { // test remove_reference<T> for various types
    CHECK_TYPEX(STD remove_reference<int>::type, int);
    CHECK_TYPEX(STD remove_reference<int&>::type, int);
}

static void t_add_lvalue_reference() { // test add_lvalue_reference<T> for various types
    CHECK_TYPEX(STD add_lvalue_reference<int>::type, int&);
    CHECK_TYPEX(STD add_lvalue_reference<int&>::type, int&);
    CHECK_TYPEX(STD add_lvalue_reference<ARR1>::type, ARR1&);
}

static void t_add_rvalue_reference() { // test add_rvalue_reference<T> for various types
    CHECK_TYPEX(STD add_rvalue_reference<int>::type, int&&);
    CHECK_TYPEX(STD add_rvalue_reference<int&>::type, int&);
    CHECK_TYPEX(STD add_rvalue_reference<ARR1>::type, ARR1 &&);
}

enum color : int { red, blue, green };

static void t_make_signed() { // test make_signed<T> for various types
    CHECK_TYPE(STD make_signed<signed char>::type, signed char);
    CHECK_TYPE(STD make_signed<unsigned char>::type, signed char);
    CHECK_TYPE(STD make_signed<signed short>::type, signed short);
    CHECK_TYPE(STD make_signed<unsigned short>::type, signed short);
    CHECK_TYPE(STD make_signed<signed int>::type, signed int);
    CHECK_TYPE(STD make_signed<unsigned int>::type, signed int);
    CHECK_TYPE(STD make_signed<signed long>::type, signed long);
    CHECK_TYPE(STD make_signed<unsigned long>::type, signed long);
    CHECK_TYPE(STD make_signed<long long>::type, long long);
    CHECK_TYPE(STD make_signed<unsigned long long>::type, long long);

    color neg = (color) (-3);
    CHECK_INT((STD make_signed<color>::type)(neg), neg);
}

static void t_make_unsigned() { // test make_unsigned<T> for various types
    CHECK_TYPE(STD make_unsigned<signed char>::type, unsigned char);
    CHECK_TYPE(STD make_unsigned<unsigned char>::type, unsigned char);
    CHECK_TYPE(STD make_unsigned<signed short>::type, unsigned short);
    CHECK_TYPE(STD make_unsigned<unsigned short>::type, unsigned short);
    CHECK_TYPE(STD make_unsigned<signed int>::type, unsigned int);
    CHECK_TYPE(STD make_unsigned<unsigned int>::type, unsigned int);
    CHECK_TYPE(STD make_unsigned<signed long>::type, unsigned long);
    CHECK_TYPE(STD make_unsigned<unsigned long>::type, unsigned long);
    CHECK_TYPE(STD make_unsigned<long long>::type, unsigned long long);
    CHECK_TYPE(STD make_unsigned<unsigned long long>::type, unsigned long long);

    color neg = (color) (-3);
    CHECK_INT((STD make_unsigned<color>::type)(neg), (unsigned int) (neg));
}

static void t_is_signed() { // test is_signed<T> for various types
    T_TEST_CV(is_signed, void, false);
    T_TEST_CV(is_signed, char, STD numeric_limits<char>::is_signed);
    T_TEST_CV(is_signed, signed char, true);
    T_TEST_CV(is_signed, unsigned char, false);
    T_TEST_CV(is_signed, wchar_t, STD numeric_limits<wchar_t>::is_signed);
    T_TEST_CV(is_signed, short, true);
    T_TEST_CV(is_signed, unsigned short, false);
    T_TEST_CV(is_signed, int, true);
    T_TEST_CV(is_signed, unsigned int, false);
    T_TEST_CV(is_signed, long, true);
    T_TEST_CV(is_signed, unsigned long, false);

    T_TEST_CV(is_signed, long long, true);
    T_TEST_CV(is_signed, unsigned long long, false);

    T_TEST_CV(is_signed, float, true);
    T_TEST_CV(is_signed, double, true);
    T_TEST_CV(is_signed, long double, true);

    T_TEST_CV(is_signed, int*, false);
    T_TEST(is_signed, int&, false);
    T_TEST_CV(is_signed, A, false);
    T_TEST_CV(is_signed, B, false);
    T_TEST_CV(is_signed, C, false);
    T_TEST_CV(is_signed, D, false);
    T_TEST_CV(is_signed, pmo, false);
    T_TEST_CV(is_signed, pmoc, false);
    T_TEST_CV(is_signed, pmov, false);
    T_TEST_CV(is_signed, pmocv, false);
    T_TEST_CV(is_signed, pmf, false);
    T_TEST_CV(is_signed, pmfc, false);
    T_TEST_CV(is_signed, pmfv, false);
    T_TEST_CV(is_signed, pmfcv, false);
    T_TEST_CV(is_signed, arr_t, false);
    T_TEST_CV(is_signed, enum_t, false);
    T_TEST_CV(is_signed, pf, false);
    T_TEST(is_signed, func, false);
    T_TEST_CV(is_signed, void*, false);
}

static void t_is_unsigned() { // test is_unsigned<T> for various types
    T_TEST_CV(is_unsigned, void, false);
    T_TEST_CV(is_unsigned, char, !STD numeric_limits<char>::is_signed);
    T_TEST_CV(is_unsigned, signed char, false);
    T_TEST_CV(is_unsigned, unsigned char, true);
    T_TEST_CV(is_unsigned, wchar_t, !STD numeric_limits<wchar_t>::is_signed);
    T_TEST_CV(is_unsigned, short, false);
    T_TEST_CV(is_unsigned, unsigned short, true);
    T_TEST_CV(is_unsigned, int, false);
    T_TEST_CV(is_unsigned, unsigned int, true);
    T_TEST_CV(is_unsigned, long, false);
    T_TEST_CV(is_unsigned, unsigned long, true);

    T_TEST_CV(is_unsigned, long long, false);
    T_TEST_CV(is_unsigned, unsigned long long, true);

    T_TEST_CV(is_unsigned, float, false);
    T_TEST_CV(is_unsigned, double, false);
    T_TEST_CV(is_unsigned, long double, false);
    T_TEST_CV(is_unsigned, int*, false);
    T_TEST(is_unsigned, int&, false);
    T_TEST_CV(is_unsigned, A, false);
    T_TEST_CV(is_unsigned, B, false);
    T_TEST_CV(is_unsigned, C, false);
    T_TEST_CV(is_unsigned, D, false);
    T_TEST_CV(is_unsigned, pmo, false);
    T_TEST_CV(is_signed, pmoc, false);
    T_TEST_CV(is_signed, pmov, false);
    T_TEST_CV(is_signed, pmocv, false);
    T_TEST_CV(is_signed, pmf, false);
    T_TEST_CV(is_signed, pmfc, false);
    T_TEST_CV(is_signed, pmfv, false);
    T_TEST_CV(is_signed, pmfcv, false);
    T_TEST_CV(is_signed, arr_t, false);
    T_TEST_CV(is_signed, enum_t, false);
    T_TEST_CV(is_signed, pf, false);
    T_TEST(is_unsigned, func, false);
    T_TEST_CV(is_unsigned, void*, false);
}

static void t_alignment_of() { // test alignment_of<T> for various types
    CHECK_INT(CHAR_ALIGN, STD alignment_of<char>::value);
    CHECK_INT(INT_ALIGN, STD alignment_of<int>::value);
    CHECK(PTR_ALIGN <= STD alignment_of<char*>::value);
    CHECK(LONG_ALIGN <= STD alignment_of<long>::value);
    CHECK(FLOAT_ALIGN <= STD alignment_of<float>::value);
    CHECK(DOUBLE_ALIGN <= STD alignment_of<double>::value);
    CHECK(LDOUBLE_ALIGN <= STD alignment_of<long double>::value);
}

static void t_rank() { // test rank<T> for various types
    CHECK_INT(STD rank<ARR0>::value, 0);
    CHECK_INT(STD rank<ARR1>::value, 1);
    CHECK_INT(STD rank<ARR2>::value, 2);
    CHECK_INT(STD rank<ARR3>::value, 3);
    CHECK_INT(STD rank<ARR4>::value, 4);
    CHECK_INT(STD rank<ARR5>::value, 5);
    CHECK_INT(STD rank<ARRN>::value, 6);
}

static void t_extent() { // test extent<T> for various types
    CHECK_INT(STD extent<ARR0>::value, 0);
    CHECK_INT(STD extent<ARR1>::value, 2);
    CHECK_INT(STD extent<ARR2>::value, 3);
    CHECK_INT(STD extent<ARR3>::value, 4);
    CHECK_INT(STD extent<ARR4>::value, 5);
    CHECK_INT(STD extent<ARR5>::value, 6);
    CHECK_INT(STD extent<ARRN>::value, 0);

    CHECK_INT((STD extent<ARR0, 0>::value), 0);
    CHECK_INT((STD extent<ARR1, 0>::value), 2);
    CHECK_INT((STD extent<ARR2, 0>::value), 3);
    CHECK_INT((STD extent<ARR3, 0>::value), 4);
    CHECK_INT((STD extent<ARR4, 0>::value), 5);
    CHECK_INT((STD extent<ARR5, 0>::value), 6);
    CHECK_INT((STD extent<ARRN, 0>::value), 0);

    CHECK_INT((STD extent<ARR0, 1>::value), 0);
    CHECK_INT((STD extent<ARR1, 1>::value), 0);
    CHECK_INT((STD extent<ARR2, 1>::value), 2);
    CHECK_INT((STD extent<ARR3, 1>::value), 3);
    CHECK_INT((STD extent<ARR4, 1>::value), 4);
    CHECK_INT((STD extent<ARR5, 1>::value), 5);
    CHECK_INT((STD extent<ARRN, 1>::value), 6);

    CHECK_INT((STD extent<ARR0, 2>::value), 0);
    CHECK_INT((STD extent<ARR1, 2>::value), 0);
    CHECK_INT((STD extent<ARR2, 2>::value), 0);
    CHECK_INT((STD extent<ARR3, 2>::value), 2);
    CHECK_INT((STD extent<ARR4, 2>::value), 3);
    CHECK_INT((STD extent<ARR5, 2>::value), 4);
    CHECK_INT((STD extent<ARRN, 2>::value), 5);

    CHECK_INT((STD extent<ARR0, 3>::value), 0);
    CHECK_INT((STD extent<ARR1, 3>::value), 0);
    CHECK_INT((STD extent<ARR2, 3>::value), 0);
    CHECK_INT((STD extent<ARR3, 3>::value), 0);
    CHECK_INT((STD extent<ARR4, 3>::value), 2);
    CHECK_INT((STD extent<ARR5, 3>::value), 3);
    CHECK_INT((STD extent<ARRN, 3>::value), 4);

    CHECK_INT((STD extent<ARR0, 4>::value), 0);
    CHECK_INT((STD extent<ARR1, 4>::value), 0);
    CHECK_INT((STD extent<ARR2, 4>::value), 0);
    CHECK_INT((STD extent<ARR3, 4>::value), 0);
    CHECK_INT((STD extent<ARR4, 4>::value), 0);
    CHECK_INT((STD extent<ARR5, 4>::value), 2);
    CHECK_INT((STD extent<ARRN, 4>::value), 3);

    CHECK_INT((STD extent<ARR0, 5>::value), 0);
    CHECK_INT((STD extent<ARR1, 5>::value), 0);
    CHECK_INT((STD extent<ARR2, 5>::value), 0);
    CHECK_INT((STD extent<ARR3, 5>::value), 0);
    CHECK_INT((STD extent<ARR4, 5>::value), 0);
    CHECK_INT((STD extent<ARR5, 5>::value), 0);
    CHECK_INT((STD extent<ARRN, 5>::value), 2);
}

static void t_remove_extent() { // test remove_extent<T> for various types
    CHECK_TYPE(STD remove_extent<ARR0>::type, ARR0);
    CHECK_TYPE(STD remove_extent<ARR1>::type, ARR0);
    CHECK_TYPE(STD remove_extent<ARR2>::type, ARR1);
    CHECK_TYPE(STD remove_extent<ARR3>::type, ARR2);
    CHECK_TYPE(STD remove_extent<ARR4>::type, ARR3);
    CHECK_TYPE(STD remove_extent<ARR5>::type, ARR4);
    CHECK_TYPE(STD remove_extent<ARRN>::type, ARR5);
}

static void t_remove_all_extents() { // test remove_all_extents<T> for various types
    CHECK_TYPE(STD remove_all_extents<ARR0>::type, ARR0);
    CHECK_TYPE(STD remove_all_extents<ARR1>::type, ARR0);
    CHECK_TYPE(STD remove_all_extents<ARR2>::type, ARR0);
    CHECK_TYPE(STD remove_all_extents<ARR3>::type, ARR0);
    CHECK_TYPE(STD remove_all_extents<ARR4>::type, ARR0);
    CHECK_TYPE(STD remove_all_extents<ARR5>::type, ARR0);
    CHECK_TYPE(STD remove_all_extents<ARRN>::type, ARR0);
}

static void t_remove_pointer() { // test remove_pointer<T> for various types
    CHECK_TYPEX(STD remove_pointer<int>::type, int);
    CHECK_TYPEX(STD remove_pointer<int*>::type, int);
    CHECK_TYPEX(STD remove_pointer<int**>::type, int*);
}

static void t_add_pointer() { // test add_pointer<T> for various types
    CHECK_TYPEX(STD add_pointer<int>::type, int*);
    CHECK_TYPEX(STD add_pointer<int*>::type, int**);
}

#define CHECK_STORAGE(len, align)                                                \
    {                                                                            \
        typedef STD alignment_of<STD aligned_storage<len, align>::type> ty;      \
        CHECK_INT(align <= ty::value, true);                                     \
        CHECK_INT((len <= sizeof(STD aligned_storage<len, align>::type)), true); \
    }

#define ALIGN1 (STD alignment_of<char>::value)
#define ALIGN2 (STD alignment_of<short>::value)
#define ALIGN3 (STD alignment_of<int>::value)
#define ALIGN4 (STD alignment_of<double>::value)

static void t_aligned_storage() { // test aligned_storage<Len, Align> for various types
    CHECK_STORAGE(1, ALIGN1);
    CHECK_STORAGE(1, ALIGN2);
    CHECK_STORAGE(1, ALIGN3);
    CHECK_STORAGE(1, ALIGN4);
    CHECK_STORAGE(2, ALIGN1);
    CHECK_STORAGE(2, ALIGN2);
    CHECK_STORAGE(2, ALIGN3);
    CHECK_STORAGE(2, ALIGN4);
    CHECK_STORAGE(4, ALIGN1);
    CHECK_STORAGE(4, ALIGN2);
    CHECK_STORAGE(4, ALIGN3);
    CHECK_STORAGE(4, ALIGN4);
    CHECK_STORAGE(100, ALIGN1);
    CHECK_STORAGE(100, ALIGN2);
    CHECK_STORAGE(100, ALIGN3);
    CHECK_STORAGE(100, ALIGN4);

    // test old non-conformant behavior for extended alignments
    {
        constexpr size_t x = alignof(max_align_t) << 1;
        static_assert(alignof(STD aligned_storage<x, x>::type) == alignof(max_align_t), "");
    }
    {
        constexpr size_t x = alignof(max_align_t) << 2;
        static_assert(alignof(STD aligned_storage<x, x>::type) == alignof(max_align_t), "");
    }
    {
        constexpr size_t x = alignof(max_align_t) << 3;
        static_assert(alignof(STD aligned_storage<x, x>::type) == alignof(max_align_t), "");
    }
    {
        constexpr size_t x = alignof(max_align_t) << 5;
        static_assert(alignof(STD aligned_storage<x, x>::type) == alignof(max_align_t), "");
    }
}

#define CHECK_UNION(len, align, ty1, ty2)                                         \
    {                                                                             \
        typedef STD alignment_of<STD aligned_union<len, ty1, ty2>::type> ty;      \
        CHECK_INT(align, ty::value);                                              \
        CHECK_INT((len <= sizeof(STD aligned_union<len, ty1, ty2>::type)), true); \
    }

static void t_aligned_union() { // test aligned_storage<Len, Align> for various types
    CHECK_UNION(1, ALIGN1, char, char);
    CHECK_UNION(1, ALIGN2, char, short);
    CHECK_UNION(1, ALIGN2, short, char);
    CHECK_UNION(100, ALIGN4, int, double);

    typedef STD aligned_union<1, char>::type au_c;
    typedef STD alignment_of<au_c> ty_c;
    CHECK_INT(ALIGN1, ty_c::value);
    CHECK_INT((1 <= sizeof(au_c)), true);

    typedef STD aligned_union<1, char, short>::type au_cs;
    typedef STD alignment_of<au_cs> ty_cs;
    CHECK_INT(ALIGN2, ty_cs::value);
    CHECK_INT((2 <= sizeof(au_cs)), true);

    typedef STD aligned_union<1, char, short, int>::type au_csi;
    typedef STD alignment_of<au_csi> ty_csi;
    CHECK_INT(ALIGN3, ty_csi::value);
    CHECK_INT((4 <= sizeof(au_csi)), true);

    typedef STD aligned_union<1, char, short, int, short>::type au_csis;
    typedef STD alignment_of<au_csis> ty_csis;
    CHECK_INT(ALIGN3, ty_csis::value);
    CHECK_INT((4 <= sizeof(au_csis)), true);

    typedef STD aligned_union<1, char, short, int, short, char>::type au_csisc;
    typedef STD alignment_of<au_csisc> ty_csisc;
    CHECK_INT(ALIGN3, ty_csisc::value);
    CHECK_INT((4 <= sizeof(au_csisc)), true);
}

void t_utility() { // test utility template classes
    char* pc  = (STD enable_if<true, char>::type*) nullptr;
    short* ps = (STD conditional<true, short, int>::type*) nullptr;
    int* pi   = (STD conditional<false, short, int>::type*) nullptr;

    pc = pc; // to quiet diagnostics
    ps = ps;
    pi = pi;

    CHECK_TYPE(STD decay<char>::type, char);
    CHECK_TYPE(STD decay<short[4]>::type, short*);

    typedef void fn(int);
    CHECK_TYPE(STD decay<fn>::type, fn*);

    typedef STD common_type<int>::type ci;
    CHECK_TYPE(ci, int);
    typedef STD common_type<int, int>::type cii;
    CHECK_TYPE(cii, int);
    typedef STD common_type<short, int>::type csi;
    CHECK_TYPE(csi, int);
    typedef STD common_type<short, float>::type csf;
    CHECK_TYPE(csf, float);
    typedef STD common_type<short, float, double>::type csfd;
    CHECK_TYPE(csfd, double);
    typedef STD common_type<short, float, int, short>::type csfis;
    CHECK_TYPE(csfis, float);
    typedef STD common_type<short, float, int, short, int>::type csfisi;
    CHECK_TYPE(csfisi, float);
}

typedef enum { a, b, c = INT_MAX } enum1;

static void t_underlying_type() { // test add_lvalue_reference<T> for various types
    CHECK_TYPE(STD underlying_type<enum1>::type, int);
}

void test_main() { // test type traits
    t_remove_const();
    t_remove_volatile();
    t_remove_cv();
    t_add_const();
    t_add_volatile();
    t_add_cv();
    t_remove_reference();

    t_add_lvalue_reference();
    t_add_rvalue_reference();
    t_make_signed();
    t_make_unsigned();

    t_is_signed();
    t_is_unsigned();
    t_alignment_of();
    t_rank();
    t_extent();
    t_remove_extent();
    t_remove_all_extents();
    t_remove_pointer();
    t_add_pointer();
    t_aligned_storage();

    t_aligned_union();
    t_utility();
    t_underlying_type();
}
