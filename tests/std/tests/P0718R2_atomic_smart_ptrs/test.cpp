// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
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

shared_ptr<int[]> sarrptr0 = make_shared<int[]>(0);
shared_ptr<int[]> sarrptr1 = make_shared<int[]>(1);
weak_ptr<int[]> warrptr0   = sarrptr0;
weak_ptr<int[]> warrptr1   = sarrptr1;

atomic<shared_ptr<int[]>> atomic_sarrptr;
atomic<weak_ptr<int[]>> atomic_warrptr;

shared_ptr<int[2]> sarrnptr0 = make_shared<int[2]>();
shared_ptr<int[2]> sarrnptr1 = make_shared<int[2]>();
weak_ptr<int[2]> warrnptr0   = sarrnptr0;
weak_ptr<int[2]> warrnptr1   = sarrnptr1;

atomic<shared_ptr<int[2]>> atomic_sarrnptr;
atomic<weak_ptr<int[2]>> atomic_warrnptr;

template <typename T>
[[nodiscard]] bool weak_ptr_equal(const weak_ptr<T>& left, const weak_ptr<T>& right) {
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

// Repeat test for unbounded array type.
void test_shared_ptr_arr_load_store() {
    shared_ptr<int[]> sp0 = sarrptr0;
    shared_ptr<int[]> sp1 = sarrptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        atomic_sarrptr         = sp0;
        shared_ptr<int[]> temp = atomic_sarrptr;
        assert(temp == sp0 || temp == sp1);
        this_thread::yield();

        atomic_sarrptr.store(sp1);
        temp = atomic_sarrptr.load();
        assert(temp == sp0 || temp == sp1);
        this_thread::yield();

        atomic_sarrptr.store(sp0, memory_order::seq_cst);
        temp = atomic_sarrptr.load(memory_order::seq_cst);
        assert(temp == sp0 || temp == sp1);
        this_thread::yield();
    }
}

void test_shared_ptr_arr_exchange() {
    shared_ptr<int[]> sp0 = sarrptr0;
    shared_ptr<int[]> sp1 = sarrptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        shared_ptr<int[]> temp = atomic_sarrptr.exchange(sp0);
        assert(temp == sp0 || temp == sp1);
        this_thread::yield();

        temp = atomic_sarrptr.exchange(sp1, memory_order::seq_cst);
        assert(temp == sp0 || temp == sp1);
        this_thread::yield();
    }
}

void test_shared_ptr_arr_compare_exchange_weak() {
    shared_ptr<int[]> sp0 = sarrptr0;
    shared_ptr<int[]> sp1 = sarrptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        shared_ptr<int[]> local = sp0;
        if (atomic_sarrptr.compare_exchange_weak(local, sp1)) {
            assert(local == sp0);
        } else {
            assert(local == sp1);
        }
        this_thread::yield();

        local = sp1;
        if (atomic_sarrptr.compare_exchange_weak(local, sp0, memory_order::seq_cst, memory_order::seq_cst)) {
            assert(local == sp1);
        } else {
            assert(local == sp0);
        }
        this_thread::yield();
    }
}

void test_shared_ptr_arr_compare_exchange_strong() {
    shared_ptr<int[]> sp0 = sarrptr0;
    shared_ptr<int[]> sp1 = sarrptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        shared_ptr<int[]> local = sp0;
        if (atomic_sarrptr.compare_exchange_strong(local, sp1)) {
            assert(local == sp0);
        } else {
            assert(local == sp1);
        }
        this_thread::yield();

        local = sp1;
        if (atomic_sarrptr.compare_exchange_strong(local, sp0, memory_order::seq_cst, memory_order::seq_cst)) {
            assert(local == sp1);
        } else {
            assert(local == sp0);
        }
        this_thread::yield();
    }
}

