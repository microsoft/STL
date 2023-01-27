// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <compare>
#include <concepts>
#include <functional>
#include <ranges>
#include <span>
#include <tuple>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

template <class RangeType>
using AllView = views::all_t<RangeType>;

template <bool IsConst, class TransformType, ranges::input_range... RangeTypes>
using TransformResultType = invoke_result_t<ranges::_Maybe_const<IsConst, TransformType>&,
    ranges::range_reference_t<ranges::_Maybe_const<IsConst, RangeTypes>>...>;

template <bool IsConst, class T>
constexpr auto& maybe_as_const(T& value) {
    if constexpr (IsConst) {
        return as_const(value);
    } else {
        return value;
    }
}

template <class Function, class... RangeTypes>
concept CanZipTransform = (ranges::viewable_range<RangeTypes> && ...)
                       && requires(Function&& func, RangeTypes&&... test_ranges) {
                              views::zip_transform(
                                  std::forward<Function>(func), std::forward<RangeTypes>(test_ranges)...);
                          };

template <class RangeType>
concept HasIteratorCategory = requires { typename ranges::iterator_t<RangeType>::iterator_category; };

#pragma warning(push)
#pragma warning(disable : 4365) // conversion from 'std::array<int,8>::size_type' to 'int', signed/unsigned mismatch
template <class TransformType, ranges::random_access_range TransformedElementsContainer, class LocalZipTransformType,
    ranges::input_range... RangeTypes>
