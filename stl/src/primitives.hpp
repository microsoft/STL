// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <exception>
#include <new>

#include "awint.hpp"

namespace Concurrency {
    namespace details {
        class stl_critical_section_win7 {
        public:
            stl_critical_section_win7() = default;

            ~stl_critical_section_win7()                                           = delete;
            stl_critical_section_win7(const stl_critical_section_win7&)            = delete;
            stl_critical_section_win7& operator=(const stl_critical_section_win7&) = delete;

            void lock() {
                AcquireSRWLockExclusive(&m_srw_lock);
            }

            bool try_lock() {
                return TryAcquireSRWLockExclusive(&m_srw_lock) != 0;
            }

            void unlock() {
                _Analysis_assume_lock_held_(m_srw_lock);
                ReleaseSRWLockExclusive(&m_srw_lock);
            }

            PSRWLOCK native_handle() {
                return &m_srw_lock;
            }

        private:
            void* unused       = nullptr; // TRANSITION, ABI: was the vptr
            SRWLOCK m_srw_lock = SRWLOCK_INIT;
        };

        class stl_condition_variable_win7 {
        public:
            stl_condition_variable_win7() = default;

            ~stl_condition_variable_win7()                                             = delete;
            stl_condition_variable_win7(const stl_condition_variable_win7&)            = delete;
            stl_condition_variable_win7& operator=(const stl_condition_variable_win7&) = delete;

            void wait(stl_critical_section_win7* lock) {
                if (!wait_for(lock, INFINITE)) {
                    std::terminate();
                }
            }

            bool wait_for(stl_critical_section_win7* lock, unsigned int timeout) {
                return SleepConditionVariableSRW(&m_condition_variable, lock->native_handle(), timeout, 0) != 0;
            }

            void notify_one() {
                WakeConditionVariable(&m_condition_variable);
            }

            void notify_all() {
                WakeAllConditionVariable(&m_condition_variable);
            }

        private:
            void* unused                            = nullptr; // TRANSITION, ABI: was the vptr
            CONDITION_VARIABLE m_condition_variable = CONDITION_VARIABLE_INIT;
        };

        inline void create_stl_critical_section(stl_critical_section_win7* p) {
            new (p) stl_critical_section_win7;
        }

        inline void create_stl_condition_variable(stl_condition_variable_win7* p) {
            new (p) stl_condition_variable_win7;
        }

#if defined(_CRT_WINDOWS) // for Windows-internal code
        const size_t stl_critical_section_max_size   = 2 * sizeof(void*);
        const size_t stl_condition_variable_max_size = 2 * sizeof(void*);
#elif defined(_WIN64) // ordinary 64-bit code
        const size_t stl_critical_section_max_size   = 64;
        const size_t stl_condition_variable_max_size = 72;
#else // vvv ordinary 32-bit code vvv
        const size_t stl_critical_section_max_size   = 36;
        const size_t stl_condition_variable_max_size = 40;
#endif // ^^^ ordinary 32-bit code ^^^

        const size_t stl_critical_section_max_alignment   = alignof(void*);
        const size_t stl_condition_variable_max_alignment = alignof(void*);
    } // namespace details
} // namespace Concurrency
