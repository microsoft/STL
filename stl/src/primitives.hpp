// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <cstdlib> // for __max
#include <exception>
#include <new>

#include "awint.hpp"

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

        class stl_critical_section_vista final : public stl_critical_section_interface {
        public:
            stl_critical_section_vista() {
                InitializeCriticalSectionEx(&_M_critical_section, 4000, 0);
            }

            stl_critical_section_vista(const stl_critical_section_vista&) = delete;
            stl_critical_section_vista& operator=(const stl_critical_section_vista&) = delete;
            ~stl_critical_section_vista()                                            = delete;

            void destroy() override {
                DeleteCriticalSection(&_M_critical_section);
            }

            void lock() override {
                EnterCriticalSection(&_M_critical_section);
            }

            bool try_lock() override {
                return TryEnterCriticalSection(&_M_critical_section) != 0;
            }

            bool try_lock_for(unsigned int) override {
                // STL will call try_lock_for once again if this call will not succeed
                return stl_critical_section_vista::try_lock();
            }

            void unlock() override {
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

            void destroy() override {}

            void lock() override {
                AcquireSRWLockExclusive(&m_srw_lock);
            }

            bool try_lock() override {
                return TryAcquireSRWLockExclusive(&m_srw_lock) != 0;
            }

            bool try_lock_for(unsigned int) override {
                // STL will call try_lock_for once again if this call will not succeed
                return stl_critical_section_win7::try_lock();
            }

            void unlock() override {
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

        inline void create_stl_critical_section(stl_critical_section_interface* p) {
            new (p) stl_critical_section_win7;
        }

        inline void create_stl_condition_variable(stl_condition_variable_interface* p) {
            new (p) stl_condition_variable_win7;
        }

// TEMPORARY VERIFICATION OF EXACT VALUES:
#ifdef _WIN64
        static_assert(sizeof(stl_critical_section_vista) == 48, "Verifying exact values.");
        static_assert(sizeof(stl_condition_variable_vista) == 16, "Verifying exact values.");
        static_assert(alignof(stl_critical_section_vista) == 8, "Verifying exact values.");
        static_assert(alignof(stl_condition_variable_vista) == 8, "Verifying exact values.");

        static_assert(sizeof(stl_critical_section_win7) == 16, "Verifying exact values.");
        static_assert(sizeof(stl_condition_variable_win7) == 16, "Verifying exact values.");
        static_assert(alignof(stl_critical_section_win7) == 8, "Verifying exact values.");
        static_assert(alignof(stl_condition_variable_win7) == 8, "Verifying exact values.");
#else // ^^^ 64-bit / 32-bit vvv
        static_assert(sizeof(stl_critical_section_vista) == 28, "Verifying exact values.");
        static_assert(sizeof(stl_condition_variable_vista) == 8, "Verifying exact values.");
        static_assert(alignof(stl_critical_section_vista) == 4, "Verifying exact values.");
        static_assert(alignof(stl_condition_variable_vista) == 4, "Verifying exact values.");

        static_assert(sizeof(stl_critical_section_win7) == 8, "Verifying exact values.");
        static_assert(sizeof(stl_condition_variable_win7) == 8, "Verifying exact values.");
        static_assert(alignof(stl_critical_section_win7) == 4, "Verifying exact values.");
        static_assert(alignof(stl_condition_variable_win7) == 4, "Verifying exact values.");
#endif // ^^^ 32-bit ^^^

        // TEMPORARY VERIFICATION OF SIZE FOR !defined(_CRT_WINDOWS) && !defined(_STL_CONCRT_SUPPORT) BELOW:
        static_assert(sizeof(stl_critical_section_vista) >= sizeof(stl_critical_section_win7),
            "Verifying sizeof(vista) >= sizeof(win7).");
        static_assert(sizeof(stl_condition_variable_vista) >= sizeof(stl_condition_variable_win7),
            "Verifying sizeof(vista) >= sizeof(win7).");

#if defined _CRT_WINDOWS
        const size_t stl_critical_section_max_size        = sizeof(stl_critical_section_win7);
        const size_t stl_condition_variable_max_size      = sizeof(stl_condition_variable_win7);
        const size_t stl_critical_section_max_alignment   = alignof(stl_critical_section_win7);
        const size_t stl_condition_variable_max_alignment = alignof(stl_condition_variable_win7);
#elif defined _STL_CONCRT_SUPPORT

#ifdef _WIN64
        const size_t sizeof_stl_critical_section_concrt    = 64;
        const size_t sizeof_stl_condition_variable_concrt  = 72;
        const size_t alignof_stl_critical_section_concrt   = 8;
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

        // TEMPORARY VERIFICATION OF SIZE:
        static_assert(stl_critical_section_max_size == sizeof_stl_critical_section_concrt,
            "Verifying that concrt is the maximum size.");
        static_assert(stl_condition_variable_max_size == sizeof_stl_condition_variable_concrt,
            "Verifying that concrt is the maximum size.");

        // TEMPORARY VERIFICATION OF ALIGNMENT:
        static_assert(alignof_stl_critical_section_concrt == alignof(void*), "Verifying pointer alignment.");
        static_assert(alignof_stl_condition_variable_concrt == alignof(void*), "Verifying pointer alignment.");
        static_assert(alignof(stl_critical_section_vista) == alignof(void*), "Verifying pointer alignment.");
        static_assert(alignof(stl_condition_variable_vista) == alignof(void*), "Verifying pointer alignment.");
        static_assert(alignof(stl_critical_section_win7) == alignof(void*), "Verifying pointer alignment.");
        static_assert(alignof(stl_condition_variable_win7) == alignof(void*), "Verifying pointer alignment.");

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
