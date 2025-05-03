// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <crtdefs.h>
#include <pplinterface.h>
#include <ppltasks.h>

#include <Windows.h>

#if defined(_CRT_APP) || defined(UNDOCKED_WINDOWS_UCRT)
#ifndef UNDOCKED_WINDOWS_UCRT
#pragma warning(push)
#pragma warning(disable : 4265) // non-virtual destructor in base class
#endif
#include <wrl.h>
#ifndef UNDOCKED_WINDOWS_UCRT
#pragma warning(pop)
#endif
#include <ctxtcall.h>
#include <functional>
#include <stdexcept>
#include <windows.foundation.diagnostics.h>
#endif

namespace Concurrency {

    namespace details {
        [[noreturn]] _CRTIMP2 void __cdecl _ReportUnobservedException() {
            __fastfail(FAST_FAIL_INVALID_ARG);
        }

        namespace platform {

            _CRTIMP2 long __cdecl GetCurrentThreadId() {
                return static_cast<long>(::GetCurrentThreadId());
            }

            /// <summary>
            ///     CRT CaptureStackBackTrace API wrapper
            /// </summary>
            _CRTIMP2 size_t __cdecl CaptureCallstack(void** stackData, size_t skipFrames, size_t captureFrames) {
                return RtlCaptureStackBackTrace(
                    static_cast<DWORD>(skipFrames + 1), static_cast<DWORD>(captureFrames), stackData, nullptr);
            }

            static unsigned int s_asyncId = 0;

            _CRTIMP2 unsigned int __cdecl GetNextAsyncId() {
                return static_cast<unsigned int>(::_InterlockedIncrement(reinterpret_cast<volatile LONG*>(&s_asyncId)));
            }

        } // namespace platform

        _CRTIMP2 void __thiscall _TaskEventLogger::_LogScheduleTask(bool) {}

        _CRTIMP2 void __thiscall _TaskEventLogger::_LogTaskCompleted() {}

        _CRTIMP2 void __thiscall _TaskEventLogger::_LogCancelTask() {}

        _CRTIMP2 void __thiscall _TaskEventLogger::_LogTaskExecutionCompleted() {}

        _CRTIMP2 void __thiscall _TaskEventLogger::_LogWorkItemStarted() {}

        _CRTIMP2 void __thiscall _TaskEventLogger::_LogWorkItemCompleted() {}

#if defined(_CRT_APP) || defined(UNDOCKED_WINDOWS_UCRT)
        using namespace ABI::Windows::Foundation;
        using namespace ABI::Windows::Foundation::Diagnostics;
        using namespace Microsoft::WRL;
        using namespace Microsoft::WRL::Wrappers;

        // This IID is exported by ole32.dll; we cannot depend on ole32.dll.
        static GUID const Local_IID_ICallbackWithNoReentrancyToApplicationSTA = {
            0x0A299774, 0x3E4E, 0xFC42, {0x1D, 0x9D, 0x72, 0xCE, 0xE1, 0x05, 0xCA, 0x57}};

        static HRESULT __stdcall _PPLTaskContextCallbackBridge(ComCallData* _PParam) {
            auto pFunc = static_cast<std::function<void()>*>(_PParam->pUserDefined);
            (*pFunc)();
            return S_OK;
        }
        _CRTIMP2 void __thiscall _ContextCallback::_CallInContext(
            _CallbackFunction _Func, bool _IgnoreDisconnect) const {
            if (!_HasCapturedContext()) {
                _Func();
            } else {
                ComCallData callData;
                ZeroMemory(&callData, sizeof(callData));
                callData.pUserDefined = &_Func;

                HRESULT hresult = static_cast<IContextCallback*>(_M_context._M_pContextCallback)
                                      ->ContextCallback(&_PPLTaskContextCallbackBridge, &callData,
                                          Local_IID_ICallbackWithNoReentrancyToApplicationSTA, 5, nullptr);
                if (FAILED(hresult) && !(_IgnoreDisconnect && _IsHRCOMDisconnected(hresult))) {
                    throw std::runtime_error("Context callback failed.");
                }
            }
        }
        _CRTIMP2 void __thiscall _ContextCallback::_Capture() {
            HRESULT _Hr = CoGetObjectContext(__uuidof(IContextCallback), &_M_context._M_pContextCallback);
            if (FAILED(_Hr)) {
                _M_context._M_pContextCallback = nullptr;
            }
        }

