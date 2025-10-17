// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <__msvc_int128.hpp>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <random>

using namespace std;

void test_lwg2524() {
    mt19937_64 mt2(1);
    mt2.discard(517517);
    assert((generate_canonical<float, 32>) (mt2) < 1.0f);
    assert((generate_canonical<float, 32>) (mt2) < 1.0f);
    assert((generate_canonical<float, 32>) (mt2) < 1.0f);
}

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
        for (const auto& value : values) {
            assert(eng() == value);
        }

        const auto expected1 = ldexpf(0xE57B2C >> (1 * 24 - 24), -24);
        const auto expected2 = ldexpf(0x91555'57B2C >> (2 * 20 - 24), -24);
        const auto expected3 = ldexpf(0xDE'55'2C >> (3 * 8 - 24), -24);

        assert((generate_with_ibe<float, 24, Engine>) () == expected1);
        assert((generate_with_ibe<float, 20, Engine>) () == expected2);
        assert((generate_with_ibe<float, 8, Engine>) () == expected3);
    }

    {
        // double, URBG range is a power of two
        using Engine                = mt19937_64;
        constexpr uint64_t values[] = {0xC96D191C'F6'F6AEA6, 0x401F7AC7'8B'C80F1C, 0xB5EE8CB6AB'E457F8};
        Engine eng;
        for (const auto& value : values) {
            assert(eng() == value);
        }

        const auto expected1 = ldexp(0xC96D191C'F6F'6AEA6 >> (1 * 64 - 53), -53);
        const auto expected2 = ldexp(0x8B'C80F1C'F6'F6AEA6 >> (2 * 32 - 53), -53);
        const auto expected3 =
            ldexp(static_cast<uint64_t>(_Unsigned128{0x57F8'C80F1C'F6AEA6, 0xE4} >> (3 * 24 - 53)), -53);


        assert((generate_with_ibe<double, 64, Engine>) () == expected1);
        assert((generate_with_ibe<double, 32, Engine>) () == expected2);
        assert((generate_with_ibe<double, 24, Engine>) () == expected3);
    }

    {
        // $k \in \{1,2\}$, URBG range is NOT a power of two
        using Engine                = minstd_rand;
        constexpr uint32_t values[] = {48271 - 1, 182605794 - 1}; // minstd_rand::min() == 1
        Engine eng;
        for (const auto& value : values) {
            assert(eng() - Engine::min() == value);
        }

        constexpr uint64_t range = Engine::max() - Engine::min() + 1;
        constexpr auto x1        = range / (1 << 24);
        constexpr auto x2        = (range * range) / (1ULL << 53);
        const float expected1    = ldexpf(static_cast<float>(values[0] / x1), -24);
        const double expected2   = ldexp((values[0] + range * values[1]) / x2, -53);

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
        for (const auto& value : values) {
            assert(eng() - Engine::min() == value);
        }

        constexpr _Unsigned128 range = Engine::max() - Engine::min() + 1;
        constexpr auto x             = static_cast<uint64_t>((range * range * range * range) / (1ULL << 53));
        const auto expected          = ldexp(
            static_cast<uint64_t>((values[0] + range * (values[1] + range * (values[2] + range * values[3]))) / x),
            -53);

        eng.seed();
        assert((generate_canonical<double, 64, Engine>) (eng) == expected);
    }

    test_lwg2524();

    return 0;
}
