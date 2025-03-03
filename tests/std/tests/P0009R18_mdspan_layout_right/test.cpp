// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <mdspan>
#include <span>
#include <type_traits>
#include <utility>

#include <test_mdspan_support.hpp>

using namespace std;

template <class IndexType, size_t... Extents, size_t... Indices>
constexpr void check_members(const extents<IndexType, Extents...>& ext, index_sequence<Indices...>) {
    using Ext     = extents<IndexType, Extents...>;
    using Mapping = layout_right::mapping<Ext>;

    // layout_right meets the requirements of N5001 [mdspan.layout.policy.overview]/1
    static_assert(check_layout_mapping_policy_requirements<layout_right, Ext>());
    static_assert(is_trivially_copyable_v<layout_right>);
    static_assert(is_trivially_default_constructible_v<layout_right>);

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
        const Mapping m;
        Mapping cpy = m;
        assert(cpy == m);
        static_assert(is_nothrow_default_constructible_v<Mapping>);
        static_assert(is_nothrow_copy_constructible_v<Mapping>);
    }

    { // Check copy assignment operator
        const Mapping m;
        Mapping cpy;
        cpy = m;
        assert(cpy == m);
        static_assert(is_nothrow_copy_assignable_v<Mapping>);
    }

    { // Check construction from extents_type
        Mapping m{ext};
        assert(m.extents() == ext);
        static_assert(is_nothrow_constructible_v<Mapping, Ext>);
        // Other tests are defined in 'check_construction_from_extents' function
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

        layout_stride::mapping<Ext> m1{ext, strides};
        Mapping m2{m1};
        assert(m1.extents() == m2.extents());
        // Other tests are defined in 'check_construction_from_other_stride_mapping' function
    }

    Mapping m{ext}; // For later use

    { // Check 'extents' function
        assert(m.extents() == ext);
        static_assert(noexcept(m.extents()));
    }

    { // Check 'required_span_size' function
        same_as<IndexType> decltype(auto) rss = m.required_span_size();
        assert(rss == static_cast<IndexType>((ext.extent(Indices) * ... * 1)));
        static_assert(noexcept(m.required_span_size()));
        // Other tests are defined in 'check_mapping_properties'
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
        // Other tests are defined in 'check_mapping_properties'
    }

    if constexpr (Ext::rank() > 0) { // Check 'stride' function
        const IndexType expected_stride0 = static_cast<IndexType>((ext.extent(Indices) * ... * 1) / ext.extent(0));
        assert(m.stride(0) == expected_stride0);
        assert(m.stride(Ext::rank() - 1) == 1);
        static_assert(noexcept(m.stride(Ext::rank() - 1)));
        static_assert(noexcept(m.stride(0)));
        // Other tests are defined in 'check_stride_function'
    } else {
        static_assert(!CheckStrideMemberFunction<Mapping>);
    }

    { // Check comparisons
        assert(m == m);
        assert(!(m != m));
        static_assert(noexcept(m == m));
        static_assert(noexcept(m != m));
        // Other tests are defined in 'check_comparisons' function
    }
}

void check_mapping_properties() {
    if constexpr (!is_permissive) {
        auto check = [](const auto& mapping) {
            const auto props = get_mapping_properties(mapping);
            assert(props.req_span_size == mapping.required_span_size());
            assert(props.uniqueness);
            assert(props.exhaustiveness);
            assert(props.strideness);
        };

        using M1 = layout_right::mapping<extents<int, 7, 11, 13>>;
        check(M1{});

        using M2 = layout_right::mapping<extents<unsigned long, dynamic_extent, 4, dynamic_extent>>;
        check(M2{M2::extents_type{2, 6}});

        using M3 = layout_right::mapping<dextents<unsigned char, 4>>;
        check(M3{M3::extents_type{4, 3, 5, 4}});
    }
}

constexpr void check_construction_from_extents() {
    using Ext = extents<int, 6, 4, 2>;
    Ext ext;

    { // Check construction from 'extents_type'
        layout_right::mapping<Ext> mp(ext);
        assert(mp.extents() == ext);
        static_assert(is_nothrow_constructible_v<decltype(mp), Ext>);
    }

    { // Check construction from other extents
        layout_right::mapping<extents<long, dynamic_extent, 4, dynamic_extent>> mp(ext);
        assert(mp.extents() == ext);
        static_assert(is_nothrow_constructible_v<decltype(mp), Ext>);
    }
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

    { // Check effects
        layout_right::mapping<extents<unsigned int, 6, 2, 6>> m1;
        layout_right::mapping<dextents<unsigned short, 3>> m2{m1};
        assert(m2.extents().extent(0) == 6);
        assert(m2.extents().extent(1) == 2);
        assert(m2.extents().extent(2) == 6);
        assert(m1.extents() == m2.extents());
    }
}

