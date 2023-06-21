// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

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

        class stl_critical_section_win7 final : public stl_critical_section_interface {
        public:
            stl_critical_section_win7() = default;

            ~stl_critical_section_win7()                                           = delete;
            stl_critical_section_win7(const stl_critical_section_win7&)            = delete;
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
                _Analysis_assume_lock_held_(m_srw_lock);
                ReleaseSRWLockExclusive(&m_srw_lock);
            }

            PSRWLOCK native_handle() {
                return &m_srw_lock;
            }

        private:
            SRWLOCK m_srw_lock = SRWLOCK_INIT;
        };

        class stl_condition_variable_win7 final : public stl_condition_variable_interface {
        public:
            stl_condition_variable_win7() {
                InitializeConditionVariable(&m_condition_variable);
            }

            ~stl_condition_variable_win7()                                             = delete;
            stl_condition_variable_win7(const stl_condition_variable_win7&)            = delete;
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
