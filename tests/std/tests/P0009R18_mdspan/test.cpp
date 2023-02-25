// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <concepts>
#include <mdspan>
#include <type_traits>
#include <vector>

using namespace std;

#ifdef __cpp_lib_concepts
// A type that's regular and trivially copyable, and also maximally nothrow.
template <class T>
using is_regular_trivial_nothrow = std::conjunction<std::bool_constant<std::regular<T>>, is_trivially_copyable<T>,
    is_nothrow_default_constructible<T>, is_nothrow_copy_constructible<T>, is_nothrow_move_constructible<T>,
    is_nothrow_copy_assignable<T>, is_nothrow_move_assignable<T>, is_nothrow_swappable<T>>;

template <class T>
inline constexpr bool is_regular_trivial_nothrow_v = is_regular_trivial_nothrow<T>::value;
#endif // __cpp_lib_concepts

struct Constructible {
    // noexcept constructible for size_t, but not convertible
    explicit operator size_t() noexcept;
};

struct Convertible {
    // convertible, but not noexcept constructible
    operator size_t();
};

struct ConstructibleAndConvertible {
    // convertible and noexcept constuctible
    constexpr operator size_t() noexcept {
        return size_t{0};
    };
};

struct ConstructibleAndConvertibleConst {
    // convertible and noexcept constuctible
    constexpr operator size_t() const noexcept {
        return size_t{0};
    };
};


void extent_tests_traits() {
#ifdef __cpp_lib_concepts
    static_assert(is_regular_trivial_nothrow_v<extents<size_t>>);
    static_assert(is_regular_trivial_nothrow_v<extents<size_t, 2, 3>>);
    static_assert(is_regular_trivial_nothrow_v<extents<size_t, dynamic_extent, 3>>);
    static_assert(is_regular_trivial_nothrow_v<extents<size_t, 2, dynamic_extent>>);
    static_assert(is_regular_trivial_nothrow_v<extents<size_t, dynamic_extent, dynamic_extent>>);
#endif // __cpp_lib_concepts

    static_assert(is_same_v<dextents<size_t, 1>, extents<size_t, dynamic_extent>>);
    static_assert(is_same_v<dextents<size_t, 2>, extents<size_t, dynamic_extent, dynamic_extent>>);
    static_assert(is_same_v<dextents<short, 1>, extents<short, dynamic_extent>>);
    static_assert(is_same_v<dextents<short, 2>, extents<short, dynamic_extent, dynamic_extent>>);

    constexpr extents e(2, 3);
    static_assert(is_same_v<remove_cvref_t<decltype(e)>, dextents<size_t, 2>>);
    static_assert(e.static_extent(0) == dynamic_extent);
    static_assert(e.static_extent(1) == dynamic_extent);
    static_assert(e.extent(0) == 2);
    static_assert(e.extent(1) == 3);

    constexpr dextents<int, 1> ex0(1);
    (void) ex0;
    static_assert(!is_constructible_v<dextents<int, 1>, int*>);

    extents<signed char>();
    extents<unsigned char>();
    extents<short>();
    extents<unsigned short>();
    extents<int>();
    extents<unsigned int>();
    extents<long>();
    extents<unsigned long>();
    extents<long long>();
    extents<unsigned long long>();
}

void extent_tests_rank() {
    static_assert(extents<size_t>::rank() == 0);
    static_assert(extents<size_t>::rank_dynamic() == 0);

    static_assert(extents<size_t, 2>::rank() == 1);
    static_assert(extents<size_t, 2>::rank_dynamic() == 0);

    static_assert(extents<size_t, dynamic_extent>::rank() == 1);
    static_assert(extents<size_t, dynamic_extent>::rank_dynamic() == 1);

    static_assert(extents<size_t, 2, 3>::rank() == 2);
    static_assert(extents<size_t, 2, 3>::rank_dynamic() == 0);

    static_assert(extents<size_t, 2, dynamic_extent>::rank() == 2);
    static_assert(extents<size_t, 2, dynamic_extent>::rank_dynamic() == 1);

    static_assert(extents<size_t, dynamic_extent, 3>::rank() == 2);
    static_assert(extents<size_t, dynamic_extent, 3>::rank_dynamic() == 1);

    static_assert(extents<size_t, dynamic_extent, dynamic_extent>::rank() == 2);
    static_assert(extents<size_t, dynamic_extent, dynamic_extent>::rank_dynamic() == 2);
}

void extent_tests_static_extent() {
    static_assert(extents<size_t, 2, 3>::static_extent(0) == 2);
    static_assert(extents<size_t, 2, 3>::static_extent(1) == 3);

    static_assert(extents<size_t, 2, dynamic_extent>::static_extent(0) == 2);
    static_assert(extents<size_t, 2, dynamic_extent>::static_extent(1) == dynamic_extent);

    static_assert(extents<size_t, dynamic_extent, 3>::static_extent(0) == dynamic_extent);
    static_assert(extents<size_t, dynamic_extent, 3>::static_extent(1) == 3);

    static_assert(extents<size_t, dynamic_extent, dynamic_extent>::static_extent(0) == dynamic_extent);
    static_assert(extents<size_t, dynamic_extent, dynamic_extent>::static_extent(1) == dynamic_extent);
}

void extent_tests_extent() {
    constexpr extents<size_t, 2, 3> e_23;
    static_assert(e_23.extent(0) == 2);
    static_assert(e_23.extent(1) == 3);

    constexpr extents<size_t, 2, dynamic_extent> e_2d{3};
    static_assert(e_2d.extent(0) == 2);
    static_assert(e_2d.extent(1) == 3);

    constexpr extents<size_t, dynamic_extent, dynamic_extent> e_dd{2, 3};
    static_assert(e_dd.extent(0) == 2);
    static_assert(e_dd.extent(1) == 3);

    constexpr extents<size_t, 2, dynamic_extent, dynamic_extent> e_2dd{3, 5};
    static_assert(e_2dd.extent(0) == 2);
    static_assert(e_2dd.extent(1) == 3);
    static_assert(e_2dd.extent(2) == 5);
}

