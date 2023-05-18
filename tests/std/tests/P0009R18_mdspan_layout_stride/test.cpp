// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <mdspan>
#include <span>
#include <type_traits>

#include "test_mdspan_support.hpp"

using namespace std;

struct CmpEqual {
    template <class T, class U>
    [[nodiscard]] constexpr bool operator()(T t, U u) const noexcept {
        return cmp_equal(t, u);
    }
};

template <size_t... Extents, class IndexType, class StridesIndexType, size_t... Indices>
constexpr void do_check_members(const extents<IndexType, Extents...>& ext,
    const array<StridesIndexType, sizeof...(Extents)>& strs, index_sequence<Indices...>) {
    using Ext     = extents<IndexType, Extents...>;
    using Strides = array<IndexType, sizeof...(Extents)>;
    using Mapping = layout_stride::mapping<Ext>;

    // layout_stride meets the layout mapping policy requirements and is a trivial type
    static_assert(check_layout_mapping_policy_requirements<layout_stride, Ext>());
    static_assert(is_trivial_v<layout_stride>);

    // layout_stride::mapping<Ext> is a trivially copyable type that models regular for each Ext
    static_assert(is_trivially_copyable_v<Mapping>);
    static_assert(regular<Mapping>);

    // Check member types
    static_assert(same_as<typename Mapping::extents_type, Ext>);
    static_assert(same_as<typename Mapping::index_type, typename Ext::index_type>);
    static_assert(same_as<typename Mapping::size_type, typename Ext::size_type>);
    static_assert(same_as<typename Mapping::rank_type, typename Ext::rank_type>);
    static_assert(same_as<typename Mapping::layout_type, layout_stride>);

    { // Check default and copy constructor
        Mapping m;
        const Mapping cpy = m;
        const layout_right::mapping<Ext> right_mapping;
        assert(m == right_mapping);
        assert(cpy == m);
        static_assert(is_nothrow_default_constructible_v<Mapping>);
        static_assert(is_nothrow_copy_constructible_v<Mapping>);
    }

    { // Check construction from extents_type and array
        Mapping m{ext, strs};
        assert(m.extents() == ext);
        assert(ranges::equal(m.strides(), strs, CmpEqual{}));
        static_assert(is_nothrow_constructible_v<Mapping, Ext, Strides>);
        // Other tests are defined in 'check_construction_from_extents_and_array' function
    }

    { // Check construction from extents_type and span
        using Span = span<const StridesIndexType, sizeof...(Extents)>;
        Mapping m{ext, Span{strs}};
        assert(m.extents() == ext);
        assert(ranges::equal(m.strides(), strs, CmpEqual{}));
        static_assert(is_nothrow_constructible_v<Mapping, Ext, Span>);
        // Other tests are defined in 'check_construction_from_extents_and_array' function
    }

    using OtherIndexType = long long;
    using Ext2           = extents<OtherIndexType, Extents...>;
    using Mapping2       = layout_stride::mapping<Ext2>;

    { // Check construction from other mappings
        Mapping m1{ext, strs};
        Mapping2 m2{m1};
        assert(m1 == m2);
        static_assert(is_nothrow_constructible_v<Mapping2, Mapping>);
        // Other tests are defined in 'check_construction_from_other_mappings' function
    }

    Mapping m{ext, strs}; // For later use

    { // Check 'extents' function
        same_as<const Ext&> decltype(auto) ext2 = m.extents();
        assert(ext2 == ext);
        static_assert(noexcept(m.extents()));
    }

    { // Check 'strides' function
        same_as<Strides> decltype(auto) strs2 = m.strides();
        assert(ranges::equal(strs2, strs, CmpEqual{}));
        static_assert(noexcept(m.strides()));
    }

    { // Check 'required_span_size' function
        if (((ext.extent(Indices) == 0) || ...)) {
            assert(m.required_span_size() == 0);
        } else {
            const IndexType expected_value =
                static_cast<IndexType>((((ext.extent(Indices) - 1) * strs[Indices]) + ... + 1));
            assert(m.required_span_size() == expected_value);
        }
        static_assert(noexcept(m.required_span_size()));
        // Other tests are defined in 'check_required_span_size' function
    }

    // Call operator() is tested in 'check_call_operator' function

    { // Check 'is_always_[unique/exhaustive/strided]' functions
        static_assert(Mapping::is_always_unique());
        static_assert(!Mapping::is_always_exhaustive());
        static_assert(Mapping::is_always_strided());
    }

    { // Check 'is_[unique/strided]' functions
        static_assert(Mapping::is_unique());
        static_assert(Mapping::is_strided());
        // Tests of 'is_exhaustive' are defined in 'check_is_exhaustive' function
    }

    { // Check 'stride' function
        for (size_t i = 0; i < strs.size(); ++i) {
            same_as<IndexType> decltype(auto) s = m.stride(i);
#pragma warning(push)
#pragma warning(disable : 28020) // TRANSITION, DevCom-923103
            assert(cmp_equal(strs[i], s));
#pragma warning(pop)
        }
    }

    { // Check comparisons
        assert(m == m);
        assert(!(m != m));
        // Other tests are defined in 'check_comparisons' function
    }
}

