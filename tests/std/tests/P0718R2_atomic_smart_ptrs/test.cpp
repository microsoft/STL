// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <cstdint>
#include <memory>
#include <thread>
#ifdef _DEBUG
#include <crtdbg.h>
#endif // _DEBUG

using namespace std;

constexpr uintmax_t iterations = 100000;

shared_ptr<int> sptr0 = make_shared<int>(0);
shared_ptr<int> sptr1 = make_shared<int>(1);
weak_ptr<int> wptr0   = sptr0;
weak_ptr<int> wptr1   = sptr1;

atomic<shared_ptr<int>> atomic_sptr;
atomic<weak_ptr<int>> atomic_wptr;

[[nodiscard]] bool weak_ptr_equal(const weak_ptr<int>& left, const weak_ptr<int>& right) {
    return !(left.owner_before(right) || right.owner_before(left));
}

void test_shared_ptr_load_store() {
    shared_ptr<int> sp0 = sptr0;
    shared_ptr<int> sp1 = sptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        atomic_sptr          = sp0;
        shared_ptr<int> temp = atomic_sptr;
        assert(temp == sp0 || temp == sp1);
        this_thread::yield();

        atomic_sptr.store(sp1);
        temp = atomic_sptr.load();
        assert(temp == sp0 || temp == sp1);
        this_thread::yield();

        atomic_sptr.store(sp0, memory_order::seq_cst);
        temp = atomic_sptr.load(memory_order::seq_cst);
        assert(temp == sp0 || temp == sp1);
        this_thread::yield();
    }
}

void test_shared_ptr_exchange() {
    shared_ptr<int> sp0 = sptr0;
    shared_ptr<int> sp1 = sptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        shared_ptr<int> temp = atomic_sptr.exchange(sp0);
        assert(temp == sp0 || temp == sp1);
        this_thread::yield();

        temp = atomic_sptr.exchange(sp1, memory_order::seq_cst);
        assert(temp == sp0 || temp == sp1);
        this_thread::yield();
    }
}

void test_shared_ptr_compare_exchange_weak() {
    shared_ptr<int> sp0 = sptr0;
    shared_ptr<int> sp1 = sptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        shared_ptr<int> local = sp0;
        if (atomic_sptr.compare_exchange_weak(local, sp1)) {
            assert(local == sp0);
        } else {
            assert(local == sp1);
        }
        this_thread::yield();

        local = sp1;
        if (atomic_sptr.compare_exchange_weak(local, sp0, memory_order::seq_cst, memory_order::seq_cst)) {
            assert(local == sp1);
        } else {
            assert(local == sp0);
        }
        this_thread::yield();
    }
}

void test_shared_ptr_compare_exchange_strong() {
    shared_ptr<int> sp0 = sptr0;
    shared_ptr<int> sp1 = sptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        shared_ptr<int> local = sp0;
        if (atomic_sptr.compare_exchange_strong(local, sp1)) {
            assert(local == sp0);
        } else {
            assert(local == sp1);
        }
        this_thread::yield();

        local = sp1;
        if (atomic_sptr.compare_exchange_strong(local, sp0, memory_order::seq_cst, memory_order::seq_cst)) {
            assert(local == sp1);
        } else {
            assert(local == sp0);
        }
        this_thread::yield();
    }
}

void test_weak_ptr_load_store() {
    weak_ptr<int> wp0 = wptr0;
    weak_ptr<int> wp1 = wptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        atomic_wptr        = wp0;
        weak_ptr<int> temp = atomic_wptr;
        assert(weak_ptr_equal(temp, wp0) || weak_ptr_equal(temp, wp1));
        this_thread::yield();

        atomic_wptr.store(wp1);
        temp = atomic_wptr.load();
        assert(weak_ptr_equal(temp, wp0) || weak_ptr_equal(temp, wp1));
        this_thread::yield();

        atomic_wptr.store(wp0, memory_order::seq_cst);
        temp = atomic_wptr.load(memory_order::seq_cst);
        assert(weak_ptr_equal(temp, wp0) || weak_ptr_equal(temp, wp1));
        this_thread::yield();
    }
}

void test_weak_ptr_exchange() {
    weak_ptr<int> wp0 = wptr0;
    weak_ptr<int> wp1 = wptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        weak_ptr<int> temp = atomic_wptr.exchange(wp0);
        assert(weak_ptr_equal(temp, wp0) || weak_ptr_equal(temp, wp1));
        this_thread::yield();

        temp = atomic_wptr.exchange(wp1, memory_order::seq_cst);
        assert(weak_ptr_equal(temp, wp0) || weak_ptr_equal(temp, wp1));
        this_thread::yield();
    }
}

