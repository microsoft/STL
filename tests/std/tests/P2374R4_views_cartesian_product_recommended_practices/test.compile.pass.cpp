// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Check MSVC-STL internal machinery

#include <array>
#include <cstddef>
#include <limits>
#include <ranges>
#include <span>
#include <tuple>
#include <utility>

using namespace std;
using ranges::_Compile_time_max_size, ranges::cartesian_product_view, ranges::sized_range, ranges::range_difference_t,
    ranges::range_size_t, views::all_t;

template <class... Rngs>
using cpv_size_t = range_size_t<cartesian_product_view<Rngs...>>;

template <class... Rngs>
using cpv_difference_t = range_difference_t<cartesian_product_view<Rngs...>>;

template <class... Rngs>
using cpv_const_size_t = range_size_t<const cartesian_product_view<Rngs...>>;

template <class... Rngs>
using cpv_const_difference_t = range_difference_t<const cartesian_product_view<Rngs...>>;

#ifdef _WIN64
constexpr bool is_64_bit = true;
#else // ^^^ 64 bit / 32 bit vvv
constexpr bool is_64_bit = false;
#endif // ^^^ 32 bit ^^^

constexpr void check_array() {
    // Check '_Compile_time_max_size' type trait
    static_assert(_Compile_time_max_size<int[3]> == 3);
    static_assert(_Compile_time_max_size<int[9]> == 9);

    // Check '_Compile_time_max_size' type trait for const arrays
    static_assert(_Compile_time_max_size<const int[3]> == 3);
    static_assert(_Compile_time_max_size<const int[9]> == 9);

    // Computing cartesian product for small arrays does not require big range_size_t
    using A1 = all_t<int (&)[4]>;
    static_assert(sizeof(cpv_size_t<A1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_size_t<A1, A1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_size_t<A1, A1, A1>) <= sizeof(size_t));

    // Same thing with range_difference_t<array>
    static_assert(sizeof(cpv_difference_t<A1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<A1, A1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<A1, A1, A1>) <= sizeof(ptrdiff_t));

    // Computing cartesian product for big arrays requires bigger types
    using A2 = all_t<int (&)[500'000'000]>;
    static_assert(sizeof(cpv_size_t<A2, A2, A2>) > sizeof(size_t));
    static_assert(sizeof(cpv_difference_t<A2, A2, A2>) > sizeof(ptrdiff_t));
}

constexpr void check_std_array() {
    // Check '_Compile_time_max_size' type trait
    static_assert(_Compile_time_max_size<array<int, 0>> == 0);
    static_assert(_Compile_time_max_size<array<int, 3>> == 3);
    static_assert(_Compile_time_max_size<array<int, 9>> == 9);

    // Check '_Compile_time_max_size' type trait for const arrays
    static_assert(_Compile_time_max_size<const array<int, 0>> == 0);
    static_assert(_Compile_time_max_size<const array<int, 3>> == 3);
    static_assert(_Compile_time_max_size<const array<int, 9>> == 9);

    // Computing cartesian product for small arrays does not require big range_size_t
    using A1 = all_t<array<int, 4>>;
    static_assert(sizeof(cpv_size_t<A1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_size_t<A1, A1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_size_t<A1, A1, A1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_const_size_t<A1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_const_size_t<A1, A1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_const_size_t<A1, A1, A1>) <= sizeof(size_t));

    // Same thing with range_difference_t<array>
    static_assert(sizeof(cpv_difference_t<A1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<A1, A1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<A1, A1, A1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<A1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<A1, A1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<A1, A1, A1>) <= sizeof(ptrdiff_t));

    // Computing cartesian product for big arrays requires bigger types
    using A2 = all_t<array<int, 500'000'000>&>;
    static_assert(sizeof(cpv_size_t<A2, A2, A2>) > sizeof(size_t));
    static_assert(sizeof(cpv_difference_t<A2, A2, A2>) > sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_size_t<A2, A2, A2>) > sizeof(size_t));
    static_assert(sizeof(cpv_const_difference_t<A2, A2, A2>) > sizeof(ptrdiff_t));
}

