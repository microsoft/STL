// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#pragma warning(push)
#pragma warning(disable : 4201) // nonstandard extension used: nameless struct/union
#pragma warning(disable : 4324) // structure was padded due to alignment specifier

#include "awint.h"
#include <crtdefs.h>
#include <exception>
#include <new>
#include <stdlib.h>

#include <Windows.h>

#ifdef _STL_CONCRT_SUPPORT
#include <concrt.h>
#include <concrtinternal.h>
#endif

enum class __stl_sync_api_modes_enum { normal, win7, vista, concrt };

extern __stl_sync_api_modes_enum __stl_sync_api_impl_mode;

namespace Concurrency {
    namespace details {
        class __declspec(novtable) stl_critical_section_interface {
        public:
            virtual void lock()                     = 0;
            virtual bool try_lock()                 = 0;
            virtual bool try_lock_for(unsigned int) = 0;
            virtual void unlock()                   = 0;
            virtual void destroy()                  = 0;
        };

        class __declspec(novtable) stl_condition_variable_interface {
        public:
            virtual void wait(stl_critical_section_interface*)                   = 0;
            virtual bool wait_for(stl_critical_section_interface*, unsigned int) = 0;
            virtual void notify_one()                                            = 0;
            virtual void notify_all()                                            = 0;
            virtual void destroy()                                               = 0;
        };

#pragma warning(push)
#pragma warning(disable : 4265) // non-virtual destructor in base class
        class stl_critical_section_vista final : public stl_critical_section_interface {
        public:
            stl_critical_section_vista() {
                __crtInitializeCriticalSectionEx(&_M_critical_section, 4000, 0);
            }

            stl_critical_section_vista(const stl_critical_section_vista&) = delete;
            stl_critical_section_vista& operator=(const stl_critical_section_vista&) = delete;
            ~stl_critical_section_vista()                                            = delete;

            virtual void destroy() override {
                DeleteCriticalSection(&_M_critical_section);
            }

            virtual void lock() override {
                EnterCriticalSection(&_M_critical_section);
            }

            virtual bool try_lock() override {
                return TryEnterCriticalSection(&_M_critical_section) != 0;
            }

            virtual bool try_lock_for(unsigned int) override {
                // STL will call try_lock_for once again if this call will not succeed
                return stl_critical_section_vista::try_lock();
            }

            virtual void unlock() override {
                LeaveCriticalSection(&_M_critical_section);
            }

            LPCRITICAL_SECTION native_handle() {
                return &_M_critical_section;
            }

        private:
            CRITICAL_SECTION _M_critical_section;
        };

        class stl_condition_variable_vista final : public stl_condition_variable_interface {
        public:
            stl_condition_variable_vista() {
                __crtInitializeConditionVariable(&m_condition_variable);
            }

            ~stl_condition_variable_vista()                                   = delete;
            stl_condition_variable_vista(const stl_condition_variable_vista&) = delete;
            stl_condition_variable_vista& operator=(const stl_condition_variable_vista&) = delete;

            virtual void destroy() override {}

            virtual void wait(stl_critical_section_interface* lock) override {
                if (!stl_condition_variable_vista::wait_for(lock, INFINITE)) {
                    std::terminate();
                }
            }

            virtual bool wait_for(stl_critical_section_interface* lock, unsigned int timeout) override {
                return __crtSleepConditionVariableCS(&m_condition_variable,
                           static_cast<stl_critical_section_vista*>(lock)->native_handle(), timeout)
                       != 0;
            }

            virtual void notify_one() override {
                __crtWakeConditionVariable(&m_condition_variable);
            }

            virtual void notify_all() override {
                __crtWakeAllConditionVariable(&m_condition_variable);
            }

        private:
            CONDITION_VARIABLE m_condition_variable;
        };

        class stl_critical_section_win7 final : public stl_critical_section_interface {
        public:
            stl_critical_section_win7() {
                __crtInitializeSRWLock(&m_srw_lock);
            }

            ~stl_critical_section_win7()                                = delete;
            stl_critical_section_win7(const stl_critical_section_win7&) = delete;
            stl_critical_section_win7& operator=(const stl_critical_section_win7&) = delete;

            virtual void destroy() override {}

            virtual void lock() override {
                __crtAcquireSRWLockExclusive(&m_srw_lock);
            }

