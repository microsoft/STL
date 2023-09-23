// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <exception>
#include <new>

#include <Windows.h>

namespace Concurrency {
    namespace details {
        class stl_condition_variable_win7 {
        public:
            stl_condition_variable_win7() = default;

            ~stl_condition_variable_win7()                                             = delete;
            stl_condition_variable_win7(const stl_condition_variable_win7&)            = delete;
            stl_condition_variable_win7& operator=(const stl_condition_variable_win7&) = delete;

            void wait(_Stl_critical_section* lock) {
                if (!wait_for(lock, INFINITE)) {
                    _CSTD abort();
                }
            }

            bool wait_for(_Stl_critical_section* lock, unsigned int timeout) {
                return SleepConditionVariableSRW(
                           &m_condition_variable, reinterpret_cast<PSRWLOCK>(&lock->_M_srw_lock), timeout, 0)
                    != 0;
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

        // TRANSITION, only used when constexpr mutex constructor is not enabled
        inline void create_stl_critical_section(_Stl_critical_section* p) {
            new (p) _Stl_critical_section;
        }

        inline void create_stl_condition_variable(stl_condition_variable_win7* p) {
            new (p) stl_condition_variable_win7;
        }
    } // namespace details
} // namespace Concurrency
