// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <type_traits>
using namespace std;

constexpr int square(const int x) {
    if (is_constant_evaluated()) {
        return 1000 + x * x;
    } else {
        return x * x;
    }
}

int cube(const int x) {
    if (is_constant_evaluated()) {
        return 2000 + x * x * x;
    } else {
        return x * x * x;
    }
}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconstant-evaluated"
#else // ^^^ clang / other vvv
#pragma warning(push)
#pragma warning(disable : 4365) // 'return': conversion from 'size_t' to 'int', signed/unsigned mismatch
#pragma warning(disable : 5063) // 'std::is_constant_evaluated' always evaluates to true
                                // in manifestly constant-evaluated expressions
#endif // __clang__
namespace example { // Test the N4842 [expr.const]/13 example.
    template <bool>
    struct X {};
    X<is_constant_evaluated()> x; // type X<true>
    int y;
    const int a = is_constant_evaluated() ? y : 1; // dynamic initialization to 1
    // double z[a]; // ill-formed: a is not usable in constant expressions
    const int b = is_constant_evaluated() ? 2 : y; // static initialization to 2
    int c       = y + (is_constant_evaluated() ? 2 : y); // dynamic initialization to y + y
    constexpr int f() {
        const int n = is_constant_evaluated() ? 13 : 17; // n is 13
        int m       = is_constant_evaluated() ? 13 : 17; // m might be 13 or 17 (see below)
        char arr[n] = {}; // char[13]
        return m + sizeof(arr);
    }
    int p = f(); // m is 13; initialized to 26
    int q = p + f(); // m is 17 for this call; initialized to 56
} // namespace example
#ifdef __clang__
#pragma clang diagnostic pop
#else // ^^^ clang / other vvv
#pragma warning(pop)
#endif // __clang__

int main() {
    static_assert(is_same_v<decltype(is_constant_evaluated()), bool>);
    static_assert(noexcept(is_constant_evaluated()));

    int val_a           = square(3);
    const int val_b     = square(4);
    constexpr int val_c = square(5);
    const int val_d     = square(val_a);

    assert(val_a == 9);
    assert(val_b == 1016);
    assert(val_c == 1025);
    assert(val_d == 81);

    static_assert(val_b == 1016);
    static_assert(val_c == 1025);

    int val_e       = cube(7);
    const int val_f = cube(8);

    assert(val_e == 343);
    assert(val_f == 512);

    static_assert(is_same_v<decltype(example::x), example::X<true>>);
    assert(example::a == 1);
    assert(example::b == 2);
    static_assert(example::b == 2);
    assert(example::c == 0);
    assert(example::p == 26);
    assert(example::q == 56);
}
