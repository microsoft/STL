// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <assert.h>
#include <type_traits>

using namespace std;

#define ASSERT(...) assert((__VA_ARGS__))


constexpr bool test() {
#ifndef __clang__ // TRANSITION, LLVM-Bug #: To Be Filed
    // is_layout_compatible tests
    {
        struct S0 {
            int v1;
            int v2;
        };

        struct S1 {
            S0 s1;
            int v3;
        };

        struct S2 {
            S0 s1;
            int v2;
        };

        struct S3 {
            S0 s1;
            int v2;
            int v3;
        };

        struct S4 {
            int v1;

        private:
            int v2;
        };

        struct S5 {
            int v1;

        private:
            int v2;
        };


        const struct S6 {
            int v1;
            int v2;
        };

        volatile struct S7 {
            int v1;
            int v2;
        };

        enum E1 { e1, e2, e3, e4 };
        enum E2 : int { e5 };
        enum E3 : unsigned int { e6, e7, e8 };
        enum class E4 : unsigned int { no, yes };
        enum class E5 { zero, fortytwo = 42 };
        const enum E6 : int {};
        volatile enum E7 : int {};

        ASSERT(is_layout_compatible_v<int, int>);
        ASSERT(is_layout_compatible_v<E1, E2>);
        ASSERT(is_layout_compatible_v<E3, E4>);
        ASSERT(is_layout_compatible_v<E5, E1>);
        ASSERT(is_layout_compatible_v<E6, E1>);
        ASSERT(is_layout_compatible_v<E7, E1>);
        ASSERT(is_layout_compatible_v<S0, S6>);
        ASSERT(is_layout_compatible_v<S0, S7>);
        ASSERT(is_layout_compatible_v<S1, S2>);
        ASSERT(is_layout_compatible_v<S4, S4>);

        ASSERT(!is_layout_compatible_v<int, char>);
        ASSERT(!is_layout_compatible_v<E1, E3>);
        ASSERT(!is_layout_compatible_v<E2, E4>);
        ASSERT(!is_layout_compatible_v<S1, S3>);
        ASSERT(!is_layout_compatible_v<S4, S5>);
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
        const class E : public A { int v1 = 2; };
        volatile class F : public A { int v1 = 2; };

        class A1 : public A {};
        class A2 : public A {};
        class A3 : public A2 {};
        class A4 : public A1, public A3 {};

        ASSERT(is_pointer_interconvertible_base_of_v<A, A>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, B>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, C>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, D>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, E>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, F>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, A1>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, A2>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, A3>);
        ASSERT(is_pointer_interconvertible_base_of_v<A2, A3>);

        ASSERT(!is_pointer_interconvertible_base_of_v<A1, E>);
        ASSERT(!is_pointer_interconvertible_base_of_v<A1, A2>);
        ASSERT(!is_pointer_interconvertible_base_of_v<A3, E>);
        ASSERT(!is_pointer_interconvertible_base_of_v<A4, A>);
        ASSERT(!is_pointer_interconvertible_base_of_v<E, F>);
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

        struct S5 {
            int v1;
            int v2;
            void* v3;
        };

        struct S6 {
            int v1;
            int v2;
            double v3;
        };

        ASSERT(is_corresponding_member(&S1::v1, &S1::v1));
        ASSERT(is_corresponding_member(&S1::v2, &S1::v2));
        ASSERT(!is_corresponding_member(&S1::v1, &S1::v2));
        ASSERT(!is_corresponding_member(&S1::v2, &S1::v1));
        ASSERT(is_corresponding_member(&S1::v1, &S2::v1));
        ASSERT(is_corresponding_member(&S1::v2, &S2::v2));
        ASSERT(is_corresponding_member(&S1::v1, &S3::v1));
        ASSERT(is_corresponding_member(&S1::v2, &S3::v2));
        ASSERT(is_corresponding_member(&S5::v1, &S6::v1));
        ASSERT(is_corresponding_member(&S5::v2, &S6::v2));

        ASSERT(!is_corresponding_member(&S1::v1, &S2::v2));
        ASSERT(!is_corresponding_member(&S1::v2, &S2::v1));
        ASSERT(!is_corresponding_member(&S1::v1, &S4::v1));
        ASSERT(!is_corresponding_member(&S1::v2, &S4::v2));
        ASSERT(!is_corresponding_member(&S3::v1, &S4::v1));
        ASSERT(!is_corresponding_member(&S3::v2, &S4::v2));
        ASSERT(!is_corresponding_member(&S5::v1, &S6::v2));
        ASSERT(!is_corresponding_member(&S5::v2, &S6::v1));
        ASSERT(!is_corresponding_member(&S5::v3, &S6::v3));
    }

    // is_pointer_interconvertible_with_class tests
    {
        struct A {
            int a;
        };

        struct B {
            int b;
        };

        struct C : A, B {};

        union U {
            int v1;
            char v2;
        };

        ASSERT(is_pointer_interconvertible_with_class(&C::b));
        ASSERT(!is_pointer_interconvertible_with_class<C>(&C::b));

        ASSERT(is_pointer_interconvertible_with_class(&U::v1));
        ASSERT(is_pointer_interconvertible_with_class(&U::v2));
    }
#endif // __clang__
    return true;
}

int main() {
    static_assert(test());
    test();
}
