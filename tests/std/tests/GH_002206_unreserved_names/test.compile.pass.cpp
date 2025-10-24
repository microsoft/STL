// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define ISA_AVAILABILITY delete
#define error_parse      delete
#define error_syntax     delete
#define nsec             delete
#define sec              delete
#define xtime            delete
#define xtime_get        delete

#define discard_block       delete
#define linear_congruential delete
#define mersenne_twister    delete
#define subtract_with_carry delete
#define uniform_int         delete
#define uniform_real        delete

// Test workaround for extensions of non-reserved names that can't be removed at this moment.
#define raw_name 1001

#define ipfx 1002
#define isfx 1003

#define opfx 1004
#define osfx 1005

#define checked_array_iterator        1006
#define make_checked_array_iterator   1007
#define make_unchecked_array_iterator 1008
#define unchecked_array_iterator      1009

// Also test GH-2645: <yvals_core.h>: Conformance issue on [[msvc::known_semantics]]
#define msvc               1
#define known_semantics    2
#define intrinsic          3
#define lifetimebound      4
#define noop_dtor          5
#define empty_bases        6
#define no_specializations 7

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

#if empty_bases != 6
#error bad macro expansion
#endif // empty_bases != 6

#if no_specializations != 7
#error bad macro expansion
#endif // no_specializations != 7

#if raw_name != 1001
#error bad macro expansion
#endif // raw_name != 1001

#if ipfx != 1002
#error bad macro expansion
#endif // ipfx != 1002

#if isfx != 1003
#error bad macro expansion
#endif // isfx != 1003

#if opfx != 1004
#error bad macro expansion
#endif // opfx != 1004

#if osfx != 1005
#error bad macro expansion
#endif // osfx != 1005

#if checked_array_iterator != 1006
#error bad macro expansion
#endif // checked_array_iterator != 1006

#if make_checked_array_iterator != 1007
#error bad macro expansion
#endif // make_checked_array_iterator != 1007

#if make_unchecked_array_iterator != 1008
#error bad macro expansion
#endif // make_unchecked_array_iterator != 1008

#if unchecked_array_iterator != 1009
#error bad macro expansion
#endif // unchecked_array_iterator != 1009
