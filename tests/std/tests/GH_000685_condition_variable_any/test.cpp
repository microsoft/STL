// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Test GH-685 "wait_for in condition_variable_any should unlock and lock"
// Test LWG-4301 "condition_variable{_any}::wait_{for, until} should take timeout by value"

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <thread>
#include <type_traits>

#if _HAS_CXX20
#include <stop_token>
#endif // _HAS_CXX20

using namespace std;
using namespace std::chrono;

namespace {
    class my_mutex { // user-defined mutex type
    public:
        void lock() {
            mtx.lock();
            ++num_lock;
        }

        void unlock() {
            mtx.unlock();
        }

        int num_locks() const {
            return num_lock;
        }

    private:
        mutex mtx;
        int num_lock = 0;
    };

    const nanoseconds interval{20};
    const nanoseconds zero_dur{0};

    void test_condition_variable_any() { // test wait functions of condition variables
        condition_variable_any cnd;
        {
            my_mutex mtx;
            unique_lock<my_mutex> guard{mtx};

            cnd.wait_for(mtx, zero_dur);
            assert(mtx.num_locks() == 2);

            cnd.wait_for(mtx, interval);
            assert(mtx.num_locks() == 3);

            cnd.wait_until(mtx, steady_clock::now());
            assert(mtx.num_locks() == 4);

            cnd.wait_until(mtx, steady_clock::now() + interval);
            assert(mtx.num_locks() == 5);
        }
    }

    void test_condition_variable_any_already_timed_out() {
        using unsigned_duration = duration<make_unsigned_t<system_clock::rep>, system_clock::period>;
        const auto right_now    = time_point_cast<unsigned_duration>(system_clock::now());
        const auto yesterday    = right_now - hours{24};
        assert(yesterday - right_now > system_clock::duration::zero()); // unsigned overflow

        my_mutex m;
        condition_variable_any cond;
        unique_lock<my_mutex> guard(m);

        const auto status = cond.wait_until(m, yesterday);
        assert(status == cv_status::timeout);
        assert(m.num_locks() == 2);

        assert(cond.wait_until(m, yesterday, []() { return false; }) == false);
        assert(m.num_locks() == 3);

#if _HAS_CXX20
        stop_token token;
        assert(cond.wait_until(m, token, yesterday, []() { return false; }) == false);
        assert(m.num_locks() == 4);
#endif // _HAS_CXX20
    }

    // Minimal example inspired by LWG-4301, modified due to missing std::latch before C++20
    // and generalized to test all overloads of condition_variable{_any}::wait_{for, until}.
    // Idea: Make the main thread wait for a CV with a short timeout and modify it from another thread in the meantime.
    // If the main thread wait times out after a short time, the modification did not influence the ongoing wait.
    template <typename CV>
    void test_timeout_immutable(const int test_number, const int retries_remaining = 5) {
        printf("\ntest %d\n", test_number);

        mutex m;
        CV cv;
        unique_lock<mutex> main_lock(m); // Prevent other thread from modifying timeout too early

        // Start with very short timeout and let other_thread change it to very large while main thread is waiting
        constexpr auto short_timeout = 1s;
        constexpr auto long_timeout  = 10s;

        atomic_flag waiting_for_other_thread{};
        waiting_for_other_thread.test_and_set();

        const auto wait_start = steady_clock::now();
        auto timeout_duration = short_timeout;
        auto timeout          = wait_start + timeout_duration;

        const auto set_timeout = [&](const auto new_timeout) {
            timeout_duration = new_timeout;
            timeout          = steady_clock::now() + new_timeout;
        };

        thread other_thread([&] {
            printf(
                "thread start after %lld ms\n", duration_cast<milliseconds>(steady_clock::now() - wait_start).count());
            waiting_for_other_thread.clear();
            // Immediately blocks since the main thread owns the mutex m.
            lock_guard<mutex> other_lock(m);
            puts("thread lock");

            // If the timeout provided to condition_variable{_any}::wait_{for, until} was mutable,
            // we will get timeout in the main thread after much longer time
            set_timeout(long_timeout);
            puts("thread end");
        });

        while (waiting_for_other_thread.test_and_set()) {
            this_thread::yield(); // freeze the main thread from proceeding until other thread is started
        }
        printf("main resumed after %lld ms\n", duration_cast<milliseconds>(steady_clock::now() - wait_start).count());
        set_timeout(short_timeout);

        puts("main waiting");
        const bool cv_wait_timed_out = [&] {
            switch (test_number) {
            case 0:
                return cv.wait_until(main_lock, timeout) == cv_status::timeout;

            case 1:
                return cv.wait_until(main_lock, timeout, [] { return false; }) == false;

            case 2:
                return cv.wait_for(main_lock, timeout_duration) == cv_status::timeout;

            case 3:
                return cv.wait_for(main_lock, timeout_duration, [] { return false; }) == false;

#if _HAS_CXX20 // because of stop_token
            case 4:
                if constexpr (is_same_v<CV, condition_variable_any>) {
                    stop_source source;
                    return cv.wait_until(main_lock, source.get_token(), timeout, [] { return false; }) == false;
                } else {
                    assert(false); // test not supported for std::condition_variable
                    return false;
                }

            case 5:
                if constexpr (is_same_v<CV, condition_variable_any>) {
                    stop_source source;
                    return cv.wait_for(main_lock, source.get_token(), timeout_duration, [] { return false; }) == false;
                } else {
                    assert(false); // test not supported for std::condition_variable
                    return false;
                }
#endif // _HAS_CXX20

            default:
                assert(false);
                return false;
            }
        }();

        const auto elapsed = steady_clock::now() - wait_start;

        if (!cv_wait_timed_out) {
            if (retries_remaining > 0) {
                printf("unexpected wakeup after %lld ms, retry %d...\n", duration_cast<milliseconds>(elapsed).count(),
                    retries_remaining);
                test_timeout_immutable<CV>(test_number, retries_remaining - 1); // recurse to try the test again
            } else {
                puts("Too many unexpected wakeups");
                assert(false);
            }
        } else {
            assert(elapsed < long_timeout / 2);
            printf("wait end after %lld ms\n", duration_cast<milliseconds>(elapsed).count());
        }

        // Make sure the child thread has indeed finished (so the next join does not block)
        assert(timeout_duration == long_timeout);
        other_thread.join();
    }
} // unnamed namespace

int main() {
    test_condition_variable_any();
    test_condition_variable_any_already_timed_out();

    puts("condition_variable");
    test_timeout_immutable<condition_variable>(0);
    test_timeout_immutable<condition_variable>(1);
    test_timeout_immutable<condition_variable>(2);
    test_timeout_immutable<condition_variable>(3);

    puts("condition_variable_any");
    test_timeout_immutable<condition_variable_any>(0);
    test_timeout_immutable<condition_variable_any>(1);
    test_timeout_immutable<condition_variable_any>(2);
    test_timeout_immutable<condition_variable_any>(3);
#if _HAS_CXX20
    test_timeout_immutable<condition_variable_any>(4);
    test_timeout_immutable<condition_variable_any>(5);
#endif // _HAS_CXX20
}
