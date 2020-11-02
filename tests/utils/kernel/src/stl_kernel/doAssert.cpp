// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "test_decls.h"
#include <Ntstrsafe.h>
#include <ntddk.h>

KGUARDED_MUTEX g_assert_mutex;
extern "C" {
long* g_test_failures;
char* g_output_buffer;
size_t g_space_available;
}

static const char ASSERTION_FAILED[]     = "assertion failed ";
static const size_t ASSERTION_FAILED_LEN = sizeof(ASSERTION_FAILED) - 1;
extern "C" void doAssert(const char* file, int line, const char* expr) {
    KeAcquireGuardedMutex(&g_assert_mutex);
    ++(*g_test_failures);

    RtlStringCbPrintfExA(g_output_buffer, g_space_available, &g_output_buffer, &g_space_available,
        0, // flags
        "assertion failed %s(%d): %s\n", file, line, expr);

    KeReleaseGuardedMutex(&g_assert_mutex);
}
