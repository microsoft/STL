// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>

#include <range_algorithm_support.hpp>

using namespace std;

struct instantiator {
    template <ranges::output_range<const int&> Out>
    static constexpr void call() {
        using ranges::generate, ranges::iterator_t;

        const auto iota_gen = [count = 0]() mutable { return count++; };

        {
            int output[] = {13, 42, 1367};
            Out out_wrapper{output};
            auto result = generate(out_wrapper, iota_gen);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<Out>>);
            assert(result == out_wrapper.end());
            for (int i = 0; i < 3; ++i) {
                assert(i == output[i]);
            }
        }
        {
            int output[] = {13, 42, 1367};
            Out out_wrapper{output};
            auto result = generate(out_wrapper.begin(), out_wrapper.end(), iota_gen);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<Out>>);
            assert(result == out_wrapper.end());
            for (int i = 0; i < 3; ++i) {
                assert(i == output[i]);
            }
        }
    }
};

int main() {
    STATIC_ASSERT((test_out<instantiator, int>(), true));
    test_out<instantiator, int>();
}