void extent_tests_ctor_other_sizes() {
    static_assert(!is_constructible_v<extents<size_t, 2>, Constructible>);
    static_assert(!is_constructible_v<extents<size_t, 2>, Convertible>);
    // static_assert(is_constructible_v<extents<size_t, 2>, ConstructibleAndConvertible>);
    constexpr extents<size_t, 2> ex0{ConstructibleAndConvertible{}};
    // static_assert(is_constructible_v<extents<size_t, 2>, ConstructibleAndConvertibleConst>);
    constexpr extents<size_t, 2> ex1{ConstructibleAndConvertibleConst{}};

    // static_assert(is_constructible_v<extents<int, dynamic_extent, 2, 2>, int>);
    constexpr extents<int, dynamic_extent, 2, 2> ex2(1);
    static_assert(!is_constructible_v<extents<int, dynamic_extent, 2, 2>, int, int>);
    // static_assert(is_constructible_v<extents<int, dynamic_extent, 2, 2>, int, int, int>);
    extents<int, dynamic_extent, 2, 2> ex3(1, 2, 3);

    (void) ex0;
    (void) ex1;
    (void) ex2;
    (void) ex3;

    extents<size_t, 2, 3> e0;
    assert(e0.extent(0) == 2);
    assert(e0.extent(1) == 3);

    extents<size_t, 2, dynamic_extent> e1(5);
    assert(e1.extent(0) == 2);
    assert(e1.extent(1) == 5);

    extents<size_t, dynamic_extent, 3> e2(5);
    assert(e2.extent(0) == 5);
    assert(e2.extent(1) == 3);

    extents<size_t, dynamic_extent, dynamic_extent> e3(5, 7);
    assert(e3.extent(0) == 5);
    assert(e3.extent(1) == 7);
}

void extent_tests_copy_ctor_other() {
    // Rank and value of static extents must match.
    static_assert(!is_constructible_v<extents<size_t, 2>, extents<size_t, 2, 3>>);
    static_assert(!is_constructible_v<extents<size_t, 2, 3>, extents<size_t, 2>>);
    static_assert(!is_constructible_v<extents<size_t, 2, 3>, extents<size_t, 3, 2>>);

    // Static extents are constuctible, but not convertible, from dynamic extents.
    // static_assert(is_constructible_v<extents<size_t, 2, 3>, extents<size_t, 2, dynamic_extent>>);
    constexpr extents<size_t, 2, 3> ex0{extents<size_t, 2, dynamic_extent>{}};
    (void) ex0;
    static_assert(!is_convertible_v<extents<size_t, 2, dynamic_extent>, extents<size_t, 2, 3>>);

    // Dynamic extents are constuctible and convertible from static extents.
    static_assert(is_constructible_v<extents<size_t, 2, dynamic_extent>, extents<size_t, 2, 3>>);
    extents<size_t, 2, dynamic_extent>{extents<size_t, 2, 3>{}};
    static_assert(is_convertible_v<extents<size_t, 2, 3>, extents<size_t, 2, dynamic_extent>>);

    // Can implicitly convert from narrower to wider size_type, but not vice-versa.
    static_assert(is_convertible_v<extents<uint32_t, dynamic_extent>, extents<uint64_t, dynamic_extent>>);
    static_assert(!is_convertible_v<extents<uint64_t, dynamic_extent>, extents<uint32_t, dynamic_extent>>);

    extents<size_t, 2, dynamic_extent> e_dyn(3);
    extents<size_t, 2, 3> e(e_dyn);
    (void) e;

    using E = extents<size_t, 2, 3>;

    extents<size_t, 2, 3> e0{extents<size_t, 2, 3>{}};
    E e1(extents<size_t, dynamic_extent, 3>(2u));
    extents<size_t, 2, 3> e2{extents<size_t, 2, dynamic_extent>{3u}};
    extents<size_t, 2, 3> e3{extents<size_t, dynamic_extent, dynamic_extent>{2u, 3u}};

    (void) e0;
    (void) e1;
    (void) e2;
    (void) e3;
}

template <class T, class IndexType, size_t N, class = void>
struct is_array_cons_avail : std::false_type {};

template <class T, class IndexType, size_t N>
struct is_array_cons_avail<T, IndexType, N,
    std::enable_if_t<std::is_same<decltype(T{std::declval<std::array<IndexType, N>>()}), T>::value>> : std::true_type {
};

template <class T, class IndexType, size_t N>
constexpr bool is_array_cons_avail_v = is_array_cons_avail<T, IndexType, N>::value;

void extent_tests_ctor_array() {
    static_assert(!is_constructible_v<extents<size_t, 2>, array<Constructible, 1>>);
    static_assert(!is_constructible_v<extents<size_t, 2>, array<Convertible, 1>>);
    static_assert(!is_constructible_v<extents<size_t, 2>, array<ConstructibleAndConvertible, 1>>);
    static_assert(is_constructible_v<extents<size_t, 2>, array<ConstructibleAndConvertibleConst, 1>>);
    constexpr extents<size_t, 2> ex0{array<ConstructibleAndConvertibleConst, 1>{}};
    (void) ex0;

    static_assert(is_constructible_v<extents<int, dynamic_extent, 2, 2>, array<int, 1>>);
    constexpr extents<int, dynamic_extent, 2, 2> ex1{array<int, 1>{}};
    static_assert(!is_constructible_v<extents<int, dynamic_extent, 2, 2>, array<int, 2>>);
    static_assert(is_constructible_v<extents<int, dynamic_extent, 2, 2>, array<int, 3>>);
    constexpr extents<int, dynamic_extent, 2, 2> ex2{array<int, 3>{1, 2, 2}};
    (void) ex1;
    (void) ex2;

    static_assert(is_constructible_v<extents<int, 10>, array<int, 0>>);
    constexpr extents<int, 10> ex3{array<int, 0>{}};
    static_assert(is_constructible_v<extents<int, 10>, array<int, 1>>);
    constexpr extents<int, 10> ex4{array<int, 1>{}};
    static_assert(!is_constructible_v<extents<int, 10>, array<int, 2>>);
    (void) ex3;
    (void) ex4;

    extents<size_t, 2, 3> e0;
    assert(e0.extent(0) == 2u);
    assert(e0.extent(1) == 3u);

    // native extent::size_type
    extents<size_t, 2, dynamic_extent> e1(to_array<size_t>({5}));
    assert(e1.extent(0) == 2u);
    assert(e1.extent(1) == 5u);

    extents<size_t, dynamic_extent, 3> e2(to_array<size_t>({5}));
    assert(e2.extent(0) == 5u);
    assert(e2.extent(1) == 3u);

    extents<size_t, dynamic_extent, dynamic_extent> e3(to_array<size_t>({5, 7}));
    assert(e3.extent(0) == 5u);
    assert(e3.extent(1) == 7u);

    // convertible size type
    extents<size_t, 2, dynamic_extent> e4(to_array<int>({5}));
    assert(e4.extent(0) == 2u);
    assert(e4.extent(1) == 5u);

    extents<size_t, dynamic_extent, 3> e5(to_array<int>({5}));
    assert(e5.extent(0) == 5u);
    assert(e5.extent(1) == 3u);

    extents<size_t, dynamic_extent, dynamic_extent> e6(to_array<int>({5, 7}));
    assert(e6.extent(0) == 5u);
    assert(e6.extent(1) == 7u);
}

