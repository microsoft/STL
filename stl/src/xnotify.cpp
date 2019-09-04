// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// thread exit notification functions

#include <stdlib.h>
#include <string.h>
#include <xthreads.h>

#include <Windows.h>

#define NITEMS 20

namespace {
    struct _At_thread_exit_data { // data for condition-variable slot
        _Thrd_t id;
        _Mtx_t mtx;
        _Cnd_t cnd;
        int* res;
    };

    struct _At_thread_exit_block { // block of condition-variable slots
        _At_thread_exit_data data[NITEMS];
        int num_used;
        _At_thread_exit_block* next;
    };

    _At_thread_exit_block _Thread_exit_data;
} // unnamed namespace

_EXTERN_C

void _Lock_at_thread_exit_mutex();
void _Unlock_at_thread_exit_mutex();

void _Cnd_register_at_thread_exit(
    _Cnd_t cnd, _Mtx_t mtx, int* p) { // register condition variable and mutex for cleanup at thread exit
    // find block with available space
    _At_thread_exit_block* block = &_Thread_exit_data;

    _Lock_at_thread_exit_mutex();
    while (block != 0) { // loop through list of blocks
        if (block->num_used == NITEMS) { // block is full; move to next block and allocate
            if (block->next == 0) {
                block->next = (_At_thread_exit_block*) calloc(1, sizeof(_At_thread_exit_block));
            }

            block = block->next;
        } else { // found block with available space
            for (int i = 0; i < NITEMS; ++i) { // find empty slot
                if (block->data[i].mtx == 0) { // store into empty slot
                    block->data[i].id._Id = GetCurrentThreadId();
                    block->data[i].mtx    = mtx;
                    block->data[i].cnd    = cnd;
                    block->data[i].res    = p;
                    ++block->num_used;
                    break;
                }
            }
            block = 0;
        }
    }
    _Unlock_at_thread_exit_mutex();
}

void _Cnd_unregister_at_thread_exit(_Mtx_t mtx) { // unregister condition variable/mutex for cleanup at thread exit
    // find condition variables waiting for this thread to exit
    _At_thread_exit_block* block = &_Thread_exit_data;

    _Lock_at_thread_exit_mutex();
    while (block != 0) { // loop through list of blocks
        for (int i = 0; block->num_used != 0 && i < NITEMS; ++i) {
            if (block->data[i].mtx == mtx) { // release slot
                block->data[i].mtx = 0;
                --block->num_used;
            }
        }

        block = block->next;
    }
    _Unlock_at_thread_exit_mutex();
}

void _Cnd_do_broadcast_at_thread_exit() { // notify condition variables waiting for this thread to exit
    // find condition variables waiting for this thread to exit
    _At_thread_exit_block* block       = &_Thread_exit_data;
    const unsigned int currentThreadId = _Thrd_id();

    _Lock_at_thread_exit_mutex();
    while (block != 0) { // loop through list of blocks
        for (int i = 0; block->num_used != 0 && i < NITEMS; ++i) {
            if (block->data[i].mtx != 0 && block->data[i].id._Id == currentThreadId) { // notify and release slot
                if (block->data[i].res) {
                    *block->data[i].res = 1;
                }
                _Mtx_unlock(block->data[i].mtx);
                _Cnd_broadcast(block->data[i].cnd);
                block->data[i].mtx = 0;
                --block->num_used;
            }
        }

        block = block->next;
    }
    _Unlock_at_thread_exit_mutex();
}

_END_EXTERN_C

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
