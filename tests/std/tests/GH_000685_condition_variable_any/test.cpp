// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Test GH-685 "wait_for in condition_variable_any should unlock and lock"
// Test LWG-4301 "condition_variable{_any}::wait_{for, until} should take timeout by value"

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <type_traits>

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

    // Minimal example inspired by LWG-4301, modified due to missing latch in C++11 to 20
    // and generalized to test all overloads of condition_variable{_any}::wait_{for, until}
    template <typename CV>
    void test_timeout_immutable(int test_number) {

        mutex m;
        CV cv;
        unique_lock<mutex> lock(m); // Ensure main thread waitins on cv before other_thread starts

        // Start with very short timeout and let other_thread change it to very large while main thread is waiting
        constexpr auto short_timeout = 100ms;
        constexpr auto long_timeout  = 2s;

        auto timeout_duration = short_timeout;
        auto timeout          = steady_clock::now() + timeout_duration;

        thread other_thread([&] {
            // Immediatelly blocks since the main thread owns lock.
            unique_lock<mutex> lock(m);

            // If the timeout provided to condition_variable{_any}::wait_{for, until} was mutable,
            // we will get timeout after much longer time
            timeout_duration = long_timeout;
            timeout          = steady_clock::now() + timeout_duration;
        });

        const auto wait_start = steady_clock::now();
#define CHECK_TIMEOUT_NOT_CHANGED() assert(steady_clock::now() - wait_start < long_timeout / 2);

        switch (test_number) {
        case 0:
            assert(cv.wait_until(lock, timeout) == cv_status::timeout);
            CHECK_TIMEOUT_NOT_CHANGED();
            break;

        case 1:
            assert(cv.wait_until(lock, timeout, [] { return false; }) == false);
            CHECK_TIMEOUT_NOT_CHANGED();
            break;

        case 2:
            assert(cv.wait_for(lock, timeout_duration) == cv_status::timeout);
            CHECK_TIMEOUT_NOT_CHANGED();
            break;

        case 3:
            assert(cv.wait_for(lock, timeout_duration, [] { return false; }) == false);
            CHECK_TIMEOUT_NOT_CHANGED();
            break;

#if _HAS_CXX20 // because of stop_token
        case 4:
            if constexpr (std::is_same_v<CV, std::condition_variable_any>) {
                stop_source source;
                assert(cv.wait_until(lock, source.get_token(), timeout, [] { return false; }) == false);
                CHECK_TIMEOUT_NOT_CHANGED();
            } else {
                assert(false);
            }
            break;

        case 5:
            if constexpr (std::is_same_v<CV, std::condition_variable_any>) {
                stop_source source;
                assert(cv.wait_for(lock, source.get_token(), timeout_duration, [] { return false; }) == false);
                CHECK_TIMEOUT_NOT_CHANGED();
            } else {
                assert(false);
            }
            break;
#endif // _HAS_CXX20

        default:
            assert(false);
        }

        other_thread.join();
    }
} // unnamed namespace

int main() {
    test_condition_variable_any();
    test_condition_variable_any_already_timed_out();

    test_timeout_immutable<std::condition_variable>(0);
    test_timeout_immutable<std::condition_variable>(1);
    test_timeout_immutable<std::condition_variable>(2);
    test_timeout_immutable<std::condition_variable>(3);

    test_timeout_immutable<std::condition_variable_any>(0);
    test_timeout_immutable<std::condition_variable_any>(1);
    test_timeout_immutable<std::condition_variable_any>(2);
    test_timeout_immutable<std::condition_variable_any>(3);
#if _HAS_CXX20
    test_timeout_immutable<std::condition_variable_any>(4);
    test_timeout_immutable<std::condition_variable_any>(5);
#endif // _HAS_CXX20
}
