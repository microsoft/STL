// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <benchmark/benchmark.h>
#include <bitset>
#include <cstddef>
#include <random>
#include <sstream>
#include <string>

using namespace std;

template <size_t N, class charT, size_t Min_length>
auto random_digits_init() {
    mt19937_64 rnd{};
    uniform_int_distribution<> dis('0', '1');

    constexpr size_t number_of_bitsets = (Min_length + N - 1) / N;
    static_assert(number_of_bitsets != 0);

    constexpr size_t actual_size = number_of_bitsets * (N + 1); // +1 for \0

    array<charT, actual_size> result;

    for (size_t i = 0; i < actual_size; ++i) {
        if (i % (N + 1) == N) {
            result[i] = charT{'\0'}; // write null terminators
        } else {
            result[i] = static_cast<charT>(dis(rnd)); // fill random digits
        }
    }

    return result;
}

enum class length_type : bool { char_count, null_term };

template <size_t N, class charT>
const auto random_digits = random_digits_init<N, charT, 2048>();

template <length_type Length, size_t N, class charT>
void bitset_from_string(benchmark::State& state) {
    auto digit_array = random_digits<N, charT>;
    for (auto _ : state) {
        benchmark::DoNotOptimize(digit_array);
        const auto arr_data = digit_array.data();
        const auto arr_size = digit_array.size();
        for (size_t pos = 0; pos != arr_size; pos += N + 1) {
            if constexpr (Length == length_type::char_count) {
                bitset<N> bs(arr_data + pos, N);
                benchmark::DoNotOptimize(bs);
            } else {
                bitset<N> bs(arr_data + pos);
                benchmark::DoNotOptimize(bs);
            }
        }
    }
}

template <class charT, size_t Length>
basic_string<charT> random_digits_contiguous_string_init() {
    mt19937_64 rnd{};
    uniform_int_distribution<> dis('0', '1');

    basic_string<charT> result;

    result.resize_and_overwrite(Length, [&](charT* ptr, size_t) {
        generate_n(ptr, Length, [&] { return static_cast<charT>(dis(rnd)); });
        return Length;
    });

    return result;
}

template <class charT, size_t Length>
const auto random_digits_contiguous_string = random_digits_contiguous_string_init<charT, Length>();

template <size_t N, class charT>
void bitset_from_stream(benchmark::State& state) {
    constexpr size_t string_length = 2048;
    constexpr size_t count         = string_length / N;
    basic_istringstream<charT> stream(random_digits_contiguous_string<charT, string_length>);
    bitset<N> bs;
    for (auto _ : state) {
        benchmark::DoNotOptimize(stream);
        for (size_t i = 0; i != count; ++i) {
            stream >> bs;
        }
        benchmark::DoNotOptimize(bs);
        stream.seekg(0);
    }
}

BENCHMARK(bitset_from_string<length_type::char_count, 15, char>);
BENCHMARK(bitset_from_string<length_type::char_count, 16, char>);
BENCHMARK(bitset_from_string<length_type::char_count, 36, char>);
BENCHMARK(bitset_from_string<length_type::char_count, 64, char>);
BENCHMARK(bitset_from_string<length_type::char_count, 512, char>);
BENCHMARK(bitset_from_string<length_type::char_count, 2048, char>);

BENCHMARK(bitset_from_string<length_type::char_count, 15, wchar_t>);
BENCHMARK(bitset_from_string<length_type::char_count, 16, wchar_t>);
BENCHMARK(bitset_from_string<length_type::char_count, 36, wchar_t>);
BENCHMARK(bitset_from_string<length_type::char_count, 64, wchar_t>);
BENCHMARK(bitset_from_string<length_type::char_count, 512, wchar_t>);
BENCHMARK(bitset_from_string<length_type::char_count, 2048, wchar_t>);

BENCHMARK(bitset_from_string<length_type::null_term, 15, char>);
BENCHMARK(bitset_from_string<length_type::null_term, 16, char>);
BENCHMARK(bitset_from_string<length_type::null_term, 36, char>);
BENCHMARK(bitset_from_string<length_type::null_term, 64, char>);
BENCHMARK(bitset_from_string<length_type::null_term, 512, char>);
BENCHMARK(bitset_from_string<length_type::null_term, 2048, char>);

BENCHMARK(bitset_from_string<length_type::null_term, 15, wchar_t>);
BENCHMARK(bitset_from_string<length_type::null_term, 16, wchar_t>);
BENCHMARK(bitset_from_string<length_type::null_term, 36, wchar_t>);
BENCHMARK(bitset_from_string<length_type::null_term, 64, wchar_t>);
BENCHMARK(bitset_from_string<length_type::null_term, 512, wchar_t>);
BENCHMARK(bitset_from_string<length_type::null_term, 2048, wchar_t>);

BENCHMARK(bitset_from_stream<15, char>);
BENCHMARK(bitset_from_stream<16, char>);
BENCHMARK(bitset_from_stream<36, char>);
BENCHMARK(bitset_from_stream<64, char>);
BENCHMARK(bitset_from_stream<512, char>);
BENCHMARK(bitset_from_stream<2048, char>);

BENCHMARK(bitset_from_stream<15, wchar_t>);
BENCHMARK(bitset_from_stream<16, wchar_t>);
BENCHMARK(bitset_from_stream<36, wchar_t>);
BENCHMARK(bitset_from_stream<64, wchar_t>);
BENCHMARK(bitset_from_stream<512, wchar_t>);
BENCHMARK(bitset_from_stream<2048, wchar_t>);

BENCHMARK_MAIN();
