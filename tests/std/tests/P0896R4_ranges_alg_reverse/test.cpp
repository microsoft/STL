// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <compare>
#include <concepts>
#include <numeric>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::reverse(borrowed<false>{})), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::reverse(borrowed<true>{})), int*>);

struct nontrivial_int {
    int val;

    constexpr nontrivial_int(int i) noexcept : val{i} {}
    constexpr nontrivial_int(const nontrivial_int& that) noexcept : val{that.val} {}
    constexpr nontrivial_int& operator=(const nontrivial_int& that) noexcept {
        val = that.val;
        return *this;
    }

    auto operator<=>(const nontrivial_int&) const = default;
};

struct instantiator {
    static constexpr nontrivial_int expected_odd[]  = {1367, 42, 13};
    static constexpr nontrivial_int expected_even[] = {1729, 1367, 42, 13};

    template <ranges::bidirectional_range R>
    static constexpr void call() {
        using ranges::reverse, ranges::equal, ranges::iterator_t;

        { // Validate iterator + sentinel overload, odd length
            nontrivial_int input[] = {13, 42, 1367};
            R wrapped_input{input};
            auto result = reverse(wrapped_input.begin(), wrapped_input.end());
            STATIC_ASSERT(same_as<decltype(result), iterator_t<R>>);
            assert(result == wrapped_input.end());
            assert(equal(input, expected_odd));
        }
        { // Validate range overload, odd length
            nontrivial_int input[] = {13, 42, 1367};
            R wrapped_input{input};
            auto result = reverse(wrapped_input);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<R>>);
            assert(result == wrapped_input.end());
            assert(equal(input, expected_odd));
        }
        { // Validate iterator + sentinel overload, even length
            nontrivial_int input[] = {13, 42, 1367, 1729};
            R wrapped_input{input};
            auto result = reverse(wrapped_input.begin(), wrapped_input.end());
            STATIC_ASSERT(same_as<decltype(result), iterator_t<R>>);
            assert(result == wrapped_input.end());
            assert(equal(input, expected_even));
        }
        { // Validate range overload, even length
            nontrivial_int input[] = {13, 42, 1367, 1729};
            R wrapped_input{input};
            auto result = reverse(wrapped_input);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<R>>);
            assert(result == wrapped_input.end());
            assert(equal(input, expected_even));
        }
        { // Validate iterator + sentinel overload, empty range
            R wrapped_input{};
            auto result = reverse(wrapped_input.begin(), wrapped_input.end());
            STATIC_ASSERT(same_as<decltype(result), iterator_t<R>>);
            assert(result == wrapped_input.end());
        }
        { // Validate range overload, empty range
            R wrapped_input{};
            auto result = reverse(wrapped_input);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<R>>);
            assert(result == wrapped_input.end());
        }
    }
};

template <size_t N>
struct bytes {
    unsigned char storage[N];

    constexpr bytes(unsigned char base) {
        iota(storage, storage + N, base);
    }

    bool operator==(const bytes&) const = default;
};

struct test_vector {
    template <ranges::bidirectional_range R>
    static constexpr void call() {
        using ranges::reverse, ranges::equal, ranges::iterator_t;

        { // Validate iterator + sentinel overload, vectorizable odd length
            ranges::range_value_t<R> input[]{0x10, 0x20, 0x30};
            R wrapped_input{input};
            auto result = reverse(wrapped_input.begin(), wrapped_input.end());
            STATIC_ASSERT(same_as<decltype(result), iterator_t<R>>);
            assert(result == wrapped_input.end());
            assert(equal(input, initializer_list<ranges::range_value_t<R>>{0x30, 0x20, 0x10}));
        }
        { // Validate range overload, vectorizable odd length
            ranges::range_value_t<R> input[]{0x10, 0x20, 0x30};
            R wrapped_input{input};
            auto result = reverse(wrapped_input);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<R>>);
            assert(result == wrapped_input.end());
            assert(equal(input, initializer_list<ranges::range_value_t<R>>{0x30, 0x20, 0x10}));
        }

        { // Validate iterator + sentinel overload, vectorizable even length
            ranges::range_value_t<R> input[]{0x10, 0x20, 0x30, 0x40};
            R wrapped_input{input};
            auto result = reverse(wrapped_input.begin(), wrapped_input.end());
            STATIC_ASSERT(same_as<decltype(result), iterator_t<R>>);
            assert(result == wrapped_input.end());
            assert(equal(input, initializer_list<ranges::range_value_t<R>>{0x40, 0x30, 0x20, 0x10}));
        }
        { // Validate range overload, vectorizable even length
            ranges::range_value_t<R> input[]{0x10, 0x20, 0x30, 0x40};
            R wrapped_input{input};
            auto result = reverse(wrapped_input);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<R>>);
            assert(result == wrapped_input.end());
            assert(equal(input, initializer_list<ranges::range_value_t<R>>{0x40, 0x30, 0x20, 0x10}));
        }

        { // Validate iterator + sentinel overload, vectorizable empty
            R wrapped_input{};
            auto result = reverse(wrapped_input.begin(), wrapped_input.end());
            STATIC_ASSERT(same_as<decltype(result), iterator_t<R>>);
            assert(result == wrapped_input.end());
        }
        { // Validate range overload, vectorizable empty
            R wrapped_input{};
            auto result = reverse(wrapped_input);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<R>>);
            assert(result == wrapped_input.end());
        }
    }
};

int main() {
    STATIC_ASSERT((test_bidi<instantiator, nontrivial_int>(), true));
    test_bidi<instantiator, nontrivial_int>();

    STATIC_ASSERT((test_contiguous<test_vector, bytes<1>>(), true));
    test_contiguous<test_vector, bytes<1>>();

    STATIC_ASSERT((test_contiguous<test_vector, bytes<2>>(), true));
    test_contiguous<test_vector, bytes<2>>();

    STATIC_ASSERT((test_contiguous<test_vector, bytes<4>>(), true));
    test_contiguous<test_vector, bytes<4>>();

    STATIC_ASSERT((test_contiguous<test_vector, bytes<8>>(), true));
    test_contiguous<test_vector, bytes<8>>();

    STATIC_ASSERT((test_contiguous<test_vector, bytes<3>>(), true));
    test_contiguous<test_vector, bytes<3>>();
}