template <class StridesIndexType, class IndexType, size_t... Extents>
constexpr void check_members_with_different_strides_index_type(
    extents<IndexType, Extents...> ext, const array<int, sizeof...(Extents)>& strides) {
    array<StridesIndexType, sizeof...(Extents)> test_strides;
    ranges::transform(strides, test_strides.begin(), [](auto i) { return static_cast<StridesIndexType>(i); });
    do_check_members<Extents...>(ext, test_strides, make_index_sequence<sizeof...(Extents)>{});
}

template <class IndexType, size_t... Extents>
constexpr void check_members(extents<IndexType, Extents...> ext, const array<int, sizeof...(Extents)>& strides) {
    // Check signed strides
    check_members_with_different_strides_index_type<signed char>(ext, strides);
    check_members_with_different_strides_index_type<short>(ext, strides);
    check_members_with_different_strides_index_type<int>(ext, strides);
    check_members_with_different_strides_index_type<long>(ext, strides);
    check_members_with_different_strides_index_type<long long>(ext, strides);

    // Check unsigned strides
    check_members_with_different_strides_index_type<unsigned char>(ext, strides);
    check_members_with_different_strides_index_type<unsigned short>(ext, strides);
    check_members_with_different_strides_index_type<unsigned int>(ext, strides);
    check_members_with_different_strides_index_type<unsigned long>(ext, strides);
    check_members_with_different_strides_index_type<unsigned long long>(ext, strides);
}

constexpr void check_construction_from_extents_and_array() {
    // Check invalid construction
    using Mapping = layout_stride::mapping<extents<int, 3, 3>>;
    static_assert(!is_constructible_v<Mapping, extents<int, 2, 3>, array<int, 2>>);
    static_assert(!is_constructible_v<Mapping, extents<int, 3, 3>, array<int, 3>>);
    static_assert(!is_constructible_v<Mapping, dextents<int, 2>, array<int, 2>>);
    static_assert(!is_constructible_v<Mapping, dextents<int, 2>, array<int, 3>>);
    static_assert(!is_constructible_v<Mapping, extents<int, 2, 3>, span<int, 2>>);
    static_assert(!is_constructible_v<Mapping, extents<int, 3, 3>, span<int, 3>>);
    static_assert(!is_constructible_v<Mapping, dextents<int, 2>, span<int, 2>>);
    static_assert(!is_constructible_v<Mapping, dextents<int, 2>, span<int, 3>>);
    static_assert(!is_constructible_v<Mapping, extents<int, 3, 3>, array<NonConvertibleToAnything, 2>>);
    static_assert(!is_constructible_v<Mapping, extents<int, 3, 3>, span<NonConvertibleToAnything, 2>>);
    static_assert(!is_constructible_v<Mapping, extents<int, 3, 3>, array<ConvertibleToInt<int, IsNothrow::no>, 2>>);
    static_assert(!is_constructible_v<Mapping, extents<int, 3, 3>, span<ConvertibleToInt<int, IsNothrow::no>, 2>>);
}

