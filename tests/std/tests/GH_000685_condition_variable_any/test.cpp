// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Test GH-685 "wait_for in condition_variable_any should unlock and lock"

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <mutex>
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
} // unnamed namespace

int main() {
    test_condition_variable_any();
    test_condition_variable_any_already_timed_out();
}