constexpr void check_span() {
    // Check '_Compile_time_max_size' type trait
    static_assert(_Compile_time_max_size<span<int, 0>> == 0);
    static_assert(_Compile_time_max_size<span<int, 3>> == 3);
    static_assert(_Compile_time_max_size<span<int, 9>> == 9);
    static_assert(_Compile_time_max_size<span<int>> == (numeric_limits<span<int>::size_type>::max)());

    // Check '_Compile_time_max_size' type trait for const spans
    static_assert(_Compile_time_max_size<const span<int, 0>> == 0);
    static_assert(_Compile_time_max_size<const span<int, 3>> == 3);
    static_assert(_Compile_time_max_size<const span<int, 9>> == 9);
    static_assert(_Compile_time_max_size<const span<int>> == (numeric_limits<span<int>::size_type>::max)());

    // Computing cartesian product for small spans does not require big range_size_t
    using S1 = all_t<span<int, 4>>;
    static_assert(sizeof(cpv_size_t<S1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_size_t<S1, S1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_size_t<S1, S1, S1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_const_size_t<S1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_const_size_t<S1, S1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_const_size_t<S1, S1, S1>) <= sizeof(size_t));

    // Same thing with range_difference_t<span>
    static_assert(sizeof(cpv_difference_t<S1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<S1, S1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<S1, S1, S1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<S1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<S1, S1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<S1, S1, S1>) <= sizeof(ptrdiff_t));

    // Computing cartesian product for big spans requires bigger types
    using S2 = span<int, 500'000'000>;
    static_assert(sizeof(cpv_size_t<S2, S2, S2>) > sizeof(size_t));
    static_assert(sizeof(cpv_difference_t<S2, S2, S2>) > sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_size_t<S2, S2, S2>) > sizeof(size_t));
    static_assert(sizeof(cpv_const_difference_t<S2, S2, S2>) > sizeof(ptrdiff_t));
}

constexpr void check_empty_view() {
    using ranges::empty_view;

    // Check '_Compile_time_max_size' type trait
    static_assert(_Compile_time_max_size<empty_view<int>> == 0);
    static_assert(_Compile_time_max_size<empty_view<const int>> == 0);
    static_assert(_Compile_time_max_size<const empty_view<int>> == 0);
    static_assert(_Compile_time_max_size<const empty_view<const int>> == 0);

    using E = empty_view<int>;
    static_assert(sizeof(cpv_size_t<E>) <= sizeof(size_t));
    static_assert(sizeof(cpv_difference_t<E>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_size_t<E, E>) <= sizeof(size_t));
    static_assert(sizeof(cpv_difference_t<E, E>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_size_t<E>) <= sizeof(size_t));
    static_assert(sizeof(cpv_const_difference_t<E>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_size_t<E, E>) <= sizeof(size_t));
    static_assert(sizeof(cpv_const_difference_t<E, E>) <= sizeof(ptrdiff_t));
}

constexpr void check_single_view() {
    using ranges::single_view;

    // Check '_Compile_time_max_size' type trait
    static_assert(_Compile_time_max_size<single_view<int>> == 1);
    static_assert(_Compile_time_max_size<single_view<const int>> == 1);
    static_assert(_Compile_time_max_size<const single_view<int>> == 1);
    static_assert(_Compile_time_max_size<const single_view<const int>> == 1);

    using S = single_view<int>;
    static_assert(sizeof(cpv_size_t<S>) <= sizeof(size_t));
    static_assert(sizeof(cpv_difference_t<S>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_size_t<S, S>) <= sizeof(size_t));
    static_assert(sizeof(cpv_difference_t<S, S>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_size_t<S>) <= sizeof(size_t));
    static_assert(sizeof(cpv_const_difference_t<S>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_size_t<S, S>) <= sizeof(size_t));
    static_assert(sizeof(cpv_const_difference_t<S, S>) <= sizeof(ptrdiff_t));
}

enum class CheckConstAdaptor : bool { no, yes };

