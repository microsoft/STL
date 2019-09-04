// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XMTX
#define _XMTX
#include <yvals.h>

#include <stdlib.h>

#include <Windows.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, 3)
#pragma push_macro("new")
#undef new

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

#pragma pop_macro("new")
#pragma warning(pop)
#pragma pack(pop)
#endif // _XMTX
