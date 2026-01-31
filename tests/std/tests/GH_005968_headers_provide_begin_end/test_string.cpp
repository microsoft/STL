// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <string>

#include "shared_test.hpp"

void test_string() {
    std::string container = "hello";
    shared_test(container);
}
