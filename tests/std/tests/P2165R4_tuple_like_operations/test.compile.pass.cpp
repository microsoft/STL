// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <functional>
#include <memory>
#include <ranges>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;
using ranges::subrange;

namespace test_apply {
    struct TakeAnythingFn {
        void operator()(auto&&...) const noexcept; // not defined
    };

    struct StrSentinel {
        constexpr bool operator==(const char*) const; // not defined
    };

    template <class T>
    concept CanApply = requires(T tuple_like) { apply(TakeAnythingFn{}, tuple_like); };

    static_assert(CanApply<tuple<>>);
    static_assert(CanApply<tuple<char, wchar_t>>);
    static_assert(CanApply<tuple<char, signed char, unsigned char>>);
    static_assert(CanApply<pair<int, unsigned int>>);
    static_assert(CanApply<array<int, 0>>);
    static_assert(CanApply<array<short, 1>>);
    static_assert(CanApply<array<long, 2>>);
    static_assert(CanApply<array<long long, 3>>);
    static_assert(CanApply<subrange<wchar_t*, wchar_t*>>);
    static_assert(CanApply<subrange<char*, StrSentinel>>);
    static_assert(!CanApply<void>);
    static_assert(!CanApply<int>);
    static_assert(!CanApply<StrSentinel>);
    static_assert(!CanApply<ranges::copy_result<int*, int*>>);

    static constexpr int some_ints[] = {1, 2, 3};

    constexpr bool test() {
        { // Test apply with empty tuple-like types
            true_type func;
            assert(apply(func, tuple{}));
            assert(apply(func, array<int, 0>{}));
        }

        { // Test apply with one-sized tuple-like types
            identity func;
            assert(apply(func, tuple{7}) == 7);
            assert(apply(func, array{7}) == 7);
        }

        { // Test apply with pair-like types
            minus func;
            assert(apply(func, tuple{1, 1}) == 0);
            assert(apply(func, pair{3, 3}) == 0);
            assert(apply(func, array{5, 5}) == 0);

            constexpr subrange rng = some_ints;
            assert(apply(func, rng) == -3);
        }

        { // Test apply with big tuple-like types
            auto func = [](auto&&... vals) { return (vals + ...); };
            assert(apply(func, tuple{1, 2, 3}) == 6);
            assert(apply(func, array{4, 5, 6}) == 15);
        }

        return true;
    }
} // namespace test_apply

namespace test_make_from_tuple {
    template <class T, class Tuple>
    concept CanMakeFromTuple = requires(Tuple tuple_like) {
        { make_from_tuple<T>(tuple_like) } -> same_as<T>;
    };

    static_assert(CanMakeFromTuple<string, tuple<>>);
    static_assert(CanMakeFromTuple<string, tuple<size_t, char>>);
    static_assert(CanMakeFromTuple<wstring, tuple<size_t, wchar_t, allocator<wchar_t>>>);
    static_assert(CanMakeFromTuple<tuple<int, unsigned int>, pair<int, unsigned int>>);
    static_assert(CanMakeFromTuple<string, array<int, 0>>);
    static_assert(CanMakeFromTuple<vector<int>, array<long, 2>>);
    static_assert(CanMakeFromTuple<vector<wchar_t>, subrange<wchar_t*, wchar_t*>>);
    static_assert(CanMakeFromTuple<vector<int>, subrange<int*, int*>>);
    static_assert(!CanMakeFromTuple<int, void>);
    static_assert(!CanMakeFromTuple<long, int>);
    static_assert(!CanMakeFromTuple<void*, int*>);
    static_assert(!CanMakeFromTuple<ranges::copy_result<int*, int*>, int*>);

