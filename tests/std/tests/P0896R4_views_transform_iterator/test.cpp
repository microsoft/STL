// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <ranges>
#include <type_traits>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;

#if _ITERATOR_DEBUG_LEVEL == 0
#define NOEXCEPT_IDL0(...) noexcept(__VA_ARGS__)
#else
#define NOEXCEPT_IDL0(...) true
#endif // _ITERATOR_DEBUG_LEVEL == 0

constexpr auto add8 = [](const auto& x) noexcept { return x + 8; };

struct iterator_instantiator {
    template <input_iterator Iter>
    static constexpr void call() {
        // Pre: Iter is a specialization of test::iterator whose element type is const int
        int mutable_ints[]   = {0, 1, 2, 3, 4, 5, 6, 7};
        const auto make_view = [&] {
            return views::transform(
                ranges::subrange{Iter{mutable_ints}, test::sentinel<int>{ranges::end(mutable_ints)}}, add8);
        };
        using R = decltype(make_view());
        using I = ranges::iterator_t<R>;
        using S = ranges::sentinel_t<R>;

        // Validate nested types
        static_assert(is_same_v<typename I::iterator_concept,
            conditional_t<random_access_iterator<Iter>, random_access_iterator_tag,
                conditional_t<bidirectional_iterator<Iter>, bidirectional_iterator_tag,
                    conditional_t<forward_iterator<Iter>, forward_iterator_tag, input_iterator_tag>>>>);

        using C = typename iterator_traits<Iter>::iterator_category;
        static_assert(is_same_v<typename I::iterator_category,
            conditional_t<is_lvalue_reference_v<invoke_result_t<decltype((add8)), iter_reference_t<Iter>>>,
                conditional_t<derived_from<C, contiguous_iterator_tag>, random_access_iterator_tag, C>,
                input_iterator_tag>>);

        { // Validate iterator special member functions and base
            I defaultConstructed{};
            assert(std::move(defaultConstructed).base().peek() == nullptr);
            static_assert(is_nothrow_default_constructible_v<I>);

            auto r0 = make_view();
            I valueConstructed{r0, Iter{mutable_ints}};
            static_assert(is_nothrow_constructible_v<I, R&, Iter>);

            if constexpr (copyable<Iter>) {
                I copyConstructed{valueConstructed};
                assert(copyConstructed == valueConstructed);
                static_assert(is_nothrow_copy_constructible_v<I>);

                defaultConstructed = copyConstructed;
                assert(defaultConstructed == valueConstructed);
                static_assert(is_nothrow_copy_assignable_v<I>);
                static_assert(same_as<const Iter&, decltype(as_const(copyConstructed).base())>);
            }
            assert(std::move(valueConstructed).base().peek() == mutable_ints);
            static_assert(same_as<Iter, decltype(move(valueConstructed).base())>);

            if constexpr (forward_iterator<Iter>) {
                auto r1      = make_view();
                const auto i = r1.begin();
                using CI     = ranges::iterator_t<const R>;
                CI conversionConstructed{i};
                assert(conversionConstructed.base().peek() == mutable_ints);
                static_assert(is_nothrow_constructible_v<CI, const I&>);
            }
        }

        { // Validate sentinel constructors and base
            S defaultConstructed{};
            assert(defaultConstructed.base().peek() == nullptr);
            static_assert(is_nothrow_default_constructible_v<S>);

            const test::sentinel<int> s{mutable_ints + 2};
            S valueConstructed{s};
            assert(valueConstructed.base().peek() == s.peek());
            static_assert(is_nothrow_constructible_v<S, const test::sentinel<int>&>);

            S copyConstructed{valueConstructed};
            assert(copyConstructed.base().peek() == valueConstructed.base().peek());
            static_assert(is_nothrow_copy_constructible_v<S>);

            defaultConstructed = copyConstructed;
            assert(defaultConstructed.base().peek() == valueConstructed.base().peek());
            static_assert(is_nothrow_copy_assignable_v<S>);

            if constexpr (forward_iterator<Iter> && indirectly_swappable<Iter>) {
                auto r       = make_view();
                using CS     = ranges::sentinel_t<const R>;
                const S last = r.end();
                CS conversionConstructed{last};
                assert(conversionConstructed.base().peek() == ranges::end(mutable_ints));
                static_assert(is_nothrow_constructible_v<CS, const S&>);
            }
        }

        { // Validate dereference ops
            auto r0 = make_view();
            auto i0 = r0.begin();
            assert(*i0 == add8(mutable_ints[0]));
            static_assert(NOEXCEPT_IDL0(*i0));

            assert(ranges::iter_move(i0) == add8(mutable_ints[0])); // NB: moving from int leaves it unchanged
            static_assert(NOEXCEPT_IDL0(ranges::iter_move(i0)));

            if constexpr (forward_iterator<Iter>) {
                auto i1 = ranges::next(i0);
                ranges::iter_swap(i0, i1);
                assert(mutable_ints[0] == 1);
                assert(mutable_ints[1] == 0);
                ranges::iter_swap(i1, i0);
                assert(mutable_ints[0] == 0);
                assert(mutable_ints[1] == 1);
                static_assert(NOEXCEPT_IDL0(ranges::iter_swap(i0, i1)));
            }
        }

        { // Validate increments
            auto r0 = make_view();
            auto i0 = r0.begin();
            assert(&++i0 == &i0);
            assert(std::move(i0).base().peek() == mutable_ints + 1);
            static_assert(noexcept(++i0));

            auto r1 = make_view();
            auto i1 = r1.begin();
            if constexpr (forward_iterator<Iter>) {
                assert(i1++ == r1.begin());
            } else {
                i1++;
            }
            assert(std::move(i1).base().peek() == mutable_ints + 1);
            static_assert(noexcept(i0++));
        }

        if constexpr (bidirectional_iterator<Iter>) { // Validate decrements
            auto r            = make_view();
            const auto second = ranges::next(r.begin());
            auto i            = second;
            assert(&--i == &i);
            assert(i.base().peek() == mutable_ints);
            static_assert(noexcept(--i));

            i = second;
            assert(i-- == second);
            assert(i.base().peek() == mutable_ints);
            static_assert(noexcept(i--));
        }

        if constexpr (random_access_iterator<Iter>) { // Validate seek operations and []
            auto r = make_view();
            auto i = r.begin();
            assert((i + 2).base().peek() == mutable_ints + 2);
            assert((I{} + 0) == I{});
            static_assert(NOEXCEPT_IDL0(i + 2));

            assert((2 + i).base().peek() == mutable_ints + 2);
            assert((0 + I{}).base().peek() == nullptr);
            static_assert(NOEXCEPT_IDL0(2 + i));

            auto vi = I{};
            assert(&(i += 5) == &i);
            assert(i.base().peek() == mutable_ints + 5);
            assert(&(vi += 0) == &vi);
            assert(vi.base().peek() == nullptr);
            static_assert(NOEXCEPT_IDL0(i += 5));

            assert((i - 2).base().peek() == mutable_ints + 3);
            assert((I{} - 0).base().peek() == nullptr);
            static_assert(NOEXCEPT_IDL0(i - 2));

            assert(&(i -= 3) == &i);
            assert(i.base().peek() == mutable_ints + 2);
            assert(&(vi -= 0) == &vi);
            assert(vi.base().peek() == nullptr);
            static_assert(NOEXCEPT_IDL0(i -= 3));

            assert(i[4] == add8(mutable_ints[6]));
            static_assert(NOEXCEPT_IDL0(i[4]));
        }

        if constexpr (equality_comparable<Iter>) {
            // Validate == and !=
            auto r           = make_view();
            const auto first = r.begin();
            const auto last  = r.end();

            const auto const_first = ranges::iterator_t<const R>{first};
            const auto const_last  = ranges::sentinel_t<const R>{last};

            assert(first == first);
            assert(I{} == I{});
            static_assert(noexcept(first == first));

            assert(first == const_first);
            static_assert(noexcept(first == const_first));
            assert(const_first == first);
            static_assert(noexcept(const_first == first));

            assert(!(first == last));
            static_assert(noexcept(first == last));
            assert(!(last == first));
            static_assert(noexcept(last == first));

            assert(!(const_first == last));
            static_assert(noexcept(const_first == last));
            assert(!(last == const_first));
            static_assert(noexcept(last == const_first));

            assert(!(first == const_last));
            static_assert(noexcept(first == const_last));
            assert(!(const_last == first));
            static_assert(noexcept(const_last == first));

            assert(!(first != first));
            assert(!(I{} != I{}));
            static_assert(noexcept(first != first));

            if constexpr (forward_iterator<Iter>) {
                const auto final       = ranges::next(first, last);
                const auto const_final = ranges::next(const_first, const_last);
                assert(!(first == final));
                assert(first != final);

                assert(last == final);
                assert(final == last);

                assert(const_last == final);
                assert(final == const_last);

                assert(last == const_final);
                assert(const_final == last);

                assert(const_last == const_final);
                assert(const_final == const_last);

                assert(!(last != final));
                assert(!(final != last));

                if constexpr (sized_sentinel_for<Iter, Iter>) { // Validate difference
                    assert(first - first == 0);
                    assert(final - first == ranges::ssize(mutable_ints));
                    assert(first - final == -ranges::ssize(mutable_ints));
                    assert(I{} - I{} == 0);
                    static_assert(noexcept(first - first));
                }

                if constexpr (sized_sentinel_for<test::sentinel<int>, Iter>) {
                    assert(last - first == ranges::ssize(mutable_ints));
                    assert(first - last == -ranges::ssize(mutable_ints));
                    static_assert(noexcept(last - first));
                    static_assert(noexcept(first - last));

                    assert(last - const_first == ranges::ssize(mutable_ints));
                    assert(const_first - last == -ranges::ssize(mutable_ints));
                    static_assert(noexcept(last - const_first));
                    static_assert(noexcept(const_first - last));

                    assert(const_last - first == ranges::ssize(mutable_ints));
                    assert(first - const_last == -ranges::ssize(mutable_ints));
                    static_assert(noexcept(const_last - first));
                    static_assert(noexcept(first - const_last));

                    assert(const_last - const_first == ranges::ssize(mutable_ints));
                    assert(const_first - const_last == -ranges::ssize(mutable_ints));
                    static_assert(noexcept(const_last - const_first));
                    static_assert(noexcept(const_first - const_last));
                }

                if constexpr (random_access_iterator<Iter>) { // Validate relational operators
                    assert(!(first < first));
                    assert(first < final);
                    assert(!(final < first));
                    assert(!(I{} < I{}));
                    static_assert(noexcept(first < final));

                    assert(!(first > first));
                    assert(!(first > final));
                    assert(final > first);
                    assert(!(I{} > I{}));
                    static_assert(noexcept(first > final));

                    assert(first <= first);
                    assert(first <= final);
                    assert(!(final <= first));
                    assert(I{} <= I{});
                    static_assert(noexcept(first <= final));

                    assert(first >= first);
                    assert(!(first >= final));
                    assert(final >= first);
                    assert(I{} >= I{});
                    static_assert(noexcept(first >= final));

                    if constexpr (three_way_comparable<Iter>) { // Validate spaceship
                        assert((first <=> first) == strong_ordering::equal);
                        assert((first <=> final) == strong_ordering::less);
                        assert((final <=> first) == strong_ordering::greater);
                        assert((I{} <=> I{}) == strong_ordering::equal);
                        static_assert(noexcept(first <=> final));
                    }
                }
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
