// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _HAS_DEPRECATED_IS_LITERAL_TYPE 1
#define _SILENCE_CXX17_IS_LITERAL_TYPE_DEPRECATION_WARNING
#define _SILENCE_CXX20_IS_POD_DEPRECATION_WARNING

#include <functional>
#include <type_traits>
#include <utility>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

#define VERIFY_BASE_CHARACTERISTIC_V(BASE, ...)       \
    STATIC_ASSERT(BASE::value == __VA_ARGS__::value); \
    STATIC_ASSERT(is_base_of_v<BASE, __VA_ARGS__>)

#define VERIFY_BASE_CHARACTERISTIC(BASE, ...)        \
    VERIFY_BASE_CHARACTERISTIC_V(BASE, __VA_ARGS__); \
    STATIC_ASSERT(is_same_v<BASE::type, __VA_ARGS__::type>)

// Regression test for:
// DevDiv-387795:  is_pod<void> should be false
// DevDiv-424157:  is_assignable/is_trivially_assignable/is_trivially_move_assignable for void

// N3376 20.9.4.1[meta.unary.cat]:  primary type categories:
STATIC_ASSERT(is_void_v<void>);
STATIC_ASSERT(!is_integral_v<void>);
STATIC_ASSERT(!is_floating_point_v<void>);
STATIC_ASSERT(!is_array_v<void>);
STATIC_ASSERT(!is_pointer_v<void>);
STATIC_ASSERT(!is_lvalue_reference_v<void>);
STATIC_ASSERT(!is_rvalue_reference_v<void>);
STATIC_ASSERT(!is_member_object_pointer_v<void>);
STATIC_ASSERT(!is_member_function_pointer_v<void>);
STATIC_ASSERT(!is_enum_v<void>);
STATIC_ASSERT(!is_union_v<void>);
STATIC_ASSERT(!is_class_v<void>);
STATIC_ASSERT(!is_function_v<void>);

// N3376 20.9.4.2[meta.unary.comp]:  composite type categories:
STATIC_ASSERT(!is_reference_v<void>);
STATIC_ASSERT(!is_arithmetic_v<void>);
STATIC_ASSERT(is_fundamental_v<void>);
STATIC_ASSERT(!is_object_v<void>);
STATIC_ASSERT(!is_scalar_v<void>);
STATIC_ASSERT(!is_compound_v<void>);
STATIC_ASSERT(!is_member_pointer_v<void>);

// N3376 20.9.4.3[meta.unary.prop]:  type properties:
STATIC_ASSERT(!is_const_v<void>);
STATIC_ASSERT(is_const_v<const void>);
STATIC_ASSERT(!is_const_v<volatile void>);
STATIC_ASSERT(is_const_v<const volatile void>);
STATIC_ASSERT(!is_volatile_v<void>);
STATIC_ASSERT(!is_volatile_v<const void>);
STATIC_ASSERT(is_volatile_v<volatile void>);
STATIC_ASSERT(is_volatile_v<const volatile void>);
STATIC_ASSERT(!is_trivial_v<void>);
STATIC_ASSERT(!is_trivially_copyable_v<void>);
STATIC_ASSERT(!is_standard_layout_v<void>);
STATIC_ASSERT(!is_pod_v<void>);

STATIC_ASSERT(is_literal_type_v<void>);
STATIC_ASSERT(is_literal_type_v<const void>);
STATIC_ASSERT(is_literal_type_v<volatile void>);
STATIC_ASSERT(is_literal_type_v<const volatile void>);

STATIC_ASSERT(!is_empty_v<void>);
STATIC_ASSERT(!is_polymorphic_v<void>);
STATIC_ASSERT(!is_abstract_v<void>);
STATIC_ASSERT(!is_signed_v<void>);
STATIC_ASSERT(!is_unsigned_v<void>);
#if _HAS_CXX20
STATIC_ASSERT(!is_bounded_array_v<void>);
STATIC_ASSERT(!is_unbounded_array_v<void>);
#endif // _HAS_CXX20
#if _HAS_CXX23
STATIC_ASSERT(!is_scoped_enum_v<void>);
#endif // _HAS_CXX23

STATIC_ASSERT(!is_constructible_v<void>);
STATIC_ASSERT(!is_default_constructible_v<void>);
STATIC_ASSERT(!is_copy_constructible_v<void>);
STATIC_ASSERT(!is_move_constructible_v<void>);
STATIC_ASSERT(!is_assignable_v<void, void>);
STATIC_ASSERT(!is_copy_assignable_v<void>);
STATIC_ASSERT(!is_move_assignable_v<void>);
STATIC_ASSERT(!is_destructible_v<void>);
STATIC_ASSERT(!is_destructible_v<const void>);
STATIC_ASSERT(!is_destructible_v<volatile void>);
STATIC_ASSERT(!is_destructible_v<const volatile void>);

STATIC_ASSERT(!is_trivially_constructible_v<void>);
STATIC_ASSERT(!is_trivially_default_constructible_v<void>);
STATIC_ASSERT(!is_trivially_copy_constructible_v<void>);
STATIC_ASSERT(!is_trivially_move_constructible_v<void>);
STATIC_ASSERT(!is_trivially_assignable_v<void, void>);
STATIC_ASSERT(!is_trivially_copy_assignable_v<void>);
STATIC_ASSERT(!is_trivially_move_assignable_v<void>);
STATIC_ASSERT(!is_trivially_destructible_v<void>);
STATIC_ASSERT(!is_trivially_destructible_v<const void>);
STATIC_ASSERT(!is_trivially_destructible_v<volatile void>);
STATIC_ASSERT(!is_trivially_destructible_v<const volatile void>);

STATIC_ASSERT(!is_nothrow_constructible_v<void>);
STATIC_ASSERT(!is_nothrow_default_constructible_v<void>);
STATIC_ASSERT(!is_nothrow_copy_constructible_v<void>);
STATIC_ASSERT(!is_nothrow_move_constructible_v<void>);
STATIC_ASSERT(!is_nothrow_assignable_v<void, void>);
STATIC_ASSERT(!is_nothrow_copy_assignable_v<void>);
STATIC_ASSERT(!is_nothrow_move_assignable_v<void>);
STATIC_ASSERT(!is_nothrow_destructible_v<void>);
STATIC_ASSERT(!is_nothrow_destructible_v<const void>);
STATIC_ASSERT(!is_nothrow_destructible_v<volatile void>);
STATIC_ASSERT(!is_nothrow_destructible_v<const volatile void>);

STATIC_ASSERT(!has_virtual_destructor_v<void>);

// N3376 20.9.5[meta.unary.prop.query]:  type property queries
// alignment_of does not accept void
STATIC_ASSERT(rank_v<void> == 0);
STATIC_ASSERT(extent_v<void> == 0);
STATIC_ASSERT(extent_v<void, 1> == 0);

// N3376 20.9.6[meta.rel]:  type relations
STATIC_ASSERT(is_same_v<void, void>);
STATIC_ASSERT(is_same_v<const void, const void>);
STATIC_ASSERT(is_same_v<volatile void, volatile void>);
STATIC_ASSERT(is_same_v<const volatile void, const volatile void>);
STATIC_ASSERT(!is_same_v<const void, void>);
STATIC_ASSERT(!is_same_v<void, const void>);

class C {};
STATIC_ASSERT(!is_base_of_v<void, C>);
STATIC_ASSERT(!is_base_of_v<C, void>);
STATIC_ASSERT(!is_base_of_v<void, void>);

STATIC_ASSERT(is_convertible_v<void, void>);
STATIC_ASSERT(!is_convertible_v<int, void>);
STATIC_ASSERT(!is_convertible_v<void, int>);

// N3376 20.9.7.1[meta.trans.cv]:  const-volatile modifications
STATIC_ASSERT(is_same_v<void, remove_const_t<void>>);
STATIC_ASSERT(is_same_v<void, remove_const_t<const void>>);
STATIC_ASSERT(is_same_v<volatile void, remove_const_t<volatile void>>);
STATIC_ASSERT(is_same_v<volatile void, remove_const_t<const volatile void>>);

STATIC_ASSERT(is_same_v<void, remove_volatile_t<void>>);
STATIC_ASSERT(is_same_v<const void, remove_volatile_t<const void>>);
STATIC_ASSERT(is_same_v<void, remove_volatile_t<volatile void>>);
STATIC_ASSERT(is_same_v<const void, remove_volatile_t<const volatile void>>);

STATIC_ASSERT(is_same_v<void, remove_cv_t<void>>);
STATIC_ASSERT(is_same_v<void, remove_cv_t<const void>>);
STATIC_ASSERT(is_same_v<void, remove_cv_t<volatile void>>);
STATIC_ASSERT(is_same_v<void, remove_cv_t<const volatile void>>);

STATIC_ASSERT(is_same_v<const void, add_const_t<void>>);
STATIC_ASSERT(is_same_v<const void, add_const_t<const void>>);
STATIC_ASSERT(is_same_v<const volatile void, add_const_t<volatile void>>);
STATIC_ASSERT(is_same_v<const volatile void, add_const_t<const volatile void>>);

STATIC_ASSERT(is_same_v<volatile void, add_volatile_t<void>>);
STATIC_ASSERT(is_same_v<const volatile void, add_volatile_t<const void>>);
STATIC_ASSERT(is_same_v<volatile void, add_volatile_t<volatile void>>);
STATIC_ASSERT(is_same_v<const volatile void, add_volatile_t<const volatile void>>);

STATIC_ASSERT(is_same_v<const volatile void, add_cv_t<void>>);
STATIC_ASSERT(is_same_v<const volatile void, add_cv_t<const void>>);
STATIC_ASSERT(is_same_v<const volatile void, add_cv_t<volatile void>>);
STATIC_ASSERT(is_same_v<const volatile void, add_cv_t<const volatile void>>);

// N3376 20.9.7.2[meta.trans.ref]:  reference modifications
STATIC_ASSERT(is_same_v<void, remove_reference_t<void>>);
STATIC_ASSERT(is_same_v<void, add_lvalue_reference_t<void>>);
STATIC_ASSERT(is_same_v<void, add_rvalue_reference_t<void>>);

// N3376 20.9.7.3[meta.trans.sign]:  sign modifications
// Neither make_signed nor make_unsigned accept 'void'

// N3376 20.9.7.4[meta.trans.arr]:  array modifications:
STATIC_ASSERT(is_same_v<void, remove_extent_t<void>>);
STATIC_ASSERT(is_same_v<void, remove_all_extents_t<void>>);

// N3376 20.9.7.5[meta.trans.ptr]:  pointer modifications:
STATIC_ASSERT(is_same_v<void, remove_pointer_t<void>>);
STATIC_ASSERT(is_same_v<void*, add_pointer_t<void>>);