constexpr bool validate_iterators_sentinels(
    LocalZipTransformType& relevant_range, const TransformedElementsContainer& transformed_elements) {
#pragma warning(pop)
    constexpr bool is_const = same_as<LocalZipTransformType, add_const_t<LocalZipTransformType>>;

    using InnerView            = ranges::zip_view<AllView<RangeTypes>...>;
    using BaseType             = ranges::_Maybe_const<is_const, InnerView>;
    using ZipIteratorTupleType = tuple<ranges::iterator_t<ranges::_Maybe_const<is_const, AllView<RangeTypes>>>...>;

    // Validate iterator type aliases
    {
        // Validate iterator_category
        if constexpr (ranges::forward_range<BaseType>) {
            STATIC_ASSERT(HasIteratorCategory<LocalZipTransformType>);

            using Cat                = typename ranges::iterator_t<LocalZipTransformType>::iterator_category;
            using transform_result_t = TransformResultType<is_const, TransformType, RangeTypes...>;

            if constexpr (!is_reference_v<transform_result_t>) {
                STATIC_ASSERT(same_as<Cat, input_iterator_tag>);
            } else {
                constexpr auto check_iterator_tags_closure = []<class TagType>() {
                    return (derived_from<typename iterator_traits<ranges::iterator_t<
                                             ranges::_Maybe_const<is_const, RangeTypes>>>::iterator_category,
                                TagType>
                            && ...);
                };

                if constexpr (check_iterator_tags_closure.template operator()<random_access_iterator_tag>()) {
                    STATIC_ASSERT(same_as<Cat, random_access_iterator_tag>);
                } else if constexpr (check_iterator_tags_closure.template operator()<bidirectional_iterator_tag>()) {
                    STATIC_ASSERT(same_as<Cat, bidirectional_iterator_tag>);
                } else if constexpr (check_iterator_tags_closure.template operator()<forward_iterator_tag>()) {
                    STATIC_ASSERT(same_as<Cat, forward_iterator_tag>);
                } else {
                    STATIC_ASSERT(same_as<Cat, input_iterator_tag>);
                }
            }
        } else {
            STATIC_ASSERT(!HasIteratorCategory<LocalZipTransformType>);
        }

        // Validate iterator_concept
        STATIC_ASSERT(same_as<typename ranges::iterator_t<LocalZipTransformType>::iterator_concept,
            typename ranges::iterator_t<BaseType>::iterator_concept>);

        // Validate value_type
        STATIC_ASSERT(same_as<typename ranges::iterator_t<LocalZipTransformType>::value_type,
            remove_cvref_t<TransformResultType<is_const, TransformType, RangeTypes...>>>);

        // Validate difference_type
        STATIC_ASSERT(same_as<typename ranges::iterator_t<LocalZipTransformType>::difference_type,
            ranges::range_difference_t<BaseType>>);
    }

    // Validate iterator constructors
    STATIC_ASSERT(is_default_constructible_v<ranges::iterator_t<LocalZipTransformType>>
                  == is_default_constructible_v<ranges::iterator_t<BaseType>>);
    STATIC_ASSERT(is_nothrow_default_constructible_v<ranges::iterator_t<LocalZipTransformType>>
                  == is_nothrow_default_constructible_v<ranges::iterator_t<BaseType>>);

    if constexpr (is_const && convertible_to<ranges::iterator_t<InnerView>, ranges::iterator_t<const InnerView>>) {
        STATIC_ASSERT(noexcept(ranges::iterator_t<LocalZipTransformType>{
                          declval<ranges::iterator_t<remove_const_t<LocalZipTransformType>>>()})
                      == is_nothrow_convertible_v<ranges::iterator_t<InnerView>, ranges::iterator_t<const InnerView>>);
    }

    same_as<ranges::iterator_t<LocalZipTransformType>> auto itr = relevant_range.begin();

    // Validate iterator operator overloads
    {
        const auto first_result = *itr;
        assert(first_result == *ranges::begin(transformed_elements));

        // NOTE: The actual noexcept specification for zip_transform_view::iterator::operator*() is as follows:
        //
        // Let Is be the pack 0, 1, ..., (sizeof...(Views)-1). The exception specification is equivalent to:
        // noexcept(invoke(*parent_->fun_, *std::get<Is>(inner_.current_)...)).
        //
        // Notably, parent_t is a pointer and inner_.current_ is a tuple, and operator->() on a pointer and
        // std::get(std::tuple<...>) are both noexcept. We thus simplify the noexcept check as follows:
        STATIC_ASSERT(
            noexcept(*itr)
            == noexcept(invoke(*declval<const ranges::_Movable_box<TransformType>&>(),
                *declval<const ranges::iterator_t<ranges::_Maybe_const<is_const, AllView<RangeTypes>>>&>()...)));
    }

    STATIC_ASSERT(noexcept(++itr) == noexcept(++declval<ranges::iterator_t<BaseType>&>()));

    if constexpr (ranges::forward_range<BaseType>) {
        same_as<ranges::iterator_t<LocalZipTransformType>> auto duplicate_itr = itr++;
        assert(*duplicate_itr == *ranges::begin(transformed_elements));
        STATIC_ASSERT(noexcept(itr++)
                      == is_nothrow_copy_constructible_v<ranges::iterator_t<LocalZipTransformType>>&& noexcept(++itr));
    } else {
        itr++;
        STATIC_ASSERT(noexcept(itr++) == noexcept(++itr));
    }

    assert(*++itr == transformed_elements[2]);

    if constexpr (ranges::bidirectional_range<BaseType>) {
        assert(*itr-- == transformed_elements[2]);
        STATIC_ASSERT(noexcept(itr--)
                      == is_nothrow_copy_constructible_v<ranges::iterator_t<LocalZipTransformType>>&& noexcept(--itr));

        assert(*--itr == transformed_elements[0]);
        STATIC_ASSERT(noexcept(--itr) == noexcept(--declval<ranges::iterator_t<BaseType>&>()));
    }

    if constexpr (ranges::random_access_range<BaseType>) {
        itr += 2;
        assert(*itr == transformed_elements[2]);
        STATIC_ASSERT(noexcept(itr += 2) == noexcept(declval<ranges::iterator_t<BaseType>&>() += 2));

        itr -= 2;
        assert(*itr == transformed_elements[0]);
        STATIC_ASSERT(noexcept(itr -= 2) == noexcept(declval<ranges::iterator_t<BaseType>&>() -= 2));

        assert(itr[2] == transformed_elements[2]);
        {
            constexpr bool is_random_access_noexcept =
                noexcept(apply([]<class... IteratorTypes>(const IteratorTypes&... itrs) noexcept(
                                   noexcept(invoke(*declval<const ranges::_Movable_box<TransformType>&>(),
                                       itrs[static_cast<iter_difference_t<IteratorTypes>>(2)]...))) { return true; },
                    declval<const ZipIteratorTupleType&>()));
            STATIC_ASSERT(noexcept(itr[2]) == is_random_access_noexcept);
        }

        const same_as<ranges::iterator_t<LocalZipTransformType>> auto itr2 = itr + 2;
        assert(*itr2 == transformed_elements[2]);
        STATIC_ASSERT(noexcept(itr + 2) == noexcept(declval<const ranges::iterator_t<BaseType>&>() + 2)
                      && is_nothrow_move_constructible_v<ranges::iterator_t<BaseType>>);

        const same_as<ranges::iterator_t<LocalZipTransformType>> auto itr3 = 2 + itr;
        assert(*itr3 == transformed_elements[2]);
        STATIC_ASSERT(noexcept(2 + itr) == noexcept(declval<const ranges::iterator_t<BaseType>&>() + 2)
                      && is_nothrow_move_constructible_v<ranges::iterator_t<BaseType>>);

        const same_as<ranges::iterator_t<LocalZipTransformType>> auto itr4 = itr3 - 2;
        assert(*itr4 == transformed_elements[0]);
        STATIC_ASSERT(noexcept(itr3 - 2) == noexcept(declval<const ranges::iterator_t<BaseType>&>() - 2)
                      && is_nothrow_move_constructible_v<ranges::iterator_t<BaseType>>);

        using three_way_ordering_category = decltype(itr <=> itr2);

        assert(itr <=> itr4 == three_way_ordering_category::equivalent);
        assert(itr <=> itr2 == three_way_ordering_category::less);
        assert(itr2 <=> itr == three_way_ordering_category::greater);

        STATIC_ASSERT(noexcept(itr <=> itr2)
                      == noexcept(declval<const ranges::iterator_t<BaseType>&>()
                                  <=> declval<const ranges::iterator_t<BaseType>&>()));
    }

    if constexpr (equality_comparable<ranges::iterator_t<BaseType>>) {
        same_as<ranges::iterator_t<LocalZipTransformType>> auto advanced_itr1 = relevant_range.begin();
        ranges::advance(advanced_itr1, 2);

        same_as<ranges::iterator_t<LocalZipTransformType>> auto advanced_itr2 = relevant_range.begin();
        ranges::advance(advanced_itr2, 2);

        assert(advanced_itr1 == advanced_itr2);
        STATIC_ASSERT(noexcept(advanced_itr1 == advanced_itr2)
                      == noexcept(declval<const ranges::iterator_t<BaseType>&>()
                                  == declval<const ranges::iterator_t<BaseType>&>()));

        assert(relevant_range.begin() != advanced_itr1);
    }

    if constexpr (!ranges::common_range<LocalZipTransformType>) {
        // Validate sentinel constructors
        STATIC_ASSERT(is_default_constructible_v<ranges::sentinel_t<LocalZipTransformType>>
                      == is_default_constructible_v<ranges::sentinel_t<BaseType>>);
        STATIC_ASSERT(is_nothrow_default_constructible_v<ranges::sentinel_t<LocalZipTransformType>>
                      == is_nothrow_default_constructible_v<ranges::sentinel_t<BaseType>>);

        if constexpr (is_const && convertible_to<ranges::sentinel_t<InnerView>, ranges::sentinel_t<const InnerView>>) {
            STATIC_ASSERT(noexcept(ranges::sentinel_t<LocalZipTransformType>{
                              declval<ranges::sentinel_t<remove_const_t<LocalZipTransformType>>>()})
                          == is_nothrow_move_constructible_v<ranges::sentinel_t<BaseType>>);
        }

        const same_as<ranges::sentinel_t<LocalZipTransformType>> auto sentinel = relevant_range.end();

        // Validate sentinel operator overloads
        {
            const auto validate_iterator_sentinel_equality_closure = [&]<bool IteratorConst>() {
                using comparison_iterator_t = ranges::iterator_t<ranges::_Maybe_const<IteratorConst, InnerView>>;
                using comparison_sentinel_t = ranges::sentinel_t<BaseType>;

                if constexpr (sentinel_for<comparison_sentinel_t, comparison_iterator_t>) {
                    auto end_iterator = relevant_range.begin();
                    ranges::advance(end_iterator, ranges::size(transformed_elements));

                    assert(end_iterator == sentinel);
                    STATIC_ASSERT(noexcept(end_iterator == sentinel)
                                  == noexcept(declval<const comparison_iterator_t&>()
                                              == declval<const comparison_sentinel_t&>()));

                    assert(itr != sentinel);
                }
            };

            validate_iterator_sentinel_equality_closure.template operator()<false>();
            validate_iterator_sentinel_equality_closure.template operator()<true>();
        }

        {
            const auto validate_iterator_sentinel_difference_closure = [&]<bool IteratorConst>() {
                using comparison_iterator_t = ranges::iterator_t<ranges::_Maybe_const<IteratorConst, InnerView>>;
                using comparison_sentinel_t = ranges::sentinel_t<BaseType>;

                if constexpr (sized_sentinel_for<comparison_sentinel_t, comparison_iterator_t>) {
                    using difference_type = ranges::range_difference_t<ranges::_Maybe_const<IteratorConst, InnerView>>;

                    const auto comparison_itr = maybe_as_const<IteratorConst>(relevant_range).begin();

                    const same_as<difference_type> auto diff1 = sentinel - comparison_itr;
                    assert(diff1 == static_cast<difference_type>(ranges::size(transformed_elements)));
                    STATIC_ASSERT(
                        noexcept(sentinel - comparison_itr)
                        == noexcept(declval<const comparison_sentinel_t&>() - declval<const comparison_iterator_t&>()));

                    const same_as<difference_type> auto diff2 = comparison_itr - sentinel;
                    assert(diff2 == -static_cast<difference_type>(ranges::size(transformed_elements)));
                    STATIC_ASSERT(
                        noexcept(comparison_itr - sentinel)
                        == noexcept(declval<const comparison_iterator_t&>() - declval<const comparison_sentinel_t&>()));
                }
            };

            validate_iterator_sentinel_difference_closure.template operator()<false>();
            validate_iterator_sentinel_difference_closure.template operator()<true>();
        }
    }

    return true;
}

