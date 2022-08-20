// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <ranges>
#include <span>
#include <tuple>

#include <range_algorithm_support.hpp>

template <class... RangeTypes>
concept CanViewZip = requires(RangeTypes&&... range) {
    std::views::zip(std::forward<RangeTypes>(range)...);
};

template <class RangeType>
using AllView = std::views::all_t<RangeType>;

template <class ElementType>
concept CanTestElementType =
    (std::copy_constructible<ElementType> && std::equality_comparable<ElementType>); // Required for iter_swap test

template <class Type1, std::size_t Type1Size, class Type2, std::size_t Type2Size, class Type3, std::size_t Type3Size>
    requires(CanTestElementType<Type1>&& CanTestElementType<Type2>&& CanTestElementType<Type3>)
class three_element_test_container {
private:
    std::array<Type1, Type1Size> type_one_array;
    std::array<Type2, Type2Size> type_two_array;
    std::array<Type3, Type3Size> type_three_array;

public:
    using element_tuple_type       = std::tuple<Type1, Type2, Type3>;
    using const_element_tuple_type = std::tuple<const Type1, const Type2, const Type3>;

    using reference_tuple_type              = std::tuple<Type1&, Type2&, Type3&>;
    using const_reference_tuple_type        = std::tuple<const Type1&, const Type2&, const Type3&>;
    using rvalue_reference_tuple_type       = std::tuple<Type1&&, Type2&&, Type3&&>;
    using const_rvalue_reference_tuple_type = std::tuple<const Type1&&, const Type2&&, const Type3&&>;

    static constexpr std::size_t smallest_array_size = (std::min)(Type1Size, (std::min)(Type2Size, Type3Size));

    constexpr three_element_test_container(const std::array<Type1, Type1Size> type_one_init_arr,
        const std::array<Type2, Type2Size> type_two_init_arr, const std::array<Type3, Type3Size> type_three_init_arr)
        : type_one_array(type_one_init_arr), type_two_array(type_two_init_arr), type_three_array(type_three_init_arr) {}

    template <std::size_t ElementIndex>
    constexpr auto get_element_span() {
        STATIC_ASSERT(ElementIndex < 3);

        if constexpr (ElementIndex == 0)
            return std::span<Type1, Type1Size>{type_one_array};

        else if constexpr (ElementIndex == 1)
            return std::span<Type2, Type2Size>{type_two_array};

        else
            return std::span<Type3, Type3Size>{type_three_array};
    }

    template <std::size_t ElementIndex>
    constexpr auto get_element_span() const {
        STATIC_ASSERT(ElementIndex < 3);

        if constexpr (ElementIndex == 0)
            return std::span<std::add_const_t<Type1>, Type1Size>{type_one_array};

        else if constexpr (ElementIndex == 1)
            return std::span<std::add_const_t<Type2>, Type2Size>{type_two_array};

        else
            return std::span<std::add_const_t<Type3>, Type3Size>{type_three_array};
    }

    template <std::size_t ElementIndex>
    constexpr auto& get_underlying_element_array() {
        STATIC_ASSERT(ElementIndex < 3);

        if constexpr (ElementIndex == 0)
            return type_one_array;

        else if constexpr (ElementIndex == 1)
            return type_two_array;

        else
            return type_three_array;
    }

    template <std::size_t ElementIndex>
    constexpr const auto& get_underlying_element_array() const {
        STATIC_ASSERT(ElementIndex < 3);

        if constexpr (ElementIndex == 0)
            return type_one_array;

        else if constexpr (ElementIndex == 1)
            return type_two_array;

        else
            return type_three_array;
    }

    constexpr reference_tuple_type get_expected_element_tuple(const std::size_t index) {
        assert(index < smallest_array_size);
        return reference_tuple_type{type_one_array[index], type_two_array[index], type_three_array[index]};
    }

    constexpr const_reference_tuple_type get_expected_element_tuple(const std::size_t index) const {
        assert(index < smallest_array_size);
        return const_reference_tuple_type{type_one_array[index], type_two_array[index], type_three_array[index]};
    }

    constexpr std::array<reference_tuple_type, smallest_array_size> get_element_tuple_arr() {
        const auto make_tuple_arr_lambda = [this]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            return std::array{get_expected_element_tuple(Indices)...};
        };

        return make_tuple_arr_lambda(std::make_index_sequence<smallest_array_size>{});
    }

    constexpr std::array<const_reference_tuple_type, smallest_array_size> get_element_tuple_arr() const {
        const auto make_tuple_arr_lambda = [this]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            return std::array{get_expected_element_tuple(Indices)...};
        };

        return make_tuple_arr_lambda(std::make_index_sequence<smallest_array_size>{});
    }
};

template <class Type, std::size_t Size>
    requires(CanTestElementType<Type>)
class single_element_test_container {
private:
    std::array<Type, Size> element_array;

public:
    using element_tuple_type       = std::tuple<Type>;
    using const_element_tuple_type = std::tuple<const Type>;

    using reference_tuple_type              = std::tuple<Type&>;
    using const_reference_tuple_type        = std::tuple<const Type&>;
    using rvalue_reference_tuple_type       = std::tuple<Type&&>;
    using const_rvalue_reference_tuple_type = std::tuple<const Type&&>;

    static constexpr std::size_t smallest_array_size = Size;

    constexpr single_element_test_container(const std::array<Type, Size> element_init_arr)
        : element_array(element_init_arr) {}

