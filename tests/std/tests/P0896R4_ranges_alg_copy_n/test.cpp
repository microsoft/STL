// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

// Validate that copy_n_result aliases in_out_result
static_assert(same_as<ranges::copy_n_result<int, double>, ranges::in_out_result<int, double>>);

struct instantiator {
    static constexpr int input[3] = {13, 42, 1729};

    template <ranges::input_range Read, indirectly_writable<ranges::range_reference_t<Read>> Write>
    static constexpr void call() {
        using ranges::copy_n, ranges::copy_n_result, ranges::iterator_t;
        int output[3] = {-1, -1, -1};
        Read wrapped_input{input};

        auto result = copy_n(wrapped_input.begin(), 3, Write{output});
        static_assert(same_as<decltype(result), copy_n_result<iterator_t<Read>, Write>>);
        assert(result.in == wrapped_input.end());
        assert(result.out.peek() == output + 3);
        assert(ranges::equal(output, input));
    }
};

int main() {
    static_assert((test_in_write<instantiator, const int, int>(), true));
    test_in_write<instantiator, const int, int>();
}
