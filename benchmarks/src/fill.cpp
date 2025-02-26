#include <benchmark/benchmark.h>
#include <vector>
#include <memory>
#include <cstring>
#include <ranges>
namespace {
    template<typename Contained, Contained Value>
    void handwritten_loop(benchmark::State& state) {
        const size_t r0 = static_cast<size_t>(state.range(0));
        std::unique_ptr<Contained[]> buffer(new Contained[r0]);
        for([[maybe_unused]] auto _ : state) {
            benchmark::DoNotOptimize(buffer.get());
            Contained* ptr = buffer.get();
            const Contained* const ptr_end = ptr + r0;
            while(ptr != ptr_end) {
                *ptr++ = Value;
            }
            benchmark::DoNotOptimize(buffer.get());
        }
    }

    template<typename Contained, Contained Value, size_t Size>
    void handwritten_loop_const_size(benchmark::State& state) {
        std::unique_ptr<Contained[]> buffer(new Contained[Size]);
        for([[maybe_unused]] auto _ : state) {
            benchmark::DoNotOptimize(buffer.get());
            Contained* ptr = buffer.get();
            const Contained* const ptr_end = ptr + Size;
            while(ptr != ptr_end) {
                *ptr++ = Value;
            }
            benchmark::DoNotOptimize(buffer.get());
        }
    }

    template<typename Contained, Contained Value>
    void handwritten_loop_n(benchmark::State& state) {
        const size_t r0 = static_cast<size_t>(state.range(0));
        std::unique_ptr<Contained[]> buffer(new Contained[r0]);
        for([[maybe_unused]] auto _ : state) {
            benchmark::DoNotOptimize(buffer.get());
            Contained* ptr = buffer.get();
            for(size_t idx = 0; idx < r0; ++idx) {
                ptr[idx] = Value;
            }
            benchmark::DoNotOptimize(buffer.get());
        }
    }
    template<typename Contained, Contained Value, size_t Size>
    void handwritten_loop_n_const_size(benchmark::State& state) {
        std::unique_ptr<Contained[]> buffer(new Contained[Size]);
        for([[maybe_unused]] auto _ : state) {
            benchmark::DoNotOptimize(buffer.get());
            Contained* ptr = buffer.get();
            for(size_t idx = 0; idx < Size; ++idx) {
                ptr[idx] = Value;
            }
            benchmark::DoNotOptimize(buffer.get());
        }
    }

    template<typename Contained, Contained Value>
    void memset_call(benchmark::State& state) {
        const size_t r0 = static_cast<size_t>(state.range(0));
        std::unique_ptr<Contained[]> buffer(new Contained[r0]);
        for([[maybe_unused]] auto _ : state) {
            benchmark::DoNotOptimize(buffer.get());
            Contained* ptr = buffer.get();
            std::memset(ptr, Value, r0);
            benchmark::DoNotOptimize(buffer.get());
        }
    }
    template<typename Contained, Contained Value>
    void std_fill_call(benchmark::State& state) {
        const size_t r0 = static_cast<size_t>(state.range(0));
        std::unique_ptr<Contained[]> buffer(new Contained[r0]);
        for([[maybe_unused]] auto _ : state) {
            benchmark::DoNotOptimize(buffer.get());
            auto begin_it = buffer.get();
            auto end_it = buffer.get() + r0;
            std::fill(begin_it, end_it, Value);
            benchmark::DoNotOptimize(buffer.get());
        }
    }

    template<typename Contained, Contained Value>
    void std_fill_n_call(benchmark::State& state) {
        const size_t r0 = static_cast<size_t>(state.range(0));
        std::unique_ptr<Contained[]> buffer(new Contained[r0]);
        for([[maybe_unused]] auto _ : state) {
            benchmark::DoNotOptimize(buffer.get());
            auto begin_it = buffer.get();
            std::fill_n(begin_it, r0, Value);
            benchmark::DoNotOptimize(buffer.get());
        }
    }
}

BENCHMARK(handwritten_loop<char, 0>)->Range(0, 1 << 18);
BENCHMARK(handwritten_loop<char, 1>)->Range(0, 1 << 18);
BENCHMARK(handwritten_loop_const_size<char, 0, 0>);
BENCHMARK(handwritten_loop_const_size<char, 0, 1>);
BENCHMARK(handwritten_loop_const_size<char, 0, 8>);
BENCHMARK(handwritten_loop_const_size<char, 0, 64>);
BENCHMARK(handwritten_loop_const_size<char, 0, 512>);
BENCHMARK(handwritten_loop_const_size<char, 0, 1000>);
BENCHMARK(handwritten_loop_const_size<char, 0, 4096>);
BENCHMARK(handwritten_loop_const_size<char, 0, 32768>);
BENCHMARK(handwritten_loop_const_size<char, 0, 262144>);
BENCHMARK(handwritten_loop_n_const_size<char, 0, 0>);
BENCHMARK(handwritten_loop_n_const_size<char, 0, 1>);
BENCHMARK(handwritten_loop_n_const_size<char, 0, 8>);
BENCHMARK(handwritten_loop_n_const_size<char, 0, 64>);
BENCHMARK(handwritten_loop_n_const_size<char, 0, 512>);
BENCHMARK(handwritten_loop_n_const_size<char, 0, 1000>);
BENCHMARK(handwritten_loop_n_const_size<char, 0, 4096>);
BENCHMARK(handwritten_loop_n_const_size<char, 0, 32768>);
BENCHMARK(handwritten_loop_n_const_size<char, 0, 262144>);
BENCHMARK(handwritten_loop_n<char, 0>)->Range(0, 1 << 18);
BENCHMARK(handwritten_loop_n<char, 1>)->Range(0, 1 << 18);
BENCHMARK(memset_call<char, 0>)->Range(0, 1 << 18);
BENCHMARK(memset_call<char, 1>)->Range(0, 1 << 18);
BENCHMARK(std_fill_call<char, 0>)->Range(0, 1 << 18);
BENCHMARK(std_fill_n_call<char, 0>)->Range(0, 1 << 18);


BENCHMARK_MAIN();