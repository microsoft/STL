// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <span>

#include <range_algorithm_support.hpp>
using namespace std;

struct instantiator {
    static constexpr int input[3] = {0, 1, 2};

    template <ranges::input_range Read>
    static constexpr void call() {
        using ranges::ref_view, ranges::iterator_t, ranges::sentinel_t, ranges::equal, ranges::begin, ranges::end;

        { // constructors
            [[maybe_unused]] ref_view<Read> default_constructed{};

            Read wrapped_input{input};
            [[maybe_unused]] ref_view<Read> same_range{wrapped_input};
        }

        { // access
            Read wrapped_input{input};
            ref_view<Read> test_view{wrapped_input};
            auto& base_range = test_view.base();
            STATIC_ASSERT(same_as<decltype(base_range), Read&>);
            if constexpr (ranges::forward_range<Read>) {
                assert(equal(base_range, wrapped_input));
            }
        }

        { // iterators
            Read wrapped_input{input};
            ref_view<Read> test_view{wrapped_input};
            const same_as<iterator_t<Read>> auto begin_iterator = test_view.begin();
            if constexpr (ranges::forward_range<Read>) {
                assert(begin_iterator == begin(wrapped_input));
            }

            const same_as<sentinel_t<Read>> auto end_iterator = test_view.end();
            if constexpr (ranges::forward_range<Read>) {
                assert(end_iterator.peek() == end(wrapped_input).peek());
            }
        }

        { // state
            if constexpr (ranges::sized_range<Read>) {
                Read wrapped_input{input};
                ref_view<Read> test_view{wrapped_input};

                const same_as<ranges::range_size_t<Read>> auto ref_size = test_view.size();
                assert(ref_size == size(wrapped_input));
            }

            if constexpr (ranges::contiguous_range<Read>) {
                Read wrapped_input{input};
                ref_view<Read> test_view{wrapped_input};

                const same_as<const int*> auto ref_data = test_view.data();
                assert(ref_data == input);
            }

            span<const int, 3> spanInput{input};
            ref_view span_view{spanInput};

            const same_as<bool> auto ref_empty = span_view.empty();
            assert(!ref_empty);
        }
    }
};

int main() {
    STATIC_ASSERT((test_in<instantiator, const int>(), true));
    test_in<instantiator, const int>();
}