// N3376 20.9.7.6[meta.trans.other]:  other transformations
// aligned_union, underlying_type, and result_of do not accept void
STATIC_ASSERT(is_same_v<void, decay_t<void>>);

STATIC_ASSERT(is_same_v<void, common_type_t<void, void>>);

// DDB.171837
STATIC_ASSERT(!is_const_v<int[3]>);
STATIC_ASSERT(is_const_v<const int[3]>);
STATIC_ASSERT(!is_const_v<volatile int[3]>);
STATIC_ASSERT(is_const_v<const volatile int[3]>);

STATIC_ASSERT(!is_const_v<int[]>);
STATIC_ASSERT(is_const_v<const int[]>);
STATIC_ASSERT(!is_const_v<volatile int[]>);
STATIC_ASSERT(is_const_v<const volatile int[]>);

STATIC_ASSERT(!is_volatile_v<int[3]>);
STATIC_ASSERT(!is_volatile_v<const int[3]>);
STATIC_ASSERT(is_volatile_v<volatile int[3]>);
STATIC_ASSERT(is_volatile_v<const volatile int[3]>);

STATIC_ASSERT(!is_volatile_v<int[]>);
STATIC_ASSERT(!is_volatile_v<const int[]>);
STATIC_ASSERT(is_volatile_v<volatile int[]>);
STATIC_ASSERT(is_volatile_v<const volatile int[]>);


STATIC_ASSERT(is_same_v<int[3], remove_const_t<int[3]>>);
STATIC_ASSERT(is_same_v<int[3], remove_const_t<const int[3]>>);
STATIC_ASSERT(is_same_v<volatile int[3], remove_const_t<volatile int[3]>>);
STATIC_ASSERT(is_same_v<volatile int[3], remove_const_t<const volatile int[3]>>);

STATIC_ASSERT(is_same_v<int[3], remove_volatile_t<int[3]>>);
STATIC_ASSERT(is_same_v<const int[3], remove_volatile_t<const int[3]>>);
STATIC_ASSERT(is_same_v<int[3], remove_volatile_t<volatile int[3]>>);
STATIC_ASSERT(is_same_v<const int[3], remove_volatile_t<const volatile int[3]>>);

STATIC_ASSERT(is_same_v<int[3], remove_cv_t<int[3]>>);
STATIC_ASSERT(is_same_v<int[3], remove_cv_t<const int[3]>>);
STATIC_ASSERT(is_same_v<int[3], remove_cv_t<volatile int[3]>>);
STATIC_ASSERT(is_same_v<int[3], remove_cv_t<const volatile int[3]>>);

STATIC_ASSERT(is_same_v<const int[3], add_const_t<int[3]>>);
STATIC_ASSERT(is_same_v<const int[3], add_const_t<const int[3]>>);
STATIC_ASSERT(is_same_v<const volatile int[3], add_const_t<volatile int[3]>>);
STATIC_ASSERT(is_same_v<const volatile int[3], add_const_t<const volatile int[3]>>);

STATIC_ASSERT(is_same_v<volatile int[3], add_volatile_t<int[3]>>);
STATIC_ASSERT(is_same_v<const volatile int[3], add_volatile_t<const int[3]>>);
STATIC_ASSERT(is_same_v<volatile int[3], add_volatile_t<volatile int[3]>>);
STATIC_ASSERT(is_same_v<const volatile int[3], add_volatile_t<const volatile int[3]>>);

STATIC_ASSERT(is_same_v<const volatile int[3], add_cv_t<int[3]>>);
STATIC_ASSERT(is_same_v<const volatile int[3], add_cv_t<const int[3]>>);
STATIC_ASSERT(is_same_v<const volatile int[3], add_cv_t<volatile int[3]>>);
STATIC_ASSERT(is_same_v<const volatile int[3], add_cv_t<const volatile int[3]>>);


STATIC_ASSERT(is_same_v<int[], remove_const_t<int[]>>);
STATIC_ASSERT(is_same_v<int[], remove_const_t<const int[]>>);
STATIC_ASSERT(is_same_v<volatile int[], remove_const_t<volatile int[]>>);
STATIC_ASSERT(is_same_v<volatile int[], remove_const_t<const volatile int[]>>);

STATIC_ASSERT(is_same_v<int[], remove_volatile_t<int[]>>);
STATIC_ASSERT(is_same_v<const int[], remove_volatile_t<const int[]>>);
STATIC_ASSERT(is_same_v<int[], remove_volatile_t<volatile int[]>>);
STATIC_ASSERT(is_same_v<const int[], remove_volatile_t<const volatile int[]>>);

STATIC_ASSERT(is_same_v<int[], remove_cv_t<int[]>>);
STATIC_ASSERT(is_same_v<int[], remove_cv_t<const int[]>>);
STATIC_ASSERT(is_same_v<int[], remove_cv_t<volatile int[]>>);
STATIC_ASSERT(is_same_v<int[], remove_cv_t<const volatile int[]>>);

STATIC_ASSERT(is_same_v<const int[], add_const_t<int[]>>);
STATIC_ASSERT(is_same_v<const int[], add_const_t<const int[]>>);
STATIC_ASSERT(is_same_v<const volatile int[], add_const_t<volatile int[]>>);
STATIC_ASSERT(is_same_v<const volatile int[], add_const_t<const volatile int[]>>);

STATIC_ASSERT(is_same_v<volatile int[], add_volatile_t<int[]>>);
STATIC_ASSERT(is_same_v<const volatile int[], add_volatile_t<const int[]>>);
STATIC_ASSERT(is_same_v<volatile int[], add_volatile_t<volatile int[]>>);
STATIC_ASSERT(is_same_v<const volatile int[], add_volatile_t<const volatile int[]>>);

STATIC_ASSERT(is_same_v<const volatile int[], add_cv_t<int[]>>);
STATIC_ASSERT(is_same_v<const volatile int[], add_cv_t<const int[]>>);
STATIC_ASSERT(is_same_v<const volatile int[], add_cv_t<volatile int[]>>);
STATIC_ASSERT(is_same_v<const volatile int[], add_cv_t<const volatile int[]>>);


// DDB.172152
STATIC_ASSERT(is_array_v<int[3]>);
STATIC_ASSERT(is_array_v<const int[3]>);
STATIC_ASSERT(is_array_v<volatile int[3]>);
STATIC_ASSERT(is_array_v<const volatile int[3]>);

STATIC_ASSERT(is_array_v<int[]>);
STATIC_ASSERT(is_array_v<const int[]>);
STATIC_ASSERT(is_array_v<volatile int[]>);
STATIC_ASSERT(is_array_v<const volatile int[]>);

// lvalue/rvalue references aren't arrays.
STATIC_ASSERT(!is_array_v<int (&)[3]>);
STATIC_ASSERT(!is_array_v<int (&)[]>);
STATIC_ASSERT(!is_array_v<int (&&)[3]>);
STATIC_ASSERT(!is_array_v<int (&&)[]>);

#if _HAS_CXX20
STATIC_ASSERT(is_bounded_array_v<int[3]>);
STATIC_ASSERT(is_bounded_array_v<const int[3]>);
STATIC_ASSERT(is_bounded_array_v<volatile int[3]>);
STATIC_ASSERT(is_bounded_array_v<const volatile int[3]>);

STATIC_ASSERT(!is_bounded_array_v<int[]>);
STATIC_ASSERT(!is_bounded_array_v<const int[]>);
STATIC_ASSERT(!is_bounded_array_v<volatile int[]>);
STATIC_ASSERT(!is_bounded_array_v<const volatile int[]>);

STATIC_ASSERT(!is_unbounded_array_v<int[3]>);
STATIC_ASSERT(!is_unbounded_array_v<const int[3]>);
STATIC_ASSERT(!is_unbounded_array_v<volatile int[3]>);
STATIC_ASSERT(!is_unbounded_array_v<const volatile int[3]>);

STATIC_ASSERT(is_unbounded_array_v<int[]>);
STATIC_ASSERT(is_unbounded_array_v<const int[]>);
STATIC_ASSERT(is_unbounded_array_v<volatile int[]>);
STATIC_ASSERT(is_unbounded_array_v<const volatile int[]>);

// lvalue/rvalue references aren't bounded/unbounded arrays.
STATIC_ASSERT(!is_bounded_array_v<int (&)[3]>);
STATIC_ASSERT(!is_bounded_array_v<int (&)[]>);
STATIC_ASSERT(!is_bounded_array_v<int (&&)[3]>);
STATIC_ASSERT(!is_bounded_array_v<int (&&)[]>);
STATIC_ASSERT(!is_unbounded_array_v<int (&)[3]>);
STATIC_ASSERT(!is_unbounded_array_v<int (&)[]>);
STATIC_ASSERT(!is_unbounded_array_v<int (&&)[3]>);
STATIC_ASSERT(!is_unbounded_array_v<int (&&)[]>);
#endif // _HAS_CXX20


// DDB.172154
STATIC_ASSERT(is_same_v<void, add_lvalue_reference_t<void>>);
STATIC_ASSERT(is_same_v<const void, add_lvalue_reference_t<const void>>);
STATIC_ASSERT(is_same_v<volatile void, add_lvalue_reference_t<volatile void>>);
STATIC_ASSERT(is_same_v<const volatile void, add_lvalue_reference_t<const volatile void>>);


// DDB.172355
STATIC_ASSERT(is_convertible_v<void, void>);
STATIC_ASSERT(is_convertible_v<void, const void>);
STATIC_ASSERT(is_convertible_v<void, volatile void>);
STATIC_ASSERT(is_convertible_v<void, const volatile void>);

STATIC_ASSERT(is_convertible_v<const void, void>);
STATIC_ASSERT(is_convertible_v<const void, const void>);
STATIC_ASSERT(is_convertible_v<const void, volatile void>);
STATIC_ASSERT(is_convertible_v<const void, const volatile void>);

STATIC_ASSERT(is_convertible_v<volatile void, void>);
STATIC_ASSERT(is_convertible_v<volatile void, const void>);
STATIC_ASSERT(is_convertible_v<volatile void, volatile void>);
STATIC_ASSERT(is_convertible_v<volatile void, const volatile void>);

STATIC_ASSERT(is_convertible_v<const volatile void, void>);
STATIC_ASSERT(is_convertible_v<const volatile void, const void>);
STATIC_ASSERT(is_convertible_v<const volatile void, volatile void>);
STATIC_ASSERT(is_convertible_v<const volatile void, const volatile void>);

STATIC_ASSERT(!is_convertible_v<int, void>);
STATIC_ASSERT(!is_convertible_v<int, const void>);
STATIC_ASSERT(!is_convertible_v<int, volatile void>);
STATIC_ASSERT(!is_convertible_v<int, const volatile void>);

