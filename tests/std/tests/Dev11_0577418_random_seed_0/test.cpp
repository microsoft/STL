// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <random>

using namespace std;

template <typename Engine>
typename Engine::result_type run_10k(Engine engine) {
    engine.discard(9999);
    return engine();
}

int main() {
    assert(run_10k(minstd_rand0()) == 1043618065UL); // N3485 26.5.5 [rand.predef]/1
    assert(run_10k(mt19937()) == 4123659995UL); // N3485 26.5.5 [rand.predef]/3
    assert(run_10k(ranlux24_base()) == 7937952UL); // N3485 26.5.5 [rand.predef]/5

    // Also test VSO-214595 "subtract_with_carry_engine::seed should accept result_type"
    subtract_with_carry_engine<unsigned long long, 64, 10, 24> ull_swc;
    ull_swc.seed(0x12341234'00000000ULL);
    assert(run_10k(ull_swc) == 0x01316AEA'3646F686ULL); // libstdc++ and libc++ agree (boost 1.60.0 disagrees)

    assert(minstd_rand0(0) == minstd_rand0()); // N3485 26.5.3.1 [rand.eng.lcong]/5
    assert(mt19937(0) != mt19937()); // N3485 26.5.3.2 [rand.eng.mers]/6
    assert(ranlux24_base(0) == ranlux24_base()); // N3485 26.5.3.3 [rand.eng.sub]/7

    assert(run_10k(minstd_rand0(0)) == 1043618065UL); // QED
    assert(run_10k(mt19937(0)) == 1543171712UL); // Boost 1.52.0 agrees
    assert(run_10k(ranlux24_base(0)) == 7937952UL); // QED
}
