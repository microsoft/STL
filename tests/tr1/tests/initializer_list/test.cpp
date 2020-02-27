// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <initializer_list> header
#define TEST_NAME "<initializer_list>"

#include "tdefs.h"
#include <initializer_list>
#include <stdexcept>

void test_main() { // test header <initializer_list>
    typedef STD initializer_list<char> Ty;
    Ty init0;
    CHECK_INT(init0.size(), 0);
    CHECK_PTR(init0.begin(), init0.end());

    CHECK_TYPE(Ty::value_type, char);
    CHECK_TYPE(Ty::size_type, CSTD size_t);
    CHECK_TYPE(Ty::const_reference, const char&);
    CHECK_TYPE(Ty::iterator, const char*);
    CHECK_TYPE(Ty::const_iterator, const char*);
    CHECK_TYPE(Ty::reference, char&);
}
