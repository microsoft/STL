// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <concepts>
#include <iterator>

#include <range_algorithm_support.hpp>
using namespace std;

struct instantiator {

    template <input_or_output_iterator Iter>
    static constexpr void call() {
        int input[5] = {1, 2, 3, 4, 5};
        // [counted.iter.const]
        {
            [[maybe_unused]] counted_iterator<Iter> defaultConstructed{};

            [[maybe_unused]] counted_iterator<Iter> constructed(Iter{input}, iter_difference_t<Iter>{2});
            [[maybe_unused]] counted_iterator<Iter> constructedEmpty{Iter{input}, iter_difference_t<Iter>{0}};

            //[[maybe_unused]] counted_iterator<Iter> convertibleConstructed{};
            //[[maybe_unused]] counted_iterator<Iter> convertibleAssigned{};
        }

        // [counted.iter.access]
        {
            if constexpr (copy_constructible<Iter>) {
                const counted_iterator<Iter> lvalueBase{Iter{input}, 2};
                const same_as<Iter> auto base1 = lvalueBase.base();
                assert(base1.peek() == Iter{input}.peek());
            }

            const same_as<Iter> auto base2 = counted_iterator<Iter>{Iter{input}, 2}.base();
            assert(base2.peek() == Iter{input}.peek());

            const same_as<iter_difference_t<Iter>> auto length = counted_iterator<Iter>{Iter{input}, 2}.count();
            assert(length == 2);
        }

        // [counted.iter.elem]
        {
            auto iter = counted_iterator<Iter>{Iter{input}, 2};
            assert(*iter == 1);

            const auto constIter = counted_iterator<Iter>{Iter{input}, 2};
            assert(*constIter == 1);

            if constexpr (random_access_iterator<Iter>) {
                assert(iter[1] == 2);
            }
        }
        // [counted.iter.nav]
        {
            { // pre increment
                auto iter = counted_iterator<Iter>{Iter{input}, 2};
                ++iter;
                assert(iter.count() == 1);
                assert(*iter == 2);
            }
            if constexpr (forward_iterator<Iter>) { // post increment
                auto iter = counted_iterator<Iter>{Iter{input}, 2};
                iter++;
                assert(iter.count() == 1);
                assert(*iter == 2);
            }
            if constexpr (bidirectional_iterator<Iter>) {
                {
                    // pre decrement
                    auto iter = counted_iterator<Iter>{Iter{input + 2}, 3};
                    --iter;
                    assert(iter.count() == 4);
                    assert(*iter == 2);
                }
                { // post decrement
                    auto iter = counted_iterator<Iter>{Iter{begin(input) + 2}, 3};
                    iter--;
                    assert(iter.count() == 4);
                    assert(*iter == 2);
                }
            }
            if constexpr (random_access_iterator<Iter>) {
                { // increment by n
                    auto iter = counted_iterator<Iter>{Iter{input}, 5};
                    auto ref  = iter + 3;
                    assert(iter.count() == 5);
                    assert(*iter == 1);
                    assert(iter.count() == ref.count() + 3);
                    assert(iter.base().peek() == prev(ref.base().peek(), 3));
                }
                { // post increment by n
                    auto iter = counted_iterator<Iter>{Iter{input}, 5};
                    auto ref  = 3 + iter;
                    assert(iter.count() == 5);
                    assert(*iter == 1);
                    assert(iter.count() == ref.count() + 3);
                    assert(iter.base().peek() == prev(ref.base().peek(), 3));
                }
                { // increment assign
                    auto iter = counted_iterator<Iter>{Iter{input}, 5};
                    auto ref  = iter;
                    iter += 3;
                    assert(iter.count() == 2);
                    assert(*iter == 4);
                    assert(iter.count() == ref.count() - 3);
                    assert(iter.base().peek() == next(ref.base().peek(), 3));
                }
                { // decrement by n
                    auto iter = counted_iterator<Iter>{Iter{begin(input) + 2}, 3};
                    auto ref  = iter - 2;
                    assert(iter.count() == 3);
                    assert(*iter == 3);
                    assert(iter.count() == ref.count() - 2);
                    assert(iter.base().peek() == next(ref.base().peek(), 2));
                }
            }
            { // difference
                auto iter1 = counted_iterator<Iter>{Iter{input}, 2};
                auto iter2 = counted_iterator<Iter>{Iter{input}, 3};

                const same_as<iter_difference_t<Iter>> auto diff1 = iter1 - iter2;
                assert(diff1 == 1);

                const same_as<iter_difference_t<Iter>> auto diff2 = iter2 - iter1;
                assert(diff2 == -1);
            }
            // TODO different type that satisfies common_with
            { // difference default sentinel
                auto iter1 = counted_iterator<Iter>{Iter{input}, 2};

                const same_as<iter_difference_t<Iter>> auto diff1 = iter1 - default_sentinel;
                assert(diff1 == -2);

                const same_as<iter_difference_t<Iter>> auto diff2 = default_sentinel - iter1;
                assert(diff2 == 2);
            }
            if constexpr (random_access_iterator<Iter>) { // decrement assign
                auto iter = counted_iterator<Iter>{Iter{end(input)}, 0};
                auto ref  = iter;
                iter -= 3;
                assert(iter.count() == 3);
                assert(*iter == 3);
                assert(iter.count() == ref.count() + 3);
                assert(iter.base().peek() == prev(ref.base().peek(), 3));
            }
        }
        // [counted.iter.cmp]
        {
            { // equality
                auto iter1 = counted_iterator<Iter>{Iter{input}, 2};
                auto iter2 = counted_iterator<Iter>{Iter{input}, 2};
                assert(iter1 == iter2);
            }
            { // inequality
                auto iter1 = counted_iterator<Iter>{Iter{input + 1}, 2};
                auto iter2 = counted_iterator<Iter>{Iter{input}, 3};
                assert(iter1 != iter2);
            }
            { // equality default sentinel
                auto iter1 = counted_iterator<Iter>{Iter{input}, 2};
                auto iter2 = counted_iterator<Iter>{Iter{input}, 0};
                assert(iter1 != default_sentinel);
                assert(iter2 == default_sentinel);
            }
            { // spaceship
                auto iter1                                 = counted_iterator<Iter>{Iter{input}, 2};
                auto iter2                                 = counted_iterator<Iter>{Iter{input}, 0};
                const same_as<strong_ordering> auto result = iter1 <=> iter2;
                assert(result == strong_ordering::less);
                assert(iter2 <=> iter1 == strong_ordering::greater);
                assert(iter1 <=> iter1 == strong_ordering::equal);
                assert(iter1 <=> iter1 == strong_ordering::equivalent);
            }
        }
    }
};

int main() {
    STATIC_ASSERT((with_writable_iterators<instantiator, int>::call(), true));
    with_writable_iterators<instantiator, int>::call();
}