void extent_tests_ctor_span() {
    static_assert(!is_constructible_v<extents<size_t, 2>, span<Constructible, 1>>);
    static_assert(!is_constructible_v<extents<size_t, 2>, span<Convertible, 1>>);
    static_assert(!is_constructible_v<extents<size_t, 2>, span<ConstructibleAndConvertible, 1>>);
    static_assert(is_constructible_v<extents<size_t, 2>, span<ConstructibleAndConvertibleConst, 1>>);
    ConstructibleAndConvertibleConst arr0[1] = {{}};
    constexpr extents<size_t, 2> ex0{span<ConstructibleAndConvertibleConst, 1>{arr0}};
    (void) ex0;

    static_assert(is_constructible_v<extents<int, dynamic_extent, 2, 2>, span<int, 1>>);
    constexpr int arr1[1] = {1};
    constexpr extents<int, dynamic_extent, 2, 2> ex1{span<const int, 1>{arr1}};
    static_assert(!is_constructible_v<extents<int, dynamic_extent, 2, 2>, span<int, 2>>);
    static_assert(is_constructible_v<extents<int, dynamic_extent, 2, 2>, span<int, 3>>);
    constexpr int arr2[3] = {3, 2, 2};
    constexpr extents<int, dynamic_extent, 2, 2> ex2{span<const int, 3>{arr2}};
    (void) ex1;
    (void) ex2;


    extents<size_t, 2, 3> e0;
    assert(e0.extent(0) == 2u);
    assert(e0.extent(1) == 3u);

    // native extent::size_type
    constexpr int one_int[] = {5};
    constexpr int two_int[] = {5, 7};
    extents<size_t, 2, dynamic_extent> e1(span{one_int});
    assert(e1.extent(0) == 2);
    assert(e1.extent(1) == 5);

    extents<size_t, dynamic_extent, 3> e2(span{one_int});
    assert(e2.extent(0) == 5);
    assert(e2.extent(1) == 3);

    extents<size_t, dynamic_extent, dynamic_extent> e3(span{two_int});
    assert(e3.extent(0) == 5);
    assert(e3.extent(1) == 7);

    // convertible size type
    constexpr size_t one_sizet[] = {5};
    constexpr size_t two_sizet[] = {5, 7};
    extents<size_t, 2, dynamic_extent> e4(span{one_sizet});
    assert(e4.extent(0) == 2);
    assert(e4.extent(1) == 5);

    extents<size_t, dynamic_extent, 3> e5(span{one_sizet});
    assert(e5.extent(0) == 5);
    assert(e5.extent(1) == 3);

    extents<size_t, dynamic_extent, dynamic_extent> e6(span{two_sizet});
    assert(e6.extent(0) == 5);
    assert(e6.extent(1) == 7);
}

void extent_tests_equality() {
    static_assert(extents<size_t, 2, 3>{} == extents<size_t, 2, 3>{});
    static_assert(extents<size_t, 2, 3>{} != extents<size_t, 3, 2>{});
    static_assert(extents<size_t, 2>{} != extents<size_t, 2, 3>{});

    extents<size_t, 2, 3> e_23;
    extents<size_t, 2, dynamic_extent> e_2d{3};
    extents<size_t, dynamic_extent, 3> e_d3{2};
    extents<size_t, dynamic_extent, dynamic_extent> e_dd{2, 3};

    assert(e_23 == e_2d);
    assert(e_23 == e_d3);
    assert(e_23 == e_dd);
    assert(e_2d == e_d3);
}

template <class Mapping, enable_if_t<Mapping::extents_type::rank() == 2, int> = 0>
void TestMapping(const Mapping& map) {
    using IndexT = typename Mapping::index_type;
    using RankT  = typename Mapping::rank_type;
    static_assert(is_same_v<IndexT, decltype(declval<Mapping>()(IndexT{0}, IndexT{0}))>);
    static_assert(is_same_v<IndexT, decltype(declval<Mapping>().stride(RankT{0}))>);

    array<IndexT, Mapping::extents_type::rank()> s;
    const auto& e      = map.extents();
    size_t num_entries = 1;
    for (size_t i = 0; i < Mapping::extents_type::rank(); ++i) {
        num_entries *= e.extent(i);
        s[i] = map.stride(i);
    }

    vector<IndexT> indices;
    indices.reserve(num_entries);

    for (IndexT i = 0; i < e.extent(0); ++i) {
        for (IndexT j = 0; j < e.extent(1); ++j) {
            const auto idx = i * s[0] + j * s[1];
            assert(map(i, j) == idx);
            indices.push_back(idx);
        }
    }

    bool is_unique  = true;
    bool is_exhaust = true;
    sort(indices.begin(), indices.end());
    for (size_t i = 1; i < indices.size(); ++i) {
        const auto diff = indices[i] - indices[i - 1];
        if (diff == 0) {
            is_unique = false;
        } else if (diff != 1) {
            is_exhaust = false;
        }
    }

    assert(map.is_unique() == is_unique);
    assert(map.is_exhaustive() == is_exhaust);
    assert(map.required_span_size() == indices.back() + 1);
}

