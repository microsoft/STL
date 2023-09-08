// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <iterator>
#include <memory>
#include <ranges>
#include <type_traits>
#include <utility>

#include "range_algorithm_support.hpp"

using namespace std;

template <class T>
concept Pointer = is_pointer_v<T>;

template <class It>
concept HasPeek = requires(const It& iter) {
    { iter.peek() } -> Pointer;
    { iter.peek() } -> convertible_to<const iter_value_t<It>*>;
};

static_assert(!HasPeek<int*>);
static_assert(!HasPeek<reverse_iterator<int*>>);
static_assert(HasPeek<test::iterator<forward_iterator_tag, int>>);

template <input_iterator It>
constexpr void test_one(It iter) {
    using ConstIt = basic_const_iterator<It>;
    using Se      = test::sentinel<const iter_value_t<ConstIt>>;

    // Validate iterator concepts
    static_assert(input_iterator<ConstIt>);
    static_assert(forward_iterator<ConstIt> == forward_iterator<It>);
    static_assert(bidirectional_iterator<ConstIt> == bidirectional_iterator<It>);
    static_assert(random_access_iterator<ConstIt> == random_access_iterator<It>);
    static_assert(contiguous_iterator<ConstIt> == contiguous_iterator<It>);

    // Validate nested types
    static_assert(same_as<typename ConstIt::value_type, iter_value_t<It>>);
    static_assert(same_as<typename ConstIt::difference_type, iter_difference_t<It>>);
    if constexpr (forward_iterator<It>) {
        using Cat = typename iterator_traits<It>::iterator_category;
        static_assert(same_as<typename ConstIt::iterator_category, Cat>);
    }

    // Validate default-initializability
    static_assert(default_initializable<ConstIt> == default_initializable<It>);

    same_as<ConstIt> auto citer = basic_const_iterator{iter};
    if (!forward_iterator<It>) { // intentionally not if constexpr
        return;
    }

    { // Validate basic_const_iterator::operator*()
        same_as<iter_const_reference_t<It>> decltype(auto) val = *citer;
        assert(val == *iter);
        static_assert(noexcept(*citer) == noexcept(static_cast<iter_const_reference_t<It>>(*iter))); // strengthened
    }

    // Validate basic_const_iterator::operator->()
    if constexpr (is_lvalue_reference_v<iter_reference_t<It>>
                  && same_as<remove_cvref_t<iter_reference_t<It>>, iter_value_t<It>>) {
        const same_as<const iter_value_t<It>*> auto ptr = citer.operator->();
        assert(ptr == addressof(*citer));

        if constexpr (contiguous_iterator<It>) {
            static_assert(noexcept(citer.operator->()) == noexcept(to_address(iter))); // strengthened
        } else {
            static_assert(noexcept(citer.operator->()) == noexcept(addressof(*iter))); // strengthened
        }
    }

    { // Validate basic_const_iterator::operator++()
        same_as<ConstIt&> decltype(auto) citer2 = ++citer;
        ++iter;
        assert(citer2 == iter);
        assert(*citer2 == *iter);
        static_assert(noexcept(++citer) == noexcept(++iter)); // strengthened
    }

    // Validate basic_const_iterator::operator++(int)
    if constexpr (forward_iterator<It>) {
        same_as<ConstIt> decltype(auto) citer2 = citer++;
        assert(citer2 == iter);
        assert(*citer2 == *iter);
        static_assert(
            noexcept(citer++) == (noexcept(iter++) && is_nothrow_copy_constructible_v<ConstIt>) ); // strengthened
    } else {
        static_assert(is_void_v<decltype(citer++)>);
        static_assert(noexcept(citer++) == noexcept(iter++)); // strengthened
    }

    iter++;
    assert(citer == iter);
    assert(*citer == *iter);

    if constexpr (bidirectional_iterator<It>) {
        { // Validate basic_const_iterator::operator--()
            same_as<ConstIt&> decltype(auto) citer2 = --citer;
            --iter;
            assert(citer2 == iter);
            assert(*citer2 == *iter);
            static_assert(noexcept(--citer) == noexcept(--iter)); // strengthened
        }

        { // Validate basic_const_iterator::operator--(int)
            same_as<ConstIt> decltype(auto) citer2 = citer--;
            assert(citer2 == iter);
            assert(*citer2 == *iter);
            static_assert(
                noexcept(citer--) == (noexcept(iter--) && is_nothrow_copy_constructible_v<ConstIt>) ); // strengthened

            iter--;
            assert(citer == iter);
            assert(*citer == *iter);
        }
    }

    if constexpr (random_access_iterator<It>) {
        { // Validate basic_const_iterator::operator+=()
            same_as<ConstIt&> decltype(auto) citer2 = (citer += 2);
            iter += 2;
            assert(citer2 == iter);
            assert(*citer2 == *iter);
            static_assert(noexcept(citer += 2) == noexcept(iter += 2)); // strengthened
        }

        { // Validate basic_const_iterator::operator-=()
            same_as<ConstIt&> decltype(auto) citer2 = (citer -= 2);
            iter -= 2;
            assert(citer2 == iter);
            assert(*citer2 == *iter);
            static_assert(noexcept(citer -= 2) == noexcept(iter -= 2)); // strengthened
        }

        { // Validate basic_const_iterator::operator[]
            same_as<iter_const_reference_t<It>> decltype(auto) val = citer[0];
            assert(val == iter[0]);
            static_assert(
                noexcept(citer[0]) == noexcept(static_cast<iter_const_reference_t<It>>(iter[0]))); // strengthened
        }

        { // Validate operator{<, >, <=, >=, <=>}(const basic_const_iterator&, const basic_const_iterator&)
            const same_as<ConstIt> auto citer2 = (citer + 1);
            assert(citer < citer2);
            assert(citer2 > citer);
            assert(citer <= citer2);
            assert(citer2 >= citer);
            assert(citer2 <=> citer == partial_ordering::greater);

            static_assert(noexcept(citer < citer2) == noexcept(citer.base() < citer2.base())); // strengthened
            static_assert(noexcept(citer2 > citer) == noexcept(citer2.base() > citer.base())); // strengthened
            static_assert(noexcept(citer <= citer2) == noexcept(citer.base() <= citer2.base())); // strengthened
            static_assert(noexcept(citer2 >= citer) == noexcept(citer2.base() >= citer.base())); // strengthened
            static_assert(noexcept(citer2 <=> citer) == noexcept(citer2.base() <=> citer.base())); // strengthened
        }

        iter += 2; // advance iter temporarily

        { // Validate operator{<, >, <=, >=, <=>}(const basic_const_iterator&, const "not same as
          // basic_const_iterator"&)
            assert(citer < iter);
            assert(!(citer > iter));
            assert(citer <= iter);
            assert(!(citer >= iter));
            assert(citer <=> iter == partial_ordering::less);

            static_assert(noexcept(citer < iter) == noexcept(citer.base() < iter)); // strengthened
            static_assert(noexcept(!(citer > iter)) == noexcept(!(citer.base() > iter))); // strengthened
            static_assert(noexcept(citer <= iter) == noexcept(citer.base() <= iter)); // strengthened
            static_assert(noexcept(!(citer >= iter)) == noexcept(!(citer.base() >= iter))); // strengthened
            static_assert(noexcept(citer <=> iter) == noexcept(citer.base() <=> iter)); // strengthened
        }

        { // Validate operator{<, >, <=, >=}(const "not a const iterator"&, const basic_const_iterator&)
            assert(!(iter < citer));
            assert(iter > citer);
            assert(!(iter <= citer));
            assert(iter >= citer);

            static_assert(noexcept(!(iter < citer)) == noexcept(!(iter < citer.base()))); // strengthened
            static_assert(noexcept(iter > citer) == noexcept(iter > citer.base())); // strengthened
            static_assert(noexcept(!(iter <= citer)) == noexcept(!(iter <= citer.base()))); // strengthened
            static_assert(noexcept(iter >= citer) == noexcept(iter >= citer.base())); // strengthened
        }

        { // Validate operator+(const basic_const_iterator&, difference_type)
            const same_as<ConstIt> auto citer2 = (citer + 2);
            const same_as<ConstIt> auto citer3 = (2 + citer);
            assert(citer2 == citer3);
            assert(*citer2 == *iter);

            static_assert(noexcept(citer + 2) == noexcept(iter + 2)); // strengthened
            static_assert(noexcept(2 + citer) == noexcept(2 + iter)); // strengthened
        }

        { // Validate operator-(const basic_const_iterator&, difference_type)
            citer += 4;
            const same_as<ConstIt> auto citer2 = (citer - 2);
            assert(*citer2 == *iter);
            citer -= 4;

            static_assert(noexcept(citer - 2) == noexcept(iter - 2)); // strengthened
        }

        { // Validate operator-(const basic_const_iterator&, sized_sentinel)
            assert(citer - citer == 0);
            assert(citer - iter == -2);
            assert(iter - citer == 2);
            static_assert(noexcept(citer - citer) == noexcept(iter - iter)); // strengthened
            static_assert(noexcept(citer - iter) == noexcept(iter - iter)); // strengthened
            static_assert(noexcept(iter - citer) == noexcept(iter - iter)); // strengthened

            if constexpr (HasPeek<It> && sized_sentinel_for<Se, It>) {
                Se sent{iter.peek()};
                assert(citer - sent == -2);
                assert(sent - citer == 2);
                static_assert(noexcept(citer - sent) == noexcept(iter - sent)); // strengthened
                static_assert(noexcept(sent - citer) == noexcept(sent - iter)); // strengthened
            }
        }

        iter -= 2;
    }

    // Validate to_address
    if constexpr (contiguous_iterator<It> && HasPeek<It>) {
        assert(to_address(citer) == iter.peek());
    }

    // Validate basic_const_iterator::operator==()
    assert(citer == iter);
    assert(iter == citer);
    static_assert(noexcept(citer == iter) == noexcept(iter == iter));
    if constexpr (HasPeek<It> && sentinel_for<Se, It>) {
        Se sent{iter.peek()};
        assert(citer == sent);
        static_assert(noexcept(citer == sent) == noexcept(iter == sent)); // strengthened
    }

    { // Validate basic_const_iterator::iter_move()
        using Expected = common_reference_t<const iter_value_t<It>&&, iter_rvalue_reference_t<It>>;
        [[maybe_unused]] same_as<Expected> decltype(auto) val = ranges::iter_move(citer);
        static_assert(
            noexcept(ranges::iter_move(citer)) == noexcept(static_cast<Expected>(ranges::iter_move(citer.base()))));
    }

    { // Validate basic_const_iterator::base() const&
        [[maybe_unused]] same_as<const It&> decltype(auto) base = citer.base();
        static_assert(noexcept(citer.base()));
    }

    { // Validate basic_const_iterator::base() &&
        [[maybe_unused]] same_as<It> decltype(auto) base = move(citer).base();
        static_assert(noexcept(move(citer).base()) == is_nothrow_move_constructible_v<It>); // strengthened
    }
}

