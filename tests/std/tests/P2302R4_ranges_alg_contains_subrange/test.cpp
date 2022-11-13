// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <span>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

using Elem1 = const pair<int, int>;
using Elem2 = const int;

struct instantiator {
    static constexpr pair<int, int> haystack[] = {{0, 42}, {1, 42}, {2, 42}, {3, 42}, {4, 42}, {5, 42}};
    static constexpr int needle[]              = {2, 3, 4};

    template <ranges::forward_range Fwd1, ranges::forward_range Fwd2>
    static constexpr void call() {
        using ranges::contains_subrange, ranges::begin, ranges::end;

        { // Validate range overload [found case]
            const same_as<bool> auto result =
                contains_subrange(Fwd1{haystack}, Fwd2{needle}, ranges::equal_to{}, get_first);
            assert(result);
        }
        { // Validate iterator + sentinel overload [found case]
            const Fwd1 wrap_hay{haystack};
            const Fwd2 wrap_needle{needle};
            const same_as<bool> auto result = contains_subrange(
                begin(wrap_hay), end(wrap_hay), begin(wrap_needle), end(wrap_needle), ranges::equal_to{}, get_first);
            assert(result);
        }
        { // Validate range overload [not found case]
            const same_as<bool> auto result =
                contains_subrange(Fwd1{haystack}, Fwd2{needle}, ranges::equal_to{}, get_second);
            assert(!result);
        }
        { // Validate iterator + sentinel overload [not found case]
            const Fwd1 wrap_hay{haystack};
            const Fwd2 wrap_needle{needle};
            const same_as<bool> auto result = contains_subrange(
                begin(wrap_hay), end(wrap_hay), begin(wrap_needle), end(wrap_needle), ranges::equal_to{}, get_second);
            assert(!result);
        }
        { // Validate empty needle case
            const span<Elem1> empty;
            const same_as<bool> auto result = contains_subrange(Fwd1{haystack}, Fwd1{empty});
            assert(result);
        }
        { // Validate unreachable_sentinel case
            const Fwd1 wrap_hay{haystack};
            const Fwd2 wrap_needle{needle};
            const same_as<bool> auto result = contains_subrange(begin(wrap_hay), unreachable_sentinel,
                begin(wrap_needle), end(wrap_needle), ranges::equal_to{}, get_first);
            assert(result);
        }
    }
};

#ifdef TEST_EVERYTHING
int main() {
    STATIC_ASSERT((test_fwd_fwd<instantiator, Elem1, Elem2>(), true));
    test_fwd_fwd<instantiator, Elem1, Elem2>();
}
#else // ^^^ test all range combinations / test only interesting range combos vvv
template <class Elem, test::Sized IsSized>
using fwd_test_range = test::range<forward_iterator_tag, Elem, IsSized, test::CanDifference::no, test::Common::no,
    test::CanCompare::yes, test::ProxyRef::yes>;
template <class Elem, test::Sized IsSized, test::Common IsCommon>
using random_test_range = test::range<random_access_iterator_tag, Elem, IsSized, test::CanDifference::no, IsCommon,
    test::CanCompare::yes, test::ProxyRef::no>;

constexpr bool run_tests() {
    // All (except contiguous) proxy reference types, since the algorithm doesn't really care. Cases with only 1 range
    // sized are not interesting; common is interesting only in that it's necessary to trigger memcmp optimization.

    using test::Common, test::Sized;

    // both forward, non-common, and sized or unsized
    instantiator::call<fwd_test_range<Elem1, Sized::no>, fwd_test_range<Elem2, Sized::no>>();
    instantiator::call<fwd_test_range<Elem1, Sized::yes>, fwd_test_range<Elem2, Sized::yes>>();

    // both random-access, and sized or unsized; all permutations of common
    instantiator::call<random_test_range<Elem1, Sized::no, Common::no>,
        random_test_range<Elem2, Sized::no, Common::no>>();
    instantiator::call<random_test_range<Elem1, Sized::no, Common::no>,
        random_test_range<Elem2, Sized::no, Common::yes>>();
    instantiator::call<random_test_range<Elem1, Sized::no, Common::yes>,
        random_test_range<Elem2, Sized::no, Common::no>>();
    instantiator::call<random_test_range<Elem1, Sized::no, Common::yes>,
        random_test_range<Elem2, Sized::no, Common::yes>>();
    instantiator::call<random_test_range<Elem1, Sized::yes, Common::no>,
        random_test_range<Elem2, Sized::yes, Common::no>>();
    instantiator::call<random_test_range<Elem1, Sized::yes, Common::no>,
        random_test_range<Elem2, Sized::yes, Common::yes>>();
    instantiator::call<random_test_range<Elem1, Sized::yes, Common::yes>,
        random_test_range<Elem2, Sized::yes, Common::no>>();
    instantiator::call<random_test_range<Elem1, Sized::yes, Common::yes>,
        random_test_range<Elem2, Sized::yes, Common::yes>>();

    return true;
}

int main() {
    STATIC_ASSERT(run_tests());
    run_tests();
}
#endif // TEST_EVERYTHING