template <class Mapping, enable_if_t<Mapping::extents_type::rank() == 3, int> = 0>
void TestMapping(const Mapping& map) {
    using IndexT = typename Mapping::index_type;
    using RankT  = typename Mapping::rank_type;
    static_assert(is_same_v<IndexT, decltype(declval<Mapping>()(IndexT{0}, IndexT{0}, IndexT{0}))>);
    static_assert(is_same_v<IndexT, decltype(declval<Mapping>().stride(RankT{0}))>);

    array<IndexT, Mapping::extents_type::rank()> s;
    const auto& e      = map.extents();
    size_t num_entries = 1;
    for (size_t i = 0; i < Mapping::extents_type::rank(); ++i) {
        num_entries *= e.extent(i);
        s[i] = map.stride(i);
    }

    vector<IndexT> indices;
    indices.reserve(num_entries);

    for (IndexT i = 0; i < e.extent(0); ++i) {
        for (IndexT j = 0; j < e.extent(1); ++j) {
            for (IndexT k = 0; k < e.extent(2); ++k) {
                const auto idx = i * s[0] + j * s[1] + k * s[2];
                assert(map(i, j, k) == idx);
                indices.push_back(idx);
            }
        }
    }

    bool is_unique  = true;
    bool is_exhaust = true;
    sort(indices.begin(), indices.end());
    for (size_t i = 1; i < indices.size(); ++i) {
        const auto diff = indices[i] - indices[i - 1];
        if (diff == 0) {
            is_unique = false;
        } else if (diff != 1) {
            is_exhaust = false;
        }
    }

    assert(map.is_unique() == is_unique);
    assert(map.is_exhaustive() == is_exhaust);
    assert(map.required_span_size() == indices.back() + 1);
}

void layout_left_tests_traits() {
#ifdef __cpp_lib_concepts
    static_assert(is_regular_trivial_nothrow_v<layout_left::mapping<extents<size_t, 2, 3>>>);
    static_assert(is_regular_trivial_nothrow_v<layout_left::mapping<extents<size_t, dynamic_extent, 3>>>);
    static_assert(is_regular_trivial_nothrow_v<layout_left::mapping<extents<size_t, 2, dynamic_extent>>>);
    static_assert(is_regular_trivial_nothrow_v<layout_left::mapping<extents<size_t, dynamic_extent, dynamic_extent>>>);
#endif // __cpp_lib_concepts

    using E = extents<int, 2, 3>;
    static_assert(is_same_v<layout_left::mapping<E>::extents_type, E>);
    static_assert(is_same_v<layout_left::mapping<E>::index_type, E::index_type>);
    static_assert(is_same_v<layout_left::mapping<E>::size_type, E::size_type>);
    static_assert(is_same_v<layout_left::mapping<E>::rank_type, E::rank_type>);
    static_assert(is_same_v<layout_left::mapping<E>::layout_type, layout_left>);
}

void layout_right_tests_traits() {
#ifdef __cpp_lib_concepts
    static_assert(is_regular_trivial_nothrow_v<layout_right::mapping<extents<size_t, 2, 3>>>);
    static_assert(is_regular_trivial_nothrow_v<layout_right::mapping<extents<size_t, dynamic_extent, 3>>>);
    static_assert(is_regular_trivial_nothrow_v<layout_right::mapping<extents<size_t, 2, dynamic_extent>>>);
    static_assert(is_regular_trivial_nothrow_v<layout_right::mapping<extents<size_t, dynamic_extent, dynamic_extent>>>);
#endif // __cpp_lib_concepts

    using E = extents<int, 2, 3>;
    static_assert(is_same_v<layout_right::mapping<E>::extents_type, E>);
    static_assert(is_same_v<layout_right::mapping<E>::index_type, E::index_type>);
    static_assert(is_same_v<layout_right::mapping<E>::size_type, E::size_type>);
    static_assert(is_same_v<layout_right::mapping<E>::rank_type, E::rank_type>);
    static_assert(is_same_v<layout_right::mapping<E>::layout_type, layout_right>);
}

void layout_stride_tests_traits() {
#ifdef __cpp_lib_concepts
    static_assert(is_regular_trivial_nothrow_v<layout_stride::mapping<extents<size_t, 2, 3>>>);
    static_assert(is_regular_trivial_nothrow_v<layout_stride::mapping<extents<size_t, dynamic_extent, 3>>>);
    static_assert(is_regular_trivial_nothrow_v<layout_stride::mapping<extents<size_t, 2, dynamic_extent>>>);
    static_assert(
        is_regular_trivial_nothrow_v<layout_stride::mapping<extents<size_t, dynamic_extent, dynamic_extent>>>);
#endif // __cpp_lib_concepts

    using E = extents<int, 2, 3>;
    static_assert(is_same_v<layout_stride::mapping<E>::extents_type, E>);
    static_assert(is_same_v<layout_stride::mapping<E>::index_type, E::index_type>);
    static_assert(is_same_v<layout_stride::mapping<E>::size_type, E::size_type>);
    static_assert(is_same_v<layout_stride::mapping<E>::rank_type, E::rank_type>);
    static_assert(is_same_v<layout_stride::mapping<E>::layout_type, layout_stride>);
}

void layout_left_tests_properties() {
    constexpr layout_left::mapping<extents<size_t, 2, 3>> map{};
    static_assert(map.is_unique() == true);
    static_assert(map.is_exhaustive() == true);
    static_assert(map.is_strided() == true);

    static_assert(decltype(map)::is_always_unique() == true);
    static_assert(decltype(map)::is_always_exhaustive() == true);
    static_assert(decltype(map)::is_always_strided() == true);
}

void layout_right_tests_properties() {
    constexpr layout_right::mapping<extents<size_t, 2, 3>> map{};
    static_assert(map.is_unique() == true);
    static_assert(map.is_exhaustive() == true);
    static_assert(map.is_strided() == true);

    static_assert(decltype(map)::is_always_unique() == true);
    static_assert(decltype(map)::is_always_exhaustive() == true);
    static_assert(decltype(map)::is_always_strided() == true);
}

void layout_stride_tests_properties() {
    constexpr layout_stride::mapping<extents<size_t, 2, 3>> map{};
    static_assert(map.is_unique() == true);
    static_assert(map.is_strided() == true);

    static_assert(decltype(map)::is_always_unique() == true);
    static_assert(decltype(map)::is_always_exhaustive() == false);
    static_assert(decltype(map)::is_always_strided() == true);
}

void layout_left_tests_extents_ctor() {
    constexpr extents<size_t, 2, 3> e1;
    constexpr extents<size_t, 5, dynamic_extent> e2{7};
    constexpr extents<size_t, dynamic_extent, 13> e3{11};
    constexpr extents<size_t, dynamic_extent, dynamic_extent> e4{17, 19};

    constexpr layout_left::mapping m1{e1};
    static_assert(m1.extents() == e1);

    constexpr layout_left::mapping m2{e2};
    static_assert(m2.extents() == e2);

    constexpr layout_left::mapping m3{e3};
    static_assert(m3.extents() == e3);

    constexpr layout_left::mapping m4{e4};
    static_assert(m4.extents() == e4);
}