    constexpr auto get_element_span() {
        return std::span<Type>{element_array};
    }

    constexpr auto get_element_span() const {
        return std::span<std::add_const_t<Type>>{element_array};
    }

    constexpr auto& get_underlying_element_array() {
        return element_array;
    }

    constexpr const auto& get_underlying_element_array() const {
        return element_array;
    }

    constexpr reference_tuple_type get_expected_element_tuple(const std::size_t index) {
        assert(index < smallest_array_size);
        return reference_tuple_type{element_array[index]};
    }

    constexpr const_reference_tuple_type get_expected_element_tuple(const std::size_t index) const {
        assert(index < smallest_array_size);
        return const_reference_tuple_type{element_array[index]};
    }

    constexpr std::array<reference_tuple_type, smallest_array_size> get_element_tuple_arr() {
        const auto make_tuple_arr_lambda = [this]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            return std::array{get_expected_element_tuple(Indices)...};
        };

        return make_tuple_arr_lambda(std::make_index_sequence<smallest_array_size>{});
    }

    constexpr std::array<const_reference_tuple_type, smallest_array_size> get_element_tuple_arr() const {
        const auto make_tuple_arr_lambda = [this]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            return std::array{get_expected_element_tuple(Indices)...};
        };

        return make_tuple_arr_lambda(std::make_index_sequence<smallest_array_size>{});
    }
};

// NOTE: std::views::zip shouldn't care about whether or not the views use proxy references,
// but for our tests, we need to.
template <class T>
struct reference_type_solver {
    using reference_type = const T&;
};

template <class Category, class Element>
struct reference_type_solver<test::proxy_reference<Category, Element>> {
    using reference_type = const Element&;
};

template <class LHSTupleType, class RHSTupleType>
constexpr bool do_tuples_reference_same_objects(const LHSTupleType& lhs_tuple, const RHSTupleType& rhs_tuple) {
    STATIC_ASSERT(std::tuple_size_v<LHSTupleType> == std::tuple_size_v<RHSTupleType>);

    const auto evaluate_single_element_lambda = [&lhs_tuple, &rhs_tuple]<std::size_t CurrIndex>() {
        using reference_type =
            typename reference_type_solver<std::tuple_element_t<CurrIndex, LHSTupleType>>::reference_type;
        return (std::addressof(static_cast<reference_type>(std::get<CurrIndex>(lhs_tuple)))
                == std::addressof(static_cast<reference_type>(std::get<CurrIndex>(rhs_tuple))));
    };

    using index_sequence_type = std::make_index_sequence<std::tuple_size_v<LHSTupleType>>;
    const auto evaluate_tuples_lambda =
        [&evaluate_single_element_lambda]<std::size_t... Indices>(std::index_sequence<Indices...>) {
        return (evaluate_single_element_lambda.template operator()<Indices>() && ...);
    };

    return evaluate_tuples_lambda(index_sequence_type{});
}