STATIC_ASSERT(!is_convertible_v<void, int>);
STATIC_ASSERT(!is_convertible_v<const void, int>);
STATIC_ASSERT(!is_convertible_v<volatile void, int>);
STATIC_ASSERT(!is_convertible_v<const volatile void, int>);

// Verify simplifications after DevDiv-1195735 "C1XX should select IsConst<const T> for IsConst<const int[3]>" was
// fixed.
STATIC_ASSERT(!is_const_v<int>);
STATIC_ASSERT(!is_const_v<int[3]>);
STATIC_ASSERT(!is_const_v<int[]>);
STATIC_ASSERT(is_const_v<const int>);
STATIC_ASSERT(is_const_v<const int[3]>);
STATIC_ASSERT(is_const_v<const int[]>);
STATIC_ASSERT(!is_const_v<int&>);
STATIC_ASSERT(!is_const_v<int&&>);
STATIC_ASSERT(!is_const_v<const int&>);
STATIC_ASSERT(!is_const_v<const int&&>);
STATIC_ASSERT(!is_const_v<int(int)>);

STATIC_ASSERT(is_same_v<int, remove_const_t<int>>);
STATIC_ASSERT(is_same_v<int[3], remove_const_t<int[3]>>);
STATIC_ASSERT(is_same_v<int[], remove_const_t<int[]>>);
STATIC_ASSERT(is_same_v<int, remove_const_t<const int>>);
STATIC_ASSERT(is_same_v<int[3], remove_const_t<const int[3]>>);
STATIC_ASSERT(is_same_v<int[], remove_const_t<const int[]>>);
STATIC_ASSERT(is_same_v<int&, remove_const_t<int&>>);
STATIC_ASSERT(is_same_v<int&&, remove_const_t<int&&>>);
STATIC_ASSERT(is_same_v<const int&, remove_const_t<const int&>>);
STATIC_ASSERT(is_same_v<const int&&, remove_const_t<const int&&>>);
STATIC_ASSERT(is_same_v<int(int), remove_const_t<int(int)>>);

STATIC_ASSERT(!is_volatile_v<int>);
STATIC_ASSERT(!is_volatile_v<int[3]>);
STATIC_ASSERT(!is_volatile_v<int[]>);
STATIC_ASSERT(is_volatile_v<volatile int>);
STATIC_ASSERT(is_volatile_v<volatile int[3]>);
STATIC_ASSERT(is_volatile_v<volatile int[]>);
STATIC_ASSERT(!is_volatile_v<int&>);
STATIC_ASSERT(!is_volatile_v<int&&>);
STATIC_ASSERT(!is_volatile_v<volatile int&>);
STATIC_ASSERT(!is_volatile_v<volatile int&&>);
STATIC_ASSERT(!is_volatile_v<int(int)>);

STATIC_ASSERT(is_same_v<int, remove_volatile_t<int>>);
STATIC_ASSERT(is_same_v<int[3], remove_volatile_t<int[3]>>);
STATIC_ASSERT(is_same_v<int[], remove_volatile_t<int[]>>);
STATIC_ASSERT(is_same_v<int, remove_volatile_t<volatile int>>);
STATIC_ASSERT(is_same_v<int[3], remove_volatile_t<volatile int[3]>>);
STATIC_ASSERT(is_same_v<int[], remove_volatile_t<volatile int[]>>);
STATIC_ASSERT(is_same_v<int&, remove_volatile_t<int&>>);
STATIC_ASSERT(is_same_v<int&&, remove_volatile_t<int&&>>);
STATIC_ASSERT(is_same_v<volatile int&, remove_volatile_t<volatile int&>>);
STATIC_ASSERT(is_same_v<volatile int&&, remove_volatile_t<volatile int&&>>);
STATIC_ASSERT(is_same_v<int(int), remove_volatile_t<int(int)>>);

// Verify is_pointer simplification.
using FP  = int (*)(int);
using PMF = int (C::*)(int);
using PMD = int C::*;
STATIC_ASSERT(!is_pointer_v<double>);
STATIC_ASSERT(!is_pointer_v<const double>);
STATIC_ASSERT(!is_pointer_v<volatile double>);
STATIC_ASSERT(!is_pointer_v<const volatile double>);
STATIC_ASSERT(is_pointer_v<int*>);
STATIC_ASSERT(is_pointer_v<int* const>);
STATIC_ASSERT(is_pointer_v<int* volatile>);
STATIC_ASSERT(is_pointer_v<int* const volatile>);
STATIC_ASSERT(is_pointer_v<void*>);
STATIC_ASSERT(is_pointer_v<void* const>);
STATIC_ASSERT(is_pointer_v<void* volatile>);
STATIC_ASSERT(is_pointer_v<void* const volatile>);
STATIC_ASSERT(is_pointer_v<FP>);
STATIC_ASSERT(is_pointer_v<const FP>);
STATIC_ASSERT(is_pointer_v<volatile FP>);
STATIC_ASSERT(is_pointer_v<const volatile FP>);
STATIC_ASSERT(!is_pointer_v<PMF>);
STATIC_ASSERT(!is_pointer_v<const PMF>);
STATIC_ASSERT(!is_pointer_v<volatile PMF>);
STATIC_ASSERT(!is_pointer_v<const volatile PMF>);
STATIC_ASSERT(!is_pointer_v<PMD>);
STATIC_ASSERT(!is_pointer_v<const PMD>);
STATIC_ASSERT(!is_pointer_v<volatile PMD>);
STATIC_ASSERT(!is_pointer_v<const volatile PMD>);

// devdiv198042_tr1_type_traits_is_function
struct test_abc1 {
    test_abc1();
    virtual ~test_abc1();
    test_abc1(const test_abc1&);
    test_abc1& operator=(const test_abc1&);
    virtual void meow()  = 0;
    virtual void meow2() = 0;
};

typedef void meow5_t(int, bool, int*, int[], int, int, int, int, int, ...);
STATIC_ASSERT(is_function_v<meow5_t>);

typedef void (test_abc1::*vproc1)(...);
STATIC_ASSERT(is_member_function_pointer_v<vproc1>);

// <P0013R1> Logical Operator Type Traits
template <bool val>
struct fake_bool {
    static const bool value = val;
};

template <bool val>
struct final_fake_bool final : fake_bool<val> {};

//   <conjunction>
// basic true cases
VERIFY_BASE_CHARACTERISTIC(true_type, conjunction<>);
VERIFY_BASE_CHARACTERISTIC(true_type, conjunction<true_type>);
VERIFY_BASE_CHARACTERISTIC(true_type, conjunction<true_type, true_type>);
VERIFY_BASE_CHARACTERISTIC(true_type, conjunction<true_type, true_type, true_type>);
// more complex true cases
VERIFY_BASE_CHARACTERISTIC(is_void<void>, conjunction<is_void<void>>);
VERIFY_BASE_CHARACTERISTIC(is_void<void>, conjunction<is_const<const void>, is_void<void>>);
VERIFY_BASE_CHARACTERISTIC(is_void<void>, conjunction<is_const<const void>, is_volatile<volatile void>, is_void<void>>);
// basic false cases
VERIFY_BASE_CHARACTERISTIC(false_type, conjunction<false_type>);
VERIFY_BASE_CHARACTERISTIC(false_type, conjunction<true_type, false_type>);
VERIFY_BASE_CHARACTERISTIC(false_type, conjunction<true_type, true_type, false_type>);
// more complex false cases, selecting last trait
VERIFY_BASE_CHARACTERISTIC(is_void<int>, conjunction<is_void<int>>);
VERIFY_BASE_CHARACTERISTIC(is_void<int>, conjunction<is_const<const int>, is_void<int>>);
VERIFY_BASE_CHARACTERISTIC(is_void<int>, conjunction<is_const<const int>, is_volatile<volatile int>, is_void<int>>);
// more complex false cases, selecting not-the-last trait
VERIFY_BASE_CHARACTERISTIC(is_void<int>, conjunction<is_void<int>, is_const<const int>, is_volatile<volatile int>>);
VERIFY_BASE_CHARACTERISTIC(is_void<int>, conjunction<is_const<const int>, is_void<int>, is_volatile<volatile int>>);
// short circuiting
VERIFY_BASE_CHARACTERISTIC(is_void<int>, conjunction<is_void<int>, void>);
VERIFY_BASE_CHARACTERISTIC(is_volatile<int>, conjunction<is_const<const int>, is_volatile<int>, void>);
VERIFY_BASE_CHARACTERISTIC(is_const<int>, conjunction<is_const<int>, void, void>);
VERIFY_BASE_CHARACTERISTIC(is_const<int>, conjunction<is_const<int>, void, void, void>);
// non-integral_constant family
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<true>, conjunction<fake_bool<true>>);
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<true>, conjunction<fake_bool<true>, fake_bool<true>>);
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<true>, conjunction<fake_bool<true>, fake_bool<true>, fake_bool<true>>);
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<false>, conjunction<fake_bool<false>>);
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<false>, conjunction<fake_bool<false>, fake_bool<true>>);
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<false>, conjunction<fake_bool<true>, fake_bool<false>>);
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<false>, conjunction<fake_bool<true>, fake_bool<true>, fake_bool<false>>);
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<false>, conjunction<fake_bool<true>, fake_bool<false>, fake_bool<true>>);
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<false>, conjunction<fake_bool<false>, fake_bool<true>, fake_bool<true>>);
//   </conjunction>
//   <disjunction>
// basic false cases
VERIFY_BASE_CHARACTERISTIC(false_type, disjunction<>);
VERIFY_BASE_CHARACTERISTIC(false_type, disjunction<false_type>);
VERIFY_BASE_CHARACTERISTIC(false_type, disjunction<false_type, false_type>);
VERIFY_BASE_CHARACTERISTIC(false_type, disjunction<false_type, false_type, false_type>);
// more complex false cases
VERIFY_BASE_CHARACTERISTIC(is_void<int>, disjunction<is_void<int>>);
VERIFY_BASE_CHARACTERISTIC(is_void<int>, disjunction<is_const<int>, is_void<int>>);
VERIFY_BASE_CHARACTERISTIC(is_void<int>, disjunction<is_const<int>, is_volatile<int>, is_void<int>>);
// basic true cases
VERIFY_BASE_CHARACTERISTIC(true_type, disjunction<true_type>);
VERIFY_BASE_CHARACTERISTIC(true_type, disjunction<false_type, true_type>);
VERIFY_BASE_CHARACTERISTIC(true_type, disjunction<false_type, false_type, true_type>);
// more complex true cases, selecting the last trait
VERIFY_BASE_CHARACTERISTIC(is_void<void>, disjunction<is_void<void>>);
VERIFY_BASE_CHARACTERISTIC(is_void<void>, disjunction<is_const<int>, is_void<void>>);
VERIFY_BASE_CHARACTERISTIC(is_void<void>, disjunction<is_const<int>, is_volatile<int>, is_void<void>>);
// more complex true cases, selecting not-the-last trait
VERIFY_BASE_CHARACTERISTIC(is_void<void>, disjunction<is_const<int>, is_void<void>, is_volatile<int>>);
VERIFY_BASE_CHARACTERISTIC(is_void<void>, disjunction<is_void<void>, is_const<int>, is_volatile<int>>);
// short circuiting
VERIFY_BASE_CHARACTERISTIC(is_void<void>, disjunction<is_void<void>, void>);
VERIFY_BASE_CHARACTERISTIC(is_void<void>, disjunction<is_const<int>, is_void<void>, void>);
VERIFY_BASE_CHARACTERISTIC(is_void<void>, disjunction<is_void<void>, void, void>);
VERIFY_BASE_CHARACTERISTIC(is_void<void>, disjunction<is_void<void>, void, void, void>);
// non-integral_constant family
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<false>, disjunction<fake_bool<false>>);
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<false>, disjunction<fake_bool<false>, fake_bool<false>>);
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<false>, disjunction<fake_bool<false>, fake_bool<false>, fake_bool<false>>);
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<true>, disjunction<fake_bool<true>>);
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<true>, disjunction<fake_bool<true>, fake_bool<false>>);
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<true>, disjunction<fake_bool<false>, fake_bool<true>>);
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<true>, disjunction<fake_bool<false>, fake_bool<false>, fake_bool<true>>);
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<true>, disjunction<fake_bool<false>, fake_bool<true>, fake_bool<false>>);
VERIFY_BASE_CHARACTERISTIC_V(fake_bool<true>, disjunction<fake_bool<true>, fake_bool<false>, fake_bool<false>>);
//   </disjunction>
//   <negation>
VERIFY_BASE_CHARACTERISTIC(bool_constant<true>, negation<false_type>);
VERIFY_BASE_CHARACTERISTIC(bool_constant<false>, negation<true_type>);
STATIC_ASSERT(is_base_of_v<bool_constant<true>, negation<final_fake_bool<false>>>);
STATIC_ASSERT(is_base_of_v<bool_constant<false>, negation<final_fake_bool<true>>>);
//   </negation>
// </P0013R1>


