// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// N3485 20.11.7.1 [time.clock.system]/3:
// "static time_t to_time_t(const time_point& t) noexcept;
// Returns: A time_t object that represents the same point in time as t when both values are
// restricted to the coarser of the precisions of time_t and time_point. It is
// implementation defined whether values are rounded or truncated to the required precision."

#include <cassert>
#include <chrono>
#include <cstdint>
#include <limits>

using namespace std;
using namespace std::chrono;

constexpr auto int64_min = numeric_limits<int64_t>::min();
constexpr auto int64_max = numeric_limits<int64_t>::max();

void test(const system_clock::time_point& tp, const long long t1, const long long t2) {
    assert(tp.time_since_epoch().count() == t1);
    assert(system_clock::to_time_t(tp) == t2);
}

int main() {
    test(system_clock::time_point(system_clock::duration(13595108447931998LL)), 13595108447931998LL, 1359510844LL);

    test(system_clock::time_point(system_clock::duration(13595108501597374LL)), 13595108501597374LL, 1359510850LL);

    test(system_clock::time_point::min(),
        /* -9223372036854775808LL */ int64_min, -922337203685LL);

    test(system_clock::time_point::max(),
        /* 9223372036854775807LL */ int64_max, 922337203685LL);
}
