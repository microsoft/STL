// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <yvals.h>

#include <cstdlib>

#include <Windows.h>

_EXTERN_C_UNLESS_PURE

using _Rmtx = CRITICAL_SECTION;

#ifdef _M_CEE_PURE
void __clrcall _Mtxinit(_Rmtx*) noexcept;
void __clrcall _Mtxdst(_Rmtx*) noexcept;
void __clrcall _Mtxlock(_Rmtx*) noexcept;
void __clrcall _Mtxunlock(_Rmtx*) noexcept;

#else // _M_CEE_PURE
_MRTIMP2 void __cdecl _Mtxinit(_Rmtx*) noexcept;
_MRTIMP2 void __cdecl _Mtxdst(_Rmtx*) noexcept;
_MRTIMP2 void __cdecl _Mtxlock(_Rmtx*) noexcept;
_MRTIMP2 void __cdecl _Mtxunlock(_Rmtx*) noexcept;
#endif // _M_CEE_PURE

_END_EXTERN_C_UNLESS_PURE
