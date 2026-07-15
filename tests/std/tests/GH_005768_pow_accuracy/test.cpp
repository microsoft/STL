// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <random>
#include <vector>

// INTENTIONALLY AVOIDED: using namespace std;

void initialize_randomness(std::mt19937_64& gen) {
    constexpr std::size_t n = std::mt19937_64::state_size;
    constexpr std::size_t w = std::mt19937_64::word_size;
    static_assert(w % 32 == 0, "w should be evenly divisible by 32");
    constexpr std::size_t k = w / 32;

    std::vector<std::uint32_t> vec(n * k);

    std::random_device rd;
    std::generate(vec.begin(), vec.end(), std::ref(rd));

    std::printf("This is a randomized test.\n");
    std::printf("DO NOT IGNORE/RERUN ANY FAILURES.\n");
    std::printf("You must report them to the STL maintainers.\n\n");

    std::seed_seq seq(vec.cbegin(), vec.cend());
    gen.seed(seq);
}

void check_equal(const float val, const float actual, const float squared) {
    if (actual != squared) {
        std::printf("val: %.6a; actual: %.6a; squared: %.6a\n", val, actual, squared);
    }
    assert(actual == squared);
}

void check_equal(const double val, const double actual, const double squared) {
    if (actual != squared) {
        std::printf("val: %a; actual: %a; squared: %a\n", val, actual, squared);
    }
    assert(actual == squared);
}

void check_equal(const long double val, const long double actual, const long double squared) {
    if (actual != squared) {
        std::printf("val: %La; actual: %La; squared: %La\n", val, actual, squared);
    }
    assert(actual == squared);
}

void test_square_flt(const float val, const float squared) {
    // float * float is float, N5014 [expr.arith.conv]/1.4.1.
    check_equal(val, val * val, squared);

    // For std::pow(float, int), the arguments are effectively cast to double, N5014 [cmath.syn]/3.
    const double as_dbl = static_cast<double>(val);
    check_equal(as_dbl, std::pow(val, 2), as_dbl * as_dbl);
}

void test_square_dbl(const double val, const double squared) {
    check_equal(val, val * val, squared);
    check_equal(val, std::pow(val, 2), squared);
}

void test_square_ldbl(const long double val, const long double squared) {
    check_equal(val, val * val, squared);
    check_equal(val, std::pow(val, 2), squared);
}

// GH-5768 <cmath>: Calling UCRT ::pow(x, 2) is less accurate than x * x
void test_gh_5768_manually_verified() {
    // Manually verified to be correctly rounded:
    test_square_flt(0x1.bffff4p-1f, 0x1.87ffecp-1f);
    test_square_flt(0x1.bc0040p-2f, 0x1.810870p-3f);
    test_square_flt(0x1.2b7bc2p-1f, 0x1.5e5a52p-2f);

    test_square_dbl(0x1.ec9a50154a6f9p-1, 0x1.d9f0c06b2463ep-1);
    test_square_dbl(0x1.12814d2dd432cp-1, 0x1.26590a84f9b12p-2);
    test_square_dbl(0x1.33994b0b751ccp-3, 0x1.719905c84494ap-6);

    test_square_ldbl(0x1.ec9a50154a6f9p-1L, 0x1.d9f0c06b2463ep-1L);
    test_square_ldbl(0x1.12814d2dd432cp-1L, 0x1.26590a84f9b12p-2L);
    test_square_ldbl(0x1.33994b0b751ccp-3L, 0x1.719905c84494ap-6L);
}

void test_gh_5768_randomized(std::mt19937_64& gen) {
    const int Trials = 1'000'000;

    {
        std::uniform_real_distribution<float> dist{-10.0f, 10.0f};
        for (int i = 0; i < Trials; ++i) {
            const auto val = dist(gen);
            test_square_flt(val, val * val);
        }
    }

    {
        std::uniform_real_distribution<double> dist{-10.0, 10.0};
        for (int i = 0; i < Trials; ++i) {
            const auto val = dist(gen);
            test_square_dbl(val, val * val);
        }
    }

    {
        std::uniform_real_distribution<long double> dist{-10.0L, 10.0L};
        for (int i = 0; i < Trials; ++i) {
            const auto val = dist(gen);
            test_square_ldbl(val, val * val);
        }
    }
}

int main() {
    test_gh_5768_manually_verified();

    std::mt19937_64 gen;
    initialize_randomness(gen);
    test_gh_5768_randomized(gen);
}
