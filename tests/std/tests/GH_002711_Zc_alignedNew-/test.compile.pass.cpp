// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// GH-2711 <functional>: incompatible with /std:c++latest and /Zc:alignedNew-
// Just test every header can be parsed with /Zc:alignedNew and /Zc:alignedNew-
#include <__msvc_all_public_headers.hpp>

int main() {} // COMPILE-ONLY
