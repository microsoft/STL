// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <ranges>
#include <tuple>

#include <range_algorithm_support.hpp>

template <typename... RangeTypes>
concept CanViewZip = requires(RangeTypes&&... range) {
    std::views::zip(std::forward<RangeTypes>(range)...);
};

template <std::ranges::input_range... RangeType>
constexpr bool test_many(RangeType...&& ranges)
{
    using std::ranges::zip_view;

    constexpr bool is_view =
        ((std::ranges::view < std::remove_cvref_t<RangeTypes> && ...) && sizeof...(RangeTypes) > 0);

    template <typename RangeType>
    using AllView = std::views::all_t<RangeType>;

    using VTuple = std::tuple<AllView<RangeType>...>;
    using R = zip_view<VTuple...>;

    STATIC_ASSERT(std::ranges::view<R>);
    STATIC_ASSERT(std::ranges::input_range<R>);
    STATIC_ASSERT(std::ranges::forward_range<R> == (std::ranges::forward_range<AllView<RangeTypes>> && ...));
    STATIC_ASSERT(std::ranges::bidirectional_range<R> == (std::ranges::bidirectional_range<AllView<RangeTypes>> && ...));
    STATIC_ASSERT(std::ranges::random_access_range<R> == (std::ranges::random_access_range<AllView<RangeTypes>> && ...));

    // Validate conditional default-initializability
    STATIC_ASSERT(std::is_default_constructible_v<R> == (std::is_default_constructible_v<AllView<RangeTypes>> && ...));

    // Validate conditional borrowed_range
    STATIC_ASSERT(std::ranges::borrowed_range<R> == (std::ranges::borrowed_range<AllView<RangeTypes>> && ...));


}


int main()
{

}