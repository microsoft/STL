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
        // Other tests are defined in 'check_construction_from_extents_and_array' function [FIXME]
    }

    { // Check construction from extents_type and span
        using Span = span<const StridesIndexType, sizeof...(Extents)>;
        Mapping m{ext, Span{strs}};
        assert(m.extents() == ext);
        assert(ranges::equal(m.strides(), strs, CmpEqual{}));
        static_assert(is_nothrow_constructible_v<Mapping, Ext, Span>);
        // Other tests are defined in 'check_construction_from_extents_and_array' function [FIXME]
    }

    using OtherIndexType = long long;
    using Ext2           = extents<OtherIndexType, Extents...>;
    using Mapping2       = layout_stride::mapping<Ext2>;

    { // Check construction from other mappings
        Mapping m1{ext, strs};
        Mapping2 m2{m1};
        assert(m1 == m2);
        static_assert(is_nothrow_constructible_v<Mapping2, Mapping>);
        // Other tests are defined in 'check_construction_from_other_mappings' function [FIXME]
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

    // Function 'required_span_size' is tested in 'check_required_span_size' function[FIXME]
    { // Check 'required_span_size' function[FIXME]
        if (((ext.extent(Indices) == 0) || ...)) {
            assert(m.required_span_size() == 0);
        } else {
            const IndexType expected_value =
                static_cast<IndexType>((((ext.extent(Indices) - 1) * strs[Indices]) + ... + 1));
            assert(m.required_span_size() == expected_value);
        }
        static_assert(noexcept(m.required_span_size()));
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
        // Tests of 'is_exhaustive' are defined in 'check_is_exhaustive' function [FIXME]
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
        // Other tests are defined in 'check_comparisons' function [FIXME]
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

constexpr void check_ctad() {
    extents<size_t, 2, 3> e;
    array<size_t, 2> s{1, 2};
    layout_stride::mapping m{e, s};
    assert(m.extents() == e);
    assert(m.strides() == s);
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

    check_ctad();

    // TRANSITION more tests
    return true;
}

int main() {
    static_assert(test());
    test();
}
