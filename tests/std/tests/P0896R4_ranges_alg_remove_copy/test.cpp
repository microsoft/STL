// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;
using P = pair<int, int>;

// Validate that remove_copy_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::remove_copy_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::remove_copy(borrowed<false>{}, static_cast<int*>(nullptr), 42)),
    ranges::remove_copy_result<ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::remove_copy(borrowed<true>{}, static_cast<int*>(nullptr), 42)),
    ranges::remove_copy_result<int*, int*>>);

struct counted_projection {
    size_t& counter;

    constexpr auto operator()(const P& val) const noexcept(is_nothrow_copy_constructible_v<decltype(P::second)>) {
        ++counter;
        return val.second;
    }
};

struct instantiator {
    static constexpr P input[5]    = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};
    static constexpr P expected[3] = {{0, 99}, {2, 99}, {4, 99}};

    template <ranges::input_range Read, indirectly_writable<ranges::range_reference_t<Read>> Write>
    static constexpr void call() {
        // Fails checking the indirect_binary_predicate requirement in C1XX's permissive mode with proxy iterators
        // (probably related to VSO-566808)
        constexpr bool non_proxy =
            is_reference_v<ranges::range_reference_t<Read>> && is_reference_v<iter_reference_t<Write>>;
        if constexpr (non_proxy || !is_permissive) {
            using ranges::remove_copy, ranges::remove_copy_result, ranges::equal, ranges::iterator_t;

            size_t counter = 0;
            counted_projection projection{counter};

            { // Validate iterator + sentinel overload
                P output[3] = {{-1, -1}, {-1, -1}, {-1, -1}};
                Read wrapped_input{input};

                auto result = remove_copy(wrapped_input.begin(), wrapped_input.end(), Write{output}, 47, projection);
                STATIC_ASSERT(same_as<decltype(result), remove_copy_result<iterator_t<Read>, Write>>);
                assert(result.in == wrapped_input.end());
                assert(result.out.peek() == output + 3);
                assert(equal(output, expected));
                assert(counter == ranges::size(input));
            }

            counter = 0;

            { // Validate range overload
                P output[3] = {{-1, -1}, {-1, -1}, {-1, -1}};
                Read wrapped_input{input};

                auto result = remove_copy(wrapped_input, Write{output}, 47, projection);
                STATIC_ASSERT(same_as<decltype(result), remove_copy_result<iterator_t<Read>, Write>>);
                assert(result.in == wrapped_input.end());
                assert(result.out.peek() == output + 3);
                assert(equal(output, expected));
                assert(counter == ranges::size(input));
            }
        }
    }
};

using test::Common, test::Sized;

template <class Category, Sized IsSized, Common IsCommon>
using test_range = test::range<Category, const P, IsSized,
    test::CanDifference{derived_from<Category, random_access_iterator_tag>}, IsCommon,
    test::CanCompare{derived_from<Category, forward_iterator_tag> || IsCommon == Common::yes}, test::ProxyRef::no>;

int main() {
#ifdef TEST_EVERYTHING
    STATIC_ASSERT((test_in_write<instantiator, const P, P>(), true));
    test_in_write<instantiator, const P, P>();
#else // ^^^ test all input range permutations / test only "interesting" permutations vvv
    // The algorithm is insensitive to _every_ range property; it's simply a conditional copy.
    // Let's test a range of each category for basic coverage, and a contiguous+sized range just in
    // case some ambitious contributor tries to vectorize some day.

    using out =
        test::iterator<output_iterator_tag, P, test::CanDifference::no, test::CanCompare::no, test::ProxyRef::no>;

    instantiator::call<test_range<input_iterator_tag, Sized::no, Common::no>, out>();
    instantiator::call<test_range<forward_iterator_tag, Sized::no, Common::no>, out>();
    instantiator::call<test_range<bidirectional_iterator_tag, Sized::no, Common::no>, out>();
    instantiator::call<test_range<random_access_iterator_tag, Sized::no, Common::no>, out>();
    instantiator::call<test_range<contiguous_iterator_tag, Sized::yes, Common::yes>, out>();
#endif // TEST_EVERYTHING
}
