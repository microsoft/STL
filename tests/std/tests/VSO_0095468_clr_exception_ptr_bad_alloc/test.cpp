// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdlib>
#include <exception>
#include <new>

using namespace std;

#if !defined(_DLL) || defined(_M_CEE_PURE) || defined(__EDG__)
#define ENABLE_TEST 1
#else
#define ENABLE_TEST 0
#endif // !defined(_DLL) || defined(_M_CEE_PURE) || defined(__EDG__)

#pragma warning(disable : 4702) // unreachable code
#pragma warning(disable : 28251) // Inconsistent annotation for 'new': this instance has no annotations.

#if ENABLE_TEST
void* operator new(size_t) {
    throw bad_alloc();
}

struct user_exception : exception {};
#endif // ENABLE_TEST

int main() {
#if ENABLE_TEST
    exception_ptr ptr = current_exception();
    assert("current_exception returned non-null with no active exception" && !ptr);

    try {
        throw user_exception();
    } catch (const exception&) {
        ptr = current_exception();
    }

    assert("current_exception didn't capture a thrown user_exception" && ptr);

    try {
        rethrow_exception(ptr);
    } catch (const user_exception&) {
        return 0;
    }

    abort();
#endif // ENABLE_TEST
}
