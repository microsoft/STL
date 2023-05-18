// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <memory>
#include <ranges>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

template <class... RangeTypes>
concept CanViewZip = requires(RangeTypes&&... rngs) { views::zip(std::forward<RangeTypes>(rngs)...); };

template <class RangeType>
using AllView = views::all_t<RangeType>;

template <class ElementType>
concept CanTestElementType = // Required for iter_swap test
    (copy_constructible<ElementType> && equality_comparable<ElementType> && swappable<ElementType>);

template <class Type1, size_t Type1Size, class Type2, size_t Type2Size, class Type3, size_t Type3Size>
    requires (CanTestElementType<Type1> && CanTestElementType<Type2> && CanTestElementType<Type3>)
class three_element_test_container {
private:
    array<Type1, Type1Size> type_one_array;
    array<Type2, Type2Size> type_two_array;
    array<Type3, Type3Size> type_three_array;

public:
    using element_tuple_type       = tuple<Type1, Type2, Type3>;
    using const_element_tuple_type = tuple<const Type1, const Type2, const Type3>;

    using reference_tuple_type              = tuple<Type1&, Type2&, Type3&>;
    using const_reference_tuple_type        = tuple<const Type1&, const Type2&, const Type3&>;
    using rvalue_reference_tuple_type       = tuple<Type1&&, Type2&&, Type3&&>;
    using const_rvalue_reference_tuple_type = tuple<const Type1&&, const Type2&&, const Type3&&>;

    static constexpr size_t smallest_array_size = (min) ({Type1Size, Type2Size, Type3Size});

    constexpr three_element_test_container(const array<Type1, Type1Size>& type_one_init_arr,
        const array<Type2, Type2Size>& type_two_init_arr, const array<Type3, Type3Size>& type_three_init_arr)
        : type_one_array(type_one_init_arr), type_two_array(type_two_init_arr), type_three_array(type_three_init_arr) {}

    template <size_t ElementIndex>
    constexpr auto get_element_span() {
        STATIC_ASSERT(ElementIndex < 3);

        if constexpr (ElementIndex == 0) {
            return span<Type1, Type1Size>{type_one_array};
        } else if constexpr (ElementIndex == 1) {
            return span<Type2, Type2Size>{type_two_array};
        } else {
            return span<Type3, Type3Size>{type_three_array};
        }
    }

    template <size_t ElementIndex>
    constexpr auto get_element_span() const {
        STATIC_ASSERT(ElementIndex < 3);

        if constexpr (ElementIndex == 0) {
            return span<add_const_t<Type1>, Type1Size>{type_one_array};
        } else if constexpr (ElementIndex == 1) {
            return span<add_const_t<Type2>, Type2Size>{type_two_array};
        } else {
            return span<add_const_t<Type3>, Type3Size>{type_three_array};
        }
    }

    template <size_t ElementIndex>
    constexpr auto& get_underlying_element_array() {
        STATIC_ASSERT(ElementIndex < 3);

        if constexpr (ElementIndex == 0) {
            return type_one_array;
        } else if constexpr (ElementIndex == 1) {
            return type_two_array;
        } else {
            return type_three_array;
        }
    }

    template <size_t ElementIndex>
    constexpr const auto& get_underlying_element_array() const {
        STATIC_ASSERT(ElementIndex < 3);

        if constexpr (ElementIndex == 0) {
            return type_one_array;
        } else if constexpr (ElementIndex == 1) {
            return type_two_array;
        } else {
            return type_three_array;
        }
    }

    constexpr reference_tuple_type get_expected_element_tuple(const size_t index) {
        assert(index < smallest_array_size);
        return reference_tuple_type{type_one_array[index], type_two_array[index], type_three_array[index]};
    }

    constexpr const_reference_tuple_type get_expected_element_tuple(const size_t index) const {
        assert(index < smallest_array_size);
        return const_reference_tuple_type{type_one_array[index], type_two_array[index], type_three_array[index]};
    }

    constexpr array<reference_tuple_type, smallest_array_size> get_element_tuple_arr() {
        const auto make_tuple_arr_lambda = [this]<size_t... Indices>(index_sequence<Indices...>) {
            return array{get_expected_element_tuple(Indices)...};
        };

        return make_tuple_arr_lambda(make_index_sequence<smallest_array_size>{});
    }

    constexpr array<const_reference_tuple_type, smallest_array_size> get_element_tuple_arr() const {
        const auto make_tuple_arr_lambda = [this]<size_t... Indices>(index_sequence<Indices...>) {
            return array{get_expected_element_tuple(Indices)...};
        };

        return make_tuple_arr_lambda(make_index_sequence<smallest_array_size>{});
    }
};

template <class Type, size_t Size>
    requires (CanTestElementType<Type>)
class single_element_test_container {
private:
    array<Type, Size> element_array;

public:
    using element_tuple_type       = tuple<Type>;
    using const_element_tuple_type = tuple<const Type>;

