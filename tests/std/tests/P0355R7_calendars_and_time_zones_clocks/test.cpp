// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>

using namespace std::chrono;

struct not_a_clock {
    bool rep();
    static char period;
    int duration();
    static float time_point;
    using is_steady = long;
    static int now;
};

struct real_fake_clock {
    using rep        = bool;
    using period     = char;
    using duration   = float;
    using time_point = int;
    static long is_steady;
    static short now();
};

static_assert(is_clock<steady_clock>::value, "steady_clock is not a clock");
static_assert(is_clock_v<steady_clock>, "steady_clock is not a clock");
static_assert(is_clock_v<real_fake_clock>, "real_fake_clock is not a clock");
static_assert(!is_clock_v<not_a_clock>, "not_a_clock is a clock");

int main() {} // COMPILE-ONLY
