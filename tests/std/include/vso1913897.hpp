// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#ifndef _M_CEE

// TRANSITION, dynamically initialize a thread_local to workaround VSO-1913897
inline int __stl_asan_init_function() {
    static volatile int __stl_asan_init_volatile = 42;
    return __stl_asan_init_volatile;
}
static thread_local int __stl_asan_init_variable = __stl_asan_init_function();

#endif // _M_CEE
