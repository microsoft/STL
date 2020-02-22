// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <typeindex>
#define TEST_NAME "<typeindex>"

#include "tdefs.h"
#include <exception>
#include <string.h>
#include <typeindex>

void test_main() { // test basic workings of type_info definitions
    STD bad_cast x1;

    STD type_index tix(typeid(x1));
    CHECK_INT(typeid(x1).hash_code(), tix.hash_code());
    CHECK_STR(typeid(x1).name(), tix.name());

    CHECK(tix == tix);
    CHECK(!(tix != tix));
    CHECK(!(tix < tix));
    CHECK(tix >= tix);
    CHECK(!(tix > tix));
    CHECK(tix <= tix);

    CHECK_INT(STD hash<STD type_index>()(tix), tix.hash_code());
}
