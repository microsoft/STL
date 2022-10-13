// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <concepts>
#include <iterator>
#include <list>
#include <memory>
#include <type_traits>

#include <range_algorithm_support.hpp>
using namespace std;

template <class>
inline constexpr void* must_be_countable = nullptr;

template <input_or_output_iterator I>
    requires requires { typename counted_iterator<I>; }
inline constexpr bool must_be_countable<I> = true;

template <class... Is>
concept Counted = (must_be_countable<Is> && ...);

// clang-format off
template <class I1, class I2>
concept CountedCompare = Counted<I1, I2>
    && requires(const counted_iterator<I1>& c1, const counted_iterator<I2>& c2) {
        c1 == c2;
        c1 != c2;
        c1 < c2;
        c1 > c2;
        c1 <= c2;
        c1 >= c2;
        c1 <=> c2;
        c1 - c2;

        c2 == c1;
        c2 != c1;
        c2 < c1;
        c2 > c1;
        c2 <= c1;
        c2 >= c1;
        c2 <=> c1;
        c2 - c1;
    };
// clang-format on

struct instantiator {
    template <input_or_output_iterator Iter>
    static constexpr void call() {
        using ConstIter = typename Iter::Consterator;

        int input[5] = {1, 2, 3, 4, 5};
        // [counted.iter.const]
        {
            STATIC_ASSERT(default_initializable<counted_iterator<Iter>> == default_initializable<Iter>);
            if constexpr (default_initializable<Iter>) {
                [[maybe_unused]] counted_iterator<Iter> defaultConstructed{};
            }

            counted_iterator<Iter> constructed(Iter{input}, iter_difference_t<Iter>{2});
            counted_iterator<Iter> constructedEmpty{Iter{input}, iter_difference_t<Iter>{0}};

            if constexpr (copyable<Iter>) { // counted_iterator only has const lvalue conversions
                counted_iterator<ConstIter> constructedConversion{constructed};
                constructedConversion = constructedEmpty;
            }
        }

        // [counted.iter.access]
        {
            if constexpr (copy_constructible<Iter>) {
                const counted_iterator<Iter> lvalueBase{Iter{input}, 2};
                const same_as<Iter> auto base1 = lvalueBase.base();
                assert(base1.peek() == input);
            }

            const same_as<Iter> auto base2 = counted_iterator<Iter>{Iter{input}, 2}.base();
            assert(base2.peek() == input);

            const same_as<iter_difference_t<Iter>> auto length = counted_iterator<Iter>{Iter{input}, 2}.count();
            assert(length == 2);
        }

        // [counted.iter.elem]
        {
            counted_iterator<Iter> iter{Iter{input}, 2};
            assert(*iter == 1);
            if constexpr (is_reference_v<iter_reference_t<Iter>>) {
                assert(addressof(*iter) == input);
            }

            const counted_iterator<Iter> constIter{Iter{input}, 2};
            assert(*constIter == 1);
            if constexpr (is_reference_v<iter_reference_t<Iter>>) {
                assert(addressof(*constIter) == input);
            }

            if constexpr (random_access_iterator<Iter>) {
                assert(iter[1] == 2);
                if constexpr (is_reference_v<iter_reference_t<Iter>>) {
                    assert(addressof(iter[1]) == input + 1);
                }
            }
        }
        // [counted.iter.nav]
        {
            { // pre increment
                counted_iterator<Iter> iter{Iter{input}, 2};
                assert(addressof(++iter) == addressof(iter));
                assert(iter.count() == 1);
                assert(*iter == 2);
                if constexpr (is_reference_v<iter_reference_t<Iter>>) {
                    assert(addressof(*iter) == input + 1);
                }
            }

            if constexpr (forward_iterator<Iter>) { // post increment
                counted_iterator<Iter> iter{Iter{input}, 2};
                auto ref_iter = iter;
                auto iter2    = iter++;
                assert(iter2 == ref_iter);
                assert(iter.count() == 1);
                assert(*iter == 2);
                if constexpr (is_reference_v<iter_reference_t<Iter>>) {
                    assert(addressof(*iter) == input + 1);
                }
            }

            if constexpr (bidirectional_iterator<Iter>) {
                {
                    // pre decrement
                    counted_iterator<Iter> iter{Iter{input + 2}, 3};
                    assert(addressof(--iter) == addressof(iter));
                    assert(iter.count() == 4);
                    assert(*iter == 2);
                    if constexpr (is_reference_v<iter_reference_t<Iter>>) {
                        assert(addressof(*iter) == input + 1);
                    }
                }
                { // post decrement
                    counted_iterator<Iter> iter{Iter{begin(input) + 2}, 3};
                    auto ref_iter = iter;
                    auto iter2    = iter--;
                    assert(iter2 == ref_iter);
                    assert(iter.count() == 4);
                    assert(*iter == 2);
                    if constexpr (is_reference_v<iter_reference_t<Iter>>) {
                        assert(addressof(*iter) == input + 1);
                    }
                }
            }

            if constexpr (random_access_iterator<Iter>) {
                { // increment by n lhs
                    counted_iterator<Iter> iter{Iter{input}, 5};
                    auto ref_iter = iter + 3;
                    assert(iter.count() == 5);
                    assert(*iter == 1);
                    assert(iter.count() == ref_iter.count() + 3);
                    assert(iter.base().peek() == prev(ref_iter.base().peek(), 3));
                }
                { // increment by n rhs
                    counted_iterator<Iter> iter{Iter{input}, 5};
                    auto ref_iter = 3 + iter;
                    assert(iter.count() == 5);
                    assert(*iter == 1);
                    assert(iter.count() == ref_iter.count() + 3);
                    assert(iter.base().peek() == prev(ref_iter.base().peek(), 3));
                }
                { // increment value-initialized by 0
                    counted_iterator<Iter> iter;
                    auto ref_iter = 0 + iter;
                    assert(iter == ref_iter);
                }
                { // increment assign
                    counted_iterator<Iter> iter{Iter{input}, 5};
                    auto ref_iter = iter;
                    assert(addressof(iter += 3) == addressof(iter));
                    assert(iter.count() == 2);
                    assert(*iter == 4);
                    assert(iter.count() == ref_iter.count() - 3);
                    assert(iter.base().peek() == next(ref_iter.base().peek(), 3));
                }
                { // decrement by n
                    counted_iterator<Iter> iter{Iter{begin(input) + 2}, 3};
                    auto ref_iter = iter - 2;
                    assert(iter.count() == 3);
                    assert(*iter == 3);
                    assert(iter.count() == ref_iter.count() - 2);
                    assert(iter.base().peek() == next(ref_iter.base().peek(), 2));
                }
            }
            { // difference
                counted_iterator<Iter> iter1{Iter{input + 1}, 2};
                counted_iterator<Iter> iter2{Iter{input}, 3};

                const same_as<iter_difference_t<Iter>> auto diff1 = iter1 - iter2;
                assert(diff1 == 1);

                const same_as<iter_difference_t<Iter>> auto diff2 = iter2 - iter1;
                assert(diff2 == -1);
            }
            if constexpr (default_initializable<Iter>) { // difference value-initialized
                const same_as<iter_difference_t<Iter>> auto diff1 = counted_iterator<Iter>{} - counted_iterator<Iter>{};
                assert(diff1 == 0);
            }
            STATIC_ASSERT(CountedCompare<Iter, ConstIter> == common_with<Iter, ConstIter>);
            STATIC_ASSERT(CountedCompare<ConstIter, Iter> == common_with<Iter, ConstIter>);

            if constexpr (common_with<Iter, ConstIter>) { // cross-type difference
                counted_iterator<Iter> iter1{Iter{input + 1}, 2};
                counted_iterator<ConstIter> iter2{ConstIter{input}, 3};

                const same_as<iter_difference_t<Iter>> auto diff1 = iter1 - iter2;
                assert(diff1 == 1);

                const same_as<iter_difference_t<ConstIter>> auto diff2 = iter2 - iter1;
                assert(diff2 == -1);
            }

            { // difference default sentinel
                counted_iterator<Iter> iter1{Iter{input}, 2};

                const same_as<iter_difference_t<Iter>> auto diff1 = iter1 - default_sentinel;
                assert(diff1 == -2);

                const same_as<iter_difference_t<Iter>> auto diff2 = default_sentinel - iter1;
                assert(diff2 == 2);
            }

            if constexpr (random_access_iterator<Iter>) { // decrement assign
                counted_iterator<Iter> iter{Iter{end(input)}, 0};
                auto ref_iter = iter;
                assert(addressof(iter -= 3) == addressof(iter));
                assert(iter.count() == 3);
                assert(*iter == 3);
                assert(iter.count() == ref_iter.count() + 3);
                assert(iter.base().peek() == prev(ref_iter.base().peek(), 3));
            }
        }
        // [counted.iter.cmp]
        {
            { // equality
                counted_iterator<Iter> iter1{Iter{input}, 2};
                counted_iterator<Iter> iter2{Iter{input}, 2};
                counted_iterator<Iter> iter3{Iter{input + 1}, 1};
                assert(iter1 == iter2);
                assert(!(iter1 == iter3));
            }
            { // inequality
                counted_iterator<Iter> iter1{Iter{input + 1}, 2};
                counted_iterator<Iter> iter2{Iter{input}, 3};
                counted_iterator<Iter> iter3{Iter{input}, 3};
                assert(iter1 != iter2);
                assert(!(iter2 != iter3));
            }
            { // equality default sentinel
                counted_iterator<Iter> iter1{Iter{input}, 2};
                counted_iterator<Iter> iter2{Iter{input}, 0};
                assert(iter1 != default_sentinel);
                assert(default_sentinel != iter1);
                assert(iter2 == default_sentinel);
                assert(default_sentinel == iter2);
            }
            { // spaceship
                counted_iterator<Iter> iter1{Iter{input}, 2};
                counted_iterator<Iter> iter2{Iter{input + 2}, 0};
                const same_as<strong_ordering> auto result = iter1 <=> iter2;
                assert(result == strong_ordering::less);
                assert(iter2 <=> iter1 == strong_ordering::greater);
                assert(iter1 <=> iter1 == strong_ordering::equal);
                assert(iter1 <=> iter1 == strong_ordering::equivalent);
            }
            if constexpr (default_initializable<Iter>) { // spaceship value-initialized
                assert(counted_iterator<Iter>{} <=> counted_iterator<Iter>{} == strong_ordering::equal);
                assert(counted_iterator<Iter>{} <=> counted_iterator<Iter>{} == strong_ordering::equivalent);
            }

            if constexpr (common_with<Iter, ConstIter>) {
                { // equality converting
                    counted_iterator<ConstIter> const_iter1{ConstIter{input}, 2};
                    counted_iterator<Iter> iter2{Iter{input}, 2};
                    counted_iterator<Iter> iter3{Iter{input + 1}, 1};
                    assert(const_iter1 == iter2);
                    assert(!(const_iter1 == iter3));
                }
                { // inequality converting
                    counted_iterator<Iter> iter1{Iter{input + 1}, 2};
                    counted_iterator<ConstIter> const_iter1{ConstIter{input}, 3};
                    counted_iterator<Iter> iter3{Iter{input}, 3};
                    assert(iter1 != const_iter1);
                    assert(!(const_iter1 != iter3));
                }
                { // spaceship converting
                    counted_iterator<Iter> iter{Iter{input}, 2};
                    counted_iterator<ConstIter> const_iter{ConstIter{input + 2}, 0};
                    const same_as<strong_ordering> auto result = iter <=> const_iter;
                    assert(result == strong_ordering::less);
                    assert(const_iter <=> iter == strong_ordering::greater);
                    assert(iter <=> iter == strong_ordering::equal);
                    assert(iter <=> iter == strong_ordering::equivalent);
                }
            }
        }
    }
};

