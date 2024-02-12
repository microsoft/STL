// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <ranges>
#include <type_traits>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;

constexpr auto is_even = [](const auto& x) { return x % 2 == 0; };
using Pred             = remove_const_t<decltype(is_even)>;

struct iterator_instantiator {
    template <input_iterator Iter>
    static constexpr void call() {
        // Pre: Iter is a specialization of test::iterator whose element type is const int
        int mutable_ints[]   = {0, 1, 2, 3, 4, 5, 6, 7};
        const auto make_view = [&] {
            return views::filter(
                ranges::subrange{Iter{mutable_ints}, test::sentinel<int>{ranges::end(mutable_ints)}}, is_even);
        };
        using R = decltype(make_view());
        using I = ranges::iterator_t<R>;
        using S = ranges::sentinel_t<R>;

        // Validate nested types
        static_assert(is_same_v<typename I::iterator_concept,
            conditional_t<bidirectional_iterator<Iter>, bidirectional_iterator_tag,
                conditional_t<forward_iterator<Iter>, forward_iterator_tag, input_iterator_tag>>>);

        static_assert(_Has_member_iterator_category<I> == forward_iterator<Iter>);
        if constexpr (forward_iterator<Iter>) {
            using C = iterator_traits<Iter>::iterator_category;
            static_assert(is_same_v<typename I::iterator_category,
                conditional_t<derived_from<C, bidirectional_iterator_tag>, bidirectional_iterator_tag,
                    conditional_t<derived_from<C, forward_iterator_tag>, forward_iterator_tag, input_iterator_tag>>>);
        }

        { // Validate iterator special member functions and base
            static_assert(default_initializable<I> == default_initializable<Iter>);
            if constexpr (default_initializable<Iter>) {
                I defaultConstructed{};
                assert(move(defaultConstructed).base().peek() == nullptr);
                static_assert(is_nothrow_default_constructible_v<I>);
            }

            auto r0 = make_view();
            I valueConstructed{r0, Iter{mutable_ints}};
            static_assert(is_nothrow_constructible_v<I, R&, Iter>);

            if constexpr (copyable<Iter>) {
                I copyConstructed{valueConstructed};
                assert(copyConstructed == valueConstructed);
                static_assert(is_nothrow_copy_constructible_v<I>);

                auto r1 = make_view();
                I copyAssigned{r1, Iter{mutable_ints + 8}};
                copyAssigned = copyConstructed;
                assert(copyAssigned == valueConstructed);
                static_assert(is_nothrow_copy_assignable_v<I>);
                static_assert(same_as<const Iter&, decltype(as_const(copyConstructed).base())>);
            }
            assert(as_const(valueConstructed).base().peek() == mutable_ints);
            assert(move(valueConstructed).base().peek() == mutable_ints);
            static_assert(same_as<Iter, decltype(move(valueConstructed).base())>);
        }

        { // Validate sentinel constructors and base
            S defaultConstructed{};
            assert(defaultConstructed.base().peek() == nullptr);
            static_assert(is_nothrow_default_constructible_v<S>);

            auto r0 = make_view();
            S valueConstructed{r0};
            assert(valueConstructed.base().peek() == end(mutable_ints));

            S copyConstructed{valueConstructed};
            assert(copyConstructed.base().peek() == valueConstructed.base().peek());
            static_assert(is_nothrow_copy_constructible_v<S>);

            defaultConstructed = copyConstructed;
            assert(defaultConstructed.base().peek() == valueConstructed.base().peek());
            static_assert(is_nothrow_copy_assignable_v<S>);
        }

        { // Validate dereference ops
            auto r0 = make_view();
            auto i0 = r0.begin();
            assert(*i0 == 0);
            static_assert(noexcept(*i0));

            assert(ranges::iter_move(i0) == 0); // NB: moving from int leaves it unchanged
            static_assert(noexcept(ranges::iter_move(i0)) == noexcept(ranges::iter_move(declval<Iter>())));

            if constexpr (forward_iterator<Iter>) {
                auto i1 = ranges::next(i0);
                ranges::iter_swap(i0, i1);
                assert(mutable_ints[0] == 2);
                assert(mutable_ints[2] == 0);
                ranges::iter_swap(i1, i0);
                assert(mutable_ints[0] == 0);
                assert(mutable_ints[2] == 2);
                static_assert(noexcept(ranges::iter_swap(i0, i1)));
            }
        }

        { // Validate increments
            auto r0 = make_view();
            auto i0 = r0.begin();
            assert(&++i0 == &i0);
            assert(move(i0).base().peek() == mutable_ints + 2);

            auto r1 = make_view();
            auto i1 = r1.begin();
            if constexpr (forward_iterator<Iter>) {
                assert(i1++ == r1.begin());
            } else {
                i1++;
            }
            assert(move(i1).base().peek() == mutable_ints + 2);
        }

        if constexpr (bidirectional_iterator<Iter>) { // Validate decrements
            auto r            = make_view();
            const auto second = ranges::next(r.begin());
            auto i            = second;
            assert(&--i == &i);
            assert(i.base().peek() == mutable_ints);

            i = second;
            assert(i-- == second);
            assert(i.base().peek() == mutable_ints);
        }

        if constexpr (equality_comparable<Iter>) {
            // Validate == and !=
            auto r           = make_view();
            const auto first = r.begin();
            const auto last  = r.end();

            assert(first == first);
            assert(I{} == I{});

            assert(!(first == last));
            assert(!(last == first));

            assert(!(first != first));
            assert(!(I{} != I{}));

            if constexpr (forward_iterator<Iter>) {
                const auto final = ranges::next(first, last);
                assert(!(first == final));
                assert(first != final);

                assert(last == final);
                assert(final == last);

                assert(!(last != final));
                assert(!(final != last));
            }
        }
    }
};

template <class Category, test::CanDifference Diff>
using test_iterator =
    test::iterator<Category, int, Diff, test::CanCompare{derived_from<Category, forward_iterator_tag>},
        test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}>;

constexpr void iterator_instantiation_test() {
    using test::CanDifference;

    iterator_instantiator::call<test_iterator<input_iterator_tag, CanDifference::no>>();

    iterator_instantiator::call<test_iterator<forward_iterator_tag, CanDifference::no>>();
    iterator_instantiator::call<test_iterator<forward_iterator_tag, CanDifference::yes>>();

    iterator_instantiator::call<test_iterator<bidirectional_iterator_tag, CanDifference::no>>();
    iterator_instantiator::call<test_iterator<bidirectional_iterator_tag, CanDifference::yes>>();

    iterator_instantiator::call<test_iterator<random_access_iterator_tag, CanDifference::yes>>();
    iterator_instantiator::call<test_iterator<contiguous_iterator_tag, CanDifference::yes>>();
}

int main() {
    static_assert((iterator_instantiation_test(), true));
    iterator_instantiation_test();
}