    using reference_tuple_type              = tuple<Type&>;
    using const_reference_tuple_type        = tuple<const Type&>;
    using rvalue_reference_tuple_type       = tuple<Type&&>;
    using const_rvalue_reference_tuple_type = tuple<const Type&&>;

    static constexpr size_t smallest_array_size = Size;

    constexpr single_element_test_container(const array<Type, Size>& element_init_arr)
        : element_array(element_init_arr) {}

    constexpr auto get_element_span() {
        return span<Type>{element_array};
    }

    constexpr auto get_element_span() const {
        return span<add_const_t<Type>>{element_array};
    }

    constexpr auto& get_underlying_element_array() {
        return element_array;
    }

    constexpr const auto& get_underlying_element_array() const {
        return element_array;
    }

    constexpr reference_tuple_type get_expected_element_tuple(const size_t index) {
        assert(index < smallest_array_size);
        return reference_tuple_type{element_array[index]};
    }

    constexpr const_reference_tuple_type get_expected_element_tuple(const size_t index) const {
        assert(index < smallest_array_size);
        return const_reference_tuple_type{element_array[index]};
    }

    constexpr array<reference_tuple_type, smallest_array_size> get_element_tuple_arr() {
        const auto make_tuple_arr_lambda = [this]<size_t... Indices>(index_sequence<Indices...>) {
            return array{get_expected_element_tuple(Indices)...};
        };

        return make_tuple_arr_lambda(make_index_sequence<smallest_array_size>{});
    }

    constexpr array<const_reference_tuple_type, smallest_array_size> get_element_tuple_arr() const {
        const auto make_tuple_arr_lambda = [this]<size_t... Indices>(index_sequence<Indices...>) {
            return array{get_expected_element_tuple(Indices)...};
        };

        return make_tuple_arr_lambda(make_index_sequence<smallest_array_size>{});
    }
};

// NOTE: views::zip shouldn't care about whether or not the views use proxy references,
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
    STATIC_ASSERT(tuple_size_v<LHSTupleType> == tuple_size_v<RHSTupleType>);

    const auto evaluate_single_element_lambda = [&lhs_tuple, &rhs_tuple]<size_t CurrIndex>() {
        using reference_type = typename reference_type_solver<tuple_element_t<CurrIndex, LHSTupleType>>::reference_type;
        return addressof(static_cast<reference_type>(get<CurrIndex>(lhs_tuple)))
            == addressof(static_cast<reference_type>(get<CurrIndex>(rhs_tuple)));
    };

    using index_sequence_type         = make_index_sequence<tuple_size_v<LHSTupleType>>;
    const auto evaluate_tuples_lambda = [&evaluate_single_element_lambda]<size_t... Indices>(
                                            index_sequence<Indices...>) {
        return (evaluate_single_element_lambda.template operator()<Indices>() && ...);
    };

    return evaluate_tuples_lambda(index_sequence_type{});
}

#pragma warning(push)
#pragma warning(disable : 4100) // unreferenced formal parameter