    constexpr bool test() {
        { // Test make_from_tuple with empty tuple-like types
            struct S {
                constexpr bool operator==(const S&) const = default;
            };

            assert(make_from_tuple<S>(tuple{}) == S{});
            assert(make_from_tuple<S>(array<int, 0>{}) == S{});
        }

        { // Test make_from_tuple with one-sized tuple-like types
            assert(make_from_tuple<int>(tuple{7}) == 7);
            assert(make_from_tuple<int>(array{7}) == 7);
        }

        { // Test make_from_tuple with pair-like types
            struct S {
                constexpr S(int* x_, int* y_) : x{x_}, y{y_} {}
                constexpr bool operator==(const S&) const = default;

                int* x;
                int* y;
            };

            int a = 0;
            int b = 1;
            S expected_val{&a, &b};

            assert(make_from_tuple<S>(tuple{&a, &b}) == expected_val);
            assert(make_from_tuple<S>(pair{&a, &b}) == expected_val);
            assert(make_from_tuple<S>(array{&a, &b}) == expected_val);
            assert(make_from_tuple<S>(subrange{&a, &b}) == expected_val);
        }

        { // Test make_from_tuple with big tuple-like types
            struct NoIncrement {};
            struct S {
                constexpr S(NoIncrement, int x_, int y_, int z_) : x{x_}, y{y_}, z{z_} {}
                constexpr S(int x_, int y_, int z_) : x{x_ + 1}, y{y_ + 1}, z{z_ + 1} {}
                constexpr bool operator==(const S&) const = default;

                int x;
                int y;
                int z;
            };

            S expected_val{NoIncrement{}, 2, 3, 4};
            assert(make_from_tuple<S>(tuple{1, 2, 3}) == expected_val);
            assert(make_from_tuple<S>(array{1, 2, 3}) == expected_val);
        }

        return true;
    }
} // namespace test_make_from_tuple

namespace test_tuple_cat {
    template <class... Tuples>
    concept CanTupleCat = requires(Tuples... tpls) { tuple_cat(tpls...); };

    template <class Expected, class... Tuples>
    concept CheckTupleCat = CanTupleCat<Tuples...> && same_as<decltype(tuple_cat(declval<Tuples>()...)), Expected>;

    static_assert(CheckTupleCat<tuple<>, tuple<>, array<int, 0>>);
    static_assert(CheckTupleCat<tuple<size_t, char, size_t, wchar_t>, tuple<size_t, char>, pair<size_t, wchar_t>>);
    static_assert(CheckTupleCat<tuple<int*, int*>, array<int, 0>, subrange<int*, int*>>);
    static_assert(CheckTupleCat<tuple<long, long, long, long>, array<long, 2>, pair<long, long>>);
    static_assert(CheckTupleCat<tuple<int, wchar_t*, wchar_t*>, array<int, 1>, subrange<wchar_t*, wchar_t*>>);
    static_assert(!CanTupleCat<void>);
    static_assert(!CanTupleCat<int>);
    static_assert(!CanTupleCat<int, tuple<int>>);
    static_assert(!CanTupleCat<int, array<int, 1>>);
    static_assert(!CanTupleCat<tuple<>, tuple<int>, int>);

    constexpr bool test() {
        // Test tuple_cat with empty tuple-like types
        assert(tuple_cat(tuple{}, array<int, 0>{}) == tuple{});

        // Test tuple_cat with one-sized tuple-like types
        assert((tuple_cat(tuple{7}, array{8}) == tuple{7, 8}));

        { // Test tuple_cat with pair-like types
            int a = 0;
            int b = 1;
            assert(
                (tuple_cat(tuple{1, 2}, array{3, 4}, pair{5, 6}, subrange{&a, &b}) == tuple{1, 2, 3, 4, 5, 6, &a, &b}));
        }

        // Test tuple_cat with big tuple-like types
        assert((tuple_cat(tuple{1, 2, 3}, array{4, 5, 6}) == tuple{1, 2, 3, 4, 5, 6}));

        return true;
    }
} // namespace test_tuple_cat

static_assert(test_apply::test());
static_assert(test_make_from_tuple::test());
static_assert(test_tuple_cat::test());
