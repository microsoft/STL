// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <system_error>

int main() {
    std::error_code ec{109, std::system_category()}; // 109 is ERROR_BROKEN_PIPE according to winerror.h
    assert(ec == std::make_error_condition(std::errc::broken_pipe));
}
