// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <benchmark/benchmark.h>
#include <bitset>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <random>

using namespace std;

namespace {
    template <typename charT, size_t Min_length, size_t N>
    const auto random_digits_init() {
        mt19937_64 rnd{};
        uniform_int_distribution<> dis('0', '1');
        std::basic_string<charT> str;

        constexpr size_t number_of_bitsets = (Min_length + N - 1) / N;
        static_assert(number_of_bitsets != 0);

        constexpr size_t actual_size = number_of_bitsets * (N + 1); // +1 for \0

        std::array<charT, actual_size> result;

        for (auto dest = result.begin(); dest != result.end();) {
            for (size_t i = 0; i != N; ++i, ++dest) {
                *dest = static_cast<charT>(dis(rnd));
            }

            *dest = charT{'\0'};
            ++dest;
        }

        return result;
    }

    enum class length_type : bool { char_count, null_term };

    template <size_t N, typename charT>
    const auto random_digits = random_digits_init<charT, 2048, N>();

    template <length_type Length, size_t N, class charT>
    void BM_bitset_from_string(benchmark::State& state) {
        const auto& bit_string = random_digits<N, charT>;
        for (auto _ : state) {
            benchmark::DoNotOptimize(bit_string);
            const charT* const data = bit_string.data();
            for (size_t pos = 0, max = bit_string.size(); pos != max; pos += N + 1) {
                if constexpr (Length == length_type::char_count) {
                    bitset<N> bs(data + pos, N);
                    benchmark::DoNotOptimize(bs);
                } else {
                    bitset<N> bs(data + pos);
                    benchmark::DoNotOptimize(bs);
                }
            }
        }
    }
} // namespace

BENCHMARK(BM_bitset_from_string<length_type::char_count, 15, char>);
BENCHMARK(BM_bitset_from_string<length_type::char_count, 16, char>);
BENCHMARK(BM_bitset_from_string<length_type::char_count, 36, char>);
BENCHMARK(BM_bitset_from_string<length_type::char_count, 64, char>);
BENCHMARK(BM_bitset_from_string<length_type::char_count, 512, char>);
BENCHMARK(BM_bitset_from_string<length_type::char_count, 2048, char>);

BENCHMARK(BM_bitset_from_string<length_type::char_count, 15, wchar_t>);
BENCHMARK(BM_bitset_from_string<length_type::char_count, 16, wchar_t>);
BENCHMARK(BM_bitset_from_string<length_type::char_count, 36, wchar_t>);
BENCHMARK(BM_bitset_from_string<length_type::char_count, 64, wchar_t>);
BENCHMARK(BM_bitset_from_string<length_type::char_count, 512, wchar_t>);
BENCHMARK(BM_bitset_from_string<length_type::char_count, 2048, wchar_t>);

BENCHMARK(BM_bitset_from_string<length_type::null_term, 15, char>);
BENCHMARK(BM_bitset_from_string<length_type::null_term, 16, char>);
BENCHMARK(BM_bitset_from_string<length_type::null_term, 36, char>);
BENCHMARK(BM_bitset_from_string<length_type::null_term, 64, char>);
BENCHMARK(BM_bitset_from_string<length_type::null_term, 512, char>);
BENCHMARK(BM_bitset_from_string<length_type::null_term, 2048, char>);

BENCHMARK(BM_bitset_from_string<length_type::null_term, 15, wchar_t>);
BENCHMARK(BM_bitset_from_string<length_type::null_term, 16, wchar_t>);
BENCHMARK(BM_bitset_from_string<length_type::null_term, 36, wchar_t>);
BENCHMARK(BM_bitset_from_string<length_type::null_term, 64, wchar_t>);
BENCHMARK(BM_bitset_from_string<length_type::null_term, 512, wchar_t>);
BENCHMARK(BM_bitset_from_string<length_type::null_term, 2048, wchar_t>);

BENCHMARK_MAIN();
