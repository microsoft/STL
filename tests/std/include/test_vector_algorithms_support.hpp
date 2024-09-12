// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <isa_availability.h>
#include <random>
#include <vector>

inline void initialize_randomness(std::mt19937_64& gen) {
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

    std::printf("Seed vector: ");
    for (const auto& e : vec) {
        std::printf("%u,", e);
    }
    std::printf("\n");

    std::seed_seq seq(vec.cbegin(), vec.cend());
    gen.seed(seq);
}

#if (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_CEE_PURE)
extern "C" long __isa_enabled;

inline void disable_instructions(ISA_AVAILABILITY isa) {
    __isa_enabled &= ~(1UL << static_cast<unsigned long>(isa));
}
#endif // (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_CEE_PURE)

constexpr std::size_t dataCount = 1024;

template <class TestFunc>
void run_randomized_tests_with_different_isa_levels(TestFunc tests) {
    std::mt19937_64 gen;
    initialize_randomness(gen);

    tests(gen);

#if (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_CEE_PURE)
    disable_instructions(__ISA_AVAILABLE_AVX2);
    tests(gen);

    disable_instructions(__ISA_AVAILABLE_SSE42);
    tests(gen);
#endif // (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_CEE_PURE)
}
