// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>

using namespace std::chrono;

struct _Not_a_clock {
    bool rep();
    static char period;
    int duration();
    static float time_point;
    using is_steady = long;
    static int now;
};

struct _Real_fake_clock {
    using rep        = bool;
    using period     = char;
    using duration   = float;
    using time_point = int;
    static long is_steady;
    static short now();
};

static_assert(is_clock<steady_clock>::value, "steady_clock is not a clock");
static_assert(is_clock_v<steady_clock>, "steady_clock is not a clock");
static_assert(is_clock_v<_Real_fake_clock>, "_Real_fake_clock is not a clock");
static_assert(!is_clock_v<_Not_a_clock>, "_Not_a_clock is a clock");

int main() {}
