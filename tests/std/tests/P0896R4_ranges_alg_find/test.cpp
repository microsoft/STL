// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>
//
#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::find, ranges::iterator_t, std::same_as;
    using P = std::pair<int, int>;

    // Validate dangling story
    STATIC_ASSERT(same_as<decltype(find(borrowed<false>{}, 42)), ranges::dangling>);
    STATIC_ASSERT(same_as<decltype(find(borrowed<true>{}, 42)), int*>);

    std::array<P, 3> const data = {{{0, 42}, {2, 42}, {4, 42}}};

    for (auto [value, _] : data) {
        {
            // Validate range overload [found case]
            auto result = find(move_only_range{data}, value, get_first);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<move_only_range<P const>>>);
            assert((*result).first == value);
        }
        {
            // Validate iterator+sentinel overload [found case]
            move_only_range x{data};
            auto result = find(x.begin(), x.end(), value, get_first);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<move_only_range<P const>>>);
            assert((*result).first == value);
        }
    }
    {
        // Validate range overload [not found case]
        auto result = find(move_only_range{data}, 42, get_first);
        STATIC_ASSERT(same_as<decltype(result), iterator_t<move_only_range<P const>>>);
        assert(result == move_only_range{data}.end());
    }
    {
        // Validate iterator+sentinel overload [not found case]
        move_only_range x{data};
        auto result = find(x.begin(), x.end(), 42, get_first);
        STATIC_ASSERT(same_as<decltype(result), iterator_t<move_only_range<P const>>>);
        assert(result == x.end());
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
        (void) ranges::find(in, value);

        struct type {
            bool operator==(type const&) const = default;
        };
        using Projection = type (*)(std::iter_common_reference_t<ranges::iterator_t<In>>);
        (void) ranges::find(in, type{}, Projection{});
    };
};

template void test_in<instantiator>();
