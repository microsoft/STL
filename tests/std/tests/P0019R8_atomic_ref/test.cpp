// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <execution>
#include <vector>

// code reuse of ../P1135R6_atomic_flag_test/test.cpp

template <typename ValueType>
void test_ops() {
    constexpr std::size_t unique      = 80; // small to avoid overflow even for char
    constexpr std::size_t repetitions = 8000;
    constexpr std::size_t total       = unique * repetitions;
    constexpr std::size_t range       = 10;

    ValueType vals[unique] = {};
    std::vector<std::atomic_ref<ValueType>> refs;
    refs.reserve(total);
    for (std::size_t i = 0; i != repetitions; ++i) {
        for (auto& val : vals) {
            refs.push_back(std::atomic_ref<ValueType>(val));
        }
    }

    using std::execution::par;

    auto load = [](const std::atomic_ref<ValueType>& ref) { return static_cast<int>(ref.load()); };
    auto add  = [](std::atomic_ref<ValueType>& ref) { return static_cast<int>(ref.fetch_add(1)); };

    assert(std::transform_reduce(par, refs.begin(), refs.end(), 0, std::plus{}, load) == 0);
    assert(std::transform_reduce(par, refs.begin(), refs.begin() + range, 0, std::plus{}, add) == 0);
    assert(std::transform_reduce(par, refs.begin(), refs.end(), 0, std::plus{}, load) == range * repetitions);
    assert(std::transform_reduce(par, refs.begin(), refs.begin() + range, 0, std::plus{}, add) == range);
    assert(std::transform_reduce(par, refs.begin(), refs.end(), 0, std::plus{}, load) == range * repetitions * 2);
}


int main() {
    test_ops<char>();
    test_ops<signed char>();
    test_ops<unsigned char>();
    test_ops<short>();
    test_ops<unsigned short>();
    test_ops<int>();
    test_ops<unsigned int>();
    test_ops<long>();
    test_ops<unsigned long>();
    test_ops<long long>();
    test_ops<unsigned long long>();
}
