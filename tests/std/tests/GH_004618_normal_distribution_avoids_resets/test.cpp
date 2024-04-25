// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <random>

class FakeGenerator {
private:
    std::mt19937 m_underlying_gen;
    std::size_t m_operator_calls = 0;

public:
    using GenType     = std::mt19937;
    using result_type = GenType::result_type;

    static constexpr result_type min() {
        return GenType::min();
    }
    static constexpr result_type max() {
        return GenType::max();
    }

    result_type operator()() {
        ++m_operator_calls;
        return m_underlying_gen();
    }

    std::size_t calls() const {
        return m_operator_calls;
    }
};

int main() {
    FakeGenerator rng;
    std::normal_distribution<> dist(0.0, 1.0);
    using dist_params = std::normal_distribution<>::param_type;
    dist_params params(50.0, 0.5);
    (void) dist(rng);
    const auto calls_before = rng.calls();
    (void) dist(rng);
    const auto calls_after = rng.calls();
    assert(calls_before == calls_after);
}