template <class TestContainerType, std::ranges::input_range... RangeTypes>
constexpr bool test_one(TestContainerType& test_container, RangeTypes&&... ranges) {
    // Ignore instances where one of the generated test ranges does not model
    // std::ranges::viewable_range.
    if constexpr ((std::ranges::viewable_range<decltype((ranges))> && ...)) {
        using ZipType = std::ranges::zip_view<AllView<decltype((ranges))>...>;

        constexpr bool are_views =
            (std::ranges::view<std::remove_cvref_t<decltype((ranges))>> && ...) && (sizeof...(RangeTypes) > 0);

        STATIC_ASSERT(std::ranges::view<ZipType>);
        STATIC_ASSERT(std::ranges::input_range<ZipType>);
        STATIC_ASSERT(std::ranges::forward_range<ZipType> == (std::ranges::forward_range<decltype((ranges))> && ...));
        STATIC_ASSERT(
            std::ranges::bidirectional_range<ZipType> == (std::ranges::bidirectional_range<decltype((ranges))> && ...));
        STATIC_ASSERT(
            std::ranges::random_access_range<ZipType> == (std::ranges::random_access_range<decltype((ranges))> && ...));
        STATIC_ASSERT(
            std::ranges::common_range<
                ZipType> == (sizeof...(RangeTypes) == 1 && (std::ranges::common_range<decltype((ranges))> && ...))
            || (!(std::ranges::bidirectional_range<decltype((ranges))> && ...)
                && (std::ranges::common_range<decltype((ranges))> && ...))
            || ((std::ranges::random_access_range<decltype((ranges))> && ...)
                && (std::ranges::sized_range<decltype((ranges))> && ...)));

        // Validate conditional default-initializability
        STATIC_ASSERT(
            std::is_default_constructible_v<ZipType> == (std::is_default_constructible_v<AllView<RangeTypes>> && ...));

        // Validate conditional borrowed_range
        STATIC_ASSERT(
            std::ranges::borrowed_range<ZipType> == (std::ranges::borrowed_range<AllView<RangeTypes>> && ...));

        // Validate range adaptor object

        // ... with lvalue arguments
        STATIC_ASSERT(
            CanViewZip<RangeTypes&...> == (!are_views || (std::copy_constructible<AllView<RangeTypes>> && ...)));
        if constexpr (CanViewZip<RangeTypes&...>) {
            using ExpectedZipType      = ZipType;
            constexpr bool is_noexcept = (std::is_nothrow_copy_constructible_v<AllView<RangeTypes>> && ...);

            STATIC_ASSERT(std::same_as<decltype(std::views::zip(ranges...)), ExpectedZipType>);
            STATIC_ASSERT(noexcept(std::views::zip(ranges...)) == is_noexcept);
        }

        // ... with const lvalue arguments
        STATIC_ASSERT(CanViewZip<const std::remove_reference_t<
                          RangeTypes>&...> == (!are_views || (std::copy_constructible<AllView<RangeTypes>> && ...)));
        if constexpr (CanViewZip<const std::remove_reference_t<RangeTypes>&...>) {
            using ExpectedZipType      = std::ranges::zip_view<AllView<const std::remove_reference_t<RangeTypes>&>...>;
            constexpr bool is_noexcept = (std::is_nothrow_copy_constructible_v<AllView<RangeTypes>> && ...);

            STATIC_ASSERT(std::same_as<decltype(std::views::zip(std::as_const(ranges)...)), ExpectedZipType>);
            STATIC_ASSERT(noexcept(std::views::zip(std::as_const(ranges)...)) == is_noexcept);
        }

        // ... with rvalue argument
        STATIC_ASSERT(CanViewZip<std::remove_reference_t<
                          RangeTypes>...> == (are_views || (std::movable<std::remove_reference_t<RangeTypes>> && ...)));
        if constexpr (CanViewZip<std::remove_reference_t<RangeTypes>...>) {
            using ExpectedZipType      = std::ranges::zip_view<AllView<std::remove_reference_t<RangeTypes>>...>;
            constexpr bool is_noexcept = (std::is_nothrow_move_constructible_v<AllView<RangeTypes>> && ...);

            STATIC_ASSERT(std::same_as<decltype(std::views::zip(std::move(ranges)...)), ExpectedZipType>);
            STATIC_ASSERT(noexcept(std::views::zip(std::move(ranges)...)) == is_noexcept);
        }

        // ... with const rvalue argument
        STATIC_ASSERT(CanViewZip<const std::remove_reference_t<
                          RangeTypes>...> == (are_views && (std::copy_constructible<AllView<RangeTypes>> && ...)));
        if constexpr (CanViewZip<const std::remove_reference_t<RangeTypes>...>) {
            using ExpectedZipType      = std::ranges::zip_view<AllView<const std::remove_reference_t<RangeTypes>>...>;
            constexpr bool is_noexcept = (std::is_nothrow_copy_constructible_v<AllView<RangeTypes>> && ...);

            STATIC_ASSERT(
                std::same_as<decltype(std::views::zip(std::move(std::as_const(ranges))...)), ExpectedZipType>);
            STATIC_ASSERT(noexcept(std::views::zip(std::move(std::as_const(ranges))...)) == is_noexcept);
        }

        // Validate deduction guide
        std::same_as<ZipType> auto zipped_range = std::ranges::zip_view{std::forward<RangeTypes>(ranges)...};
        const auto tuple_element_arr            = test_container.get_element_tuple_arr();
        const auto const_tuple_element_arr      = std::as_const(test_container).get_element_tuple_arr();

        // Validate zip_view::size()
        STATIC_ASSERT(CanMemberSize<ZipType> == (std::ranges::sized_range<AllView<RangeTypes>> && ...));
        if constexpr (CanMemberSize<ZipType>) {
            using expected_size_type = std::_Make_unsigned_like_t<
                std::common_type_t<decltype(std::ranges::size(std::declval<AllView<RangeTypes>>()))...>>;
            std::same_as<expected_size_type> auto zip_size = zipped_range.size();

            assert(zip_size == std::ranges::size(tuple_element_arr));
            STATIC_ASSERT(noexcept(zipped_range.size())
                          == noexcept(std::ranges::min(
                              {static_cast<expected_size_type>(std::declval<AllView<RangeTypes>>().size())...})));
        }

        STATIC_ASSERT(CanMemberSize<const ZipType> == (std::ranges::sized_range<const AllView<RangeTypes>> && ...));
        if constexpr (CanMemberSize<const ZipType>) {
            using expected_size_type = std::_Make_unsigned_like_t<
                std::common_type_t<decltype(std::ranges::size(std::declval<const AllView<RangeTypes>>()))...>>;
            std::same_as<expected_size_type> auto zip_size = zipped_range.size();

            assert(zip_size == std::ranges::size(tuple_element_arr));
            STATIC_ASSERT(noexcept(std::as_const(zipped_range).size())
                          == noexcept(std::ranges::min(
                              {static_cast<expected_size_type>(std::declval<const AllView<RangeTypes>>().size())...})));
        }

        const bool is_empty = std::ranges::empty(tuple_element_arr);

        // Validate view_interface::empty() and view_interface::operator bool()
        //
        // From here on out, we'll be re-using concepts which we already verified to reduce
        // redundancy.
        STATIC_ASSERT(
            CanMemberEmpty<ZipType> == (std::ranges::sized_range<ZipType> || std::ranges::forward_range<ZipType>) );
        if constexpr (CanMemberEmpty<ZipType>) {
            assert(zipped_range.empty() == is_empty);
        }

        STATIC_ASSERT(
            CanMemberEmpty<
                const ZipType> == (std::ranges::sized_range<const ZipType> || std::ranges::forward_range<const ZipType>) );
        if constexpr (CanMemberEmpty<const ZipType>) {
            assert(std::as_const(zipped_range).empty() == is_empty);
        }

        STATIC_ASSERT(CanBool<ZipType> == CanMemberEmpty<ZipType>);
        if constexpr (CanBool<ZipType>) {
            assert(static_cast<bool>(zipped_range) != is_empty);
        }

        STATIC_ASSERT(CanBool<const ZipType> == CanMemberEmpty<const ZipType>);
        if constexpr (CanBool<const ZipType>) {
            assert(static_cast<bool>(std::as_const(zipped_range)) != is_empty);
        }

        // Validate contents of zipped range
        assert(std::ranges::equal(zipped_range, tuple_element_arr, [](const auto& lhs_tuple, const auto& rhs_tuple) {
            return do_tuples_reference_same_objects(lhs_tuple, rhs_tuple);
        }));

#pragma warning(push)
#pragma warning(disable : 4127)
        if (!(std::ranges::forward_range<AllView<RangeTypes>> && ...)) // intentionally not if constexpr
        {
            return true;
        }
#pragma warning(pop)

        // Validate view_interface::data()
        //
        // This should never exist because zip_view's iterator never models contiguous_iterator.
        STATIC_ASSERT(!std::contiguous_iterator<std::ranges::iterator_t<ZipType>>);
        STATIC_ASSERT(!CanMemberData<ZipType>);
        STATIC_ASSERT(!std::contiguous_iterator<std::ranges::iterator_t<const ZipType>>);
        STATIC_ASSERT(!CanMemberData<const ZipType>);

        // Validate view_interface::operator[]
        STATIC_ASSERT(CanIndex<ZipType> == std::ranges::random_access_range<ZipType>);
        if constexpr (CanIndex<ZipType>) {
            assert(do_tuples_reference_same_objects(zipped_range[0], tuple_element_arr[0]));
        }

        STATIC_ASSERT(CanIndex<const ZipType> == std::ranges::random_access_range<const ZipType>);
        if constexpr (CanIndex<const ZipType>) {
            assert(do_tuples_reference_same_objects(std::as_const(zipped_range)[0], tuple_element_arr[0]));
        }

        // Validate view_interface::front()
        STATIC_ASSERT(CanMemberFront<ZipType> == std::ranges::forward_range<ZipType>);
        if constexpr (CanMemberFront<ZipType>) {
            assert(do_tuples_reference_same_objects(zipped_range.front(), tuple_element_arr[0]));
        }

        STATIC_ASSERT(CanMemberFront<const ZipType> == std::ranges::forward_range<const ZipType>);
        if constexpr (CanMemberFront<const ZipType>) {
            assert(do_tuples_reference_same_objects(std::as_const(zipped_range).front(), const_tuple_element_arr[0]));
        }

        // Validate view_interface::back()
        STATIC_ASSERT(
            CanMemberBack<
                ZipType> == (std::ranges::bidirectional_range<ZipType> && std::ranges::common_range<ZipType>) );
        if constexpr (CanMemberBack<ZipType>) {
            assert(do_tuples_reference_same_objects(
                zipped_range.back(), tuple_element_arr[TestContainerType::smallest_array_size - 1]));
        }

        STATIC_ASSERT(
            CanMemberBack<
                const ZipType> == (std::ranges::bidirectional_range<const ZipType> && std::ranges::common_range<const ZipType>) );
        if constexpr (CanMemberBack<const ZipType>) {
            assert(do_tuples_reference_same_objects(std::as_const(zipped_range).back(),
                const_tuple_element_arr[TestContainerType::smallest_array_size - 1]));
        }

        // Validate zip_view::begin()
        STATIC_ASSERT(CanMemberBegin<ZipType>);
        {
            const std::same_as<std::ranges::iterator_t<ZipType>> auto itr = zipped_range.begin();
            assert(do_tuples_reference_same_objects(*itr, tuple_element_arr[0]));
        }

        STATIC_ASSERT(CanMemberBegin<const ZipType> == (std::ranges::range<const AllView<RangeTypes>> && ...));
        if constexpr (CanMemberBegin<const ZipType>) {
            assert(
                do_tuples_reference_same_objects(*(std::as_const(zipped_range).begin()), const_tuple_element_arr[0]));
        }

        // Validate zip_view::end()
        STATIC_ASSERT(CanMemberEnd<ZipType>);
        if constexpr (std::equality_comparable<std::ranges::iterator_t<ZipType>>) {
            auto end = zipped_range.begin();
            std::ranges::advance(end, TestContainerType::smallest_array_size);

            assert(end == zipped_range.end());
        }

        STATIC_ASSERT(CanMemberEnd<const ZipType> == (std::ranges::range<const AllView<RangeTypes>> && ...));
        if constexpr (CanMemberEnd<const ZipType> && std::equality_comparable<std::ranges::iterator_t<const ZipType>>) {
            auto end = std::as_const(zipped_range).begin();
            std::ranges::advance(end, TestContainerType::smallest_array_size);

            assert(end == std::as_const(zipped_range).end());
        }

        const auto validate_iterators_lambda = []<typename ArrayType, class LocalZipType, class... LocalRangeTypes>(
                                                   LocalZipType& relevant_range,
                                                   const ArrayType& relevant_tuple_element_arr) {
            constexpr bool is_const = std::same_as<LocalZipType, std::add_const_t<LocalZipType>>;

            STATIC_ASSERT(
                std::is_default_constructible_v<std::ranges::iterator_t<
                    LocalZipType>> == (std::is_default_constructible_v<std::ranges::iterator_t<LocalRangeTypes>> && ...));

            std::same_as<std::ranges::iterator_t<LocalZipType>> auto itr = relevant_range.begin();

            // Validate iterator operator overloads
            if constexpr (std::ranges::forward_range<LocalZipType>) {
                assert(do_tuples_reference_same_objects(*itr++, relevant_tuple_element_arr[0]));
            } else {
                // If LocalZipType does not model forward_range, we can still use post-fix operator
                // which returns void.
                itr++;
            }

            assert(do_tuples_reference_same_objects(*++itr, relevant_tuple_element_arr[2]));

            if constexpr (std::ranges::bidirectional_range<LocalZipType>) {
                assert(do_tuples_reference_same_objects(*itr--, relevant_tuple_element_arr[2]));
                assert(do_tuples_reference_same_objects(*--itr, relevant_tuple_element_arr[0]));
            }

            if constexpr (std::ranges::random_access_range<LocalZipType>) {
                itr += 2;
                assert(do_tuples_reference_same_objects(*itr, relevant_tuple_element_arr[2]));

                itr -= 2;
                assert(do_tuples_reference_same_objects(*itr, relevant_tuple_element_arr[0]));

                assert(do_tuples_reference_same_objects(itr[2], relevant_tuple_element_arr[2]));

                const std::same_as<std::ranges::iterator_t<LocalZipType>> auto itr2 = (itr + 2);
                assert(do_tuples_reference_same_objects(*itr2, relevant_tuple_element_arr[2]));

                const std::same_as<std::ranges::iterator_t<LocalZipType>> auto itr3 = (2 + itr);
                assert(do_tuples_reference_same_objects(*itr3, relevant_tuple_element_arr[2]));

                const std::same_as<std::ranges::iterator_t<LocalZipType>> auto itr4 = itr3 - 2;
                assert(do_tuples_reference_same_objects(*itr4, relevant_tuple_element_arr[0]));

                using iterator_difference_type =
                    std::common_type_t<std::ranges::range_difference_t<LocalRangeTypes>...>;

                const std::same_as<iterator_difference_type> auto diff1 = (itr2 - itr);
                assert(diff1 == static_cast<iterator_difference_type>(2));

                const std::same_as<iterator_difference_type> auto diff2 = (itr - itr2);
                assert(diff2 == static_cast<iterator_difference_type>(-2));

                if constexpr ((std::equality_comparable<std::ranges::iterator_t<LocalRangeTypes>> && ...)) {
                    assert(itr == itr4);
                    assert(itr != itr2);
                }

                // Per LWG-3692: The only constraint placed on operator<=> for zip_view's iterator is that
                // (std::ranges::random_access_range<LocalZipType>) is satisfied. We need no additional checks.
                using three_way_ordering_category = decltype(itr <=> itr2);

                assert(itr <=> itr4 == three_way_ordering_category::equivalent);
                assert(itr <=> itr2 == three_way_ordering_category::less);
                assert(itr2 <=> itr == three_way_ordering_category::greater);
            }

            // Validate [ADL::]iter_move()
            if constexpr (is_const) {
                STATIC_ASSERT(std::is_same_v<decltype(iter_move(itr)),
                    std::tuple<decltype(std::ranges::iter_move(
                        std::declval<std::ranges::iterator_t<const AllView<RangeTypes>>>()))...>>);
            } else {
                STATIC_ASSERT(std::is_same_v<decltype(iter_move(itr)),
                    std::tuple<decltype(std::ranges::iter_move(
                        std::declval<std::ranges::iterator_t<AllView<RangeTypes>>>()))...>>);
            }

            STATIC_ASSERT(noexcept(iter_move(itr))
                              == (noexcept(std::ranges::iter_move(
                                      std::declval<const std::ranges::iterator_t<LocalRangeTypes>&>()))
                                  && ...)
                          && (std::is_nothrow_move_constructible_v<
                                  std::ranges::range_rvalue_reference_t<LocalRangeTypes>> && ...));

            assert(do_tuples_reference_same_objects(*itr, iter_move(itr)));

            // Validate [ADL::]iter_swap()
            if constexpr ((std::indirectly_swappable<std::ranges::iterator_t<LocalRangeTypes>> && ...)) {
                const typename TestContainerType::element_tuple_type old_itr_value = *itr;

                std::same_as<std::ranges::iterator_t<LocalZipType>> auto itr2 = itr;
                itr2++;

                const typename TestContainerType::element_tuple_type old_itr2_value = *itr2;

                iter_swap(itr, itr2);

                assert(*itr == old_itr2_value);
                assert(*itr2 == old_itr_value);

                // We can't test iter_swap()'s noexcept specifier without access to the member variables
                // of the iterator.
            }

            // Validate sentinels
            if constexpr (!std::ranges::common_range<LocalZipType>) {
                STATIC_ASSERT(
                    std::is_default_constructible_v<std::ranges::sentinel_t<
                        LocalZipType>> == (std::is_default_constructible_v<std::ranges::sentinel_t<LocalRangeTypes>> && ...));

                const std::same_as<std::ranges::iterator_t<LocalZipType>> auto itr2 = relevant_range.begin();
                const std::same_as<std::ranges::sentinel_t<LocalZipType>> auto sen  = relevant_range.end();

                if constexpr ((std::sentinel_for<std::ranges::sentinel_t<LocalRangeTypes>,
                                   std::ranges::iterator_t<LocalRangeTypes>> && ...)) {
                    auto advanced_itr = relevant_range.begin();
                    std::ranges::advance(advanced_itr, TestContainerType::smallest_array_size);

                    assert(advanced_itr == sen);
                    assert(itr2 != sen);
                }

                if constexpr ((std::sized_sentinel_for<std::ranges::sentinel_t<LocalRangeTypes>,
                                   std::ranges::iterator_t<LocalRangeTypes>> && ...)) {
                    using difference_type = std::common_type_t<std::ranges::range_difference_t<LocalRangeTypes>...>;

                    const std::same_as<difference_type> auto diff1 = (itr2 - sen);
                    const std::same_as<difference_type> auto diff2 = (sen - itr2);

                    if constexpr (std::is_arithmetic_v<difference_type>) {
                        assert(diff1 == -(static_cast<difference_type>(TestContainerType::smallest_array_size)));
                        assert(diff2 == static_cast<difference_type>(TestContainerType::smallest_array_size));
                    } else if constexpr (std::constructible_from<difference_type,
                                             decltype(TestContainerType::smallest_array_size)>) {
                        assert(diff1 == -difference_type{TestContainerType::smallest_array_size});
                        assert(diff2 == difference_type{TestContainerType::smallest_array_size});
                    }
                }
            }
        };

        // Validate iterators and sentinels
        if constexpr (CanMemberBegin<ZipType> && CanMemberEnd<ZipType>) {
            validate_iterators_lambda.template operator()<decltype(tuple_element_arr), ZipType, AllView<RangeTypes>...>(
                zipped_range, tuple_element_arr);
        }

        if constexpr (CanMemberBegin<const ZipType> && CanMemberEnd<const ZipType>) {
            validate_iterators_lambda
                .template operator()<decltype(const_tuple_element_arr), const ZipType, const AllView<RangeTypes>...>(
                    std::as_const(zipped_range), const_tuple_element_arr);
        }
    }

    return true;
}

