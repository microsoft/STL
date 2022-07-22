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
    assert(uncaught_exceptions() == 0);
    exception_ptr e = current_exception();
    assert(e);
    try {
        rethrow_exception(e);
        abort();
    } catch (const runtime_error&) {
        assert(uncaught_exceptions() == 0);
        exit(0);
    }
}
#pragma warning(pop)

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexceptions"
#endif // __clang__
void meow() noexcept {
#pragma warning(suppress : 4297) // 'meow': function assumed not to throw an exception but does
    throw runtime_error("Runtime error happened");
}
#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

int main() {
    set_terminate(handleTerminate);
    meow();
}
