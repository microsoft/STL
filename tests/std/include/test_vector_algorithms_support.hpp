// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <isa_availability.h>
#include <random>
#include <vector>

#pragma warning(disable : 4984) // 'if constexpr' is a C++17 language extension
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++17-extensions" // constexpr if is a C++17 extension
#endif // __clang__

void initialize_randomness(std::mt19937_64& gen) {
    constexpr size_t n = std::mt19937_64::state_size;
    constexpr size_t w = std::mt19937_64::word_size;
    static_assert(w % 32 == 0, "w should be evenly divisible by 32");
    constexpr size_t k = w / 32;

    std::vector<uint32_t> vec(n * k);

    std::random_device rd;
    std::generate(vec.begin(), vec.end(), ref(rd));

    printf("This is a randomized test.\n");
    printf("DO NOT IGNORE/RERUN ANY FAILURES.\n");
    printf("You must report them to the STL maintainers.\n\n");

    printf("Seed vector: ");
    for (const auto& e : vec) {
        printf("%u,", e);
    }
    printf("\n");

    std::seed_seq seq(vec.cbegin(), vec.cend());
    gen.seed(seq);
}

#if (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_CEE_PURE)
extern "C" long __isa_enabled;

void disable_instructions(ISA_AVAILABILITY isa) {
    __isa_enabled &= ~(1UL << static_cast<unsigned long>(isa));
}
#endif // (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_CEE_PURE)

constexpr size_t dataCount = 1024;

void run_randomized_tests_with_different_isa_levels(void tests(std::mt19937_64& gen)) {
    std::mt19937_64 gen;
    initialize_randomness(gen);

    tests(gen);
#ifndef _M_CEE_PURE
#if defined(_M_IX86) || defined(_M_X64)
    disable_instructions(__ISA_AVAILABLE_AVX2);
    tests(gen);

    disable_instructions(__ISA_AVAILABLE_SSE42);
    tests(gen);
#endif // defined(_M_IX86) || defined(_M_X64)
#endif // _M_CEE_PURE
}