void test_weak_ptr_arr_load_store() {
    weak_ptr<int[]> wp0 = warrptr0;
    weak_ptr<int[]> wp1 = warrptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        atomic_warrptr       = wp0;
        weak_ptr<int[]> temp = atomic_warrptr;
        assert(weak_ptr_equal(temp, wp0) || weak_ptr_equal(temp, wp1));
        this_thread::yield();

        atomic_warrptr.store(wp1);
        temp = atomic_warrptr.load();
        assert(weak_ptr_equal(temp, wp0) || weak_ptr_equal(temp, wp1));
        this_thread::yield();

        atomic_warrptr.store(wp0, memory_order::seq_cst);
        temp = atomic_warrptr.load(memory_order::seq_cst);
        assert(weak_ptr_equal(temp, wp0) || weak_ptr_equal(temp, wp1));
        this_thread::yield();
    }
}

void test_weak_ptr_arr_exchange() {
    weak_ptr<int[]> wp0 = warrptr0;
    weak_ptr<int[]> wp1 = warrptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        weak_ptr<int[]> temp = atomic_warrptr.exchange(wp0);
        assert(weak_ptr_equal(temp, wp0) || weak_ptr_equal(temp, wp1));
        this_thread::yield();

        temp = atomic_warrptr.exchange(wp1, memory_order::seq_cst);
        assert(weak_ptr_equal(temp, wp0) || weak_ptr_equal(temp, wp1));
        this_thread::yield();
    }
}

void test_weak_ptr_arr_compare_exchange_weak() {
    weak_ptr<int[]> wp0 = warrptr0;
    weak_ptr<int[]> wp1 = warrptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        weak_ptr<int[]> local = wp0;
        if (atomic_warrptr.compare_exchange_weak(local, wp1)) {
            assert(weak_ptr_equal(local, wp0));
        } else {
            assert(weak_ptr_equal(local, wp1));
        }
        this_thread::yield();

        local = wp1;
        if (atomic_warrptr.compare_exchange_weak(local, wp0, memory_order::seq_cst, memory_order::seq_cst)) {
            assert(weak_ptr_equal(local, wp1));
        } else {
            assert(weak_ptr_equal(local, wp0));
        }
        this_thread::yield();
    }
}

void test_weak_ptr_arr_compare_exchange_strong() {
    weak_ptr<int[]> wp0 = warrptr0;
    weak_ptr<int[]> wp1 = warrptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        weak_ptr<int[]> local = wp0;
        if (atomic_warrptr.compare_exchange_strong(local, wp1)) {
            assert(weak_ptr_equal(local, wp0));
        } else {
            assert(weak_ptr_equal(local, wp1));
        }
        this_thread::yield();

        local = wp1;
        if (atomic_warrptr.compare_exchange_strong(local, wp0, memory_order::seq_cst, memory_order::seq_cst)) {
            assert(weak_ptr_equal(local, wp1));
        } else {
            assert(weak_ptr_equal(local, wp0));
        }
        this_thread::yield();
    }
}

// Repeat test for bounded array type.
void test_shared_ptr_arrn_load_store() {
    shared_ptr<int[2]> sp0 = sarrnptr0;
    shared_ptr<int[2]> sp1 = sarrnptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        atomic_sarrnptr         = sp0;
        shared_ptr<int[2]> temp = atomic_sarrnptr;
        assert(temp == sp0 || temp == sp1);
        this_thread::yield();

        atomic_sarrnptr.store(sp1);
        temp = atomic_sarrnptr.load();
        assert(temp == sp0 || temp == sp1);
        this_thread::yield();

        atomic_sarrnptr.store(sp0, memory_order::seq_cst);
        temp = atomic_sarrnptr.load(memory_order::seq_cst);
        assert(temp == sp0 || temp == sp1);
        this_thread::yield();
    }
}

void test_shared_ptr_arrn_exchange() {
    shared_ptr<int[2]> sp0 = sarrnptr0;
    shared_ptr<int[2]> sp1 = sarrnptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        shared_ptr<int[2]> temp = atomic_sarrnptr.exchange(sp0);
        assert(temp == sp0 || temp == sp1);
        this_thread::yield();

        temp = atomic_sarrnptr.exchange(sp1, memory_order::seq_cst);
        assert(temp == sp0 || temp == sp1);
        this_thread::yield();
    }
}

