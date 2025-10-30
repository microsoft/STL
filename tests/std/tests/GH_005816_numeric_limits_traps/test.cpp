// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <limits>

#include <test_death.hpp>

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++17-extensions" // constexpr if is a C++17 extension
#else
#pragma warning(disable : 4984) //  warning C4984: 'if constexpr' is a C++17 language extension
#endif // __clang__

void trap_operation() {
    const volatile int op1 = 1;
    const volatile int op2 = 0;
    const volatile int res = op1 / op2;
    (void) res;
}

int main(int argc, char* argv[]) {
    if constexpr (std::numeric_limits<int>::traps) {
        std_testing::death_test_executive exec;
        const std_testing::death_function_t one_trap[] = {trap_operation};
        exec.add_death_tests(one_trap);
        return exec.run(argc, argv);
    } else {
        trap_operation();
    }
}