#pragma warning(push)
#pragma warning(disable : 4100) // unreferenced formal parameter

template <class TransformType_, ranges::random_access_range TransformedElementsContainer,
    ranges::input_range... RangeTypes>
constexpr bool test_one(TransformType_&& transformer, const TransformedElementsContainer& transformed_elements,
    RangeTypes&&... test_ranges) {
    // Ignore instances where one of the generated test ranges does not model
    // ranges::viewable_range.
    if constexpr ((ranges::viewable_range<RangeTypes> && ...)) {
        using TransformType    = remove_cvref_t<TransformType_>;
        using ZipTransformType = ranges::zip_transform_view<TransformType, AllView<RangeTypes>...>;
        using InnerView        = ranges::zip_view<AllView<RangeTypes>...>;

        constexpr bool are_views = (ranges::view<remove_cvref_t<RangeTypes>> && ...) && (sizeof...(RangeTypes) > 0);

        STATIC_ASSERT(ranges::view<ZipTransformType>);

        // Validate commonality
        STATIC_ASSERT(ranges::common_range<ZipTransformType> == ranges::common_range<InnerView>);

        constexpr bool has_const_begin_end =
            ranges::range<const InnerView>
            && regular_invocable<const TransformType&, ranges::range_reference_t<const RangeTypes>...>;

        if constexpr (has_const_begin_end) {
            STATIC_ASSERT(ranges::common_range<const ZipTransformType> == ranges::common_range<const InnerView>);
        }

        // Validate conditional default-initializability
        STATIC_ASSERT(is_default_constructible_v<ZipTransformType>
                      == (is_default_constructible_v<ranges::_Movable_box<TransformType>>
                          && is_default_constructible_v<InnerView>) );
        STATIC_ASSERT(is_nothrow_default_constructible_v<ZipTransformType>
                      == (is_nothrow_default_constructible_v<ranges::_Movable_box<TransformType>>
                          && is_nothrow_default_constructible_v<InnerView>) );

        // Validate range adaptor object
        {
            constexpr bool can_copy_construct_ranges = !are_views || (copy_constructible<AllView<RangeTypes>> && ...);
            constexpr bool can_move_ranges           = are_views || (movable<remove_reference_t<RangeTypes>> && ...);

            // ... with lvalue arguments
            STATIC_ASSERT(CanZipTransform<TransformType, RangeTypes&...>
                          == (can_copy_construct_ranges && move_constructible<TransformType>) );
            if constexpr (CanZipTransform<TransformType, RangeTypes&...>) {
                using ExpectedZipTransformType = ZipTransformType;
                constexpr bool is_noexcept     = is_nothrow_move_constructible_v<ranges::_Movable_box<TransformType>>
                                          && is_nothrow_constructible_v<InnerView, AllView<RangeTypes>&&...>;

                STATIC_ASSERT(
                    same_as<decltype(views::zip_transform(std::forward<TransformType_>(transformer), test_ranges...)),
                        ExpectedZipTransformType>);
                STATIC_ASSERT(noexcept(views::zip_transform(std::forward<TransformType_>(transformer), test_ranges...))
                              == is_noexcept);
            }

            // ... with const lvalue arguments
            STATIC_ASSERT(CanZipTransform<TransformType, const remove_reference_t<RangeTypes>&...>
                          == (can_copy_construct_ranges && move_constructible<TransformType>) );
            if constexpr (CanZipTransform<TransformType, const remove_reference_t<RangeTypes>&...>) {
                using ExpectedZipTransformType =
                    ranges::zip_transform_view<TransformType, AllView<const remove_reference_t<RangeTypes>&>...>;
                constexpr bool is_noexcept = is_nothrow_move_constructible_v<ranges::_Movable_box<TransformType>>
                                          && is_nothrow_constructible_v<InnerView, const AllView<RangeTypes>&&...>;

                STATIC_ASSERT(same_as<decltype(views::zip_transform(
                                          std::forward<TransformType_>(transformer), as_const(test_ranges)...)),
                    ExpectedZipTransformType>);
                STATIC_ASSERT(
                    noexcept(views::zip_transform(std::forward<TransformType_>(transformer), as_const(test_ranges)...))
                    == is_noexcept);
            }

            // ... with rvalue arguments
            STATIC_ASSERT(CanZipTransform<TransformType, remove_reference_t<RangeTypes>...>
                          == (can_move_ranges && move_constructible<TransformType>) );
            if constexpr (CanZipTransform<TransformType, remove_reference_t<RangeTypes>...>) {
                using ExpectedZipTransformType =
                    ranges::zip_transform_view<TransformType, AllView<remove_reference_t<RangeTypes>>...>;
                constexpr bool is_noexcept = is_nothrow_move_constructible_v<ranges::_Movable_box<TransformType>>
                                          && is_nothrow_constructible_v<InnerView, AllView<RangeTypes>&&...>;

                STATIC_ASSERT(same_as<decltype(views::zip_transform(
                                          std::forward<TransformType_>(transformer), std::move(test_ranges)...)),
                    ExpectedZipTransformType>);
                STATIC_ASSERT(
                    noexcept(views::zip_transform(std::forward<TransformType_>(transformer), std::move(test_ranges)...))
                    == is_noexcept);
            }

            // ... with const rvalue arguments
            STATIC_ASSERT(CanZipTransform<TransformType, const remove_reference_t<RangeTypes>...>
                          == (are_views && (copy_constructible<AllView<RangeTypes>> && ...)
                              && move_constructible<TransformType>) );
            if constexpr (CanZipTransform<TransformType, const remove_reference_t<RangeTypes>...>) {
                using ExpectedZipTransformType =
                    ranges::zip_transform_view<TransformType, AllView<const remove_reference_t<RangeTypes>>...>;
                constexpr bool is_noexcept = is_nothrow_move_constructible_v<ranges::_Movable_box<TransformType>>
                                          && is_nothrow_constructible_v<InnerView, const AllView<RangeTypes>&&...>;

                STATIC_ASSERT(same_as<decltype(views::zip_transform(std::forward<TransformType_>(transformer),
                                          std::move(as_const(test_ranges))...)),
                    ExpectedZipTransformType>);
                STATIC_ASSERT(noexcept(views::zip_transform(
                                  std::forward<TransformType_>(transformer), std::move(as_const(test_ranges))...))
                              == is_noexcept);
            }
        }

        if constexpr (move_constructible<TransformType>) {
            // Validate deduction guide
            same_as<ZipTransformType> auto zipped_transformed_range = ranges::zip_transform_view{
                std::forward<TransformType_>(transformer), std::forward<RangeTypes>(test_ranges)...};

            // Validate zip_transform_view::size()
            STATIC_ASSERT(CanMemberSize<ZipTransformType> == ranges::sized_range<InnerView>);
            if constexpr (CanMemberSize<ZipTransformType>) {
                using expected_size_type                            = decltype(declval<InnerView&>().size());
                same_as<expected_size_type> auto zip_transform_size = zipped_transformed_range.size();

                assert(zip_transform_size == ranges::size(transformed_elements));
                STATIC_ASSERT(noexcept(zipped_transformed_range.size()) == noexcept(declval<InnerView&>().size()));
            }

            STATIC_ASSERT(CanMemberSize<const ZipTransformType> == ranges::sized_range<const InnerView>);
            if constexpr (CanMemberSize<const ZipTransformType>) {
                using expected_size_type                            = decltype(declval<const InnerView&>().size());
                same_as<expected_size_type> auto zip_transform_size = as_const(zipped_transformed_range).size();

                assert(zip_transform_size == ranges::size(transformed_elements));
                STATIC_ASSERT(noexcept(as_const(zipped_transformed_range).size())
                              == noexcept(declval<const InnerView&>().size()));
            }

            const bool is_empty = ranges::empty(transformed_elements);

            // We don't want an empty results range, since we still need to do additional testing.
            assert(!is_empty);

            // Validate view_interface::empty() and view_interface::operator bool()
            //
            // From here on out, we'll be reusing concepts which we already verified to reduce
            // redundancy.
            STATIC_ASSERT(CanMemberEmpty<ZipTransformType>
                          == (ranges::sized_range<ZipTransformType> || ranges::forward_range<ZipTransformType>) );
            if constexpr (CanMemberEmpty<ZipTransformType>) {
                assert(zipped_transformed_range.empty() == is_empty);
            }

            STATIC_ASSERT(
                CanMemberEmpty<const ZipTransformType>
                == (ranges::sized_range<const ZipTransformType> || ranges::forward_range<const ZipTransformType>) );
            if constexpr (CanMemberEmpty<const ZipTransformType>) {
                assert(as_const(zipped_transformed_range).empty() == is_empty);
            }

            STATIC_ASSERT(CanBool<ZipTransformType> == CanMemberEmpty<ZipTransformType>);
            if constexpr (CanBool<ZipTransformType>) {
                assert(static_cast<bool>(zipped_transformed_range) != is_empty);
            }

            STATIC_ASSERT(CanBool<const ZipTransformType> == CanMemberEmpty<const ZipTransformType>);
            if constexpr (CanBool<const ZipTransformType>) {
                assert(static_cast<bool>(as_const(zipped_transformed_range)) != is_empty);
            }

            // Validate view_interface::cbegin() and view_interface::cend()
            STATIC_ASSERT(CanMemberCBegin<ZipTransformType>);
            STATIC_ASSERT(CanMemberCEnd<ZipTransformType>);
            {
                STATIC_ASSERT(
                    is_same_v<decltype(zipped_transformed_range.cbegin()), ranges::const_iterator_t<ZipTransformType>>);
                STATIC_ASSERT(
                    is_same_v<decltype(zipped_transformed_range.cend()), ranges::const_sentinel_t<ZipTransformType>>);

                if constexpr ((derived_from<remove_cvref_t<RangeTypes>, forward_iterator_tag> && ...)) {
                    const auto cbegin_itr                = zipped_transformed_range.cbegin();
                    [[maybe_unused]] const auto cend_sen = zipped_transformed_range.cend();

                    assert(*cbegin_itr == *ranges::begin(transformed_elements));
                    assert(static_cast<decltype(ranges::size(transformed_elements))>(
                               ranges::distance(zipped_transformed_range.cbegin(), zipped_transformed_range.cend()))
                           == ranges::size(transformed_elements));
                }
            }

            STATIC_ASSERT(CanMemberCBegin<const ZipTransformType> == has_const_begin_end);
            STATIC_ASSERT(CanMemberCEnd<const ZipTransformType> == has_const_begin_end);
            if constexpr (has_const_begin_end) {
                STATIC_ASSERT(is_same_v<decltype(as_const(zipped_transformed_range).cbegin()),
                    ranges::const_iterator_t<const ZipTransformType>>);
                STATIC_ASSERT(is_same_v<decltype(as_const(zipped_transformed_range).cend()),
                    ranges::const_sentinel_t<const ZipTransformType>>);

                if constexpr ((derived_from<remove_cvref_t<RangeTypes>, forward_iterator_tag> && ...)) {
                    const same_as<ranges::const_iterator_t<const ZipTransformType>> auto cbegin_itr =
                        as_const(zipped_transformed_range).cbegin();
                    [[maybe_unused]] const same_as<ranges::const_sentinel_t<const ZipTransformType>> auto cend_sen =
                        as_const(zipped_transformed_range).cend();

                    assert(*cbegin_itr == *ranges::begin(transformed_elements));
                    assert(static_cast<decltype(ranges::size(transformed_elements))>(ranges::distance(
                               as_const(zipped_transformed_range).cbegin(), as_const(zipped_transformed_range).cend()))
                           == ranges::size(transformed_elements));
                }
            }

            // Validate contents of zip-transformed range
            assert(ranges::equal(zipped_transformed_range, transformed_elements));

#pragma warning(push)
#pragma warning(disable : 4127) // Conditional Expression is Constant
            if (!(ranges::forward_range<AllView<RangeTypes>> && ...)) // intentionally not if constexpr
            {
                return true;
            }
#pragma warning(pop)

            // Validate view_interface::data()
            //
            // This should never exist because zip_transform_view does not model
            // std::contiguous_range.
            STATIC_ASSERT(!ranges::contiguous_range<ZipTransformType>);
            STATIC_ASSERT(!CanMemberData<ZipTransformType>);
            STATIC_ASSERT(!ranges::contiguous_range<const ZipTransformType>);
            STATIC_ASSERT(!CanMemberData<const ZipTransformType>);

            // Validate view_interface::front()
            {
                const auto validate_front_closure = [&]<bool IsConst>() {
                    STATIC_ASSERT(CanMemberFront<ranges::_Maybe_const<IsConst, ZipTransformType>>
                                  == ranges::forward_range<ranges::_Maybe_const<IsConst, ZipTransformType>>);
                    if constexpr (CanMemberFront<ranges::_Maybe_const<IsConst, ZipTransformType>>) {
                        using transform_result_t = TransformResultType<IsConst, TransformType, RangeTypes...>;
                        same_as<transform_result_t> auto first_result =
                            maybe_as_const<IsConst>(zipped_transformed_range).front();

                        assert(first_result == *ranges::begin(transformed_elements));
                    }
                };

                validate_front_closure.template operator()<false>();
                validate_front_closure.template operator()<true>();
            }

            // Validate view_interface::back()
            {
                const auto validate_back_closure = [&]<bool IsConst>() {
                    STATIC_ASSERT(CanMemberBack<ranges::_Maybe_const<IsConst, ZipTransformType>>
                                  == (ranges::bidirectional_range<ranges::_Maybe_const<IsConst, ZipTransformType>>
                                      && ranges::common_range<ranges::_Maybe_const<IsConst, ZipTransformType>>) );
                    if constexpr (CanMemberBack<ranges::_Maybe_const<IsConst, ZipTransformType>>) {
                        using transform_result_t = TransformResultType<IsConst, TransformType, RangeTypes...>;
                        same_as<transform_result_t> auto last_result =
                            maybe_as_const<IsConst>(zipped_transformed_range).back();

                        assert(last_result == *ranges::prev(ranges::end(transformed_elements)));
                    }
                };

                validate_back_closure.template operator()<false>();
                validate_back_closure.template operator()<true>();
            }

            // Validate view_interface::operator[]
            {
                const auto validate_random_access_closure = [&]<bool IsConst>() {
                    STATIC_ASSERT(CanIndex<ranges::_Maybe_const<IsConst, ZipTransformType>>
                                  == ranges::random_access_range<ranges::_Maybe_const<IsConst, ZipTransformType>>);
                    if constexpr (CanIndex<ranges::_Maybe_const<IsConst, ZipTransformType>>) {
                        using transform_result_t = TransformResultType<IsConst, TransformType, RangeTypes...>;
                        same_as<transform_result_t> auto first_result =
                            maybe_as_const<IsConst>(zipped_transformed_range)[0];

                        assert(first_result == ranges::begin(transformed_elements)[0]);
                    }
                };

                validate_random_access_closure.template operator()<false>();
                validate_random_access_closure.template operator()<true>();
            }

            // Validate zip_transform_view::begin() and zip_transform_view::end()
            STATIC_ASSERT(CanMemberBegin<ZipTransformType>);
            STATIC_ASSERT(CanMemberEnd<ZipTransformType>);
            {
                const same_as<ranges::iterator_t<ZipTransformType>> auto begin_itr = zipped_transformed_range.begin();
                [[maybe_unused]] const same_as<ranges::sentinel_t<ZipTransformType>> auto end_sentinel =
                    zipped_transformed_range.end();

                assert(*begin_itr == *ranges::begin(transformed_elements));
                assert(static_cast<decltype(ranges::size(transformed_elements))>(
                           ranges::distance(zipped_transformed_range.begin(), zipped_transformed_range.end()))
                       == ranges::size(transformed_elements));

                STATIC_ASSERT(noexcept(zipped_transformed_range.begin()) == noexcept(declval<InnerView&>().begin())
                              && is_nothrow_move_constructible_v<ranges::iterator_t<InnerView>>);
                STATIC_ASSERT(noexcept(zipped_transformed_range.end()) == noexcept(declval<InnerView&>().end())
                              && is_nothrow_move_constructible_v<ranges::sentinel_t<InnerView>>);
            }

            STATIC_ASSERT(CanMemberBegin<const ZipTransformType> == has_const_begin_end);
            STATIC_ASSERT(CanMemberEnd<const ZipTransformType> == has_const_begin_end);
            if constexpr (has_const_begin_end) {
                const same_as<ranges::iterator_t<const ZipTransformType>> auto begin_itr =
                    as_const(zipped_transformed_range).begin();
                [[maybe_unused]] const same_as<ranges::sentinel_t<const ZipTransformType>> auto end_sentinel =
                    as_const(zipped_transformed_range).end();

                assert(*begin_itr == *ranges::begin(transformed_elements));
                assert(static_cast<decltype(ranges::size(transformed_elements))>(ranges::distance(
                           as_const(zipped_transformed_range).begin(), as_const(zipped_transformed_range).end()))
                       == ranges::size(transformed_elements));
                STATIC_ASSERT(noexcept(as_const(zipped_transformed_range).begin())
                                  == noexcept(declval<const InnerView&>().begin())
                              && is_nothrow_move_constructible_v<ranges::iterator_t<const InnerView>>);
                STATIC_ASSERT(
                    noexcept(as_const(zipped_transformed_range).end()) == noexcept(declval<const InnerView&>().end())
                    && is_nothrow_move_constructible_v<ranges::sentinel_t<const InnerView>>);
            }

            validate_iterators_sentinels<TransformType, TransformedElementsContainer,
                decltype(zipped_transformed_range), RangeTypes...>(zipped_transformed_range, transformed_elements);
            validate_iterators_sentinels<TransformType, TransformedElementsContainer,
                decltype(as_const(zipped_transformed_range)), RangeTypes...>(
                as_const(zipped_transformed_range), transformed_elements);
        }
    }

    return true;
}

