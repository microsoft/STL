// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <limits>
#include <type_traits>

using namespace std;
using namespace std::chrono;

#define DURATION_TEST(TYPE, BITS, ...)                                                                                \
    static_assert(                                                                                                    \
        is_integral_v<TYPE::rep> && is_signed_v<TYPE::rep>, "chrono::" #TYPE "::rep is not a signed integral type."); \
    static_assert(                                                                                                    \
        numeric_limits<TYPE::rep>::digits >= BITS, "chrono::" #TYPE "::rep is not at least " #BITS " bits.");         \
    static_assert(is_same_v<TYPE::period, __VA_ARGS__>, "chrono::" #TYPE "::period is not " #__VA_ARGS__ ".");

DURATION_TEST(days, 25, ratio_multiply<ratio<24>, hours::period>)
DURATION_TEST(weeks, 22, ratio_multiply<ratio<7>, days::period>)
DURATION_TEST(months, 20, ratio_divide<years::period, ratio<12>>)
DURATION_TEST(years, 17, ratio_multiply<ratio<146097, 400>, days::period>)

static_assert(is_same_v<sys_seconds, time_point<system_clock, seconds>>, //
    "sys_seconds is not time_point<system_clock, seconds>.");
static_assert(is_same_v<sys_days, time_point<system_clock, days>>, //
    "sys_days is not time_point<system_clock, days>.");

static_assert(is_same_v<local_seconds, time_point<local_t, seconds>>, //
    "local_seconds is not time_point<local_t, seconds>.");
static_assert(is_same_v<local_days, time_point<local_t, days>>, //
    "local_days is not time_point<local_t, days>.");

constexpr bool test() {
    steady_clock::time_point tp1;

    static_assert(noexcept(++tp1)); // strengthened
    static_assert(noexcept(tp1++)); // strengthened
    static_assert(noexcept(--tp1)); // strengthened
    static_assert(noexcept(tp1--)); // strengthened

    auto tp2 = tp1++;
    assert(tp1.time_since_epoch().count() == 1);
    assert(tp2.time_since_epoch().count() == 0);
    tp2 = ++tp1;
    assert(tp1.time_since_epoch().count() == 2);
    assert(tp2.time_since_epoch().count() == 2);

    tp2 = tp1--;
    assert(tp1.time_since_epoch().count() == 1);
    assert(tp2.time_since_epoch().count() == 2);
    tp2 = --tp1;
    assert(tp1.time_since_epoch().count() == 0);
    assert(tp2.time_since_epoch().count() == 0);

    return true;
}

int main() {
    test();
    static_assert(test());
}