template <class TestContainerType, ranges::input_range... RangeTypes>
constexpr bool test_one(TestContainerType& test_container, RangeTypes&&... rngs) {
    // Ignore instances where one of the generated test ranges does not model
    // ranges::viewable_range.
    if constexpr ((ranges::viewable_range<RangeTypes&> && ...)) {
        using ZipType = ranges::zip_view<AllView<RangeTypes&>...>;

        constexpr bool are_views = (ranges::view<remove_cvref_t<RangeTypes&>> && ...) && (sizeof...(RangeTypes) > 0);

        STATIC_ASSERT(ranges::view<ZipType>);
        STATIC_ASSERT(ranges::input_range<ZipType>);
        STATIC_ASSERT(ranges::forward_range<ZipType> == (ranges::forward_range<RangeTypes&> && ...));
        STATIC_ASSERT(ranges::bidirectional_range<ZipType> == (ranges::bidirectional_range<RangeTypes&> && ...));
        STATIC_ASSERT(ranges::random_access_range<ZipType> == (ranges::random_access_range<RangeTypes&> && ...));
        STATIC_ASSERT(
            ranges::common_range<ZipType> == (sizeof...(RangeTypes) == 1 && (ranges::common_range<RangeTypes&> && ...))
            || (!(ranges::bidirectional_range<RangeTypes&> && ...) && (ranges::common_range<RangeTypes&> && ...))
            || ((ranges::random_access_range<RangeTypes&> && ...) && (ranges::sized_range<RangeTypes&> && ...)));

        // Validate conditional default-initializability
        STATIC_ASSERT(is_default_constructible_v<ZipType> == (is_default_constructible_v<AllView<RangeTypes>> && ...));

        // Validate conditional borrowed_range
        STATIC_ASSERT(ranges::borrowed_range<ZipType> == (ranges::borrowed_range<AllView<RangeTypes>> && ...));

        // Validate range adaptor object

        // ... with lvalue arguments
        STATIC_ASSERT(CanViewZip<RangeTypes&...> == (!are_views || (copy_constructible<AllView<RangeTypes>> && ...)));
        if constexpr (CanViewZip<RangeTypes&...>) {
            using ExpectedZipType      = ZipType;
            constexpr bool is_noexcept = (is_nothrow_copy_constructible_v<AllView<RangeTypes>> && ...);

            STATIC_ASSERT(same_as<decltype(views::zip(rngs...)), ExpectedZipType>);
            STATIC_ASSERT(noexcept(views::zip(rngs...)) == is_noexcept);
        }

        // ... with const lvalue arguments
        STATIC_ASSERT(CanViewZip<const remove_reference_t<RangeTypes>&...>
                      == (!are_views || (copy_constructible<AllView<RangeTypes>> && ...)));
        if constexpr (CanViewZip<const remove_reference_t<RangeTypes>&...>) {
            using ExpectedZipType      = ranges::zip_view<AllView<const remove_reference_t<RangeTypes>&>...>;
            constexpr bool is_noexcept = (is_nothrow_copy_constructible_v<AllView<RangeTypes>> && ...);

            STATIC_ASSERT(same_as<decltype(views::zip(as_const(rngs)...)), ExpectedZipType>);
            STATIC_ASSERT(noexcept(views::zip(as_const(rngs)...)) == is_noexcept);
        }

        // ... with rvalue argument
        STATIC_ASSERT(CanViewZip<remove_reference_t<RangeTypes>...>
                      == (are_views || (movable<remove_reference_t<RangeTypes>> && ...)));
        if constexpr (CanViewZip<remove_reference_t<RangeTypes>...>) {
            using ExpectedZipType      = ranges::zip_view<AllView<remove_reference_t<RangeTypes>>...>;
            constexpr bool is_noexcept = (is_nothrow_move_constructible_v<AllView<RangeTypes>> && ...);

            STATIC_ASSERT(same_as<decltype(views::zip(std::move(rngs)...)), ExpectedZipType>);
            STATIC_ASSERT(noexcept(views::zip(std::move(rngs)...)) == is_noexcept);
        }

        // ... with const rvalue argument
        STATIC_ASSERT(CanViewZip<const remove_reference_t<RangeTypes>...>
                      == (are_views && (copy_constructible<AllView<RangeTypes>> && ...)));
        if constexpr (CanViewZip<const remove_reference_t<RangeTypes>...>) {
            using ExpectedZipType      = ranges::zip_view<AllView<const remove_reference_t<RangeTypes>>...>;
            constexpr bool is_noexcept = (is_nothrow_copy_constructible_v<AllView<RangeTypes>> && ...);

            STATIC_ASSERT(same_as<decltype(views::zip(std::move(as_const(rngs))...)), ExpectedZipType>);
            STATIC_ASSERT(noexcept(views::zip(std::move(as_const(rngs))...)) == is_noexcept);
        }

        // Validate deduction guide
        same_as<ZipType> auto zipped_range = ranges::zip_view{std::forward<RangeTypes>(rngs)...};
        const auto tuple_element_arr       = test_container.get_element_tuple_arr();
        const auto const_tuple_element_arr = as_const(test_container).get_element_tuple_arr();

        // Validate zip_view::size()
        STATIC_ASSERT(CanMemberSize<ZipType> == (ranges::sized_range<AllView<RangeTypes>> && ...));
        if constexpr (CanMemberSize<ZipType>) {
            using expected_size_type =
                _Make_unsigned_like_t<common_type_t<decltype(ranges::size(declval<AllView<RangeTypes>>()))...>>;
            same_as<expected_size_type> auto zip_size = zipped_range.size();

            assert(zip_size == ranges::size(tuple_element_arr));
            STATIC_ASSERT(noexcept(zipped_range.size())
                          == (noexcept(static_cast<expected_size_type>(declval<AllView<RangeTypes>>().size())) && ...));
        }

        STATIC_ASSERT(CanMemberSize<const ZipType> == (ranges::sized_range<const AllView<RangeTypes>> && ...));
        if constexpr (CanMemberSize<const ZipType>) {
            using expected_size_type =
                _Make_unsigned_like_t<common_type_t<decltype(ranges::size(declval<const AllView<RangeTypes>>()))...>>;
            same_as<expected_size_type> auto zip_size = as_const(zipped_range).size();

            assert(zip_size == ranges::size(tuple_element_arr));
            STATIC_ASSERT(
                noexcept(as_const(zipped_range).size())
                == (noexcept(static_cast<expected_size_type>(declval<const AllView<RangeTypes>>().size())) && ...));
        }

        const bool is_empty = ranges::empty(tuple_element_arr);

        // Validate view_interface::empty() and view_interface::operator bool()
        //
        // From here on out, we'll be reusing concepts which we already verified to reduce
        // redundancy.
        STATIC_ASSERT(CanMemberEmpty<ZipType> == (ranges::sized_range<ZipType> || ranges::forward_range<ZipType>) );
        if constexpr (CanMemberEmpty<ZipType>) {
            assert(zipped_range.empty() == is_empty);
        }

        STATIC_ASSERT(CanMemberEmpty<const ZipType>
                      == (ranges::sized_range<const ZipType> || ranges::forward_range<const ZipType>) );
        if constexpr (CanMemberEmpty<const ZipType>) {
            assert(as_const(zipped_range).empty() == is_empty);
        }

        STATIC_ASSERT(CanBool<ZipType> == CanMemberEmpty<ZipType>);
        if constexpr (CanBool<ZipType>) {
            assert(static_cast<bool>(zipped_range) != is_empty);
        }

        STATIC_ASSERT(CanBool<const ZipType> == CanMemberEmpty<const ZipType>);
        if constexpr (CanBool<const ZipType>) {
            assert(static_cast<bool>(as_const(zipped_range)) != is_empty);
        }

        // Validate contents of zipped range
        assert(ranges::equal(zipped_range, tuple_element_arr, [](const auto& lhs_tuple, const auto& rhs_tuple) {
            return do_tuples_reference_same_objects(lhs_tuple, rhs_tuple);
        }));

#pragma warning(push)
#pragma warning(disable : 4127) // Conditional Expression is Constant
        if (!(ranges::forward_range<AllView<RangeTypes>> && ...)) { // intentionally not if constexpr
            return true;
        }
#pragma warning(pop)

        // Validate view_interface::data()
        //
        // This should never exist because zip_view's iterator never models contiguous_iterator.
        STATIC_ASSERT(!contiguous_iterator<ranges::iterator_t<ZipType>>);
        STATIC_ASSERT(!CanMemberData<ZipType>);
        STATIC_ASSERT(!contiguous_iterator<ranges::iterator_t<const ZipType>>);
        STATIC_ASSERT(!CanMemberData<const ZipType>);

        // Validate view_interface::operator[]
        STATIC_ASSERT(CanIndex<ZipType> == ranges::random_access_range<ZipType>);
        if constexpr (CanIndex<ZipType>) {
            assert(do_tuples_reference_same_objects(zipped_range[0], tuple_element_arr[0]));
        }

        STATIC_ASSERT(CanIndex<const ZipType> == ranges::random_access_range<const ZipType>);
        if constexpr (CanIndex<const ZipType>) {
            assert(do_tuples_reference_same_objects(as_const(zipped_range)[0], tuple_element_arr[0]));
        }

        // Validate view_interface::front()
        STATIC_ASSERT(CanMemberFront<ZipType> == ranges::forward_range<ZipType>);
        if constexpr (CanMemberFront<ZipType>) {
            assert(do_tuples_reference_same_objects(zipped_range.front(), tuple_element_arr[0]));
        }

        STATIC_ASSERT(CanMemberFront<const ZipType> == ranges::forward_range<const ZipType>);
        if constexpr (CanMemberFront<const ZipType>) {
            assert(do_tuples_reference_same_objects(as_const(zipped_range).front(), const_tuple_element_arr[0]));
        }

        // Validate view_interface::back()
        STATIC_ASSERT(
            CanMemberBack<ZipType> == (ranges::bidirectional_range<ZipType> && ranges::common_range<ZipType>) );
        if constexpr (CanMemberBack<ZipType>) {
            assert(do_tuples_reference_same_objects(
                zipped_range.back(), tuple_element_arr[TestContainerType::smallest_array_size - 1]));
        }

        STATIC_ASSERT(CanMemberBack<const ZipType>
                      == (ranges::bidirectional_range<const ZipType> && ranges::common_range<const ZipType>) );
        if constexpr (CanMemberBack<const ZipType>) {
            assert(do_tuples_reference_same_objects(
                as_const(zipped_range).back(), const_tuple_element_arr[TestContainerType::smallest_array_size - 1]));
        }

        // Validate zip_view::begin()
        STATIC_ASSERT(CanMemberBegin<ZipType>);
        {
            const same_as<ranges::iterator_t<ZipType>> auto itr = zipped_range.begin();
            assert(do_tuples_reference_same_objects(*itr, tuple_element_arr[0]));
        }

        STATIC_ASSERT(CanMemberBegin<const ZipType> == (ranges::range<const AllView<RangeTypes>> && ...));
        if constexpr (CanMemberBegin<const ZipType>) {
            assert(do_tuples_reference_same_objects(*(as_const(zipped_range).begin()), const_tuple_element_arr[0]));
        }

        // Validate zip_view::end()
        STATIC_ASSERT(CanMemberEnd<ZipType>);
        if constexpr (equality_comparable<ranges::iterator_t<ZipType>>) {
            auto end = zipped_range.begin();
            ranges::advance(end, TestContainerType::smallest_array_size);

            assert(end == zipped_range.end());
        }

        STATIC_ASSERT(CanMemberEnd<const ZipType> == (ranges::range<const AllView<RangeTypes>> && ...));
        if constexpr (CanMemberEnd<const ZipType> && equality_comparable<ranges::iterator_t<const ZipType>>) {
            auto end = as_const(zipped_range).begin();
            ranges::advance(end, TestContainerType::smallest_array_size);

            assert(end == as_const(zipped_range).end());
        }

        const auto validate_iterators_lambda = []<class LocalZipType, class ArrayType, class... LocalRangeTypes>(
                                                   LocalZipType& relevant_range,
                                                   const ArrayType& relevant_tuple_element_arr) {
            constexpr bool is_const = same_as<LocalZipType, add_const_t<LocalZipType>>;

            STATIC_ASSERT(is_default_constructible_v<ranges::iterator_t<LocalZipType>>
                          == (is_default_constructible_v<ranges::iterator_t<LocalRangeTypes>> && ...));

            same_as<ranges::iterator_t<LocalZipType>> auto itr = relevant_range.begin();

            // Validate iterator operator overloads
            if constexpr (ranges::forward_range<LocalZipType>) {
                assert(do_tuples_reference_same_objects(*itr++, relevant_tuple_element_arr[0]));
            } else {
                // If LocalZipType does not model forward_range, we can still use the postfix operator
                // which returns void.
                itr++;
            }

            assert(do_tuples_reference_same_objects(*++itr, relevant_tuple_element_arr[2]));

            if constexpr (ranges::bidirectional_range<LocalZipType>) {
                assert(do_tuples_reference_same_objects(*itr--, relevant_tuple_element_arr[2]));
                assert(do_tuples_reference_same_objects(*--itr, relevant_tuple_element_arr[0]));
            }

            if constexpr (ranges::random_access_range<LocalZipType>) {
                itr += 2;
                assert(do_tuples_reference_same_objects(*itr, relevant_tuple_element_arr[2]));

                itr -= 2;
                assert(do_tuples_reference_same_objects(*itr, relevant_tuple_element_arr[0]));

                assert(do_tuples_reference_same_objects(itr[2], relevant_tuple_element_arr[2]));

                const same_as<ranges::iterator_t<LocalZipType>> auto itr2 = (itr + 2);
                assert(do_tuples_reference_same_objects(*itr2, relevant_tuple_element_arr[2]));

                const same_as<ranges::iterator_t<LocalZipType>> auto itr3 = (2 + itr);
                assert(do_tuples_reference_same_objects(*itr3, relevant_tuple_element_arr[2]));

                const same_as<ranges::iterator_t<LocalZipType>> auto itr4 = itr3 - 2;
                assert(do_tuples_reference_same_objects(*itr4, relevant_tuple_element_arr[0]));

                using iterator_difference_type = common_type_t<ranges::range_difference_t<LocalRangeTypes>...>;

                const same_as<iterator_difference_type> auto diff1 = (itr2 - itr);
                assert(diff1 == static_cast<iterator_difference_type>(2));

                const same_as<iterator_difference_type> auto diff2 = (itr - itr2);
                assert(diff2 == static_cast<iterator_difference_type>(-2));

                if constexpr ((equality_comparable<ranges::iterator_t<LocalRangeTypes>> && ...)) {
                    assert(itr == itr4);
                    assert(itr != itr2);
                }

                // Per LWG-3692: The only constraint placed on operator<=> for zip_view's iterator is that
                // (ranges::random_access_range<LocalZipType>) is satisfied. We need no additional checks.
                using three_way_ordering_category = decltype(itr <=> itr2);

                assert(itr <=> itr4 == three_way_ordering_category::equivalent);
                assert(itr <=> itr2 == three_way_ordering_category::less);
                assert(itr2 <=> itr == three_way_ordering_category::greater);
            }

            // Validate [ADL::]iter_move()
            if constexpr (is_const) {
                STATIC_ASSERT(is_same_v<decltype(ranges::iter_move(itr)),
                    tuple<decltype(ranges::iter_move(declval<ranges::iterator_t<const AllView<RangeTypes>>>()))...>>);
            } else {
                STATIC_ASSERT(is_same_v<decltype(ranges::iter_move(itr)),
                    tuple<decltype(ranges::iter_move(declval<ranges::iterator_t<AllView<RangeTypes>>>()))...>>);
            }

            STATIC_ASSERT(
                noexcept(ranges::iter_move(itr))
                    == (noexcept(ranges::iter_move(declval<const ranges::iterator_t<LocalRangeTypes>&>())) && ...)
                && (is_nothrow_move_constructible_v<ranges::range_rvalue_reference_t<LocalRangeTypes>> && ...));

            assert(do_tuples_reference_same_objects(*itr, ranges::iter_move(itr)));

            // Validate [ADL::]iter_swap()
            if constexpr ((indirectly_swappable<ranges::iterator_t<LocalRangeTypes>> && ...)) {
                const typename TestContainerType::element_tuple_type old_itr_value = *itr;

                // itr is currently at relevant_range.begin() right now, so we assign itr2 to that
                // instead of itr. This avoids copy construction, which is problematic for move-only
                // ranges.
                same_as<ranges::iterator_t<LocalZipType>> auto itr2 = relevant_range.begin();
                itr2++;

                const typename TestContainerType::element_tuple_type old_itr2_value = *itr2;

                ranges::iter_swap(itr, itr2);

                assert(*itr == old_itr2_value);
                assert(*itr2 == old_itr_value);

                // We can't test iter_swap()'s noexcept specifier without access to the member variables
                // of the iterator.
            }

            // Validate sentinels
            if constexpr (!ranges::common_range<LocalZipType>) {
                STATIC_ASSERT(is_default_constructible_v<ranges::sentinel_t<LocalZipType>>
                              == (is_default_constructible_v<ranges::sentinel_t<LocalRangeTypes>> && ...));

                const same_as<ranges::iterator_t<LocalZipType>> auto itr2 = relevant_range.begin();
                const same_as<ranges::sentinel_t<LocalZipType>> auto sen  = relevant_range.end();

                if constexpr ((sentinel_for<ranges::sentinel_t<LocalRangeTypes>, ranges::iterator_t<LocalRangeTypes>>
                                  && ...)) {
                    auto advanced_itr = relevant_range.begin();
                    ranges::advance(advanced_itr, TestContainerType::smallest_array_size);

                    assert(advanced_itr == sen);
                    assert(itr2 != sen);
                }

                if constexpr ((sized_sentinel_for<ranges::sentinel_t<LocalRangeTypes>,
                                   ranges::iterator_t<LocalRangeTypes>>
                                  && ...)) {
                    using difference_type = common_type_t<ranges::range_difference_t<LocalRangeTypes>...>;

                    const same_as<difference_type> auto diff1 = (itr2 - sen);
                    const same_as<difference_type> auto diff2 = (sen - itr2);

                    assert(diff1 == -(static_cast<difference_type>(TestContainerType::smallest_array_size)));
                    assert(diff2 == static_cast<difference_type>(TestContainerType::smallest_array_size));
                }
            }
        };

        // Validate iterators and sentinels
        if constexpr (CanMemberBegin<ZipType> && CanMemberEnd<ZipType>) {
            validate_iterators_lambda
                .template operator()<ZipType, decltype(tuple_element_arr), AllView<RangeTypes&>...>(
                    zipped_range, tuple_element_arr);
        }

        if constexpr (CanMemberBegin<const ZipType> && CanMemberEnd<const ZipType>) {
            validate_iterators_lambda
                .template operator()<const ZipType, decltype(const_tuple_element_arr), const AllView<RangeTypes&>...>(
                    as_const(zipped_range), const_tuple_element_arr);
        }
    }

    return true;
}

