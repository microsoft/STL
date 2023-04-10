// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// REQUIRES: x64 || x86

#include <yvals.h>

#if defined(_M_IX86) || defined(_M_X64)
extern "C" int __declspec(dllimport) __stdcall __std_init_once_begin_initialize(void**, unsigned long, int*, void**);
extern "C" int __declspec(dllimport) __stdcall __std_init_once_complete(void**, unsigned long, void*);
#endif // defined(_M_IX86) || defined(_M_X64)

int main() {
#if defined(_M_IX86) || defined(_M_X64)
    void* once  = nullptr;
    int pending = 0;
    __std_init_once_begin_initialize(&once, 0, &pending, nullptr);
    __std_init_once_complete(&once, 0, nullptr);
#endif // defined(_M_IX86) || defined(_M_X64)
    return 0;
}
