// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <ranges>

#include <test_death.hpp>
using namespace std;

static int some_ints[] = {0, 1, 2, 3};

void test_constructor_negative_size() {
    (void) views::counted(some_ints, -3); // The size passed to views::counted must be non-negative
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#ifdef _DEBUG
    exec.add_death_tests({
        test_constructor_negative_size,
    });
#endif // _DEBUG

    return exec.run(argc, argv);
}
