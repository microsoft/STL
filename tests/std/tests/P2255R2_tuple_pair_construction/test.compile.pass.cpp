// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <tuple>
#include <type_traits>
#include <utility>

using namespace std;

// TRANSITION, MSVC and EDG haven't implemented intrinsics needed for P2255R2.
#ifdef __cpp_lib_reference_from_temporary
template <bool Expected, class TupleOrPair, class... Args>
void assert_constref_constructible_single() {
    static_assert(is_constructible_v<TupleOrPair, Args&...> == Expected);
    static_assert(is_constructible_v<TupleOrPair, Args...> == Expected);
    static_assert(is_constructible_v<TupleOrPair, const Args&...> == Expected);
    static_assert(is_constructible_v<TupleOrPair, const Args...> == Expected);
    if constexpr (sizeof...(Args) == 2) {
        static_assert(is_constructible_v<TupleOrPair, pair<Args...>&> == Expected);
        static_assert(is_constructible_v<TupleOrPair, const pair<Args...>&> == Expected);
        static_assert(is_constructible_v<TupleOrPair, pair<Args...>> == Expected);
        static_assert(is_constructible_v<TupleOrPair, const pair<Args...>> == Expected);
    }
    static_assert(is_constructible_v<TupleOrPair, tuple<Args...>&> == Expected);
    static_assert(is_constructible_v<TupleOrPair, const tuple<Args...>&> == Expected);
    static_assert(is_constructible_v<TupleOrPair, tuple<Args...>> == Expected);
    static_assert(is_constructible_v<TupleOrPair, const tuple<Args...>> == Expected);
}

template <bool Expected, class TupleOrPair, class... Args>
void assert_mutref_constructible_single() {
    static_assert(is_constructible_v<TupleOrPair, Args...> == Expected);
    if constexpr (sizeof...(Args) == 2) {
        static_assert(is_constructible_v<TupleOrPair, pair<Args...>> == Expected);
    }
    static_assert(is_constructible_v<TupleOrPair, tuple<Args...>> == Expected);
}

void assert_normal_constructible() {
    assert_constref_constructible_single<true, tuple<const int&>, int>();
    assert_constref_constructible_single<false, tuple<const int&>, long>();

    assert_constref_constructible_single<true, tuple<const int&, const int&>, int, int>();
    assert_constref_constructible_single<false, tuple<const int&, const int&>, long, int>();
    assert_constref_constructible_single<false, tuple<const int&, const int&>, long, long>();

    assert_constref_constructible_single<true, pair<const int&, const int&>, int, int>();
    assert_constref_constructible_single<false, pair<const int&, const int&>, long, int>();
    assert_constref_constructible_single<false, pair<const int&, const int&>, int, long>();

    assert_mutref_constructible_single<true, tuple<int&&>, int>();
    assert_mutref_constructible_single<false, tuple<int&&>, long>();

    assert_mutref_constructible_single<true, tuple<int&&, int&&>, int, int>();
    assert_mutref_constructible_single<false, tuple<int&&, int&&>, long, int>();
    assert_mutref_constructible_single<false, tuple<int&&, int&&>, long, long>();

    assert_mutref_constructible_single<true, pair<int&&, int&&>, int, int>();
    assert_mutref_constructible_single<false, pair<int&&, int&&>, long, int>();
    assert_mutref_constructible_single<false, pair<int&&, int&&>, int, long>();
}

struct LvalueTempConverter {
    operator int() &;
    operator int&&() &&;
    operator const int&() &&;
};

template <class TupleOrPair, class... Args>
void assert_lvalue_temp_converter_single() {
    static_assert(!is_constructible_v<TupleOrPair, Args&..., LvalueTempConverter&>);
    static_assert(is_constructible_v<TupleOrPair, Args..., LvalueTempConverter>);
    if constexpr (sizeof...(Args) == 1) {
        static_assert(!is_constructible_v<TupleOrPair, pair<Args&..., LvalueTempConverter&>>);
        static_assert(is_constructible_v<TupleOrPair, pair<Args..., LvalueTempConverter>>);
    }
    static_assert(!is_constructible_v<TupleOrPair, tuple<Args&..., LvalueTempConverter&>>);
    static_assert(is_constructible_v<TupleOrPair, tuple<Args..., LvalueTempConverter>>);
}

void assert_lvalue_temp_converter() {
    assert_lvalue_temp_converter_single<tuple<const int&>>();
    assert_lvalue_temp_converter_single<tuple<int&&>>();

    assert_lvalue_temp_converter_single<tuple<const int&, const int&>, const int&>();
    assert_lvalue_temp_converter_single<tuple<int&&, int&&>, int&&>();

    assert_lvalue_temp_converter_single<pair<const int&, const int&>, const int&>();
    assert_lvalue_temp_converter_single<pair<int&&, int&&>, int&&>();
}
#endif // ^^^ no workaround ^^^