constexpr void check_construction_from_other_mappings() {
    { // Check construction
        using Mapping = layout_stride::mapping<extents<int, 4, 4>>;
        static_assert(is_constructible_v<Mapping, layout_left::mapping<extents<int, 4, 4>>>);
        static_assert(is_constructible_v<Mapping, layout_right::mapping<extents<int, 4, 4>>>);
        static_assert(is_constructible_v<Mapping, layout_left::mapping<extents<long, 4, 4>>>);
        static_assert(is_constructible_v<Mapping, layout_right::mapping<extents<long, 4, 4>>>);
        static_assert(is_constructible_v<Mapping, layout_stride::mapping<extents<long, 4, 4>>>);
    }

    { // Check invalid construction
        using Mapping = layout_stride::mapping<extents<int, 4, 4>>;
        static_assert(!is_constructible_v<Mapping, layout_left::mapping<extents<int, 4, 3>>>);
        static_assert(!is_constructible_v<Mapping, layout_right::mapping<extents<int, 3, 4>>>);
        static_assert(!is_constructible_v<Mapping, layout_left::mapping<extents<long, 4, 4, 4>>>);
        static_assert(!is_constructible_v<Mapping, layout_right::mapping<extents<long, 3, 3>>>);
        static_assert(!is_constructible_v<Mapping, layout_stride::mapping<extents<int, 4, 4, 3>>>);
        // TRANSITION, Check other kinds of invalid construction (requires new helper types)
    }

    { // Check construction from layout_left::mapping
        layout_left::mapping<dextents<int, 3>> left_mapping{dextents<int, 3>{4, 3, 2}};
        layout_stride::mapping<dextents<int, 3>> strided_mapping{left_mapping};
        assert(ranges::equal(strided_mapping.strides(), array{1, 4, 12}, CmpEqual{}));
    }

    { // Check construction from layout_right::mapping
        layout_right::mapping<dextents<int, 3>> right_mapping{dextents<int, 3>{4, 3, 2}};
        layout_stride::mapping<dextents<int, 3>> strided_mapping{right_mapping};
        assert(ranges::equal(strided_mapping.strides(), array{6, 2, 1}, CmpEqual{}));
    }
}

constexpr void check_required_span_size() {
    { // Check [mdspan.layout.stride.expo]/1.1: Ext::rank() == 0
        using M1 = layout_stride::mapping<extents<int>>;
        static_assert(M1{}.required_span_size() == 1);

        layout_stride::mapping<extents<int>> m2;
        assert(m2.required_span_size() == 1);
    }

    { // Check [mdspan.layout.stride.expo]/1.2: size of the multidimensional index space e is 0
        using M1 = layout_stride::mapping<extents<int, 3, 3, 0, 3>>;
        static_assert(M1{}.required_span_size() == 0);

        layout_stride::mapping<dextents<int, 4>> m2{dextents<int, 4>{3, 0, 3, 3}, array{1, 3, 1, 1}};
        assert(m2.required_span_size() == 0);
    }

    { // Check [mdspan.layout.stride.expo]/1.3: final case
        using M1 = layout_stride::mapping<extents<int, 3, 4, 3>>;
        static_assert(M1{}.required_span_size() == 36);

        layout_stride::mapping<dextents<int, 3>> m2{dextents<int, 3>{4, 3, 4}, array{1, 4, 12}};
        assert(m2.required_span_size() == 48);
    }
}

constexpr void check_is_exhaustive() {
    { // Check exhaustive mappings (all possibilities)
        using E = extents<int, 2, 3, 5>;
        assert((layout_stride::mapping<E>{E{}, array{1, 2, 6}}.is_exhaustive()));
        assert((layout_stride::mapping<E>{E{}, array{1, 10, 2}}.is_exhaustive()));
        assert((layout_stride::mapping<E>{E{}, array{3, 1, 6}}.is_exhaustive()));
        assert((layout_stride::mapping<E>{E{}, array{15, 1, 3}}.is_exhaustive()));
        assert((layout_stride::mapping<E>{E{}, array{5, 10, 1}}.is_exhaustive()));
        assert((layout_stride::mapping<E>{E{}, array{15, 5, 1}}.is_exhaustive()));
    }

    { // Check non-exhaustive mappings
        using E = extents<int, 2, 5, 8>;
        assert((!layout_stride::mapping<E>{E{}, array{1, 2, 12}}.is_exhaustive()));
        assert((!layout_stride::mapping<E>{E{}, array{8, 18, 1}}.is_exhaustive()));
        assert((!layout_stride::mapping<E>{E{}, array{5, 1, 12}}.is_exhaustive()));
    }
}