// DDB-198043 "[VS2008 / TR1] problems with is_pod and has_trivial_constructor"
struct TrivialExceptConstruct {
    TrivialExceptConstruct();
    int i;
};

template <typename T>
struct Wrap {
    T t;
    int j;
};

STATIC_ASSERT(!is_trivially_default_constructible_v<Wrap<TrivialExceptConstruct>>);
STATIC_ASSERT(!is_pod_v<Wrap<TrivialExceptConstruct>>);


// VSO-152213 "<type_traits>: is_function does not match qualified function types"
// VSO-154500 "<type_traits>: [Feedback]std::is_function returns false for cv/ref qualified functions"
// C++14 LWG-2196 "Specification of is_*[copy/move]_[constructible/assignable] unclear for non-referenceable types"
// C++17 LWG-2101 "Some transformation types can produce impossible types"

template <typename T>
void test_function_type() {
    // Primary type categories. Exactly one is true.
    STATIC_ASSERT(!is_void_v<T>);
    STATIC_ASSERT(!is_null_pointer_v<T>);
    STATIC_ASSERT(!is_integral_v<T>);
    STATIC_ASSERT(!is_floating_point_v<T>);
    STATIC_ASSERT(!is_array_v<T>);
    STATIC_ASSERT(!is_pointer_v<T>);
    STATIC_ASSERT(!is_lvalue_reference_v<T>);
    STATIC_ASSERT(!is_rvalue_reference_v<T>);
    STATIC_ASSERT(!is_member_object_pointer_v<T>);
    STATIC_ASSERT(!is_member_function_pointer_v<T>);
    STATIC_ASSERT(!is_enum_v<T>);
    STATIC_ASSERT(!is_union_v<T>);
    STATIC_ASSERT(!is_class_v<T>);
    STATIC_ASSERT(is_function_v<T>);

    // Composite type traits. Function types are compound, but not anything else.
    STATIC_ASSERT(!is_reference_v<T>);
    STATIC_ASSERT(!is_arithmetic_v<T>);
    STATIC_ASSERT(!is_fundamental_v<T>);
    STATIC_ASSERT(!is_object_v<T>);
    STATIC_ASSERT(!is_scalar_v<T>);
    STATIC_ASSERT(is_compound_v<T>);
    STATIC_ASSERT(!is_member_pointer_v<T>);

    // Type properties, convenience forms. They're false for plain function types,
    // and they're required to be false for non-referenceable types.
    STATIC_ASSERT(!is_copy_constructible_v<T>);
    STATIC_ASSERT(!is_move_constructible_v<T>);
    STATIC_ASSERT(!is_copy_assignable_v<T>);
    STATIC_ASSERT(!is_move_assignable_v<T>);
#if _HAS_CXX17
    STATIC_ASSERT(!is_swappable_v<T>);
#endif // _HAS_CXX17
    STATIC_ASSERT(!is_trivially_copy_constructible_v<T>);
    STATIC_ASSERT(!is_trivially_move_constructible_v<T>);
    STATIC_ASSERT(!is_trivially_copy_assignable_v<T>);
    STATIC_ASSERT(!is_trivially_move_assignable_v<T>);
    STATIC_ASSERT(!is_nothrow_copy_constructible_v<T>);
    STATIC_ASSERT(!is_nothrow_move_constructible_v<T>);
    STATIC_ASSERT(!is_nothrow_copy_assignable_v<T>);
    STATIC_ASSERT(!is_nothrow_move_assignable_v<T>);
#if _HAS_CXX17
    STATIC_ASSERT(!is_nothrow_swappable_v<T>);
#endif // _HAS_CXX17

#if _HAS_CXX17
    STATIC_ASSERT(!has_unique_object_representations_v<T>);
#endif // _HAS_CXX17

#if _HAS_CXX20
    STATIC_ASSERT(is_same_v<remove_cvref_t<T>, T>);

    STATIC_ASSERT(!is_bounded_array_v<T>);
    STATIC_ASSERT(!is_unbounded_array_v<T>);
#endif // _HAS_CXX20

#if _HAS_CXX23
    STATIC_ASSERT(!is_scoped_enum_v<T>);
#endif
}

template <typename T>
void test_plain_function_type() {
    test_function_type<T>();

    // These transformations work normally on plain function types.
    STATIC_ASSERT(is_same_v<add_lvalue_reference_t<T>, T&>);
    STATIC_ASSERT(is_same_v<add_rvalue_reference_t<T>, T&&>);
    STATIC_ASSERT(is_same_v<add_pointer_t<T>, T*>);

#if _HAS_CXX20
    STATIC_ASSERT(is_same_v<remove_cvref_t<T&>, T>);
    STATIC_ASSERT(is_same_v<remove_cvref_t<T&&>, T>);
#endif // _HAS_CXX20
}

template <typename T>
void test_weird_function_type() {
    test_function_type<T>();

    // These transformations are required to leave non-referenceable function types unchanged.
    STATIC_ASSERT(is_same_v<add_lvalue_reference_t<T>, T>);
    STATIC_ASSERT(is_same_v<add_rvalue_reference_t<T>, T>);
    STATIC_ASSERT(is_same_v<add_pointer_t<T>, T>);
}

void test_all_function_types() {
    test_plain_function_type<int(int)>();
    test_weird_function_type<int(int) const>();
    test_weird_function_type<int(int) volatile>();
    test_weird_function_type<int(int) const volatile>();
    test_weird_function_type<int(int)&>();
    test_weird_function_type<int(int) const&>();
    test_weird_function_type<int(int) volatile&>();
    test_weird_function_type<int(int) const volatile&>();
    test_weird_function_type<int(int) &&>();
    test_weird_function_type<int(int) const&&>();
    test_weird_function_type<int(int) volatile&&>();
    test_weird_function_type<int(int) const volatile&&>();

    test_plain_function_type<int(int, ...)>();
    test_weird_function_type<int(int, ...) const>();
    test_weird_function_type<int(int, ...) volatile>();
    test_weird_function_type<int(int, ...) const volatile>();
    test_weird_function_type<int(int, ...)&>();
    test_weird_function_type<int(int, ...) const&>();
    test_weird_function_type<int(int, ...) volatile&>();
    test_weird_function_type<int(int, ...) const volatile&>();
    test_weird_function_type<int(int, ...) &&>();
    test_weird_function_type<int(int, ...) const&&>();
    test_weird_function_type<int(int, ...) volatile&&>();
    test_weird_function_type<int(int, ...) const volatile&&>();
}

// Test more transformations, just in case.
STATIC_ASSERT(is_same_v<add_lvalue_reference_t<int>, int&>);
STATIC_ASSERT(is_same_v<add_rvalue_reference_t<int>, int&&>);
STATIC_ASSERT(is_same_v<add_pointer_t<int>, int*>);

STATIC_ASSERT(is_same_v<add_lvalue_reference_t<int&>, int&>);
STATIC_ASSERT(is_same_v<add_rvalue_reference_t<int&>, int&>);
STATIC_ASSERT(is_same_v<add_pointer_t<int&>, int*>);

STATIC_ASSERT(is_same_v<add_lvalue_reference_t<int&&>, int&>);
STATIC_ASSERT(is_same_v<add_rvalue_reference_t<int&&>, int&&>);
STATIC_ASSERT(is_same_v<add_pointer_t<int&&>, int*>);

STATIC_ASSERT(is_same_v<add_lvalue_reference_t<void>, void>);
STATIC_ASSERT(is_same_v<add_rvalue_reference_t<void>, void>);
STATIC_ASSERT(is_same_v<add_pointer_t<void>, void*>);

// Test simplified is_void.
STATIC_ASSERT(is_void_v<void>);
STATIC_ASSERT(is_void_v<const void>);
STATIC_ASSERT(is_void_v<volatile void>);
STATIC_ASSERT(is_void_v<const volatile void>);

STATIC_ASSERT(!is_void_v<int>);
STATIC_ASSERT(!is_void_v<const int>);
STATIC_ASSERT(!is_void_v<volatile int>);
STATIC_ASSERT(!is_void_v<const volatile int>);

#if _HAS_CXX17
// P0185R1 swappable traits
namespace swappable_trait_tests {
    enum swap_kind { implicitly_unswappable, explicitly_unswappable, implicitly_swappable, hidden_swap, visible_swap };

    template <swap_kind, bool>
    struct type {};

    template <bool Throws>
    struct type<implicitly_unswappable, Throws> {
        type()                  = default;
        type(type&&)            = delete;
        type& operator=(type&&) = delete;
    };

