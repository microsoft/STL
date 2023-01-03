// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

template <bool Movable>
class MoveDeleter {
public:
    MoveDeleter() = default;
};

template <>
class MoveDeleter<false> {
public:
    MoveDeleter() = default;

    MoveDeleter(const MoveDeleter& rhs)            = default;
    MoveDeleter& operator=(const MoveDeleter& rhs) = default;

    MoveDeleter(MoveDeleter&& rhs) noexcept            = delete;
    MoveDeleter& operator=(MoveDeleter&& rhs) noexcept = delete;
};

template <class ValueType, bool Movable>
class TestCallback : public MoveDeleter<Movable> {
public:
    TestCallback() = default;

    ValueType operator()(const ValueType& lhs, const ValueType& rhs) const {
        return (lhs + rhs);
    }
};

template <bool Movable>
class TestCallback<void, Movable> : public MoveDeleter<Movable> {
public:
    TestCallback() = default;

    void operator()() const {}
};

template <class Function, class... RangeTypes>
concept CanZipTransform =
    requires(Function&& function, RangeTypes&&... ranges) {
        views::zip_transform(std::forward<Function>(function), std::forward<RangeTypes>(ranges)...);
    };

template <class RangeType>
using AllView = views::all_t<RangeType>;

template <bool IsConst, class TransformType, ranges::input_range... RangeTypes>
using TransformResultType = invoke_result_t<ranges::_Maybe_const<IsConst, TransformType>&,
    ranges::range_reference_t<ranges::_Maybe_const<IsConst, remove_cvref_t<RangeTypes>>>...>;

template <bool IsConst, class T>
constexpr auto& maybe_as_const(T& value) {
    if constexpr (IsConst) {
        return as_const(value);
    } else {
        return value;
    }
}

template <class TransformType_, ranges::random_access_range TransformedElementsContainer,
    ranges::input_range... RangeTypes>
