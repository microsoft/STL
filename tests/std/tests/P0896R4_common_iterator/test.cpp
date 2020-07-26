// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <concepts>
#include <iterator>

#include <range_algorithm_support.hpp>
using namespace std;

struct instantiator {
    static constexpr int input[3] = {1, 2, 3};

    template <input_or_output_iterator Iter>
    static constexpr void call() {
        if constexpr (copyable<Iter>) {
            using Sen = test::sentinel<const iter_value_t<Iter>>;
            using Cit = common_iterator<Iter, Sen>;

            // [common.iter.types]
            if constexpr (forward_iterator<Iter>) {
                STATIC_ASSERT(same_as<iterator_traits<Cit>::iterator_concept, forward_iterator_tag>);
            } else {
                STATIC_ASSERT(same_as<iterator_traits<Cit>::iterator_concept, input_iterator_tag>);
            }

            if constexpr (derived_from<iterator_traits<Iter>::iterator_category, forward_iterator_tag>) {
                STATIC_ASSERT(same_as<iterator_traits<Cit>::iterator_category, forward_iterator_tag>);
            } else {
                STATIC_ASSERT(same_as<iterator_traits<Cit>::iterator_category, input_iterator_tag>);
            }

            if constexpr (_Has_op_arrow<Iter>) {
                STATIC_ASSERT(same_as<iterator_traits<Cit>::pointer, decltype(declval<Cit&>().operator->())>);
            } else {
                STATIC_ASSERT(same_as<iterator_traits<Cit>::pointer, void>);
            }

            { // [counted.iter.const]
                [[maybe_unused]] Cit defaultConstructed{};

                [[maybe_unused]] Cit iterConstructed{Iter{input}};
                [[maybe_unused]] Cit sentinelConstructed(Sen{});
            }

            { // [counted.iter.access]
                Cit iter{Iter{input}};
                assert(*iter == 1);

                const Cit constIter{Iter{input}};
                assert(*constIter == 1);
            }

            { // [counted.iter.nav]
                Cit iter{Iter{input}};
                ++iter;
                assert(*iter == 2);

                assert(*iter++ == 2);
                assert(*iter == 3);
            }

            { // [counted.iter.cmp]
                Cit iter1{Iter{input}};
                Cit iter2{Iter{input}};
                assert(iter1 == iter2);

                Sen sen{};
                assert(iter1 == sen);
            }

            { //[common.iter.cust]
                { // iter_move
                    Cit iter1{Iter{input}};

                    auto iter2 = ranges::iter_move(iter1);
                    assert(*iter1 == 2);
                }
                { // iter_swap
                    Cit iter1{Iter{input}};
                    Cit iter2{Iter{input + 1}};

                    ranges::iter_swap(iter1, iter2);
                    assert(*iter1 == 2);
                    assert(*iter2 == 1);
                }
            }
        }
    }
};

int main() {
    with_writable_iterators<instantiator, const int>::call();
}
