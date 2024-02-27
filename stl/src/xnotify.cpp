// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstdlib>
#include <mutex>
#include <xthreads.h>

#include <Windows.h>

constexpr int _Nitems = 20;

namespace {
    struct _At_thread_exit_data { // data for condition-variable slot
        _Thrd_t id;
        _Mtx_t mtx;
        _Cnd_t cnd;
        int* res;
    };

    struct _At_thread_exit_block { // block of condition-variable slots
        _At_thread_exit_data data[_Nitems];
        int num_used;
        _At_thread_exit_block* next;
    };

    _At_thread_exit_block _Thread_exit_data;

    constinit std::mutex _Thread_exit_data_mutex;
} // unnamed namespace

extern "C" {

_CRTIMP2_PURE void __cdecl _Cnd_register_at_thread_exit(_Cnd_t cnd, _Mtx_t mtx, int* p) noexcept {
    // register condition variable and mutex for cleanup at thread exit

    // find block with available space
    _At_thread_exit_block* block = &_Thread_exit_data;

    std::lock_guard _Lock{_Thread_exit_data_mutex};
    while (block != nullptr) { // loop through list of blocks
        if (block->num_used == _Nitems) { // block is full; move to next block and allocate
            if (block->next == nullptr) {
                block->next = static_cast<_At_thread_exit_block*>(calloc(1, sizeof(_At_thread_exit_block)));
            }

            block = block->next;
        } else { // found block with available space
            for (int i = 0; i < _Nitems; ++i) { // find empty slot
                if (block->data[i].mtx == nullptr) { // store into empty slot
                    block->data[i].id._Id = GetCurrentThreadId();
                    block->data[i].mtx    = mtx;
                    block->data[i].cnd    = cnd;
                    block->data[i].res    = p;
                    ++block->num_used;
                    break;
                }
            }
            block = nullptr;
        }
    }
}

_CRTIMP2_PURE void __cdecl _Cnd_unregister_at_thread_exit(_Mtx_t mtx) noexcept {
    // unregister condition variable/mutex for cleanup at thread exit

    // find condition variables waiting for this thread to exit
    _At_thread_exit_block* block = &_Thread_exit_data;

    std::lock_guard _Lock{_Thread_exit_data_mutex};
    while (block != nullptr) { // loop through list of blocks
        for (int i = 0; block->num_used != 0 && i < _Nitems; ++i) {
            if (block->data[i].mtx == mtx) { // release slot
                block->data[i].mtx = nullptr;
                --block->num_used;
            }
        }

        block = block->next;
    }
}

_CRTIMP2_PURE void __cdecl _Cnd_do_broadcast_at_thread_exit() noexcept {
    // notify condition variables waiting for this thread to exit

    // find condition variables waiting for this thread to exit
    _At_thread_exit_block* block       = &_Thread_exit_data;
    const unsigned int currentThreadId = _Thrd_id();

    std::lock_guard _Lock{_Thread_exit_data_mutex};
    while (block != nullptr) { // loop through list of blocks
        for (int i = 0; block->num_used != 0 && i < _Nitems; ++i) {
            if (block->data[i].mtx != nullptr && block->data[i].id._Id == currentThreadId) { // notify and release slot
                if (block->data[i].res) {
                    *block->data[i].res = 1;
                }
                _Mtx_unlock(block->data[i].mtx);
                _Cnd_broadcast(block->data[i].cnd);
                block->data[i].mtx = nullptr;
                --block->num_used;
            }
        }

        block = block->next;
    }
}

} // extern "C"

/*
 * This file is derived from software bearing the following
 * restrictions:
 *
 * (c) Copyright William E. Kempf 2001
 *
 * Permission to use, copy, modify, distribute and sell this
 * software and its documentation for any purpose is hereby
 * granted without fee, provided that the above copyright
 * notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting
 * documentation. William E. Kempf makes no representations
 * about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 */
