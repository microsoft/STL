// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Test GH-685 "wait_for in condition_variable_any should unlock and lock"

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <mutex>

using namespace std;

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

    const chrono::nanoseconds interval{20};
    const chrono::nanoseconds zero_dur{0};

    void test_condition_variable_any() { // test wait functions of condition variables
        condition_variable_any cnd;
        {
            my_mutex mtx;
            unique_lock<my_mutex> guard{mtx};

            cnd.wait_for(mtx, zero_dur);
            assert(mtx.num_locks() == 2);

            cnd.wait_for(mtx, interval);
            assert(mtx.num_locks() == 3);

            cnd.wait_until(mtx, chrono::steady_clock::now());
            assert(mtx.num_locks() == 4);

            cnd.wait_until(mtx, chrono::steady_clock::now() + interval);
            assert(mtx.num_locks() == 5);
        }
    }
} // unnamed namespace

int main() {
    test_condition_variable_any();
}