constexpr bool test_one(
    TransformType_&& transform, const TransformedElementsContainer& transformed_elements, RangeTypes&&... ranges) {
    using TransformType    = remove_cvref_t<TransformType_>;
    using ZipTransformType = ranges::zip_transform_view<TransformType, AllView<RangeTypes>...>;

    constexpr bool are_views = (ranges::view<remove_cvref_t<RangeTypes>> && ...) && (sizeof...(RangeTypes) > 0);
    constexpr bool is_transform =
        is_object_v<TransformType>
        && regular_invocable<TransformType&, ranges::range_reference_t<remove_cvref_t<RangeTypes>>...>
        && _Can_reference<invoke_result_t<TransformType&, ranges::range_reference_t<remove_cvref_t<RangeTypes>>...>>;

    STATIC_ASSERT(ranges::view<ZipTransformType>);

    // Validate iterator concept
    {
        constexpr auto check_iterator_concept_closure = []<bool IsConst>() {
            using transform_result_t = TransformResultType<IsConst, TransformType, RangeTypes...>;

            if constexpr (!is_reference_v<transform_result_t>) {
                STATIC_ASSERT(ranges::input_range<ZipTransformType>);
                STATIC_ASSERT(!ranges::forward_range<ZipTransformType>);
                STATIC_ASSERT(!ranges::bidirectional_range<ZipTransformType>);
                STATIC_ASSERT(!ranges::random_access_range<ZipTransformType>);
            } else {
                constexpr auto check_iterator_tags_closure = []<class TagType>() {
                    return (
                        derived_from<typename iterator_traits<ranges::iterator_t<
                                         ranges::_Maybe_const<IsConst, remove_cvref_t<RangeTypes>>>>::iterator_category,
                            TagType>
                        && ...);
                };

                STATIC_ASSERT(ranges::input_range<ZipTransformType>);
                STATIC_ASSERT(ranges::forward_range<ZipTransformType>
                              == check_iterator_tags_closure.template operator()<forward_iterator_tag>());
                STATIC_ASSERT(ranges::bidirectional_range<ZipTransformType>
                              == check_iterator_tags_closure.template operator()<bidirectional_iterator_tag>());
                STATIC_ASSERT(ranges::random_access_range<ZipTransformType>
                              == check_iterator_tags_closure.template operator()<random_access_iterator_tag>());
            }
        };

        check_iterator_concept_closure.template operator()<false>();
        check_iterator_concept_closure.template operator()<true>();
    }

    using InnerView = ranges::zip_view<AllView<RangeTypes>...>;

    // Validate commonality
    STATIC_ASSERT(ranges::common_range<ZipTransformType> == ranges::common_range<InnerView>);

    if constexpr (should_have_const_begin_end) {
        STATIC_ASSERT(ranges::common_range<const ZipTransformType> == ranges::common_range<const InnerView>);
    }

    // Validate conditional default-initializability
    STATIC_ASSERT(
        is_default_constructible_v<ZipTransformType> == is_default_constructible_v<ranges::_Movable_box<TransformType>>
        && is_default_constructible_v<InnerView>);
    STATIC_ASSERT(is_nothrow_default_constructible_v<ZipTransformType>
                      == is_nothrow_default_constructible_v<ranges::_Movable_box<TransformType>>
                  && is_nothrow_default_constructible_v<InnerView>);

    // Validate range adaptor object
    {
        constexpr bool can_copy_construct_ranges = (!are_views || (copy_constructible<AllView<RangeTypes>> && ...));
        constexpr bool can_move_ranges           = (are_views || (movable<remove_reference_t<RangeTypes>> && ...));

        // ... with lvalue arguments
        STATIC_ASSERT(CanZipTransform<TransformType, RangeTypes&...> == can_copy_construct_ranges
                      && move_constructible<TransformType>);
        if constexpr (CanZipTransform<TransformType, RangeTypes&...>) {
            using ExpectedZipTransformType = ZipTransformType;
            constexpr bool is_noexcept     = is_nothrow_move_constructible_v<ranges::_Movable_box<TransformType>>
                                      && is_nothrow_constructible_v<InnerView, RangeTypes&...>;

            STATIC_ASSERT(same_as<decltype(views::zip_transform(std::forward<TransformType>(transform), ranges...)),
                ExpectedZipTransformType>);
            STATIC_ASSERT(
                noexcept(views::zip_transform(std::forward<TransformType>(transform), ranges...)) == is_noexcept);
        }

        // ... with const lvalue arguments
        STATIC_ASSERT(
            CanZipTransform<TransformType, const remove_reference_t<RangeTypes>&...> == can_copy_construct_ranges
            && move_constructible<TransformType>);
        if constexpr (CanZipTransform<TransformType, const remove_reference_t<RangeTypes>&...>) {
            using ExpectedZipTransformType =
                ranges::zip_transform_view<TransformType, AllView<const remove_reference_t<RangeTypes>&>...>;
            constexpr bool is_noexcept =
                is_nothrow_move_constructible_v<ranges::_Movable_box<TransformType>>
                && is_nothrow_constructible_v<InnerView, AllView<const remove_reference_t<RangeTypes>&>...>;

            STATIC_ASSERT(
                same_as<decltype(views::zip_transform(std::forward<TransformType>(transform), as_const(ranges)...)),
                    ExpectedZipTransformType>);
            STATIC_ASSERT(noexcept(views::zip_transform(std::forward<TransformType>(transform), as_const(ranges)...))
                          == is_noexcept);
        }

        // ... with rvalue arguments
        STATIC_ASSERT(CanZipTransform<TransformType, remove_reference_t<RangeTypes>> == can_move_ranges
                      && move_constructible<TransformType>);
        if constexpr (CanZipTransform<TransformType, remove_reference_t<RangeTypes>...>) {
            using ExpectedZipTransformType =
                ranges::zip_transform_view<TransformType, AllView<remove_reference_t<RangeTypes>>...>;
            constexpr bool is_noexcept =
                is_nothrow_move_constructible_v<ranges::_Movable_box<TransformType>>
                && is_nothrow_constructible_v<InnerView, AllView<remove_reference_t<RangeTypes>>...>;

            STATIC_ASSERT(
                same_as<decltype(views::zip_transform(std::forward<TransformType>(transform), std::move(ranges)...)),
                    ExpectedZipTransformType>);
            STATIC_ASSERT(noexcept(views::zip_transform(std::forward<TransformType>(transform), std::move(ranges)...))
                          == is_noexcept);
        }

        // ... with const rvalue arguments
        STATIC_ASSERT(CanZipTransform<TransformType, const remove_reference_t<RangeTypes>> == can_copy_construct_ranges
                      && move_constructible<TransformType>);
        if constexpr (CanZipTransform<TransformType, const remove_reference_t<RangeTypes>>) {
            using ExpectedZipTransformType =
                ranges::zip_transform_view<TransformType, AllView<const remove_reference_t<RangeTypes>>...>;
            constexpr bool is_noexcept =
                is_nothrow_move_constructible_v<ranges::_Movable_box<TransformType>>
                && is_nothrow_constructible_v<InnerView, AllView<const remove_reference_t<RangeTypes>>...>;

            STATIC_ASSERT(same_as<decltype(views::zip_transform(
                                      std::forward<TransformType>(transform), std::move(as_const(ranges))...)),
                ExpectedZipTransformType>);
            STATIC_ASSERT(
                noexcept(views::zip_transform(std::forward<TransformType>(transform), std::move(as_const(ranges))...))
                == is_noexcept);
        }
    }

    if constexpr (move_constructible<TransformType>) {
        // Validate deduction guide
        same_as<ZipTransformType> auto zipped_transformed_range =
            ranges::zip_transform_view{std::forward<TransformType>(transform), std::forward<RangeTypes>(ranges)...};

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
            STATIC_ASSERT(noexcept(zipped_transformed_range.size()) == noexcept(declval<const InnerView&>().size()));
        }

        const bool is_empty = ranges::empty(transformed_elements);

        // We don't want an empty results range, since we still need to do additional testing.
        assert(!is_empty);

        // Validate view_interface::empty() and view_interface::operator bool
        //
        // From here on out, we'll be re-using concepts which we already verified to reduce
        // redundancy.
        STATIC_ASSERT(CanMemberEmpty<ZipTransformType> == ranges::sized_range<ZipTransformType>
                      || ranges::forward_range<ZipTransformType>);
        if constexpr (CanMemberEmpty<ZipTransformType>) {
            assert(zipped_transformed_range.empty() == is_empty);
        }

        STATIC_ASSERT(CanMemberEmpty<const ZipTransformType> == ranges::sized_range<const ZipTransformType>
                      || ranges::forward_range<const ZipTransformType>);
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

        // Validate contents of zip-transformed range
        assert(ranges::equal(zipped_transformed_range, transformed_elements));

#pragma warning(push)
#pragma warning(disable : 4127) // Conditional Expression is Constant
        if (!(ranges::forward_range<AllView<RangeTypes>> && ...)) // intentionally not if constexpr
        {
            return false;
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
            constexpr auto validate_front_closure = [&]<bool IsConst>() {
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
            constexpr auto validate_back_closure = [&]<bool IsConst>() {
                STATIC_ASSERT(CanMemberBack<ranges::_Maybe_const<IsConst, ZipTransformType>>
                                  == ranges::bidirectional_range<ranges::_Maybe_const<IsConst, ZipTransformType>>
                              && ranges::common_range<ranges::_Maybe_const<IsConst, ZipTransformType>>);
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
            constexpr auto validate_random_access_closure = [&]<bool IsConst>() {
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
        STATIC_ASSERT(CanMemberEnd<ZipTransformType>) {
            same_as<ranges::iterator_t<ZipTransformType>> auto begin_itr    = zipped_transformed_range.begin();
            same_as<ranges::sentinel_t<ZipTransformType>> auto end_sentinel = zipped_transformed_range.end();

            assert(*begin_itr == *ranges::begin(transformed_elements));
            assert(ranges::distance(begin_itr, end_sentinel) == ranges::size(transformed_elements));
            STATIC_ASSERT(noexcept(zipped_transformed_range.begin())
                          == is_nothrow_constructible_v<ranges::iterator_t<ZipTransformType>, ZipTransformType&,
                              ranges::iterator_t<InnerView>>);

            if constexpr (ranges::common_range<ZipTransformType>) {
                STATIC_ASSERT(noexcept(zipped_transformed_range.end())
                              == is_nothrow_constructible_v<ranges::iterator_t<ZipTransformType>, ZipTransformType&,
                                  ranges::iterator_t<InnerView>>);
            } else {
                STATIC_ASSERT(
                    noexcept(zipped_transformed_range.end())
                    == is_nothrow_constructible_v<ranges::sentinel_t<ZipTransformType>, ranges::sentinel_t<InnerView>>);
            }
        }

        constexpr bool has_const_begin_end =
            ranges::range<const InnerView>
            && regular_invocable<const TransformType&, ranges::range_reference_t<const RangeTypes>...>;

        STATIC_ASSERT(CanMemberBegin<const ZipTransformType> == has_const_begin_end);
        STATIC_ASSERT(CanMemberEnd<const ZipTransformType> == has_const_begin_end);
        if constexpr (has_const_begin_end) {
            same_as<ranges::iterator_t<const ZipTransformType>> auto begin_itr =
                as_const(zipped_transformed_range).begin();
            same_as<ranges::sentinel_t<const ZipTransformType>> auto end_sentinel =
                as_const(zipped_transformed_range).end();

            assert(*begin_itr == *ranges::begin(transformed_elements));
            assert(ranges::distance(begin_itr, end_sentinel) == ranges::size(transformed_elements));
            STATIC_ASSERT(noexcept(as_const(zipped_transformed_range).begin())
                          == is_nothrow_constructible_v<ranges::iterator_t<const ZipTransformType>,
                              const ZipTransformType&, ranges::iterator_t<const InnerView>>);

            if constexpr (ranges::common_range<const ZipTransformType>) {
                STATIC_ASSERT(noexcept(as_const(zipped_transformed_range).end())
                              == is_nothrow_constructible_v<ranges::iterator_t<const ZipTransformType>,
                                  const ZipTransformType&, ranges::iterator_t<const InnerView>>);
            } else {
                STATIC_ASSERT(noexcept(as_const(zipped_transformed_range).end())
                              == is_nothrow_constructible_v<ranges::sentinel_t<const ZipTransformType>,
                                  ranges::sentinel_t<const InnerView>>);
            }
        }

        // TODO: Add testing for iterators and sentinels returned by zip_transform_view!
    }

    return true;
}

constexpr auto transform_closure = []<typename Type1, typename Type2, typename Type3>(
                                       const Type1& a, const Type2& b, const Type3& c) { return (a * b + c); };