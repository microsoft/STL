// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdlib>
#include <exception>
#include <stdexcept>

using namespace std;

#pragma warning(push)
#pragma warning(disable : 4702) // unreachable code
[[noreturn]] void handleTerminate() noexcept {
    assert(uncaught_exceptions() == 1);
    exception_ptr e = current_exception();
    assert(e);
    try {
        rethrow_exception(e);
        abort();
    } catch (const runtime_error&) {
        assert(uncaught_exceptions() == 1);
        exit(0);
    }
}
#pragma warning(pop)

struct A {
    ~A() noexcept(false) {
        throw runtime_error("Runtime error happened");
    }
};

int main() {
    set_terminate(handleTerminate);
    try {
        A a;
        throw 42;
    } catch (int) {
    }
}
