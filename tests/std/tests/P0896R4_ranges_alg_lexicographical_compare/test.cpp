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
    static constexpr P left[]                  = {{0, 10}, {1, 20}, {2, 30}};
    static constexpr P right_shorter_less[]    = {{300, 0}, {200, 0}};
    static constexpr P right_shorter_same[]    = {{300, 0}, {200, 1}};
    static constexpr P right_shorter_greater[] = {{300, 0}, {200, 2}};
    static constexpr P right_less[]            = {{300, 0}, {200, 1}, {100, 1}};
    static constexpr P right_equal[]           = {{300, 0}, {200, 1}, {100, 2}};
    static constexpr P right_greater[]         = {{300, 0}, {200, 1}, {100, 3}};
    static constexpr P right_longer_less[]     = {{300, 0}, {200, 1}, {100, 1}, {0, 3}};
    static constexpr P right_longer_same[]     = {{300, 0}, {200, 1}, {100, 2}, {0, 3}};
    static constexpr P right_longer_greater[]  = {{300, 0}, {200, 1}, {100, 3}, {0, 3}};

    template <class In1, class In2>
    static constexpr void call() {
        using ranges::lexicographical_compare, ranges::less;

        // Validate range overload
        {
            In1 range1{left};
            In2 range2{right_shorter_less};
            const same_as<bool> auto result = lexicographical_compare(range1, range2, less{}, get_first, get_second);
            assert(!result);
        }
        {
            In1 range1{left};
            In2 range2{right_shorter_same};
            const same_as<bool> auto result = lexicographical_compare(range1, range2, less{}, get_first, get_second);
            assert(!result);
        }
        {
            In1 range1{left};
            In2 range2{right_shorter_greater};
            const same_as<bool> auto result = lexicographical_compare(range1, range2, less{}, get_first, get_second);
            assert(result);
        }

        {
            In1 range1{left};
            In2 range2{right_less};
            const same_as<bool> auto result = lexicographical_compare(range1, range2, less{}, get_first, get_second);
            assert(!result);
        }
        {
            In1 range1{left};
            In2 range2{right_equal};
            const same_as<bool> auto result = lexicographical_compare(range1, range2, less{}, get_first, get_second);
            assert(!result);
        }
        {
            In1 range1{left};
            In2 range2{right_greater};
            const same_as<bool> auto result = lexicographical_compare(range1, range2, less{}, get_first, get_second);
            assert(result);
        }

        {
            In1 range1{left};
            In2 range2{right_longer_less};
            const same_as<bool> auto result = lexicographical_compare(range1, range2, less{}, get_first, get_second);
            assert(!result);
        }
        {
            In1 range1{left};
            In2 range2{right_longer_same};
            const same_as<bool> auto result = lexicographical_compare(range1, range2, less{}, get_first, get_second);
            assert(result);
        }
        {
            In1 range1{left};
            In2 range2{right_longer_greater};
            const same_as<bool> auto result = lexicographical_compare(range1, range2, less{}, get_first, get_second);
            assert(result);
        }

        {
            In1 empty1{};
            In2 range2{right_equal};
            const same_as<bool> auto result = lexicographical_compare(empty1, range2, less{}, get_first, get_second);
            assert(result);
        }
        {
            In1 range1{left};
            In2 empty2{};
            const same_as<bool> auto result = lexicographical_compare(range1, empty2, less{}, get_first, get_second);
            assert(!result);
        }
        {
            In1 empty1{};
            In2 empty2{};
            const same_as<bool> auto result = lexicographical_compare(empty1, empty2, less{}, get_first, get_second);
            assert(!result);
        }

        // Validate iterator overload
        {
            In1 range1{left};
            In2 range2{right_shorter_less};
            const same_as<bool> auto result = lexicographical_compare(
                range1.begin(), range1.end(), range2.begin(), range2.end(), less{}, get_first, get_second);
            assert(!result);
        }
        {
            In1 range1{left};
            In2 range2{right_shorter_same};
            const same_as<bool> auto result = lexicographical_compare(
                range1.begin(), range1.end(), range2.begin(), range2.end(), less{}, get_first, get_second);
            assert(!result);
        }
        {
            In1 range1{left};
            In2 range2{right_shorter_greater};
            const same_as<bool> auto result = lexicographical_compare(
                range1.begin(), range1.end(), range2.begin(), range2.end(), less{}, get_first, get_second);
            assert(result);
        }

        {
            In1 range1{left};
            In2 range2{right_less};
            const same_as<bool> auto result = lexicographical_compare(
                range1.begin(), range1.end(), range2.begin(), range2.end(), less{}, get_first, get_second);
            assert(!result);
        }
        {
            In1 range1{left};
            In2 range2{right_equal};
            const same_as<bool> auto result = lexicographical_compare(
                range1.begin(), range1.end(), range2.begin(), range2.end(), less{}, get_first, get_second);
            assert(!result);
        }
        {
            In1 range1{left};
            In2 range2{right_greater};
            const same_as<bool> auto result = lexicographical_compare(
                range1.begin(), range1.end(), range2.begin(), range2.end(), less{}, get_first, get_second);
            assert(result);
        }

        {
            In1 range1{left};
            In2 range2{right_longer_less};
            const same_as<bool> auto result = lexicographical_compare(
                range1.begin(), range1.end(), range2.begin(), range2.end(), less{}, get_first, get_second);
            assert(!result);
        }
        {
            In1 range1{left};
            In2 range2{right_longer_same};
            const same_as<bool> auto result = lexicographical_compare(
                range1.begin(), range1.end(), range2.begin(), range2.end(), less{}, get_first, get_second);
            assert(result);
        }
        {
            In1 range1{left};
            In2 range2{right_longer_greater};
            const same_as<bool> auto result = lexicographical_compare(
                range1.begin(), range1.end(), range2.begin(), range2.end(), less{}, get_first, get_second);
            assert(result);
        }

        {
            In1 empty1{};
            In2 range2{right_equal};
            const same_as<bool> auto result = lexicographical_compare(
                empty1.begin(), empty1.end(), range2.begin(), range2.end(), less{}, get_first, get_second);
            assert(result);
        }
        {
            In1 range1{left};
            In2 empty2{};
            const same_as<bool> auto result = lexicographical_compare(
                range1.begin(), range1.end(), empty2.begin(), empty2.end(), less{}, get_first, get_second);
            assert(!result);
        }
        {
            In1 empty1{};
            In2 empty2{};
            const same_as<bool> auto result = lexicographical_compare(
                empty1.begin(), empty1.end(), empty2.begin(), empty2.end(), less{}, get_first, get_second);
            assert(!result);
        }
    }
};

#ifdef TEST_EVERYTHING
int main() {
    // No constexpr tests here: we hit the constexpr step limits too quickly.
    test_in_in<instantiator, const P, const P>();
}
#else // ^^^ test all range combinations / test only interesting combinations vvv
template <test::ProxyRef IsProxy>
using range_type = test::range<input_iterator_tag, const P, test::Sized::no, test::CanDifference::no, test::Common::no,
    test::CanCompare::no, IsProxy>;

constexpr void run_tests() {
    // The algorithm is very much oblivious to any properties above the minimum requirements: category, size,
    // difference, none of it matters. Let's test input ranges with and without proxy references.
    using test::ProxyRef;

    instantiator::call<range_type<ProxyRef::yes>, range_type<ProxyRef::yes>>();
    instantiator::call<range_type<ProxyRef::yes>, range_type<ProxyRef::no>>();
    instantiator::call<range_type<ProxyRef::no>, range_type<ProxyRef::yes>>();
    instantiator::call<range_type<ProxyRef::no>, range_type<ProxyRef::no>>();
}

int main() {
    STATIC_ASSERT((run_tests(), true));
    run_tests();
}
#endif // TEST_EVERYTHING
