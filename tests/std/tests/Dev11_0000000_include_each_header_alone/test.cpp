// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef MEOW_DONT_FAKE_STD

#include <vcruntime_new.h>

namespace X::std {}
using namespace X;

#endif // ! MEOW_DONT_FAKE_STD

#define MEOW_ANGLE(X) <X>

#include MEOW_ANGLE(MEOW_HEADER)

int main() {} // COMPILE-ONLY
