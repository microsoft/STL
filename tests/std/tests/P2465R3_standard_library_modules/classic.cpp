// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This translation unit exists to keep the classic #include directives isolated.

#include <assert.h> // intentionally not <cassert>

#include <force_include.hpp>

void prepare_test_environment() {
    assert(test_environment_preparer.succeeded());
}
