// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#if BOTH
#include <cmath>
#include <cstdlib>

#include "type_asserts.hpp"
#elif CMATH
#include "cmath.cpp"
#elif CSTDLIB
#include "cstdlib.cpp"
#endif

int main() {} // COMPILE-ONLY
