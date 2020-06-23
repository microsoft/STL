// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <range_algorithm_support.hpp>
#include <ranges>
#include <utility>

constexpr void smoke_test() {
    using ranges::generate, ranges::iterator_t;
    using std::same_as;

    {
        int output[]    = {13, 42, 1367};
        const int value = 7;
        auto result     = generate(std::begin(output), std::end(output), []() { return value; });
        for (auto elem : output) {
            assert(elem == value);
        }
        assert(result == std::end(output));
    }
    {
        int output[]    = {13, 42, 1367};
        const int value = 13;
        auto result     = generate(output, []() { return value; });
        for (auto elem : output) {
            assert(elem == value);
        }
        assert(result == std::end(output));
    }
    {
        int output[]          = {13, 42, 1367};
        int calls_to_generate = -1;
        auto result           = generate(output, [&calls_to_generate]() {
            calls_to_generate++;
            return calls_to_generate;
        });
        for (int i = 0; i < 3; i++) {
            assert(i == output[i]);
        }
        assert(result == std::end(output));
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class Out>
    static void call(Out&& out = {}) {
        (void) ranges::generate(out, []() { return 42; });
        (void) ranges::generate(ranges::begin(out), ranges::end(out), []() { return 42; });
    }
};

template void test_out<instantiator>();
