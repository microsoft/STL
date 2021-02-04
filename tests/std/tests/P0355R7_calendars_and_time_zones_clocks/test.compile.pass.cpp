// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

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

struct no_rep {
    using period     = char;
    using duration   = float;
    using time_point = int;
    static long is_steady;
    static short now();
};

struct no_period {
    using rep        = bool;
    using duration   = float;
    using time_point = int;
    static long is_steady;
    static short now();
};

struct no_duration {
    using rep        = bool;
    using period     = char;
    using time_point = int;
    static long is_steady;
    static short now();
};

struct no_time_point {
    using rep      = bool;
    using period   = char;
    using duration = float;
    static long is_steady;
    static short now();
};

struct no_steady {
    using rep        = bool;
    using period     = char;
    using duration   = float;
    using time_point = int;
    static short now();
};

struct no_now {
    using rep        = bool;
    using period     = char;
    using duration   = float;
    using time_point = int;
    static long is_steady;
};

static_assert(is_clock<steady_clock>::value, "steady_clock is not a clock");
static_assert(is_clock_v<steady_clock>, "steady_clock is not a clock");
static_assert(is_clock_v<real_fake_clock>, "real_fake_clock is not a clock");
static_assert(!is_clock_v<not_a_clock>, "not_a_clock is a clock");

static_assert(!is_clock_v<no_rep>, "no_rep is a clock");
static_assert(!is_clock_v<no_period>, "no_period is a clock");
static_assert(!is_clock_v<no_duration>, "no_duration is a clock");
static_assert(!is_clock_v<no_time_point>, "no_time_point is a clock");
static_assert(!is_clock_v<no_steady>, "no_steady is a clock");
static_assert(!is_clock_v<no_now>, "no_now is a clock");

int main() {} // COMPILE-ONLY