#pragma warning(pop)

// zip_view<RangeTypes...> is sensitive to the following:
//
//   - The categories of RangeTypes...
//   - The commonality of RangeTypes...
//   - Whether or not the size() member exists for each of RangeTypes..., as it is used by ranges::size()
//   - Whether or not each range's iterator and sentinel models sized_sentinel_for
//   - The sizeof...(RangeTypes)
//
// Other conditions are irrelevant.

// TRANSITION, VSO-1655299: use a helper function as a workaround
template <class Category, test::Common IsCommon>
[[nodiscard]] constexpr test::CanCompare test_range_can_compare() {
    return test::CanCompare{to_bool(IsCommon) || derived_from<Category, forward_iterator_tag>};
}

template <class Category, class Element, test::Sized IsSized, test::Common IsCommon, test::CanDifference Diff>
using test_range =
    test::range<Category, Element, IsSized, Diff, IsCommon, test_range_can_compare<Category, IsCommon>()>;

constexpr array default_int_array{0, 1, 2, 3, 4, 5, 6};

constexpr single_element_test_container<int, 7> single_element_container_instance{default_int_array};
constexpr three_element_test_container<int, 7, float, 9, char, 5> three_element_container_instance{
    default_int_array, array{0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f}, array{'a', 'b', 'c', 'd', 'e'}};