#pragma warning(pop)

constexpr auto one_element_transform_closure = [](const auto& a) { return a * 5; };

constexpr auto three_element_transform_closure = [](const auto& a, const auto& b, const auto& c) { return a * b + c; };

constexpr array test_element_array_one{0, 1, 2, 3, 4, 5, 6, 7};
constexpr array test_element_array_two{5, 13, 6, -4, 12};
constexpr array test_element_array_three{6534, 23, 62, -124, 6, 42, 9};

constexpr auto single_range_transform_results_array = []() {
    auto transformed_elements_array{test_element_array_one};
    ranges::transform(transformed_elements_array, transformed_elements_array.begin(), one_element_transform_closure);

    return transformed_elements_array;
}();

constexpr auto three_range_transform_results_array = []() {
    constexpr size_t results_array_size =
        (min) ((min) (test_element_array_one.size(), test_element_array_two.size()), test_element_array_three.size());
    array<int, results_array_size> transformed_elements_array{};

    for (auto [destValue, a, b, c] : views::zip(
             transformed_elements_array, test_element_array_one, test_element_array_two, test_element_array_three)) {
        destValue = three_element_transform_closure(a, b, c);
    }

    return transformed_elements_array;
}();

// zip_transform_view<RangeTypes...> is implemented in terms of zip_view<RangeTypes...>, so it inherits
// its sensitivity towards all of the traits which zip_view<RangeTypes...> is sensitive to. This includes
// the following:
//
//   - The categories of RangeTypes...
//   - The commonality of RangeTypes...
//   - Whether or not the size() member exists for each of RangeTypes..., as it is used by ranges::size()
//   - Whether or not each range's iterator and sentinel models sized_sentinel_for
//   - The sizeof...(RangeTypes)
//
// Other conditions are irrelevant.

