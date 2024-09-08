// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// DevCom-10342063 VSO-1804139 False positive C28020 iterating over single element std::array

#include <array>
#include <cstddef>
using namespace std;

bool IsSmallPrime(const int val) {
    static constexpr array<int, 1> small_primes{2};

    for (size_t i = 0; i < small_primes.size(); ++i) {
        if (val == small_primes[i]) {
            return true;
        }
    }

    return false;
}

bool IsPrime(const int val) {
    static constexpr array<int, 10> primes{2, 3, 5, 7, 11, 13, 17, 19, 23, 29};

    for (size_t i = 0; i < primes.size(); ++i) {
        if (val == primes[i]) {
            return true;
        }
    }

    return false;
}