void test_shared_ptr_arrn_compare_exchange_weak() {
    shared_ptr<int[2]> sp0 = sarrnptr0;
    shared_ptr<int[2]> sp1 = sarrnptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        shared_ptr<int[2]> local = sp0;
        if (atomic_sarrnptr.compare_exchange_weak(local, sp1)) {
            assert(local == sp0);
        } else {
            assert(local == sp1);
        }
        this_thread::yield();

        local = sp1;
        if (atomic_sarrnptr.compare_exchange_weak(local, sp0, memory_order::seq_cst, memory_order::seq_cst)) {
            assert(local == sp1);
        } else {
            assert(local == sp0);
        }
        this_thread::yield();
    }
}

void test_shared_ptr_arrn_compare_exchange_strong() {
    shared_ptr<int[2]> sp0 = sarrnptr0;
    shared_ptr<int[2]> sp1 = sarrnptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        shared_ptr<int[2]> local = sp0;
        if (atomic_sarrnptr.compare_exchange_strong(local, sp1)) {
            assert(local == sp0);
        } else {
            assert(local == sp1);
        }
        this_thread::yield();

        local = sp1;
        if (atomic_sarrnptr.compare_exchange_strong(local, sp0, memory_order::seq_cst, memory_order::seq_cst)) {
            assert(local == sp1);
        } else {
            assert(local == sp0);
        }
        this_thread::yield();
    }
}

void test_weak_ptr_arrn_load_store() {
    weak_ptr<int[2]> wp0 = warrnptr0;
    weak_ptr<int[2]> wp1 = warrnptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        atomic_warrnptr       = wp0;
        weak_ptr<int[2]> temp = atomic_warrnptr;
        assert(weak_ptr_equal(temp, wp0) || weak_ptr_equal(temp, wp1));
        this_thread::yield();

        atomic_warrnptr.store(wp1);
        temp = atomic_warrnptr.load();
        assert(weak_ptr_equal(temp, wp0) || weak_ptr_equal(temp, wp1));
        this_thread::yield();

        atomic_warrnptr.store(wp0, memory_order::seq_cst);
        temp = atomic_warrnptr.load(memory_order::seq_cst);
        assert(weak_ptr_equal(temp, wp0) || weak_ptr_equal(temp, wp1));
        this_thread::yield();
    }
}

void test_weak_ptr_arrn_exchange() {
    weak_ptr<int[2]> wp0 = warrnptr0;
    weak_ptr<int[2]> wp1 = warrnptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        weak_ptr<int[2]> temp = atomic_warrnptr.exchange(wp0);
        assert(weak_ptr_equal(temp, wp0) || weak_ptr_equal(temp, wp1));
        this_thread::yield();

        temp = atomic_warrnptr.exchange(wp1, memory_order::seq_cst);
        assert(weak_ptr_equal(temp, wp0) || weak_ptr_equal(temp, wp1));
        this_thread::yield();
    }
}

void test_weak_ptr_arrn_compare_exchange_weak() {
    weak_ptr<int[2]> wp0 = warrnptr0;
    weak_ptr<int[2]> wp1 = warrnptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        weak_ptr<int[2]> local = wp0;
        if (atomic_warrnptr.compare_exchange_weak(local, wp1)) {
            assert(weak_ptr_equal(local, wp0));
        } else {
            assert(weak_ptr_equal(local, wp1));
        }
        this_thread::yield();

        local = wp1;
        if (atomic_warrnptr.compare_exchange_weak(local, wp0, memory_order::seq_cst, memory_order::seq_cst)) {
            assert(weak_ptr_equal(local, wp1));
        } else {
            assert(weak_ptr_equal(local, wp0));
        }
        this_thread::yield();
    }
}

