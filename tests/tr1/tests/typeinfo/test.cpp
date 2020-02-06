// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <typeinfo>
#define TEST_NAME "<typeinfo>"

#include "tdefs.h"
#include <exception>
#include <string.h>
#include <typeinfo>

void test_main() { // test basic workings of type_info definitions
    STD bad_cast x1;
    STD bad_typeid x2;
    STD exception* pex = &x1;
    STD type_info* p1  = (STD type_info*) nullptr;

    pex = &x2;
    pex = pex; // to quiet diagnostics

    CHECK(p1 != &typeid(x1));
    CHECK(!(typeid(x1) == typeid(x2)));
    CHECK(typeid(x1) != typeid(x2));
    CHECK(!typeid(x1).before(typeid(x1)));
    CHECK(typeid(x1).before(typeid(x2)) || typeid(x2).before(typeid(x1)));
    CHECK(typeid(x1).name() != nullptr && typeid(x2).name() != nullptr);
    CHECK(CSTD strcmp(typeid(x1).name(), typeid(x2).name()) != 0);
    CHECK(typeid(int).name() != nullptr);

    CHECK_INT(typeid(x1).hash_code(), typeid(x1).hash_code());

    if (!terse) { // not terse, display names
        CSTD printf("typeid(int).name() is %s\n", typeid(int).name());
        CSTD printf("typeid(bad_cast).name() is %s\n", typeid(STD bad_cast).name());
    }
}