// zip_view<RangeTypes...> is sensitive to the following:
//
//   - The categories of RangeTypes...
//   - Whether or not differencing for RangeTypes... is valid; in C++,
//     (std::sized_sentinel_for<std::ranges::sentinel_t<RangeTypes>, std::ranges::iterator_t<RangeTypes>> && ...)
//   - The commonality of RangeTypes...
//   - Whether or not (std::sentinel_for<std::ranges::iterator_t<RangeTypes>, std::ranges::iterator_t<RangeTypes>> &&
//   ...)
//     is true
//   - Whether or not the size() member exists for each of RangeTypes..., as it is used by std::ranges::size()
//   - The sizeof...(RangeTypes)
//
// Other conditions are irrelevant:
//
//   - The type of std::ranges::range_reference_t<RangeTypes> for any RangeTypes... is not considered
//   - The raw types of RangeTypes... need not fulfill the view concept because std::views::all_t<RangeTypes>... is
//     used instead, which does satisfy the view concept

template <class Category, class Element, test::Sized IsSized, test::CanDifference Diff, test::Common IsCommon,
    test::CanCompare Eq>
using test_range = test::range<Category, Element, IsSized, Diff, IsCommon,
    (std::derived_from<Category, std::forward_iterator_tag> || to_bool(IsCommon) ? test::CanCompare::yes : Eq)>;

