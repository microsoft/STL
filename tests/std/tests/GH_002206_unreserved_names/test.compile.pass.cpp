// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define ISA_AVAILABILITY delete
#define nsec             delete
#define sec              delete
#define xtime            delete
#define xtime_get        delete

// Test workaround for extensions of non-reserved names that can't be removed at this moment.
#define terminate_function  1001
#define unexpected_function 1002

#define raw_name 1003

#define ipfx 1004
#define isfx 1005

#define opfx 1006
#define osfx 1007

#define checked_array_iterator        1008
#define make_checked_array_iterator   1009
#define make_unchecked_array_iterator 1010
#define unchecked_array_iterator      1011

#define discard_block       1012
#define linear_congruential 1013
#define mersenne_twister    1014
#define subtract_with_carry 1015
#define uniform_int         1016
#define uniform_real        1017

// Also test GH-2645: <yvals_core.h>: Conformance issue on [[msvc::known_semantics]]
#define msvc            1
#define known_semantics 2
#define intrinsic       3
#define lifetimebound   4
#define noop_dtor       5
#define empty_bases     6

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

#if terminate_function != 1001
#error bad macro expansion
#endif // terminate_function != 1001

#if unexpected_function != 1002
#error bad macro expansion
#endif // unexpected_function != 1002

#if raw_name != 1003
#error bad macro expansion
#endif // raw_name != 1003

#if ipfx != 1004
#error bad macro expansion
#endif // ipfx != 1004

#if isfx != 1005
#error bad macro expansion
#endif // raw_name != 1005

#if opfx != 1006
#error bad macro expansion
#endif // raw_name != 1006

#if osfx != 1007
#error bad macro expansion
#endif // raw_name != 1007

#if checked_array_iterator != 1008
#error bad macro expansion
#endif // checked_array_iterator != 1008

#if make_checked_array_iterator != 1009
#error bad macro expansion
#endif // make_checked_array_iterator != 1009

#if make_unchecked_array_iterator != 1010
#error bad macro expansion
#endif // make_unchecked_array_iterator != 1010

#if unchecked_array_iterator != 1011
#error bad macro expansion
#endif // unchecked_array_iterator != 1011

#if discard_block != 1012
#error bad macro expansion
#endif // discard_block != 1012

#if linear_congruential != 1013
#error bad macro expansion
#endif // linear_congruential != 1013

#if mersenne_twister != 1014
#error bad macro expansion
#endif // mersenne_twister != 1014

#if subtract_with_carry != 1015
#error bad macro expansion
#endif // subtract_with_carry != 1015

#if uniform_int != 1016
#error bad macro expansion
#endif // uniform_int != 1016

#if uniform_real != 1017
#error bad macro expansion
#endif // uniform_real != 1017
