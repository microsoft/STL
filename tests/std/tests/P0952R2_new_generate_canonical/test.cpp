// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <random>
#include <utility>

using namespace std;

template <class Real, size_t Bits, class Engine>
Real generate_with_ibe() {
    independent_bits_engine<Engine, Bits, uint64_t> ibe;
    return generate_canonical<Real, 64>(ibe);
}

int main() {
    {
        // edge case, Bits == 0
        using Engine = ranlux24;
        Engine eng;
        assert((generate_canonical<double, 0, Engine>) (eng) == 0.0);
        assert(eng() == Engine{}());
    }

    {
        // float, URBG range is a power of two
        using Engine                = ranlux24;
        constexpr uint32_t values[] = {0xE57B2C, 0xF91555, 0xD9F2DE};
        Engine eng;
        for (size_t i = 0; i < std::size(values); ++i) {
            assert(eng() == values[i]);
        }

        constexpr float expected1 = 0.8964107f;
        constexpr float expected2 = 0.5677083f;
        constexpr float expected3 = 0.8684871f;

#ifdef __cpp_hex_float
        // independent_bits_engine keeps lower 24, 20, or 8 bits
        static_assert(expected1 == 0x0.E57B2Cp0f, "expected value mismatch");
        static_assert(expected2 == 0x0.91555'5p0f, "expected value mismatch"); // upper 24 bits of cat(91555, 57B2C)
        static_assert(expected3 == 0x0.DE'55'2Cp0f, "expected value mismatch");
#endif
        assert((generate_with_ibe<float, 24, Engine>) () == expected1);
        assert((generate_with_ibe<float, 20, Engine>) () == expected2); // needs a 64 bit accumulator for $S$
        assert((generate_with_ibe<float, 8, Engine>) () == expected3);
    }

    {
        // double, URBG range is a power of two
        using Engine                = mt19937_64;
        constexpr uint64_t values[] = {0xC96D191CF6F6AEA6, 0x401F7AC78BC80F1C, 0xB5EE8CB6ABE457F8};
        Engine eng;
        for (size_t i = 0; i < std::size(values); ++i) {
            assert(eng() == values[i]);
        }

        constexpr double expected1 = 0.7868209548678019;
        constexpr double expected2 = 0.5460214086260416;
        constexpr double expected3 = 0.8919673431802158;

#ifdef __cpp_hex_float
        // upper 53 bits of C96D191CF6F6AEA6
        static_assert(expected1 == 0x0.C96D191CF6F6A8p0, "expected value mismatch");
        // upper 53 bits of cat(8BC80F1C, F6F6AEA6)
        static_assert(expected2 == 0x0.8BC80F1C'F6F6A8p0, "expected value mismatch");
        // upper 53 bits of cat(E457F8, C80F1C, F6AEA6)
        static_assert(expected3 == 0x0.E457F8'C80F1C'F0p0, "expected value mismatch");
#endif
        assert((generate_with_ibe<double, 64, Engine>) () == expected1);
        assert((generate_with_ibe<double, 32, Engine>) () == expected2);
        assert((generate_with_ibe<double, 24, Engine>) () == expected3); // needs a 128 bit accumulator for $S$
    }

    {
        // $k \in \{1,2\}$, URBG range is NOT a power of two
        using Engine                = minstd_rand; // R = 2^31-2 = 2'147'483'646, minstd_rand::min() == 1
        constexpr uint32_t values[] = {48270 + 1, 182605793 + 1};
        Engine eng;
        for (size_t i = 0; i < std::size(values); ++i) {
            assert(eng() == values[i]);
        }

        constexpr float expected1  = 2.2649765e-5f;
        constexpr double expected2 = 0.08519885256797011;

#ifdef __cpp_hex_float
        static_assert(expected1 == 0x0.00017Cp0f, "expected value mismatch"); // 48270 / 127 = 380 = 0x17C

        // 392,142,954,132,409,548 / 511 = 0x0002B9F2F1ADF500
        // p=3 to get bottom 53 bits of a 56-bit constant
        static_assert(expected2 == 0x0.02B9F2F1ADF500p3, "expected value mismatch");
#endif

        // $k$ == 1
        eng.seed();
        assert((generate_canonical<float, 32, Engine>) (eng) == expected1);

        // $k$ == 2
        eng.seed();
        assert((generate_canonical<double, 64, Engine>) (eng) == expected2);
    }

    {
        // $k = 4, URBG range is NOT a power of two, 128-bit accumulator needed

        // ZX81 generator, R = 65537, https://oeis.org/A357907
        using Engine                = linear_congruential_engine<uint32_t, 75, 74, 65537>;
        constexpr uint32_t values[] = {149, 11249, 57305, 38044};
        Engine eng;
        for (size_t i = 0; i < std::size(values); ++i) {
            assert(eng() == values[i]);
        }

        constexpr double expected = 0.5805452877334312;

#ifdef __cpp_hex_float
        // 10,709,170,346,016,678,139 / 2,048 = 0x001293D3B6152F14
        static_assert(expected == 0x0.1293D3B6152F14p3, "expected value mismatch");
#endif

        eng.seed();
        assert((generate_canonical<double, 64, Engine>) (eng) == expected);
    }

    return 0;
}