constexpr std::array default_int_array{0, 1, 2, 3, 4, 5, 6};

constexpr single_element_test_container<int, 7> single_element_container_instance{default_int_array};
constexpr three_element_test_container<int, 7, float, 9, char, 5> three_element_container_instance{default_int_array,
    std::array{0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f}, std::array{'a', 'b', 'c', 'd', 'e'}};

// The typical instantiator struct isn't templated like this, so that it could then be used with the
// existing testing machinery defined in range_algorithm_support.hpp. We can't use that stuff here,
// however, because std::views::zip takes multiple template parameters.
//
// The idea here might be useful for other view types which take multiple range parameters. Feel
// free to move something similar into range_algorithm_support.hpp if you feel that this could be
// used in another test suite.

template <bool IsMoveOnly>
struct range_type_solver {
protected:
    template <class Category, class Element, test::Sized IsSized, test::CanDifference Diff, test::Common IsCommon,
        test::CanCompare Eq>
    using range_type = test_range<Category, Element, IsSized, Diff, IsCommon, Eq>;
};

template <>
struct range_type_solver<true> {
protected:
    template <class Category, class Element, test::Sized IsSized, test::CanDifference Diff, test::Common IsCommon,
        test::CanCompare Eq>
    using range_type = test::range<Category, Element, IsSized, Diff, IsCommon,
        (std::derived_from<Category, std::forward_iterator_tag> || to_bool(IsCommon) ? test::CanCompare::yes : Eq),
        test::ProxyRef{!std::derived_from<Category, std::contiguous_iterator_tag>}, test::CanView::yes,
        test::Copyability::move_only>;
};

