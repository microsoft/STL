// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <numeric>
#include <ranges>

#include <range_algorithm_support.hpp>

using namespace std;

// Validate that move_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::iota_result<int*, long>, ranges::out_value_result<int*, long>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::iota(borrowed<false>{}, 0L)), ranges::iota_result<ranges::dangling, long>>);
STATIC_ASSERT(same_as<decltype(ranges::iota(borrowed<true>{}, 0L)), ranges::iota_result<int*, long>>);

struct instantiator {
    template <ranges::output_range<const int&> Out>
    static constexpr void call() {
        using ranges::iterator_t, ranges::iota_result;

        {
            int output[] = {13, 42, 1367};
            Out out_wrapper{output};
            auto result = ranges::iota(out_wrapper, 0L);
            STATIC_ASSERT(same_as<decltype(result), iota_result<iterator_t<Out>, long>>);
            assert(result.out == out_wrapper.end());
            assert(result.value == 3L);
            for (int i = 0; i < 3; ++i) {
                assert(i == output[i]);
            }
        }
        {
            int output[] = {13, 42, 1367};
            Out out_wrapper{output};
            auto result = ranges::iota(out_wrapper.begin(), out_wrapper.end(), 0L);
            STATIC_ASSERT(same_as<decltype(result), iota_result<iterator_t<Out>, long>>);
            assert(result.out == out_wrapper.end());
            assert(result.value == 3L);
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