        _CRTIMP2 void __thiscall _ContextCallback::_Reset() {
            if (_M_context._M_captureMethod != _S_captureDeferred && _M_context._M_pContextCallback != nullptr) {
                static_cast<IContextCallback*>(_M_context._M_pContextCallback)->Release();
            }
        }

        _CRTIMP2 void __thiscall _ContextCallback::_Assign(void* _PContextCallback) {
            _M_context._M_pContextCallback = _PContextCallback;
            if (_M_context._M_captureMethod != _S_captureDeferred && _M_context._M_pContextCallback != nullptr) {
                static_cast<IContextCallback*>(_M_context._M_pContextCallback)->AddRef();
            }
        }

        _CRTIMP2 bool __cdecl _ContextCallback::_IsCurrentOriginSTA() {
            APTTYPE _AptType;
            APTTYPEQUALIFIER _AptTypeQualifier;

            HRESULT hr = CoGetApartmentType(&_AptType, &_AptTypeQualifier);
            if (SUCCEEDED(hr)) {
                // We determine the origin of a task continuation by looking at where .then is called, so we can tell
                // whether to need to marshal the continuation back to the originating apartment. If an STA thread is in
                // executing in a neutral apartment when it schedules a continuation, we will not marshal continuations
                // back to the STA, since variables used within a neutral apartment are expected to be apartment
                // neutral.
                switch (_AptType) {
                case APTTYPE_MAINSTA:
                case APTTYPE_STA:
                    return true;
                default:
                    break;
                }
            }
            return false;
        }

        _CRTIMP2 bool __cdecl _Task_impl_base::_IsNonBlockingThread() {
            APTTYPE _AptType;
            APTTYPEQUALIFIER _AptTypeQualifier;

            HRESULT hr = CoGetApartmentType(&_AptType, &_AptTypeQualifier);
            //
            // If it failed, it's not a Windows Runtime/COM initialized thread. This is not a failure.
            //
            if (SUCCEEDED(hr)) {
                switch (_AptType) {
                case APTTYPE_STA:
                case APTTYPE_MAINSTA:
                    return true;
                case APTTYPE_NA:
                    switch (_AptTypeQualifier) {
                        // A thread executing in a neutral apartment is either STA or MTA. To find out if this thread is
                        // allowed to wait, we check the app qualifier. If it is an STA thread executing in a neutral
                        // apartment, waiting is illegal, because the thread is responsible for pumping messages and
                        // waiting on a task could take the thread out of circulation for a while.
                    case APTTYPEQUALIFIER_NA_ON_STA:
                    case APTTYPEQUALIFIER_NA_ON_MAINSTA:
                        return true;
                    }
                    break;
                }
            }
            return false;
        }

#else // ^^^ defined(_CRT_APP) || defined(UNDOCKED_WINDOWS_UCRT)
      //                                         / !defined(_CRT_APP) && !defined(UNDOCKED_WINDOWS_UCRT) vvv
        _CRTIMP2 void __thiscall _ContextCallback::_CallInContext(_CallbackFunction _Func, bool) const {
            _Func();
        }

        _CRTIMP2 void __thiscall _ContextCallback::_Capture() {}

        _CRTIMP2 void __thiscall _ContextCallback::_Reset() {}

        _CRTIMP2 void __thiscall _ContextCallback::_Assign(void*) {}

        _CRTIMP2 bool __cdecl _ContextCallback::_IsCurrentOriginSTA() {
            return false;
        }

        _CRTIMP2 bool __cdecl _Task_impl_base::_IsNonBlockingThread() {
            return false;
        }
#endif // ^^^ !defined(_CRT_APP) && !defined(UNDOCKED_WINDOWS_UCRT) ^^^
    } // namespace details

#ifdef _CRT_APP
    _CRTIMP2 __thiscall task_continuation_context::task_continuation_context()
        : _ContextCallback(true), _M_RunInline(false) {}
#else // ^^^ defined(_CRT_APP) / !defined(_CRT_APP) vvv
    _CRTIMP2 __thiscall task_continuation_context::task_continuation_context()
        : _ContextCallback(false), _M_RunInline(false) {}
#endif // ^^^ !defined(_CRT_APP) ^^^

} // namespace Concurrency