// The typical instantiator struct isn't templated like this, so that it could then be used with the
// existing testing machinery defined in range_algorithm_support.hpp. We can't use that stuff here,
// however, because views::zip takes multiple template parameters.
//
// The idea here might be useful for other view types which take multiple range parameters. Feel
// free to move something similar into range_algorithm_support.hpp if you feel that this could be
// used in another test suite.

template <bool IsMoveOnly>
class range_type_solver {
protected:
    template <class Category, class Element, test::Sized IsSized, test::Common IsCommon, test::CanDifference Diff>
    using range_type = test::range<Category, Element, IsSized, Diff, IsCommon,
        test_range_can_compare<Category, IsCommon>(), test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>},
        test::CanView::yes, test::Copyability::move_only>;
};

template <>
class range_type_solver<false> {
protected:
    template <class Category, class Element, test::Sized IsSized, test::Common IsCommon, test::CanDifference Diff>
    using range_type = test_range<Category, Element, IsSized, IsCommon, Diff>;
};

template <bool IsMoveOnly, class Category, test::Sized IsSized, test::Common IsCommon, test::CanDifference Diff>
class instantiator_impl : private range_type_solver<IsMoveOnly> {
private:
    template <class OtherCategory, class Element, test::Sized OtherIsSized, test::Common OtherIsCommon,
        test::CanDifference OtherDiff>
    using range_type = typename range_type_solver<IsMoveOnly>::template range_type<OtherCategory, Element, OtherIsSized,
        OtherIsCommon, OtherDiff>;

