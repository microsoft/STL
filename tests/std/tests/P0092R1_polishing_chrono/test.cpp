// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//
// Tests the new functions added as part of P0092R1, "Polishing Chrono"
//

#include <cassert>
#include <chrono>
#include <cstdint>
#include <ratio>

using namespace std;
using namespace std::chrono;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

typedef duration<double> double_s;
typedef duration<double, milli> double_ms;
typedef duration<float> float_s;
typedef duration<float, milli> float_ms;

namespace ties_to_even_towards_zero_positive {
    constexpr milliseconds d1(2500);
    STATIC_ASSERT(floor<seconds>(d1).count() == 2);
    STATIC_ASSERT(ceil<seconds>(d1).count() == 3);
    STATIC_ASSERT(round<seconds>(d1).count() == 2);
    constexpr double_ms d2(2500.0);
    STATIC_ASSERT(floor<double_s>(d2).count() == 2.5);
    STATIC_ASSERT(ceil<double_s>(d2).count() == 2.5);
} // namespace ties_to_even_towards_zero_positive

namespace rounds_away_from_zero_positive {
    constexpr milliseconds d1(1729);
    STATIC_ASSERT(floor<seconds>(d1).count() == 1);
    STATIC_ASSERT(ceil<seconds>(d1).count() == 2);
    STATIC_ASSERT(round<seconds>(d1).count() == 2);
    constexpr double_ms d2(1875.0);
    STATIC_ASSERT(floor<double_s>(d2).count() == 1.875);
    STATIC_ASSERT(ceil<double_s>(d2).count() == 1.875);
} // namespace rounds_away_from_zero_positive

namespace ties_to_even_away_from_zero_positive {
    constexpr milliseconds d1(1500);
    STATIC_ASSERT(floor<seconds>(d1).count() == 1);
    STATIC_ASSERT(ceil<seconds>(d1).count() == 2);
    STATIC_ASSERT(round<seconds>(d1).count() == 2);
    constexpr double_ms d2(1500.0);
    STATIC_ASSERT(floor<double_s>(d2).count() == 1.5);
    STATIC_ASSERT(ceil<double_s>(d2).count() == 1.5);
} // namespace ties_to_even_away_from_zero_positive

namespace rounds_toward_zero_positive {
    constexpr milliseconds d1(1229);
    STATIC_ASSERT(floor<seconds>(d1).count() == 1);
    STATIC_ASSERT(ceil<seconds>(d1).count() == 2);
    STATIC_ASSERT(round<seconds>(d1).count() == 1);
    constexpr double_ms d2(1250.0);
    STATIC_ASSERT(floor<double_s>(d2).count() == 1.250);
    STATIC_ASSERT(ceil<double_s>(d2).count() == 1.250);
} // namespace rounds_toward_zero_positive

namespace exact_positive {
    constexpr milliseconds d1(1000);
    STATIC_ASSERT(floor<seconds>(d1).count() == 1);
    STATIC_ASSERT(ceil<seconds>(d1).count() == 1);
    STATIC_ASSERT(round<seconds>(d1).count() == 1);
    constexpr double_ms d2(1000.0);
    STATIC_ASSERT(floor<double_s>(d2).count() == 1.0);
    STATIC_ASSERT(ceil<double_s>(d2).count() == 1.0);
} // namespace exact_positive

namespace zero {
    constexpr milliseconds d1(0);
    STATIC_ASSERT(floor<seconds>(d1).count() == 0);
    STATIC_ASSERT(ceil<seconds>(d1).count() == 0);
    STATIC_ASSERT(round<seconds>(d1).count() == 0);
    constexpr double_ms d2(0.0);
    STATIC_ASSERT(floor<double_s>(d2).count() == 0.0);
    STATIC_ASSERT(ceil<double_s>(d2).count() == 0.0);
} // namespace zero

namespace exact_negative {
    constexpr milliseconds d1(-1000);
    STATIC_ASSERT(floor<seconds>(d1).count() == -1);
    STATIC_ASSERT(ceil<seconds>(d1).count() == -1);
    STATIC_ASSERT(round<seconds>(d1).count() == -1);
    constexpr double_ms d2(-1000.0);
    STATIC_ASSERT(floor<double_s>(d2).count() == -1.0);
    STATIC_ASSERT(ceil<double_s>(d2).count() == -1.0);
} // namespace exact_negative

namespace rounds_toward_zero_negative {
    constexpr milliseconds d1(-1229);
    STATIC_ASSERT(floor<seconds>(d1).count() == -2);
    STATIC_ASSERT(ceil<seconds>(d1).count() == -1);
    STATIC_ASSERT(round<seconds>(d1).count() == -1);
    constexpr double_ms d2(-1250.0);
    STATIC_ASSERT(floor<double_s>(d2).count() == -1.250);
    STATIC_ASSERT(ceil<double_s>(d2).count() == -1.250);
} // namespace rounds_toward_zero_negative

namespace ties_to_even_away_from_zero_negative {
    constexpr milliseconds d1(-1500);
    STATIC_ASSERT(floor<seconds>(d1).count() == -2);
    STATIC_ASSERT(ceil<seconds>(d1).count() == -1);
    STATIC_ASSERT(round<seconds>(d1).count() == -2);
    constexpr double_ms d2(-1500.0);
    STATIC_ASSERT(floor<double_s>(d2).count() == -1.5);
    STATIC_ASSERT(ceil<double_s>(d2).count() == -1.5);
} // namespace ties_to_even_away_from_zero_negative