template <template <class, class...> class SimpleRangeAdaptor, CheckConstAdaptor CCA, class... Args>
constexpr void check_simple_range_adaptor() {
    using ValTy = tuple<int>; // for ranges::elements_view
    using V1    = SimpleRangeAdaptor<all_t<array<ValTy, 100>>, Args...>; // owning_view
    using V2    = SimpleRangeAdaptor<span<ValTy, 500'000'000>, Args...>; // "already view"
    using V3    = SimpleRangeAdaptor<all_t<array<ValTy, 500'000'000>&>, Args...>; // ref_view

    // Check '_Compile_time_max_size' type trait
    static_assert(_Compile_time_max_size<V1> == 100);
    static_assert(_Compile_time_max_size<V2> == 500'000'000);
    static_assert(_Compile_time_max_size<V3> == 500'000'000);

    static_assert(
        sized_range<V1> == (sized_range<V2> && sized_range<V3>) ); // If V1 is sized, then all of them should be
    if constexpr (sized_range<V1>) {
        static_assert(sizeof(cpv_size_t<V1>) <= sizeof(size_t));
        static_assert(sizeof(cpv_size_t<V1, V1>) <= sizeof(size_t));
        static_assert(sizeof(cpv_size_t<V2>) <= sizeof(size_t));
        static_assert((sizeof(cpv_size_t<V2, V2>) <= sizeof(size_t)) == is_64_bit);
        static_assert(sizeof(cpv_size_t<V3>) <= sizeof(size_t));
        static_assert(sizeof(cpv_size_t<V3, V3, V3>) > sizeof(size_t));
    }

    static_assert(sizeof(cpv_difference_t<V1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V1, V1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V2>) <= sizeof(ptrdiff_t));
    static_assert((sizeof(cpv_difference_t<V2, V2>) <= sizeof(ptrdiff_t)) == is_64_bit);
    static_assert(sizeof(cpv_difference_t<V3>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V3, V3, V3>) > sizeof(ptrdiff_t));

    static_assert(
        to_underlying(CCA) == (ranges::range<const V1> && ranges::range<const V2> && ranges::range<const V3>) );
    if constexpr (to_underlying(CCA)) {
        static_assert(_Compile_time_max_size<const V1> == 100);
        static_assert(_Compile_time_max_size<const V2> == 500'000'000);
        static_assert(_Compile_time_max_size<const V3> == 500'000'000);

        if constexpr (sized_range<const V1>) {
            static_assert(sizeof(cpv_const_size_t<V1>) <= sizeof(size_t));
            static_assert(sizeof(cpv_const_size_t<V1, V1>) <= sizeof(size_t));
        }

        if constexpr (sized_range<const V2>) {
            static_assert(sizeof(cpv_const_size_t<V2>) <= sizeof(size_t));
            static_assert(sizeof(cpv_const_size_t<V2, V2, V2>) > sizeof(size_t));
        }

        if constexpr (sized_range<const V3>) {
            static_assert(sizeof(cpv_const_size_t<V3>) <= sizeof(size_t));
            static_assert(sizeof(cpv_const_size_t<V3, V3, V3>) > sizeof(size_t));
        }

        static_assert(sizeof(cpv_const_difference_t<V1>) <= sizeof(ptrdiff_t));
        static_assert(sizeof(cpv_const_difference_t<V1, V1>) <= sizeof(ptrdiff_t));
        static_assert(sizeof(cpv_const_difference_t<V2>) <= sizeof(ptrdiff_t));
        static_assert(sizeof(cpv_const_difference_t<V2, V2, V2>) > sizeof(ptrdiff_t));
        static_assert(sizeof(cpv_const_difference_t<V3>) <= sizeof(ptrdiff_t));
        static_assert(sizeof(cpv_const_difference_t<V3, V3, V3>) > sizeof(ptrdiff_t));
    }
}

constexpr void check_join_view() {
    using V1 = ranges::join_view<all_t<array<array<int, 10>, 20>>>;
    using V2 = ranges::join_view<all_t<array<span<int, 5'000'000>, 100>>>;

    // Check '_Compile_time_max_size' type trait
    static_assert(_Compile_time_max_size<V1> == 200);
    static_assert(_Compile_time_max_size<V2> == 500'000'000);

    static_assert(sizeof(cpv_difference_t<V1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V1, V1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V2>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V2, V2, V2>) > sizeof(ptrdiff_t));

    static_assert(_Compile_time_max_size<const V1> == 200);
    static_assert(_Compile_time_max_size<const V2> == 500'000'000);

    static_assert(sizeof(cpv_const_difference_t<V1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<V1, V1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<V2>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<V2, V2, V2>) > sizeof(ptrdiff_t));

#ifdef _WIN64
    using V3 = ranges::join_view<all_t<span<span<int, 10'000'000'000>, 5'000'000'000>>>;
    static_assert(_Compile_time_max_size<V3> == (numeric_limits<size_t>::max)());
    static_assert(sizeof(cpv_difference_t<V3>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V3, V3, V3>) > sizeof(ptrdiff_t));
    static_assert(_Compile_time_max_size<const V3> == (numeric_limits<size_t>::max)());
    static_assert(sizeof(cpv_const_difference_t<V3>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<V3, V3, V3>) > sizeof(ptrdiff_t));
#endif // ^^^ 64 bit ^^^
}