    template <class... Types>
    struct first_type_solver {};

    template <class FirstType, class... OtherTypes>
    struct first_type_solver<FirstType, OtherTypes...> {
        using first_type = FirstType;
    };

    using standard_range_tuple_type = tuple<range_type<Category, int, IsSized, IsCommon, Diff>,
        range_type<Category, float, IsSized, IsCommon, Diff>, range_type<Category, char, IsSized, IsCommon, Diff>>;

    template <class OtherCategory>
    struct differing_category_solver {
        using category_type = input_iterator_tag;
    };

    template <>
    struct differing_category_solver<input_iterator_tag> {
        using category_type = forward_iterator_tag;
    };

    using differing_category_range_type =
        range_type<typename differing_category_solver<Category>::category_type, int, IsSized, IsCommon, Diff>;
    using differing_size_member_range_type =
        range_type<Category, int, (IsSized == test::Sized::yes ? test::Sized::no : test::Sized::yes), IsCommon, Diff>;
    using differing_is_common_range_type              = range_type<Category, int, IsSized,
        (IsCommon == test::Common::yes ? test::Common::no : test::Common::yes), Diff>;
    using differing_iterator_sentinel_diff_range_type = range_type<Category, int, IsSized, IsCommon,
        (Diff == test::CanDifference::yes ? test::CanDifference::no : test::CanDifference::yes)>;

