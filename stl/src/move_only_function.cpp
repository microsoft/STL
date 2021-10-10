// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <synchapi.h>

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

// these declarations must be in sync with those in <function>

extern "C" {

void __stdcall __std_function_noop_move(uintptr_t*, uintptr_t*) noexcept {}
void __stdcall __std_function_noop_destroy(uintptr_t*) noexcept {}

void __stdcall __std_function_move_large(uintptr_t* _Self, uintptr_t* _Src) {
    *_Self = *_Src;
}
}
