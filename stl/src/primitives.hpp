// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <cstdlib> // for __max
#include <exception>
#include <new>
#include <xthreads.h>

#include "awint.hpp"

enum class __stl_sync_api_modes_enum { normal, win7, vista, concrt };

extern __stl_sync_api_modes_enum __stl_sync_api_impl_mode;

namespace Concurrency {
    namespace details {
        using stl_critical_section_interface = ::_Stl_critical_section_interface;


        class __declspec(novtable) stl_condition_variable_interface {
        public:
            virtual void wait(stl_critical_section_interface*)                   = 0;
            virtual bool wait_for(stl_critical_section_interface*, unsigned int) = 0;
            virtual void notify_one()                                            = 0;
            virtual void notify_all()                                            = 0;
            virtual void destroy()                                               = 0;
        };

        class stl_critical_section_vista final : public stl_critical_section_interface {
        public:
            stl_critical_section_vista() {
                InitializeCriticalSectionEx(&_M_critical_section, 4000, 0);
            }

            stl_critical_section_vista(const stl_critical_section_vista&) = delete;
            stl_critical_section_vista& operator=(const stl_critical_section_vista&) = delete;
            ~stl_critical_section_vista()                                            = delete;


            void _Lock() override {
                EnterCriticalSection(&_M_critical_section);
            }

            bool _Try_lock() override {
                return TryEnterCriticalSection(&_M_critical_section) != 0;
            }

            bool _Try_lock_for(unsigned int) override {
                // STL will call try_lock_for once again if this call will not succeed
                return stl_critical_section_vista::_Try_lock();
            }

            void _Unlock() override {
                LeaveCriticalSection(&_M_critical_section);
            }

            LPCRITICAL_SECTION native_handle() {
                return &_M_critical_section;
            }

            void _Destroy() override {
                DeleteCriticalSection(&_M_critical_section);
            }

        private:
            CRITICAL_SECTION _M_critical_section;
        };

        class stl_condition_variable_vista final : public stl_condition_variable_interface {
        public:
            stl_condition_variable_vista() {
                InitializeConditionVariable(&m_condition_variable);
            }

            ~stl_condition_variable_vista()                                   = delete;
            stl_condition_variable_vista(const stl_condition_variable_vista&) = delete;
            stl_condition_variable_vista& operator=(const stl_condition_variable_vista&) = delete;

            void destroy() override {}

            void wait(stl_critical_section_interface* lock) override {
                if (!stl_condition_variable_vista::wait_for(lock, INFINITE)) {
                    std::terminate();
                }
            }

            bool wait_for(stl_critical_section_interface* lock, unsigned int timeout) override {
                return SleepConditionVariableCS(&m_condition_variable,
                           static_cast<stl_critical_section_vista*>(lock)->native_handle(), timeout)
                    != 0;
            }

            void notify_one() override {
                WakeConditionVariable(&m_condition_variable);
            }

            void notify_all() override {
                WakeAllConditionVariable(&m_condition_variable);
            }

        private:
            CONDITION_VARIABLE m_condition_variable;
        };

        class stl_critical_section_win7 final : public stl_critical_section_interface {
        public:
            stl_critical_section_win7() {
                InitializeSRWLock(&m_srw_lock);
            }

            ~stl_critical_section_win7()                                = delete;
            stl_critical_section_win7(const stl_critical_section_win7&) = delete;
            stl_critical_section_win7& operator=(const stl_critical_section_win7&) = delete;

            void _Destroy() override {}

            void _Lock() override {
                AcquireSRWLockExclusive(&m_srw_lock);
            }

            bool _Try_lock() override {
                return TryAcquireSRWLockExclusive(&m_srw_lock) != 0;
            }

            bool _Try_lock_for(unsigned int) override {
                // STL will call try_lock_for once again if this call will not succeed
                return stl_critical_section_win7::_Try_lock();
            }

