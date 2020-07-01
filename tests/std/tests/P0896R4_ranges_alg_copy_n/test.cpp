// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using ranges::copy_n, ranges::copy_n_result, ranges::iterator_t;
using std::same_as;

// Validate that copy_n_result aliases in_out_result
STATIC_ASSERT(same_as<copy_n_result<int, double>, ranges::in_out_result<int, double>>);

struct instantiator {
    static constexpr int input[3] = {13, 42, 1729};
    template <class In, class, class Write>
    static constexpr void call() {
        int output[3] = {-1, -1, -1};
        auto result   = copy_n(In{input}, ranges::distance(input), Write{output});
        STATIC_ASSERT(same_as<decltype(result), copy_n_result<In, Write>>);
        assert(result.in.base() == input + 3);
        if constexpr (std::equality_comparable<Write>) {
            assert(result.out == Write{output + 3});
        }
        assert(ranges::equal(output, input));
    }
};

int main() {
    STATIC_ASSERT((test_counted_write<instantiator>(), true));
    test_counted_write<instantiator>();
}
