// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <mdspan>
#include <span>
#include <type_traits>

#include "test_mdspan_support.hpp"

using namespace std;

template <class IndexType, size_t... Extents, size_t... Indices>
constexpr void do_check_members(const extents<IndexType, Extents...>& ext, index_sequence<Indices...>) {
    using Ext     = extents<IndexType, Extents...>;
    using Mapping = layout_right::mapping<Ext>;

    // layout_right meets the layout mapping policy requirements and is a trivial type
    static_assert(check_layout_mapping_policy_requirements<layout_right, Ext>());
    static_assert(is_trivial_v<layout_right>);

    // layout_right::mapping<Ext> is a trivially copyable type that models regular for each Ext
    static_assert(is_trivially_copyable_v<Mapping>);
    static_assert(regular<Mapping>);

    // Check member types
    static_assert(same_as<typename Mapping::extents_type, Ext>);
    static_assert(same_as<typename Mapping::index_type, typename Ext::index_type>);
    static_assert(same_as<typename Mapping::size_type, typename Ext::size_type>);
    static_assert(same_as<typename Mapping::rank_type, typename Ext::rank_type>);
    static_assert(same_as<typename Mapping::layout_type, layout_right>);

    { // Check default and copy constructor
        Mapping m;
        Mapping cpy = m;
        assert(cpy == m);
        static_assert(is_nothrow_default_constructible_v<Mapping>);
        static_assert(is_nothrow_copy_constructible_v<Mapping>);
    }

    { // Check construction from extents_type
        Mapping m{ext};
        assert(m.extents() == ext);
        static_assert(is_nothrow_constructible_v<Mapping, Ext>);
    }

    using OtherIndexType = long long;
    using Ext2           = extents<OtherIndexType, Extents...>;
    using Mapping2       = layout_right::mapping<Ext2>;

    { // Check construction from other layout_right::mapping
        Mapping m1{ext};
        Mapping2 m2{m1};
        assert(m1 == m2);
        static_assert(is_nothrow_constructible_v<Mapping2, Mapping>);
        // Other tests are defined in 'check_construction_from_other_right_mapping' function
    }

    { // Check construction from layout_left::mapping
        using LeftMapping = layout_left::mapping<Ext>;
        if constexpr (Ext::rank() <= 1) {
            LeftMapping left_mapping{ext};
            [[maybe_unused]] Mapping m1{left_mapping};
            [[maybe_unused]] Mapping2 m2{left_mapping};
            assert(m1 == m2);
            static_assert(is_nothrow_constructible_v<Mapping, LeftMapping>);
            static_assert(is_nothrow_constructible_v<Mapping2, LeftMapping>);
        } else {
            static_assert(!is_constructible_v<Mapping, LeftMapping>);
            static_assert(!is_constructible_v<Mapping2, LeftMapping>);
        }
        // Other tests are defined in 'check_construction_from_other_left_mapping' function
    }

    { // Check construction from layout_stride::mapping
        array<IndexType, Ext::rank()> strides{};
        if constexpr (Ext::rank() > 0) {
            strides.back() = 1;
            for (size_t i = Ext::rank() - 1; i-- > 0;) {
                strides[i] = static_cast<IndexType>(strides[i + 1] * ext.extent(i + 1));
            }
        }

        using StrideMapping = layout_stride::mapping<Ext>;
        StrideMapping stride_mapping{ext, strides};
        [[maybe_unused]] Mapping m{stride_mapping};
        // Other tests are defined in 'check_construction_from_other_stride_mapping' function
    }

    Mapping m{ext}; // For later use

    { // Check 'extents' function
        assert(m.extents() == ext);
        static_assert(noexcept(m.extents()));
    }

    { // Check 'required_span_size' function
        const IndexType expected_value = static_cast<IndexType>((ext.extent(Indices) * ... * 1));
        assert(m.required_span_size() == expected_value);
        static_assert(noexcept(m.required_span_size()));
    }

    { // Check operator()
        assert(m(((void) Indices, 0)...) == 0);
        assert(m((ext.extent(Indices) - 1)...) == static_cast<IndexType>((ext.extent(Indices) * ... * 1)) - 1);
        static_assert(noexcept(m(((void) Indices, 0)...)));
        static_assert(noexcept(m((ext.extent(Indices) - 1)...)));
        // Other tests are defined in 'check_call_operator' function
    }

    { // Check 'is_always_[unique/exhaustive/strided]' functions
        static_assert(Mapping::is_always_unique());
        static_assert(Mapping::is_always_exhaustive());
        static_assert(Mapping::is_always_strided());
    }

    { // Check 'is_[unique/exhaustive/strided]' functions
        static_assert(Mapping::is_unique());
        static_assert(Mapping::is_exhaustive());
        static_assert(Mapping::is_strided());
    }

    if constexpr (Ext::rank() > 0) { // Check 'stride' function
        const IndexType expected_stride0 = static_cast<IndexType>((ext.extent(Indices) * ... * 1) / ext.extent(0));
        assert(m.stride(0) == expected_stride0);
        assert(m.stride(Ext::rank() - 1) == 1);
        static_assert(noexcept(m.stride(Ext::rank() - 1)));
        static_assert(noexcept(m.stride(0)));
    } else {
        static_assert(!CheckStrideMemberFunction<Mapping>);
    }

    { // Check comparisons
        assert(m == m);
        assert(!(m != m));
        // Other tests are defined in 'check_comparisons' function
    }
}