constexpr void check_call_operator() {
    { // Check call with invalid amount of indices
        using Mapping = layout_stride::mapping<dextents<int, 3>>;
        static_assert(!CheckCallOperatorOfLayoutMapping<Mapping, int>);
        static_assert(!CheckCallOperatorOfLayoutMapping<Mapping, int, int>);
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, int, int, int>);
        static_assert(!CheckCallOperatorOfLayoutMapping<Mapping, int, int, int, int>);
    }

    { // Check call with invalid types
        using Mapping = layout_stride::mapping<dextents<long, 2>>;
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, long, long>);
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, long, short>);
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, long, ConvertibleToInt<long>>);
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, long, ConvertibleToInt<short>>);
        static_assert(!CheckCallOperatorOfLayoutMapping<Mapping, long, NonConvertibleToAnything>);
    }

    { // Check call with types that might throw during conversion
        using Mapping = layout_stride::mapping<dextents<long long, 1>>;
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, ConvertibleToInt<long long, IsNothrow::yes>>);
        static_assert(!CheckCallOperatorOfLayoutMapping<Mapping, ConvertibleToInt<long long, IsNothrow::no>>);
    }

    { // Check various mappings
        layout_stride::mapping<extents<short>> m1;
        assert(m1() == 0);

        layout_stride::mapping<dextents<int, 1>> m2{dextents<int, 1>{4}, array{1}};
        assert(m2(0) == 0);
        assert(m2(1) == 1);
        assert(m2(2) == 2);
        assert(m2(3) == 3);

        layout_stride::mapping<extents<long, 4, 5>> m3{{}, array{1, 5}}; // non-exhaustive mapping
        assert(!m3.is_exhaustive());
        assert(m3(0, 0) == 0);
        assert(m3(0, 1) == 5);
        assert(m3(0, 2) == 10);
        assert(m3(0, 3) == 15);
        assert(m3(0, 4) == 20);
        assert(m3(1, 0) == 1);
        assert(m3(1, 1) == 6);
        assert(m3(1, 2) == 11);
        assert(m3(1, 3) == 16);
        assert(m3(1, 4) == 21);
        assert(m3(2, 0) == 2);
        assert(m3(2, 1) == 7);
        assert(m3(2, 2) == 12);
        assert(m3(2, 3) == 17);
        assert(m3(3, 0) == 3);
        assert(m3(3, 1) == 8);
        assert(m3(3, 2) == 13);
        assert(m3(3, 4) == 23);

        layout_stride::mapping<extents<unsigned char, 2, 3, 5>> m4{{}, array{15, 1, 3}}; // exhaustive mapping
        assert(m4.is_exhaustive());
        assert(m4(0, 0, 0) == 0);
        assert(m4(0, 0, 1) == 3);
        assert(m4(0, 1, 0) == 1);
        assert(m4(0, 1, 1) == 4);
        assert(m4(1, 0, 0) == 15);
        assert(m4(1, 0, 1) == 18);
        assert(m4(1, 1, 0) == 16);
        assert(m4(1, 1, 1) == 19);
        assert(m4(1, 2, 4) == 29);
    }
}

