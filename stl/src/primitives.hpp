// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <exception>
#include <new>

#include "awint.hpp"

namespace Concurrency::details {
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
        stl_critical_section_win7() {
            InitializeSRWLock(&m_srw_lock);
        }

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

#ifdef _WIN64
    constexpr size_t sizeof_stl_critical_section_concrt   = 64;
    constexpr size_t sizeof_stl_condition_variable_concrt = 72;
    constexpr size_t sizeof_stl_critical_section_vista    = 48;
    constexpr size_t sizeof_stl_condition_variable_vista  = 16;
#else // ^^^ 64-bit / 32-bit vvv
    constexpr size_t sizeof_stl_critical_section_concrt   = 36;
    constexpr size_t sizeof_stl_condition_variable_concrt = 40;
    constexpr size_t sizeof_stl_critical_section_vista    = 28;
    constexpr size_t sizeof_stl_condition_variable_vista  = 8;
#endif // ^^^ 32-bit ^^^

#if defined(_CRT_WINDOWS)
    constexpr size_t stl_critical_section_max_size   = sizeof(stl_critical_section_win7);
    constexpr size_t stl_condition_variable_max_size = sizeof(stl_condition_variable_win7);
#elif defined(_STL_CONCRT_SUPPORT)
    constexpr size_t stl_critical_section_max_size        = sizeof_stl_critical_section_concrt;
    constexpr size_t stl_condition_variable_max_size      = sizeof_stl_condition_variable_concrt;
#else // vvv !defined(_CRT_WINDOWS) && !defined(_STL_CONCRT_SUPPORT) vvv
    constexpr size_t stl_critical_section_max_size   = sizeof_stl_critical_section_vista;
    constexpr size_t stl_condition_variable_max_size = sizeof_stl_condition_variable_vista;
#endif // ^^^ !defined(_CRT_WINDOWS) && !defined(_STL_CONCRT_SUPPORT) ^^^

    // concrt, vista, and win7 alignments are all identical to alignof(void*)
    constexpr size_t stl_critical_section_max_alignment   = alignof(stl_critical_section_win7);
    constexpr size_t stl_condition_variable_max_alignment = alignof(stl_condition_variable_win7);
} // namespace Concurrency::details
