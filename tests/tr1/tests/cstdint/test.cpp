// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <cstdint>
#define TEST_NAME "<cstdint>"

#include "tdefs.h"
#include <cstdint>

#define STDx STD

#ifndef INT_LEAST8_MAX
#define INT_LEAST8_MAX 0x7f
#endif // INT_LEAST8_MAX

#ifndef INT_LEAST16_MAX
#define INT_LEAST16_MAX 0x7fff
#endif // INT_LEAST16_MAX

#ifndef INT_LEAST32_MAX
#define INT_LEAST32_MAX 0x7fffffff
#endif // INT_LEAST32_MAX

#ifndef INT_LEAST64_MAX
#define INT_LEAST64_MAX 0x7fffffffffffffffLL
#endif // INT_LEAST8_MAX

#ifndef UINT_LEAST8_MAX
#define UINT_LEAST8_MAX 0xff
#endif // UINT_LEAST8_MAX

#ifndef UINT_LEAST16_MAX
#define UINT_LEAST16_MAX 0xffff
#endif // UINT_LEAST16_MAX

#ifndef UINT_LEAST32_MAX
#define UINT_LEAST32_MAX 0xffffffff
#endif // UINT_LEAST32_MAX

#ifndef UINT_LEAST64_MAX
#define UINT_LEAST64_MAX 0xffffffffffffffffULL
#endif // UINT_LEAST8_MAX

#ifndef INTMAX_MAX
#define INTMAX_MAX 0x7fffffffffffffffLL
#endif // INTMAX_MAX

#ifndef UINTMAX_MAX
#define UINTMAX_MAX 0xffffffffffffffffULL
#endif // UINTMAX_MAX

void test_cpp() { // test C++ header
    static const STDx int8_t int8_val[]   = {INT8_MAX};
    static const STDx int16_t int16_val[] = {INT16_MAX};
    static const STDx int32_t int32_val[] = {INT32_MAX};
    static const STDx int64_t int64_val[] = {INT64_MAX};

    static const STDx uint8_t uint8_val[]   = {UINT_LEAST8_MAX};
    static const STDx uint16_t uint16_val[] = {UINT_LEAST16_MAX};
    static const STDx uint32_t uint32_val[] = {UINT_LEAST32_MAX};
    static const STDx uint64_t uint64_val[] = {UINT_LEAST64_MAX};

    static const STDx int_least8_t int_least8_val[]   = {INT8_C(0) + INT_LEAST8_MAX};
    static const STDx int_least16_t int_least16_val[] = {INT16_C(0) + INT_LEAST16_MAX};
    static const STDx int_least32_t int_least32_val[] = {INT32_C(0) + INT_LEAST32_MAX};
    static const STDx int_least64_t int_least64_val[] = {INT64_C(0) + INT_LEAST64_MAX};

    static const STDx uint_least8_t uint_least8_val[]   = {UINT8_C(0) + UINT_LEAST8_MAX};
    static const STDx uint_least16_t uint_least16_val[] = {UINT16_C(0) + UINT_LEAST16_MAX};
    static const STDx uint_least32_t uint_least32_val[] = {UINT32_C(0) + UINT_LEAST32_MAX};
    static const STDx uint_least64_t uint_least64_val[] = {UINT64_C(0) + UINT_LEAST64_MAX};

    static const STDx int_fast8_t int_fast8_val[]   = {INT_FAST8_MAX};
    static const STDx int_fast16_t int_fast16_val[] = {INT_FAST16_MAX};
    static const STDx int_fast32_t int_fast32_val[] = {INT_FAST32_MAX};
    static const STDx int_fast64_t int_fast64_val[] = {INT_FAST64_MAX};

    static const STDx uint_fast8_t uint_fast8_val[]   = {UINT_FAST8_MAX};
    static const STDx uint_fast16_t uint_fast16_val[] = {UINT_FAST16_MAX};
    static const STDx uint_fast32_t uint_fast32_val[] = {UINT_FAST32_MAX};
    static const STDx uint_fast64_t uint_fast64_val[] = {UINT_FAST64_MAX};

    static const STDx intmax_t intmax_val[] = {INTMAX_C(0) + INTMAX_MAX};
    static const STDx intptr_t intptr_val[] = {INTPTR_MAX};

    static const STDx uintmax_t uintmax_val[] = {UINTMAX_C(0) + UINTMAX_MAX};
    static const STDx uintptr_t uintptr_val[] = {UINTPTR_MAX};

    CHECK_INT(int8_val[0], INT8_MAX);
    CHECK_INT(int16_val[0], INT16_MAX);
    CHECK_INT(int32_val[0], INT32_MAX);
    CHECK(int64_val[0] == INT64_MAX);

    CHECK(uint8_val[0] == UINT8_MAX);
    CHECK(uint16_val[0] == UINT16_MAX);
    CHECK(uint32_val[0] == UINT32_MAX);
    CHECK(uint64_val[0] == UINT64_MAX);

    CHECK_INT(int_least8_val[0], INT_LEAST8_MAX);
    CHECK_INT(int_least16_val[0], INT_LEAST16_MAX);
    CHECK_INT(int_least32_val[0], INT_LEAST32_MAX);
    CHECK(int_least64_val[0] == INT_LEAST64_MAX);

    CHECK(uint_least8_val[0] == UINT_LEAST8_MAX);
    CHECK(uint_least16_val[0] == UINT_LEAST16_MAX);
    CHECK(uint_least32_val[0] == UINT_LEAST32_MAX);
    CHECK(uint_least64_val[0] == UINT_LEAST64_MAX);

    CHECK_INT(int_fast8_val[0], INT_FAST8_MAX);
    CHECK_INT(int_fast16_val[0], INT_FAST16_MAX);
    CHECK_INT(int_fast32_val[0], INT_FAST32_MAX);
    CHECK(int_fast64_val[0] == INT_FAST64_MAX);

    CHECK(uint_fast8_val[0] == UINT_FAST8_MAX);
    CHECK(uint_fast16_val[0] == UINT_FAST16_MAX);
    CHECK(uint_fast32_val[0] == UINT_FAST32_MAX);
    CHECK(uint_fast64_val[0] == UINT_FAST64_MAX);

    CHECK(intmax_val[0] == INTMAX_MAX);
    CHECK(intptr_val[0] == INTPTR_MAX);

    CHECK(uintmax_val[0] == UINTMAX_MAX);
    CHECK(uintptr_val[0] == UINTPTR_MAX);
}

void test_main() { // test basic workings of cstdint definitions
    test_cpp();
}