template <class IndexType, size_t... Extents>
constexpr void check_members(extents<IndexType, Extents...> ext) {
    do_check_members<IndexType, Extents...>(ext, make_index_sequence<sizeof...(Extents)>{});
}

constexpr void check_construction_from_other_right_mapping() {
    { // Check invalid construction
        using Mapping = layout_right::mapping<extents<int, 3, 3>>;
        static_assert(!is_constructible_v<Mapping, layout_right::mapping<extents<int, 3>>>);
        static_assert(!is_constructible_v<Mapping, layout_right::mapping<extents<int, 3, 3, 3>>>);
    }

    { // Check implicit conversions
        static_assert(!NotImplicitlyConstructibleFrom<layout_right::mapping<extents<int, 3>>,
                      layout_right::mapping<extents<int, 3>>>);
        static_assert(NotImplicitlyConstructibleFrom<layout_right::mapping<extents<int, 3>>,
            layout_right::mapping<extents<long long, 3>>>);
        static_assert(NotImplicitlyConstructibleFrom<layout_right::mapping<extents<int, 3, 3>>,
            layout_right::mapping<extents<int, dynamic_extent, 3>>>);
        static_assert(NotImplicitlyConstructibleFrom<layout_right::mapping<extents<int, 3, 3>>,
            layout_right::mapping<extents<int, dynamic_extent, dynamic_extent>>>);
    }
}

constexpr void check_construction_from_other_left_mapping() {
    { // Check construction from layout_left::mapping<E> with various values of E::rank()
        static_assert(
            is_constructible_v<layout_right::mapping<dextents<int, 0>>, layout_left::mapping<dextents<int, 0>>>);
        static_assert(
            is_constructible_v<layout_right::mapping<dextents<int, 1>>, layout_left::mapping<dextents<int, 1>>>);
        static_assert(
            !is_constructible_v<layout_right::mapping<dextents<int, 2>>, layout_left::mapping<dextents<int, 2>>>);
        static_assert(
            !is_constructible_v<layout_right::mapping<dextents<int, 3>>, layout_left::mapping<dextents<int, 3>>>);
    }

    { // Check construction from layout_left::mapping<E> when E is invalid
        using Mapping = layout_right::mapping<extents<int, 3>>;
        static_assert(!is_constructible_v<Mapping, layout_left::mapping<extents<int, 1>>>);
        static_assert(!is_constructible_v<Mapping, layout_left::mapping<extents<int, 2>>>);
    }

    { // Check implicit conversions
        using Mapping = layout_right::mapping<extents<int, 4>>;
        static_assert(!NotImplicitlyConstructibleFrom<Mapping, layout_left::mapping<extents<int, 4>>>);
        static_assert(NotImplicitlyConstructibleFrom<Mapping, layout_left::mapping<extents<long long, 4>>>);
        static_assert(NotImplicitlyConstructibleFrom<Mapping, layout_left::mapping<extents<int, dynamic_extent>>>);
    }
}