constexpr void check_join_with_view() {
    using V1 = ranges::join_with_view<all_t<array<array<int, 3>, 4>>, all_t<array<int, 2>>>;
    using V2 = ranges::join_with_view<all_t<array<span<int, 100'000>, 100>&>, all_t<array<int, 2>>>;

    // Check '_Compile_time_max_size' type trait
    static_assert(_Compile_time_max_size<V1> == 18);
    static_assert(_Compile_time_max_size<V2> == 10'000'198);

    static_assert(sizeof(cpv_difference_t<V1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V1, V1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V2>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V2, V2, V2>) > sizeof(ptrdiff_t));

    static_assert(_Compile_time_max_size<const V1> == 18);
    static_assert(_Compile_time_max_size<const V2> == 10'000'198);

    static_assert(sizeof(cpv_const_difference_t<V1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<V1, V1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<V2>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<V2, V2, V2>) > sizeof(ptrdiff_t));

#ifdef _WIN64
    using V3 = ranges::join_with_view<span<span<int, 10'000'000'000>, 5'000'000'000>, span<int, 1>>;
    static_assert(_Compile_time_max_size<V3> == (numeric_limits<size_t>::max)());
    static_assert(sizeof(cpv_difference_t<V3>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V3, V3, V3>) > sizeof(ptrdiff_t));
    static_assert(_Compile_time_max_size<const V3> == (numeric_limits<size_t>::max)());
    static_assert(sizeof(cpv_const_difference_t<V3>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<V3, V3, V3>) > sizeof(ptrdiff_t));
#endif // ^^^ 64 bit ^^^

    // Check '_Compile_time_max_size' when size of joined range is 0
    using V4 = ranges::join_with_view<span<span<int, 5>, 0>, span<int, 2>>;
    static_assert(_Compile_time_max_size<V4> == 0);
    static_assert(sizeof(cpv_difference_t<V4>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V4, V4, V4>) <= sizeof(ptrdiff_t));
    static_assert(_Compile_time_max_size<const V4> == 0);
    static_assert(sizeof(cpv_const_difference_t<V4>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<V4, V4, V4>) <= sizeof(ptrdiff_t));
}

template <template <class...> class ZipAdaptor>
constexpr void check_zip_adaptor() {
    using ValTy = int;
    using V1    = all_t<array<ValTy, 100>>; // owning_view
    using V2    = span<ValTy, 100'000'000>; // "already view"
    using V3    = all_t<array<ValTy, 500'000'000>&>; // ref_view
    using Z1    = ZipAdaptor<V1, V2, V3>;
    using Z2    = ZipAdaptor<V2, V3>;

    // Check '_Compile_time_max_size' type trait
    static_assert(_Compile_time_max_size<Z1> == 100);
    static_assert(_Compile_time_max_size<Z2> == 100'000'000);

    static_assert(sizeof(cpv_size_t<Z1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_size_t<Z1, Z1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_size_t<Z2>) <= sizeof(size_t));
    static_assert((sizeof(cpv_size_t<Z2, Z2>) <= sizeof(size_t)) == is_64_bit);
    static_assert(sizeof(cpv_size_t<Z2, Z2, Z2>) > sizeof(size_t));

    static_assert(sizeof(cpv_difference_t<Z1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<Z1, Z1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<Z2>) <= sizeof(ptrdiff_t));
    static_assert((sizeof(cpv_difference_t<Z2, Z2>) <= sizeof(ptrdiff_t)) == is_64_bit);
    static_assert(sizeof(cpv_difference_t<Z2, Z2, Z2>) > sizeof(ptrdiff_t));

    static_assert(_Compile_time_max_size<const Z1> == 100);
    static_assert(_Compile_time_max_size<const Z2> == 100'000'000);

    static_assert(sizeof(cpv_const_size_t<Z1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_const_size_t<Z1, Z1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_const_size_t<Z2>) <= sizeof(size_t));
    static_assert(sizeof(cpv_const_size_t<Z2, Z2, Z2>) > sizeof(size_t));

    static_assert(sizeof(cpv_const_difference_t<Z1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<Z1, Z1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<Z2>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<Z2, Z2, Z2>) > sizeof(ptrdiff_t));
}

