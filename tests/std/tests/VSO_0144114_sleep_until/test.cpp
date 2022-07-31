// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <ratio>
#include <thread>

#include <test_thread_support.hpp>

using namespace std;
using namespace std::chrono;

template <typename ClockType>
void assert_sleep_until_invariants() {
    watchdog wd(60s);
    const auto target = ClockType::now() + 1s;
    this_thread::sleep_until(target);
    // When VSO-166543 "wait_until and sleep_until functions don't behave correctly for user-defined clocks"
    // is fixed, this 100ms "fudge factor" must be removed.
    const auto actual = ClockType::now() + 100ms;

    assert("sleep_until did not wait long enough" && target <= actual);
}

void test_high_resolution_clock_waits_should_not_return_immediately() {
    // VSO-144114 "<thread>: C++ (stdlib): sleep_until does not convert high_resolution_clock::time_point..."
    assert_sleep_until_invariants<high_resolution_clock>();
}

void test_sleep_for_should_compile_with_floating_durations() {
    // VSO-122248 "<thread>: std::this_thread::sleep_for(1.5s) error"
    // VSO-106920 (duplicate use in condition_variable)
    // VSO-153606 (exact duplicate)
    this_thread::sleep_for(duration<double, milli>(20.0));
    this_thread::sleep_for(duration<float, milli>(20.0f));
}

struct tomorrow_clock {
    using rep        = system_clock::rep;
    using period     = system_clock::period;
    using duration   = system_clock::duration;
    using time_point = std::chrono::time_point<tomorrow_clock, duration>;

    static const bool is_steady = system_clock::is_steady;

    static time_point now() noexcept {
        const auto today = system_clock::now().time_since_epoch();
        return static_cast<time_point>(today + 24h);
    }
};

void test_sleep_until_works_with_user_defined_clocks() {
    // VSO-99350 "<thread>: std::this_thread::sleep_until doesn't work with user defined clocks"
    // VSO-123833 (exact duplicate)
    assert_sleep_until_invariants<tomorrow_clock>();
}

int main() {
    test_high_resolution_clock_waits_should_not_return_immediately();
    test_sleep_for_should_compile_with_floating_durations();
    test_sleep_until_works_with_user_defined_clocks();
}
