// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "test_atomic_wait.hpp"

int main() {
#if defined(_M_IX86) || defined(_M_X64) && !defined(_M_ARM64EC)
    assert(__std_atomic_set_api_level(__std_atomic_api_level::__has_srwlock) == __std_atomic_api_level::__has_srwlock);
    test_atomic_wait();
#endif // defined(_M_IX86) || defined(_M_X64) && !defined(_M_ARM64EC)
}
