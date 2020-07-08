// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test atomic operations on shared_ptr objects
#define TEST_NAME "atomic shared_ptr operations"

#define _SILENCE_CXX20_OLD_SHARED_PTR_ATOMIC_SUPPORT_DEPRECATION_WARNING

#include "tdefs.h"
#include <assert.h>
#include <atomic>
#include <memory>
#include <thread>

static const unsigned long iterations = 100000;

typedef STD shared_ptr<int> sp;
const sp s0(new int(0));
const sp s1(new int(1));

static void test_load_store() { // test atomic load and store operations
    sp sp0(s0);
    sp sp1(s1);
    static sp shared(sp0);
    for (unsigned long i = 0; i < iterations; ++i) { // repeat several times
        STD atomic_store(&shared, sp0);
        sp temp = STD atomic_load_explicit(&shared, STD memory_order_seq_cst);
        assert(temp == sp0 || temp == sp1);
        STD this_thread::yield();
        STD atomic_store_explicit(&shared, sp1, STD memory_order_seq_cst);
        temp = STD atomic_load(&shared);
        assert(temp == sp0 || temp == sp1);
        STD this_thread::yield();
    }
}

static void test_exchange() { // test atomic exchange operation
    sp sp0(s0);
    sp sp1(s1);
    static sp shared(sp0);
    for (unsigned long i = 0; i < iterations; ++i) { // repeat several times
        sp temp = STD atomic_exchange(&shared, sp0);
        assert(temp == sp0 || temp == sp1);
        STD this_thread::yield();
        temp = STD atomic_exchange_explicit(&shared, sp1, STD memory_order_seq_cst);
        assert(temp == sp0 || temp == sp1);
        STD this_thread::yield();
    }
}

static void test_compare_exchange_weak() { // test atomic compare-exchange weak operation
    sp sp0(s0);
    sp sp1(s1);
    static sp shared(sp0);
    for (unsigned long i = 0; i < iterations; ++i) { // repeat several times
        sp sp_local(sp0);
        if (STD atomic_compare_exchange_weak(&shared, &sp_local, sp1))
            assert(sp_local == sp0);
        else
            assert(sp_local == sp1);
        STD this_thread::yield();
        sp_local = sp1;
        if (STD atomic_compare_exchange_weak_explicit(
                &shared, &sp_local, sp0, STD memory_order_seq_cst, STD memory_order_seq_cst))
            assert(sp_local == sp1);
        else
            assert(sp_local == sp0);
        STD this_thread::yield();
    }
}

static void test_compare_exchange_strong() { // test atomic compare-exchange strong operation
    sp sp0(s0);
    sp sp1(s1);
    static sp shared(sp0);
    for (unsigned long i = 0; i < iterations; ++i) { // repeat several times
        sp sp_local(sp0);
        if (STD atomic_compare_exchange_strong(&shared, &sp_local, sp1))
            assert(sp_local == sp0);
        else
            assert(sp_local == sp1);
        STD this_thread::yield();
        sp_local = sp1;
        if (STD atomic_compare_exchange_strong_explicit(
                &shared, &sp_local, sp0, STD memory_order_seq_cst, STD memory_order_seq_cst))
            assert(sp_local == sp1);
        else
            assert(sp_local == sp0);
        STD this_thread::yield();
    }
}

static void run_test(void (*fp)()) { // run *fp in four threads and wait for completion
    STD thread thr0(fp);
    STD thread thr1(fp);
    STD thread thr2(fp);
    STD thread thr3(fp);
    thr0.join();
    thr1.join();
    thr2.join();
    thr3.join();
}

// RUN TESTS
void test_main() { // test header <atomic>
    sp sp0(new int(0));
    bool is_lock_free = STD atomic_is_lock_free(&sp0);
    CHECK_INT(is_lock_free == false || is_lock_free == true, 1);
    run_test(test_load_store);
    run_test(test_exchange);
    run_test(test_compare_exchange_weak);
    run_test(test_compare_exchange_strong);
}