constexpr void check_construction_from_other_stride_mapping() {
    { // Check construction from layout_stride::mapping<E> with various values of E::rank()
        static_assert(
            is_constructible_v<layout_right::mapping<dextents<int, 0>>, layout_stride::mapping<dextents<int, 0>>>);
        static_assert(
            is_constructible_v<layout_right::mapping<dextents<int, 1>>, layout_stride::mapping<dextents<int, 1>>>);
        static_assert(
            is_constructible_v<layout_right::mapping<dextents<int, 2>>, layout_stride::mapping<dextents<int, 2>>>);
        static_assert(
            is_constructible_v<layout_right::mapping<dextents<int, 3>>, layout_stride::mapping<dextents<int, 3>>>);
    }

    { // Check construction from layout_stride::mapping<E> when E is invalid
        using Mapping = layout_right::mapping<extents<int, 3>>;
        static_assert(!is_constructible_v<Mapping, layout_stride::mapping<extents<int, 1>>>);
        static_assert(!is_constructible_v<Mapping, layout_stride::mapping<extents<int, 2>>>);
    }

    { // Check correctness
        using Ext = extents<int, 4, 3, 2, 3, 4>;
        layout_stride::mapping<Ext> stride_mapping{Ext{}, array{72, 24, 12, 4, 1}};
        [[maybe_unused]] layout_right::mapping<Ext> m{stride_mapping};
    }

    { // Check implicit conversions
        static_assert(
            !NotImplicitlyConstructibleFrom<layout_right::mapping<extents<int>>, layout_stride::mapping<extents<int>>>);
        static_assert(NotImplicitlyConstructibleFrom<layout_right::mapping<extents<int, 3>>,
            layout_stride::mapping<extents<int, 3>>>);
        static_assert(NotImplicitlyConstructibleFrom<layout_right::mapping<extents<int, 3>>,
            layout_stride::mapping<extents<long long, 3>>>);
        static_assert(NotImplicitlyConstructibleFrom<layout_right::mapping<extents<int, 3>>,
            layout_stride::mapping<extents<int, dynamic_extent>>>);
    }
}

constexpr void check_call_operator() {
    { // Check call with invalid amount of indices
        using Mapping = layout_right::mapping<dextents<int, 3>>;
        static_assert(!CheckCallOperatorOfLayoutMapping<Mapping, int>);
        static_assert(!CheckCallOperatorOfLayoutMapping<Mapping, int, int>);
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, int, int, int>);
        static_assert(!CheckCallOperatorOfLayoutMapping<Mapping, int, int, int, int>);
    }

    { // Check call with invalid types
        using Mapping = layout_right::mapping<dextents<long, 2>>;
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, long, long>);
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, long, short>);
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, long, ConvertibleToInt<long>>);
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, long, ConvertibleToInt<short>>);
        static_assert(!CheckCallOperatorOfLayoutMapping<Mapping, long, NonConvertibleToAnything>);
    }

    { // Check call with types that might throw during conversion
        using Mapping = layout_right::mapping<dextents<long long, 1>>;
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, ConvertibleToInt<long long, IsNothrow::yes>>);
        static_assert(!CheckCallOperatorOfLayoutMapping<Mapping, ConvertibleToInt<long long, IsNothrow::no>>);
    }

    { // Check various mappings
        layout_right::mapping<extents<short>> m1;
        assert(m1() == 0);

        layout_right::mapping<dextents<int, 1>> m2{dextents<int, 1>{4}};
        assert(m2(0) == 0);
        assert(m2(1) == 1);
        assert(m2(2) == 2);
        assert(m2(3) == 3);

        layout_right::mapping<extents<long, 4, 5>> m3;
        assert(m3(0, 0) == 0);
        assert(m3(0, 1) == 1);
        assert(m3(1, 0) == 5);
        assert(m3(1, 1) == 6);
        assert(m3(1, 2) == 7);
        assert(m3(2, 1) == 11);
        assert(m3(2, 2) == 12);
        assert(m3(3, 4) == 19);
    }
}