template <bool IsMoveOnly>
class range_type_solver {
protected:
    template <class Category, class Element, test::Sized IsSized, test::Common IsCommon, test::CanDifference Diff>
    using range_type = test::range<Category, Element, IsSized, Diff, IsCommon,
        (test::to_bool(IsCommon) ? test::CanCompare::yes
                                 : test::CanCompare{derived_from<Category, forward_iterator_tag>}),
        test::ProxyRef{derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes,
        test::Copyability::move_only>;
};

template <>
class range_type_solver<false> {
protected:
    template <class Category, class Element, test::Sized IsSized, test::Common IsCommon, test::CanDifference Diff>
    using range_type = test::range<Category, Element, IsSized, Diff, IsCommon,
        (test::to_bool(IsCommon) ? test::CanCompare::yes
                                 : test::CanCompare{derived_from<Category, forward_iterator_tag>})>;
};

template <bool IsMoveOnly, class Category, test::Sized IsSized, test::Common IsCommon, test::CanDifference Diff>
class instantiator_impl : private range_type_solver<IsMoveOnly> {
private:
    template <class OtherCategory, class Element, test::Sized OtherIsSized, test::Common OtherIsCommon,
        test::CanDifference OtherDiff>
    using range_type = typename range_type_solver<IsMoveOnly>::template range_type<OtherCategory, Element, OtherIsSized,
        OtherIsCommon, OtherDiff>;

