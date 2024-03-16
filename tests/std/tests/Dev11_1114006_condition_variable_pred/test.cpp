// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Test DevDiv-1114006 "[+ VS2015] conditional_variable with predicate does not behave according to the standard".

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <mutex>
#include <thread>

using namespace std;
using namespace std::chrono;

struct Pred {
    unsigned int& counter_;
    explicit Pred(unsigned int& counter) : counter_(counter) {}

    // Note that the standard actually requires these to be copyable. As a "nonstandard extension" we want
    // to ensure we don't copy them, because copying some functors (e.g. std::function) is comparatively
    // expensive, and even for relatively cheap to copy function objects we care (somewhat) about debug
    // mode perf.
    Pred(const Pred&)            = delete;
    Pred(Pred&& o)               = default;
    Pred& operator=(const Pred&) = delete;
    Pred& operator=(Pred&&)      = delete;

    bool operator()() {
        switch (counter_) {
        case 0:
            return false;

        case 1:
            counter_ = 2;
            return true;

        default:
            abort();
        }
    }
};

template <typename CV>
void test(const int waiter, const int sleeper) {
    const auto Timeout = 10s; // Timing assumption: This will be plenty of time.
    const auto Snooze  = 100ms;

    unsigned int counter = 0;
    CV cv;
    mutex mut;

    thread t0([&] {
        if (sleeper == 0) {
            this_thread::sleep_for(Snooze);
        }

        {
            unique_lock<mutex> ul(mut);

            switch (waiter) {
            case 0:
                assert(cv.wait_for(ul, Timeout, Pred{counter}));
                break;

            case 1:
                assert(cv.wait_until(ul, steady_clock::now() + Timeout, Pred{counter}));
                break;

            case 2:
                cv.wait(ul, Pred{counter});
                break;

            default:
                abort();
            }

            assert(counter == 2);
        }
    });

    thread t1([&] {
        if (sleeper == 1) {
            this_thread::sleep_for(Snooze);
        }

        {
            lock_guard<mutex> g(mut);
            assert(counter == 0);
            counter = 1;
        }

        cv.notify_one();
    });

    t0.join();
    t1.join();
}

#ifndef _M_CEE // TRANSITION, VSO-1659496
// GH-140: "STL: We should _STD qualify _Ugly function calls to avoid ADL"
template <class T>
struct tagged_pred {
    bool operator()() const noexcept {
        return true;
    }
};

template <class T>
struct holder {
    T t;
};

struct incomplete;

template <class CV>
void test_adl_proof_waiting() { // COMPILE-ONLY
    using validating_pred = tagged_pred<holder<incomplete>>;

    mutex mut;
    {
        CV cv;
        unique_lock<mutex> guard(mut);
        (void) cv.wait(guard, validating_pred{});
    }
    {
        CV cv;
        unique_lock<mutex> guard(mut);
        (void) cv.wait_for(guard, 1ms, validating_pred{});
    }
    {
        CV cv;
        unique_lock<mutex> guard(mut);
        (void) cv.wait_until(guard, steady_clock::now() + 1ms, validating_pred{});
    }
}

void test_adl_proof_waiting_all() { // COMPILE-ONLY
    test_adl_proof_waiting<condition_variable>();
    test_adl_proof_waiting<condition_variable_any>();
}
#endif // ^^^ no workaround ^^^

int main() {
    for (int waiter = 0; waiter < 3; ++waiter) {
        for (int sleeper = 0; sleeper < 3; ++sleeper) { // Intentionally test a case without sleeping.
            test<condition_variable>(waiter, sleeper);
            test<condition_variable_any>(waiter, sleeper);
        }
    }
}
