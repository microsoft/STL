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
constexpr void do_check_members(const extents<IndexType, Extents...>& ext,
    const array<IndexType, sizeof...(Extents)> strs, index_sequence<Indices...>) {
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

#pragma warning(push) // TRANSITION, "/analyze:only" BUG?
#pragma warning(disable : 28020) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call
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
        assert(m.strides() == strs);
        static_assert(is_nothrow_constructible_v<Mapping, Ext, Strides>);
        // Other tests are defined in 'check_construction_from_extents_and_array' function [FIXME]
    }

    { // Check construction from extents_type and span
        using Span = span<const IndexType, sizeof...(Extents)>;
        Mapping m{ext, Span{strs}};
        assert(m.extents() == ext);
        assert(m.strides() == strs);
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
        assert(strs2 == strs);
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

    { // Check 'stride' function (intentionally not if constexpr)
        for (size_t i = 0; i < strs.size(); ++i) {
            same_as<IndexType> decltype(auto) s = m.stride(i);
            assert(strs[i] == s);
        }
    }

    { // Check comparisons
        assert(m == m);
        assert(!(m != m));
        // Other tests are defined in 'check_comparisons' function [FIXME]
    }
#pragma warning(pop) // TRANSITION, "/analyze:only" BUG?
}

template <class IndexType, size_t... Extents>
constexpr void check_members(extents<IndexType, Extents...> ext, const array<IndexType, sizeof...(Extents)> strides) {
    do_check_members<IndexType, Extents...>(ext, strides, make_index_sequence<sizeof...(Extents)>{});
}

constexpr bool test() {
    check_members(extents<short>{}, array<short, 0>{});
    check_members(extents<long, 4>{}, array<long, 1>{1});
    check_members(extents<int, 3, dynamic_extent>{3}, array<int, 2>{1, 3});
    // TRANSITION more tests
    return true;
}

int main() {
    static_assert(test());
    test();
}
