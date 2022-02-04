// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <cstdint>
#include <random>
#include <utility>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

int naive_iterations(const int bits, const uint64_t gmin, const uint64_t gmax) {
    // Naive implementation of [rand.util.canonical]. Note that for large values of range, it's possible that
    // log2(range) == bits when range < 2^bits. This can lead to incorrect results, so we can't use this function as
    // a reference for all values.

    const double range = static_cast<double>(gmax) - static_cast<double>(gmin) + 1.0;
    return max(1, static_cast<int>(ceil(static_cast<double>(bits) / log2(range))));
}

void test(const int target_bits) {
    // Increase the range until the number of iterations repeats.
    uint64_t range = 2;
    int k          = 0;
    int prev_k     = -1;
    while (k != prev_k) {
        prev_k       = exchange(k, naive_iterations(target_bits, 1, range));
        const int k1 = _Generate_canonical_iterations(target_bits, 1, range);
        assert(k == k1);
        ++range;
    }

    // Now only check the crossover points, where incrementing the range actually causes the number of iterations to
    // increase.
    while (--k != 0) {
        // The largest range such that k iterations generating [1,range] produces less than target_bits bits.
        if (k == 1) {
            range = ~uint64_t{0} >> (64 - target_bits);
        } else {
            range = static_cast<uint64_t>(ceil(pow(2.0, static_cast<double>(target_bits) / k))) - 1;
        }

        int k0 = (k == 1) ? 2 : naive_iterations(target_bits, 1, range);
        int k1 = _Generate_canonical_iterations(target_bits, 1, range);
        assert(k0 == k1);
        assert(k1 == k + 1);

        k0 = (k == 1) ? 1 : naive_iterations(target_bits, 0, range);
        k1 = _Generate_canonical_iterations(target_bits, 0, range);
        assert(k0 == k1);
        assert(k1 == k);
    }
}

int main() {
    STATIC_ASSERT(_Generate_canonical_iterations(53, 1, uint64_t{1} << 32) == 2);
    STATIC_ASSERT(_Generate_canonical_iterations(64, 0, ~uint64_t{0}) == 1);

    for (int bits = 0; bits <= 64; ++bits) {
        test(bits);
    }
}
