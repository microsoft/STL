// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#include <ntddk.h>

extern const wchar_t* const STL_KERNEL_NT_DEVICE_NAME;
extern const wchar_t* const STL_KERNEL_DOS_DEVICE_NAME;
extern KGUARDED_MUTEX g_assert_mutex;

extern "C" int main();
