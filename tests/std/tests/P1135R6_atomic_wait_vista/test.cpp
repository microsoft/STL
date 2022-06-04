// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "test_atomic_wait.hpp"

int main() {
    assert(__std_atomic_set_api_level(__std_atomic_api_level::__has_srwlock) == __std_atomic_api_level::__has_srwlock);
    test_atomic_wait();
}
