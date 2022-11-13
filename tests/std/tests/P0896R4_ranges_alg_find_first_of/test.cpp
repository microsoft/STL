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

constexpr auto pred = [](const int x, const int y) { return x == y + 1; };

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::find_first_of(borrowed<false>{}, borrowed<true>{}, pred)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::find_first_of(borrowed<true>{}, borrowed<true>{}, pred)), int*>);

struct instantiator {
    static constexpr P haystack[7]      = {{0, 42}, {1, 42}, {2, 42}, {3, 42}, {4, 42}, {5, 42}, {6, 42}};
    static constexpr int good_needle[2] = {29, 1};
    static constexpr int bad_needle[2]  = {29, 17};

    template <ranges::input_range Read1, ranges::forward_range Read2>
    static constexpr void call() {
        using ranges::find_first_of, ranges::iterator_t;

        { // Validate range overload [found case]
            Read1 wrapped_haystack{haystack};
            Read2 wrapped_needle{good_needle};

            auto result = find_first_of(wrapped_haystack, wrapped_needle, pred, get_first);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<Read1>>);
            assert(result.peek() == begin(haystack) + 2);
        }
        { // Validate iterator + sentinel overload [found case]
            Read1 wrapped_haystack{haystack};
            Read2 wrapped_needle{good_needle};

            auto result = find_first_of(wrapped_haystack.begin(), wrapped_haystack.end(), wrapped_needle.begin(),
                wrapped_needle.end(), pred, get_first);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<Read1>>);
            assert(result.peek() == begin(haystack) + 2);
        }

        { // Validate range overload [not found case]
            Read1 wrapped_haystack{haystack};
            Read2 wrapped_needle{bad_needle};

            auto result = find_first_of(wrapped_haystack, wrapped_needle, pred, get_first);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<Read1>>);
            assert(result == wrapped_haystack.end());
        }
        {
            // Validate iterator + sentinel overload [not found case]
            Read1 wrapped_haystack{haystack};
            Read2 wrapped_needle{bad_needle};

            auto result = find_first_of(wrapped_haystack.begin(), wrapped_haystack.end(), wrapped_needle.begin(),
                wrapped_needle.end(), pred, get_first);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<Read1>>);
            assert(result == wrapped_haystack.end());
        }
    }
};

#ifdef TEST_EVERYTHING
int main() {
    STATIC_ASSERT((test_in_fwd<instantiator, const P, const int>(), true));
    test_in_fwd<instantiator, const P, const int>();
}
#else // ^^^ test all range combinations / test only interesting range combos vvv
constexpr bool run_tests() {
    // The algorithm is oblivious to anything except maybe proxies so take the bare minimum input/forward range
    using in_test_range  = test::range<input_iterator_tag, const P, test::Sized::no, test::CanDifference::no,
        test::Common::no, test::CanCompare::no, test::ProxyRef::yes>;
    using fwd_test_range = test::range<forward_iterator_tag, const int, test::Sized::no, test::CanDifference::no,
        test::Common::no, test::CanCompare::yes, test::ProxyRef::yes>;

    instantiator::call<in_test_range, fwd_test_range>();
    return true;
}

int main() {
    STATIC_ASSERT(run_tests());
    run_tests();
}
#endif // TEST_EVERYTHING
