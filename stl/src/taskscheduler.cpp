// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "awint.h"
#include <condition_variable>
#include <mutex>
#include <ppltaskscheduler.h>
#include <stddef.h>

#include <Windows.h>

#pragma warning(disable : 4074)
#pragma init_seg(compiler)

static std::_Init_locks initlocks;

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace Concurrency {
    namespace details {
        namespace {
            bool _Is_vista_threadpool_supported() {
#if _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA
                return true;
#else // ^^^ _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA ^^^ // vvv _STL_WIN32_WINNT < _WIN32_WINNT_VISTA vvv
                DYNAMICGETCACHEDFUNCTION(PFNCREATETHREADPOOLWORK, CreateThreadpoolWork, pfCreateThreadpoolWork);
                return pfCreateThreadpoolWork != nullptr;
#endif // _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA
            }

            // When the CRT and STL are statically linked into an EXE, their uninitialization will take place
            // inside of the call to exit(), before ExitProcess() is called.  This means that their
            // uninitialization occurs before other threads in the process are terminated.  We block the exit
            // from proceeding until all outstanding tasks have completed, to ensure that they do not run after
            // we destroy the STL's internal locks.

            // When the CRT and STL are hosted in DLLs (either in their own DLLs or statically linked into a
            // user DLL), they are uninitialized when the DLL is notified for DLL_PROCESS_DETACH before it is
            // unloaded.  Termination unload occurs after all other threads in the process have been terminated,
            // so there is no risk of other threads touching internal STL state.  We prevent non-termination
            // unload from occurring while there are outstanding tasks, by having each task own a reference to
            // the DLL in which the callback is located.

            HMODULE _Call_get_module_handle_ex(DWORD _Flags, LPCWSTR _Addr) {
#if defined(_CRT_APP)
                // We can't call GetModuleHandleExW from an app context, so treat
                // that as a failure to call.
                (void) _Flags;
                (void) _Addr;
                return 0;
#else // ^^^ defined(_CRT_APP) ^^^ // vvv !defined(_CRT_APP) vvv
                HMODULE _Result;
                if (::GetModuleHandleExW(_Flags, _Addr, &_Result) == 0) {
                    return 0;
                }

                return _Result;
#endif // defined(_CRT_APP)
            }

            enum class _STL_host_status { _Exe, _Dll, _Unknown };

            _STL_host_status _Get_STL_host_status() {
#ifdef CRTDLL2
                return _STL_host_status::_Dll;
#else // ^^^ CRTDLL2 ^^^ // vvv !CRTDLL2 vvv
                HANDLE _HExe = _Call_get_module_handle_ex(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, nullptr);
                if (_HExe == 0) {
                    return _STL_host_status::_Unknown;
                } else if (_HExe == reinterpret_cast<HMODULE>(&__ImageBase)) {
                    return _STL_host_status::_Exe;
                } else {
                    return _STL_host_status::_Dll;
                }
#endif // CRTDLL2
            }

#ifdef CRTDLL2
            // If the STL is a DLL, no reference counting is necessary, because the CRT shutdown is
            // always through DLL_PROCESS_DETACH, so keeping the owning reference to the callback
            // code is sufficient.
            void _Increment_outstanding() {}
            void _Decrement_outstanding() {}
#else // ^^^ CRTDLL2 ^^^ // vvv !CRTDLL2 vvv
            size_t _Outstanding_tasks = 0;
            _STD mutex _Task_cv_mutex;
            _STD condition_variable _Task_cv;

            void _Increment_outstanding() { // block shutdown
                if (_Get_STL_host_status() == _STL_host_status::_Dll) {
                    return;
                }

                _STD lock_guard<_STD mutex> _Lg(_Task_cv_mutex);
                ++_Outstanding_tasks;
            }

            void _Decrement_outstanding() { // release shutdown
                if (_Get_STL_host_status() == _STL_host_status::_Dll) {
                    return;
                }

                size_t _Dec_outstanding;
                {
                    _STD lock_guard<_STD mutex> _Lg(_Task_cv_mutex);
                    _Dec_outstanding = --_Outstanding_tasks;
                }

                if (_Dec_outstanding == 0) {
                    _Task_cv.notify_all();
                }
            }

            struct _Task_scheduler_main_block {
                _Task_scheduler_main_block() = default;
                _Task_scheduler_main_block(const _Task_scheduler_main_block&) = delete;
                _Task_scheduler_main_block& operator=(const _Task_scheduler_main_block&) = delete;
                ~_Task_scheduler_main_block() noexcept { // block shutdown of the CRT until std::async shutdown has
                                                         // completed
                    _STD unique_lock<_STD mutex> _Lck(_Task_cv_mutex);
                    _Task_cv.wait(_Lck, [] { return _Outstanding_tasks == 0; });
                }
            } _Task_scheduler_main_block_instance;
#endif // CRTDLL2

            void CALLBACK _Task_scheduler_callback(PTP_CALLBACK_INSTANCE _Pci, PVOID _Args, PTP_WORK) noexcept {
                _Increment_outstanding();
                const auto _Chore = static_cast<_Threadpool_chore*>(_Args);
                if (_Get_STL_host_status() != _STL_host_status::_Exe) { // ensure user code held alive by
                                                                        // _Reschedule_chore is freed when we're done
                    const HMODULE _Callback_dll = _Call_get_module_handle_ex(
                        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                        reinterpret_cast<LPCWSTR>(_Chore->_M_callback));

                    if (_Callback_dll != 0) {
                        __crtFreeLibraryWhenCallbackReturns(_Pci, _Callback_dll);
                    }
                }

                _Chore->_M_callback(_Chore->_M_data);
                _Decrement_outstanding();
            }

            DWORD __stdcall _Task_scheduler_callback_xp(LPVOID _Args) noexcept {
                _Increment_outstanding();
                const auto _Chore = static_cast<_Threadpool_chore*>(_Args);
                _Chore->_M_callback(_Chore->_M_data);
                _Decrement_outstanding();
                return 0;
            }
        } // namespace

        _CRTIMP2 void __cdecl _Release_chore(_Threadpool_chore* _Chore) {
            if (_Chore->_M_work != nullptr) {
                // Windows XP threadpool doesn't need to release chore
                if (_Is_vista_threadpool_supported()) {
                    __crtCloseThreadpoolWork(static_cast<PTP_WORK>(_Chore->_M_work));
                }
                _Chore->_M_work = nullptr;
            }
        }

        _CRTIMP2 int __cdecl _Reschedule_chore(const _Threadpool_chore* _Chore) {
            // reschedule supports only Windows Vista and above
            _ASSERT(_Is_vista_threadpool_supported());
            _ASSERT(_Chore->_M_work);

            // Adds a reference to the DLL with the code to execute on async; the callback will
            // FreeLibraryWhenCallbackReturns this DLL once it starts running.
            if (_Get_STL_host_status() != _STL_host_status::_Exe) {
                (void) _Call_get_module_handle_ex(
                    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCWSTR>(_Chore->_M_callback));
            }

            __crtSubmitThreadpoolWork(static_cast<PTP_WORK>(_Chore->_M_work));
            return 0;
        }

        _CRTIMP2 int __cdecl _Schedule_chore(_Threadpool_chore* _Chore) {
            _ASSERT(_Chore->_M_work == nullptr);
            _ASSERT(_Chore->_M_callback != nullptr);

            if (_Is_vista_threadpool_supported()) {
                _Chore->_M_work = __crtCreateThreadpoolWork(_Task_scheduler_callback, _Chore, nullptr);

                if (_Chore->_M_work) {
                    return _Reschedule_chore(_Chore);
                } else {
                    return static_cast<int>(GetLastError()); // LastError won't be 0 when it's in error state
                }
            } else {
                // Windows XP doesn't support FreeLibraryWhenCallbackReturns,
                // so we prevent the callback DLL from ever unloading
                if (_Get_STL_host_status() != _STL_host_status::_Exe) {
                    (void) _Call_get_module_handle_ex(
                        GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                        reinterpret_cast<LPCWSTR>(_Chore->_M_callback));
                }

                _Chore->_M_work = _Chore; // give a dummy non-null worker
                if (__crtQueueUserWorkItem(_Task_scheduler_callback_xp, _Chore, WT_EXECUTEDEFAULT) == 0) {
                    _Chore->_M_work = nullptr;
                    return static_cast<int>(GetLastError()); // LastError won't be 0 when it's in error state
                } else {
                    return 0;
                }
            }
        }
    } // namespace details
} // namespace Concurrency