void layout_right_tests_extents_ctor() {
    constexpr extents<size_t, 2, 3> e1;
    constexpr extents<size_t, 5, dynamic_extent> e2{7};
    constexpr extents<size_t, dynamic_extent, 13> e3{11};
    constexpr extents<size_t, dynamic_extent, dynamic_extent> e4{17, 19};

    constexpr layout_right::mapping m1{e1};
    static_assert(m1.extents() == e1);

    constexpr layout_right::mapping m2{e2};
    static_assert(m2.extents() == e2);

    constexpr layout_right::mapping m3{e3};
    static_assert(m3.extents() == e3);

    constexpr layout_right::mapping m4{e4};
    static_assert(m4.extents() == e4);
}

void layout_stride_tests_extents_ctor() {
    constexpr extents<size_t, 2, 3> e1;
    constexpr array<size_t, 2> s1{1, 2};

    constexpr layout_stride::mapping m1{e1, s1};
    static_assert(m1.extents() == e1);
    static_assert(m1.strides() == s1);

    constexpr extents<size_t, 5, dynamic_extent> e2{7};
    constexpr array<size_t, 2> s2{7, 1};

    constexpr layout_stride::mapping m2{e2, s2};
    static_assert(m2.extents() == e2);
    static_assert(m2.strides() == s2);
}

template <class Extents>
void copy_ctor_helper_left(const Extents& e) {
    const layout_left::mapping m1{e};
    const layout_left::mapping m2{m1};
    assert(m1 == m2);
}

template <class Extents>
void copy_ctor_helper_right(const Extents& e) {
    const layout_right::mapping m1{e};
    const layout_right::mapping m2{m1};
    assert(m1 == m2);
}

void layout_left_tests_copy_ctor() {
    copy_ctor_helper_left(extents<size_t, 2, 3>{});
    copy_ctor_helper_left(extents<size_t, 5, dynamic_extent>{7});
    copy_ctor_helper_left(extents<size_t, dynamic_extent, 13>{11});
    copy_ctor_helper_left(extents<size_t, dynamic_extent, dynamic_extent>{17, 19});
}

void layout_right_tests_copy_ctor() {
    copy_ctor_helper_right(extents<size_t, 2, 3>{});
    copy_ctor_helper_right(extents<size_t, 5, dynamic_extent>{7});
    copy_ctor_helper_right(extents<size_t, dynamic_extent, 13>{11});
    copy_ctor_helper_right(extents<size_t, dynamic_extent, dynamic_extent>{17, 19});
}

void layout_left_tests_copy_other_extent() {
    using E1 = extents<size_t, 2, 3>;
    using E2 = extents<size_t, 2, dynamic_extent>;
    constexpr E1 e1;
    constexpr E2 e2{3};
    constexpr layout_left::mapping<E1> m1(static_cast<E1>(e2));
    constexpr layout_left::mapping<E2> m2(e1);

    static_assert(m1.extents() == e1);
    static_assert(m2.extents() == e1);
    static_assert(m1.extents() == e2);
    static_assert(m2.extents() == e2);
}

void layout_right_tests_copy_ctor_other() {
    using E1 = extents<size_t, 2, 3>;
    using E2 = extents<size_t, 2, dynamic_extent>;
    constexpr E1 e1;
    constexpr E2 e2{3};
    constexpr layout_right::mapping<E1> m1(static_cast<E1>(e2));
    constexpr layout_right::mapping<E2> m2(e1);

    static_assert(m1.extents() == e1);
    static_assert(m2.extents() == e1);
    static_assert(m1.extents() == e2);
    static_assert(m2.extents() == e2);
}

template <class Extents>
void assign_helper_left(const Extents& e) {
    const layout_left::mapping m1{e};
    layout_left::mapping<Extents> m2;
    m2 = m1;
    assert(m1 == m2);
}

template <class Extents>
void assign_helper_right(const Extents& e) {
    const layout_right::mapping m1{e};
    layout_right::mapping<Extents> m2;
    m2 = m1;
    assert(m1 == m2);
}

void layout_left_tests_assign() {
    assign_helper_left(extents<size_t, 2, 3>{});
    assign_helper_left(extents<size_t, 5, dynamic_extent>{7});
    assign_helper_left(extents<size_t, dynamic_extent, 13>{11});
    assign_helper_left(extents<size_t, dynamic_extent, dynamic_extent>{17, 19});
}

void layout_right_tests_assign() {
    assign_helper_right(extents<size_t, 2, 3>{});
    assign_helper_right(extents<size_t, 5, dynamic_extent>{7});
    assign_helper_right(extents<size_t, dynamic_extent, 13>{11});
    assign_helper_right(extents<size_t, dynamic_extent, dynamic_extent>{17, 19});
}

void layout_left_tests_ctor_other_layout() {
    using E = extents<size_t, 1>;

    // from layout_left
    using OE1 = extents<size_t, 1>;
    static_assert(is_nothrow_constructible_v<layout_left::mapping<E>, layout_right::mapping<OE1>>);
    static_assert(is_nothrow_convertible_v<layout_right::mapping<OE1>, layout_left::mapping<E>>);

    using OE2 = extents<size_t, dynamic_extent>; // not convertible
    static_assert(is_nothrow_constructible_v<layout_left::mapping<E>, layout_right::mapping<OE2>>);
    static_assert(!is_convertible_v<layout_right::mapping<OE2>, layout_left::mapping<E>>);

    using OE3 = extents<size_t, 1, 2>; // not constructible, rank > 1
    static_assert(!is_constructible_v<layout_left::mapping<E>, layout_right::mapping<OE3>>);
    static_assert(!is_convertible_v<layout_right::mapping<OE3>, layout_left::mapping<E>>);

    static_assert(!is_constructible_v<layout_left::mapping<OE3>, layout_right::mapping<OE3>>);
    static_assert(!is_convertible_v<layout_right::mapping<OE3>, layout_left::mapping<OE3>>);

    // from layout_stride
    static_assert(is_constructible_v<layout_left::mapping<extents<size_t>>, layout_stride::mapping<extents<size_t>>>);
    static_assert(is_convertible_v<layout_stride::mapping<extents<size_t>>, layout_left::mapping<extents<size_t>>>);

    static_assert(is_constructible_v<layout_left::mapping<E>, layout_stride::mapping<OE1>>);
    static_assert(!is_convertible_v<layout_stride::mapping<OE1>, layout_left::mapping<E>>);

    static_assert(!is_constructible_v<layout_left::mapping<E>, layout_stride::mapping<OE3>>);
    static_assert(!is_convertible_v<layout_stride::mapping<OE3>, layout_left::mapping<E>>);
}