template <template <class, size_t> class AdjacentAdaptor>
constexpr void check_adjacent_adaptor() {
    using ValTy = int;
    using V1    = AdjacentAdaptor<all_t<array<ValTy, 100>>, 1>; // owning_view
    using V2    = AdjacentAdaptor<span<ValTy, 500'000'000>, 6>; // "already view"
    using V3    = AdjacentAdaptor<all_t<array<ValTy, 500'000'000>&>, 11>; // ref_view

    // Check '_Compile_time_max_size' type trait
    static_assert(_Compile_time_max_size<V1> == 100);
    static_assert(_Compile_time_max_size<V2> == 499'999'995);
    static_assert(_Compile_time_max_size<V3> == 499'999'990);

    static_assert(sizeof(cpv_size_t<V1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_size_t<V1, V1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_size_t<V2>) <= sizeof(size_t));
    static_assert((sizeof(cpv_size_t<V2, V2>) <= sizeof(size_t)) == is_64_bit);
    static_assert(sizeof(cpv_size_t<V3>) <= sizeof(size_t));
    static_assert(sizeof(cpv_size_t<V3, V3, V3>) > sizeof(size_t));

    static_assert(sizeof(cpv_difference_t<V1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V1, V1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V2>) <= sizeof(ptrdiff_t));
    static_assert((sizeof(cpv_difference_t<V2, V2>) <= sizeof(ptrdiff_t)) == is_64_bit);
    static_assert(sizeof(cpv_difference_t<V3>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V3, V3, V3>) > sizeof(ptrdiff_t));

    static_assert(_Compile_time_max_size<const V1> == 100);
    static_assert(_Compile_time_max_size<const V2> == 499'999'995);
    static_assert(_Compile_time_max_size<const V3> == 499'999'990);

    static_assert(sizeof(cpv_const_size_t<V1>) <= sizeof(size_t));
    static_assert(sizeof(cpv_const_size_t<V1, V1>) <= sizeof(size_t));

    static_assert(sizeof(cpv_const_size_t<V2>) <= sizeof(size_t));
    static_assert(sizeof(cpv_const_size_t<V2, V2, V2>) > sizeof(size_t));

    static_assert(sizeof(cpv_const_size_t<V3>) <= sizeof(size_t));
    static_assert(sizeof(cpv_const_size_t<V3, V3, V3>) > sizeof(size_t));

    static_assert(sizeof(cpv_const_difference_t<V1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<V1, V1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<V2>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<V2, V2, V2>) > sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<V3>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<V3, V3, V3>) > sizeof(ptrdiff_t));
}

constexpr void check_cartesian_product_view() {
    using V1 = cartesian_product_view<all_t<array<int, 2>>, all_t<array<float, 3>>, all_t<array<char, 5>>>;
    using V2 = cartesian_product_view<all_t<array<int, 200'000>&>, all_t<array<float, 300'000>&>, span<char, 500'000>>;

    // Check '_Compile_time_max_size' type trait
    static_assert(_Compile_time_max_size<V1> == 30);
    static_assert(_Compile_time_max_size<V2> == 30'000'000'000'000'000);

    static_assert(sizeof(cpv_difference_t<V1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V1, V1>) <= sizeof(ptrdiff_t));
    static_assert((sizeof(cpv_difference_t<V2>) <= sizeof(ptrdiff_t)) == is_64_bit);
    static_assert(sizeof(cpv_difference_t<V2, V2, V2>) > sizeof(ptrdiff_t));

    static_assert(_Compile_time_max_size<const V1> == 30);
    static_assert(_Compile_time_max_size<const V2> == 30'000'000'000'000'000);

    static_assert(sizeof(cpv_const_difference_t<V1>) <= sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<V1, V1>) <= sizeof(ptrdiff_t));
    static_assert((sizeof(cpv_const_difference_t<V2>) <= sizeof(ptrdiff_t)) == is_64_bit);
    static_assert(sizeof(cpv_const_difference_t<V2, V2, V2>) > sizeof(ptrdiff_t));

#ifdef _WIN64
    using V3 = cartesian_product_view<span<int, 10'000'000'000'000>, span<float, 500'000'000'000>,
        span<char, 900'000'000'000'000>>;
    static_assert(_Compile_time_max_size<V3> == (numeric_limits<_Unsigned128>::max)());
    static_assert(sizeof(cpv_difference_t<V3>) > sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_difference_t<V3, V3, V3>) > sizeof(ptrdiff_t));
    static_assert(_Compile_time_max_size<const V3> == (numeric_limits<_Unsigned128>::max)());
    static_assert(sizeof(cpv_const_difference_t<V3>) > sizeof(ptrdiff_t));
    static_assert(sizeof(cpv_const_difference_t<V3, V3, V3>) > sizeof(ptrdiff_t));
#endif // ^^^ 64 bit ^^^
}

