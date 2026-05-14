// __msvc_doom_core.hpp internal header (core)

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef __MSVC_DOOM_CORE_HPP
#define __MSVC_DOOM_CORE_HPP
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#ifdef _MSVC_STL_USE_ABORT_AS_DOOM_FUNCTION
#include <cstdlib>
#elif defined(_M_CEE)
#include <corecrt.h>
#endif // ^^^ defined(_M_CEE) ^^^

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

// The STL's "doom function" can be replaced. Notes:
// * It must not throw. (Attempting to throw would slam into noexcept.)
// * Common case: If it doesn't return, it should be marked as `[[noreturn]]`.
// * Uncommon case: If it returns, the STL will attempt to "continue on error", behaving as if no checking was done.
//   + For example, a legacy codebase with a long startup time might want to log errors for investigation later.
//   + WARNING: If you replace the STL's "doom function" to "continue on error", you do so at your own risk!
//     After the STL has detected a precondition violation, undefined behavior is imminent. The STL will support
//     "continue on error" by proceeding to do what it would have done anyways (instead of falling off the end of
//     a non-void function, etc.), but it will not attempt to replace undefined behavior with implementation-defined
//     behavior. (For example, we will not transform `pop_back()` of an empty `vector` to be a no-op.)
#ifndef _MSVC_STL_DOOM_FUNCTION
#ifdef _MSVC_STL_USE_ABORT_AS_DOOM_FUNCTION // The user wants to use abort():
#define _MSVC_STL_DOOM_FUNCTION(mesg) _CSTD abort()
#elif defined(__clang__) // Use the Clang intrinsic:
#define _MSVC_STL_DOOM_FUNCTION(mesg) __builtin_verbose_trap("MSVC STL error", mesg)
#elif defined(_M_CEE) // TRANSITION, VSO-2457624 (/clr silent bad codegen for __fastfail); /clr:pure lacks __fastfail
#define _MSVC_STL_DOOM_FUNCTION(mesg) ::_invoke_watson(nullptr, nullptr, nullptr, 0, 0)
#else // Use the MSVC __fastfail intrinsic:
extern "C" __declspec(noreturn) void __fastfail(unsigned int); // declared by <intrin.h>
#define _MSVC_STL_DOOM_FUNCTION(mesg)                                                  \
    __fastfail(5); /* __fastfail(FAST_FAIL_INVALID_ARG), value defined by <winnt.h> */ \
    _STL_UNREACHABLE /* TRANSITION, DevCom-10914110 */
#endif // choose "doom function"
#endif // ^^^ !defined(_MSVC_STL_DOOM_FUNCTION) ^^^

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // __MSVC_DOOM_CORE_HPP