    template <class ContainerType>
    static constexpr void test_single_range(ContainerType&& single_element_container) {
        // Create a copy of the container. That way, we can always test iter_swap,
        // even if container has const elements.
        auto writable = single_element_container;
        tuple_element_t<0, standard_range_tuple_type> single_range{writable.get_element_span()};

        test_one(writable, single_range);
    }

    template <class DifferingRangeType, class ContainerType>
    static constexpr void test_three_ranges(ContainerType&& three_element_container) {
        // Create a copy of the container. That way, we can always test iter_swap,
        // even if container has const elements.
        auto writable = three_element_container;
        DifferingRangeType first_range{writable.template get_element_span<0>()};
        tuple_element_t<1, standard_range_tuple_type> second_range{writable.template get_element_span<1>()};
        tuple_element_t<2, standard_range_tuple_type> third_range{writable.template get_element_span<2>()};

        test_one(writable, first_range, second_range, third_range);
    }

public:
    static constexpr void call() {
        // Test the single-range use of views::zip (i.e., sizeof...(RangeTypes) == 1).
        test_single_range(single_element_container_instance);

        // Test three ranges with views::zip with...

        // all of their traits being the same, ...
        test_three_ranges<tuple_element_t<0, standard_range_tuple_type>>(three_element_container_instance);

        // one range having a different category, ...
        test_three_ranges<differing_category_range_type>(three_element_container_instance);

        // one range having a different path for ranges::size(), ...
        test_three_ranges<differing_size_member_range_type>(three_element_container_instance);

        // one range having a different commonality, ...
        test_three_ranges<differing_is_common_range_type>(three_element_container_instance);

        // and one range having iterators and sentinels which model sized_sentinel_for
        // differently.
        test_three_ranges<differing_iterator_sentinel_diff_range_type>(three_element_container_instance);
    }
};

template <class Category, test::Sized IsSized, test::Common IsCommon, test::CanDifference Diff>
class instantiator : public instantiator_impl<false, Category, IsSized, IsCommon, Diff> {};

template <class Category, test::Sized IsSized, test::Common IsCommon, test::CanDifference Diff>
class move_only_view_instantiator : public instantiator_impl<true, Category, IsSized, IsCommon, Diff> {};

template <class Category, template <class, test::Sized, test::Common, test::CanDifference> class InstantiatorType>
constexpr bool instantiation_test_for_category() {
    // Unlike previous tested range/view types, views::zip takes an unbounded parameter pack of ranges, so
    // we cannot test every possible combination of sensitive inputs. However, every evaluated condition
    // other than sizeof... is evaluated as (Condition && ...); so, if one condition fails, then the entire
    // constraint should fail to be met.

    using test::Sized, test::Common, test::CanDifference;

    InstantiatorType<Category, Sized::no, Common::no, CanDifference::no>::call();
    InstantiatorType<Category, Sized::no, Common::no, CanDifference::yes>::call();
    InstantiatorType<Category, Sized::no, Common::yes, CanDifference::no>::call();
    InstantiatorType<Category, Sized::no, Common::yes, CanDifference::yes>::call();
    InstantiatorType<Category, Sized::yes, Common::no, CanDifference::no>::call();
    InstantiatorType<Category, Sized::yes, Common::no, CanDifference::yes>::call();
    InstantiatorType<Category, Sized::yes, Common::yes, CanDifference::no>::call();
    InstantiatorType<Category, Sized::yes, Common::yes, CanDifference::yes>::call();

    return true;
}

