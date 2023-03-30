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

template <class Mapping, class... Indices>
concept CanInvokeCallOperatorOfMapping = requires(Mapping m, Indices... i) {
                                             { m(i...) } -> same_as<typename Mapping::index_type>;
                                         };

template <class IndexType, size_t... Extents, size_t... Indices>
constexpr void do_check_members(const extents<IndexType, Extents...>& ext, index_sequence<Indices...>) {
    using Ext     = extents<IndexType, Extents...>;
    using Mapping = layout_left::mapping<Ext>;

    // layout_left meets the layout mapping policy requirements and is a trivial type
    static_assert(check_layout_mapping_policy_requirements<layout_left, Ext>());
    static_assert(is_trivial_v<layout_left>);

    // layout_left::mapping<Ext> is a trivially copyable type that models regular for each Ext
    static_assert(is_trivially_copyable_v<Mapping>);
    static_assert(regular<Mapping>);

    // Check member types
    static_assert(same_as<typename Mapping::extents_type, Ext>);
    static_assert(same_as<typename Mapping::index_type, typename Ext::index_type>);
    static_assert(same_as<typename Mapping::size_type, typename Ext::size_type>);
    static_assert(same_as<typename Mapping::rank_type, typename Ext::rank_type>);
    static_assert(same_as<typename Mapping::layout_type, layout_left>);

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
    using Mapping2       = layout_left::mapping<Ext2>;

    { // Check construction from other layout_left::mapping
        Mapping m1{ext};
        Mapping2 m2{m1};
        assert(m1 == m2);
        static_assert(is_nothrow_constructible_v<Mapping2, Mapping>);
        // Other tests are defined in 'check_construction_from_other_left_mapping' function
    }

    { // Check construction from layout_right::mapping
        using RightMapping = layout_right::mapping<Ext>;
        if constexpr (Ext::rank() <= 1) {
            RightMapping right_mapping{ext};
            [[maybe_unused]] Mapping m1{right_mapping};
            [[maybe_unused]] Mapping2 m2{right_mapping};
            assert(m1 == m2);
            static_assert(is_nothrow_constructible_v<Mapping, RightMapping>);
            static_assert(is_nothrow_constructible_v<Mapping2, RightMapping>);
        } else {
            static_assert(!is_constructible_v<Mapping, RightMapping>);
            static_assert(!is_constructible_v<Mapping2, RightMapping>);
        }
        // Other tests are defined in 'check_construction_from_other_right_mapping' function
    }

#pragma warning(push) // TRANSITION, "/analyze:only" BUG?
#pragma warning(disable : 28020) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call
    { // Check construction from layout_stride::mapping
        array<IndexType, Ext::rank()> strides{1};
        for (size_t i = 1; i < Ext::rank(); ++i) {
            strides[i] = static_cast<IndexType>(strides[i - 1] * ext.extent(i));
        }

        using StrideMapping = layout_stride::mapping<Ext>;
        StrideMapping stride_mapping{ext, strides};
        [[maybe_unused]] Mapping m{stride_mapping};
        // Other tests are defined in 'check_construction_from_other_stride_mapping' function
    }
#pragma warning(pop) // TRANSITION, "/analyze:only" BUG?

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
        const IndexType expected_value =
            static_cast<IndexType>((ext.extent(Indices) * ... * 1) / ext.extent(Ext::rank() - 1));
        assert(m.stride(Ext::rank() - 1) == expected_value);
        assert(m.stride(0) == 1);
        static_assert(noexcept(m.stride(Ext::rank() - 1)));
        static_assert(noexcept(m.stride(0)));
    } else {
        static_assert(!CheckStrideMemberFunction<Mapping>);
    }

    { // Check comparisons
        assert(m == m);
        // Other tests are defined in 'check_comparisons' function
    }
}

template <class IndexType, size_t... Extents>
constexpr void check_members(extents<IndexType, Extents...> ext) {
    do_check_members<IndexType, Extents...>(ext, make_index_sequence<sizeof...(Extents)>{});
}

constexpr void check_construction_from_other_left_mapping() {
    { // Check invalid construction
        using Mapping = layout_left::mapping<extents<int, 3, 3>>;
        static_assert(!is_constructible_v<Mapping, layout_left::mapping<extents<int, 3>>>);
        static_assert(!is_constructible_v<Mapping, layout_left::mapping<extents<int, 3, 3, 3>>>);
    }

    { // Check implicit conversions
        static_assert(!NotImplicitlyConstructibleFrom<layout_left::mapping<extents<int, 3>>,
                      layout_left::mapping<extents<int, 3>>>);
        static_assert(NotImplicitlyConstructibleFrom<layout_left::mapping<extents<int, 3>>,
            layout_left::mapping<extents<long long, 3>>>);
        static_assert(NotImplicitlyConstructibleFrom<layout_left::mapping<extents<int, 3, 3>>,
            layout_left::mapping<extents<int, dynamic_extent, 3>>>);
        static_assert(NotImplicitlyConstructibleFrom<layout_left::mapping<extents<int, 3, 3>>,
            layout_left::mapping<extents<int, dynamic_extent, dynamic_extent>>>);
    }
}