template <bool IsMoveOnly, class Category, test::Sized IsSized, test::CanDifference Diff, test::Common IsCommon,
    test::CanCompare Eq>
struct instantiator_impl : private range_type_solver<IsMoveOnly> {
private:
    template <class OtherCategory, class Element, test::Sized OtherIsSized, test::CanDifference OtherDiff,
        test::Common OtherIsCommon, test::CanCompare OtherEq>
    using range_type = typename range_type_solver<IsMoveOnly>::template range_type<OtherCategory, Element, OtherIsSized,
        OtherDiff, OtherIsCommon, OtherEq>;

    template <class... Types>
    struct first_type_solver {};

    template <class FirstType, class... OtherTypes>
    struct first_type_solver<FirstType, OtherTypes...> {
        using first_type = FirstType;
    };

    using standard_range_tuple_type = std::tuple<range_type<Category, const int, IsSized, Diff, IsCommon, Eq>,
        range_type<Category, const float, IsSized, Diff, IsCommon, Eq>,
        range_type<Category, const char, IsSized, Diff, IsCommon, Eq>>;

    using differing_category_range_type = range_type<std::input_iterator_tag, const int, IsSized, Diff, IsCommon, Eq>;
    using differing_size_member_range_type  = range_type<Category, const int,
        (IsSized == test::Sized::yes ? test::Sized::no : test::Sized::yes), Diff, IsCommon, Eq>;
    using differing_sentinel_for_range_type = range_type<Category, const int, IsSized,
        (Diff == test::CanDifference::yes ? test::CanDifference::no : test::CanDifference::yes), IsCommon, Eq>;
    using differing_is_common_range_type    = range_type<Category, const int, IsSized, Diff,
        (IsCommon == test::Common::yes ? test::Common::no : test::Common::yes), Eq>;
    using differing_can_compare_range_tupe  = range_type<Category, const int, IsSized, Diff, IsCommon,
        (Eq == test::CanCompare::yes ? test::CanCompare::no : test::CanCompare::yes)>;

public:
    static constexpr void call() {
        const auto test_instances_lambda = []<std::size_t... Indices>(std::index_sequence<Indices...>) {
            // NOTE: We intentionally create lots of ranges here so that we do not have to worry about
            // making multiple calls to std::views::zip::begin() for input iterators.

            // Test the single-range use of std::views::zip (i.e., sizeof...(RangeTypes) == 1).
            auto single_range = std::tuple_element_t<0, standard_range_tuple_type>{
                single_element_container_instance.get_element_span()};
            test_one(single_element_container_instance, single_range);

            // Test three ranges with std::views::zip with...

            // all of their traits being the same,...
            {
                auto first_range = std::tuple_element_t<0, standard_range_tuple_type>{
                    three_element_container_instance.get_element_span<0>()};
                auto second_range = std::tuple_element_t<1, standard_range_tuple_type>{
                    three_element_container_instance.get_element_span<1>()};
                auto third_range = std::tuple_element_t<2, standard_range_tuple_type>{
                    three_element_container_instance.get_element_span<2>()};

                test_one(three_element_container_instance, first_range, second_range, third_range);
            }

            // one range having a different category,...
            if constexpr (!std::is_same_v<Category, std::input_iterator_tag>) {
                auto differing_category_range =
                    differing_category_range_type{three_element_container_instance.get_element_span<0>()};
                auto second_range = std::tuple_element_t<1, standard_range_tuple_type>{
                    three_element_container_instance.get_element_span<1>()};
                auto third_range = std::tuple_element_t<2, standard_range_tuple_type>{
                    three_element_container_instance.get_element_span<2>()};

                test_one(three_element_container_instance, differing_category_range, second_range, third_range);
            }

            // one range having a different path for std::ranges::size(),...
            {
                auto differing_size_member_range =
                    differing_size_member_range_type{three_element_container_instance.get_element_span<0>()};
                auto second_range = std::tuple_element_t<1, standard_range_tuple_type>{
                    three_element_container_instance.get_element_span<1>()};
                auto third_range = std::tuple_element_t<2, standard_range_tuple_type>{
                    three_element_container_instance.get_element_span<2>()};

                test_one(three_element_container_instance, differing_size_member_range, second_range, third_range);
            }

            // one range having a different std::sized_sentinel_for value,...
            {
                auto differing_sentinel_for_range =
                    differing_sentinel_for_range_type{three_element_container_instance.get_element_span<0>()};
                auto second_range = std::tuple_element_t<1, standard_range_tuple_type>{
                    three_element_container_instance.get_element_span<1>()};
                auto third_range = std::tuple_element_t<2, standard_range_tuple_type>{
                    three_element_container_instance.get_element_span<2>()};

                test_one(three_element_container_instance, differing_sentinel_for_range, second_range, third_range);
            }

            // one range having a different commonality,...
            {
                auto differing_is_common_range =
                    differing_is_common_range_type{three_element_container_instance.get_element_span<0>()};
                auto second_range = std::tuple_element_t<1, standard_range_tuple_type>{
                    three_element_container_instance.get_element_span<1>()};
                auto third_range = std::tuple_element_t<2, standard_range_tuple_type>{
                    three_element_container_instance.get_element_span<2>()};

                test_one(three_element_container_instance, differing_is_common_range, second_range, third_range);
            }

            // and one range having a different iterator/sentinel type equality.
            {
                auto differing_can_compare_range =
                    differing_can_compare_range_tupe{three_element_container_instance.get_element_span<0>()};
                auto second_range = std::tuple_element_t<1, standard_range_tuple_type>{
                    three_element_container_instance.get_element_span<1>()};
                auto third_range = std::tuple_element_t<2, standard_range_tuple_type>{
                    three_element_container_instance.get_element_span<2>()};

                test_one(three_element_container_instance, differing_can_compare_range, second_range, third_range);
            }
        };

        test_instances_lambda(std::make_index_sequence<3>{});
    }
};

