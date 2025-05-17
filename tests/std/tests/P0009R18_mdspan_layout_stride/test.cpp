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
#include <utility>

#include <test_mdspan_support.hpp>

using namespace std;

struct CmpEqual {
    template <class T, class U>
    [[nodiscard]] constexpr bool operator()(T t, U u) const noexcept {
        return cmp_equal(t, u);
    }
};

struct NotLayoutMappingAlikeAtAll {
    template <class Extents>
    class mapping : public layout_right::mapping<Extents> {
    public:
        using layout_type = NotLayoutMappingAlikeAtAll;

    private:
        using layout_right::mapping<Extents>::is_always_exhaustive;
    };
};

static_assert(!_Layout_mapping_alike<NotLayoutMappingAlikeAtAll::mapping<extents<int, 4, 4>>>);

enum class AlwaysUnique : bool { no, yes };
enum class AlwaysStrided : bool { no, yes };

template <AlwaysUnique Unique, AlwaysStrided Strided>
struct LyingLayout {
    template <class Extents>
    class mapping : public layout_left::mapping<Extents> {
    public:
        using layout_type = LyingLayout;

        constexpr bool is_unique() const {
            return is_unique();
        }

        constexpr bool is_exhaustive() const {
            return is_exhaustive();
        }

        constexpr bool is_strided() const {
            return is_strided();
        }

        static constexpr bool is_always_unique() {
            // might be a lie, allowed by the standard (N4950 [mdspan.layout.reqmts]/23 Note 5)
            return to_underlying(Unique);
        }

        static constexpr bool is_always_exhaustive() {
            return layout_left::mapping<Extents>::is_always_exhaustive();
        }

        static constexpr bool is_always_strided() {
            // might be a lie, allowed by the standard (N4950 [mdspan.layout.reqmts]/27 Note 7)
            return to_underlying(Strided);
        }
    };
};

static_assert(
    check_layout_mapping_policy_requirements<LyingLayout<AlwaysUnique::no, AlwaysStrided::no>, extents<int, 3, 4>>());
static_assert(
    check_layout_mapping_policy_requirements<LyingLayout<AlwaysUnique::yes, AlwaysStrided::yes>, dextents<int, 3>>());

struct HollowLayout {
    template <class Extents>
        requires (Extents::rank() == 0)
    class mapping : public layout_right::mapping<Extents> {
    public:
        using index_type  = Extents::index_type;
        using layout_type = HollowLayout;

        constexpr index_type operator()() const noexcept {
            return 1; // NB: used by 'check_comparisons' (OFFSET(*this) != 0)
        }

        constexpr index_type required_span_size() const noexcept {
            return 2;
        }
    };
};

static_assert(check_layout_mapping_policy_requirements<HollowLayout, extents<short>>());

