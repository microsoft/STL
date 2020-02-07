// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <csignal>
#define TEST_NAME "<csignal>"

#include "tdefs.h"
#include <csignal>

#ifdef _M_CEE_PURE
void test_main() { // test nothing
}
#else // _M_CEE_PURE
extern "C" {
typedef void (*Psig)(int);

STD sig_atomic_t ctr = 0;

void field_int(int sig) { // handle SIGINT and return
    CHECK_INT(sig, SIGINT);
    ++ctr;
}
} // extern "C"

void test_cpp() { // test C++ header
    CHECK(STD signal(SIGINT, &field_int) == SIG_DFL);
    CHECK(STD raise(SIGINT) == 0);
    STD signal(SIGINT, SIG_DFL);
    CHECK_INT(ctr, 1);
}

void test_main() { // test basic workings of csignal definitions
    static const int sigs[]  = {SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM};
    static const Psig rets[] = {SIG_DFL, SIG_ERR, SIG_IGN};

    CHECK(sigs[0] != sigs[1]);
    CHECK(rets[0] != rets[1]);

    test_cpp();
}
#endif // _M_CEE_PURE
