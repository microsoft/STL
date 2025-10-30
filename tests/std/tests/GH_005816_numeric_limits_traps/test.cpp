// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <limits>

#include <test_death.hpp>

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++17-extensions" // constexpr if is a C++17 extension
#else // ^^^ defined(__clang__) / !defined(__clang__) vvv
#pragma warning(disable : 4984) //  warning C4984: 'if constexpr' is a C++17 language extension
#endif // ^^^ !defined(__clang__) ^^^

template <class T>
constexpr bool traps_ = std::numeric_limits<T>::traps;

static_assert(!traps_<bool>, "bool does not trap for a moot reason");

static_assert(!traps_<float> && !traps_<double> && !traps_<long double>,
    "floats don't trap because even if '/fp:except' is passed, it should be enabled at runtime");

static_assert(traps_<char> == traps_<int> && traps_<signed char> == traps_<int> && traps_<unsigned char> == traps_<int>
                  && traps_<short> == traps_<int> && traps_<unsigned short> == traps_<int>
                  && traps_<unsigned int> == traps_<int> //
                  && traps_<long> == traps_<int> && traps_<unsigned long> == traps_<int>
                  && traps_<long long> == traps_<int> && traps_<unsigned long long> == traps_<int>,
    "all integers should trap or not trap equally");

void trap_operation() {
    const volatile int op1 = 1;
    const volatile int op2 = 0;
    const volatile int res = op1 / op2;
    (void) res;
}

int main(int argc, char* argv[]) {
    if constexpr (traps_<int>) {
        std_testing::death_test_executive exec;
        const std_testing::death_function_t one_trap[] = {trap_operation};
        exec.add_death_tests(one_trap);
        return exec.run(argc, argv);
    } else {
        trap_operation();
    }
}