namespace rounds_away_from_zero_negative {
    constexpr milliseconds d1(-1729);
    STATIC_ASSERT(floor<seconds>(d1).count() == -2);
    STATIC_ASSERT(ceil<seconds>(d1).count() == -1);
    STATIC_ASSERT(round<seconds>(d1).count() == -2);
    constexpr double_ms d2(-1875.0);
    STATIC_ASSERT(floor<double_s>(d2).count() == -1.875);
    STATIC_ASSERT(ceil<double_s>(d2).count() == -1.875);
} // namespace rounds_away_from_zero_negative

namespace ties_to_even_towards_zero_negative {
    constexpr milliseconds d1(-2500);
    STATIC_ASSERT(floor<seconds>(d1).count() == -3);
    STATIC_ASSERT(ceil<seconds>(d1).count() == -2);
    STATIC_ASSERT(round<seconds>(d1).count() == -2);
    constexpr double_ms d2(-2500.0);
    STATIC_ASSERT(floor<double_s>(d2).count() == -2.5);
    STATIC_ASSERT(ceil<double_s>(d2).count() == -2.5);
} // namespace ties_to_even_towards_zero_negative

namespace identical_types {
    constexpr milliseconds d1(-2500);
    STATIC_ASSERT(floor<milliseconds>(d1).count() == -2500);
    STATIC_ASSERT(ceil<milliseconds>(d1).count() == -2500);
    STATIC_ASSERT(round<milliseconds>(d1).count() == -2500);
    constexpr double_ms d2(-2500.0);
    STATIC_ASSERT(floor<double_ms>(d2).count() == -2500.0);
    STATIC_ASSERT(ceil<double_ms>(d2).count() == -2500.0);
} // namespace identical_types

namespace increasing_precision {
    constexpr seconds d1(2);
    STATIC_ASSERT(floor<milliseconds>(d1).count() == 2000);
    STATIC_ASSERT(ceil<milliseconds>(d1).count() == 2000);
    STATIC_ASSERT(round<milliseconds>(d1).count() == 2000);
    constexpr double_s d2(2.0);
    STATIC_ASSERT(floor<double_ms>(d2).count() == 2000.0);
    STATIC_ASSERT(ceil<double_ms>(d2).count() == 2000.0);
} // namespace increasing_precision

namespace floating_point_conversions {
    // double -> integral
    constexpr double_ms d1(1329.0);
    STATIC_ASSERT(floor<seconds>(d1).count() == 1);
    STATIC_ASSERT(ceil<seconds>(d1).count() == 2);
    STATIC_ASSERT(round<seconds>(d1).count() == 1);
    // float -> integral
    constexpr float_ms d2(1329.0f);
    STATIC_ASSERT(floor<seconds>(d2).count() == 1);
    STATIC_ASSERT(ceil<seconds>(d2).count() == 2);
    STATIC_ASSERT(round<seconds>(d2).count() == 1);
} // namespace floating_point_conversions

// Make sure round() handles cases where taking half the divisor itself
// truncates.
using odd_divisor = duration<intmax_t, ratio<1, 7>>;
inline constexpr odd_divisor operator"" _odd(unsigned long long val) {
    return odd_divisor(val);
}

// 10/7 ~= 1.42, which is closer to 1. It should *NOT* tie to even
STATIC_ASSERT(round<seconds>(10_odd).count() == 1);

// Test abs
STATIC_ASSERT(abs(1729ms).count() == 1729);
STATIC_ASSERT(abs(-1729ms).count() == 1729);
STATIC_ASSERT(abs(1329.0ms).count() == 1329.0);
STATIC_ASSERT(abs(-1329.0ms).count() == 1329.0);
STATIC_ASSERT(abs(1s).count() == 1);
STATIC_ASSERT(abs(-1s).count() == 1);
STATIC_ASSERT(abs(0s).count() == 0);

// Test time_point versions
constexpr time_point<system_clock, milliseconds> tp1(1729ms);
STATIC_ASSERT(floor<seconds>(tp1).time_since_epoch().count() == 1);
STATIC_ASSERT(ceil<seconds>(tp1).time_since_epoch().count() == 2);
STATIC_ASSERT(round<seconds>(tp1).time_since_epoch().count() == 2);


int overloaded(milliseconds) {
    return 11;
}

int overloaded(seconds) {
    return 22;
}

int overloaded(duration<int, exa>) {
    return 33;
}

int main() {
    // DevDiv-453376 "std::chrono::duration_cast lacks double support"
    duration<double> d(17.29);

    assert(duration_cast<duration<long long>>(d).count() == 17);


    // DevDiv-742944 "non-conforming return value for std::chrono::duration::operator%()"
    assert((milliseconds(1050) % seconds(1)).count() == 50);

    assert((milliseconds(1729) / 10).count() == 172);
    assert(minutes(4) / milliseconds(1729) == 138);
    assert((milliseconds(1729) % 10).count() == 9);
    assert((minutes(4) % milliseconds(1729)).count() == 1398);


    // DevDiv-1134356 "[STL]Test failed with std::chrono::milliseconds"
    // LWG-2094 "duration conversion overflow shouldn't participate in overload resolution"
    assert(overloaded(40ms) == 11);
    assert(overloaded(50s) == 22);
    assert(overloaded(duration<int, exa>(60)) == 33);
}