template <size_t... Extents, class IndexType, class StridesIndexType, size_t... Indices>
constexpr void do_check_members(const extents<IndexType, Extents...>& ext,
    const array<StridesIndexType, sizeof...(Extents)>& strs, index_sequence<Indices...>) {
    using Ext     = extents<IndexType, Extents...>;
    using Strides = array<IndexType, sizeof...(Extents)>;
    using Mapping = layout_stride::mapping<Ext>;

    // layout_stride meets the requirements of N5001 [mdspan.layout.policy.overview]/1
    static_assert(check_layout_mapping_policy_requirements<layout_stride, Ext>());
    static_assert(is_trivially_copyable_v<layout_stride>);
    static_assert(is_trivially_default_constructible_v<layout_stride>);

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
        const Mapping m;
        const Mapping cpy = m;
        const layout_right::mapping<Ext> right_mapping;
        assert(m == right_mapping);
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

    { // Check construction from extents_type and array
        Mapping m{ext, strs};
        assert(m.extents() == ext);
        assert(ranges::equal(m.strides(), strs, CmpEqual{}));
        static_assert(is_nothrow_constructible_v<Mapping, Ext, Strides>);
        // Other tests are defined in 'check_construction_from_extents_and_array_or_span' function
    }

    { // Check construction from extents_type and span
        using Span = span<const StridesIndexType, sizeof...(Extents)>;
        Mapping m{ext, Span{strs}};
        assert(m.extents() == ext);
        assert(ranges::equal(m.strides(), strs, CmpEqual{}));
        static_assert(is_nothrow_constructible_v<Mapping, Ext, Span>);
        // Other tests are defined in 'check_construction_from_extents_and_array_or_span' function
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
        // Other tests are defined in 'check_stride_function'
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
        // Other tests are defined in 'check_required_span_size' and 'check_mapping_properties' functions
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
        // Other tests are defined in 'check_mapping_properties'
    }

    { // Check 'stride' function
        for (size_t i = 0; i < Ext::rank(); ++i) {
            same_as<IndexType> decltype(auto) s = m.stride(i);
            assert(cmp_equal(strs[i], s));
        }
    }

    { // Check comparisons
        assert(m == m);
        assert(!(m != m));
        static_assert(noexcept(m == m));
        static_assert(noexcept(m != m));
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

void check_mapping_properties() {
    if constexpr (!is_permissive) {
        auto check = [](const auto& mapping, const bool expected_exhaustiveness) {
            const auto props = get_mapping_properties(mapping);
            assert(props.req_span_size == mapping.required_span_size());
            assert(props.uniqueness);
            assert(props.exhaustiveness == expected_exhaustiveness);
            assert(props.exhaustiveness == mapping.is_exhaustive());
            assert(props.strideness);
        };

        { // Check exhaustive mappings
            using M1 = layout_stride::mapping<extents<unsigned int, 4, 2, 3>>;
            check(M1{M1::extents_type{}, array{6, 1, 2}}, true);

            using M2 = layout_stride::mapping<extents<unsigned long long, dynamic_extent, dynamic_extent, 8>>;
            check(M2{M2::extents_type{6, 7}, array{1, 48, 6}}, true);

            using M3 = layout_stride::mapping<dextents<short, 4>>;
            check(M3{M3::extents_type{3, 5, 2, 4}, array{20, 1, 60, 5}}, true);
        }

        { // Check non-exhaustive mappings
            using M1 = layout_stride::mapping<extents<int, 2, 4, 3>>;
            check(M1{M1::extents_type{}, array{9, 18, 1}}, false);

            using M2 = layout_stride::mapping<extents<long long, 3, dynamic_extent, dynamic_extent>>;
            check(M2{M2::extents_type{4, 3}, array{12, 36, 1}}, false);

            using M3 = layout_stride::mapping<dextents<unsigned short, 3>>;
            check(M3{M3::extents_type{4, 3, 2}, array{8, 32, 2}}, false);
        }
    }
}

constexpr void check_construction_from_extents_and_array_or_span() {
    { // Check invalid construction
        using Mapping = layout_stride::mapping<extents<int, 3, 3>>;
        static_assert(!is_constructible_v<Mapping, extents<int, 2, 3>, array<int, 2>>);
        static_assert(!is_constructible_v<Mapping, extents<int, 2, 3>, span<int, 2>>);
        static_assert(!is_constructible_v<Mapping, extents<int, 3, 3>, array<int, 3>>);
        static_assert(!is_constructible_v<Mapping, extents<int, 3, 3>, span<int, 3>>);
        static_assert(!is_constructible_v<Mapping, dextents<int, 2>, array<int, 2>>);
        static_assert(!is_constructible_v<Mapping, dextents<int, 2>, span<int, 2>>);
        static_assert(!is_constructible_v<Mapping, dextents<int, 2>, array<int, 3>>);
        static_assert(!is_constructible_v<Mapping, dextents<int, 2>, span<int, 3>>);
        static_assert(!is_constructible_v<Mapping, extents<int, 3, 3>, array<NonConvertibleToAnything, 2>>);
        static_assert(!is_constructible_v<Mapping, extents<int, 3, 3>, span<NonConvertibleToAnything, 2>>);
        static_assert(!is_constructible_v<Mapping, extents<int, 3, 3>, array<ConvertibleToInt<int, IsNothrow::no>, 2>>);
        static_assert(!is_constructible_v<Mapping, extents<int, 3, 3>, span<ConvertibleToInt<int, IsNothrow::no>, 2>>);
    }

    using Ext = extents<int, 2, 4, 6>;
    Ext ext;

    { // Check construction from 'extents_type'
        array strs{24, 1, 4};
        layout_stride::mapping<Ext> m1(ext, strs);
        assert(m1.extents() == ext);
        static_assert(is_nothrow_constructible_v<decltype(m1), Ext, decltype(strs)>);

        span s{strs};
        layout_stride::mapping<Ext> m2(ext, s);
        assert(m2.extents() == ext);
        static_assert(is_nothrow_constructible_v<decltype(m2), Ext, decltype(s)>);
    }

    { // Check construction from other extents
        using Ext2 = extents<long, dynamic_extent, 4, dynamic_extent>;

        const array strs{1, 12, 2};
        layout_stride::mapping<Ext2> m1(ext, strs);
        assert(m1.extents() == ext);
        static_assert(is_nothrow_constructible_v<decltype(m1), Ext, decltype(strs)>);

        const span s{strs};
        layout_stride::mapping<Ext2> m2(ext, s);
        static_assert(is_nothrow_constructible_v<decltype(m2), Ext, decltype(s)>);
    }
}

constexpr void check_construction_from_other_mappings() {
    { // Check valid construction
        using Mapping = layout_stride::mapping<extents<int, 4, 4>>;
        static_assert(is_nothrow_constructible_v<Mapping, layout_left::mapping<extents<int, 4, 4>>>);
        static_assert(is_nothrow_constructible_v<Mapping, layout_right::mapping<extents<int, 4, 4>>>);
        static_assert(is_nothrow_constructible_v<Mapping, layout_left::mapping<extents<long, 4, 4>>>);
        static_assert(is_nothrow_constructible_v<Mapping, layout_right::mapping<extents<long, 4, 4>>>);
        static_assert(is_nothrow_constructible_v<Mapping, layout_stride::mapping<extents<long, 4, 4>>>);
        static_assert(is_nothrow_constructible_v<Mapping,
            LyingLayout<AlwaysUnique::yes, AlwaysStrided::yes>::mapping<extents<long, 4, 4>>>);
    }

    { // Check invalid construction
        using Mapping = layout_stride::mapping<extents<unsigned int, 4, 4>>;
        static_assert(!is_constructible_v<Mapping, NotLayoutMappingAlikeAtAll::mapping<extents<int, 4, 4>>>);
        static_assert(!is_constructible_v<Mapping, layout_left::mapping<extents<int, 4, 3>>>);
        static_assert(!is_constructible_v<Mapping, layout_right::mapping<extents<int, 3, 4>>>);
        static_assert(!is_constructible_v<Mapping, layout_left::mapping<extents<long, 4, 4, 4>>>);
        static_assert(!is_constructible_v<Mapping, layout_right::mapping<extents<long, 3, 3>>>);
        static_assert(!is_constructible_v<Mapping, layout_stride::mapping<extents<int, 4, 4, 3>>>);
        static_assert(!is_constructible_v<Mapping,
            LyingLayout<AlwaysUnique::no, AlwaysStrided::yes>::mapping<extents<long, 4, 4>>>);
        static_assert(!is_constructible_v<Mapping,
            LyingLayout<AlwaysUnique::yes, AlwaysStrided::no>::mapping<extents<long, 4, 4>>>);
        static_assert(!is_constructible_v<Mapping,
            LyingLayout<AlwaysUnique::no, AlwaysStrided::no>::mapping<extents<long, 4, 4>>>);
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
    { // Check N4950 [mdspan.layout.stride.expo]/1.1: Ext::rank() == 0
        using M1 = layout_stride::mapping<extents<int>>;
        static_assert(M1{}.required_span_size() == 1);

        M1 m1;
        assert(m1.required_span_size() == 1);
    }

    { // Check N4950 [mdspan.layout.stride.expo]/1.2: size of the multidimensional index space e is 0
        using M1 = layout_stride::mapping<extents<int, 3, 3, 0, 3>>;
        static_assert(M1{}.required_span_size() == 0);

        layout_stride::mapping<dextents<int, 4>> m2{dextents<int, 4>{3, 0, 3, 3}, array{1, 3, 1, 3}};
        assert(m2.required_span_size() == 0);
    }

    { // Check N4950 [mdspan.layout.stride.expo]/1.3: final case
        using M1 = layout_stride::mapping<extents<int, 3, 4, 3>>;
        static_assert(M1{}.required_span_size() == 36);

        layout_stride::mapping<dextents<int, 3>> m2{dextents<int, 3>{4, 3, 4}, array{1, 4, 12}};
        assert(m2.required_span_size() == 48);
    }
}

constexpr void check_is_exhaustive() {
    auto check = [](const auto& exts, const auto& strides, bool expected) {
        layout_stride::mapping m{exts, strides};
        assert(m.is_exhaustive() == expected);
    };

    // rank() is equal to 0
    check(extents<int>{}, array<int, 0>{}, true);

    // rank() is equal to 1
    check(extents<int, 0>{}, array{1}, true);
    check(dextents<int, 1>{0}, array{2}, false);
    check(extents<int, 1>{}, array{3}, false);
    check(dextents<int, 1>{2}, array{2}, false);
    check(extents<int, 3>{}, array{1}, true);
    check(dextents<int, 1>{4}, array{1}, true);

    // rank() is equal to 2
    check(extents<int, 3, 3>{}, array{1, 3}, true);
    check(extents<int, dynamic_extent, 3>{3}, array{3, 1}, true);
    check(extents<int, 3, dynamic_extent>{3}, array{4, 1}, false);
    check(dextents<int, 2>{3, 3}, array{3, 1}, true);
    check(extents<int, 4, 5>{}, array{5, 1}, true);
    check(extents<int, 6, dynamic_extent>{5}, array{1, 6}, true);
    check(extents<int, dynamic_extent, 7>{5}, array{1, 8}, false);
    check(dextents<int, 2>{6, 5}, array{1, 10}, false);
    check(extents<int, 0, 3>{}, array{3, 1}, true);
    check(extents<int, 0, 3>{}, array{6, 2}, false);
    check(extents<int, dynamic_extent, 3>{0}, array{6, 1}, false);
    check(extents<int, 0, dynamic_extent>{3}, array{6, 2}, false);
    check(dextents<int, 2>{0, 3}, array{7, 2}, false);
    check(extents<int, 0, 0>{}, array{1, 1}, false);
    check(extents<int, 0, dynamic_extent>{0, 0}, array{1, 1}, false);
    check(dextents<int, 2>{0, 0}, array{1, 2}, false);
    check(extents<int, 1, dynamic_extent>{0}, array{1, 2}, false);

    // rank() is greater than 2
    check(extents<int, 2, 3, 5>{}, array{1, 2, 6}, true);
    check(extents<int, dynamic_extent, 3, 5>{2}, array{1, 10, 2}, true);
    check(extents<int, 2, 3, dynamic_extent>{5}, array{3, 1, 6}, true);
    check(extents<int, dynamic_extent, dynamic_extent, 5>{2, 3}, array{15, 1, 3}, true);
    check(extents<int, 2, dynamic_extent, dynamic_extent>{3, 5}, array{5, 10, 1}, true);
    check(dextents<int, 3>{2, 3, 5}, array{15, 5, 1}, true);
    check(extents<int, 2, 5, 8>{}, array{1, 2, 12}, false);
    check(extents<int, 2, dynamic_extent, 8>{5}, array{8, 18, 1}, false);
    check(dextents<int, 3>{2, 5, 8}, array{5, 1, 12}, false);

    // rank() is greater than 2 and some extents are equal to 0
    check(extents<int, 2, 0, 7>{}, array{7, 14, 1}, true);
    check(extents<int, dynamic_extent, 0, 7>{2}, array{1, 14, 2}, true);
    check(extents<int, 2, dynamic_extent, 7>{0}, array{14, 28, 1}, false);
    check(extents<int, 2, dynamic_extent, dynamic_extent>{0, 7}, array{1, 2, 2}, false);
    check(dextents<int, 3>{2, 0, 7}, array{2, 28, 4}, false);
    check(extents<int, 5, 0, 0>{}, array{3, 1, 1}, false);
    check(extents<int, 5, dynamic_extent, 0>{0}, array{1, 5, 1}, false);
    check(dextents<int, 3>{5, 0, 0}, array{2, 1, 10}, false);
    check(extents<int, 0, 0, 0>{}, array{1, 1, 1}, false);
    check(extents<int, 0, 1, 1>{}, array{1, 1, 1}, true);

    // rank() is greater than 2 - one extent is equal to 0 while others are equal to each other
    check(extents<int, 3, 0, 3>{}, array{1, 9, 3}, true);
    check(extents<int, dynamic_extent, 0, 3>{3}, array{3, 9, 1}, true);
    check(extents<int, 3, dynamic_extent, dynamic_extent>{0, 3}, array{1, 3, 3}, false);
    check(dextents<int, 3>{3, 0, 3}, array{1, 4, 8}, false);
    check(dextents<int, 3>{0, 1, 1}, array{1, 1, 1}, true);

    // required_span_size() is equal to 1
    check(extents<int, 1>{}, array{1}, true);
    check(dextents<int, 1>{1}, array{3}, false);
    check(extents<int, 1, dynamic_extent>{1}, array{1, 1}, true);
    check(extents<int, 1, 1, 1>{}, array{1, 2, 1}, false);

    // Mapping is exhaustive, but is_exhaustive() should return false because of the way standard defined this function
    check(extents<int, 3, 1>{}, array{1, 4}, false);
    check(dextents<int, 3>{5, 1, 2}, array{2, 11, 1}, false);
    check(dextents<int, 3>{2, 3, 1}, array{3, 1, 8}, false);
    check(extents<int, 1, dynamic_extent, 7>{6}, array{50, 7, 1}, false);
    check(dextents<int, 2>{1, 2}, array{5, 1}, false);
    check(extents<int, 6, 1>{}, array{1, 10}, false);
    check(dextents<int, 3>{2, 1, 2}, array{3, 3, 1}, false);
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

constexpr void check_stride_function() {
    using Ext = extents<unsigned int, 2, 4, 6, 8, 10>;
    layout_stride::mapping<Ext> even_mapping{Ext{}, array{80, 160, 640, 1, 8}};

    { // Check return type
        same_as<unsigned int> decltype(auto) s = even_mapping.stride(0);
        assert(s == 80);
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

        assert(even_mapping.stride(ConvertibleToRankType{}) == 160);
    }

    { // Check other strides
        assert(even_mapping.stride(2) == 640);
        assert(even_mapping.stride(3) == 1);
        assert(even_mapping.stride(4) == 8);
    }
}

constexpr void check_comparisons() {
    using E                    = extents<int, 2, 3>;
    using StaticStrideMapping  = layout_stride::mapping<E>;
    using DynamicStrideMapping = layout_stride::mapping<dextents<int, 2>>;
    using RightMapping         = layout_right::mapping<E>;
    using LeftMapping          = layout_left::mapping<E>;

    { // Check equality_comparable_with concept (correct comparisons)
        static_assert(equality_comparable_with<StaticStrideMapping, DynamicStrideMapping>);
        static_assert(equality_comparable_with<StaticStrideMapping, RightMapping>);
        static_assert(equality_comparable_with<StaticStrideMapping, LeftMapping>);
        static_assert(equality_comparable_with<DynamicStrideMapping, RightMapping>);
        static_assert(equality_comparable_with<DynamicStrideMapping, LeftMapping>);
    }

    { // Check equality_comparable_with concept (incorrect comparisons)
        static_assert(
            !equality_comparable_with<StaticStrideMapping, NotLayoutMappingAlikeAtAll::mapping<extents<int, 2, 3>>>);
        static_assert(!equality_comparable_with<DynamicStrideMapping, layout_stride::mapping<extents<int, 2, 3, 4>>>);
        static_assert(!equality_comparable_with<StaticStrideMapping, layout_stride::mapping<dextents<int, 3>>>);
        static_assert(!equality_comparable_with<DynamicStrideMapping, layout_right::mapping<extents<int, 2>>>);
        static_assert(!equality_comparable_with<StaticStrideMapping, layout_left::mapping<dextents<int, 1>>>);
        static_assert(!equality_comparable_with<DynamicStrideMapping,
            LyingLayout<AlwaysUnique::yes, AlwaysStrided::no>::mapping<dextents<int, 2>>>);
        static_assert(!equality_comparable_with<StaticStrideMapping,
            LyingLayout<AlwaysUnique::yes, AlwaysStrided::no>::mapping<extents<int, 2, 3>>>);
        static_assert(!equality_comparable_with<DynamicStrideMapping,
            LyingLayout<AlwaysUnique::no, AlwaysStrided::no>::mapping<dextents<int, 2>>>);
        static_assert(!equality_comparable_with<StaticStrideMapping,
            LyingLayout<AlwaysUnique::no, AlwaysStrided::no>::mapping<extents<int, 2, 3>>>);
    }

    { // Check correctness: layout_stride::mapping with layout_stride::mapping
        StaticStrideMapping m1{E{}, array{3, 1}};
        DynamicStrideMapping m2{dextents<int, 2>{2, 3}, array{3, 1}};
        same_as<bool> decltype(auto) cond = m1 == m2;
        assert(cond); // extents are equal, OFFSET(rhs) == 0, strides are equal

        DynamicStrideMapping m3{dextents<int, 2>{2, 3}, array{1, 2}};
        assert(m1 != m3); // extents are equal, OFFSET(rhs) == 0, strides are not equal
        assert(m2 != m3); // ditto

        DynamicStrideMapping m4{dextents<int, 2>{1, 3}, array{3, 1}};
        assert(m1 != m4); // extents are not equal, OFFSET(rhs) == 0, strides are equal
        assert(m2 != m4); // ditto
        assert(m3 != m4); // extents are not equal, OFFSET(rhs) == 0, strides are not equal

        // NB: OFFSET(layout_stride::mapping) is always equal to 0

        static_assert(noexcept(m1 == m2));
        static_assert(noexcept(m1 != m3));
    }

    { // Check correctness: layout_stride::mapping with layout_left::mapping
        LeftMapping m1;
        StaticStrideMapping m2{E{}, array{1, 2}};
        same_as<bool> decltype(auto) cond = m1 == m2;
        assert(cond); // extents are equal, OFFSET(rhs) == 0, strides are equal

        DynamicStrideMapping m3{dextents<int, 2>{2, 3}, array{3, 1}};
        assert(m1 != m3); // extents are equal, OFFSET(rhs) == 0, strides are not equal
        assert(m2 != m3); // ditto

        DynamicStrideMapping m4{dextents<int, 2>{2, 1}, array{1, 2}};
        assert(m1 != m4); // extents are not equal, OFFSET(rhs) == 0, strides are equal
        assert(m2 != m4); // ditto
        assert(m3 != m4); // extents are not equal, OFFSET(rhs) == 0, strides are not equal

        // NB: OFFSET(layout_left::mapping) is always equal to 0

        static_assert(noexcept(m1 == m2));
        static_assert(noexcept(m1 != m3));
    }

    { // Check correctness: layout_stride::mapping with layout_right::mapping
        RightMapping m1;
        StaticStrideMapping m2{E{}, array{3, 1}};
        same_as<bool> decltype(auto) cond = m1 == m2;
        assert(cond); // extents are equal, OFFSET(rhs) == 0, strides are equal

        DynamicStrideMapping m3{dextents<int, 2>{2, 3}, array{1, 2}};
        assert(m1 != m3); // extents are equal, OFFSET(rhs) == 0, strides are not equal
        assert(m2 != m3); // ditto

        DynamicStrideMapping m4{dextents<int, 2>{1, 3}, array{3, 1}};
        assert(m1 != m4); // extents are not equal, OFFSET(rhs) == 0, strides are equal
        assert(m2 != m4); // ditto
        assert(m3 != m4); // extents are not equal, OFFSET(rhs) == 0, strides are not equal

        // NB: OFFSET(layout_right::mapping) is always equal to 0

        static_assert(noexcept(m1 == m2));
        static_assert(noexcept(m1 != m3));
    }

    { // Check correctness: layout_stride::mapping with LyingLayout<AlwaysUnique::no, AlwaysStrided::yes>::mapping
        using CustomMapping = LyingLayout<AlwaysUnique::no, AlwaysStrided::yes>::mapping<E>;
        CustomMapping m1;
        StaticStrideMapping m2{E{}, array{1, 2}};
        same_as<bool> decltype(auto) cond = m1 == m2;
        assert(cond); // extents are equal, OFFSET(rhs) == 0, strides are equal

        DynamicStrideMapping m3{dextents<int, 2>{2, 3}, array{3, 1}};
        assert(m1 != m3); // extents are equal, OFFSET(rhs) == 0, strides are not equal
        assert(m2 != m3); // ditto

        DynamicStrideMapping m4{dextents<int, 2>{1, 3}, array{1, 2}};
        assert(m1 != m4); // extents are not equal, OFFSET(rhs) == 0, strides are equal
        assert(m2 != m4); // ditto
        assert(m3 != m4); // extents are not equal, OFFSET(rhs) == 0, strides are not equal

        // NB: OFFSET(CustomMapping) is always equal to 0

        static_assert(noexcept(m1 == m2));
        static_assert(noexcept(m1 != m3));
    }

    { // Check correctness: layout_stride::mapping with HollowLayout::mapping
        HollowLayout::mapping<extents<short>> m1;
        if constexpr (!is_permissive) {
            if (!is_constant_evaluated()) { // too heavy for compile time
                const auto props = get_mapping_properties(m1);
                assert(props.req_span_size == m1.required_span_size());
                assert(props.uniqueness);
                assert(props.exhaustiveness);
                assert(props.strideness);
            }
        }

        layout_stride::mapping<extents<unsigned short>> m2;
        same_as<bool> decltype(auto) cond = m1 == m2;
        assert(!cond); // extents are equal, OFFSET(rhs) != 0, strides are equal

        static_assert(noexcept(m1 == m2));
        static_assert(noexcept(m1 != m2));
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

#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
        assert((vec[0] == 1));
        assert((vec[1] == 2));
        assert((vec[2] == 3));
#else // ^^^ defined(__cpp_multidimensional_subscript) / !defined(__cpp_multidimensional_subscript) vvv
        assert((vec[array{0}] == 1));
        assert((vec[array{1}] == 2));
        assert((vec[array{2}] == 3));
#endif // ^^^ !defined(__cpp_multidimensional_subscript) ^^^
    }

    { // 2x3 matrix with row-major order
        using E = extents<int, 2, 3>;
        const array vals{1, 2, 3, 4, 5, 6};
        layout_stride::mapping<E> m{E{}, array{3, 1}};
        mdspan<const int, E, layout_stride> matrix{vals.data(), m};

#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
        assert((matrix[0, 0] == 1));
        assert((matrix[0, 1] == 2));
        assert((matrix[0, 2] == 3));
        assert((matrix[1, 0] == 4));
        assert((matrix[1, 1] == 5));
        assert((matrix[1, 2] == 6));
#else // ^^^ defined(__cpp_multidimensional_subscript) / !defined(__cpp_multidimensional_subscript) vvv
        assert((matrix[array{0, 0}] == 1));
        assert((matrix[array{0, 1}] == 2));
        assert((matrix[array{0, 2}] == 3));
        assert((matrix[array{1, 0}] == 4));
        assert((matrix[array{1, 1}] == 5));
        assert((matrix[array{1, 2}] == 6));
#endif // ^^^ !defined(__cpp_multidimensional_subscript) ^^^
    }

    { // 3x2x2 tensor
        using E = extents<int, 3, 2, 2>;
        const array vals{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
        layout_stride::mapping<E> m{E{}, array{7, 1, 3}}; // non-exhaustive mapping
        assert(!m.is_exhaustive());
        mdspan<const int, E, layout_stride> tensor{vals.data(), m};

#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
        assert((tensor[0, 0, 0] == 0));
        assert((tensor[0, 0, 1] == 3));
        assert((tensor[0, 1, 0] == 1));
        assert((tensor[0, 1, 1] == 4));
        assert((tensor[1, 0, 0] == 7));
        assert((tensor[1, 0, 1] == 10));
        assert((tensor[1, 1, 0] == 8));
        assert((tensor[1, 1, 1] == 11));
        assert((tensor[2, 0, 0] == 14));
        assert((tensor[2, 0, 1] == 17));
        assert((tensor[2, 1, 0] == 15));
        assert((tensor[2, 1, 1] == 18));
#else // ^^^ defined(__cpp_multidimensional_subscript) / !defined(__cpp_multidimensional_subscript) vvv
        assert((tensor[array{0, 0, 0}] == 0));
        assert((tensor[array{0, 0, 1}] == 3));
        assert((tensor[array{0, 1, 0}] == 1));
        assert((tensor[array{0, 1, 1}] == 4));
        assert((tensor[array{1, 0, 0}] == 7));
        assert((tensor[array{1, 0, 1}] == 10));
        assert((tensor[array{1, 1, 0}] == 8));
        assert((tensor[array{1, 1, 1}] == 11));
        assert((tensor[array{2, 0, 0}] == 14));
        assert((tensor[array{2, 0, 1}] == 17));
        assert((tensor[array{2, 1, 0}] == 15));
        assert((tensor[array{2, 1, 1}] == 18));
#endif // ^^^ !defined(__cpp_multidimensional_subscript) ^^^
    }

    { // 2x3x3x2 tensor
        using E = extents<int, 2, 3, 3, 2>;
        const array vals{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
            26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
        layout_stride::mapping<E> m{E{}, array{18, 1, 3, 9}}; // exhaustive mapping
        assert(m.is_exhaustive());
        mdspan<const int, E, layout_stride> tensor{vals.data(), m};

#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
        assert((tensor[0, 0, 0, 0] == 0));
        assert((tensor[0, 0, 0, 1] == 9));
        assert((tensor[0, 0, 1, 0] == 3));
        assert((tensor[0, 0, 1, 1] == 12));
        assert((tensor[0, 1, 0, 0] == 1));
        assert((tensor[0, 1, 0, 1] == 10));
        assert((tensor[0, 1, 1, 0] == 4));
        assert((tensor[0, 1, 1, 1] == 13));
        assert((tensor[1, 0, 0, 0] == 18));
        assert((tensor[1, 0, 0, 1] == 27));
        assert((tensor[1, 0, 1, 0] == 21));
        assert((tensor[1, 0, 1, 1] == 30));
        assert((tensor[1, 1, 0, 0] == 19));
        assert((tensor[1, 1, 0, 1] == 28));
        assert((tensor[1, 1, 1, 0] == 22));
        assert((tensor[1, 1, 1, 1] == 31));
        assert((tensor[0, 2, 2, 0] == 8));
        assert((tensor[1, 2, 2, 1] == 35));
#else // ^^^ defined(__cpp_multidimensional_subscript) / !defined(__cpp_multidimensional_subscript) vvv
        assert((tensor[array{0, 0, 0, 0}] == 0));
        assert((tensor[array{0, 0, 0, 1}] == 9));
        assert((tensor[array{0, 0, 1, 0}] == 3));
        assert((tensor[array{0, 0, 1, 1}] == 12));
        assert((tensor[array{0, 1, 0, 0}] == 1));
        assert((tensor[array{0, 1, 0, 1}] == 10));
        assert((tensor[array{0, 1, 1, 0}] == 4));
        assert((tensor[array{0, 1, 1, 1}] == 13));
        assert((tensor[array{1, 0, 0, 0}] == 18));
        assert((tensor[array{1, 0, 0, 1}] == 27));
        assert((tensor[array{1, 0, 1, 0}] == 21));
        assert((tensor[array{1, 0, 1, 1}] == 30));
        assert((tensor[array{1, 1, 0, 0}] == 19));
        assert((tensor[array{1, 1, 0, 1}] == 28));
        assert((tensor[array{1, 1, 1, 0}] == 22));
        assert((tensor[array{1, 1, 1, 1}] == 31));
        assert((tensor[array{0, 2, 2, 0}] == 8));
        assert((tensor[array{1, 2, 2, 1}] == 35));
#endif // ^^^ !defined(__cpp_multidimensional_subscript) ^^^
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

    // Check degenerate extents
    check_members(extents<long>{}, array<int, 0>{});
    check_members(extents<unsigned char>{}, array<int, 0>{});

    if (!is_constant_evaluated()) { // too heavy for compile time
        check_mapping_properties();
    }
    check_construction_from_extents_and_array_or_span();
    check_construction_from_other_mappings();
    check_required_span_size();
    check_is_exhaustive();
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
