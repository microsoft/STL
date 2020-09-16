// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <concepts>
#include <iterator>
#include <type_traits>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;
using P = pair<int, int>;

// clang-format off
template <class Iter>
concept CanDifference = requires(Iter it) {
    { it - it };
};

template <class Iter>
concept HasProxy = !is_reference_v<iter_reference_t<Iter>>;
// clang-format on

struct instantiator {
    template <input_or_output_iterator Iter>
    static constexpr void call() {
        if constexpr (copyable<Iter>) {
            using ConstIter = typename Iter::Consterator;
            using Sen       = test::sentinel<iter_value_t<Iter>>;
            using OSen      = test::sentinel<const iter_value_t<Iter>>;
            using Cit       = common_iterator<Iter, Sen>;
            using OCit      = common_iterator<ConstIter, OSen>;
            P input[3]      = {{0, 1}, {0, 2}, {0, 3}};

            // [common.iter.types]
            {
                using iconcept = typename iterator_traits<Cit>::iterator_concept;
                if constexpr (forward_iterator<Iter>) {
                    STATIC_ASSERT(same_as<iconcept, forward_iterator_tag>);
                } else {
                    STATIC_ASSERT(same_as<typename iterator_traits<Cit>::iterator_concept, input_iterator_tag>);
                }

                using icat = typename iterator_traits<Cit>::iterator_category;
                if constexpr (derived_from<icat, forward_iterator_tag>) {
                    STATIC_ASSERT(same_as<icat, forward_iterator_tag>);
                } else {
                    STATIC_ASSERT(same_as<icat, input_iterator_tag>);
                }

                using ipointer = typename iterator_traits<Cit>::pointer;
                if constexpr (_Has_member_arrow<Iter>) {
                    STATIC_ASSERT(same_as<ipointer, decltype(declval<const Iter&>().operator->())>);
                } else {
                    STATIC_ASSERT(same_as<ipointer, void>);
                }
            }

            { // [common.iter.const]
                Cit defaultConstructed{};
                Cit iterConstructed{Iter{input}};
                Cit sentinelConstructed(Sen{});
                Cit copyConstructed{defaultConstructed};
                copyConstructed = iterConstructed;

                OCit conversionConstructed{defaultConstructed};
                conversionConstructed = iterConstructed;

                OCit conversionConstructedSentinel{sentinelConstructed};
                conversionConstructed = iterConstructed;
            }

            { // [common.iter.access]
                Cit iter{Iter{input}};
                assert(*iter == P(0, 1));
                assert(iter->first == 0);
                assert(iter->second == 1);
                if constexpr (HasProxy<Iter>) {
                    // We return a proxy class here
                    static_assert(is_class_v<decltype(iter.operator->())>);
                } else {
                    // Either a pointer or the wrapped iterator
                    static_assert(!is_class_v<decltype(iter.operator->())>);
                }

                const Cit constIter{Iter{input}};
                assert(*constIter == P(0, 1));
                assert(constIter->first == 0);
                assert(constIter->second == 1);
                if constexpr (HasProxy<Iter>) {
                    // We return a proxy class here
                    static_assert(is_class_v<decltype(constIter.operator->())>);
                } else {
                    // Either a pointer or the wrapped iterator
                    static_assert(!is_class_v<decltype(constIter.operator->())>);
                }
            }

            { // [common.iter.nav]
                Cit iter{Iter{input}};
                ++iter;
                assert(*iter == P(0, 2));

                assert(*iter++ == P(0, 2));
                assert(*iter == P(0, 3));
            }

            { // [common.iter.cmp]
                // Compare iterator / iterator
                assert(Cit{Iter{input}} == Cit{Iter{input}});
                assert(Cit{Iter{input}} != Cit{Iter{input + 1}});

                // Compare iterator / sentinel
                assert(Cit{Iter{input}} == Cit{Sen{input}});
                assert(Cit{Sen{input}} != Cit{Iter{input + 1}});

                // Compare sentinel / sentinel
                assert(Cit{Sen{input}} == Cit{Sen{input}});
                assert(Cit{Sen{input}} == Cit{Sen{input + 1}});

                if constexpr (CanDifference<Iter>) {
                    // Difference iterator / iterator
                    const same_as<iter_difference_t<Iter>> auto diff_it_it = Cit{Iter{input}} - Cit{Iter{input + 1}};
                    assert(diff_it_it == -1);

                    // Difference iterator / sentinel
                    const same_as<iter_difference_t<Iter>> auto diff_it_sen = Cit{Iter{input}} - Cit{Sen{input + 1}};
                    const same_as<iter_difference_t<Iter>> auto diff_sen_it = Cit{Sen{input + 1}} - Cit{Iter{input}};
                    assert(diff_it_sen == -1);
                    assert(diff_sen_it == 1);

                    // Difference sentinel / sentinel
                    const same_as<iter_difference_t<Iter>> auto diff_sen_sen = Cit{Sen{input}} - Cit{Sen{input + 1}};
                    assert(diff_sen_sen == 0);

                    // Difference iterator / other iterator
                    const same_as<iter_difference_t<Iter>> auto diff_it_oit = Cit{Iter{input}} - OCit{Iter{input + 1}};
                    assert(diff_it_oit == -1);

                    // Difference iterator / other sentinel
                    const same_as<iter_difference_t<Iter>> auto diff_it_osen = Cit{Iter{input}} - OCit{OSen{input + 1}};
                    assert(diff_it_osen == -1);

                    // Difference other iterator / sentinel
                    const same_as<iter_difference_t<Iter>> auto diff_sen_oit = Cit{Sen{input + 1}} - OCit{Iter{input}};
                    assert(diff_sen_oit == 1);

                    // Difference sentinel / other sentinel
                    const same_as<iter_difference_t<Iter>> auto diff_sen_osen = Cit{Sen{input}} - OCit{OSen{input + 1}};
                    assert(diff_sen_osen == 0);
                }
            }

            { // [common.iter.cust]
                if constexpr (input_iterator<Iter>) { // iter_move
                    Cit iter1{Iter{input}};

                    const same_as<iter_value_t<Iter>> auto element1 = ranges::iter_move(iter1);
                    assert(element1 == P(0, 1));
                }

                if constexpr (indirectly_swappable<Iter>) { // iter_swap
                    Cit iter1{Iter{input}};
                    Cit iter2{Iter{input + 1}};

                    ranges::iter_swap(iter1, iter2);
                    assert(*iter1 == P(0, 2));
                    assert(*iter2 == P(0, 1));
                }
            }
        }
    }
};

bool test_operator_arrow() {
    P input[3] = {{0, 1}, {0, 2}, {0, 3}};

    using pointerTest = common_iterator<P*, void*>;
    pointerTest pointerIter{input};

    assert(*pointerIter == P(0, 1));
    assert(pointerIter->first == 0);
    assert(pointerIter->second == 1);
    static_assert(is_same_v<decltype(pointerIter.operator->()), P* const&>);

    using countedTest = common_iterator<counted_iterator<P*>, default_sentinel_t>;
    countedTest countedIter{counted_iterator{input, 3}};

    assert(*countedIter == P(0, 1));
    assert(countedIter->first == 0);
    assert(countedIter->second == 1);
    static_assert(is_same_v<decltype(countedIter.operator->()), P*>);

    return true;
}

int main() {
    with_writable_iterators<instantiator, P>::call();

    test_operator_arrow();
}
