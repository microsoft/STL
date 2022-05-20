// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <yvals.h>

#if defined(_M_IX86) | defined(_M_X64)
extern "C" int __declspec(dllimport) __stdcall __std_init_once_begin_initialize(void**, unsigned long, int*, void**);
#pragma comment(linker, "/INCLUDE:__imp___std_init_once_begin_initialize")
#endif

int main() {
    return 0;
}