            void _Unlock() override {
                ReleaseSRWLockExclusive(&m_srw_lock);
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
                InitializeConditionVariable(&m_condition_variable);
            }

            ~stl_condition_variable_win7()                                  = delete;
            stl_condition_variable_win7(const stl_condition_variable_win7&) = delete;
            stl_condition_variable_win7& operator=(const stl_condition_variable_win7&) = delete;

            void destroy() override {}

            void wait(stl_critical_section_interface* lock) override {
                if (!stl_condition_variable_win7::wait_for(lock, INFINITE)) {
                    std::terminate();
                }
            }

            bool wait_for(stl_critical_section_interface* lock, unsigned int timeout) override {
                return SleepConditionVariableSRW(&m_condition_variable,
                           static_cast<stl_critical_section_win7*>(lock)->native_handle(), timeout, 0)
                    != 0;
            }

            void notify_one() override {
                WakeConditionVariable(&m_condition_variable);
            }

            void notify_all() override {
                WakeAllConditionVariable(&m_condition_variable);
            }

        private:
            CONDITION_VARIABLE m_condition_variable;
        };

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

        inline void create_stl_critical_section(stl_critical_section_interface* p) {
            new (p) stl_critical_section_win7;
        }

        inline void create_stl_condition_variable(stl_condition_variable_interface* p) {
            new (p) stl_condition_variable_win7;
        }

#if defined _CRT_WINDOWS
        const size_t stl_critical_section_max_size        = sizeof(stl_critical_section_win7);
        const size_t stl_condition_variable_max_size      = sizeof(stl_condition_variable_win7);
        const size_t stl_critical_section_max_alignment   = alignof(stl_critical_section_win7);
        const size_t stl_condition_variable_max_alignment = alignof(stl_condition_variable_win7);
#elif defined _STL_CONCRT_SUPPORT

#ifdef _WIN64
        const size_t sizeof_stl_critical_section_concrt = 64;
        const size_t sizeof_stl_condition_variable_concrt = 72;
        const size_t alignof_stl_critical_section_concrt = 8;
        const size_t alignof_stl_condition_variable_concrt = 8;
#else // ^^^ 64-bit / 32-bit vvv
        const size_t sizeof_stl_critical_section_concrt    = 36;
        const size_t sizeof_stl_condition_variable_concrt  = 40;
        const size_t alignof_stl_critical_section_concrt   = 4;
        const size_t alignof_stl_condition_variable_concrt = 4;
#endif // ^^^ 32-bit ^^^

        const size_t stl_critical_section_max_size =
            __max(__max(sizeof_stl_critical_section_concrt, sizeof(stl_critical_section_vista)),
                sizeof(stl_critical_section_win7));
        const size_t stl_condition_variable_max_size =
            __max(__max(sizeof_stl_condition_variable_concrt, sizeof(stl_condition_variable_vista)),
                sizeof(stl_condition_variable_win7));
        const size_t stl_critical_section_max_alignment =
            __max(__max(alignof_stl_critical_section_concrt, alignof(stl_critical_section_vista)),
                alignof(stl_critical_section_win7));
        const size_t stl_condition_variable_max_alignment =
            __max(__max(alignof_stl_condition_variable_concrt, alignof(stl_condition_variable_vista)),
                alignof(stl_condition_variable_win7));
#else
        const size_t stl_critical_section_max_size =
            __max(sizeof(stl_critical_section_vista), sizeof(stl_critical_section_win7));
        const size_t stl_condition_variable_max_size =
            __max(sizeof(stl_condition_variable_vista), sizeof(stl_condition_variable_win7));
        const size_t stl_critical_section_max_alignment =
            __max(alignof(stl_critical_section_vista), alignof(stl_critical_section_win7));
        const size_t stl_condition_variable_max_alignment =
            __max(alignof(stl_condition_variable_vista), alignof(stl_condition_variable_win7));
#endif
    } // namespace details
} // namespace Concurrency