    using standard_range_type = range_type<Category, const int, IsSized, IsCommon, Diff>;

    using differing_category_range_type =
        range_type<conditional_t<same_as<Category, input_iterator_tag>, forward_iterator_tag, input_iterator_tag>,
            const int, IsSized, IsCommon, Diff>;
    using differing_size_member_range_type            = range_type<Category, const int,
        (IsSized == test::Sized::yes ? test::Sized::no : test::Sized::yes), IsCommon, Diff>;
    using differing_is_common_range_type              = range_type<Category, const int, IsSized,
        (IsCommon == test::Common::yes ? test::Common::no : test::Common::yes), Diff>;
    using differing_iterator_sentinel_diff_range_type = range_type<Category, const int, IsSized, IsCommon,
        (Diff == test::CanDifference::yes ? test::CanDifference::no : test::CanDifference::yes)>;

    static constexpr void test_single_range() {
        standard_range_type single_range{span<const int>{test_element_array_one}};
        test_one(one_element_transform_closure, single_range_transform_results_array, single_range);
    }

    template <class DifferingRangeType>
    static constexpr void test_three_ranges() {
        DifferingRangeType first_range{span<const int>{test_element_array_one}};
        standard_range_type second_range{span<const int>{test_element_array_two}};
        standard_range_type third_range{span<const int>{test_element_array_three}};

        test_one(three_element_transform_closure, three_range_transform_results_array, first_range, second_range,
            third_range);
    }

public:
    static constexpr void call() {
        // Test the single-range use of views::zip_transform (i.e., sizeof...(RangeTypes) == 1).
        test_single_range();

        // Test three ranges with views::zip_transform with...

        // all of their traits being the same, ...
        test_three_ranges<standard_range_type>();

        // one range having a different category, ...
        test_three_ranges<differing_category_range_type>();

        // one range having a different path for ranges::size(), ...
        test_three_ranges<differing_size_member_range_type>();

        // one range having a different commonality, ...
        test_three_ranges<differing_is_common_range_type>();

        // and one range having iterators and sentinels which model sized_sentinel_for
        // differently.
        test_three_ranges<differing_iterator_sentinel_diff_range_type>();
    }
};

template <class Category, test::Sized IsSized, test::Common IsCommon, test::CanDifference Diff>
class instantiator : public instantiator_impl<false, Category, IsSized, IsCommon, Diff> {};

template <class Category, test::Sized IsSized, test::Common IsCommon, test::CanDifference Diff>
class move_only_view_instantiator : public instantiator_impl<true, Category, IsSized, IsCommon, Diff> {};

template <class Category, template <class, test::Sized, test::Common, test::CanDifference> class InstantiatorType>
constexpr bool instantiation_test_for_category() {
    using test::Sized, test::Common, test::CanDifference;

#ifndef _PREFAST_ // TRANSITION, GH-1030
    InstantiatorType<Category, Sized::no, Common::no, CanDifference::no>::call();
    InstantiatorType<Category, Sized::no, Common::no, CanDifference::yes>::call();
    InstantiatorType<Category, Sized::no, Common::yes, CanDifference::no>::call();
    InstantiatorType<Category, Sized::no, Common::yes, CanDifference::yes>::call();
    InstantiatorType<Category, Sized::yes, Common::no, CanDifference::no>::call();
    InstantiatorType<Category, Sized::yes, Common::no, CanDifference::yes>::call();
    InstantiatorType<Category, Sized::yes, Common::yes, CanDifference::no>::call();
#endif // TRANSITION, GH-1030
    InstantiatorType<Category, Sized::yes, Common::yes, CanDifference::yes>::call();

    return true;
}

template <template <class, test::Sized, test::Common, test::CanDifference> class InstantiatorType>
constexpr bool instantiation_test() {
#if defined(TEST_INPUT)
    instantiation_test_for_category<input_iterator_tag, InstantiatorType>();
#elif defined(TEST_FORWARD) // ^^^ TEST_INPUT / TEST_FORWARD vvv
    instantiation_test_for_category<forward_iterator_tag, InstantiatorType>();
#elif defined(TEST_BIDIRECTIONAL) // ^^^ TEST_FORWARD / TEST_BIDIRECTIONAL vvv
    instantiation_test_for_category<bidirectional_iterator_tag, InstantiatorType>();
#elif defined(TEST_RANDOM) // ^^^ TEST_BIDIRECTIONAL / TEST_RANDOM vvv
    instantiation_test_for_category<random_access_iterator_tag, InstantiatorType>();
#else // ^^^ TEST_RANDOM / UNKNOWN vvv
    static_assert(
        false, "ERROR: A defined test macro was never specified when executing test P2321R2_views_zip_transform!");
#endif // ^^^ UNKNOWN ^^^

    return true;
}

constexpr bool validate_empty_ranges() {
    bool was_dummy_function_called = false;

    struct DummyType {};

    const auto dummy_function_closure = [&was_dummy_function_called]() {
        was_dummy_function_called = true;
        return DummyType{};
    };

    [[maybe_unused]] const auto useless_transform_view = views::zip_transform(dummy_function_closure);
    assert(!was_dummy_function_called);

    // When no ranges are used with the std::views::zip_transform CPO, the decayed return type of
    // the provided function must not be void.
    STATIC_ASSERT(
        CanZipTransform<decltype([]() noexcept -> const auto& { return single_range_transform_results_array; })>);
    STATIC_ASSERT(!CanZipTransform<decltype([]() noexcept -> void {})>);

    return true;
}

int main() {
    // Empty RangeTypes... parameter pack
    {
        STATIC_ASSERT(validate_empty_ranges());
        validate_empty_ranges();
    }

    // Validate views
    {
        // ... copyable, single view
        constexpr span<const int> int_span{test_element_array_one};

        STATIC_ASSERT(test_one(one_element_transform_closure, single_range_transform_results_array, int_span));
        test_one(one_element_transform_closure, single_range_transform_results_array, int_span);
    }

    {
        // ... copyable, multiple views
        constexpr span<const int> first_span{test_element_array_one};
        constexpr span<const int> second_span{test_element_array_two};
        constexpr span<const int> third_span{test_element_array_three};

        STATIC_ASSERT(test_one(
            three_element_transform_closure, three_range_transform_results_array, first_span, second_span, third_span));
        test_one(
            three_element_transform_closure, three_range_transform_results_array, first_span, second_span, third_span);
    }

    {
        // ... move-only, single and multiple views
        STATIC_ASSERT(instantiation_test<move_only_view_instantiator>());
        instantiation_test<move_only_view_instantiator>();
    }

    {
        // ... immobile, single and multiple views
        STATIC_ASSERT(instantiation_test<instantiator>());
        instantiation_test<instantiator>();
    }

    // Validate non-views
    {
        STATIC_ASSERT(
            test_one(one_element_transform_closure, single_range_transform_results_array, test_element_array_one));
        test_one(one_element_transform_closure, single_range_transform_results_array, test_element_array_one);
    }
    {
        STATIC_ASSERT(test_one(three_element_transform_closure, three_range_transform_results_array,
            test_element_array_one, test_element_array_two, test_element_array_three));
        test_one(three_element_transform_closure, three_range_transform_results_array, test_element_array_one,
            test_element_array_two, test_element_array_three);
    }

    return 0;
}
