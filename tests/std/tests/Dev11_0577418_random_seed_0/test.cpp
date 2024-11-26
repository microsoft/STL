// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdint>
#include <random>
#include <type_traits>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

template <typename Engine>
typename Engine::result_type run_10k(Engine engine) {
    engine.discard(9999);
    return engine();
}

int main() {
    assert(run_10k(minstd_rand0()) == 1043618065UL); // N4964 [rand.predef]/1
    assert(run_10k(mt19937()) == 4123659995UL); // N4964 [rand.predef]/3
    assert(run_10k(ranlux24_base()) == 7937952UL); // N4964 [rand.predef]/5

    // Also test VSO-214595 "subtract_with_carry_engine::seed should accept result_type"
    subtract_with_carry_engine<unsigned long long, 64, 10, 24> ull_swc;
    ull_swc.seed(0x12341234'00000000ULL);
    assert(run_10k(ull_swc) == 0x01316AEA'3646F686ULL); // libstdc++ 14.2 and libc++ 19.1 agree, Boost 1.86.0 disagrees

    assert(minstd_rand0(0) == minstd_rand0()); // N4964 [rand.eng.lcong]/5
    assert(mt19937(0) != mt19937()); // N4964 [rand.eng.mers]/6
    assert(ranlux24_base(0) == ranlux24_base()); // N4964 [rand.eng.sub]/7

    assert(run_10k(minstd_rand0(0)) == 1043618065UL); // QED
    assert(run_10k(mt19937(0)) == 1543171712UL); // Boost 1.86.0 agrees
    assert(run_10k(ranlux24_base(0)) == 7937952UL); // QED

    // Also test LWG-3809 Is std::subtract_with_carry_engine<uint16_t> supposed to work?
    STATIC_ASSERT(is_same_v<decltype(subtract_with_carry_engine<unsigned short, 16, 10, 24>::default_seed),
        const uint_least32_t>);
    STATIC_ASSERT(
        is_same_v<decltype(subtract_with_carry_engine<unsigned int, 32, 10, 24>::default_seed), const uint_least32_t>);
    STATIC_ASSERT(
        is_same_v<decltype(subtract_with_carry_engine<unsigned long, 32, 10, 24>::default_seed), const uint_least32_t>);
    STATIC_ASSERT(is_same_v<decltype(subtract_with_carry_engine<unsigned long long, 64, 10, 24>::default_seed),
        const uint_least32_t>);
    assert(run_10k(subtract_with_carry_engine<unsigned long long, 64, 10, 24>{}) == 6793538734622947770ULL);
}
