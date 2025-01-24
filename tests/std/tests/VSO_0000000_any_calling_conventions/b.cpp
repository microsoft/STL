// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <any>
#include <utility>

#include "ab.hpp"

std::any __cdecl f(std::any a) {
    std::any b = a;
    {
        std::any c = std::move(a);
    }
    return small_but_nontrivial{};
}