constexpr void check_comparisons() {
    using E                    = extents<int, 2, 3>;
    using StaticStrideMapping  = layout_stride::mapping<E>;
    using DynamicStrideMapping = layout_stride::mapping<dextents<int, 2>>;
    using RightMapping         = layout_right::mapping<E>;
    using LeftMapping          = layout_left::mapping<E>;

    { // Check equality_comparable_with concept
        static_assert(equality_comparable_with<StaticStrideMapping, DynamicStrideMapping>);
        static_assert(equality_comparable_with<StaticStrideMapping, RightMapping>);
        static_assert(equality_comparable_with<StaticStrideMapping, LeftMapping>);
        static_assert(equality_comparable_with<DynamicStrideMapping, RightMapping>);
        static_assert(equality_comparable_with<DynamicStrideMapping, LeftMapping>);
        static_assert(!equality_comparable_with<DynamicStrideMapping, layout_stride::mapping<extents<int, 2, 3, 4>>>);
        static_assert(!equality_comparable_with<StaticStrideMapping, layout_stride::mapping<dextents<int, 3>>>);
        static_assert(!equality_comparable_with<DynamicStrideMapping, layout_right::mapping<extents<int, 2>>>);
        static_assert(!equality_comparable_with<StaticStrideMapping, layout_left::mapping<dextents<int, 1>>>);
        // TRANSITION, Check other constraints: [mdspan.layout.stride.obs]/6.1, 6.3
    }

    { // Check correctness: layout_stride::mapping with layout_stride::mapping
        StaticStrideMapping m1{E{}, array{3, 1}};
        DynamicStrideMapping m2{dextents<int, 2>{2, 3}, array{3, 1}};
        assert(m1 == m2); // extents are equal, OFFSET(rhs) == 0, strides are equal

        DynamicStrideMapping m3{dextents<int, 2>{2, 3}, array{1, 2}};
        assert(m1 != m3); // extents are equal, OFFSET(rhs) == 0, strides are not equal
        assert(m2 != m3); // ditto

        DynamicStrideMapping m4{dextents<int, 2>{1, 3}, array{3, 1}};
        assert(m1 != m4); // extents are not equal, OFFSET(rhs) == 0, strides are equal
        assert(m2 != m4); // ditto
        assert(m3 != m4); // extents are not equal, OFFSET(rhs) == 0, strides are not equal

        // NB: OFFSET(layout_stride::mapping) is always equal to 0
    }

    { // Check correctness: layout_stride::mapping with layout_left::mapping
        LeftMapping m1;
        StaticStrideMapping m2{E{}, array{1, 2}};
        assert(m1 == m2); // extents are equal, OFFSET(rhs) == 0, strides are equal

        DynamicStrideMapping m3{dextents<int, 2>{2, 3}, array{3, 1}};
        assert(m1 != m3); // extents are equal, OFFSET(rhs) == 0, strides are not equal
        assert(m2 != m3); // ditto

        DynamicStrideMapping m4{dextents<int, 2>{2, 1}, array{1, 2}};
        assert(m1 != m4); // extents are not equal, OFFSET(rhs) == 0, strides are equal
        assert(m2 != m4); // ditto
        assert(m3 != m4); // extents are not equal, OFFSET(rhs) == 0, strides are not equal

        // NB: OFFSET(layout_left::mapping) is always equal to 0
    }

    { // Check correctness: layout_stride::mapping with layout_right::mapping
        RightMapping m1;
        StaticStrideMapping m2{E{}, array{3, 1}};
        assert(m1 == m2); // extents are equal, OFFSET(rhs) == 0, strides are equal

        DynamicStrideMapping m3{dextents<int, 2>{2, 3}, array{1, 2}};
        assert(m1 != m3); // extents are equal, OFFSET(rhs) == 0, strides are not equal
        assert(m2 != m3); // ditto

        DynamicStrideMapping m4{dextents<int, 2>{1, 3}, array{3, 1}};
        assert(m1 != m4); // extents are not equal, OFFSET(rhs) == 0, strides are equal
        assert(m2 != m4); // ditto
        assert(m3 != m4); // extents are not equal, OFFSET(rhs) == 0, strides are not equal

        // NB: OFFSET(layout_right::mapping) is always equal to 0
    }

    // TRANSITION, Check comparisons with custom layout mapping
}

