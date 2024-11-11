// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

module;

#include <cassert>
#include <format>
#include <string>

#include <force_include.hpp>

export module User;

namespace user {
    export void prepare_test_environment() {
        assert(test_environment_preparer.succeeded());
    }

    // DevCom-10313766 VSO-1775715 "Using std::format in a module
    // requires including <format> header in .cpp files using that module"
    export template <class T>
    void test_vso_1775715(const T& value, const char* const expected_str) {
        const std::string actual_str = std::format("{0:#} {0:#d} {0:#b} {0:#B} {0:#x} {0:#X} {0:#o}", value);
        assert(actual_str == expected_str);
    }
} // namespace user
