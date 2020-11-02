// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#ifdef __STL_IS_KERNEL
#include <ntddk.h>
#else
#include <winioctl.h>
#endif

#define IOCTL_SIOCTL_METHOD_RUN_TEST CTL_CODE(40000, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct TestResults {
    int main_return;
    long tests_failed;
    char output[4088];
} TestResults;