void test_lwg3853() { // COMPILE-ONLY
    basic_const_iterator<volatile int*> it;
    [[maybe_unused]] same_as<const volatile int*> auto ptr = it.operator->();
}

static constexpr int some_ints[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// Check LWG-3872
using Zipped = decltype(views::zip(some_ints) | views::as_const | views::as_rvalue);
static_assert(same_as<ranges::range_reference_t<Zipped>, tuple<const int&&>>);

struct instantiator {
    template <input_iterator It>
    static constexpr void call() {
        It iter{some_ints};
        test_one(iter);
    }
};

template <class Category, test::CanDifference CanDiff>
using test_iterator = test::iterator<Category, const int, CanDiff, test::CanCompare::yes,
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}>;

constexpr void instantiation_test() {
    using test::CanDifference;

    // The iterator is sensitive to category and differencing, but oblivious to proxyness
    instantiator::call<test_iterator<input_iterator_tag, CanDifference::no>>();
    instantiator::call<test_iterator<forward_iterator_tag, CanDifference::no>>();
    instantiator::call<test_iterator<bidirectional_iterator_tag, CanDifference::no>>();
    instantiator::call<test_iterator<random_access_iterator_tag, CanDifference::no>>();
    instantiator::call<test_iterator<contiguous_iterator_tag, CanDifference::no>>();

    instantiator::call<test_iterator<input_iterator_tag, CanDifference::yes>>();
    instantiator::call<test_iterator<forward_iterator_tag, CanDifference::yes>>();
    instantiator::call<test_iterator<bidirectional_iterator_tag, CanDifference::yes>>();
    instantiator::call<test_iterator<random_access_iterator_tag, CanDifference::yes>>();
    instantiator::call<test_iterator<contiguous_iterator_tag, CanDifference::yes>>();
}

int main() {
    static_assert((instantiation_test(), true));
    instantiation_test();
}
