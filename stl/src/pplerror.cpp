// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <ppltasks.h>

#if defined(_CRT_APP) || defined(UNDOCKED_WINDOWS_UCRT)

#include <RestrictedErrorInfo.h>
#include <ehdata.h>
#include <exception>
#include <roerrorapi.h>
#include <trnsctrl.h>

extern "C" void* __GetPlatformExceptionInfo(int*);

namespace Concurrency {
    namespace details {

        _CRTIMP2 void __thiscall _ExceptionHolder::ReportUnhandledError() {
            if (_M_stdException) {
                try {
                    std::rethrow_exception(_M_stdException);
                } catch (...) {
                    int isBadAlloc;
                    auto info = static_cast<WINRTEXCEPTIONINFO*>(__GetPlatformExceptionInfo(&isBadAlloc));
                    // If it is a winrt exception
                    if (!isBadAlloc && info != nullptr) {
                        auto restrictedInfo(static_cast<IRestrictedErrorInfo*>(info->restrictedInfo));
                        if (restrictedInfo != nullptr) {
#ifndef _M_ARM64 // CRT_REFACTOR TODO
                            ::RoReportUnhandledError(restrictedInfo);
#endif // _M_ARM64

                            // IRestrictedErrorInfo should be put back
                            // to make sure that RoFailFastWithErrorContext can extract the information
                            ::SetRestrictedErrorInfo(restrictedInfo);
                        }
                        ::RoFailFastWithErrorContext(info->hr);
                    }
                }
            }
        }

    } // namespace details
} // namespace Concurrency

#else // defined(_CRT_APP) || defined(UNDOCKED_WINDOWS_UCRT)

namespace Concurrency {
    namespace details {

        _CRTIMP2 void __thiscall _ExceptionHolder::ReportUnhandledError() {}

    } // namespace details
} // namespace Concurrency

#endif // defined(_CRT_APP) || defined(UNDOCKED_WINDOWS_UCRT)