            virtual bool try_lock() override {
                return __crtTryAcquireSRWLockExclusive(&m_srw_lock) != 0;
            }

            virtual bool try_lock_for(unsigned int) override {
                // STL will call try_lock_for once again if this call will not succeed
                return stl_critical_section_win7::try_lock();
            }

            virtual void unlock() override {
                __crtReleaseSRWLockExclusive(&m_srw_lock);
            }

            PSRWLOCK native_handle() {
                return &m_srw_lock;
            }

        private:
            SRWLOCK m_srw_lock;
        };

        class stl_condition_variable_win7 final : public stl_condition_variable_interface {
        public:
            stl_condition_variable_win7() {
                __crtInitializeConditionVariable(&m_condition_variable);
            }

            ~stl_condition_variable_win7()                                  = delete;
            stl_condition_variable_win7(const stl_condition_variable_win7&) = delete;
            stl_condition_variable_win7& operator=(const stl_condition_variable_win7&) = delete;

            virtual void destroy() override {}

            virtual void wait(stl_critical_section_interface* lock) override {
                if (!stl_condition_variable_win7::wait_for(lock, INFINITE)) {
                    std::terminate();
                }
            }

            virtual bool wait_for(stl_critical_section_interface* lock, unsigned int timeout) override {
                return __crtSleepConditionVariableSRW(&m_condition_variable,
                           static_cast<stl_critical_section_win7*>(lock)->native_handle(), timeout, 0)
                       != 0;
            }

            virtual void notify_one() override {
                __crtWakeConditionVariable(&m_condition_variable);
            }

            virtual void notify_all() override {
                __crtWakeAllConditionVariable(&m_condition_variable);
            }

        private:
            CONDITION_VARIABLE m_condition_variable;
        };

#ifdef _STL_CONCRT_SUPPORT
        class stl_critical_section_concrt final : public stl_critical_section_interface {
        public:
            stl_critical_section_concrt()                                   = default;
            ~stl_critical_section_concrt()                                  = delete;
            stl_critical_section_concrt(const stl_critical_section_concrt&) = delete;
            stl_critical_section_concrt& operator=(const stl_critical_section_concrt&) = delete;

            virtual void lock() override {
                m_critical_section.lock();
            }

            virtual bool try_lock() override {
                return m_critical_section.try_lock();
            }

            virtual bool try_lock_for(unsigned int duration) override {
                return m_critical_section.try_lock_for(duration);
            }

            virtual void unlock() override {
                m_critical_section.unlock();
            }

            virtual void destroy() override {
                // the destructor of stl_critical_section_concrt will never be invoked
                m_critical_section.~critical_section();
            }

            critical_section& native_handle() {
                return m_critical_section;
            }

        private:
            critical_section m_critical_section;
        };

        class stl_condition_variable_concrt final : public stl_condition_variable_interface {
        public:
            stl_condition_variable_concrt()                                     = default;
            ~stl_condition_variable_concrt()                                    = delete;
            stl_condition_variable_concrt(const stl_condition_variable_concrt&) = delete;
            stl_condition_variable_concrt& operator=(const stl_condition_variable_concrt&) = delete;

            virtual void wait(stl_critical_section_interface* lock) override {
                m_condition_variable.wait(static_cast<stl_critical_section_concrt*>(lock)->native_handle());
            }

            virtual bool wait_for(stl_critical_section_interface* lock, unsigned int timeout) override {
                return m_condition_variable.wait_for(
                    static_cast<stl_critical_section_concrt*>(lock)->native_handle(), timeout);
            }

            virtual void notify_one() override {
                m_condition_variable.notify_one();
            }

            virtual void notify_all() override {
                m_condition_variable.notify_all();
            }

            virtual void destroy() override {
                // the destructor of stl_condition_variable_concrt will never be invoked
                m_condition_variable.~_Condition_variable();
            }

        private:
            _Condition_variable m_condition_variable;
        };

#endif // _STL_CONCRT_SUPPORT

#pragma warning(pop)

        inline bool are_win7_sync_apis_available() {
#if _STL_WIN32_WINNT >= _WIN32_WINNT_WIN7
            return true;
#else
            // TryAcquireSRWLockExclusive ONLY available on Windows 7+
            DYNAMICGETCACHEDFUNCTION(
                PFNTRYACQUIRESRWLOCKEXCLUSIVE, TryAcquireSRWLockExclusive, pfTryAcquireSRWLockExclusive);
            return pfTryAcquireSRWLockExclusive != nullptr;
#endif
        }

