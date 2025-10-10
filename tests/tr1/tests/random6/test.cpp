// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <random> C++11 header, part 6
#define TEST_NAME "<random>, part 6"

#include <math.h>
#define FLOAT_TYPE IS_DOUBLE
#include "tdefs.h"
#include "tspec_random.h"

void test_main() { // test random distributions
    test_random();
}
