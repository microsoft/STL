// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <stdexcept>
#define TEST_NAME "<stdexcept>"

#include "tdefs.h"
#include <exception>
#include <stdexcept>

void test_main() { // test basic workings of exception definitions
    STD exception x1;
    STD logic_error x2("logic_error");
    STD domain_error x3("domain_error");
    STD invalid_argument x4("invalid_argument");
    STD length_error x5("length_error");
    STD out_of_range x6("out_of_range");
    STD runtime_error x7("runtime_error");
    STD overflow_error x8("overflow_error");
    STD underflow_error x9("underflow_error");
    STD range_error x10("range_error");

    STD exception* pex      = &x1;
    STD logic_error* plog   = &x2;
    STD runtime_error* prun = &x7;

    if (!terse) {
        printf("exception().what() returns \"%s\"\n", pex->what());
    }

    CHECK(pex->what() != nullptr);
    pex = plog;
    CHECK_STR(pex->what(), "logic_error");
    plog = &x3;
    CHECK_STR(plog->what(), "domain_error");
    plog = &x4;
    CHECK_STR(plog->what(), "invalid_argument");
    plog = &x5;
    CHECK_STR(plog->what(), "length_error");
    plog = &x6;
    CHECK_STR(plog->what(), "out_of_range");

    pex = prun;
    CHECK_STR(pex->what(), "runtime_error");
    prun = &x8;
    CHECK_STR(prun->what(), "overflow_error");
    prun = &x9;
    CHECK_STR(prun->what(), "underflow_error");
    prun = &x10;
    CHECK_STR(prun->what(), "range_error");
}
