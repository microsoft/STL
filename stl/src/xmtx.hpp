// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <yvals.h>

#include <cstdlib>

#include <Windows.h>

#ifdef _M_CEE
#define _RELIABILITY_CONTRACT                                                    \
    [System::Runtime::ConstrainedExecution::ReliabilityContract(                 \
        System::Runtime::ConstrainedExecution::Consistency::WillNotCorruptState, \
        System::Runtime::ConstrainedExecution::Cer::Success)]
#else // ^^^ defined(_M_CEE) / !defined(_M_CEE) vvv
#define _RELIABILITY_CONTRACT
#endif // ^^^ !defined(_M_CEE) ^^^

_EXTERN_C_UNLESS_PURE

using _Rmtx = CRITICAL_SECTION;

_MRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Mtxinit(_Rmtx*) noexcept;
_MRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Mtxdst(_Rmtx*) noexcept;
_MRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Mtxlock(_Rmtx*) noexcept;
_MRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Mtxunlock(_Rmtx*) noexcept;

_END_EXTERN_C_UNLESS_PURE