template <class Category, test::Sized IsSized, test::CanDifference Diff, test::Common IsCommon, test::CanCompare Eq>
struct instantiator : public instantiator_impl<false, Category, IsSized, Diff, IsCommon, Eq> {};

template <class Category, test::Sized IsSized, test::CanDifference Diff, test::Common IsCommon, test::CanCompare Eq>
struct move_only_view_instantiator : public instantiator_impl<true, Category, IsSized, Diff, IsCommon, Eq> {};

template <class Category,
    template <class, test::Sized, test::CanDifference, test::Common, test::CanCompare> class InstantiatorType>
constexpr bool instantiation_test_for_category() {
    // Unlike previous tested range/view types, std::views::zip takes an unbounded parameter pack of ranges, so
    // we cannot test every possible combination of sensitive inputs. However, every evaluated condition
    // other than sizeof... is evaluated as (Condition && ...); so, if one condition fails, then the entire
    // constraint should fail to be met.

    using test::Sized, test::CanDifference, test::Common, test::CanCompare;

    InstantiatorType<Category, Sized::no, CanDifference::no, Common::no, CanCompare::no>::call();
    InstantiatorType<Category, Sized::no, CanDifference::no, Common::no, CanCompare::yes>::call();
    InstantiatorType<Category, Sized::no, CanDifference::no, Common::yes, CanCompare::no>::call();
    InstantiatorType<Category, Sized::no, CanDifference::no, Common::yes, CanCompare::yes>::call();
    InstantiatorType<Category, Sized::no, CanDifference::yes, Common::no, CanCompare::no>::call();
    InstantiatorType<Category, Sized::no, CanDifference::yes, Common::no, CanCompare::yes>::call();
    InstantiatorType<Category, Sized::no, CanDifference::yes, Common::yes, CanCompare::no>::call();
    InstantiatorType<Category, Sized::no, CanDifference::yes, Common::yes, CanCompare::yes>::call();
    InstantiatorType<Category, Sized::yes, CanDifference::no, Common::no, CanCompare::no>::call();
    InstantiatorType<Category, Sized::yes, CanDifference::no, Common::no, CanCompare::yes>::call();
    InstantiatorType<Category, Sized::yes, CanDifference::no, Common::yes, CanCompare::no>::call();
    InstantiatorType<Category, Sized::yes, CanDifference::no, Common::yes, CanCompare::yes>::call();
    InstantiatorType<Category, Sized::yes, CanDifference::yes, Common::no, CanCompare::no>::call();
    InstantiatorType<Category, Sized::yes, CanDifference::yes, Common::no, CanCompare::yes>::call();
    InstantiatorType<Category, Sized::yes, CanDifference::yes, Common::yes, CanCompare::no>::call();
    InstantiatorType<Category, Sized::yes, CanDifference::yes, Common::yes, CanCompare::yes>::call();

    return true;
}

