// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <span>

#include <range_algorithm_support.hpp>
using namespace std;

// clang-format off
template<class Range>
concept can_empty = requires (Range&& range) { ranges::empty(range); };
// clang-format on

struct instantiator {

    template <class Read>
    static constexpr void call() {
        using ranges::ref_view, ranges::iterator_t, ranges::sentinel_t, ranges::equal, ranges::begin, ranges::end;
        int input[3] = {0, 1, 2};

        { // traits
            STATIC_ASSERT(ranges::input_range<Read> || ranges::output_range<Read, const int&>);
            STATIC_ASSERT(ranges::enable_borrowed_range<ref_view<Read>>);
        }

        { // constructors
            STATIC_ASSERT(!constructible_from<ref_view<Read>, Read>);

            [[maybe_unused]] ref_view<Read> default_constructed{};
            STATIC_ASSERT(is_nothrow_default_constructible_v<ref_view<Read>>);

            Read wrapped_input{input};
            [[maybe_unused]] ref_view<Read> same_range{wrapped_input};
        }

        { // access
            Read wrapped_input{input};
            ref_view<Read> test_view{wrapped_input};
            auto& base_range = test_view.base();
            STATIC_ASSERT(same_as<decltype(base_range), Read&>);
            assert(addressof(base_range) == addressof(wrapped_input));

            STATIC_ASSERT(noexcept(test_view.base()));
        }

        { // iterators
            Read wrapped_input{input};
            ref_view<Read> test_view{wrapped_input};
            const same_as<iterator_t<Read>> auto first = test_view.begin();
            assert(first.peek() == input);

            const same_as<sentinel_t<Read>> auto last = test_view.end();
            assert(last.peek() == end(input));
        }

        { // state
            if constexpr (ranges::sized_range<Read>) {
                Read wrapped_input{input};
                ref_view<Read> test_view{wrapped_input};

                const same_as<ranges::range_size_t<Read>> auto ref_size = test_view.size();
                assert(ref_size == size(wrapped_input));
            } else {
                STATIC_ASSERT(!ranges::_Size::_Has_member<ref_view<Read>, remove_cvref_t<ref_view<Read>>>);
            }

            if constexpr (ranges::contiguous_range<Read>) {
                Read wrapped_input{input};
                ref_view<Read> test_view{wrapped_input};

                const same_as<int*> auto ref_data = test_view.data();
                assert(ref_data == input);
            } else {
                STATIC_ASSERT(!ranges::_Data::_Has_member<ref_view<Read>>);
            }

            if constexpr (can_empty<Read>) {
                Read wrapped_input{input};
                ref_view<Read> test_view{wrapped_input};

                const same_as<bool> auto ref_empty = test_view.empty();
                assert(!ref_empty);
            } else {
                STATIC_ASSERT(!ranges::_Empty::_Has_member<ref_view<Read>>);
            }
        }

        { // CTAD
            span<const int, 3> spanInput{input};
            ref_view span_view{spanInput};
            STATIC_ASSERT(same_as<decltype(span_view), ref_view<span<const int, 3>>>);
        }
    }
};

int main() {
    STATIC_ASSERT((test_inout<instantiator, int>(), true));
    test_inout<instantiator, int>();
}
