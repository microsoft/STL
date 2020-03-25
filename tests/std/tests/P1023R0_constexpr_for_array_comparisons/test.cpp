// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>

using namespace std;

const array<int, 5> a0{2, 8, 9, 1, 9};
const array<int, 3> a1{2, 8, 9};
const array<int, 5> a2{2, 8, 9, 1, 8};
const array<int, 0> a3{};

constexpr bool test_operator_eq() {
    assert(a0 == a0);
    assert(a1 == a1);
    assert(!(a0 == a2));
    assert(a3 == a3);

    return true;
}

constexpr bool test_operator_neq() {
    assert(!(a0 != a0));
    assert(!(a1 != a1));
    assert((a0 != a2));
    assert(!(a3 != a3));

    return true;
}

constexpr bool test_operator_lt() {
    assert(!(a0 < a0));
    assert(!(a1 < a1));
    assert(a2 < a0);
    assert(!(a3 < a3));

    return true;
}

constexpr bool test_operator_gt() {
    assert(!(a0 > a0));
    assert(!(a1 > a1));
    assert(a0 > a2);
    assert(!(a3 > a3));

    return true;
}


constexpr bool test_operator_leq() {
    assert(a0 <= a0);
    assert(a1 <= a1);
    assert(a2 <= a0);
    assert(a3 <= a3);

    return true;
}


constexpr bool test_operator_geq() {
    assert(a0 >= a0);
    assert(a1 >= a1);
    assert(a0 >= a2);
    assert(a3 >= a3);

    return true;
}

constexpr bool test() {
    // clang-format off
    return test_operator_eq()
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
