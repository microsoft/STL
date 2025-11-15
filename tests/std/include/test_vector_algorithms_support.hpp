// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
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
    const unsigned long as_ulong = static_cast<unsigned long>(isa);

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else // ^^^ defined(__clang__) / !defined(__clang__) vvv
#pragma warning(push)
#pragma warning(disable : 4996)
#endif // ^^^ !defined(__clang__) ^^^
    const char* const env_val = std::getenv("STL_TEST_DOWNLEVEL_HOST");
#ifdef __clang__
#pragma clang diagnostic pop
#else // ^^^ defined(__clang__) / !defined(__clang__) vvv
#pragma warning(pop)
#endif // ^^^ !defined(__clang__) ^^^

    if (env_val == nullptr || std::atoi(env_val) == 0) {
        if ((__isa_enabled & (1UL << as_ulong)) == 0) {
            std::printf("The feature %lu is not available, the test does not have full coverage!\n", as_ulong);
            std::printf("You can set environment variable STL_TEST_DOWNLEVEL_HOST to 1,\n"
                        "if you intentionally test on a host with not all features available.");
            abort();
        }
    }

    __isa_enabled &= ~(1UL << as_ulong);
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