template <template <class, test::Sized, test::Common, test::CanDifference> class InstantiatorType>
constexpr bool instantiation_test() {
    // The MSVC tends to run out of compiler heap space due to the sheer number of instantiations
    // if we try to test everything at once. So, we split it up into segments based on category.

#if defined(TEST_INPUT)
    return instantiation_test_for_category<input_iterator_tag, InstantiatorType>();
#elif defined(TEST_FORWARD) // ^^^ TEST_INPUT / TEST_FORWARD vvv
    return instantiation_test_for_category<forward_iterator_tag, InstantiatorType>();
#elif defined(TEST_BIDIRECTIONAL) // ^^^ TEST_FORWARD / TEST_BIDIRECTIONAL vvv
    return instantiation_test_for_category<bidirectional_iterator_tag, InstantiatorType>();
#elif defined(TEST_RANDOM) // ^^^ TEST_BIDIRECTIONAL / TEST_RANDOM vvv
    return instantiation_test_for_category<random_access_iterator_tag, InstantiatorType>();
#else // ^^^ TEST_RANDOM / UNKNOWN vvv
    static_assert(false, "ERROR: A defined test macro was never specified when executing test P2321R2_views_zip!");
    return false;
#endif // ^^^ UNKNOWN ^^^
}

int main() {
    // Validate views
    { // ... copyable, single view
        constexpr span<const int> int_span = single_element_container_instance.get_element_span();

        STATIC_ASSERT(test_one(single_element_container_instance, int_span));
        test_one(single_element_container_instance, int_span);
    }

    { // ... copyable, multiple views
        constexpr span<const int> int_span     = three_element_container_instance.get_element_span<0>();
        constexpr span<const float> float_span = three_element_container_instance.get_element_span<1>();
        constexpr span<const char> char_span   = three_element_container_instance.get_element_span<2>();

        STATIC_ASSERT(test_one(three_element_container_instance, int_span, float_span, char_span));
        test_one(three_element_container_instance, int_span, float_span, char_span);
    }

    { // ... move-only, single and multiple views
        STATIC_ASSERT(instantiation_test<move_only_view_instantiator>());
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
    STATIC_ASSERT(is_same_v<decltype(views::zip()), decay_t<decltype(views::empty<tuple<>>)>>);

    STATIC_ASSERT(instantiation_test<instantiator>());
    instantiation_test<instantiator>();
}

void test_noexcept_strengthening() { // COMPILE-ONLY
    // This quickly verifies that various operations have been noexcept-strengthened.
    // This isn't attempting to be exhaustive - it skips operations that haven't been strengthened, or that are
    // difficult to test. Also, this isn't attempting to test corner cases for conditional noexcept.

    int arr1[]{10, 20, 30};
    int arr2[]{11, 22, 33};

    {
        STATIC_ASSERT(noexcept(ranges::zip_view{arr1, arr2}));
        STATIC_ASSERT(noexcept(views::zip(arr1, arr2)));

        ranges::zip_view zipped{arr1, arr2};

        STATIC_ASSERT(noexcept(zipped.begin()));
        STATIC_ASSERT(noexcept(zipped.end()));
        STATIC_ASSERT(noexcept(zipped.size()));

        auto it = zipped.begin();

        STATIC_ASSERT(noexcept(*it));
        STATIC_ASSERT(noexcept(++it));
        STATIC_ASSERT(noexcept(it++));
        STATIC_ASSERT(noexcept(--it));
        STATIC_ASSERT(noexcept(it--));
        STATIC_ASSERT(noexcept(it += 0));
        STATIC_ASSERT(noexcept(it -= 0));
        STATIC_ASSERT(noexcept(it[0]));
        STATIC_ASSERT(noexcept(it == it));
        STATIC_ASSERT(noexcept(it + 0));
        STATIC_ASSERT(noexcept(0 + it));
        STATIC_ASSERT(noexcept(it - 0));
        STATIC_ASSERT(noexcept(it - it));
        STATIC_ASSERT(noexcept(iter_move(it)));
        STATIC_ASSERT(noexcept(iter_swap(it, it)));
    }

    {
        ranges::zip_view zipped2{arr1, ranges::subrange{arr2, unreachable_sentinel}};

        auto iter = zipped2.begin();
        auto sent = zipped2.end();

        // The unreachable_sentinel causes sent to be a zip_view::_Sentinel.
        STATIC_ASSERT(!is_same_v<decltype(iter), decltype(sent)>);

        STATIC_ASSERT(noexcept(iter == sent));
    }
}
