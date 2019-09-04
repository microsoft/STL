// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// TRANSITION, ABI: The functions in this file are preserved for binary compatibility

#include "awint.h"
#include <stdexcept>

#include <Windows.h>

namespace stdext {
    namespace threads {

        _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Mtx_new(void*& _Ptr) {
            _Ptr = new CRITICAL_SECTION;
            __crtInitializeCriticalSectionEx(static_cast<CRITICAL_SECTION*>(_Ptr), 4000, 0);
        }

        _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Mtx_delete(void* _Ptr) {
            DeleteCriticalSection(static_cast<CRITICAL_SECTION*>(_Ptr));
            delete static_cast<CRITICAL_SECTION*>(_Ptr);
        }

        _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Mtx_lock(void* _Ptr) {
            EnterCriticalSection(static_cast<CRITICAL_SECTION*>(_Ptr));
        }

        _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Mtx_unlock(void* _Ptr) {
            LeaveCriticalSection(static_cast<CRITICAL_SECTION*>(_Ptr));
        }

    } // namespace threads
} // namespace stdext
