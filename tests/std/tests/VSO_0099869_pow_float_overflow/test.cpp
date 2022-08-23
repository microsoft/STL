// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <map>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

void test_VSO_99869_pow_returned_zero_for_very_small_values() {
    double negativeExponentResult        = pow(2.0, -1024);
    double oneOverPositiveExponentResult = pow(1.0 / 2.0, 1024);
    double doubleExponentResult          = pow(2.0, -1024.0);

    if (negativeExponentResult != oneOverPositiveExponentResult
        || oneOverPositiveExponentResult != doubleExponentResult) {
        puts("The following results should all be the same:");
        printf("pow(2.0, -1024): %E\n", negativeExponentResult);
        printf("pow(1.0/2.0, 1024): %E\n", oneOverPositiveExponentResult);
        printf("pow(2.0, -1024.0): %E\n", doubleExponentResult);
        abort();
    }
}

template <typename Target, typename Source>
Target reinterpret_as(Source f) {
    STATIC_ASSERT(sizeof(Target) == sizeof(Source));
    Target t;
    memcpy(&t, &f, sizeof(t));
    return t;
}

class test_std_pow_against_crt {
    uint32_t badClasses = 0;
    map<uint32_t, uint32_t> ulpOff;

    void single(uint32_t baseCandidate) {
        float input   = reinterpret_as<float>(baseCandidate);
        float powOut  = static_cast<float>(pow(input, 2));
        float powfOut = powf(input, 2.0f);
        int powClass  = fpclassify(powOut);
        int powfClass = fpclassify(powfOut);
        if (powClass != powfClass) {
            ++badClasses;
            printf("Bad class for 0x%08X (%E)\n", static_cast<unsigned int>(baseCandidate), input);
            return;
        }

        auto powInt  = reinterpret_as<uint32_t>(powOut);
        auto powfInt = reinterpret_as<uint32_t>(powfOut);
        if (powInt != powfInt) {
            uint32_t diff;
            if (powInt > powfInt) {
                diff = powInt - powfInt;
            } else {
                diff = powfInt - powInt;
            }

            ++ulpOff[diff];
        }
    }

public:
    void exhaustive() {
        for (uint32_t baseCandidate = 0u; baseCandidate < numeric_limits<uint32_t>::max(); ++baseCandidate) {
            this->single(baseCandidate);
        }

        this->single(numeric_limits<uint32_t>::max());
        for (auto const& p : ulpOff) {
            printf(
                "%05u ULP off: %05u values\n", static_cast<unsigned int>(p.second), static_cast<unsigned int>(p.first));
        }

        assert(badClasses == 0);
        assert(ulpOff.empty());
    }
};

int main() {
    test_VSO_99869_pow_returned_zero_for_very_small_values();

    // This test takes too long to run in the normal battery of tests; but
    // leaving the code here in case we make further changes to pow guts in the
    // future.
    // test_std_pow_against_crt{}.exhaustive();
}