struct Pred {
    bool operator()(auto&&) const; // not defined
    bool operator()(auto&&, auto&&) const; // not defined
};

struct Transformer {
    float operator()(auto&&...) const; // not defined
};

template <class View>
using CommonView = ranges::common_view<ranges::take_while_view<View, Pred>>;

template <class View>
using ElementsView = ranges::elements_view<View, 0>;

template <class... Views>
using ZipTransformView = ranges::zip_transform_view<Transformer, Views...>;

template <class View, size_t N>
using AdjacentTransformView = ranges::adjacent_transform_view<View, Transformer, N>;

constexpr bool test() {
    // Containers
    check_array();
    check_std_array();
    check_span();

    // Range factories
    check_empty_view();
    check_single_view();

    // Simple range adaptors (take one range and possibly extra arguments)
    check_simple_range_adaptor<ranges::as_const_view, CheckConstAdaptor::yes>();
    check_simple_range_adaptor<ranges::as_rvalue_view, CheckConstAdaptor::yes>();
    check_simple_range_adaptor<ranges::chunk_view, CheckConstAdaptor::yes>();
    check_simple_range_adaptor<ranges::chunk_by_view, CheckConstAdaptor::no, Pred>();
    check_simple_range_adaptor<CommonView, CheckConstAdaptor::yes>();
    check_simple_range_adaptor<ranges::drop_view, CheckConstAdaptor::yes>();
    check_simple_range_adaptor<ranges::drop_while_view, CheckConstAdaptor::no, Pred>();
    check_simple_range_adaptor<ElementsView, CheckConstAdaptor::yes>();
    check_simple_range_adaptor<ranges::enumerate_view, CheckConstAdaptor::yes>();
    check_simple_range_adaptor<ranges::filter_view, CheckConstAdaptor::no, Pred>();
    check_simple_range_adaptor<ranges::lazy_split_view, CheckConstAdaptor::yes, ranges::single_view<tuple<int>>>();
    check_simple_range_adaptor<ranges::split_view, CheckConstAdaptor::no, ranges::single_view<tuple<int>>>();
    check_simple_range_adaptor<ranges::owning_view, CheckConstAdaptor::yes>();
    check_simple_range_adaptor<ranges::ref_view, CheckConstAdaptor::yes>();
    check_simple_range_adaptor<ranges::reverse_view, CheckConstAdaptor::yes>();
    check_simple_range_adaptor<ranges::slide_view, CheckConstAdaptor::yes>();
    check_simple_range_adaptor<ranges::stride_view, CheckConstAdaptor::yes>();
    check_simple_range_adaptor<ranges::take_view, CheckConstAdaptor::yes>();
    check_simple_range_adaptor<ranges::take_while_view, CheckConstAdaptor::yes, Pred>();
    check_simple_range_adaptor<ranges::transform_view, CheckConstAdaptor::yes, Transformer>();

    check_join_view();
    check_join_with_view();
    check_zip_adaptor<ranges::zip_view>();
    check_zip_adaptor<ZipTransformView>();
    check_adjacent_adaptor<ranges::adjacent_view>();
    check_adjacent_adaptor<AdjacentTransformView>();
    check_cartesian_product_view();

    return true;
}

static_assert(test());