    template <bool Throws>
    void swap(type<explicitly_unswappable, Throws>&, type<explicitly_unswappable, Throws>&) = delete;

    template <bool Throws>
    struct type<implicitly_swappable, Throws> {
        type() = default;
        type(type&&) noexcept(!Throws) {}
        type& operator=(type&&) noexcept(!Throws) {
            return *this;
        }
        type(const type&)            = default;
        type& operator=(const type&) = default;
    };

    template <bool Throws>
    struct type<hidden_swap, Throws> : type<implicitly_unswappable, Throws> {
        friend void swap(type&, type&) noexcept(!Throws) {}
    };

    template <bool Throws>
    struct type<visible_swap, Throws> : type<implicitly_unswappable, Throws> {};

    template <bool Throws>
    void swap(type<visible_swap, Throws>&, type<visible_swap, Throws>&) noexcept(!Throws) {}

    STATIC_ASSERT(is_swappable_v<int>);
    STATIC_ASSERT(!is_swappable_v<void>);
    STATIC_ASSERT(is_swappable_v<void (*)(int, double)>);
    STATIC_ASSERT(!is_swappable_v<void(int, double)>);
    STATIC_ASSERT(is_swappable_v<long>);
    STATIC_ASSERT(is_swappable_v<int[4]>);
    STATIC_ASSERT(is_swappable_v<int[2][3][4][5]>);
    STATIC_ASSERT(!is_swappable_v<double[]>);
    STATIC_ASSERT(!is_swappable_v<int[][4]>);

    STATIC_ASSERT(!is_swappable_v<type<implicitly_unswappable, true>>);
    STATIC_ASSERT(!is_swappable_v<type<implicitly_unswappable, false>>);
    STATIC_ASSERT(!is_swappable_v<type<explicitly_unswappable, true>>);
    STATIC_ASSERT(!is_swappable_v<type<explicitly_unswappable, false>>);
    STATIC_ASSERT(is_swappable_v<type<implicitly_swappable, true>>);
    STATIC_ASSERT(is_swappable_v<type<implicitly_swappable, false>>);
    STATIC_ASSERT(is_swappable_v<type<hidden_swap, true>>);
    STATIC_ASSERT(is_swappable_v<type<hidden_swap, false>>);
    STATIC_ASSERT(is_swappable_v<type<visible_swap, true>>);
    STATIC_ASSERT(is_swappable_v<type<visible_swap, false>>);

    STATIC_ASSERT(!is_swappable_v<type<implicitly_unswappable, true>[2]>);
    STATIC_ASSERT(!is_swappable_v<type<implicitly_unswappable, false>[2]>);
    STATIC_ASSERT(!is_swappable_v<type<explicitly_unswappable, true>[2]>);
    STATIC_ASSERT(!is_swappable_v<type<explicitly_unswappable, false>[2]>);
    STATIC_ASSERT(is_swappable_v<type<implicitly_swappable, true>[2]>);
    STATIC_ASSERT(is_swappable_v<type<implicitly_swappable, false>[2]>);
    STATIC_ASSERT(is_swappable_v<type<hidden_swap, true>[2]>);
    STATIC_ASSERT(is_swappable_v<type<hidden_swap, false>[2]>);
    STATIC_ASSERT(is_swappable_v<type<visible_swap, true>[2]>);
    STATIC_ASSERT(is_swappable_v<type<visible_swap, false>[2]>);

    STATIC_ASSERT(is_nothrow_swappable_v<int>);
    STATIC_ASSERT(!is_nothrow_swappable_v<void>);
    STATIC_ASSERT(is_nothrow_swappable_v<void (*)(int, double)>);
    STATIC_ASSERT(!is_nothrow_swappable_v<void(int, double)>);
    STATIC_ASSERT(is_nothrow_swappable_v<long>);
    STATIC_ASSERT(is_nothrow_swappable_v<int[4]>);
    STATIC_ASSERT(is_nothrow_swappable_v<int[2][3][4][5]>);
    STATIC_ASSERT(!is_nothrow_swappable_v<double[]>);
    STATIC_ASSERT(!is_nothrow_swappable_v<int[][4]>);

    STATIC_ASSERT(!is_nothrow_swappable_v<type<implicitly_unswappable, true>>);
    STATIC_ASSERT(!is_nothrow_swappable_v<type<implicitly_unswappable, false>>);
    STATIC_ASSERT(!is_nothrow_swappable_v<type<explicitly_unswappable, true>>);
    STATIC_ASSERT(!is_nothrow_swappable_v<type<explicitly_unswappable, false>>);
    STATIC_ASSERT(!is_nothrow_swappable_v<type<implicitly_swappable, true>>);
    STATIC_ASSERT(is_nothrow_swappable_v<type<implicitly_swappable, false>>);
    STATIC_ASSERT(!is_nothrow_swappable_v<type<hidden_swap, true>>);
    STATIC_ASSERT(is_nothrow_swappable_v<type<hidden_swap, false>>);
    STATIC_ASSERT(!is_nothrow_swappable_v<type<visible_swap, true>>);
    STATIC_ASSERT(is_nothrow_swappable_v<type<visible_swap, false>>);

    STATIC_ASSERT(!is_nothrow_swappable_v<type<implicitly_unswappable, true>[2]>);
    STATIC_ASSERT(!is_nothrow_swappable_v<type<implicitly_unswappable, false>[2]>);
    STATIC_ASSERT(!is_nothrow_swappable_v<type<explicitly_unswappable, true>[2]>);
    STATIC_ASSERT(!is_nothrow_swappable_v<type<explicitly_unswappable, false>[2]>);
    STATIC_ASSERT(!is_nothrow_swappable_v<type<implicitly_swappable, true>[2]>);
    STATIC_ASSERT(is_nothrow_swappable_v<type<implicitly_swappable, false>[2]>);
    STATIC_ASSERT(!is_nothrow_swappable_v<type<hidden_swap, true>[2]>);
    STATIC_ASSERT(is_nothrow_swappable_v<type<hidden_swap, false>[2]>);
    STATIC_ASSERT(!is_nothrow_swappable_v<type<visible_swap, true>[2]>);
    STATIC_ASSERT(is_nothrow_swappable_v<type<visible_swap, false>[2]>);

    STATIC_ASSERT(!is_swappable_with_v<void, void>);
    STATIC_ASSERT(!is_swappable_with_v<int, int>);
    STATIC_ASSERT(!is_swappable_with_v<int&, double&>);
    STATIC_ASSERT(!is_swappable_with_v<int (&)[4], bool (&)[4]>);
    STATIC_ASSERT(!is_swappable_with_v<int (&)[3][4][1][2], int (&)[4][4][1][2]>);

    STATIC_ASSERT(!is_nothrow_swappable_with_v<void, void>);
    STATIC_ASSERT(!is_nothrow_swappable_with_v<int, int>);
    STATIC_ASSERT(!is_nothrow_swappable_with_v<int&, double&>);
    STATIC_ASSERT(!is_nothrow_swappable_with_v<int (&)[4], bool (&)[4]>);
    STATIC_ASSERT(!is_nothrow_swappable_with_v<int (&)[3][4][1][2], int (&)[4][4][1][2]>);
} // namespace swappable_trait_tests
#else // _HAS_CXX17
// In C++14 mode, when std::swap() is unconstrained,
// verify that we can explicitly specialize it for an immovable type.

struct Immovable {
    Immovable() {}
    ~Immovable() {}
    Immovable(const Immovable&)            = delete;
    Immovable& operator=(const Immovable&) = delete;
};

namespace std {
    template <>
    void swap(Immovable&, Immovable&) {}
} // namespace std

#endif // _HAS_CXX17

enum ExampleEnum { xExample, yExample };
enum LLEnum : long long { xLongExample, yLongExample };
enum class ExampleEnumClass { xExample, yExample };
enum class LLEnumClass : long long { xLongExample, yLongExample };

#if _HAS_CXX23
STATIC_ASSERT(!is_scoped_enum<ExampleEnum>::value);
STATIC_ASSERT(!is_scoped_enum_v<ExampleEnum>);
STATIC_ASSERT(!is_scoped_enum_v<test_abc1>);
STATIC_ASSERT(is_scoped_enum_v<ExampleEnumClass>);
STATIC_ASSERT(is_scoped_enum_v<LLEnumClass>);
enum E { e = is_scoped_enum_v<E> };
static_assert(!e, "is_scoped_enum of an incomplete unscoped enum type is true");
#endif // _HAS_CXX23

// P0258R2 has_unique_object_representations
#if _HAS_CXX17
STATIC_ASSERT(!has_unique_object_representations_v<void>);
STATIC_ASSERT(!has_unique_object_representations_v<const void>);
STATIC_ASSERT(!has_unique_object_representations_v<volatile void>);
STATIC_ASSERT(!has_unique_object_representations_v<const volatile void>);

STATIC_ASSERT(has_unique_object_representations_v<int>);
STATIC_ASSERT(has_unique_object_representations_v<const int>);
STATIC_ASSERT(has_unique_object_representations_v<volatile int>);
STATIC_ASSERT(has_unique_object_representations_v<const volatile int>);

STATIC_ASSERT(has_unique_object_representations_v<void*>);
STATIC_ASSERT(has_unique_object_representations_v<const void*>);
STATIC_ASSERT(has_unique_object_representations_v<volatile void*>);
STATIC_ASSERT(has_unique_object_representations_v<const volatile void*>);

STATIC_ASSERT(has_unique_object_representations_v<int*>);
STATIC_ASSERT(has_unique_object_representations_v<const int*>);
STATIC_ASSERT(has_unique_object_representations_v<volatile int*>);
STATIC_ASSERT(has_unique_object_representations_v<const volatile int*>);

STATIC_ASSERT(has_unique_object_representations_v<FP>);
STATIC_ASSERT(has_unique_object_representations_v<const FP>);
STATIC_ASSERT(has_unique_object_representations_v<volatile FP>);
STATIC_ASSERT(has_unique_object_representations_v<const volatile FP>);

STATIC_ASSERT(has_unique_object_representations_v<PMF>);
STATIC_ASSERT(has_unique_object_representations_v<const PMF>);
STATIC_ASSERT(has_unique_object_representations_v<volatile PMF>);
STATIC_ASSERT(has_unique_object_representations_v<const volatile PMF>);

STATIC_ASSERT(has_unique_object_representations_v<PMD>);
STATIC_ASSERT(has_unique_object_representations_v<const PMD>);
STATIC_ASSERT(has_unique_object_representations_v<volatile PMD>);
STATIC_ASSERT(has_unique_object_representations_v<const volatile PMD>);

