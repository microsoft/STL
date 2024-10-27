// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

using namespace std;

#if 1 // TRANSITION, GH-5043

template <class T, size_t Alignment, size_t Skew>
struct skewed_allocator {
    using value_type = T;

    static_assert(
        Alignment % alignof(T) == 0 && Skew % alignof(T) == 0, "Chosen parameters will produce unaligned T objects");

    template <class U>
    struct rebind {
        using type = skewed_allocator<U, Alignment, Skew>;
    };

    skewed_allocator() = default;
    template <class U>
    skewed_allocator(const skewed_allocator<U, Alignment, Skew>&) {}

    template <class U>
    bool operator==(const skewed_allocator<U, Alignment, Skew>&) const {
        return true;
    }

    T* allocate(size_t n) {
        const auto p = static_cast<unsigned char*>(_aligned_malloc(n * sizeof(T) + Skew, Alignment));
        if (!p) {
            throw std::bad_alloc{};
        }
        return reinterpret_cast<T*>(p + Skew);
    }

    void deallocate(T* p, size_t) {
        if (p) {
            _aligned_free(reinterpret_cast<unsigned char*>(p) - Skew);
        }
    }
};

// The purpose is to provide consistent behavior for benchmarks.
// 64 seems to be reasonable alignment for practical perf uses,
// as it is both cache line size and maximum vector instruction size (on x64).
// However to provide even more consistency, aligning to page,
// to make sure the same number of page boundaries is crossed each time.
constexpr size_t page_size = 4096;

// A realistic skew relative to allocation granularity, when a variable is placed
// next to a pointer in a structure or on stack. Also corresponds to the default packing.
constexpr size_t skew = 8;

template <class T>
struct not_highly_aligned_allocator : skewed_allocator<T, page_size, skew> {};

#endif // ^^^ TRANSITION, GH-5043 ^^^

enum class AlgType { std_func, str_member_first, str_member_last };

template <AlgType Alg, class T, T Start = T{'!'}>
void bm(benchmark::State& state) {
    const size_t Pos   = static_cast<size_t>(state.range(0));
    const size_t NSize = static_cast<size_t>(state.range(1));
    const size_t HSize = Pos * 2;
    const size_t Which = 0;

    using container = conditional_t<Alg == AlgType::std_func, vector<T, not_highly_aligned_allocator<T>>,
        basic_string<T, char_traits<T>, not_highly_aligned_allocator<T>>>;

    constexpr T HaystackFiller{' '};
    static_assert(HaystackFiller < Start, "The following iota() should not produce the haystack filler.");

    container h(HSize, HaystackFiller);
    container n(NSize, T{0});

    if (NSize - 1 > static_cast<size_t>(numeric_limits<T>::max()) - static_cast<size_t>(Start)) {
        puts("ERROR: The following iota() would overflow.");
        abort();
    }

    iota(n.begin(), n.end(), Start);

    if (Pos >= HSize || Which >= NSize) {
        abort();
    }

    h[Pos] = n[Which];

    for (auto _ : state) {
        benchmark::DoNotOptimize(h);
        benchmark::DoNotOptimize(n);
        if constexpr (Alg == AlgType::str_member_first) {
            benchmark::DoNotOptimize(h.find_first_of(n));
        } else if constexpr (Alg == AlgType::str_member_last) {
            benchmark::DoNotOptimize(h.find_last_of(n));
        } else {
            benchmark::DoNotOptimize(find_first_of(h.begin(), h.end(), n.begin(), n.end()));
        }
    }
}

void common_args(auto bm) {
    bm->Args({2, 3})->Args({6, 81})->Args({7, 4})->Args({9, 3})->Args({22, 5})->Args({58, 2});
    bm->Args({75, 85})->Args({102, 4})->Args({200, 46})->Args({325, 1})->Args({400, 50});
    bm->Args({1011, 11})->Args({1280, 46})->Args({1502, 23})->Args({2203, 54})->Args({3056, 7});
}

BENCHMARK(bm<AlgType::std_func, uint8_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::std_func, uint16_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::std_func, uint32_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::std_func, uint64_t>)->Apply(common_args);

BENCHMARK(bm<AlgType::str_member_first, char>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member_first, wchar_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member_first, wchar_t, L'\x03B1'>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member_first, char32_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member_first, char32_t, U'\x03B1'>)->Apply(common_args);

BENCHMARK(bm<AlgType::str_member_last, char>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member_last, wchar_t>)->Apply(common_args);
BENCHMARK(bm<AlgType::str_member_last, wchar_t, L'\x03B1'>)->Apply(common_args);

BENCHMARK_MAIN();
