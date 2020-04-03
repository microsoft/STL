// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <cstdbool>
#define TEST_NAME "<cstdbool>"

#define _SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING

#include "tdefs.h"
#include <cstdbool>

void test_cpp() { // test C++ header
#undef OK
#ifdef bool
#define OK 0
#else // bool
#define OK 1
#endif // bool
    CHECK_MSG("bool is not a macro", OK);

#undef OK
#ifdef false
#define OK 0
#else // false
#define OK 1
#endif // false
    CHECK_MSG("false is not a macro", OK);

#undef OK
#ifdef true
#define OK 0
#else // true
#define OK 1
#endif // true
    CHECK_MSG("true is not a macro", OK);
}

void test_main() { // test basic workings of cstdbool definitions
    bool f = false;
    bool t = true;

    CHECK_INT(false, 0);
    CHECK_INT(true, 1);
    CHECK_INT(f, false);
    CHECK_INT(t, true);
    CHECK_INT(__bool_true_false_are_defined, 1);

    test_cpp();
}
