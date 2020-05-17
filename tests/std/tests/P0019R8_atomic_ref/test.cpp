// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <execution>
#include <vector>

struct bigint {
    int value;
    int more_value[10];

    bigint(int value = 0) : value(value) {}

    operator int() const {
        return value;
    }
};

// code reuse of ../P1135R6_atomic_flag_test/test.cpp

template <bool AddViaCas, typename ValueType>
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

    auto load  = [](const std::atomic_ref<ValueType>& ref) { return static_cast<int>(ref.load()); };
    auto xchg0 = [](std::atomic_ref<ValueType>& ref) { return static_cast<int>(ref.exchange(0)); };

    int (*inc)(std::atomic_ref<ValueType> & ref);
    if constexpr (AddViaCas) {
        inc = [](std::atomic_ref<ValueType>& ref) {
            for (;;) {
                ValueType e = ref.load();
                ValueType d = static_cast<ValueType>(static_cast<int>(e) + 1);
                if (ref.compare_exchange_weak(e, d)) {
                    return static_cast<int>(e);
                }
            }
        };
    } else {
        inc = [](std::atomic_ref<ValueType>& ref) { return static_cast<int>(ref.fetch_add(1)); };
    }

    assert(std::transform_reduce(par, refs.begin(), refs.end(), 0, std::plus{}, load) == 0);
    assert(std::transform_reduce(par, refs.begin(), refs.begin() + range, 0, std::plus{}, inc) == 0);
    assert(std::transform_reduce(par, refs.begin(), refs.end(), 0, std::plus{}, load) == range * repetitions);
    assert(std::transform_reduce(par, refs.begin(), refs.begin() + range, 0, std::plus{}, inc) == range);
    assert(std::transform_reduce(par, refs.begin(), refs.end(), 0, std::plus{}, load) == range * repetitions * 2);
    assert(std::transform_reduce(par, refs.begin(), refs.end(), 0, std::plus{}, xchg0) == range * 2);
    assert(std::transform_reduce(par, refs.begin(), refs.end(), 0, std::plus{}, load) == 0);
}

int main() {
    test_ops<false, char>();
    test_ops<false, signed char>();
    test_ops<false, unsigned char>();
    test_ops<false, short>();
    test_ops<false, unsigned short>();
    test_ops<false, int>();
    test_ops<false, unsigned int>();
    test_ops<false, long>();
    test_ops<false, unsigned long>();
    test_ops<false, long long>();
    test_ops<false, unsigned long long>();
    test_ops<false, float>();
    test_ops<false, double>();
    test_ops<false, long double>();

    test_ops<true, char>();
    test_ops<true, signed char>();
    test_ops<true, unsigned char>();
    test_ops<true, short>();
    test_ops<true, unsigned short>();
    test_ops<true, int>();
    test_ops<true, unsigned int>();
    test_ops<true, long>();
    test_ops<true, unsigned long>();
    test_ops<true, long long>();
    test_ops<true, unsigned long long>();
    test_ops<true, float>();
    test_ops<true, double>();
    test_ops<true, long double>();
    test_ops<true, bigint>();
}
