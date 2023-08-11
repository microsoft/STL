// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <yvals.h>

#include <cstdlib>

#include <Windows.h>

_EXTERN_C_UNLESS_PURE

using _Rmtx = CRITICAL_SECTION;

_MRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Mtxinit(_Rmtx*) noexcept;
_MRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Mtxdst(_Rmtx*) noexcept;
_MRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Mtxlock(_Rmtx*) noexcept;
_MRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Mtxunlock(_Rmtx*) noexcept;

_END_EXTERN_C_UNLESS_PURE