        inline bool are_vista_sync_apis_available() {
#if _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA
            return true;
#else
            // InitializeConditionVariable ONLY available on Windows Vista+
            DYNAMICGETCACHEDFUNCTION(
                PFNINITIALIZECONDITIONVARIABLE, InitializeConditionVariable, pfInitializeConditionVariable);
            return pfInitializeConditionVariable != nullptr;
#endif
        }

        inline void create_stl_critical_section(stl_critical_section_interface* p) {
#ifdef _CRT_WINDOWS
            new (p) stl_critical_section_win7;
#else
            switch (__stl_sync_api_impl_mode) {
            case __stl_sync_api_modes_enum::normal:
            case __stl_sync_api_modes_enum::win7:
                if (are_win7_sync_apis_available()) {
                    new (p) stl_critical_section_win7;
                    return;
                }
                // fall through
            case __stl_sync_api_modes_enum::vista:
                if (are_vista_sync_apis_available()) {
                    new (p) stl_critical_section_vista;
                    return;
                }
                // fall through
            case __stl_sync_api_modes_enum::concrt:
            default:
#ifdef _STL_CONCRT_SUPPORT
                new (p) stl_critical_section_concrt;
                return;
#else
                std::terminate();
#endif // _STL_CONCRT_SUPPORT
            }
#endif // _CRT_WINDOWS
        }

        inline void create_stl_condition_variable(stl_condition_variable_interface* p) {
#ifdef _CRT_WINDOWS
            new (p) stl_condition_variable_win7;
#else
            switch (__stl_sync_api_impl_mode) {
            case __stl_sync_api_modes_enum::normal:
            case __stl_sync_api_modes_enum::win7:
                if (are_win7_sync_apis_available()) {
                    new (p) stl_condition_variable_win7;
                    return;
                }
                // fall through
            case __stl_sync_api_modes_enum::vista:
                if (are_vista_sync_apis_available()) {
                    new (p) stl_condition_variable_vista;
                    return;
                }
                // fall through
            case __stl_sync_api_modes_enum::concrt:
            default:
#ifdef _STL_CONCRT_SUPPORT
                new (p) stl_condition_variable_concrt;
                return;
#else
                std::terminate();
#endif // _STL_CONCRT_SUPPORT
            }
#endif // _CRT_WINDOWS
        }

#if defined _CRT_WINDOWS
        const size_t stl_critical_section_max_size        = sizeof(stl_critical_section_win7);
        const size_t stl_condition_variable_max_size      = sizeof(stl_condition_variable_win7);
        const size_t stl_critical_section_max_alignment   = __alignof(stl_critical_section_win7);
        const size_t stl_condition_variable_max_alignment = __alignof(stl_condition_variable_win7);
#elif defined _STL_CONCRT_SUPPORT
        const size_t stl_critical_section_max_size =
            __max(__max(sizeof(stl_critical_section_concrt), sizeof(stl_critical_section_vista)),
                sizeof(stl_critical_section_win7));
        const size_t stl_condition_variable_max_size =
            __max(__max(sizeof(stl_condition_variable_concrt), sizeof(stl_condition_variable_vista)),
                sizeof(stl_condition_variable_win7));
        const size_t stl_critical_section_max_alignment =
            __max(__max(__alignof(stl_critical_section_concrt), __alignof(stl_critical_section_vista)),
                __alignof(stl_critical_section_win7));
        const size_t stl_condition_variable_max_alignment =
            __max(__max(__alignof(stl_condition_variable_concrt), __alignof(stl_condition_variable_vista)),
                __alignof(stl_condition_variable_win7));
#else
        const size_t stl_critical_section_max_size =
            __max(sizeof(stl_critical_section_vista), sizeof(stl_critical_section_win7));
        const size_t stl_condition_variable_max_size =
            __max(sizeof(stl_condition_variable_vista), sizeof(stl_condition_variable_win7));
        const size_t stl_critical_section_max_alignment =
            __max(__alignof(stl_critical_section_vista), __alignof(stl_critical_section_win7));
        const size_t stl_condition_variable_max_alignment =
            __max(__alignof(stl_condition_variable_vista), __alignof(stl_condition_variable_win7));
#endif
    } // namespace details
} // namespace Concurrency

#pragma warning(pop)
