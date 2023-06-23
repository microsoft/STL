// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <exception>
#include <new>

#include "awint.hpp"

namespace Concurrency {
    namespace details {
        // TRANSITION, only used when constexpr mutex constructor is not enabled
        class stl_critical_section_win7 final {
        public:
            stl_critical_section_win7() = default;

            ~stl_critical_section_win7()                                           = delete;
            stl_critical_section_win7(const stl_critical_section_win7&)            = delete;
            stl_critical_section_win7& operator=(const stl_critical_section_win7&) = delete;

            virtual void lock() {
                AcquireSRWLockExclusive(&m_srw_lock);
            }

            virtual bool try_lock() {
                return TryAcquireSRWLockExclusive(&m_srw_lock) != 0;
            }

            virtual bool try_lock_for(unsigned int) {
                return try_lock();
            }

            virtual void unlock() {
                _Analysis_assume_lock_held_(m_srw_lock);
                ReleaseSRWLockExclusive(&m_srw_lock);
            }

            virtual void destroy() {}

        private:
            SRWLOCK m_srw_lock = SRWLOCK_INIT;
        };

        class stl_condition_variable_win7 final {
        public:
            stl_condition_variable_win7() = default;

            ~stl_condition_variable_win7()                                             = delete;
            stl_condition_variable_win7(const stl_condition_variable_win7&)            = delete;
            stl_condition_variable_win7& operator=(const stl_condition_variable_win7&) = delete;

            virtual void wait(_Stl_critical_section* lock) {
                if (!wait_for(lock, INFINITE)) {
                    std::terminate();
                }
            }

            virtual bool wait_for(_Stl_critical_section* lock, unsigned int timeout) {
                return SleepConditionVariableSRW(
                           &m_condition_variable, reinterpret_cast<PSRWLOCK>(lock->_M_srw_lock), timeout, 0)
                    != 0;
            }

            virtual void notify_one() {
                WakeConditionVariable(&m_condition_variable);
            }

            virtual void notify_all() {
                WakeAllConditionVariable(&m_condition_variable);
            }

            virtual void destroy() {}

        private:
            CONDITION_VARIABLE m_condition_variable = CONDITION_VARIABLE_INIT;
        };

        // TRANSITION, only used when constexpr mutex constructor is not enabled
        inline void create_stl_critical_section(void* p) {
            new (p) stl_critical_section_win7;
        }

        inline void create_stl_condition_variable(void* p) {
            new (p) stl_condition_variable_win7;
        }

#if defined(_CRT_WINDOWS) // for Windows-internal code
        const size_t stl_condition_variable_max_size = 2 * sizeof(void*);
#elif defined(_WIN64) // ordinary 64-bit code
        const size_t stl_condition_variable_max_size = 72;
#else // vvv ordinary 32-bit code vvv
        const size_t stl_condition_variable_max_size = 40;
#endif // ^^^ ordinary 32-bit code ^^^

        const size_t stl_condition_variable_max_alignment = alignof(void*);
    } // namespace details
} // namespace Concurrency
