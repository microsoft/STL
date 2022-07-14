// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "test_atomic_wait.hpp"

int main() {
    auto level = __std_atomic_set_api_level(__std_atomic_api_level::__has_srwlock);
#if _ATOMIC_WAIT_ON_ADDRESS_STATICALLY_AVAILABLE
    assert(level == __std_atomic_api_level::__has_wait_on_address);
#else // ^^^ wait on address statically available ^^^ / vvv wait on address NOT statically available vvv
    assert(level == __std_atomic_api_level::__has_srwlock);
#endif // ^^^ wait on address NOT statically available ^^^
    test_atomic_wait();
}
