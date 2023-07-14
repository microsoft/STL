// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define ISA_AVAILABILITY delete
#define nsec             delete
#define sec              delete
#define xtime            delete
#define xtime_get        delete

// Also test GH-2645: <yvals_core.h>: Conformance issue on [[msvc::known_semantics]]
#define msvc            1
#define known_semantics 2
#define intrinsic       3
#define lifetimebound   4
#define noop_dtor       5

#include <__msvc_all_public_headers.hpp>

#if msvc != 1
#error bad macro expansion
#endif // msvc != 1

#if known_semantics != 2
#error bad macro expansion
#endif // known_semantics != 2

#if intrinsic != 3
#error bad macro expansion
#endif // intrinsic != 3

#if lifetimebound != 4
#error bad macro expansion
#endif // lifetimebound != 4

#if noop_dtor != 5
#error bad macro expansion
#endif // noop_dtor != 5
