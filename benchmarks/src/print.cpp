#include <benchmark/benchmark.h>
#include <print>

namespace {
    using PrintType = void (*)(FILE*, std::string_view, std::format_args);

    template <PrintType PrintFunction>
    void BM_vprint(benchmark::State& state) {
        for (auto _ : state) {
            PrintFunction(stdout, "Hello cool unicode 😊😊😊\n", std::make_format_args());
        }
    }
    BENCHMARK(BM_vprint<&std::vprint_unicode>);
    BENCHMARK(BM_vprint<&std::vprint_unicode_buffered>);

    template <PrintType PrintFunction>
    void BM_vprint_complex(benchmark::State& state) {
        const int i           = 42;
        const std::string str = "Hello world 😊😊😊😊😊😊😊😊😊😊😊😊😊";
        const double f        = -902.16283758;
        const std::pair<int, double> p{16, 2.073f};
        for (auto _ : state) {
            PrintFunction(stdout,
                "Hello cool unicode 😊😊😊 {:X}, {}, {:a}, I am a big string, lots of lines, multiple {} formats\n",
                std::make_format_args(i, str, f, p));
        }
    }
    BENCHMARK(BM_vprint_complex<&std::vprint_unicode>);
    BENCHMARK(BM_vprint_complex<&std::vprint_unicode_buffered>);
} // namespace

BENCHMARK_MAIN();
