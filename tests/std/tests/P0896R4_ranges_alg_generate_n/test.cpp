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
        using ranges::generate_n, ranges::equal, ranges::iterator_t;

        const auto iota_gen = [val = 0]() mutable { return val++; };

        {
            int output[] = {13, 42, 1367};
            Out out_wrapper{output};
            auto result = generate_n(out_wrapper.begin(), ranges::distance(output), iota_gen);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<Out>>);
            assert(result == out_wrapper.end());
            for (int i = 0; i < 3; ++i) {
                assert(i == output[i]);
            }
        }

        constexpr int expected_output[] = {13, 42, 1367};
        int output[]                    = {13, 42, 1367};
        {
            Out out_wrapper{output};
            auto result = generate_n(out_wrapper.begin(), 0, iota_gen);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<Out>>);
            assert(result.peek() == output);
            assert(equal(output, expected_output));
        }
        {
            Out out_wrapper{output};
            auto result = generate_n(out_wrapper.begin(), -1, iota_gen);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<Out>>);
            assert(result.peek() == output);
            assert(equal(output, expected_output));
        }
    }
};

int main() {
    STATIC_ASSERT((test_out<instantiator, int>(), true));
    test_out<instantiator, int>();
}
