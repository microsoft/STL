// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>

using namespace std;

constexpr bool test_operator_eq() {
    array<int, 5> a0{2, 8, 9, 1, 9};
    array<int, 3> a1{2, 8, 9};
    array<int, 5> a2{2, 8, 9, 1, 8};
    array<int, 0> a3({});

    assert(a0 == a0);
    assert(a1 == a1);
    assert(!(a0 == a2));
    assert(a3 == a3);

#ifdef __cpp_lib_to_array
    assert(a0 == to_array({2, 8, 9, 1, 9}));
    assert(!(a0 == to_array({2, 8, 9, 1, 8})));
#endif //__cpp_lib_to_array

    return true;
}

constexpr bool test_operator_neq() {
    array<int, 5> a0{2, 8, 9, 1, 9};
    array<int, 3> a1{2, 8, 9};
    array<int, 5> a2{2, 8, 9, 1, 8};
    array<int, 0> a3({});

    assert(!(a0 != a0));
    assert(!(a1 != a1));
    assert(!(a0 == a2));
    assert(!(a3 != a3));

#ifdef __cpp_lib_to_array
    assert(!(a0 != to_array({2, 8, 9, 1, 9})));
    assert(a0 != to_array({2, 8, 9, 1, 8}));
#endif //__cpp_lib_to_array

    return true;
}

constexpr bool test_operator_lt() {
    array<int, 5> a0{2, 8, 9, 1, 9};
    array<int, 3> a1{2, 8, 9};
    array<int, 5> a2{2, 8, 9, 1, 8};
    array<int, 0> a3({});


    assert(!(a0 < a0));
    assert(!(a1 < a1));
    assert(a2 < a0);
    assert(!(a3 < a3));

#ifdef __cpp_lib_to_array
    assert(!(a0 < to_array({2, 8, 9, 1, 9})));
    assert(a0 < to_array({2, 8, 9, 1, 10}));
#endif //__cpp_lib_to_array

    return true;
}

constexpr bool test_operator_gt() {
    array<int, 5> a0{2, 8, 9, 1, 9};
    array<int, 3> a1{2, 8, 9};
    array<int, 5> a2{2, 8, 9, 1, 8};
    array<int, 0> a3({});


    assert(!(a0 > a0));
    assert(!(a1 > a1));
    assert(a0 > a2);
    assert(!(a3 > a3));

#ifdef __cpp_lib_to_array
    assert(!(a0 > to_array({2, 8, 9, 1, 9})));
    assert(a0 > to_array({2, 8, 9, 1, 0}));
#endif //__cpp_lib_to_array

    return true;
}


constexpr bool test_operator_leq() {
    array<int, 5> a0{2, 8, 9, 1, 9};
    array<int, 3> a1{2, 8, 9};
    array<int, 5> a2{2, 8, 9, 1, 8};
    array<int, 0> a3({});


    assert(a0 <= a0);
    assert(a1 <= a1);
    assert(a2 <= a0);
    assert(a3 <= a3);

#ifdef __cpp_lib_to_array
    assert(a0 <= to_array({2, 8, 9, 1, 9}));
    assert(a0 <= to_array({2, 8, 9, 1, 10}));
#endif //__cpp_lib_to_array

    return true;
}


constexpr bool test_operator_geq() {
    array<int, 5> a0{2, 8, 9, 1, 9};
    array<int, 3> a1{2, 8, 9};
    array<int, 5> a2{2, 8, 9, 1, 8};
    array<int, 0> a3({});


    assert(a0 >= a0);
    assert(a1 >= a1);
    assert(a0 >= a2);
    assert(a3 >= a3);

#ifdef __cpp_lib_to_array
    assert(a0 >= to_array({2, 8, 9, 1, 9}));
    assert(a0 >= to_array({2, 8, 9, 1, 0}));
#endif //__cpp_lib_to_array

    return true;
}

constexpr bool test() {
    // clang-format off
    return
        test_operator_eq()
        && test_operator_neq()
        && test_operator_lt()
        && test_operator_gt()
        && test_operator_leq()
        && test_operator_geq()
        ;
    // clang-format on
}

int main() {
    test();
    static_assert(test());
}
