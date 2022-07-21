// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <cassert>
#include <type_traits>

using namespace std;

#define ASSERT(...) assert((__VA_ARGS__))

struct S { // Must be declared at namespace scope due to static data member
    static int s1;
    int v1;
    int v2;
};

constexpr bool test() {
#ifndef __EDG__ // TRANSITION, VSO-1268984
#ifndef __clang__ // TRANSITION, LLVM-48860
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

        enum E1 { e1, e2, e3, e4 };
        enum E2 : int { e5 };
        enum E3 : unsigned int { e6, e7, e8 };
        enum class E4 : unsigned int { no, yes };
        enum class E5 { zero, fortytwo = 42 };

        ASSERT(is_layout_compatible_v<int, int>);
        ASSERT(is_layout_compatible_v<const void, void>);
        ASSERT(is_layout_compatible_v<S1, volatile S2>);
        ASSERT(is_layout_compatible_v<S1, S2>);
        ASSERT(is_layout_compatible_v<S4, S4>);
        ASSERT(is_layout_compatible_v<const volatile S4, S4>);
        ASSERT(is_layout_compatible_v<E1, E2>);
        ASSERT(is_layout_compatible_v<E3, E4>);
        ASSERT(is_layout_compatible_v<E5, E1>);
        ASSERT(is_layout_compatible_v<const E1, E2>);
        ASSERT(is_layout_compatible_v<volatile E3, const E4>);
        ASSERT(is_layout_compatible_v<int[], int[]>);
        ASSERT(is_layout_compatible_v<int[3], int[3]>);

        ASSERT(is_layout_compatible_v<const int[], int[]>);
        ASSERT(is_layout_compatible_v<const int[3], int[3]>);
        ASSERT(is_layout_compatible_v<int[], volatile int[]>);

        ASSERT(!is_layout_compatible_v<int, char>);
        ASSERT(!is_layout_compatible_v<int, void>);
        ASSERT(!is_layout_compatible_v<S1, void>);
        ASSERT(!is_layout_compatible_v<S1, S3>);
        ASSERT(!is_layout_compatible_v<S4, S5>);
        ASSERT(!is_layout_compatible_v<E1, void>);
        ASSERT(!is_layout_compatible_v<E1, E3>);
        ASSERT(!is_layout_compatible_v<E2, E4>);
        ASSERT(!is_layout_compatible_v<int[], int[2]>);
        ASSERT(!is_layout_compatible_v<int[3], int[1]>);
    }

    // is_pointer_interconvertible_base_of tests
    {
        class A {};
        class B : public A {};
        class C : public A {
            int : 0;
        };
        class D : public C {};
// Disable warning C4408: anonymous union did not declare any data members
#pragma warning(push)
#pragma warning(disable : 4408)
        class E : public A {
            union {};
        };
#pragma warning(pop)
        class F : private A {}; // Non-public inheritance
        class NS : public B, public C {}; // Non-standard layout
        class I; // Incomplete

        union U {
            int i;
            char c;
        };

        ASSERT(is_pointer_interconvertible_base_of_v<A, A>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, B>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, const B>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, C>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, volatile C>);
        ASSERT(is_pointer_interconvertible_base_of_v<volatile A, const C>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, D>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, E>);
        ASSERT(is_pointer_interconvertible_base_of_v<A, F>);
        ASSERT(is_pointer_interconvertible_base_of_v<C, D>);
        ASSERT(is_pointer_interconvertible_base_of_v<I, I>);
        ASSERT(is_pointer_interconvertible_base_of_v<const I, I>);

        ASSERT(!is_pointer_interconvertible_base_of_v<int, int>);
        ASSERT(!is_pointer_interconvertible_base_of_v<void, void>);
        ASSERT(!is_pointer_interconvertible_base_of_v<A, int>);
        ASSERT(!is_pointer_interconvertible_base_of_v<B, C>);
        ASSERT(!is_pointer_interconvertible_base_of_v<A, NS>);
        ASSERT(!is_pointer_interconvertible_base_of_v<B, NS>);
        ASSERT(!is_pointer_interconvertible_base_of_v<int, I>);
        ASSERT(!is_pointer_interconvertible_base_of_v<U, U>);
        ASSERT(!is_pointer_interconvertible_base_of_v<U, I>);
    }

    // is_corresponding_member tests
    {
        struct S1 {
            int v1;
            int v2;
        };

        struct S2 {
            int w1;
            int w2;
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

        struct S7 {
            int f1() {
                return 0;
            }
        };

        struct NS : S1, S2 {}; // Non-standard layout

        ASSERT(is_corresponding_member(&S1::v1, &S::v1));
        ASSERT(is_corresponding_member(&S1::v2, &S::v2));
        ASSERT(is_corresponding_member(&S1::v1, &S1::v1));
        ASSERT(is_corresponding_member(&S1::v2, &S1::v2));
        ASSERT(is_corresponding_member(&S1::v1, &S2::w1));
        ASSERT(is_corresponding_member(&S1::v2, &S2::w2));
        ASSERT(is_corresponding_member(&S1::v1, &S3::v1));
        ASSERT(is_corresponding_member(&S1::v2, &S3::v2));
        ASSERT(is_corresponding_member(&S5::v1, &S6::v1));
        ASSERT(is_corresponding_member(&S5::v2, &S6::v2));

        ASSERT(!is_corresponding_member(&S1::v1, &S1::v2));
        ASSERT(!is_corresponding_member(&S1::v2, &S1::v1));
        ASSERT(!is_corresponding_member(&S1::v2, &S2::w1));
        ASSERT(!is_corresponding_member(&S1::v1, &S4::v1));
        ASSERT(!is_corresponding_member(&S1::v2, &S4::v2));
        ASSERT(!is_corresponding_member(&S3::v1, &S4::v1));
        ASSERT(!is_corresponding_member(&S3::v2, &S4::v2));
        ASSERT(!is_corresponding_member(&S5::v1, &S6::v2));
        ASSERT(!is_corresponding_member(&S5::v2, &S6::v1));
        ASSERT(!is_corresponding_member(&S5::v3, &S6::v3));
        ASSERT(!is_corresponding_member<NS, NS>(&NS::v1, &NS::w1));
        ASSERT(!is_corresponding_member(&S7::f1, &S7::f1));
        ASSERT(!is_corresponding_member(static_cast<int S1::*>(nullptr), static_cast<int S2::*>(nullptr)));
        ASSERT(!is_corresponding_member(&S1::v1, static_cast<int S2::*>(nullptr)));
    }

    // is_pointer_interconvertible_with_class tests
    {
        struct A {
            int a;
        };

        struct B {
            int b;
        };

        struct C {
            int f1() {
                return 0;
            }
        };

        struct NS : A, B {}; // Non-standard layout

        union U {
            int v1;
            char v2;
        };

        ASSERT(is_pointer_interconvertible_with_class(&A::a));
        ASSERT(is_pointer_interconvertible_with_class(&NS::b));
        ASSERT(is_pointer_interconvertible_with_class(&U::v1));
        ASSERT(is_pointer_interconvertible_with_class(&U::v2));

        ASSERT(!is_pointer_interconvertible_with_class<NS>(&NS::a));
        ASSERT(!is_pointer_interconvertible_with_class<NS>(&NS::b));
        ASSERT(!is_pointer_interconvertible_with_class(&C::f1));
        ASSERT(!is_pointer_interconvertible_with_class(static_cast<int A::*>(nullptr)));
    }
#endif // __clang__
#endif // __EDG__
    return true;
}

int main() {
    static_assert(test());
    test();
}
