// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <compare>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;
template <class T, class U>
struct not_pair {
    T first;
    U second;

    auto operator<=>(not_pair const&) const = default;

    template <size_t I>
    friend constexpr auto&& get(not_pair& np) noexcept {
        if constexpr (I == 0) {
            return np.first;
        } else {
            static_assert(I == 1);
            return np.second;
        }
    }
    template <size_t I>
    friend constexpr auto&& get(not_pair const& np) noexcept {
        if constexpr (I == 0) {
            return np.first;
        } else {
            static_assert(I == 1);
            return np.second;
        }
    }
    template <size_t I>
    friend constexpr auto&& get(not_pair&& np) noexcept {
        if constexpr (I == 0) {
            return move(np).first;
        } else {
            static_assert(I == 1);
            return move(np).second;
        }
    }
    template <size_t I>
    friend constexpr auto&& get(not_pair const&& np) noexcept {
        if constexpr (I == 0) {
            return move(np).first;
        } else {
            static_assert(I == 1);
            return move(np).second;
        }
    }
};

using P = not_pair<int, int>;

constexpr auto is_odd = [](int const x) { return x % 2 != 0; };

// Validate that copy_if_result aliases in_out_result
static_assert(same_as<ranges::copy_if_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
static_assert(same_as<decltype(ranges::copy_if(borrowed<false>{}, nullptr_to<int>, is_odd)),
    ranges::copy_if_result<ranges::dangling, int*>>);
static_assert(
    same_as<decltype(ranges::copy_if(borrowed<true>{}, nullptr_to<int>, is_odd)), ranges::copy_if_result<int*, int*>>);

struct instantiator {
    static constexpr P input[3]    = {{1, 99}, {4, 98}, {5, 97}};
    static constexpr P expected[2] = {{1, 99}, {5, 97}};

    template <ranges::input_range Read, indirectly_writable<ranges::range_reference_t<Read>> Write>
    static constexpr void call() {
        using ranges::copy_if, ranges::copy_if_result, ranges::iterator_t;
        { // Validate range overload
            array<P, 2> output = {{{-1, -1}, {-1, -1}}};
            Read wrapped_input{input};

            auto result = copy_if(wrapped_input, Write{output.data()}, is_odd, get_first);
            static_assert(same_as<decltype(result), copy_if_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == output.data() + 2);
            assert(ranges::equal(output, expected));
        }
        { // Validate iterator + sentinel overload
            array<P, 2> output = {{{-1, -1}, {-1, -1}}};
            Read wrapped_input{input};

            auto result = copy_if(wrapped_input.begin(), wrapped_input.end(), Write{output.data()}, is_odd, get_first);
            static_assert(same_as<decltype(result), copy_if_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == output.data() + 2);
            assert(ranges::equal(output, expected));
        }
    }
};

int main() {
#ifndef _PREFAST_ // TRANSITION, GH-1030
    static_assert((test_in_write<instantiator, P const, P>(), true));
#endif // TRANSITION, GH-1030
    test_in_write<instantiator, P const, P>();
}