STATIC_ASSERT(has_unique_object_representations_v<bool>);
STATIC_ASSERT(has_unique_object_representations_v<char>);
STATIC_ASSERT(has_unique_object_representations_v<signed char>);
STATIC_ASSERT(has_unique_object_representations_v<unsigned char>);
STATIC_ASSERT(has_unique_object_representations_v<short>);
STATIC_ASSERT(has_unique_object_representations_v<unsigned short>);
STATIC_ASSERT(has_unique_object_representations_v<int>);
STATIC_ASSERT(has_unique_object_representations_v<unsigned int>);
STATIC_ASSERT(has_unique_object_representations_v<long>);
STATIC_ASSERT(has_unique_object_representations_v<unsigned long>);
STATIC_ASSERT(has_unique_object_representations_v<long long>);
STATIC_ASSERT(has_unique_object_representations_v<unsigned long long>);
STATIC_ASSERT(has_unique_object_representations_v<wchar_t>);
STATIC_ASSERT(has_unique_object_representations_v<char16_t>);
STATIC_ASSERT(has_unique_object_representations_v<char32_t>);

STATIC_ASSERT(!has_unique_object_representations_v<void>);
STATIC_ASSERT(!has_unique_object_representations_v<nullptr_t>);
STATIC_ASSERT(!has_unique_object_representations_v<float>);
STATIC_ASSERT(!has_unique_object_representations_v<double>);
STATIC_ASSERT(!has_unique_object_representations_v<long double>);

struct NoPadding {
    int a;
    int b;
};

STATIC_ASSERT(has_unique_object_representations_v<NoPadding>);

struct Padding {
    char a;
    int b;
};

STATIC_ASSERT(!has_unique_object_representations_v<Padding>);

struct TailPadding {
    int a;
    char b;
};

STATIC_ASSERT(!has_unique_object_representations_v<TailPadding>);

union NoPaddingUnion {
    int a;
    unsigned int b;
};

STATIC_ASSERT(has_unique_object_representations_v<NoPaddingUnion>);

union PaddingUnion {
    int a;
    long long b;
};

STATIC_ASSERT(!has_unique_object_representations_v<PaddingUnion>);

struct NotTriviallyCopyable {
    int x;
    NotTriviallyCopyable(const NotTriviallyCopyable&) {}
};

STATIC_ASSERT(!has_unique_object_representations_v<NotTriviallyCopyable>);

STATIC_ASSERT(has_unique_object_representations_v<ExampleEnum>);
STATIC_ASSERT(has_unique_object_representations_v<LLEnum>);
STATIC_ASSERT(has_unique_object_representations_v<ExampleEnumClass>);
STATIC_ASSERT(has_unique_object_representations_v<LLEnumClass>);

// function types tested above in test_function_type

// N.B. because reference types aren't object types
STATIC_ASSERT(!has_unique_object_representations_v<int&>);
STATIC_ASSERT(!has_unique_object_representations_v<const int&>);
STATIC_ASSERT(!has_unique_object_representations_v<volatile int&>);
STATIC_ASSERT(!has_unique_object_representations_v<const volatile int&>);

class Empty {};

STATIC_ASSERT(!has_unique_object_representations_v<Empty>);

struct Compressed : Empty {
    int x;
};

STATIC_ASSERT(has_unique_object_representations_v<Compressed>);

class CompressedD : Compressed {};

STATIC_ASSERT(has_unique_object_representations_v<CompressedD>);

STATIC_ASSERT(has_unique_object_representations_v<int[42]>);
STATIC_ASSERT(has_unique_object_representations_v<int[]>);
STATIC_ASSERT(!has_unique_object_representations_v<double[42]>);
STATIC_ASSERT(!has_unique_object_representations_v<double[]>);

#endif // _HAS_CXX17

#if _HAS_CXX20
// P0887R1 type_identity
template <typename T>
constexpr bool test_type_identity() {
    STATIC_ASSERT(is_same_v<T, typename type_identity<T>::type>);
    STATIC_ASSERT(is_same_v<T, type_identity_t<T>>);

    if constexpr (!is_function_v<T>) {
        STATIC_ASSERT(is_same_v<const T, typename type_identity<const T>::type>);
        STATIC_ASSERT(is_same_v<volatile T, typename type_identity<volatile T>::type>);
        STATIC_ASSERT(is_same_v<const volatile T, typename type_identity<const volatile T>::type>);

        STATIC_ASSERT(is_same_v<const T, type_identity_t<const T>>);
        STATIC_ASSERT(is_same_v<volatile T, type_identity_t<volatile T>>);
        STATIC_ASSERT(is_same_v<const volatile T, type_identity_t<const volatile T>>);
    }

    if constexpr (!is_void_v<T>) {
        STATIC_ASSERT(is_same_v<T&, typename type_identity<T&>::type>);
        STATIC_ASSERT(is_same_v<T&&, typename type_identity<T&&>::type>);

        STATIC_ASSERT(is_same_v<T&, type_identity_t<T&>>);
        STATIC_ASSERT(is_same_v<T&&, type_identity_t<T&&>>);
    }

    if constexpr (!is_void_v<T> && !is_function_v<T>) {
        STATIC_ASSERT(is_same_v<const T&, typename type_identity<const T&>::type>);
        STATIC_ASSERT(is_same_v<volatile T&, typename type_identity<volatile T&>::type>);
        STATIC_ASSERT(is_same_v<const volatile T&, typename type_identity<const volatile T&>::type>);
        STATIC_ASSERT(is_same_v<const T&&, typename type_identity<const T&&>::type>);
        STATIC_ASSERT(is_same_v<volatile T&&, typename type_identity<volatile T&&>::type>);
        STATIC_ASSERT(is_same_v<const volatile T&&, typename type_identity<const volatile T&&>::type>);

        STATIC_ASSERT(is_same_v<const T&, type_identity_t<const T&>>);
        STATIC_ASSERT(is_same_v<volatile T&, type_identity_t<volatile T&>>);
        STATIC_ASSERT(is_same_v<const volatile T&, type_identity_t<const volatile T&>>);
        STATIC_ASSERT(is_same_v<const T&&, type_identity_t<const T&&>>);
        STATIC_ASSERT(is_same_v<volatile T&&, type_identity_t<volatile T&&>>);
        STATIC_ASSERT(is_same_v<const volatile T&&, type_identity_t<const volatile T&&>>);
    }

    return true;
}

static_assert(test_type_identity<void>());
static_assert(test_type_identity<int>());
static_assert(test_type_identity<int*>());
static_assert(test_type_identity<const int*>());
static_assert(test_type_identity<volatile int*>());
static_assert(test_type_identity<const volatile int*>());
static_assert(test_type_identity<int[3]>());
static_assert(test_type_identity<int[]>());
static_assert(test_type_identity<int(int)>());
static_assert(test_type_identity<int&(int)>());
static_assert(test_type_identity<const int&(int)>());
static_assert(test_type_identity<volatile int&(int)>());
static_assert(test_type_identity<const volatile int&(int)>());
static_assert(test_type_identity<int(int&)>());
static_assert(test_type_identity<int(const int&)>());
static_assert(test_type_identity<int(volatile int&)>());
static_assert(test_type_identity<int(const volatile int&)>());
static_assert(test_type_identity<int C::*>());
static_assert(test_type_identity<const int C::*>());
static_assert(test_type_identity<volatile int C::*>());
static_assert(test_type_identity<const volatile int C::*>());
static_assert(test_type_identity<int (C::*)(int)>());
static_assert(test_type_identity<int (C::*)(int&)>());
static_assert(test_type_identity<int (C::*)(const int&) const>());
static_assert(test_type_identity<int (C::*)(volatile int&) volatile>());
static_assert(test_type_identity<int (C::*)(const volatile int&) const volatile>());
static_assert(test_type_identity<int (C::*)(int) &>());
static_assert(test_type_identity<int (C::*)(int) const&>());
static_assert(test_type_identity<int (C::*)(int) &&>());
static_assert(test_type_identity<int (C::*)(int) const&&>());
static_assert(test_type_identity<int& (C::*) (int)>());
static_assert(test_type_identity<const int& (C::*) (int)>());
static_assert(test_type_identity<volatile int& (C::*) (int)>());
static_assert(test_type_identity<const volatile int& (C::*) (int)>());

// P0550R2 remove_cvref
template <typename T>
constexpr bool test_remove_cvref() {
    STATIC_ASSERT(is_same_v<T, typename remove_cvref<T>::type>);
    STATIC_ASSERT(is_same_v<T, remove_cvref_t<T>>);

    if constexpr (!is_function_v<T>) {
        STATIC_ASSERT(is_same_v<T, typename remove_cvref<const T>::type>);
        STATIC_ASSERT(is_same_v<T, typename remove_cvref<volatile T>::type>);
        STATIC_ASSERT(is_same_v<T, typename remove_cvref<const volatile T>::type>);

        STATIC_ASSERT(is_same_v<T, remove_cvref_t<const T>>);
        STATIC_ASSERT(is_same_v<T, remove_cvref_t<volatile T>>);
        STATIC_ASSERT(is_same_v<T, remove_cvref_t<const volatile T>>);
    }

    if constexpr (!is_void_v<T>) {
        STATIC_ASSERT(is_same_v<T, typename remove_cvref<T&>::type>);
        STATIC_ASSERT(is_same_v<T, typename remove_cvref<T&&>::type>);

        STATIC_ASSERT(is_same_v<T, remove_cvref_t<T&>>);
        STATIC_ASSERT(is_same_v<T, remove_cvref_t<T&&>>);
    }

    if constexpr (!is_void_v<T> && !is_function_v<T>) {
        STATIC_ASSERT(is_same_v<T, typename remove_cvref<const T&>::type>);
        STATIC_ASSERT(is_same_v<T, typename remove_cvref<volatile T&>::type>);
        STATIC_ASSERT(is_same_v<T, typename remove_cvref<const volatile T&>::type>);
        STATIC_ASSERT(is_same_v<T, typename remove_cvref<const T&&>::type>);
        STATIC_ASSERT(is_same_v<T, typename remove_cvref<volatile T&&>::type>);
        STATIC_ASSERT(is_same_v<T, typename remove_cvref<const volatile T&&>::type>);

        STATIC_ASSERT(is_same_v<T, remove_cvref_t<const T&>>);
        STATIC_ASSERT(is_same_v<T, remove_cvref_t<volatile T&>>);
        STATIC_ASSERT(is_same_v<T, remove_cvref_t<const volatile T&>>);
        STATIC_ASSERT(is_same_v<T, remove_cvref_t<const T&&>>);
        STATIC_ASSERT(is_same_v<T, remove_cvref_t<volatile T&&>>);
        STATIC_ASSERT(is_same_v<T, remove_cvref_t<const volatile T&&>>);
    }

    return true;
}

