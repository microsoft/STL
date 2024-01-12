// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <stdlib.h>

// TRANSITION, dynamically initialize a thread_local to workaround VSO-1913897
static thread_local int stl_asan_init_hack = ::rand();
