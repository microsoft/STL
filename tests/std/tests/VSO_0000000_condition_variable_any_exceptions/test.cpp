// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <mutex>
#include <thread>
#include <utility>

#include <test_death.hpp>

using namespace std;
using namespace std::chrono;

struct lock_exception : exception {};

struct throwing_mutex {
    deque<bool> lock_result;
    deque<bool> unlock_result;
    mutex state_lock;
    mutex inner_lock;

    void record_lock_result(const bool succeeds) {
        lock_guard<mutex> lck(state_lock);
        lock_result.push_back(succeeds);
    }

    void record_lock_success() {
        record_lock_result(true);
    }
    void record_lock_failure() {
        record_lock_result(false);
    }

    void record_unlock_result(const bool succeeds) {
        lock_guard<mutex> lck(state_lock);
        unlock_result.push_back(succeeds);
    }

    void record_unlock_success() {
        record_unlock_result(true);
    }
    void record_unlock_failure() {
        record_unlock_result(false);
    }

    void lock() {
        bool success_desired;
        {
            lock_guard<mutex> lck(state_lock);
            if (lock_result.empty()) {
                puts("Tried to lock too many times!");
                abort();
            }

            success_desired = lock_result.front();
            lock_result.pop_front();
        }

        if (success_desired) {
            inner_lock.lock();
        } else {
            throw lock_exception();
        }
    }

    void unlock() {
        bool success_desired;
        {
            lock_guard<mutex> lck(state_lock);
            if (unlock_result.empty()) {
                puts("Tried to unlock too many times!");
                abort();
            }

            success_desired = unlock_result.front();
            unlock_result.pop_front();
        }

        if (success_desired) {
            inner_lock.unlock();
        } else {
            throw lock_exception();
        }
    }
};

void test_case_condition_variable_any_unlock_ex_should_have_no_effect() {
    throwing_mutex mtx;
    condition_variable_any tested_unit;

    {
        mtx.record_lock_success();
        for (size_t idx = 0; idx < 5; ++idx) {
            mtx.record_unlock_failure();
        }

        mtx.record_unlock_success();

        unique_lock<throwing_mutex> mtx_lck(mtx);
        for (size_t idx = 0; idx < 5; ++idx) {
            try {
                // no timing assumptions since we're failing to unlock the mutex
                // which happens before we go to sleep
                (void) tested_unit.wait_for(mtx_lck, 50ms);
                abort();
            } catch (const lock_exception&) {
                assert(mtx_lck.owns_lock());
            }
        }
    }

    // before this change the throwing unlock would leave this thread irrevocably owning the
    // condition variable mutex, just make sure another thread can wait
    atomic<bool> done(false); // "spinlock"
    thread t([&]() {
        mtx.record_lock_success();
        mtx.record_unlock_success();
        mtx.record_lock_success();
        mtx.record_unlock_success();
        unique_lock<throwing_mutex> mtx_lck(mtx);
        tested_unit.wait(mtx_lck);
        done.store(true);
    });

    // this loop is just making sure notify_one is called after t has gone to sleep
    for (;;) {
        tested_unit.notify_one();
        this_thread::yield();
        if (done.load()) {
            break;
        }

        this_thread::sleep_for(100ms);
        if (done.load()) {
            break;
        }
    }

    t.join();
}

void test_case_condition_variable_any_lock_ex_should_terminate() {
    throwing_mutex mtx;
    condition_variable_any tested_unit;
    mtx.record_lock_success();
    mtx.record_unlock_success();
    mtx.record_lock_failure();
    unique_lock<throwing_mutex> mtx_lck(mtx);
    // no timing assumptions since we're failing to relock the mutex, which
    // terminates whether the condition variable times out or not
    (void) tested_unit.wait_for(mtx_lck, 50ms);
}

// Also test VSO-299271 "condition_variable_any::wait_until doesn't compile with less accurate time_points"
void test_case_condition_variable_any_less_accurate_time_points() {
    condition_variable_any tested_unit;
    const auto currentTime  = steady_clock::now();
    const auto nextDeadline = time_point_cast<milliseconds>(currentTime) + 5ms;
    recursive_mutex rm;
    unique_lock<recursive_mutex> lck(rm);
    (void) tested_unit.wait_until(lck, nextDeadline);
}

void normal_tests() {
    test_case_condition_variable_any_unlock_ex_should_have_no_effect();
    test_case_condition_variable_any_less_accurate_time_points();
}

// Test strengthened exception specification of condition_variable::wait
static_assert(noexcept(declval<condition_variable&>().wait(declval<unique_lock<mutex>&>())),
    "condition_variable::wait that takes no predicate should be noexcept");

// Test strengthened exception specification of notify_all_at_thread_exit
static_assert(noexcept(notify_all_at_thread_exit(declval<condition_variable&>(), declval<unique_lock<mutex>>())),
    "notify_all_at_thread_exit should be noexcept");

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec(normal_tests);
    exec.add_death_tests({test_case_condition_variable_any_lock_ex_should_terminate});
    return exec.run(argc, argv);
}
