// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <../../../../Auxiliary/VS/include/CppCoreCheck/Warnings.h>

#pragma warning(disable : ALL_CPPCORECHECK_WARNINGS)

// clang-format off

// TRANSITION, DevCom-1407027
#pragma warning(disable : 26457) // (void) should not be used to ignore return values, use 'std::ignore =' instead (es.48)

#pragma warning(default : 26433) // Function '%1$s' should be marked with 'override'(c .128).

// TODO: add C:\Program Files\Microsoft Visual Studio\2022\Preview\VC\Tools\MSVC\14.30.30423\include to %caexcludepath%
//#pragma warning(default : 26435) // Function '%1$s' should specify exactly one of 'virtual', 'override', or 'final' (c.128).

#pragma warning(default : 26437) // Do not slice (es.63)
#pragma warning(default : 26441) // Guard objects must be named (cp.44)
#pragma warning(default : 26444) // Don't try to declare a local variable with no name (es.84)
#pragma warning(default : 26471) // Don't use reinterpret_cast. A cast from void* can use static_cast (type.1).
#pragma warning(default : 26475) // Do not use function style C-casts (es.49).
#pragma warning(default : 26477) // Use 'nullptr' rather than 0 or NULL (es.47).
#pragma warning(default : 26478) // Don't use std::move on constant variables. (es.56).
#pragma warning(default : 26479) // Don't use std::move to return a local variable. (f.48).
#pragma warning(default : 26483) // Value %1$lld is outside the bounds (0, %2$lld) of variable '%3$s'.
                                 // Only index into arrays using constant expressions that are within bounds of the array (bounds.2).
#pragma warning(default : 26486) // Don't pass a pointer that may be invalid to a function. Parameter %1$d '%2$s' in call to '%3$s' may be invalid (lifetime.3).
#pragma warning(default : 26487) // Don't return a pointer '%1$s' that may be invalid (lifetime.4).
#pragma warning(default : 26488) // Do not dereference a potentially null pointer: '%1$s'. '%2$s' was null at line %3$u (lifetime.1).
#pragma warning(default : 26489) // Don't dereference a pointer that may be invalid: '%1$s'. '%2$s' may have been invalidated at line %3$u (lifetime.1).
#pragma warning(default : 26814) // The const variable '%1$s' can be computed at compile-time. Consider using constexpr (con.5).
#pragma warning(default : 26815) // The pointer is dangling because it points at a temporary instance which was destroyed.
#pragma warning(default : 26816) // The pointer points to memory allocated on the stack.
#pragma warning(default : 26817) // Potentially expensive copy of variable '%1$s' in range-for loop. Consider making it a const reference (es.71).
// clang-format on

#define MEOW_ANGLE(X) <X>

#include MEOW_ANGLE(MEOW_HEADER)

int main() noexcept {} // COMPILE-ONLY