void layout_right_tests_ctor_other_layout() {
    using E = extents<size_t, 1>;

    // from layout_left
    using OE1 = extents<size_t, 1>;
    static_assert(is_nothrow_constructible_v<layout_right::mapping<E>, layout_left::mapping<OE1>>);
    static_assert(is_nothrow_convertible_v<layout_left::mapping<OE1>, layout_right::mapping<E>>);

    using OE2 = extents<size_t, dynamic_extent>; // not convertible
    static_assert(is_nothrow_constructible_v<layout_right::mapping<E>, layout_left::mapping<OE2>>);
    static_assert(!is_convertible_v<layout_left::mapping<OE2>, layout_right::mapping<E>>);

    using OE3 = extents<size_t, 1, 2>; // not constructible, rank > 1
    static_assert(!is_constructible_v<layout_right::mapping<E>, layout_left::mapping<OE3>>);
    static_assert(!is_convertible_v<layout_left::mapping<OE3>, layout_right::mapping<E>>);

    static_assert(!is_constructible_v<layout_right::mapping<OE3>, layout_left::mapping<OE3>>);
    static_assert(!is_convertible_v<layout_left::mapping<OE3>, layout_right::mapping<OE3>>);

    // from layout_stride
    static_assert(is_constructible_v<layout_right::mapping<extents<size_t>>, layout_stride::mapping<extents<size_t>>>);
    static_assert(is_convertible_v<layout_stride::mapping<extents<size_t>>, layout_right::mapping<extents<size_t>>>);

    static_assert(is_constructible_v<layout_right::mapping<E>, layout_stride::mapping<OE1>>);
    static_assert(!is_convertible_v<layout_stride::mapping<OE1>, layout_right::mapping<E>>);

    static_assert(!is_constructible_v<layout_right::mapping<E>, layout_stride::mapping<OE3>>);
    static_assert(!is_convertible_v<layout_stride::mapping<OE3>, layout_right::mapping<E>>);
}

void layout_left_tests_strides() {
    using E = extents<size_t, 2, 3, 5, 7>;
    layout_left::mapping<E> map;
    static_assert(map.stride(0) == 1);
    static_assert(map.stride(1) == 2);
    static_assert(map.stride(2) == 2 * 3);
    static_assert(map.stride(3) == 2 * 3 * 5);
}

void layout_right_tests_strides() {
    using E = extents<size_t, 2, 3, 5, 7>;
    layout_right::mapping<E> map;
    static_assert(map.stride(0) == 7 * 5 * 3);
    static_assert(map.stride(1) == 7 * 5);
    static_assert(map.stride(2) == 7);
    static_assert(map.stride(3) == 1);
}

void layout_stride_tests_strides() {
    using E = extents<size_t, 3, 5>;
    constexpr array<size_t, 2> s{1, 3};
    constexpr layout_stride::mapping<E> map{E{}, s};
    static_assert(map.stride(0) == s[0]);
    static_assert(map.stride(1) == s[1]);
    static_assert(map.strides() == s);
}

void layout_left_tests_indexing() {
    static_assert(layout_left::mapping<extents<size_t>>{}() == 0);
    TestMapping(layout_left::mapping<extents<size_t, 2, 3>>{});
    TestMapping(layout_left::mapping<extents<int, 2, 3>>{});
}

void layout_right_tests_indexing() {
    static_assert(layout_right::mapping<extents<size_t>>{}() == 0);
    TestMapping(layout_right::mapping<extents<size_t, 2, 3>>{});
    TestMapping(layout_right::mapping<extents<int, 2, 3>>{});
}

template <class Extents>
void copy_ctor_helper_stride(const Extents& e) {
    const layout_stride::mapping<Extents> m1{layout_right::mapping<Extents>{e}};
    const layout_stride::mapping<Extents> m2{m1};
    assert(m1 == m2);
}

void layout_stride_tests_copy_ctor() {
    copy_ctor_helper_stride(extents<size_t, 2, 3>{});
    copy_ctor_helper_stride(extents<size_t, 5, dynamic_extent>{7});
    copy_ctor_helper_stride(extents<size_t, dynamic_extent, 13>{11});
    copy_ctor_helper_stride(extents<size_t, dynamic_extent, dynamic_extent>{17, 19});
}

void layout_stride_tests_ctor_other_extents() {
    constexpr extents<size_t, 2, 3> e1;
    constexpr extents<size_t, 2, dynamic_extent> e2{3};
    constexpr array<size_t, 2> s{3, 1};

    constexpr layout_stride::mapping<decltype(e1)> m1(e1, s);
    constexpr layout_stride::mapping<decltype(e2)> m2(m1);

    static_assert(m2.extents() == e1);
    static_assert(m2.strides() == s);
}

template <class LayoutMapping>
void other_mapping_helper() {
    constexpr LayoutMapping other;
    constexpr layout_stride::mapping<typename LayoutMapping::extents_type> map{other};
    static_assert(map.extents() == other.extents());
    for (size_t i = 0; i < LayoutMapping::extents_type::rank(); ++i) {
        assert(map.stride(i) == other.stride(i));
    }
}
void layout_stride_tests_ctor_other_mapping() {
    using E = extents<size_t, 2, 3>;
    other_mapping_helper<layout_left::mapping<E>>();
    other_mapping_helper<layout_right::mapping<E>>();
}

template <class Extents>
void assign_helper_stride(const Extents& e) {
    const layout_stride::mapping<Extents> m1{layout_right::mapping<Extents>{e}};
    layout_stride::mapping<Extents> m2;
    m2 = m1;
    assert(m1 == m2);
}

void layout_stride_tests_assign() {
    assign_helper_stride(extents<size_t, 2, 3>{});
    assign_helper_stride(extents<size_t, 5, dynamic_extent>{7});
    assign_helper_stride(extents<size_t, dynamic_extent, 13>{11});
    assign_helper_stride(extents<size_t, dynamic_extent, dynamic_extent>{17, 19});
}