constexpr void check_comparisons() {
    using StaticMapping  = layout_right::mapping<extents<int, 4, 4>>;
    using DynamicMapping = layout_right::mapping<dextents<int, 2>>;

    { // Check equality_comparable_with concept
        static_assert(equality_comparable_with<StaticMapping, DynamicMapping>);
        static_assert(!equality_comparable_with<StaticMapping, layout_right::mapping<extents<int, 3>>>);
        static_assert(!equality_comparable_with<DynamicMapping, layout_right::mapping<dextents<int, 1>>>);
    }

    { // Check correctness
        StaticMapping m1;
        DynamicMapping m2{dextents<int, 2>{4, 4}};
        DynamicMapping m3{dextents<int, 2>{2, 2}};
        assert(m1 == m2);
        assert(m2 != m3);
        assert(m1 != m3);
    }
}

constexpr void check_correctness() {
    { // empty extents
        const array<int, 0> vals{};
        mdspan<const int, extents<int>, layout_right> nothing{vals.data()};
        assert(nothing.size() == 1);
    }

    { // regular vector
        const array vals{2, 1, 0};
        mdspan<const int, extents<int, 3>, layout_right> vec{vals.data()};

        // TRANSITION, use operator[]
        assert(vec(0) == 2);
        assert(vec(1) == 1);
        assert(vec(2) == 0);
    }

    { // 4x3 matrix with row-major order
        const array vals{11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        mdspan<const int, extents<int, 4, 3>, layout_right> matrix{vals.data()};

        // TRANSITION, use operator[]
        assert(matrix(0, 0) == 11);
        assert(matrix(0, 2) == 9);
        assert(matrix(1, 1) == 7);
        assert(matrix(2, 0) == 5);
        assert(matrix(2, 2) == 3);
        assert(matrix(3, 1) == 1);
    }

    { // 4x3x2 tensor
        const array vals{23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        mdspan<const int, dextents<size_t, 3>, layout_right> tensor{vals.data(), 4, 3, 2};

        // TRANSITION, use operator[]
        assert(tensor(0, 0, 0) == 23);
        assert(tensor(0, 0, 1) == 22);
        assert(tensor(0, 1, 0) == 21);
        assert(tensor(0, 1, 1) == 20);
        assert(tensor(1, 0, 0) == 17);
        assert(tensor(1, 0, 1) == 16);
        assert(tensor(1, 1, 0) == 15);
        assert(tensor(1, 1, 1) == 14);
        assert(tensor(2, 2, 1) == 6);
        assert(tensor(3, 2, 1) == 0);
    }

    { // 3x2x3x2 tensor
        const array vals{35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12,
            11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        mdspan<const int, extents<long, 3, dynamic_extent, 3, dynamic_extent>, layout_right> tensor{vals.data(), 2, 2};

        // TRANSITION, use operator[]
        assert(tensor(0, 0, 0, 0) == 35);
        assert(tensor(0, 0, 0, 1) == 34);
        assert(tensor(0, 0, 1, 0) == 33);
        assert(tensor(0, 0, 1, 1) == 32);
        assert(tensor(0, 1, 0, 0) == 29);
        assert(tensor(0, 1, 0, 1) == 28);
        assert(tensor(0, 1, 1, 0) == 27);
        assert(tensor(0, 1, 1, 1) == 26);
        assert(tensor(1, 0, 0, 0) == 23);
        assert(tensor(1, 0, 0, 1) == 22);
        assert(tensor(1, 0, 1, 0) == 21);
        assert(tensor(1, 0, 1, 1) == 20);
        assert(tensor(1, 1, 0, 0) == 17);
        assert(tensor(1, 1, 0, 1) == 16);
        assert(tensor(1, 1, 1, 0) == 15);
        assert(tensor(1, 1, 1, 1) == 14);
        assert(tensor(2, 0, 2, 0) == 7);
        assert(tensor(2, 1, 2, 1) == 0);
    }
}

constexpr bool test() {
    check_members(extents<short>{});
    check_members(extents<int, 1, 2, 3>{});
    check_members(extents<unsigned short, 4, 4>{});
    check_members(extents<unsigned long long, dynamic_extent, 4, 5>{3});
    check_members(extents<short, dynamic_extent, dynamic_extent, 6>{4, 5});
    check_members(extents<unsigned char, dynamic_extent, dynamic_extent, dynamic_extent>{3, 3, 3});
    check_construction_from_other_right_mapping();
    check_construction_from_other_left_mapping();
    check_construction_from_other_stride_mapping();
    check_call_operator();
    check_comparisons();
    check_correctness();
    return true;
}

int main() {
    static_assert(test());
    test();
}
