// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <assert.h>
#include <iostream>
#include <type_traits>

using namespace std;

#define ASSERT(...) assert((__VA_ARGS__))

constexpr bool test() {

    // is_layout_compatible tests
    {
        struct S {
            int v1;
            int v2;
        };

        struct S1 {
            S s1;
            int v3;
        };

        struct S2 {
            S s1;
            int v2;
        };

        struct S3 {
            S s1;
            int v2;
            int v3;
        };

        enum E1 { e1, e2, e3 };

        enum E2 : int { e4, e5, e6 };

        enum E3 : unsigned int { e7, e8, e9 };

        ASSERT(is_layout_compatible_v<int, int>);
        ASSERT(is_layout_compatible_v<E1, E2>);
        ASSERT(is_layout_compatible_v<S1, S2>);

        ASSERT(!is_layout_compatible_v<int, char>);
        ASSERT(!is_layout_compatible_v<E1, E3>);
        ASSERT(!is_layout_compatible_v<S1, S3>);
    }

    // is_pointer_interconvertible_base_of tests
    {
        class A {};
        class B : public A {};
        class C : public A {
            int : 0;
        };
// Disable warning C4408: anonymous union did not declare any data members
#pragma warning(push)
#pragma warning(disable : 4408)
        class D : public A {
            union {};
        };
#pragma warning(pop)
        class E : public A {
            int v1 = 2;
        };

        class A1 : public A {};
        class A2 : public A {};
        class A3 : public A2 {};
        class A4 : A1, A3 {};

        ASSERT(is_pointer_interconvertible_base_of_v<A, A>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, B>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, C>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, D>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, E>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, A1>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, A2>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, A3>);
        ASSERT(is_pointer_interconvertible_base_of_v<A2, A3>);

        ASSERT(!is_pointer_interconvertible_base_of_v<A4, A>);
        ASSERT(!is_pointer_interconvertible_base_of_v<A1, E>);
        ASSERT(!is_pointer_interconvertible_base_of_v<A3, E>);
    }

    // is_corresponding_member tests
    {
        struct S1 {
            int v1;
            int v2;
        };

        struct S2 {
            int v1;
            int v2;
        };

        struct S3 {
            int v1;
            int v2;
            int v3;
        };

        struct S4 {
            char v1;
            int v2;
            int v3;
        };

        // Same member - same class
        ASSERT(is_corresponding_member(&S1::v1, &S1::v1));
        ASSERT(is_corresponding_member(&S1::v2, &S1::v2));

        // Different member - same class
        ASSERT(!is_corresponding_member(&S1::v1, &S1::v2));
        ASSERT(!is_corresponding_member(&S1::v2, &S1::v1));

        // Same member - different class (layout compatible)
        ASSERT(is_corresponding_member(&S1::v1, &S2::v1));
        ASSERT(is_corresponding_member(&S1::v2, &S2::v2));

        // Same member - different class (layout compatible)
        ASSERT(is_corresponding_member(&S1::v1, &S3::v1));
        ASSERT(is_corresponding_member(&S1::v2, &S3::v2));

        // Different member - different class (layout compatible)
        ASSERT(!is_corresponding_member(&S1::v1, &S2::v2));
        ASSERT(!is_corresponding_member(&S1::v2, &S2::v1));

        // Same member - different class (not layout compatible)
        ASSERT(!is_corresponding_member(&S1::v1, &S4::v1));
        ASSERT(!is_corresponding_member(&S1::v2, &S4::v2));

        // Same member - different class (not layout compatible)
        ASSERT(!is_corresponding_member(&S3::v1, &S4::v1));
        ASSERT(!is_corresponding_member(&S3::v2, &S4::v2));
    }

    // is_pointer_interconvertible_with_class tests
    {
        // A standard-layout class
        struct A {
            int a;
        };

        // A standard-layout class
        struct B {
            int b;
        };

        // Not a standard-layout class
        struct C : public A, public B {};

        // Succeeds because, despite its appearance, &C::b has type: "pointer to
        // member of B of type int."
        ASSERT(is_pointer_interconvertible_with_class(&C::b));

        // Forces the use of class C, and fails.
        ASSERT(!is_pointer_interconvertible_with_class<C>(&C::b));

        // Succeeds because, despite appearances, &C::a and &C::b have types:
        //  "pointer to member of A of type int" and
        //  "pointer to member of B of type int," respectively.
        ASSERT(is_corresponding_member(&C::a, &C::b));

        // Forces the use of class C, and fails
        ASSERT(!is_corresponding_member<C, C>(&C::a, &C::b));
    }
    return true;
}

int main() {

    static_assert(test());
    test();
}
