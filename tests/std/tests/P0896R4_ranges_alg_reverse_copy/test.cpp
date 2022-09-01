// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <compare>
#include <concepts>
#include <numeric>
#include <ranges>
#include <span>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

// Validate that reverse_copy_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::reverse_copy_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::reverse_copy(borrowed<false>{}, nullptr_to<int>)),
    ranges::reverse_copy_result<ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::reverse_copy(borrowed<true>{}, nullptr_to<int>)),
    ranges::reverse_copy_result<int*, int*>>);

struct nontrivial_int {
    int val;

    constexpr nontrivial_int(int i = 0) noexcept : val{i} {}
    constexpr nontrivial_int(const nontrivial_int& that) noexcept : val{that.val} {}
    constexpr nontrivial_int& operator=(const nontrivial_int& that) noexcept {
        val = that.val;
        return *this;
    }

    auto operator<=>(const nontrivial_int&) const = default;
};

struct instantiator {
    static constexpr nontrivial_int input[]    = {13, 42, 1367};
    static constexpr nontrivial_int expected[] = {1367, 42, 13};

    template <ranges::bidirectional_range In, indirectly_writable<ranges::range_reference_t<In>> Out>
    static constexpr void call() {
        if constexpr (!ranges::contiguous_range<In>) { // the vectorized tests below have plenty of contiguous coverage
            using ranges::reverse_copy, ranges::reverse_copy_result, ranges::begin, ranges::end, ranges::equal,
                ranges::iterator_t;

            { // Validate iterator overload
                nontrivial_int output[3];
                In wrapped_input{input};
                const same_as<reverse_copy_result<iterator_t<In>, Out>> auto result =
                    reverse_copy(wrapped_input.begin(), wrapped_input.end(), Out{output});
                assert(result.in == wrapped_input.end());
                assert(result.out.peek() == end(output));
                assert(equal(output, expected));
            }
            { // Validate range overload
                nontrivial_int output[3];
                In wrapped_input{input};
                const same_as<reverse_copy_result<iterator_t<In>, Out>> auto result =
                    reverse_copy(wrapped_input, Out{output});
                assert(result.in == wrapped_input.end());
                assert(result.out.peek() == end(output));
                assert(equal(output, expected));
            }

            { // Validate iterator overload, empty range
                nontrivial_int output[3];
                In wrapped_input{span<const nontrivial_int, 0>{}};
                const same_as<reverse_copy_result<iterator_t<In>, Out>> auto result =
                    reverse_copy(wrapped_input.begin(), wrapped_input.end(), Out{output});
                assert(result.in == wrapped_input.end());
                assert(result.out.peek() == begin(output));
            }
            { // Validate range overload, empty range
                nontrivial_int output[3];
                In wrapped_input{span<const nontrivial_int, 0>{}};
                const same_as<reverse_copy_result<iterator_t<In>, Out>> auto result =
                    reverse_copy(wrapped_input, Out{output});
                assert(result.in == wrapped_input.end());
                assert(result.out.peek() == begin(output));
            }
        }
    }
};

template <size_t N>
struct bytes {
    unsigned char storage[N];

    constexpr bytes() {
        ranges::fill(storage, static_cast<unsigned char>(-1));
    }

    constexpr bytes(unsigned char base) {
        iota(storage, storage + N, base);
    }

    bool operator==(const bytes&) const = default;
};

struct test_vector {
    template <ranges::contiguous_range In, indirectly_writable<ranges::range_reference_t<In>> Out>
    static constexpr void call() {
        using ranges::reverse_copy, ranges::reverse_copy_result, ranges::begin, ranges::end, ranges::equal,
            ranges::iterator_t, ranges::range_value_t;

        const range_value_t<In> input[3]    = {0x10, 0x20, 0x30};
        const range_value_t<In> expected[3] = {0x30, 0x20, 0x10};

        { // Validate iterator overload, vectorizable
            range_value_t<In> output[3];
            In wrapped_input{input};
            const same_as<reverse_copy_result<iterator_t<In>, Out>> auto result =
                reverse_copy(wrapped_input.begin(), wrapped_input.end(), Out{output});
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == end(output));
            assert(equal(output, expected));
        }
        { // Validate range overload, vectorizable
            range_value_t<In> output[3];
            In wrapped_input{input};
            const same_as<reverse_copy_result<iterator_t<In>, Out>> auto result =
                reverse_copy(wrapped_input, Out{output});
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == end(output));
            assert(equal(output, expected));
        }

        { // Validate iterator overload, vectorizable empty
            range_value_t<In> output[3];
            In wrapped_input{span<range_value_t<In>, 0>{}};
            const same_as<reverse_copy_result<iterator_t<In>, Out>> auto result =
                reverse_copy(wrapped_input.begin(), wrapped_input.end(), Out{output});
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == begin(output));
        }
        { // Validate range overload, vectorizable empty
            range_value_t<In> output[3];
            In wrapped_input{span<range_value_t<In>, 0>{}};
            const same_as<reverse_copy_result<iterator_t<In>, Out>> auto result =
                reverse_copy(wrapped_input, Out{output});
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == begin(output));
        }
    }
};


// These tests take up a lot of memory,
// so we split them up into multiple functions to make sure the compiler doesn't run out of heap.
void test1() {
    STATIC_ASSERT((test_bidi_write<instantiator, const nontrivial_int, nontrivial_int>(), true));
    test_bidi_write<instantiator, const nontrivial_int, nontrivial_int>();

    STATIC_ASSERT((test_contiguous_write<test_vector, const bytes<1>, bytes<1>>(), true));
    STATIC_ASSERT((test_contiguous_write<test_vector, const bytes<2>, bytes<2>>(), true));
    test_contiguous_write<test_vector, const bytes<1>, bytes<1>>();
    test_contiguous_write<test_vector, const bytes<2>, bytes<2>>();
}

void test2() {
    STATIC_ASSERT((test_contiguous_write<test_vector, const bytes<4>, bytes<4>>(), true));
    STATIC_ASSERT((test_contiguous_write<test_vector, const bytes<8>, bytes<8>>(), true));
    STATIC_ASSERT((test_contiguous_write<test_vector, const bytes<3>, bytes<3>>(), true));
    test_contiguous_write<test_vector, const bytes<4>, bytes<4>>();
    test_contiguous_write<test_vector, const bytes<8>, bytes<8>>();
    test_contiguous_write<test_vector, const bytes<3>, bytes<3>>();
}

int main() {
    test1();
    test2();
}