void test_weak_ptr_arrn_compare_exchange_strong() {
    weak_ptr<int[2]> wp0 = warrnptr0;
    weak_ptr<int[2]> wp1 = warrnptr1;
    for (uintmax_t i = 0; i < iterations; ++i) {
        weak_ptr<int[2]> local = wp0;
        if (atomic_warrnptr.compare_exchange_strong(local, wp1)) {
            assert(weak_ptr_equal(local, wp0));
        } else {
            assert(weak_ptr_equal(local, wp1));
        }
        this_thread::yield();

        local = wp1;
        if (atomic_warrnptr.compare_exchange_strong(local, wp0, memory_order::seq_cst, memory_order::seq_cst)) {
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

template <class AtomicType>
void ensure_member_calls_compile() {
    AtomicType instance;
    const AtomicType constInstance;
    auto loaded = instance.load();
    loaded      = constInstance.load();
    loaded      = instance;
    loaded      = constInstance;
    instance.store(loaded);
    loaded = instance.exchange(instance);
    loaded = instance.exchange(constInstance);
    if (instance.compare_exchange_weak(loaded, constInstance)) {
        // intentionally empty
    }
    if (instance.compare_exchange_strong(loaded, constInstance)) {
        // intentionally empty
    }
}

// LWG-3661: constinit atomic<shared_ptr<T>> a(nullptr); should work
constinit atomic<shared_ptr<bool>> a{};
constinit atomic<shared_ptr<bool>> b{nullptr};

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
    run_test(test_shared_ptr_arr_load_store);
    run_test(test_shared_ptr_arr_exchange);
    run_test(test_shared_ptr_arr_compare_exchange_weak);
    run_test(test_shared_ptr_arr_compare_exchange_strong);
    run_test(test_weak_ptr_arr_load_store);
    run_test(test_weak_ptr_arr_exchange);
    run_test(test_weak_ptr_arr_compare_exchange_weak);
    run_test(test_weak_ptr_arr_compare_exchange_strong);
    ensure_nonmember_calls_compile<atomic<shared_ptr<int[]>>>();
    ensure_nonmember_calls_compile<atomic<weak_ptr<int[]>>>();
    run_test(test_shared_ptr_arrn_load_store);
    run_test(test_shared_ptr_arrn_exchange);
    run_test(test_shared_ptr_arrn_compare_exchange_weak);
    run_test(test_shared_ptr_arrn_compare_exchange_strong);
    run_test(test_weak_ptr_arrn_load_store);
    run_test(test_weak_ptr_arrn_exchange);
    run_test(test_weak_ptr_arrn_compare_exchange_weak);
    run_test(test_weak_ptr_arrn_compare_exchange_strong);
    ensure_nonmember_calls_compile<atomic<shared_ptr<int[2]>>>();
    ensure_nonmember_calls_compile<atomic<weak_ptr<int[2]>>>();
    ensure_nonmember_calls_compile<atomic<shared_ptr<int[][2]>>>();
    ensure_nonmember_calls_compile<atomic<weak_ptr<int[][2]>>>();
    ensure_nonmember_calls_compile<atomic<shared_ptr<int[2][2]>>>();
    ensure_nonmember_calls_compile<atomic<weak_ptr<int[2][2]>>>();
    ensure_member_calls_compile<atomic<shared_ptr<int[][2]>>>();
    ensure_member_calls_compile<atomic<weak_ptr<int[][2]>>>();
    ensure_member_calls_compile<atomic<shared_ptr<int[2][2]>>>();
    ensure_member_calls_compile<atomic<weak_ptr<int[2][2]>>>();

#ifdef _DEBUG
    sptr0 = {};
    sptr1 = {};
    wptr0 = {};
    wptr1 = {};
    atomic_sptr.store({});
    atomic_wptr.store({});

    sarrptr0 = {};
    sarrptr1 = {};
    warrptr0 = {};
    warrptr1 = {};
    atomic_sarrptr.store({});
    atomic_warrptr.store({});

    sarrnptr0 = {};
    sarrnptr1 = {};
    warrnptr0 = {};
    warrnptr1 = {};
    atomic_sarrnptr.store({});
    atomic_warrnptr.store({});
    assert(!_CrtDumpMemoryLeaks());
#endif // _DEBUG
}