static_assert(test_remove_cvref<void>());
static_assert(test_remove_cvref<int>());
static_assert(test_remove_cvref<int*>());
static_assert(test_remove_cvref<const int*>());
static_assert(test_remove_cvref<volatile int*>());
static_assert(test_remove_cvref<const volatile int*>());
static_assert(test_remove_cvref<int[3]>());
static_assert(test_remove_cvref<int[]>());
static_assert(test_remove_cvref<int(int)>());
static_assert(test_remove_cvref<int&(int)>());
static_assert(test_remove_cvref<const int&(int)>());
static_assert(test_remove_cvref<volatile int&(int)>());
static_assert(test_remove_cvref<const volatile int&(int)>());
static_assert(test_remove_cvref<int(int&)>());
static_assert(test_remove_cvref<int(const int&)>());
static_assert(test_remove_cvref<int(volatile int&)>());
static_assert(test_remove_cvref<int(const volatile int&)>());
static_assert(test_remove_cvref<int C::*>());
static_assert(test_remove_cvref<const int C::*>());
static_assert(test_remove_cvref<volatile int C::*>());
static_assert(test_remove_cvref<const volatile int C::*>());
static_assert(test_remove_cvref<int (C::*)(int)>());
static_assert(test_remove_cvref<int (C::*)(int&)>());
static_assert(test_remove_cvref<int (C::*)(const int&) const>());
static_assert(test_remove_cvref<int (C::*)(volatile int&) volatile>());
static_assert(test_remove_cvref<int (C::*)(const volatile int&) const volatile>());
static_assert(test_remove_cvref<int (C::*)(int) &>());
static_assert(test_remove_cvref<int (C::*)(int) const&>());
static_assert(test_remove_cvref<int (C::*)(int) &&>());
static_assert(test_remove_cvref<int (C::*)(int) const&&>());
static_assert(test_remove_cvref<int& (C::*) (int)>());
static_assert(test_remove_cvref<const int& (C::*) (int)>());
static_assert(test_remove_cvref<volatile int& (C::*) (int)>());
static_assert(test_remove_cvref<const volatile int& (C::*) (int)>());
#endif // _HAS_CXX20

// VSO-707437 "<type_traits>: [Feedback] Template parameter is ambiguous after VS update"
template <typename T>
void test_VSO_707437_c(T, add_const_t<T>*) {}
template <typename T>
void test_VSO_707437_v(T, add_volatile_t<T>*) {}
template <typename T>
void test_VSO_707437_cv(T, add_cv_t<T>*) {}

#if _HAS_CXX20
template <typename T>
void test_VSO_707437_i(T, type_identity_t<T>*) {}
#endif // _HAS_CXX20

// VSO-781535 "[RWC][Regression][prod/fe] WebKit failed with error C2938"
// The compiler can't track that the type __underlying_type(T) is dependent on T, and so will diagnose this definition
// as ill-formed ("'std::enable_if_t<false,void>' : Failed to specialize alias template") if we expose the intrinsic
// directly in std::underlying_type_t.
template <typename E, enable_if_t<is_same_v<underlying_type_t<E>, bool>>* = nullptr>
constexpr bool isValidEnum(int) {
    return true;
}
template <typename>
constexpr bool isValidEnum(long) {
    return false;
}
enum bool_enum : bool {};
enum char_enum : char {};
STATIC_ASSERT(isValidEnum<bool_enum>(0));
STATIC_ASSERT(!isValidEnum<char_enum>(0));

// Let's validate that __make_integer_seq doesn't have the same problem.
template <typename T, enable_if_t<is_same_v<make_integer_sequence<T, 0>, integer_sequence<long long>>>* = nullptr>
constexpr bool test_make_integer_sequence(int) {
    return true;
}
template <typename>
constexpr bool test_make_integer_sequence(long) {
    return false;
}
STATIC_ASSERT(test_make_integer_sequence<long long>(0));
STATIC_ASSERT(!test_make_integer_sequence<char>(0));


// P0340R3 SFINAE-Friendly underlying_type
template <typename T, typename = void>
struct HasUnderlyingTypeStruct : false_type {};

template <typename T>
struct HasUnderlyingTypeStruct<T, void_t<typename underlying_type<T>::type>> : true_type {};

STATIC_ASSERT(HasUnderlyingTypeStruct<ExampleEnum>::value);
STATIC_ASSERT(HasUnderlyingTypeStruct<LLEnum>::value);
STATIC_ASSERT(HasUnderlyingTypeStruct<ExampleEnumClass>::value);
STATIC_ASSERT(HasUnderlyingTypeStruct<LLEnumClass>::value);

STATIC_ASSERT(!HasUnderlyingTypeStruct<int>::value);
STATIC_ASSERT(!HasUnderlyingTypeStruct<double>::value);
STATIC_ASSERT(!HasUnderlyingTypeStruct<ExampleEnum*>::value);
STATIC_ASSERT(!HasUnderlyingTypeStruct<ExampleEnum&>::value);
STATIC_ASSERT(!HasUnderlyingTypeStruct<ExampleEnum[3]>::value);
STATIC_ASSERT(!HasUnderlyingTypeStruct<ExampleEnum (&)[3]>::value);
STATIC_ASSERT(!HasUnderlyingTypeStruct<void>::value);
STATIC_ASSERT(!HasUnderlyingTypeStruct<ExampleEnum[]>::value);

template <typename T, typename = void>
struct HasUnderlyingTypeAlias : false_type {};

template <typename T>
struct HasUnderlyingTypeAlias<T, void_t<underlying_type_t<T>>> : true_type {};

STATIC_ASSERT(HasUnderlyingTypeAlias<ExampleEnum>::value);
STATIC_ASSERT(HasUnderlyingTypeAlias<LLEnum>::value);
STATIC_ASSERT(HasUnderlyingTypeAlias<ExampleEnumClass>::value);
STATIC_ASSERT(HasUnderlyingTypeAlias<LLEnumClass>::value);

STATIC_ASSERT(!HasUnderlyingTypeAlias<int>::value);
STATIC_ASSERT(!HasUnderlyingTypeAlias<double>::value);
STATIC_ASSERT(!HasUnderlyingTypeAlias<ExampleEnum*>::value);
STATIC_ASSERT(!HasUnderlyingTypeAlias<ExampleEnum&>::value);
STATIC_ASSERT(!HasUnderlyingTypeAlias<ExampleEnum[3]>::value);
STATIC_ASSERT(!HasUnderlyingTypeAlias<ExampleEnum (&)[3]>::value);
STATIC_ASSERT(!HasUnderlyingTypeAlias<void>::value);
STATIC_ASSERT(!HasUnderlyingTypeAlias<ExampleEnum[]>::value);


int main() {
    test_all_function_types();

    test_VSO_707437_c(11L, nullptr);
    test_VSO_707437_v(11L, nullptr);
    test_VSO_707437_cv(11L, nullptr);

#if _HAS_CXX20
    test_VSO_707437_i(11L, nullptr);
#endif // _HAS_CXX20
}

#if _HAS_CXX20
// Test the P0898R3 changes to <type_traits>:
// * Addition of common_reference and basic_common_reference
// * New fallback case for common_type

namespace {
    template <class, class = void>
    constexpr bool is_trait = false;
    template <class T>
    constexpr bool is_trait<T, void_t<typename T::type>> = true;

    namespace detail {
        static constexpr bool permissive() {
            return false;
        }

        template <class>
        struct DependentBase {
            static constexpr bool permissive() {
                return true;
            }
        };

        template <class T>
        struct Derived : DependentBase<T> {
            static constexpr bool test() {
                return permissive();
            }
        };
    } // namespace detail
    constexpr bool is_permissive = detail::Derived<int>::test();

    struct move_only {
        move_only()                       = default;
        move_only(move_only&&)            = default;
        move_only& operator=(move_only&&) = default;
    };

    template <class T>
    struct converts_from {
        converts_from() = default;
        constexpr converts_from(T) noexcept {}
    };

    template <int>
    struct interconvertible {
        interconvertible() = default;
        template <int N>
        explicit interconvertible(interconvertible<N>) {}
    };

    template <class... Ts>
    struct derives_from : Ts... {};

    struct simple_base {};
    using simple_derived = derives_from<simple_base>;
} // unnamed namespace

template <>
struct common_type<interconvertible<0>, interconvertible<1>> {
    using type = interconvertible<2>;
};
template <>
struct common_type<interconvertible<1>, interconvertible<0>> {
    using type = interconvertible<2>;
};

// A slightly simplified variation of std::tuple
template <class...>
struct tuple_ish {};

template <class, class, class>
struct tuple_ish_helper {};
template <class... Ts, class... Us>
struct tuple_ish_helper<void_t<common_reference_t<Ts, Us>...>, tuple_ish<Ts...>, tuple_ish<Us...>> {
    using type = tuple_ish<common_reference_t<Ts, Us>...>;
};

template <class... Ts, class... Us, template <class> class TQual, template <class> class UQual>
struct basic_common_reference<tuple_ish<Ts...>, tuple_ish<Us...>, TQual, UQual>
    : tuple_ish_helper<void, tuple_ish<TQual<Ts>...>, tuple_ish<UQual<Us>...>> {};

// N4928 [meta.trans.other]/5.1: If sizeof...(T) is zero, there shall be no member type.
STATIC_ASSERT(!is_trait<common_reference<>>);


// N4928 [meta.trans.other]/5.2: Otherwise, if sizeof...(T) is one, let T0 denote the sole type in the pack T. The
// member typedef type shall denote the same type as T0.
STATIC_ASSERT(is_same_v<common_reference_t<void>, void>);
STATIC_ASSERT(is_same_v<common_reference_t<int>, int>);
STATIC_ASSERT(is_same_v<common_reference_t<int&>, int&>);
STATIC_ASSERT(is_same_v<common_reference_t<int&&>, int&&>);
STATIC_ASSERT(is_same_v<common_reference_t<int const>, int const>);
STATIC_ASSERT(is_same_v<common_reference_t<int const&>, int const&>);
STATIC_ASSERT(is_same_v<common_reference_t<int const&&>, int const&&>);
STATIC_ASSERT(is_same_v<common_reference_t<int volatile[]>, int volatile[]>);
STATIC_ASSERT(is_same_v<common_reference_t<int volatile (&)[]>, int volatile (&)[]>);
STATIC_ASSERT(is_same_v<common_reference_t<int volatile (&&)[]>, int volatile (&&)[]>);
STATIC_ASSERT(is_same_v<common_reference_t<void (&)()>, void (&)()>);
STATIC_ASSERT(is_same_v<common_reference_t<void (&&)()>, void (&&)()>);
STATIC_ASSERT(is_same_v<common_reference_t<void() volatile>, void() volatile>);
STATIC_ASSERT(is_same_v<common_reference_t<void() &&>, void() &&>);