constexpr void check_construction_from_other_right_mapping() {
    { // Check construction from layout_right::mapping<E> with various values of E::rank()
        static_assert(
            is_constructible_v<layout_left::mapping<dextents<int, 0>>, layout_right::mapping<dextents<int, 0>>>);
        static_assert(
            is_constructible_v<layout_left::mapping<dextents<int, 1>>, layout_right::mapping<dextents<int, 1>>>);
        static_assert(
            !is_constructible_v<layout_left::mapping<dextents<int, 2>>, layout_right::mapping<dextents<int, 2>>>);
        static_assert(
            !is_constructible_v<layout_left::mapping<dextents<int, 3>>, layout_right::mapping<dextents<int, 3>>>);
    }

    { // Check construction from layout_right::mapping<E> when E is invalid
        using Mapping = layout_left::mapping<extents<int, 3>>;
        static_assert(!is_constructible_v<Mapping, layout_right::mapping<extents<int, 1>>>);
        static_assert(!is_constructible_v<Mapping, layout_right::mapping<extents<int, 2>>>);
    }

    { // Check implicit conversions
        static_assert(!NotImplicitlyConstructibleFrom<layout_left::mapping<extents<int, 3>>,
                      layout_right::mapping<extents<int, 3>>>);
        static_assert(NotImplicitlyConstructibleFrom<layout_left::mapping<extents<int, 3>>,
            layout_right::mapping<extents<long long, 3>>>);
        static_assert(NotImplicitlyConstructibleFrom<layout_left::mapping<extents<int, 3>>,
            layout_right::mapping<extents<int, dynamic_extent>>>);
    }
}

constexpr void check_construction_from_other_stride_mapping() {
    { // Check construction from layout_stride::mapping<E> with various values of E::rank()
        static_assert(
            is_constructible_v<layout_left::mapping<dextents<int, 0>>, layout_stride::mapping<dextents<int, 0>>>);
        static_assert(
            is_constructible_v<layout_left::mapping<dextents<int, 1>>, layout_stride::mapping<dextents<int, 1>>>);
        static_assert(
            is_constructible_v<layout_left::mapping<dextents<int, 2>>, layout_stride::mapping<dextents<int, 2>>>);
        static_assert(
            is_constructible_v<layout_left::mapping<dextents<int, 3>>, layout_stride::mapping<dextents<int, 3>>>);
    }

    { // Check construction from layout_stride::mapping<E> when E is invalid
        using Mapping = layout_left::mapping<extents<int, 3>>;
        static_assert(!is_constructible_v<Mapping, layout_stride::mapping<extents<int, 1>>>);
        static_assert(!is_constructible_v<Mapping, layout_stride::mapping<extents<int, 2>>>);
    }

    { // Check implicit conversions
        static_assert(
            !NotImplicitlyConstructibleFrom<layout_left::mapping<extents<int>>, layout_stride::mapping<extents<int>>>);
        static_assert(NotImplicitlyConstructibleFrom<layout_left::mapping<extents<int, 3>>,
            layout_stride::mapping<extents<int, 3>>>);
        static_assert(NotImplicitlyConstructibleFrom<layout_left::mapping<extents<int, 3>>,
            layout_stride::mapping<extents<long long, 3>>>);
        static_assert(NotImplicitlyConstructibleFrom<layout_left::mapping<extents<int, 3>>,
            layout_stride::mapping<extents<int, dynamic_extent>>>);
    }
}