void layout_stride_tests_indexing_static() {
    using E = extents<size_t, 2, 3>;
    TestMapping(layout_stride::mapping<E>{E{}, array<size_t, 2>{1, 2}});
    TestMapping(layout_stride::mapping<E>{E{}, array<size_t, 2>{3, 1}});

    // non-exhaustive mappings
    TestMapping(layout_stride::mapping<E>{E{}, array<size_t, 2>{1, 3}});
    TestMapping(layout_stride::mapping<E>{E{}, array<size_t, 2>{4, 1}});
    TestMapping(layout_stride::mapping<E>{E{}, array<size_t, 2>{2, 3}});

    // exhaustive mappings with singleton dimensions
    using E1 = extents<int, 2, 1, 3>;
    TestMapping(layout_stride::mapping<E1>{E1{}, array<int, 3>{3, 1, 1}});
    TestMapping(layout_stride::mapping<E1>{E1{}, array<int, 3>{3, 7, 1}});

    using E2 = extents<int, 2, 3, 1>;
    TestMapping(layout_stride::mapping<E2>{E2{}, array<int, 3>{3, 1, 1}});
    TestMapping(layout_stride::mapping<E2>{E2{}, array<int, 3>{3, 1, 11}});

    using E3 = extents<int, 3, 2, 1>;
    TestMapping(layout_stride::mapping<E3>{E3{}, array<int, 3>{1, 3, 1}});
    TestMapping(layout_stride::mapping<E3>{E3{}, array<int, 3>{1, 3, 13}});
}


void layout_stride_tests_equality() {
    using E = extents<size_t, 2, 3>;
    constexpr layout_stride::mapping<E> map1(layout_right::mapping<E>{});
    constexpr layout_stride::mapping<E> map2(layout_right::mapping<E>{});
    static_assert(map1 == map2);

    constexpr layout_stride::mapping<E> map3{layout_left::mapping<E>{}};
    static_assert(map1 != map3);

    using ED = extents<size_t, dynamic_extent, dynamic_extent>;
    constexpr layout_stride::mapping<ED> map4{ED{2, 3}, map1.strides()};
    static_assert(map1 == map4);
}

void accessor_tests_general() {
    default_accessor<double> a;
    double arr[4] = {};
    static_assert(a.offset(arr, 3) == &arr[3]);

    a.access(arr, 2) = 42;
    assert(arr[2] == 42);

    static_assert(is_constructible_v<default_accessor<const double>, default_accessor<double>>);
    static_assert(!is_constructible_v<default_accessor<double>, default_accessor<const double>>);
}

namespace Pathological {

    struct Empty {};

    struct Extents {
        using index_type = int;
        using size_type  = std::make_unsigned_t<index_type>;
        using rank_type  = size_t;

        explicit Extents(Empty) {}

        template <size_t N>
        explicit Extents(const array<Empty, N>&) {}

        static constexpr size_t rank() {
            return 0;
        }

        static constexpr size_t rank_dynamic() {
            return 0;
        }
    };

    struct Layout {
        template <class E>
        struct mapping {
            using extents_type = E;
            using layout_type  = Layout;
            mapping(Extents) {}
        };
    };

    struct Accessor {
        using data_handle_type = int*;
        using reference        = int&;
        Accessor(int) {}
    };
} // namespace Pathological

void mdspan_tests_traits() {
    using M = mdspan<const int, extents<size_t, 2, 3>>;
    static_assert(is_trivially_copyable_v<M> /*&& is_default_constructible_v<M>*/
                  && is_copy_constructible_v<M> && is_move_constructible_v<M> && is_copy_assignable_v<M>
                  && is_move_assignable_v<M>);

    static_assert(is_same_v<M::extents_type, extents<size_t, 2, 3>>);
    static_assert(is_same_v<M::layout_type, layout_right>);
    static_assert(is_same_v<M::accessor_type, default_accessor<const int>>);
    static_assert(is_same_v<M::mapping_type, layout_right::mapping<extents<size_t, 2, 3>>>);
    static_assert(is_same_v<M::element_type, const int>);
    static_assert(is_same_v<M::value_type, int>);
    static_assert(is_same_v<M::size_type, size_t>);
    static_assert(is_same_v<M::data_handle_type, const int*>);
    static_assert(is_same_v<M::reference, const int&>);
}

void mdspan_tests_ctor_sizes() {
    static constexpr int arr[6] = {};
    constexpr mdspan<const int, extents<size_t, dynamic_extent, 3>> mds1(arr, 2);
    static_assert(mds1.data_handle() == arr);
    static_assert((mds1.extents() == extents<size_t, 2, 3>{}));
    static_assert(mds1.is_exhaustive());

    static_assert(!is_constructible_v<mdspan<int, Pathological::Extents, Pathological::Layout>, int*,
                  Pathological::Empty>); // Empty not convertible to size_type

    // TRANSITION: this assert should be true
    // static_assert(!is_constructible_v<mdspan<int, Pathological::Extents, Pathological::Layout>, int*,
    //              int>); // Pathological::Extents not constructible from int

    static_assert(!is_constructible_v<mdspan<int, extents<size_t, dynamic_extent>, Pathological::Layout>, int*,
                  int>); // Pathological::Layout not constructible from extents<size_t, dynamic_extent>

    static_assert(
        !is_constructible_v<mdspan<int, extents<size_t, dynamic_extent>, layout_right, Pathological::Accessor>, int*,
            int>); // Pathological::Accessor not default constructible
}

void mdspan_tests_ctor_array() {
    static constexpr int arr[6] = {};
    constexpr mdspan<const int, extents<size_t, dynamic_extent, 3>> mds1(arr, array{2});
    static_assert(mds1.data_handle() == arr);
    static_assert(mds1.extents() == extents<size_t, 2, 3>{});

    static_assert(!is_constructible_v<mdspan<int, Pathological::Extents, Pathological::Layout>, int*,
                  array<Pathological::Empty, 1>>); // Empty not convertible to size_type

    static_assert(!is_constructible_v<mdspan<int, Pathological::Extents, Pathological::Layout>, int*,
                  array<int, 1>>); // Pathological::Extents not constructible from int

    static_assert(!is_constructible_v<mdspan<int, extents<size_t, dynamic_extent>, Pathological::Layout>, int*,
                  array<int, 1>>); // Pathological::Layout not constructible from extents<size_t, dynamic_extent>

    static_assert(
        !is_constructible_v<mdspan<int, extents<size_t, dynamic_extent>, layout_right, Pathological::Accessor>, int*,
            array<int, 1>>); // Pathological::Accessor not default constructible
}

