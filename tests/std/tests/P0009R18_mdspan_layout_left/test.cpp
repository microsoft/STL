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
    using Mapping = layout_left::mapping<Ext>;

    // layout_left meets the requirements of N5001 [mdspan.layout.policy.overview]/1
    static_assert(check_layout_mapping_policy_requirements<layout_left, Ext>());
    static_assert(is_trivially_copyable_v<layout_left>);
    static_assert(is_trivially_default_constructible_v<layout_left>);

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

    { // Check construction from layout_stride::mapping
        array<IndexType, Ext::rank()> strides{};
        if constexpr (Ext::rank() > 0) {
            strides.front() = 1;
            for (size_t i = 1; i < Ext::rank(); ++i) {
                strides[i] = static_cast<IndexType>(strides[i - 1] * ext.extent(i - 1));
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
        const IndexType expected_value =
            static_cast<IndexType>((ext.extent(Indices) * ... * 1) / ext.extent(Ext::rank() - 1));
        assert(m.stride(Ext::rank() - 1) == expected_value);
        assert(m.stride(0) == 1);
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

        using M1 = layout_left::mapping<extents<int, 2, 3, 5>>;
        check(M1{});

        using M2 = layout_left::mapping<extents<unsigned long, 8, dynamic_extent, 5>>;
        check(M2{M2::extents_type{6}});

        using M3 = layout_left::mapping<dextents<signed char, 4>>;
        check(M3{M3::extents_type{3, 5, 4, 2}});
    }
}

constexpr void check_construction_from_extents() {
    using Ext = extents<int, 2, 4, 6>;
    Ext ext;

    { // Check construction from 'extents_type'
        layout_left::mapping<Ext> mp(ext);
        assert(mp.extents() == ext);
        static_assert(is_nothrow_constructible_v<decltype(mp), Ext>);
    }

    { // Check construction from other extents
        layout_left::mapping<extents<long, dynamic_extent, 4, dynamic_extent>> mp(ext);
        assert(mp.extents() == ext);
        static_assert(is_nothrow_constructible_v<decltype(mp), Ext>);
    }
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

    { // Check effects
        layout_left::mapping<extents<int, 5, 3, 5>> m1;
        layout_left::mapping<dextents<short, 3>> m2{m1};
        assert(m2.extents().extent(0) == 5);
        assert(m2.extents().extent(1) == 3);
        assert(m2.extents().extent(2) == 5);
        assert(m1.extents() == m2.extents());
    }
}

constexpr void check_construction_from_other_right_mapping() {
    { // Check construction from layout_right::mapping<E> with various values of E::rank()
        static_assert(is_nothrow_constructible_v<layout_left::mapping<dextents<int, 0>>,
            layout_right::mapping<dextents<int, 0>>>);
        static_assert(is_nothrow_constructible_v<layout_left::mapping<dextents<int, 1>>,
            layout_right::mapping<dextents<int, 1>>>);
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
        using Mapping = layout_left::mapping<extents<int, 3>>;
        static_assert(!NotImplicitlyConstructibleFrom<Mapping, layout_right::mapping<extents<int, 3>>>);
        static_assert(NotImplicitlyConstructibleFrom<Mapping, layout_right::mapping<extents<long long, 3>>>);
        static_assert(NotImplicitlyConstructibleFrom<Mapping, layout_right::mapping<extents<int, dynamic_extent>>>);
    }

    { // Check effects
        layout_right::mapping<extents<int, 8>> m1;
        layout_left::mapping<dextents<signed char, 1>> m2{m1};
        assert(m2.extents().extent(0) == 8);
    }
}

constexpr void check_construction_from_other_stride_mapping() {
    { // Check construction from layout_stride::mapping<E> with various values of E::rank()
        static_assert(is_nothrow_constructible_v<layout_left::mapping<dextents<int, 0>>,
            layout_stride::mapping<dextents<int, 0>>>); // strengthened
        static_assert(is_nothrow_constructible_v<layout_left::mapping<dextents<int, 1>>,
            layout_stride::mapping<dextents<int, 1>>>); // strengthened
        static_assert(is_nothrow_constructible_v<layout_left::mapping<dextents<int, 2>>,
            layout_stride::mapping<dextents<int, 2>>>); // strengthened
        static_assert(is_nothrow_constructible_v<layout_left::mapping<dextents<int, 3>>,
            layout_stride::mapping<dextents<int, 3>>>); // strengthened
    }

    { // Check construction from layout_stride::mapping<E> when E is invalid
        using Mapping = layout_left::mapping<extents<int, 3>>;
        static_assert(!is_constructible_v<Mapping, layout_stride::mapping<extents<int, 1>>>);
        static_assert(!is_constructible_v<Mapping, layout_stride::mapping<extents<int, 2>>>);
    }

    { // Check correctness
        using Ext = extents<int, 4, 3, 2, 3, 4>;
        layout_stride::mapping<Ext> m1{Ext{}, array{1, 4, 12, 24, 72}};
        layout_left::mapping<Ext> m2{m1};
        assert(m2.extents().extent(0) == 4);
        assert(m2.extents().extent(1) == 3);
        assert(m2.stride(2) == 12);
        assert(m2.stride(3) == 24);
        assert(m2.extents() == m1.extents());
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
        static_assert(!CheckCallOperatorOfLayoutMapping<Mapping, int>);
        static_assert(!CheckCallOperatorOfLayoutMapping<Mapping, int, int>);
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, int, int, int>);
        static_assert(!CheckCallOperatorOfLayoutMapping<Mapping, int, int, int, int>);
    }

    { // Check call with invalid types
        using Mapping = layout_left::mapping<dextents<long, 2>>;
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, long, long>);
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, long, short>);
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, long, ConvertibleToInt<long>>);
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, long, ConvertibleToInt<short>>);
        static_assert(!CheckCallOperatorOfLayoutMapping<Mapping, long, NonConvertibleToAnything>);
    }

    { // Check call with types that might throw during conversion
        using Mapping = layout_left::mapping<dextents<long long, 1>>;
        static_assert(CheckCallOperatorOfLayoutMapping<Mapping, ConvertibleToInt<long long, IsNothrow::yes>>);
        static_assert(!CheckCallOperatorOfLayoutMapping<Mapping, ConvertibleToInt<long long, IsNothrow::no>>);
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

constexpr void check_stride_function() {
    layout_left::mapping<extents<unsigned short, 2, 3, 5, 7, 11>> prime_mapping;

    { // Check return type
        same_as<unsigned short> decltype(auto) s = prime_mapping.stride(0);
        assert(s == 1);
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

        assert(prime_mapping.stride(ConvertibleToRankType{}) == 2); // 1 * 2
    }

    { // Check other strides
        assert(prime_mapping.stride(2) == 6);
        assert(prime_mapping.stride(3) == 30);
        assert(prime_mapping.stride(4) == 210);
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

constexpr void check_ctad() {
    using Ext = extents<long, 3, 3>;
    layout_left::mapping m{Ext{}};
    static_assert(same_as<decltype(m), layout_left::mapping<Ext>>);
    assert(m.extents().extent(0) == 3);
    assert(m.stride(1) == 3);
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

#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
        assert(vec[0] == 0);
        assert(vec[1] == 1);
        assert(vec[2] == 2);
#else // ^^^ defined(__cpp_multidimensional_subscript) / !defined(__cpp_multidimensional_subscript) vvv
        assert(vec[array{0}] == 0);
        assert(vec[array{1}] == 1);
        assert(vec[array{2}] == 2);
#endif // ^^^ !defined(__cpp_multidimensional_subscript) ^^^
    }

    { // 3x2 matrix with column-major order
        const array values{0, 1, 2, 3, 4, 5};
        mdspan<const int, extents<int, 3, 2>, layout_left> matrix{values.data()};

#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
        assert((matrix[0, 0] == 0));
        assert((matrix[1, 0] == 1));
        assert((matrix[2, 0] == 2));
        assert((matrix[0, 1] == 3));
        assert((matrix[1, 1] == 4));
        assert((matrix[2, 1] == 5));
#else // ^^^ defined(__cpp_multidimensional_subscript) / !defined(__cpp_multidimensional_subscript) vvv
        assert((matrix[array{0, 0}] == 0));
        assert((matrix[array{1, 0}] == 1));
        assert((matrix[array{2, 0}] == 2));
        assert((matrix[array{0, 1}] == 3));
        assert((matrix[array{1, 1}] == 4));
        assert((matrix[array{2, 1}] == 5));
#endif // ^^^ !defined(__cpp_multidimensional_subscript) ^^^
    }

    { // 3x2x4 tensor
        const array values{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
        mdspan<const int, dextents<size_t, 3>, layout_left> tensor{values.data(), 3, 2, 4};

#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
        assert((tensor[0, 0, 0] == 0));
        assert((tensor[2, 0, 0] == 2));
        assert((tensor[1, 1, 1] == 10));
        assert((tensor[0, 0, 3] == 18));
        assert((tensor[2, 1, 2] == 17));
        assert((tensor[2, 1, 3] == 23));
#else // ^^^ defined(__cpp_multidimensional_subscript) / !defined(__cpp_multidimensional_subscript) vvv
        assert((tensor[array{0, 0, 0}] == 0));
        assert((tensor[array{2, 0, 0}] == 2));
        assert((tensor[array{1, 1, 1}] == 10));
        assert((tensor[array{0, 0, 3}] == 18));
        assert((tensor[array{2, 1, 2}] == 17));
        assert((tensor[array{2, 1, 3}] == 23));
#endif // ^^^ !defined(__cpp_multidimensional_subscript) ^^^
    }

    { // 2x3x2x3 tensor
        const array values{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
            26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
        mdspan<const int, extents<long, 2, 3, dynamic_extent, dynamic_extent>, layout_left> tensor{values.data(), 2, 3};

#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
        assert((tensor[0, 0, 0, 0] == 0));
        assert((tensor[1, 0, 0, 0] == 1));
        assert((tensor[0, 1, 1, 0] == 8));
        assert((tensor[0, 0, 0, 1] == 12));
        assert((tensor[0, 0, 0, 2] == 24));
        assert((tensor[0, 2, 0, 2] == 28));
        assert((tensor[1, 2, 1, 2] == 35));
#else // ^^^ defined(__cpp_multidimensional_subscript) / !defined(__cpp_multidimensional_subscript) vvv
        assert((tensor[array{0, 0, 0, 0}] == 0));
        assert((tensor[array{1, 0, 0, 0}] == 1));
        assert((tensor[array{0, 1, 1, 0}] == 8));
        assert((tensor[array{0, 0, 0, 1}] == 12));
        assert((tensor[array{0, 0, 0, 2}] == 24));
        assert((tensor[array{0, 2, 0, 2}] == 28));
        assert((tensor[array{1, 2, 1, 2}] == 35));
#endif // ^^^ !defined(__cpp_multidimensional_subscript) ^^^
    }
}

constexpr bool test() {
    check_members_with_various_extents([]<class E>(const E& e) { check_members(e, make_index_sequence<E::rank()>{}); });
    if (!is_constant_evaluated()) { // too heavy for compile time
        check_mapping_properties();
    }
    check_construction_from_extents();
    check_construction_from_other_left_mapping();
    check_construction_from_other_right_mapping();
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