// Also test P2259R1 Repairing input range adaptors and counted_iterator
struct simple_forward_iter {
    using value_type        = double;
    using difference_type   = long;
    using iterator_category = input_iterator_tag;
    using iterator_concept  = forward_iterator_tag;

    value_type operator*() const;
    simple_forward_iter& operator++();
    simple_forward_iter operator++(int);

    bool operator==(const simple_forward_iter&) const;
};

using CI = counted_iterator<simple_forward_iter>;

static_assert(same_as<iterator_traits<simple_forward_iter>::iterator_category, input_iterator_tag>);
static_assert(forward_iterator<simple_forward_iter>);
static_assert(forward_iterator<CI>);
static_assert(!contiguous_iterator<CI>);
static_assert(same_as<CI::value_type, double>);
static_assert(same_as<CI::difference_type, long>);
static_assert(same_as<CI::iterator_category, input_iterator_tag>);
static_assert(same_as<CI::iterator_concept, forward_iterator_tag>);

void test_P2259() {
    struct A {
        int m;
    };
    A a[2] = {{1}, {2}};
    counted_iterator ci{a, 2};
    reverse_iterator ri{ci + 1};
    static_assert(contiguous_iterator<decltype(ci)>);
    assert(ci->m == 1);
    assert(ri->m == 1);
}

int main() {
    STATIC_ASSERT(with_writable_iterators<instantiator, int>::call());
    with_writable_iterators<instantiator, int>::call();

    { // Validate unwrapping
        list<int> lst{0, 1, 2};
        counted_iterator ci{lst.begin(), 2};
        same_as<counted_iterator<_Unwrapped_t<list<int>::iterator>>> auto uci = _Get_unwrapped(ci);
        ++uci;
        _Seek_wrapped(ci, uci);
        assert((ci == counted_iterator{ranges::next(lst.begin()), 1}));
    }

    test_P2259();
}
