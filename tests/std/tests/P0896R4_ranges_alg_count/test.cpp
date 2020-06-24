// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::count;
    using P                  = std::pair<int, int>;
    std::array<P, 5> const x = {{{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}}};
    using D                  = ranges::range_difference_t<move_only_range<P const>>;

    {
        // Validate range overload
        auto result = count(move_only_range{x}, 99, get_second);
        STATIC_ASSERT(std::same_as<decltype(result), D>);
        assert(result == 3);
    }
    {
        // Validate iterator + sentinel overload
        move_only_range wrapped_x{x};
        auto result = count(wrapped_x.begin(), wrapped_x.end(), 47, get_second);
        STATIC_ASSERT(std::same_as<decltype(result), D>);
        assert(result == 2);
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In>
    static void call(In&& in = {}) {
        ranges::range_value_t<In> const value{};
        (void) ranges::count(in, value);

        struct type {
            bool operator==(type const&) const = default;
        };
        using Projection = type (*)(std::iter_common_reference_t<ranges::iterator_t<In>>);
        (void) ranges::count(in, type{}, Projection{});
    }
};

template void test_in<instantiator>();