// N4928 [meta.trans.other]/5.3.1 as updated by P2655R3 (TRANSITION, cite new WP here):
// "Let R be COMMON-REF(T1, T2). If T1 and T2 are reference types, R is well-formed, and
// is_convertible_v<add_pointer_t<T1>, add_pointer_t<R>> && is_convertible_v<add_pointer_t<T2>, add_pointer_t<R>>
// is true, then the member typedef type denotes R."
STATIC_ASSERT(is_same_v<common_reference_t<simple_base&, simple_derived&>, simple_base&>);
STATIC_ASSERT(is_same_v<common_reference_t<simple_base&, simple_derived const&>, simple_base const&>);
STATIC_ASSERT(is_same_v<common_reference_t<simple_base&, simple_derived&&>, simple_base const&>);
STATIC_ASSERT(is_same_v<common_reference_t<simple_base&, simple_derived const&&>, simple_base const&>);

STATIC_ASSERT(is_same_v<common_reference_t<simple_base const&, simple_derived&>, simple_base const&>);
STATIC_ASSERT(is_same_v<common_reference_t<simple_base const&, simple_derived const&>, simple_base const&>);
STATIC_ASSERT(is_same_v<common_reference_t<simple_base const&, simple_derived&&>, simple_base const&>);
STATIC_ASSERT(is_same_v<common_reference_t<simple_base const&, simple_derived const&&>, simple_base const&>);

STATIC_ASSERT(is_same_v<common_reference_t<simple_base&&, simple_derived&>, simple_base const&>);
STATIC_ASSERT(is_same_v<common_reference_t<simple_base&&, simple_derived const&>, simple_base const&>);
STATIC_ASSERT(is_same_v<common_reference_t<simple_base&&, simple_derived&&>, simple_base&&>);
STATIC_ASSERT(is_same_v<common_reference_t<simple_base&&, simple_derived const&&>, simple_base const&&>);

STATIC_ASSERT(is_same_v<common_reference_t<simple_base const&&, simple_derived&>, simple_base const&>);
STATIC_ASSERT(is_same_v<common_reference_t<simple_base const&&, simple_derived const&>, simple_base const&>);
STATIC_ASSERT(is_same_v<common_reference_t<simple_base const&&, simple_derived&&>, simple_base const&&>);
STATIC_ASSERT(is_same_v<common_reference_t<simple_base const&&, simple_derived const&&>, simple_base const&&>);

#ifdef __EDG__
// When f is the name of a function of type int(), C1XX incorrectly believes that
//   decltype(false ? f : f)
// is int() in permissive mode and int(*)() in strict mode (Yes, two different incorrect results). It also
// correctly believes that
//   decltype(false ? declval<decltype((f))>() : declval<decltype((f))>())
// is int(&)(), which is nice because it allows this test case to pass. EDG believes the type of both the above
// is int() in all modes. I suspect this is intentional bug compatibility with C1XX, so I'm not filing a bug. I
// _do_ assert here that EDG produces the _wrong_ type from common_reference_t, however, so that THIS TEST WILL
// FAIL IF AND WHEN EDG STARTS BEHAVING CORRECTLY. We can then remove the non-workaround to defend against
// regression.
STATIC_ASSERT(!is_same_v<common_reference_t<int (&)(), int (&)()>, int (&)()>);
STATIC_ASSERT(!is_same_v<common_reference_t<int (&&)(), int (&)()>, int (&)()>);
STATIC_ASSERT(!is_same_v<common_reference_t<int (&)(), int (&&)()>, int (&)()>);
STATIC_ASSERT(!is_same_v<common_reference_t<int (&&)(), int (&&)()>, int (&&)()>);
#else // ^^^ EDG / not EDG vvv
STATIC_ASSERT(is_same_v<common_reference_t<int (&)(), int (&)()>, int (&)()>);
STATIC_ASSERT(is_same_v<common_reference_t<int (&&)(), int (&)()>, int (&)()>);
STATIC_ASSERT(is_same_v<common_reference_t<int (&)(), int (&&)()>, int (&)()>);
STATIC_ASSERT(is_same_v<common_reference_t<int (&&)(), int (&&)()>, int (&&)()>);
#endif // __EDG__

STATIC_ASSERT(is_same_v<common_reference_t<int const volatile&&, int volatile&&>, int const volatile&&>);
STATIC_ASSERT(is_same_v<common_reference_t<int&&, int const&, int volatile&>, int const volatile&>);

template <class T = int>
constexpr bool strict_only_common_reference_cases() {
    if constexpr (!is_permissive) {
        STATIC_ASSERT(is_same_v<common_reference_t<T(&)[10], T(&&)[10]>, T const(&)[10]>);
        STATIC_ASSERT(is_same_v<common_reference_t<T const(&)[10], T volatile(&)[10]>, T const volatile(&)[10]>);
    }

    return true;
}
STATIC_ASSERT(strict_only_common_reference_cases());


// N4928 [meta.trans.other]/5.3.2: Otherwise, if basic_common_reference<remove_cvref_t<T1>, remove_cvref_t<T2>,
// XREF(T1), XREF(T2)>::type is well-formed, then the member typedef type denotes that type.
STATIC_ASSERT(is_same_v<common_reference_t<tuple_ish<int, short> const&, tuple_ish<int&, short volatile&>>,
    tuple_ish<int const&, short const volatile&>>);
STATIC_ASSERT(is_same_v<common_reference_t<tuple_ish<int, short> volatile&, tuple_ish<int, short> const&>,
    tuple_ish<int, short> const volatile&>);


// N4928 [meta.trans.other]/5.3.3: Otherwise, if COND_RES(T1, T2) is well-formed, then the member typedef type
// denotes that type.
STATIC_ASSERT(is_same_v<common_reference_t<void, void>, void>);
STATIC_ASSERT(is_same_v<common_reference_t<void const, void>, void>);
STATIC_ASSERT(is_same_v<common_reference_t<void volatile, void>, void>);
STATIC_ASSERT(is_same_v<common_reference_t<void const volatile, void>, void>);
STATIC_ASSERT(is_same_v<common_reference_t<int, short>, int>);
STATIC_ASSERT(is_same_v<common_reference_t<int, short&>, int>);
STATIC_ASSERT(is_same_v<common_reference_t<int&, short&>, int>);
STATIC_ASSERT(is_same_v<common_reference_t<int&, short>, int>);

// tricky reference-to-volatile case
STATIC_ASSERT(is_same_v<common_reference_t<int&&, int volatile&>, int>);
STATIC_ASSERT(is_same_v<common_reference_t<int volatile&, int&&>, int>);

STATIC_ASSERT(is_same_v<common_reference_t<int (&)[10], int (&)[11]>, int*>);

// https://github.com/ericniebler/stl2/issues/338
STATIC_ASSERT(is_same_v<common_reference_t<int&, converts_from<int&>>, converts_from<int&>>);


// N4928 [meta.trans.other]/5.3.4: Otherwise, if common_type_t<T1, T2> is well-formed, then the member typedef type
// denotes that type.
STATIC_ASSERT(is_same_v<common_reference_t<interconvertible<0>&, interconvertible<1> const&>, interconvertible<2>>);

STATIC_ASSERT(is_same_v<common_reference_t<move_only const&, move_only>, move_only>);
STATIC_ASSERT(is_same_v<common_reference_t<derives_from<move_only> const&, move_only>, move_only>);
STATIC_ASSERT(is_same_v<common_reference_t<move_only const&, derives_from<move_only>>, move_only>);


// N4928 [meta.trans.other]/5.3.5: Otherwise, there shall be no member type.
STATIC_ASSERT(!is_trait<common_reference<tuple_ish<short> volatile&, tuple_ish<int, short> const&>>);

STATIC_ASSERT(!is_trait<common_reference<void() volatile, void() volatile>>);
STATIC_ASSERT(!is_trait<common_reference<void() volatile, int>>);
STATIC_ASSERT(!is_trait<common_reference<int, void() volatile>>);
STATIC_ASSERT(!is_trait<common_reference<void() &&, void() &&>>);
STATIC_ASSERT(!is_trait<common_reference<int*, void() &&>>);
STATIC_ASSERT(!is_trait<common_reference<void() &&, int (&)()>>);
STATIC_ASSERT(!is_trait<common_reference<void() volatile, void() &&>>);


// N4928 [meta.trans.other]/5.4: Otherwise, if sizeof...(T) is greater than two, let T1, T2, and Rest, respectively,
// denote the first, second, and (pack of) remaining types comprising T. Let C be the type
// common_reference_t<T1, T2>. Then:
// N4928 [meta.trans.other]/5.4.1: If there is such a type C, the member typedef type shall denote the same type, if
// any, as common_reference_t<C, Rest...>.
STATIC_ASSERT(is_same_v<common_reference_t<int, int, int>, int>);
STATIC_ASSERT(is_same_v<common_reference_t<int&&, int const&, int volatile&>, int const volatile&>);
STATIC_ASSERT(is_same_v<common_reference_t<int&&, int const&, float&>, float>);

STATIC_ASSERT(is_same_v<common_reference_t<simple_base&, simple_derived const&, simple_derived&>, simple_base const&>);
STATIC_ASSERT(
    is_same_v<common_reference_t<simple_base&, simple_derived&, simple_base&, simple_derived&>, simple_base&>);


// N4928 [meta.trans.other]/5.4.2: Otherwise, there shall be no member type.
STATIC_ASSERT(!is_trait<common_reference<int, short, int, char*>>);

template <class T>
struct bad_reference_wrapper {
    bad_reference_wrapper(T&);
    bad_reference_wrapper(T&&) = delete;
    operator T&() const;
};

// N4928 [meta.trans.other]/3.3.4 (per the proposed resolution of LWG-3205): Otherwise, if
//   remove_cvref_t<decltype(false ? declval<const D1&>() : declval<const D2&>())>
// denotes a type, let C denote that type.
STATIC_ASSERT(is_same_v<common_type_t<int, bad_reference_wrapper<int>>, int>);
STATIC_ASSERT(is_same_v<common_type_t<bad_reference_wrapper<double>, double>, double>);

#ifdef __cpp_lib_concepts // TRANSITION, GH-395
// P2655R3 common_reference_t Of reference_wrapper Should Be A Reference Type
STATIC_ASSERT(is_same_v<common_reference_t<int&, reference_wrapper<int>>, int&>);
STATIC_ASSERT(is_same_v<common_reference_t<int&, reference_wrapper<int>&>, int&>);
STATIC_ASSERT(is_same_v<common_reference_t<int&, const reference_wrapper<int>&>, int&>);
#endif // __cpp_lib_concepts
#endif // _HAS_CXX20