constexpr void check_call_operator() {
    { // Check call with invalid amount of indices
        using Mapping = layout_left::mapping<dextents<int, 3>>;
        static_assert(!CanInvokeCallOperatorOfMapping<Mapping, int>);
        static_assert(!CanInvokeCallOperatorOfMapping<Mapping, int, int>);
        static_assert(CanInvokeCallOperatorOfMapping<Mapping, int, int, int>);
        static_assert(!CanInvokeCallOperatorOfMapping<Mapping, int, int, int, int>);
    }

    { // Check call with invalid types
        using Mapping = layout_left::mapping<dextents<long, 2>>;
        static_assert(CanInvokeCallOperatorOfMapping<Mapping, long, long>);
        static_assert(CanInvokeCallOperatorOfMapping<Mapping, long, short>);
        static_assert(CanInvokeCallOperatorOfMapping<Mapping, long, ConvertibleToInt<long>>);
        static_assert(CanInvokeCallOperatorOfMapping<Mapping, long, ConvertibleToInt<short>>);
        static_assert(!CanInvokeCallOperatorOfMapping<Mapping, long, NonConvertibleToAnything>);
    }

    { // Check call with types that might throw during conversion
        using Mapping = layout_left::mapping<dextents<long long, 1>>;
        static_assert(CanInvokeCallOperatorOfMapping<Mapping, ConvertibleToInt<long long, IsNothrow::yes>>);
        static_assert(!CanInvokeCallOperatorOfMapping<Mapping, ConvertibleToInt<long long, IsNothrow::no>>);
    }

    { // Check various mappings
        layout_left::mapping<extents<short>> m1;
        assert(m1() == 0);

        layout_left::mapping<extents<int, 3>> m2;
        assert(m2(0) == 0);
        assert(m2(1) == 1);
        assert(m2(2) == 2);

        layout_left::mapping<dextents<long, 2>> m3{dextents<int, 2>{5, 6}};
        assert(m3(0, 0) == 0);
        assert(m3(1, 0) == 1);
        assert(m3(0, 1) == 5);
        assert(m3(1, 1) == 6);
        assert(m3(2, 1) == 7);
        assert(m3(1, 2) == 11);
        assert(m3(4, 5) == 29);
    }
}

constexpr void check_comparisons() {
    using StaticMapping  = layout_left::mapping<extents<int, 3>>;
    using DynamicMapping = layout_left::mapping<dextents<int, 1>>;

    { // Check equality_comparable_with concept
        static_assert(equality_comparable_with<StaticMapping, DynamicMapping>);
        static_assert(!equality_comparable_with<StaticMapping, layout_left::mapping<extents<int, 3, 3>>>);
        static_assert(!equality_comparable_with<DynamicMapping, layout_left::mapping<dextents<int, 2>>>);
    }

    { // Check correctness
        StaticMapping m1;
        DynamicMapping m2{dextents<int, 1>{3}};
        DynamicMapping m3{dextents<int, 1>{2}};
        assert(m1 == m2);
        assert(m2 != m3);
        assert(m1 != m3);
    }
}

constexpr void check_correctness() {
    { // empty extents
        const array<int, 0> values{};
        mdspan<const int, extents<int>, layout_left> nothing{values.data()};
        assert(nothing.size() == 1);
    }

    { // regular vector
        const array values{0, 1, 2};
        mdspan<const int, extents<int, 3>, layout_left> vec{values.data()};

        // TRANSITION, use operator[]
        assert(vec(0) == 0);
        assert(vec(1) == 1);
        assert(vec(2) == 2);
    }

    { // 3x2 matrix with column-major order
        const array values{0, 1, 2, 3, 4, 5};
        mdspan<const int, extents<int, 3, 2>, layout_left> matrix{values.data()};

        // TRANSITION, use operator[]
        assert(matrix(0, 0) == 0);
        assert(matrix(1, 0) == 1);
        assert(matrix(2, 0) == 2);
        assert(matrix(0, 1) == 3);
        assert(matrix(1, 1) == 4);
        assert(matrix(2, 1) == 5);
    }

    { // 3x2x4 tensor
        const array values{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
        mdspan<const int, dextents<size_t, 3>, layout_left> tensor{values.data(), 3, 2, 4};

        // TRANSITION, use operator[]
        assert(tensor(0, 0, 0) == 0);
        assert(tensor(2, 0, 0) == 2);
        assert(tensor(1, 1, 1) == 10);
        assert(tensor(0, 0, 3) == 18);
        assert(tensor(2, 2, 2) == 20);
        assert(tensor(2, 1, 3) == 23);
    }

    { // 2x3x2x3 tensor
        const array values{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
            26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
        mdspan<const int, extents<long, 2, 3, dynamic_extent, dynamic_extent>, layout_left> tensor{values.data(), 2, 3};

        // TRANSITION, use operator[]
        assert(tensor(0, 0, 0, 0) == 0);
        assert(tensor(1, 0, 0, 0) == 1);
        assert(tensor(0, 1, 1, 0) == 8);
        assert(tensor(0, 0, 0, 1) == 12);
        assert(tensor(0, 0, 0, 2) == 24);
        assert(tensor(0, 2, 0, 2) == 28);
        assert(tensor(1, 2, 1, 2) == 35);
    }
}

constexpr bool test() {
    check_members(extents<short>{});
    check_members(extents<int, 1, 2, 3>{});
    check_members(extents<unsigned short, 4, 4>{});
    check_members(extents<unsigned long long, dynamic_extent, 4, 5>{3});
    check_members(extents<short, dynamic_extent, dynamic_extent, 6>{4, 5});
    check_members(extents<unsigned char, dynamic_extent, dynamic_extent, dynamic_extent>{3, 3, 3});
    check_construction_from_other_left_mapping();
    check_construction_from_other_right_mapping();
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
