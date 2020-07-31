// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <assert.h>
#include <execution>
#include <random>
#include <stdint.h>
#include <vector>

using namespace std;
using namespace std::execution;

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
                         microtest<URNG, int16_t, INT16_MIN, INT16_MAX>,
                         microtest<URNG, int16_t, INT16_MIN + 1, INT16_MAX>,
                         microtest<URNG, int16_t, INT16_MIN, INT16_MAX - 1>,
                         microtest<URNG, int16_t, INT16_MIN + 2, INT16_MAX - 3>,
                         microtest<URNG, int16_t, -4, 4>,
                         microtest<URNG, int16_t, 11, 22>,
                         microtest<URNG, int16_t, -44, -33>,
                         microtest<URNG, int16_t, 5, 6>,
                         microtest<URNG, int16_t, -7, -7>,

                         microtest<URNG, uint16_t, 0, UINT16_MAX>,
                         microtest<URNG, uint16_t, 1, UINT16_MAX>,
                         microtest<URNG, uint16_t, 0, UINT16_MAX - 1>,
                         microtest<URNG, uint16_t, 2, UINT16_MAX - 3>,
                         microtest<URNG, uint16_t, 123, 456>,
                         microtest<URNG, uint16_t, 1000, 1001>,
                         microtest<URNG, uint16_t, 777, 777>,

                         microtest<URNG, int32_t, INT32_MIN, INT32_MAX>,
                         microtest<URNG, int32_t, INT32_MIN + 2, INT32_MAX - 3>,
                         microtest<URNG, uint32_t, 0, UINT32_MAX>,
                         microtest<URNG, uint32_t, 2, UINT32_MAX - 3>,
                         microtest<URNG, int64_t, INT64_MIN, INT64_MAX>,
                         microtest<URNG, int64_t, INT64_MIN + 2, INT64_MAX - 3>,
                         microtest<URNG, uint64_t, 0, UINT64_MAX>, // Test DDB-181509 "TR1 VC9 SP1: Infinite loop in
                                                                   // uniform_int<unsigned long long>::_Eval()".
                         microtest<URNG, uint64_t, 2, UINT64_MAX - 3>,

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
    vector<fp_t> tests;

    add_tests<mt19937>(tests);
    add_tests<mt19937_64>(tests);
    add_tests<Wacky>(tests);

    for_each(par, tests.begin(), tests.end(), [](fp_t fn) { fn(); });
}