template <template <class, test::Sized, test::CanDifference, test::Common, test::CanCompare> class InstantiatorType>
constexpr bool instantiation_test() {
    return (instantiation_test_for_category<std::input_iterator_tag, InstantiatorType>()
            && instantiation_test_for_category<std::bidirectional_iterator_tag, InstantiatorType>()
            && instantiation_test_for_category<std::forward_iterator_tag, InstantiatorType>()
            && instantiation_test_for_category<std::random_access_iterator_tag, InstantiatorType>());
}
int main() {
    // Validate views
    { // ... copyable, single view
        constexpr std::span<const int> int_span = single_element_container_instance.get_element_span();

        STATIC_ASSERT(test_one(single_element_container_instance, int_span));
        test_one(single_element_container_instance, int_span);
    }

    { // ... copyable, multiple views
        constexpr std::span<const int> int_span     = three_element_container_instance.get_element_span<0>();
        constexpr std::span<const float> float_span = three_element_container_instance.get_element_span<1>();
        constexpr std::span<const char> char_span   = three_element_container_instance.get_element_span<2>();

        STATIC_ASSERT(test_one(three_element_container_instance, int_span, float_span, char_span));
        test_one(three_element_container_instance, int_span, float_span, char_span);
    }

    { // ... move-only, single and multiple views
        instantiation_test<move_only_view_instantiator>();
    }

    // Validate non-views
    {
        STATIC_ASSERT(test_one(
            single_element_container_instance, single_element_container_instance.get_underlying_element_array()));
        test_one(single_element_container_instance, single_element_container_instance.get_underlying_element_array());
    }
    {
        STATIC_ASSERT(test_one(three_element_container_instance,
            three_element_container_instance.get_underlying_element_array<0>(),
            three_element_container_instance.get_underlying_element_array<1>(),
            three_element_container_instance.get_underlying_element_array<2>()));
        test_one(three_element_container_instance, three_element_container_instance.get_underlying_element_array<0>(),
            three_element_container_instance.get_underlying_element_array<1>(),
            three_element_container_instance.get_underlying_element_array<2>());
    }

    // Empty RangeTypes... parameter pack
    STATIC_ASSERT(std::is_same_v<decltype(std::views::zip()), std::decay_t<decltype(std::views::empty<std::tuple<>>)>>);

    // The number of generated template instances is so large here that we cannot evaluate them
    // all in a constant-evaluated context without hitting the constexpr step limit. So, we split
    // them up by category.
    STATIC_ASSERT(instantiation_test_for_category<std::input_iterator_tag, instantiator>());
    STATIC_ASSERT(instantiation_test_for_category<std::bidirectional_iterator_tag, instantiator>());
    STATIC_ASSERT(instantiation_test_for_category<std::forward_iterator_tag, instantiator>());
    STATIC_ASSERT(instantiation_test_for_category<std::random_access_iterator_tag, instantiator>());

    instantiation_test<instantiator>();
}