// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <type_traits>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

// Regression test for:
// DevDiv-417110:  is_scalar<nullptr_t> should be true

int main() {

    // N3376 20.9.4.1[meta.unary.cat]:  primary type categories:
    STATIC_ASSERT(!std::is_void_v<std::nullptr_t>);
    STATIC_ASSERT(!std::is_integral_v<std::nullptr_t>);
    STATIC_ASSERT(!std::is_floating_point_v<std::nullptr_t>);
    STATIC_ASSERT(!std::is_array_v<std::nullptr_t>);
    STATIC_ASSERT(!std::is_pointer_v<std::nullptr_t>);
    STATIC_ASSERT(!std::is_lvalue_reference_v<std::nullptr_t>);
    STATIC_ASSERT(!std::is_rvalue_reference_v<std::nullptr_t>);
    STATIC_ASSERT(!std::is_member_object_pointer_v<std::nullptr_t>);
    STATIC_ASSERT(!std::is_member_function_pointer_v<std::nullptr_t>);
    STATIC_ASSERT(!std::is_enum_v<std::nullptr_t>);
    STATIC_ASSERT(!std::is_union_v<std::nullptr_t>);
    STATIC_ASSERT(!std::is_class_v<std::nullptr_t>);
    STATIC_ASSERT(!std::is_function_v<std::nullptr_t>);

    STATIC_ASSERT(std::is_null_pointer_v<std::nullptr_t>);
    STATIC_ASSERT(std::is_null_pointer_v<std::nullptr_t const>);
    STATIC_ASSERT(std::is_null_pointer_v<std::nullptr_t volatile>);
    STATIC_ASSERT(std::is_null_pointer_v<std::nullptr_t const volatile>);

    struct C {};
    enum class E { enumerator };
    union U {
        int i;
        float f;
    };

    STATIC_ASSERT(!std::is_null_pointer_v<void>);
    STATIC_ASSERT(!std::is_null_pointer_v<int>);
    STATIC_ASSERT(!std::is_null_pointer_v<float>);
    STATIC_ASSERT(!std::is_null_pointer_v<C[1]>);
    STATIC_ASSERT(!std::is_null_pointer_v<C*>);
    STATIC_ASSERT(!std::is_null_pointer_v<C&>);
    STATIC_ASSERT(!std::is_null_pointer_v<C&&>);
    STATIC_ASSERT(!std::is_null_pointer_v<int C::*>);
    STATIC_ASSERT(!std::is_null_pointer_v<void (C::*)()>);
    STATIC_ASSERT(!std::is_null_pointer_v<E>);
    STATIC_ASSERT(!std::is_null_pointer_v<U>);
    STATIC_ASSERT(!std::is_null_pointer_v<C>);
    STATIC_ASSERT(!std::is_null_pointer_v<void()>);

    // N3376 20.9.4.2[meta.unary.comp]:  composite type categories:
    STATIC_ASSERT(!std::is_reference_v<std::nullptr_t>);
    STATIC_ASSERT(!std::is_arithmetic_v<std::nullptr_t>);
    STATIC_ASSERT(std::is_fundamental_v<std::nullptr_t>);
    STATIC_ASSERT(std::is_object_v<std::nullptr_t>);
    STATIC_ASSERT(std::is_scalar_v<std::nullptr_t>);
    STATIC_ASSERT(!std::is_compound_v<std::nullptr_t>);
    STATIC_ASSERT(!std::is_member_pointer_v<std::nullptr_t>);

#if _HAS_CXX20
    // [meta.unary.prop]: Type properties
    STATIC_ASSERT(!std::is_bounded_array_v<std::nullptr_t>);
    STATIC_ASSERT(!std::is_unbounded_array_v<std::nullptr_t>);
#endif // _HAS_CXX20
}