void mdspan_tests_ctor_extents() {
    static constexpr int arr[6] = {};
    constexpr mdspan<const int, extents<size_t, dynamic_extent, 3>> mds1(arr, extents<size_t, dynamic_extent, 3>{2});
    static_assert(mds1.data_handle() == arr);
    static_assert(mds1.extents() == extents<size_t, 2, 3>{});

    static_assert(!is_constructible_v<mdspan<int, extents<size_t, dynamic_extent>, Pathological::Layout>, int*,
                  extents<size_t, dynamic_extent>>); // Pathological::Layout not constructible from extents<size_t,
                                                     // dynamic_extent>

    static_assert(
        !is_constructible_v<mdspan<int, extents<size_t, dynamic_extent>, layout_right, Pathological::Accessor>, int*,
            extents<size_t, dynamic_extent>>); // Pathological::Accessor not default constructible
}

void mdspan_tests_ctor_mapping() {
    static constexpr int arr[6] = {};
    using E                     = extents<size_t, dynamic_extent, 3>;
    constexpr layout_left::mapping<extents<size_t, 2, 3>> map(extents<size_t, 2, 3>{});

    constexpr mdspan<const int, E, layout_left> mds1(arr, map);
    static_assert(mds1.data_handle() == arr);
    static_assert(mds1.extents() == extents<size_t, 2, 3>{});
    static_assert(mds1.mapping() == map);

    static_assert(
        !is_constructible_v<mdspan<int, extents<size_t, dynamic_extent>, layout_right, Pathological::Accessor>, int*,
            extents<size_t, dynamic_extent>>); // Pathological::Accessor not default constructible
}

template <class Type>
struct stateful_accessor {
    using data_handle_type = Type*;
    using reference        = Type&;

    constexpr stateful_accessor(int i_) : i(i_){};
    int i = 0;
};

void mdspan_tests_ctor_accessor() {
    static constexpr int arr[6] = {};
    using E                     = extents<size_t, dynamic_extent, 3>;
    constexpr layout_left::mapping<extents<size_t, 2, 3>> map(extents<size_t, 2, 3>{});
    constexpr stateful_accessor<const int> acc(1);

    constexpr mdspan<const int, E, layout_left, stateful_accessor<const int>> mds1(arr, map, acc);
    static_assert(mds1.data_handle() == arr);
    static_assert(mds1.extents() == extents<size_t, 2, 3>{});
    static_assert(mds1.mapping() == map);
    static_assert(mds1.accessor().i == 1);

    static_assert(
        !is_constructible_v<mdspan<int, extents<size_t, dynamic_extent>, layout_right, Pathological::Accessor>, int*,
            extents<size_t, dynamic_extent>>); // Pathological::Accessor not default constructible
}

void mdspan_tests_assign() {
    using E2   = extents<size_t, 2>;
    int arr[6] = {};
    mdspan<int, extents<size_t, dynamic_extent>> mds1(arr, 2);
    mdspan<int, extents<size_t, dynamic_extent>> mds2(nullptr, 3);
    mds2 = mds1;
    assert(mds2.data_handle() == arr);
    assert(mds2.extents() == E2{});
    assert(mds2.mapping() == mds1.mapping());
}

void mdspan_tests_observers() {
    using E                    = extents<size_t, dynamic_extent, 3>;
    static constexpr int arr[] = {0, 1, 2, 3, 4, 5, 6, 7};
    constexpr mdspan<const int, E, layout_stride> mds{arr, layout_stride::mapping<E>{E{2}, array<size_t, 2>{1, 3}}};

    static_assert(mds.rank() == 2);
    static_assert(mds.rank_dynamic() == 1);

    static_assert(mds.static_extent(0) == dynamic_extent);
    static_assert(mds.static_extent(1) == 3);
    static_assert(mds.extent(0) == 2);
    static_assert(mds.extent(1) == 3);
    static_assert(mds.size() == 6);

    static_assert(mds.stride(0) == 1);
    static_assert(mds.stride(1) == 3);

    static_assert(mds.is_always_unique());
    static_assert(!mds.is_always_exhaustive());
    static_assert(mds.is_always_strided());

    static_assert(mds.is_unique());
    static_assert(!mds.is_exhaustive());
    static_assert(mds.is_strided());

    static_assert(mds(1, 0) == 1);
    static_assert(mds(1, 2) == 7);

    static_assert(mds[array{0, 1}] == 3);
    static_assert(mds[array{1, 1}] == 4);
}

int main() {
    extent_tests_rank();
    extent_tests_static_extent();
    extent_tests_extent();
    extent_tests_ctor_other_sizes();
    extent_tests_copy_ctor_other();
    extent_tests_ctor_array();
    extent_tests_ctor_span();
    extent_tests_equality();

    layout_left_tests_traits();
    layout_left_tests_properties();
    layout_left_tests_extents_ctor();
    layout_left_tests_copy_ctor();
    layout_left_tests_copy_other_extent();
    layout_left_tests_assign();
    layout_left_tests_ctor_other_layout();
    layout_left_tests_strides();
    layout_left_tests_indexing();

    layout_right_tests_traits();
    layout_right_tests_properties();
    layout_right_tests_extents_ctor();
    layout_right_tests_copy_ctor();
    layout_right_tests_copy_ctor_other();
    layout_right_tests_assign();
    layout_right_tests_ctor_other_layout();
    layout_right_tests_strides();
    layout_right_tests_indexing();

    layout_stride_tests_traits();
    layout_stride_tests_properties();
    layout_stride_tests_extents_ctor();
    layout_stride_tests_strides();
    layout_stride_tests_copy_ctor();
    layout_stride_tests_ctor_other_extents();
    layout_stride_tests_ctor_other_mapping();
    layout_stride_tests_assign();
    layout_stride_tests_indexing_static();
    layout_stride_tests_equality();

    accessor_tests_general();

    mdspan_tests_traits();
    mdspan_tests_ctor_sizes();
    mdspan_tests_ctor_array();
    mdspan_tests_ctor_extents();
    mdspan_tests_ctor_mapping();
    mdspan_tests_ctor_accessor();
    mdspan_tests_assign();
    mdspan_tests_observers();

    return 0;
}