constexpr void check_construction_from_other_left_mapping() {
    { // Check construction from layout_left::mapping<E> with various values of E::rank()
        static_assert(is_nothrow_constructible_v<layout_right::mapping<dextents<int, 0>>,
            layout_left::mapping<dextents<int, 0>>>);
        static_assert(is_nothrow_constructible_v<layout_right::mapping<dextents<int, 1>>,
            layout_left::mapping<dextents<int, 1>>>);
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

    { // Check effects
        layout_left::mapping<extents<int, 8>> m1;
        layout_right::mapping<dextents<signed char, 1>> m2{m1};
        assert(m2.extents().extent(0) == 8);
    }
}

constexpr void check_construction_from_other_stride_mapping() {
    { // Check construction from layout_stride::mapping<E> with various values of E::rank()
        static_assert(is_nothrow_constructible_v<layout_right::mapping<dextents<int, 0>>,
            layout_stride::mapping<dextents<int, 0>>>);
        static_assert(is_nothrow_constructible_v<layout_right::mapping<dextents<int, 1>>,
            layout_stride::mapping<dextents<int, 1>>>);
        static_assert(is_nothrow_constructible_v<layout_right::mapping<dextents<int, 2>>,
            layout_stride::mapping<dextents<int, 2>>>);
        static_assert(is_nothrow_constructible_v<layout_right::mapping<dextents<int, 3>>,
            layout_stride::mapping<dextents<int, 3>>>);
    }

    { // Check construction from layout_stride::mapping<E> when E is invalid
        using Mapping = layout_right::mapping<extents<int, 3>>;
        static_assert(!is_constructible_v<Mapping, layout_stride::mapping<extents<int, 1>>>);
        static_assert(!is_constructible_v<Mapping, layout_stride::mapping<extents<int, 2>>>);
    }

    { // Check correctness
        using Ext = extents<int, 4, 3, 2, 3, 4>;
        layout_stride::mapping<Ext> m1{Ext{}, array{72, 24, 12, 4, 1}};
        layout_right::mapping<Ext> m2{m1};
        assert(m2.extents().extent(0) == 4);
        assert(m2.extents().extent(1) == 3);
        assert(m2.stride(2) == 12);
        assert(m2.stride(3) == 4);
        assert(m2.extents() == m1.extents());
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

constexpr void check_stride_function() {
    layout_right::mapping<extents<unsigned short, 2, 3, 5, 7, 11>> prime_mapping;

    { // Check return type
        same_as<unsigned short> decltype(auto) s = prime_mapping.stride(0);
        assert(s == 1155); // 11 * 7 * 5 * 3
    }

    { // Check that argument is 'rank_type'
        struct ConvertibleToRankType {
            constexpr operator integral auto() const {
                return 0;
            }

            constexpr operator size_t() const { // NB: 'rank_type' is always 'size_t'
                return 1;
            }
        };

        assert(prime_mapping.stride(ConvertibleToRankType{}) == 385); // 11 * 7 * 5
    }

    { // Check other strides
        assert(prime_mapping.stride(2) == 77);
        assert(prime_mapping.stride(3) == 11);
        assert(prime_mapping.stride(4) == 1);
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

constexpr void check_ctad() {
    using Ext = extents<long, 5, 5>;
    layout_right::mapping m{Ext{}};
    static_assert(same_as<decltype(m), layout_right::mapping<Ext>>);
    assert(m.extents().extent(0) == 5);
    assert(m.stride(1) == 1);
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

#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
        assert(vec[0] == 2);
        assert(vec[1] == 1);
        assert(vec[2] == 0);
#else // ^^^ defined(__cpp_multidimensional_subscript) / !defined(__cpp_multidimensional_subscript) vvv
        assert(vec[array{0}] == 2);
        assert(vec[array{1}] == 1);
        assert(vec[array{2}] == 0);
#endif // ^^^ !defined(__cpp_multidimensional_subscript) ^^^
    }

    { // 4x3 matrix with row-major order
        const array vals{11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        mdspan<const int, extents<int, 4, 3>, layout_right> matrix{vals.data()};

#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
        assert((matrix[0, 0] == 11));
        assert((matrix[0, 2] == 9));
        assert((matrix[1, 1] == 7));
        assert((matrix[2, 0] == 5));
        assert((matrix[2, 2] == 3));
        assert((matrix[3, 1] == 1));
#else // ^^^ defined(__cpp_multidimensional_subscript) / !defined(__cpp_multidimensional_subscript) vvv
        assert((matrix[array{0, 0}] == 11));
        assert((matrix[array{0, 2}] == 9));
        assert((matrix[array{1, 1}] == 7));
        assert((matrix[array{2, 0}] == 5));
        assert((matrix[array{2, 2}] == 3));
        assert((matrix[array{3, 1}] == 1));
#endif // ^^^ !defined(__cpp_multidimensional_subscript) ^^^
    }

    { // 4x3x2 tensor
        const array vals{23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        mdspan<const int, dextents<size_t, 3>, layout_right> tensor{vals.data(), 4, 3, 2};

#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
        assert((tensor[0, 0, 0] == 23));
        assert((tensor[0, 0, 1] == 22));
        assert((tensor[0, 1, 0] == 21));
        assert((tensor[0, 1, 1] == 20));
        assert((tensor[1, 0, 0] == 17));
        assert((tensor[1, 0, 1] == 16));
        assert((tensor[1, 1, 0] == 15));
        assert((tensor[1, 1, 1] == 14));
        assert((tensor[2, 2, 1] == 6));
        assert((tensor[3, 2, 1] == 0));
#else // ^^^ defined(__cpp_multidimensional_subscript) / !defined(__cpp_multidimensional_subscript) vvv
        assert((tensor[array{0, 0, 0}] == 23));
        assert((tensor[array{0, 0, 1}] == 22));
        assert((tensor[array{0, 1, 0}] == 21));
        assert((tensor[array{0, 1, 1}] == 20));
        assert((tensor[array{1, 0, 0}] == 17));
        assert((tensor[array{1, 0, 1}] == 16));
        assert((tensor[array{1, 1, 0}] == 15));
        assert((tensor[array{1, 1, 1}] == 14));
        assert((tensor[array{2, 2, 1}] == 6));
        assert((tensor[array{3, 2, 1}] == 0));
#endif // ^^^ !defined(__cpp_multidimensional_subscript) ^^^
    }

    { // 3x2x3x2 tensor
        const array vals{35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12,
            11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        mdspan<const int, extents<long, 3, dynamic_extent, 3, dynamic_extent>, layout_right> tensor{vals.data(), 2, 2};

#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
        assert((tensor[0, 0, 0, 0] == 35));
        assert((tensor[0, 0, 0, 1] == 34));
        assert((tensor[0, 0, 1, 0] == 33));
        assert((tensor[0, 0, 1, 1] == 32));
        assert((tensor[0, 1, 0, 0] == 29));
        assert((tensor[0, 1, 0, 1] == 28));
        assert((tensor[0, 1, 1, 0] == 27));
        assert((tensor[0, 1, 1, 1] == 26));
        assert((tensor[1, 0, 0, 0] == 23));
        assert((tensor[1, 0, 0, 1] == 22));
        assert((tensor[1, 0, 1, 0] == 21));
        assert((tensor[1, 0, 1, 1] == 20));
        assert((tensor[1, 1, 0, 0] == 17));
        assert((tensor[1, 1, 0, 1] == 16));
        assert((tensor[1, 1, 1, 0] == 15));
        assert((tensor[1, 1, 1, 1] == 14));
        assert((tensor[2, 0, 2, 0] == 7));
        assert((tensor[2, 1, 2, 1] == 0));
#else // ^^^ defined(__cpp_multidimensional_subscript) / !defined(__cpp_multidimensional_subscript) vvv
        assert((tensor[array{0, 0, 0, 0}] == 35));
        assert((tensor[array{0, 0, 0, 1}] == 34));
        assert((tensor[array{0, 0, 1, 0}] == 33));
        assert((tensor[array{0, 0, 1, 1}] == 32));
        assert((tensor[array{0, 1, 0, 0}] == 29));
        assert((tensor[array{0, 1, 0, 1}] == 28));
        assert((tensor[array{0, 1, 1, 0}] == 27));
        assert((tensor[array{0, 1, 1, 1}] == 26));
        assert((tensor[array{1, 0, 0, 0}] == 23));
        assert((tensor[array{1, 0, 0, 1}] == 22));
        assert((tensor[array{1, 0, 1, 0}] == 21));
        assert((tensor[array{1, 0, 1, 1}] == 20));
        assert((tensor[array{1, 1, 0, 0}] == 17));
        assert((tensor[array{1, 1, 0, 1}] == 16));
        assert((tensor[array{1, 1, 1, 0}] == 15));
        assert((tensor[array{1, 1, 1, 1}] == 14));
        assert((tensor[array{2, 0, 2, 0}] == 7));
        assert((tensor[array{2, 1, 2, 1}] == 0));
#endif // ^^^ !defined(__cpp_multidimensional_subscript) ^^^
    }
}

constexpr bool test() {
    check_members_with_various_extents([]<class E>(const E& e) { check_members(e, make_index_sequence<E::rank()>{}); });
    if (!is_constant_evaluated()) { // too heavy for compile time
        check_mapping_properties();
    }
    check_construction_from_extents();
    check_construction_from_other_right_mapping();
    check_construction_from_other_left_mapping();
    check_construction_from_other_stride_mapping();
    check_call_operator();
    check_stride_function();
    check_comparisons();
    check_ctad();
    check_correctness();

    return true;
}

int main() {
    static_assert(test());
    test();
}