constexpr void check_correctness() {
    { // empty extents
        const array<int, 0> vals{};
        mdspan<const int, extents<int>, layout_stride> nothing{vals.data(), {}};
        assert(nothing.size() == 1);
    }

    { // regular vector
        using E = extents<int, 3>;
        const array vals{1, 2, 3};
        layout_stride::mapping<E> m{E{}, array{1}};
        mdspan<const int, extents<int, 3>, layout_stride> vec{vals.data(), m};

        // TRANSITION, use operator[]
        assert(vec(0) == 1);
        assert(vec(1) == 2);
        assert(vec(2) == 3);
    }

    { // 2x3 matrix with row-major order
        using E = extents<int, 2, 3>;
        const array vals{1, 2, 3, 4, 5, 6};
        layout_stride::mapping<E> m{E{}, array{3, 1}};
        mdspan<const int, E, layout_stride> matrix{vals.data(), m};

        // TRANSITION, use operator[]
        assert(matrix(0, 0) == 1);
        assert(matrix(0, 1) == 2);
        assert(matrix(0, 2) == 3);
        assert(matrix(1, 0) == 4);
        assert(matrix(1, 1) == 5);
        assert(matrix(1, 2) == 6);
    }

    { // 3x2x2 tensor
        using E = extents<int, 3, 2, 2>;
        const array vals{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
        layout_stride::mapping<E> m{E{}, array{8, 1, 6}}; // non-exhaustive mapping
        assert(!m.is_exhaustive());
        mdspan<const int, E, layout_stride> tensor{vals.data(), m};

        // TRANSITION, use operator[]
        assert(tensor(0, 0, 0) == 0);
        assert(tensor(0, 0, 1) == 6);
        assert(tensor(0, 1, 0) == 1);
        assert(tensor(0, 1, 1) == 7);
        assert(tensor(1, 0, 0) == 8);
        assert(tensor(1, 0, 1) == 14);
        assert(tensor(1, 1, 0) == 9);
        assert(tensor(1, 1, 1) == 15);
        assert(tensor(2, 0, 0) == 16);
        assert(tensor(2, 0, 1) == 22);
        assert(tensor(2, 1, 0) == 17);
        assert(tensor(2, 1, 1) == 23);
    }

    { // 2x3x3x2 tensor
        using E = extents<int, 2, 3, 3, 2>;
        const array vals{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
            26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
        layout_stride::mapping<E> m{E{}, array{18, 1, 3, 9}}; // exhaustive mapping
        assert(m.is_exhaustive());
        mdspan<const int, E, layout_stride> tensor{vals.data(), m};

        // TRANSITION, use operator[]
        assert(tensor(0, 0, 0, 0) == 0);
        assert(tensor(0, 0, 0, 1) == 9);
        assert(tensor(0, 0, 1, 0) == 3);
        assert(tensor(0, 0, 1, 1) == 12);
        assert(tensor(0, 1, 0, 0) == 1);
        assert(tensor(0, 1, 0, 1) == 10);
        assert(tensor(0, 1, 1, 0) == 4);
        assert(tensor(0, 1, 1, 1) == 13);
        assert(tensor(1, 0, 0, 0) == 18);
        assert(tensor(1, 0, 0, 1) == 27);
        assert(tensor(1, 0, 1, 0) == 21);
        assert(tensor(1, 0, 1, 1) == 30);
        assert(tensor(1, 1, 0, 0) == 19);
        assert(tensor(1, 1, 0, 1) == 28);
        assert(tensor(1, 1, 1, 0) == 22);
        assert(tensor(1, 1, 1, 1) == 31);
        assert(tensor(0, 2, 2, 0) == 8);
        assert(tensor(1, 2, 2, 1) == 35);
    }
}

constexpr void check_ctad() {
    using E = extents<size_t, 2, 3>;
    E e;

    { // E::index_type and array::value_type are the same
        array<size_t, 2> a{1, 2};
        layout_stride::mapping m1{e, a};
        static_assert(same_as<decltype(m1), layout_stride::mapping<E>>);
        assert(m1.extents() == e);
        assert(m1.strides() == a);

        span s{a};
        layout_stride::mapping m2{e, s};
        static_assert(same_as<decltype(m2), layout_stride::mapping<E>>);
        assert(m2.extents() == e);
        assert(m2.strides() == a);
    }

    { // E::index_type and array::value_type are different
        array<int, 2> a{1, 2};
        layout_stride::mapping m1{e, a};
        static_assert(same_as<decltype(m1), layout_stride::mapping<E>>);
        assert(m1.extents() == e);
        assert(ranges::equal(m1.strides(), a, CmpEqual{}));

        span s{a};
        layout_stride::mapping m2{e, s};
        static_assert(same_as<decltype(m2), layout_stride::mapping<E>>);
        assert(m2.extents() == e);
        assert(ranges::equal(m2.strides(), a, CmpEqual{}));
    }
}

constexpr bool test() {
    // Check signed integers
    check_members(extents<signed char, 5>{5}, array{1});
    check_members(extents<short, 6, 7>{}, array{1, 6});
    check_members(extents<int, 3, dynamic_extent>{3}, array{1, 3});
    check_members(extents<long, 4>{}, array{1});
    check_members(extents<long long, 3, 2, dynamic_extent>{3}, array{1, 3, 6});

    // Check unsigned integers
    check_members(extents<unsigned char, 5>{5}, array{1});
    check_members(extents<unsigned short, 6, 7>{}, array{1, 6});
    check_members(extents<unsigned int, 3, dynamic_extent>{3}, array{1, 3});
    check_members(extents<unsigned long, 4>{}, array{1});
    check_members(extents<unsigned long long, 3, 2, dynamic_extent>{3}, array{1, 3, 6});

    check_construction_from_extents_and_array();
    check_construction_from_other_mappings();
    check_required_span_size();
    check_is_exhaustive();
    check_call_operator();
    check_comparisons();
    check_correctness();
    check_ctad();

    return true;
}

int main() {
    static_assert(test());
    test();
}
