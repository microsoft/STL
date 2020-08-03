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

struct instantiator {
    static constexpr P haystack_elements[]     = {{0, 0}, {0, 1}, {1, 0}, {2, 0}, {2, 1}, {4, 0}, {4, 1}};
    static constexpr int needle_elements[]     = {-1, 0, 3, 3};
    static constexpr int bad_needle_elements[] = {-1, 0, 2, 2};

    static constexpr auto add_one = [](const int x) { return x + 1; };

    template <ranges::input_range Haystack, ranges::input_range Needle>
    static constexpr void call() {
        using ranges::includes, ranges::less;

        { // Validate range overload
            Haystack haystack{haystack_elements};
            Needle needle{needle_elements};
            const same_as<bool> auto result = includes(haystack, needle, ranges::less{}, get_first, add_one);
            assert(result);
        }
        { // Validate iterator overload
            Haystack haystack{haystack_elements};
            Needle needle{needle_elements};
            const same_as<bool> auto result = includes(
                haystack.begin(), haystack.end(), needle.begin(), needle.end(), ranges::less{}, get_first, add_one);
            assert(result);
        }

        { // Validate range overload, empty haystack
            Haystack haystack{};
            Needle needle{needle_elements};
            const same_as<bool> auto result = includes(haystack, needle, ranges::less{}, get_first, add_one);
            assert(!result);
        }
        { // Validate iterator overload, empty needle
            Haystack haystack{haystack_elements};
            Needle needle{};
            const same_as<bool> auto result = includes(
                haystack.begin(), haystack.end(), needle.begin(), needle.end(), ranges::less{}, get_first, add_one);
            assert(result);
        }

        { // Validate range overload, needle not found
            Haystack haystack{haystack_elements};
            Needle needle{bad_needle_elements};
            const same_as<bool> auto result = includes(haystack, needle, ranges::less{}, get_first, add_one);
            assert(!result);
        }
    }
};

#ifdef TEST_EVERYTHING
int main() {
    STATIC_ASSERT((test_in_in<instantiator, const P, const int>(), true));
    test_in_in<instantiator, const P, const int>();
}
#else // ^^^ test all permutations of range properties / test only interesting permutations vvv
template <class Category, class Element, test::ProxyRef IsProxyRef>
using test_range = test::range<Category, Element, test::Sized::no, test::CanDifference::no, test::Common::no,
    test::CanCompare{derived_from<Category, forward_iterator_tag>}, IsProxyRef>;

constexpr void run_tests() {
    using namespace test;
    using test::iterator, test::range;

    // The algorithm is completely oblivious to:
    // * categories stronger than input
    // * whether the end sentinel is an iterator
    // * size information
    // * iterator and/or sentinel differencing
    // so let's vary proxyness for coverage and add a range of each category out of paranoia.

    instantiator::call<test_range<input, const P, ProxyRef::yes>, test_range<input, const int, ProxyRef::yes>>();
    instantiator::call<test_range<input, const P, ProxyRef::yes>, test_range<input, const int, ProxyRef::no>>();
    instantiator::call<test_range<input, const P, ProxyRef::no>, test_range<input, const int, ProxyRef::yes>>();
    instantiator::call<test_range<input, const P, ProxyRef::no>, test_range<input, const int, ProxyRef::no>>();

    instantiator::call<test_range<input, const P, ProxyRef::yes>, test_range<fwd, const int, ProxyRef::yes>>();
    instantiator::call<test_range<input, const P, ProxyRef::yes>, test_range<bidi, const int, ProxyRef::yes>>();
    instantiator::call<test_range<input, const P, ProxyRef::yes>, test_range<random, const int, ProxyRef::yes>>();
    instantiator::call<test_range<input, const P, ProxyRef::yes>, test_range<contiguous, const int, ProxyRef::no>>();

    instantiator::call<test_range<fwd, const P, ProxyRef::yes>, test_range<input, const int, ProxyRef::yes>>();
    instantiator::call<test_range<bidi, const P, ProxyRef::yes>, test_range<input, const int, ProxyRef::yes>>();
    instantiator::call<test_range<random, const P, ProxyRef::yes>, test_range<input, const int, ProxyRef::yes>>();
    instantiator::call<test_range<contiguous, const P, ProxyRef::no>, test_range<input, const int, ProxyRef::yes>>();
}

int main() {
    STATIC_ASSERT((run_tests(), true));
    run_tests();
}
#endif // TEST_EVERYTHING
