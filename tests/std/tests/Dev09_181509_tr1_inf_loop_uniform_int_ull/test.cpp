// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <execution>
#include <limits>
#include <random>
#include <vector>

using namespace std;
using namespace std::execution;

constexpr auto int16_min  = numeric_limits<int16_t>::min();
constexpr auto int16_max  = numeric_limits<int16_t>::max();
constexpr auto int32_min  = numeric_limits<int32_t>::min();
constexpr auto int32_max  = numeric_limits<int32_t>::max();
constexpr auto int64_min  = numeric_limits<int64_t>::min();
constexpr auto int64_max  = numeric_limits<int64_t>::max();
constexpr auto uint16_max = numeric_limits<uint16_t>::max();
constexpr auto uint32_max = numeric_limits<uint32_t>::max();
constexpr auto uint64_max = numeric_limits<uint64_t>::max();

constexpr uint32_t N = 500'000;

template <typename URNG, typename T, T A, T B>
void microtest() {
    URNG engine;
    uniform_int_distribution<T> dist(A, B);

    for (uint32_t i = 0; i < N; ++i) {
        const T val = dist(engine);

        assert(val >= A && val <= B);
    }
}

using fp_t = void (*)();
template <typename URNG>
void add_tests(vector<fp_t>& tests) {
    // Test DevDiv-83370 "uniform_int_distribution isn't uniform".
    tests.insert(
        tests.end(), {
                         microtest<URNG, int16_t, int16_min, int16_max>,
                         microtest<URNG, int16_t, int16_min + 1, int16_max>,
                         microtest<URNG, int16_t, int16_min, int16_max - 1>,
                         microtest<URNG, int16_t, int16_min + 2, int16_max - 3>,
                         microtest<URNG, int16_t, -4, 4>,
                         microtest<URNG, int16_t, 11, 22>,
                         microtest<URNG, int16_t, -44, -33>,
                         microtest<URNG, int16_t, 5, 6>,
                         microtest<URNG, int16_t, -7, -7>,

                         microtest<URNG, uint16_t, 0, uint16_max>,
                         microtest<URNG, uint16_t, 1, uint16_max>,
                         microtest<URNG, uint16_t, 0, uint16_max - 1>,
                         microtest<URNG, uint16_t, 2, uint16_max - 3>,
                         microtest<URNG, uint16_t, 123, 456>,
                         microtest<URNG, uint16_t, 1000, 1001>,
                         microtest<URNG, uint16_t, 777, 777>,

                         microtest<URNG, int32_t, int32_min, int32_max>,
                         microtest<URNG, int32_t, int32_min + 2, int32_max - 3>,
                         microtest<URNG, uint32_t, 0, uint32_max>,
                         microtest<URNG, uint32_t, 2, uint32_max - 3>,
                         microtest<URNG, int64_t, int64_min, int64_max>,
                         microtest<URNG, int64_t, int64_min + 2, int64_max - 3>,
                         microtest<URNG, uint64_t, 0, uint64_max>, // Test DDB-181509 "TR1 VC9 SP1: Infinite loop in
                                                                   // uniform_int<unsigned long long>::_Eval()".
                         microtest<URNG, uint64_t, 2, uint64_max - 3>,

                         microtest<URNG, int32_t, -4, 4>,
                         microtest<URNG, int32_t, 11, 22>,
                         microtest<URNG, int32_t, -44, -33>,
                         microtest<URNG, int32_t, 5, 6>,
                         microtest<URNG, int32_t, -7, -7>,

                         microtest<URNG, uint32_t, 123, 456>,
                         microtest<URNG, uint32_t, 1000, 1001>,
                         microtest<URNG, uint32_t, 777, 777>,

                         microtest<URNG, int64_t, -4, 4>,
                         microtest<URNG, int64_t, 11, 22>,
                         microtest<URNG, int64_t, -44, -33>,
                         microtest<URNG, int64_t, 5, 6>,
                         microtest<URNG, int64_t, -7, -7>,

                         microtest<URNG, uint64_t, 123, 456>,
                         microtest<URNG, uint64_t, 1000, 1001>,
                         microtest<URNG, uint64_t, 777, 777>,
                     });
}

class Wacky {
public:
    Wacky() : mt(1701) {}

    typedef unsigned long result_type;

    static result_type min() {
        return 0;
    }

    static result_type max() {
        return 90000;
    }

    result_type operator()() {
        for (;;) {
            const result_type ret = mt() & 0x1FFFFUL;

            if (ret >= min() && ret <= max()) {
                return ret;
            }
        }
    }

private:
    mt19937 mt;
};

int main() {
#ifndef _M_CEE // TRANSITION, VSO-1659695
    vector<fp_t> tests;

    add_tests<mt19937>(tests);
    add_tests<mt19937_64>(tests);
    add_tests<Wacky>(tests);

    for_each(par, tests.begin(), tests.end(), [](fp_t fn) { fn(); });
#endif // _M_CEE
}