void test_weak_ptr_compare_exchange_weak() {
    weak_ptr<int> wp0 = wptr0;
    weak_ptr<int> wp1 = wptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        weak_ptr<int> local = wp0;
        if (atomic_wptr.compare_exchange_weak(local, wp1)) {
            assert(weak_ptr_equal(local, wp0));
        } else {
            assert(weak_ptr_equal(local, wp1));
        }
        this_thread::yield();

        local = wp1;
        if (atomic_wptr.compare_exchange_weak(local, wp0, memory_order::seq_cst, memory_order::seq_cst)) {
            assert(weak_ptr_equal(local, wp1));
        } else {
            assert(weak_ptr_equal(local, wp0));
        }
        this_thread::yield();
    }
}

void test_weak_ptr_compare_exchange_strong() {
    weak_ptr<int> wp0 = wptr0;
    weak_ptr<int> wp1 = wptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        weak_ptr<int> local = wp0;
        if (atomic_wptr.compare_exchange_strong(local, wp1)) {
            assert(weak_ptr_equal(local, wp0));
        } else {
            assert(weak_ptr_equal(local, wp1));
        }
        this_thread::yield();

        local = wp1;
        if (atomic_wptr.compare_exchange_strong(local, wp0, memory_order::seq_cst, memory_order::seq_cst)) {
            assert(weak_ptr_equal(local, wp1));
        } else {
            assert(weak_ptr_equal(local, wp0));
        }
        this_thread::yield();
    }
}

void run_test(void (*fp)()) {
    thread thr0(fp);
    thread thr1(fp);
    thread thr2(fp);
    thread thr3(fp);
    thr0.join();
    thr1.join();
    thr2.join();
    thr3.join();
}

template <class AtomicType>
void ensure_nonmember_calls_compile() {
    AtomicType instance;
    const AtomicType constInstance;
    auto loaded = atomic_load(&instance);
    loaded      = atomic_load(&constInstance);
    loaded      = atomic_load_explicit(&instance, memory_order::relaxed);
    loaded      = atomic_load_explicit(&constInstance, memory_order::relaxed);
    atomic_store(&instance, loaded);
    atomic_store_explicit(&instance, loaded, memory_order::relaxed);
    loaded = atomic_exchange(&instance, loaded);
    loaded = atomic_exchange_explicit(&instance, loaded, memory_order::relaxed);
    if (atomic_compare_exchange_weak(&instance, &loaded, loaded)) {
        // intentionally empty
    }
    if (atomic_compare_exchange_weak_explicit(
            &instance, &loaded, loaded, memory_order::relaxed, memory_order::relaxed)) {
        // intentionally empty
    }
    if (atomic_compare_exchange_strong(&instance, &loaded, loaded)) {
        // intentionally empty
    }
    if (atomic_compare_exchange_strong_explicit(
            &instance, &loaded, loaded, memory_order::relaxed, memory_order::relaxed)) {
        // intentionally empty
    }
}

int main() {
    // These values for is_always_lock_free are not required by the standard, but they are true for our implementation.
    static_assert(atomic<shared_ptr<int>>::is_always_lock_free == false);
    static_assert(atomic<weak_ptr<int>>::is_always_lock_free == false);
    assert(atomic_sptr.is_lock_free() == false);
    assert(atomic_wptr.is_lock_free() == false);

    run_test(test_shared_ptr_load_store);
    run_test(test_shared_ptr_exchange);
    run_test(test_shared_ptr_compare_exchange_weak);
    run_test(test_shared_ptr_compare_exchange_strong);
    run_test(test_weak_ptr_load_store);
    run_test(test_weak_ptr_exchange);
    run_test(test_weak_ptr_compare_exchange_weak);
    run_test(test_weak_ptr_compare_exchange_strong);
    ensure_nonmember_calls_compile<atomic<shared_ptr<int>>>();
    ensure_nonmember_calls_compile<atomic<weak_ptr<int>>>();

#ifdef _DEBUG
    sptr0 = {};
    sptr1 = {};
    wptr0 = {};
    wptr1 = {};
    atomic_sptr.store({});
    atomic_wptr.store({});
    assert(!_CrtDumpMemoryLeaks());
#endif // _DEBUG
}
