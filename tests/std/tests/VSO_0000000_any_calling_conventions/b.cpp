// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "ab.hpp"
#include <any>
#include <utility>

std::any __cdecl f(std::any a) {
    std::any b = a;
    { std::any c = std::move(a); }
    return small_but_nontrivial{};
}
