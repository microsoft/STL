// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <memory>
#include <ranges>
#include <type_traits>
#include <utility>

#include <is_permissive.hpp>

#pragma warning(disable : 4793) // function compiled as native: non-clrcall vcall thunks must be compiled as native

namespace ranges = std::ranges;

template <bool>
struct borrowed { // borrowed<true> is a borrowed_range; borrowed<false> is not
    int* begin() const;
    int* end() const;
};

template <>
inline constexpr bool std::ranges::enable_borrowed_range<borrowed<true>> = true;

template <class T>
struct simple_reference {
    operator T() const;
};

template <class T>
struct simple_common_reference {
    simple_common_reference(simple_reference<T>);
    simple_common_reference(T const&);
};

template <int I, class T = int>
struct simple_iter_archetype {
    using value_type = T;

    simple_reference<T> operator*() const
        requires (I != 0); // 0: not indirectly_readable
    friend void iter_swap(simple_iter_archetype const&, simple_iter_archetype const&) {}
};
static_assert(!std::indirectly_readable<simple_iter_archetype<0>>);
static_assert(std::indirectly_readable<simple_iter_archetype<1>>);

template <class T, template <class> class TQuals, template <class> class UQuals>
struct std::basic_common_reference<T, ::simple_reference<T>, TQuals, UQuals> {
    using type = ::simple_common_reference<T>;
};
template <class T, template <class> class TQuals, template <class> class UQuals>
struct std::basic_common_reference<::simple_reference<T>, T, TQuals, UQuals> {
    using type = ::simple_common_reference<T>;
};

namespace indirectly_unary_invocable_test {
    // Also validate indirect_result_t, and indirectly_regular_unary_invocable which is syntactically equivalent to
    // indirectly_unary_invocable since regular_invocable and invocable are syntactically equivalent.

    template <int I>
    struct base {};
    template <>
    struct base<0> {
        // 0: not copy_constructible
        base()            = default;
        base(base const&) = delete;
    };

    template <int I>
    struct Fn : base<I> {
        // 1: not invocable<Fn&, iter_value_t<simple_iter_archetype>&>
        void operator()(int&) const
            requires (I == 1)
        = delete;
        // 2: not invocable<Fn&, iter_reference_t<simple_iter_archetype>>
        void operator()(simple_reference<int>) const
            requires (I == 2)
        = delete;
        // 3: not invocable<Fn&, iter_common_reference_t<simple_iter_archetype>>
        // This case is made valid by P2997R1
        // "Removing The Common Reference Requirement From The Indirectly Invocable Concepts".
        void operator()(simple_common_reference<int>) const
            requires (I == 3)
        = delete;

        // 4 : not common_reference_with<invoke_result_t<Fn&, iter_value_t<simple_iter_archetype>&>,
        //                               invoke_result_t<Fn&, iter_reference_t<simple_iter_archetype>>>;
        void operator()(int&) const
            requires (I == 4);
        int operator()(simple_reference<int>) const
            requires (I == 4);

        int operator()(int&) const
            requires (I != 1 && I != 4);
        int operator()(simple_reference<int>) const
            requires (I != 2 && I != 4);
        int operator()(simple_common_reference<int>) const
            requires (I != 3 && I != 4);
    };

    template <class F, class I>
    constexpr bool test() {
        constexpr bool result = std::indirectly_unary_invocable<F, I>;
        static_assert(result == std::indirectly_regular_unary_invocable<F, I>);
        return result;
    }
    static_assert(!test<Fn<0>, simple_iter_archetype<1>>());
    static_assert(!test<Fn<1>, simple_iter_archetype<1>>());
    static_assert(!test<Fn<2>, simple_iter_archetype<1>>());
    static_assert(test<Fn<3>, simple_iter_archetype<1>>());
    static_assert(!test<Fn<4>, simple_iter_archetype<1>>());
    static_assert(!test<Fn<5>, simple_iter_archetype<0>>());
    static_assert(test<Fn<5>, simple_iter_archetype<1>>());

    static_assert(std::same_as<std::indirect_result_t<Fn<5>, simple_iter_archetype<1>>, int>);
} // namespace indirectly_unary_invocable_test

namespace indirect_unary_predicate_test {
    // Also validate indirect_result_t
    using std::indirect_unary_predicate;

    template <int I>
    struct base {};
    template <>
    struct base<0> {
        // 0: not copy_constructible
        base()            = default;
        base(base const&) = delete;
    };

    template <int I>
    struct Fn : base<I> {
        // 1: not predicate<Fn&, iter_value_t<simple_iter_archetype>&>
        void operator()(int&) const
            requires (I == 1);
        // 2: not predicate<Fn&, iter_reference_t<simple_iter_archetype>>
        void operator()(simple_reference<int>) const
            requires (I == 2)
        = delete;
        // 3: not predicate<Fn&, iter_common_reference_t<simple_iter_archetype>>
        // This case is made valid by P2997R1
        // "Removing The Common Reference Requirement From The Indirectly Invocable Concepts".
        void operator()(simple_common_reference<int>) const
            requires (I == 3)
        = delete;

        // 4: all of the above
        int operator()(int&) const
            requires (I != 1 && I != 4);
        int operator()(simple_reference<int>) const
            requires (I != 2 && I != 4);
        int operator()(simple_common_reference<int>) const
            requires (I != 3 && I != 4);
    };

    static_assert(!indirect_unary_predicate<Fn<0>, simple_iter_archetype<1>>);
    static_assert(!indirect_unary_predicate<Fn<1>, simple_iter_archetype<1>>);
    static_assert(!indirect_unary_predicate<Fn<2>, simple_iter_archetype<1>>);
    static_assert(indirect_unary_predicate<Fn<3>, simple_iter_archetype<1>>);
    static_assert(!indirect_unary_predicate<Fn<4>, simple_iter_archetype<1>>);
    static_assert(!indirect_unary_predicate<Fn<5>, simple_iter_archetype<0>>);
    static_assert(indirect_unary_predicate<Fn<5>, simple_iter_archetype<1>>);

    static_assert(std::same_as<std::indirect_result_t<Fn<5>, simple_iter_archetype<1>>, int>);
} // namespace indirect_unary_predicate_test

namespace indirect_binary_predicate_test {
    // Also validate indirect_equivalence_relation, indirect_strict_weak_order, and indirect_result_t

    template <int>
    struct base {};
    template <>
    struct base<0> {
        // 0: not copy_constructible
        base()            = default;
        base(base const&) = delete;
    };

    template <int I>
    struct Fn : base<I> {
        // 1: not predicate<Fn&, iter_value_t<simple_iter_archetype>&, iter_value_t<simple_iter_archetype>&>
        void operator()(int&, int&) const
            requires (I == 1);
        // 2: not predicate<Fn&, iter_value_t<simple_iter_archetype>&, iter_reference_t<simple_iter_archetype>>
        void operator()(int&, simple_reference<int>) const
            requires (I == 2);
        // 3: not predicate<Fn&, iter_reference_t<simple_iter_archetype>, iter_value_t<simple_iter_archetype>&>
        void operator()(simple_reference<int>, int&) const
            requires (I == 3);
        // 4: not predicate<Fn&, iter_reference_t<simple_iter_archetype>, iter_reference_t<simple_iter_archetype>>
        void operator()(simple_reference<int>, simple_reference<int>) const
            requires (I == 4);
        // 5: not predicate<Fn&, iter_common_reference_t</**/>, iter_common_reference_t</**/>>
        // This case is made valid by P2997R1
        // "Removing The Common Reference Requirement From The Indirectly Invocable Concepts".
        void operator()(simple_common_reference<int>, simple_common_reference<int>) const
            requires (I == 5);

        bool operator()(int&, int&) const
            requires (I != 1);
        int operator()(int&, simple_reference<int>) const
            requires (I != 2);
        int* operator()(simple_reference<int>, int&) const
            requires (I != 3);
        std::true_type operator()(simple_reference<int>, simple_reference<int>) const
            requires (I != 4);
        std::false_type operator()(simple_common_reference<int>, simple_common_reference<int>) const
            requires (I != 5);
    };

    template <int FuncSelector, int IterSelector1, int IterSelector2>
    constexpr bool test() {
        using std::indirect_binary_predicate, std::indirect_equivalence_relation, std::indirect_strict_weak_order;
        using F  = Fn<FuncSelector>;
        using I1 = simple_iter_archetype<IterSelector1>;
        using I2 = simple_iter_archetype<IterSelector2>;

        constexpr bool result = indirect_binary_predicate<F, I1, I2>;
        static_assert(indirect_equivalence_relation<F, I1, I2> == result);
        static_assert(indirect_strict_weak_order<F, I1, I2> == result);
        return result;
    }

    static_assert(!test<0, 1, 1>());
    static_assert(!test<1, 1, 1>());
    static_assert(!test<2, 1, 1>());
    static_assert(!test<3, 1, 1>());
    static_assert(!test<4, 1, 1>());
    static_assert(test<5, 1, 1>());

    static_assert(!test<6, 0, 1>());
    static_assert(!test<6, 1, 0>());
    static_assert(test<6, 1, 1>());

    static_assert(std::same_as<std::indirect_result_t<Fn<6>, simple_iter_archetype<1>, simple_iter_archetype<1>>,
        std::true_type>);
} // namespace indirect_binary_predicate_test

namespace projected_test {
    template <class Iter, class Proj, class Value, class Reference>
    constexpr bool test() {
        static_assert(std::indirectly_readable<Iter>);

        using P = std::projected<Iter, Proj>;
        static_assert(std::indirectly_readable<P>);
        static_assert(std::same_as<std::iter_value_t<P>, Value>);
        static_assert(std::same_as<std::iter_reference_t<P>, Reference>);
        return true;
    }
    static_assert(test<int*, std::identity, int, int&>());
    static_assert(test<int const*, std::identity, int, int const&>());

    struct S {};
    static_assert(test<S*, int(S::*), int, int&>());
    static_assert(test<S const*, int(S::*), int, int const&>());

    struct iter {
        using value_type = int;
        struct reference {
            operator value_type() const;
        };
        reference operator*() const;
    };
    static_assert(test<iter, std::identity, iter::reference, iter::reference&&>());
    static_assert(test<iter, double (*)(int), double, double>());
} // namespace projected_test

namespace indirectly_movable_test { // also covers indirectly_movable_storable
    using std::assignable_from, std::constructible_from, std::indirectly_writable, std::movable;
    using std::indirectly_movable, std::indirectly_movable_storable;

    template <int I>
    struct value_type {
        value_type()             = default;
        value_type(value_type&&) = default;
        value_type& operator=(value_type&&)
            requires (I != 0)
        = default; // 0: not movable
        // 1: not constructible_from<iter_rvalue_reference_t<In>>:
        template <class T>
        value_type(simple_reference<T>)
            requires (I == 1)
        = delete;
        // 2: not assignable_from<iter_rvalue_reference_t<In>>:
        template <class T>
        value_type& operator=(simple_reference<T>)
            requires (I != 2);
        template <class T>
        void operator=(simple_reference<T>)
            requires (I == 2)
        = delete;
    };
    // Ensure specializations of value_type have the intended properties
    static_assert(!movable<value_type<0>>);
    static_assert(constructible_from<value_type<0>, simple_reference<value_type<0>>>);
    static_assert(assignable_from<value_type<0>&, simple_reference<value_type<0>>>);
    static_assert(movable<value_type<1>>);
    static_assert(!constructible_from<value_type<1>, simple_reference<value_type<1>>>);
    static_assert(assignable_from<value_type<1>&, simple_reference<value_type<1>>>);
    static_assert(movable<value_type<2>>);
    static_assert(constructible_from<value_type<2>, simple_reference<value_type<2>>>);
    static_assert(!assignable_from<value_type<2>&, simple_reference<value_type<2>>>);
    static_assert(movable<value_type<3>>);
    static_assert(constructible_from<value_type<3>, simple_reference<value_type<3>>>);
    static_assert(assignable_from<value_type<3>&, simple_reference<value_type<3>>>);

    template <int I, int J>
    struct out_archetype {
        out_archetype& operator*() const;
        // 0: not indirectly_writable<simple_reference>
        void operator=(simple_reference<value_type<J>>&&) const
            requires (I == 0)
        = delete;
        void operator=(simple_reference<value_type<J>>&&) const
            requires (I != 0);
        // 1: not indirectly_writable<value_type>
        void operator=(value_type<J>&&) const
            requires (I == 1)
        = delete;
        void operator=(value_type<J>&&) const
            requires (I != 1);
    };
    // Ensure specializations of out_archetype have the intended properties
    static_assert(!indirectly_writable<out_archetype<0, 3>, simple_reference<value_type<3>>>);
    static_assert(indirectly_writable<out_archetype<0, 3>, value_type<3>>);
    static_assert(indirectly_writable<out_archetype<1, 3>, simple_reference<value_type<3>>>);
    static_assert(!indirectly_writable<out_archetype<1, 3>, value_type<3>>);
    static_assert(indirectly_writable<out_archetype<2, 3>, simple_reference<value_type<3>>>);
    static_assert(indirectly_writable<out_archetype<2, 3>, value_type<3>>);

    // Validate indirectly_movable
    static_assert(!indirectly_movable<simple_iter_archetype<0, value_type<3>>, out_archetype<1, 3>>);
    static_assert(!indirectly_movable<simple_iter_archetype<1, value_type<3>>, out_archetype<0, 3>>);
    static_assert(indirectly_movable<simple_iter_archetype<1, value_type<3>>, out_archetype<1, 3>>);

    // Validate indirectly_movable_storable
    static_assert(!indirectly_movable_storable<simple_iter_archetype<0, value_type<3>>, out_archetype<2, 3>>);
    static_assert(!indirectly_movable_storable<simple_iter_archetype<1, value_type<3>>, out_archetype<0, 3>>);
    static_assert(!indirectly_movable_storable<simple_iter_archetype<1, value_type<0>>, out_archetype<2, 0>>);
    static_assert(!indirectly_movable_storable<simple_iter_archetype<1, value_type<1>>, out_archetype<2, 1>>);
    static_assert(!indirectly_movable_storable<simple_iter_archetype<1, value_type<2>>, out_archetype<2, 2>>);
    static_assert(indirectly_movable_storable<simple_iter_archetype<1, value_type<3>>, out_archetype<2, 3>>);
} // namespace indirectly_movable_test

namespace indirectly_copyable_test { // also covers indirectly_copyable_storable
    using std::assignable_from, std::constructible_from, std::copyable, std::indirectly_writable;
    using std::indirectly_copyable, std::indirectly_copyable_storable;

    template <int I>
    struct value_type {
        value_type()                  = default;
        value_type(value_type const&) = default;
        value_type& operator=(value_type const&)
            requires (I != 0)
        = default; // 0: not copyable
        // 1: not constructible_from<iter_reference_t<In>>:
        template <class T>
        value_type(simple_reference<T>)
            requires (I == 1)
        = delete;
        // 2: not assignable_from<iter_reference_t<In>>:
        template <class T>
        value_type& operator=(simple_reference<T>)
            requires (I != 2);
        template <class T>
        void operator=(simple_reference<T>)
            requires (I == 2)
        = delete;
    };
    // Ensure specializations of value_type have the intended properties
    static_assert(!copyable<value_type<0>>);
    static_assert(constructible_from<value_type<0>, simple_reference<value_type<0>>>);
    static_assert(assignable_from<value_type<0>&, simple_reference<value_type<0>>>);
    static_assert(copyable<value_type<1>>);
    static_assert(!constructible_from<value_type<1>, simple_reference<value_type<1>>>);
    static_assert(assignable_from<value_type<1>&, simple_reference<value_type<1>>>);
    static_assert(copyable<value_type<2>>);
    static_assert(constructible_from<value_type<2>, simple_reference<value_type<2>>>);
    static_assert(!assignable_from<value_type<2>&, simple_reference<value_type<2>>>);
    static_assert(copyable<value_type<3>>);
    static_assert(constructible_from<value_type<3>, simple_reference<value_type<3>>>);
    static_assert(assignable_from<value_type<3>&, simple_reference<value_type<3>>>);

    template <int I, int J>
    struct out_archetype {
        out_archetype& operator*() const;
        // 0: not indirectly_writable<simple_reference>
        void operator=(simple_reference<value_type<J>>&&) const
            requires (I == 0)
        = delete;
        void operator=(simple_reference<value_type<J>>&&) const
            requires (I != 0);
        // 1: not indirectly_writable<value_type&>
        void operator=(value_type<J>&) const
            requires (I == 1)
        = delete;
        void operator=(value_type<J>&) const
            requires (I != 1);
        // 2: not indirectly_writable<value_type&&>
        void operator=(value_type<J>&&) const
            requires (I == 2)
        = delete;
        void operator=(value_type<J>&&) const
            requires (I != 2);
        // 3: not indirectly_writable<const value_type&&>
        void operator=(value_type<J> const&&) const
            requires (I == 3)
        = delete;
        void operator=(value_type<J> const&&) const
            requires (I != 3);
        // 4: not indirectly_writable<const value_type&>
        void operator=(value_type<J> const&) const
            requires (I == 4)
        = delete;
        void operator=(value_type<J> const&) const
            requires (I != 4);
    };
    // Ensure specializations of out_archetype have the intended properties
    static_assert(!indirectly_writable<out_archetype<0, 3>, simple_reference<value_type<3>>>);
    static_assert(indirectly_writable<out_archetype<0, 3>, value_type<3>&>);
    static_assert(indirectly_writable<out_archetype<0, 3>, value_type<3>&&>);
    static_assert(indirectly_writable<out_archetype<0, 3>, const value_type<3>&&>);
    static_assert(indirectly_writable<out_archetype<0, 3>, const value_type<3>&>);

    static_assert(indirectly_writable<out_archetype<1, 3>, simple_reference<value_type<3>>>);
    static_assert(!indirectly_writable<out_archetype<1, 3>, value_type<3>&>);
    static_assert(indirectly_writable<out_archetype<1, 3>, value_type<3>&&>);
    static_assert(indirectly_writable<out_archetype<1, 3>, const value_type<3>&&>);
    static_assert(indirectly_writable<out_archetype<1, 3>, const value_type<3>&>);

    static_assert(indirectly_writable<out_archetype<2, 3>, simple_reference<value_type<3>>>);
    static_assert(indirectly_writable<out_archetype<2, 3>, value_type<3>&>);
    static_assert(!indirectly_writable<out_archetype<2, 3>, value_type<3>&&>);
    static_assert(indirectly_writable<out_archetype<2, 3>, const value_type<3>&&>);
    static_assert(indirectly_writable<out_archetype<2, 3>, const value_type<3>&>);

    static_assert(indirectly_writable<out_archetype<3, 3>, simple_reference<value_type<3>>>);
    static_assert(indirectly_writable<out_archetype<3, 3>, value_type<3>&>);
    static_assert(indirectly_writable<out_archetype<3, 3>, value_type<3>&&>);
    static_assert(!indirectly_writable<out_archetype<3, 3>, const value_type<3>&&>);
    static_assert(indirectly_writable<out_archetype<3, 3>, const value_type<3>&>);

    static_assert(indirectly_writable<out_archetype<4, 3>, simple_reference<value_type<3>>>);
    static_assert(indirectly_writable<out_archetype<4, 3>, value_type<3>&>);
    static_assert(indirectly_writable<out_archetype<4, 3>, value_type<3>&&>);
    static_assert(indirectly_writable<out_archetype<4, 3>, const value_type<3>&&>);
    static_assert(!indirectly_writable<out_archetype<4, 3>, const value_type<3>&>);

    static_assert(indirectly_writable<out_archetype<5, 3>, simple_reference<value_type<3>>>);
    static_assert(indirectly_writable<out_archetype<5, 3>, value_type<3>&>);
    static_assert(indirectly_writable<out_archetype<5, 3>, value_type<3>&&>);
    static_assert(indirectly_writable<out_archetype<5, 3>, const value_type<3>&&>);
    static_assert(indirectly_writable<out_archetype<5, 3>, const value_type<3>&>);

    // Validate indirectly_copyable
    static_assert(!indirectly_copyable<simple_iter_archetype<0, value_type<3>>, out_archetype<1, 3>>);
    static_assert(!indirectly_copyable<simple_iter_archetype<1, value_type<3>>, out_archetype<0, 3>>);
    static_assert(indirectly_copyable<simple_iter_archetype<1, value_type<3>>, out_archetype<1, 3>>);

    // Validate indirectly_copyable_storable
    static_assert(!indirectly_copyable_storable<simple_iter_archetype<0, value_type<3>>, out_archetype<5, 3>>);
    static_assert(!indirectly_copyable_storable<simple_iter_archetype<1, value_type<3>>, out_archetype<0, 3>>);
    static_assert(!indirectly_copyable_storable<simple_iter_archetype<1, value_type<3>>, out_archetype<1, 3>>);
    static_assert(!indirectly_copyable_storable<simple_iter_archetype<1, value_type<3>>, out_archetype<2, 3>>);
    static_assert(!indirectly_copyable_storable<simple_iter_archetype<1, value_type<3>>, out_archetype<3, 3>>);
    static_assert(!indirectly_copyable_storable<simple_iter_archetype<1, value_type<3>>, out_archetype<4, 3>>);
    static_assert(!indirectly_copyable_storable<simple_iter_archetype<1, value_type<0>>, out_archetype<5, 0>>);
    static_assert(!indirectly_copyable_storable<simple_iter_archetype<1, value_type<1>>, out_archetype<5, 1>>);
    static_assert(!indirectly_copyable_storable<simple_iter_archetype<1, value_type<2>>, out_archetype<5, 2>>);
    static_assert(indirectly_copyable_storable<simple_iter_archetype<1, value_type<3>>, out_archetype<5, 3>>);
} // namespace indirectly_copyable_test

namespace indirectly_swappable_test {
    using std::indirectly_readable, std::indirectly_swappable;

    template <int I>
    struct archetype {
        using value_type = int;

        struct reference {
            operator int() const;
        };

        reference operator*() const noexcept
            requires (I != 0);

        friend constexpr void iter_swap(archetype const&, archetype const&)
            requires (I != 1)
        {}

        template <int J>
            requires (I != J && I != 2 && J != 2)
        friend constexpr void iter_swap(archetype const&, archetype<J> const&) {}
    };

    // We have to be careful to avoid checking some !indirectly_swappable cases that fail when the Evil Extension is
    // active (i.e., in permissive mode).
    template <bool IsPermissive>
    constexpr bool compile_only() {
        static_assert(!indirectly_readable<archetype<0>>); // <0> is not indirectly_readable
        static_assert(indirectly_readable<archetype<1>>); // <1+> is indirectly_readable
        static_assert(indirectly_readable<archetype<2>>);
        static_assert(indirectly_readable<archetype<3>>);
        static_assert(indirectly_readable<archetype<4>>);

        static_assert(!indirectly_swappable<archetype<0>, archetype<0>>); // <0> is (still) not indirectly_readable
        if constexpr (!IsPermissive) {
            static_assert(!indirectly_swappable<archetype<1>, archetype<1>>); // <1> is not self-indirectly_swappable
        }
        static_assert(indirectly_swappable<archetype<2>, archetype<2>>); // <2+> is self-indirectly_swappable
        static_assert(indirectly_swappable<archetype<3>, archetype<3>>);
        static_assert(indirectly_swappable<archetype<4>, archetype<4>>);

        static_assert(!indirectly_swappable<archetype<0>, archetype<4>>); // <0> is not indirectly_readable
        static_assert(!indirectly_swappable<archetype<4>, archetype<0>>); // <0> is not indirectly_readable
        if constexpr (!IsPermissive) {
            static_assert(!indirectly_swappable<archetype<1>, archetype<4>>); // <1> is not self-indirectly_swappable
            static_assert(!indirectly_swappable<archetype<4>, archetype<1>>); // <1> is not self-indirectly_swappable
        }
        static_assert(!indirectly_swappable<archetype<2>, archetype<4>>); // <2> & <4> aren't cross-indirectly_swappable
        static_assert(!indirectly_swappable<archetype<4>, archetype<2>>); // <2> & <4> aren't cross-indirectly_swappable

        static_assert(indirectly_swappable<archetype<3>, archetype<4>>);
        static_assert(indirectly_swappable<archetype<4>, archetype<3>>);

        return true;
    }
    static_assert(compile_only<is_permissive>());
} // namespace indirectly_swappable_test

namespace indirectly_comparable_test {
    // Also validate indirect_result_t
    using std::indirectly_comparable;

    using Proj    = int (&)(simple_common_reference<int>);
    using BadProj = char const* (&) (simple_common_reference<int>);

    template <int>
    struct base {};
    template <>
    struct base<0> {
        // 0: not copy_constructible
        base()            = default;
        base(base const&) = delete;
    };

    template <int I>
    struct Fn : base<I> {
        // 1: not predicate
        void operator()(int, int) const
            requires (I == 1);
        void* operator()(int, int) const
            requires (I != 1);
    };

    static_assert(!indirectly_comparable<simple_iter_archetype<1>, simple_iter_archetype<1>, Fn<0>, Proj, Proj>);
    static_assert(!indirectly_comparable<simple_iter_archetype<1>, simple_iter_archetype<1>, Fn<1>, Proj, Proj>);
    static_assert(!indirectly_comparable<simple_iter_archetype<0>, simple_iter_archetype<1>, Fn<2>, Proj, Proj>);
    static_assert(!indirectly_comparable<simple_iter_archetype<1>, simple_iter_archetype<0>, Fn<2>, Proj, Proj>);
    static_assert(!indirectly_comparable<simple_iter_archetype<1>, simple_iter_archetype<1>, Fn<2>, BadProj, Proj>);
    static_assert(!indirectly_comparable<simple_iter_archetype<1>, simple_iter_archetype<1>, Fn<2>, Proj, BadProj>);
    static_assert(indirectly_comparable<simple_iter_archetype<1>, simple_iter_archetype<1>, Fn<2>, Proj, Proj>);

    using Projected = std::projected<simple_iter_archetype<1>, Proj>;
    static_assert(std::same_as<std::indirect_result_t<Fn<2>, Projected, Projected>, void*>);
} // namespace indirectly_comparable_test

namespace dangling_test {
    // Also test borrowed_iterator_t and borrowed_subrange_t
    using ranges::dangling, ranges::borrowed_iterator_t, ranges::borrowed_subrange_t;
    using std::is_nothrow_constructible_v, std::same_as;

    static_assert(std::is_class_v<dangling>);
    static_assert(std::semiregular<dangling>);
    static_assert(std::is_trivially_default_constructible_v<dangling>);

    // dangling is constructible from any sequence of arguments without throwing
    static_assert(is_nothrow_constructible_v<dangling>);
    static_assert(is_nothrow_constructible_v<dangling, int>);
    static_assert(is_nothrow_constructible_v<dangling, int*>);
    static_assert(is_nothrow_constructible_v<dangling, int[42]>);
    static_assert(is_nothrow_constructible_v<dangling, int (*)()>);
    static_assert(is_nothrow_constructible_v<dangling, const int (*)[42]>);
    static_assert(is_nothrow_constructible_v<dangling, int, int*, int[42], int (*)(), const int (*)[42]>);

    static_assert(same_as<borrowed_iterator_t<borrowed<false>>, dangling>);
    static_assert(same_as<borrowed_iterator_t<borrowed<true>>, int*>);

    static_assert(same_as<borrowed_subrange_t<borrowed<false>>, dangling>);
    static_assert(same_as<borrowed_subrange_t<borrowed<true>>, ranges::subrange<int*>>);
} // namespace dangling_test

namespace result_test {
    using ranges::in_found_result, ranges::in_fun_result, ranges::in_in_result, ranges::in_out_result,
        ranges::in_in_out_result, ranges::in_out_out_result, ranges::min_max_result;
    using std::is_aggregate_v, std::is_convertible_v, std::is_trivially_copyable_v,
        std::is_trivially_default_constructible_v;

    // Validate the result types are:
    // * aggregates
    static_assert(is_aggregate_v<in_found_result<int>>);
    static_assert(is_aggregate_v<in_fun_result<int, int>>);
    static_assert(is_aggregate_v<in_in_result<int, int>>);
    static_assert(is_aggregate_v<in_out_result<int, int>>);
    static_assert(is_aggregate_v<in_in_out_result<int, int, int>>);
    static_assert(is_aggregate_v<in_out_out_result<int, int, int>>);
    static_assert(is_aggregate_v<min_max_result<int>>);

    // * trivially copyable when parameter types are trivially copyable
    static_assert(is_trivially_copyable_v<in_found_result<int>>);
    static_assert(is_trivially_copyable_v<in_fun_result<int, int>>);
    static_assert(is_trivially_copyable_v<in_in_result<int, int>>);
    static_assert(is_trivially_copyable_v<in_out_result<int, int>>);
    static_assert(is_trivially_copyable_v<in_in_out_result<int, int, int>>);
    static_assert(is_trivially_copyable_v<in_out_out_result<int, int, int>>);
    static_assert(is_trivially_copyable_v<min_max_result<int>>);

    // * trivially default constructible when parameter types are trivially default constructible
    static_assert(is_trivially_default_constructible_v<in_found_result<int>>);
    static_assert(is_trivially_default_constructible_v<in_fun_result<int, int>>);
    static_assert(is_trivially_default_constructible_v<in_in_result<int, int>>);
    static_assert(is_trivially_default_constructible_v<in_out_result<int, int>>);
    static_assert(is_trivially_default_constructible_v<in_in_out_result<int, int, int>>);
    static_assert(is_trivially_default_constructible_v<in_out_out_result<int, int, int>>);
    static_assert(is_trivially_default_constructible_v<min_max_result<int>>);

    // * usable with structured bindings
    constexpr bool test_bindings_in_found_result() {
        auto [x, y] = in_found_result<int>{42, true};
        assert(x == 42);
        assert(y == true);
        return true;
    }
    template <class T>
    constexpr bool test_bindings_2() {
        auto [x, y] = T{13, 42};
        assert(x == 13);
        assert(y == 42);
        return true;
    }
    template <class T>
    constexpr bool test_bindings_3() {
        auto [x, y, z] = T{2, 3, 5};
        assert(x == 2);
        assert(y == 3);
        assert(z == 5);
        return true;
    }

    static_assert(test_bindings_in_found_result());
    static_assert(test_bindings_2<in_fun_result<int, int>>());
    static_assert(test_bindings_2<in_in_result<int, int>>());
    static_assert(test_bindings_2<in_out_result<int, int>>());
    static_assert(test_bindings_3<in_in_out_result<int, int, int>>());
    static_assert(test_bindings_3<in_out_out_result<int, int, int>>());
    static_assert(test_bindings_2<min_max_result<int>>());

    // * appropriately lvalue and rvalue inter-specialization convertible
    template <class T>
    struct convertible_from {
        convertible_from(std::add_rvalue_reference_t<T>);
    };

    using CFI = convertible_from<int>;

    constexpr bool test_convertible_in_found_result() {
        static_assert(!is_convertible_v<in_found_result<int> const&, in_found_result<CFI>>);
        static_assert(is_convertible_v<in_found_result<int>, in_found_result<CFI>>);
        in_found_result<int> a{42, true};
        {
            in_found_result<long> b = std::as_const(a);
            auto [x, y]             = b;
            assert(x == 42);
            assert(y);
        }
        {
            in_found_result<long> b = std::move(a);
            auto [x, y]             = b;
            assert(x == 42);
            assert(y);
        }
        return true;
    }
    static_assert(test_convertible_in_found_result());
    constexpr bool test_convertible_min_max_result() {
        static_assert(!is_convertible_v<min_max_result<int> const&, min_max_result<CFI>>);
        static_assert(is_convertible_v<min_max_result<int>, min_max_result<CFI>>);
        min_max_result<int> a{13, 42};
        {
            min_max_result<long> b = std::as_const(a);
            auto [x, y]            = b;
            assert(x == 13);
            assert(y == 42);
        }
        {
            min_max_result<long> b = std::move(a);
            auto [x, y]            = b;
            assert(x == 13);
            assert(y == 42);
        }
        return true;
    }
    static_assert(test_convertible_min_max_result());

    template <template <class, class> class R>
    constexpr bool test_convertible_2() {
        static_assert(!is_convertible_v<R<int, int> const&, R<CFI, CFI>>);
        static_assert(is_convertible_v<R<int, int>, R<CFI, CFI>>);
        R<int, int> a{13, 42};
        {
            R<long, long> b = std::as_const(a);
            auto [x, y]     = b;
            assert(x == 13);
            assert(y == 42);
        }
        {
            R<long, long> b = std::move(a);
            auto [x, y]     = b;
            assert(x == 13);
            assert(y == 42);
        }
        return true;
    }
    static_assert(test_convertible_2<in_fun_result>());
    static_assert(test_convertible_2<in_in_result>());
    static_assert(test_convertible_2<in_out_result>());

    template <template <class, class, class> class R>
    constexpr bool test_convertible_3() {
        static_assert(!is_convertible_v<R<int, int, int> const&, R<CFI, CFI, CFI>>);
        static_assert(is_convertible_v<R<int, int, int>, R<CFI, CFI, CFI>>);
        R<int, int, int> a{13, 42, 1729};
        {
            R<long, long, long> b = std::as_const(a);
            auto [x, y, z]        = b;
            assert(x == 13);
            assert(y == 42);
            assert(z == 1729);
        }
        {
            R<long, long, long> b = std::move(a);
            auto [x, y, z]        = b;
            assert(x == 13);
            assert(y == 42);
            assert(z == 1729);
        }
        return true;
    }
    static_assert(test_convertible_3<in_in_out_result>());
    static_assert(test_convertible_3<in_out_out_result>());
} // namespace result_test

namespace permutable_test {
    using std::forward_iterator, std::input_iterator, std::indirectly_movable_storable, std::indirectly_swappable,
        std::permutable;

    template <int I>
    struct archetype {
        using value_type      = int;
        using difference_type = int;

        struct proxy {
            proxy()                        = default;
            proxy(proxy const&)            = delete;
            proxy& operator=(proxy const&) = delete;

            operator int() const;
            // 1: not indirectly_movable_storable<archetype, archetype>
            void operator=(int) const
                requires (I != 1);
        };

        proxy operator*() const;

        archetype& operator++();
        archetype operator++(int);

        // 0: not forward_iterator (input only)
        bool operator==(archetype const&) const
            requires (I != 0)
        = default;

        friend int iter_move(archetype const&) {
            return 42;
        }
        friend void iter_swap(archetype const&, archetype const&) {}

        // Ideally we'd have a "not reflexively indirectly_swappable" case as well, but there's no way for a class to
        // satisfy indirectly_movable_storable<T, T> without satisfying indirectly_swappable<T, T> thanks to N4861
        // [iterator.cust.swap]/4.3. permutable requires indirectly_swappable<T, T> only to forbid a user type from
        // defining an iter_swap overload that doesn't meet the semantic requirements.
    };
    static_assert(input_iterator<archetype<0>>);
    static_assert(!forward_iterator<archetype<0>>);
    static_assert(indirectly_movable_storable<archetype<0>, archetype<0>>);
    static_assert(indirectly_swappable<archetype<0>, archetype<0>>);

    static_assert(forward_iterator<archetype<1>>);
    static_assert(!indirectly_movable_storable<archetype<1>, archetype<1>>);
    static_assert(indirectly_swappable<archetype<1>, archetype<1>>);

    static_assert(forward_iterator<archetype<2>>);
    static_assert(indirectly_movable_storable<archetype<2>, archetype<2>>);
    static_assert(indirectly_swappable<archetype<2>, archetype<2>>);

    static_assert(!permutable<archetype<0>>);
    static_assert(!permutable<archetype<1>>);
    static_assert(permutable<archetype<2>>);
} // namespace permutable_test

namespace mergeable_test {
    using ranges::less;
    using std::identity, std::indirect_strict_weak_order, std::permutable, std::projected, std::mergeable;

    enum class readable_status { not_input_iter, good };

    template <class T, readable_status RS>
    struct readable_archetype {
        using value_type      = T;
        using difference_type = int;

        T const& operator*() const;
        readable_archetype& operator++();

        // 0: not input_iterator
        void operator++(int)
            requires (RS != readable_status::not_input_iter);
    };

    enum class writable_status { not_weakly_incrementable, not_ind_copy_int, not_ind_copy_long, good };

    template <writable_status WS>
    struct writable_archetype {
        using difference_type = int;

        writable_archetype& operator*();
        writable_archetype& operator++();

        writable_archetype operator++(int)
            requires (WS != writable_status::not_weakly_incrementable);

        // 1: not indirectly_copyable<const int*, writable_archetype>
        void operator=(int)
            requires (WS == writable_status::not_ind_copy_int)
        = delete;
        writable_archetype& operator=(int)
            requires (WS != writable_status::not_ind_copy_int);

        // 2: not indirectly_copyable<const long*, writable_archetype>
        void operator=(long)
            requires (WS == writable_status::not_ind_copy_long)
        = delete;
        writable_archetype& operator=(long)
            requires (WS != writable_status::not_ind_copy_long);
    };

    void test() {
        using std::indirect_strict_weak_order, std::indirectly_copyable, std::input_iterator, std::mergeable,
            std::weakly_incrementable;

        using I1  = readable_archetype<int, readable_status::good>;
        using I2  = readable_archetype<long, readable_status::good>;
        using O   = writable_archetype<writable_status::good>;
        using Pr  = ranges::less;
        using Pj1 = std::identity;
        using Pj2 = std::identity;

        {
            using Bad_I1 = readable_archetype<int, readable_status::not_input_iter>;
#ifndef _M_CEE // TRANSITION, VSO-1665670
            static_assert(!input_iterator<Bad_I1>);
#endif // ^^^ no workaround ^^^
            static_assert(input_iterator<I2>);
            static_assert(weakly_incrementable<O>);
            static_assert(indirectly_copyable<Bad_I1, O>);
            static_assert(indirectly_copyable<I2, O>);
            static_assert(indirect_strict_weak_order<Pr, projected<Bad_I1, Pj1>, projected<I2, Pj2>>);
#ifndef _M_CEE // TRANSITION, VSO-1665670
            static_assert(!mergeable<Bad_I1, I2, O, Pr, Pj1, Pj2>);
#endif // ^^^ no workaround ^^^
        }

        {
            using Bad_I2 = readable_archetype<long, readable_status::not_input_iter>;
            static_assert(input_iterator<I1>);
#ifndef _M_CEE // TRANSITION, VSO-1665670
            static_assert(!input_iterator<Bad_I2>);
#endif // ^^^ no workaround ^^^
            static_assert(weakly_incrementable<O>);
            static_assert(indirectly_copyable<I1, O>);
            static_assert(indirectly_copyable<Bad_I2, O>);
            static_assert(indirect_strict_weak_order<Pr, projected<I1, Pj1>, projected<Bad_I2, Pj2>>);
#ifndef _M_CEE // TRANSITION, VSO-1665670
            static_assert(!mergeable<I1, Bad_I2, O, Pr, Pj1, Pj2>);
#endif // ^^^ no workaround ^^^
        }

        {
            using Bad_O = writable_archetype<writable_status::not_weakly_incrementable>;
            static_assert(input_iterator<I1>);
            static_assert(input_iterator<I2>);
#ifndef _M_CEE // TRANSITION, VSO-1665670
            static_assert(!weakly_incrementable<Bad_O>);
#endif // ^^^ no workaround ^^^
            static_assert(indirectly_copyable<I1, Bad_O>);
            static_assert(indirectly_copyable<I2, Bad_O>);
            static_assert(indirect_strict_weak_order<Pr, projected<I1, Pj1>, projected<I2, Pj2>>);
#ifndef _M_CEE // TRANSITION, VSO-1665670
            static_assert(!mergeable<I1, I2, Bad_O, Pr, Pj1, Pj2>);
#endif // ^^^ no workaround ^^^
        }

        {
            using Bad_O = writable_archetype<writable_status::not_ind_copy_int>;
            static_assert(input_iterator<I1>);
            static_assert(input_iterator<I2>);
            static_assert(weakly_incrementable<Bad_O>);
            static_assert(!indirectly_copyable<I1, Bad_O>);
            static_assert(indirectly_copyable<I2, Bad_O>);
            static_assert(indirect_strict_weak_order<Pr, projected<I1, Pj1>, projected<I2, Pj2>>);
            static_assert(!mergeable<I1, I2, Bad_O, Pr, Pj1, Pj2>);
        }

        {
            using Bad_O = writable_archetype<writable_status::not_ind_copy_long>;
            static_assert(input_iterator<I1>);
            static_assert(input_iterator<I2>);
            static_assert(weakly_incrementable<Bad_O>);
            static_assert(indirectly_copyable<I1, Bad_O>);
            static_assert(!indirectly_copyable<I2, Bad_O>);
            static_assert(indirect_strict_weak_order<Pr, projected<I1, Pj1>, projected<I2, Pj2>>);
            static_assert(!mergeable<I1, I2, Bad_O, Pr, Pj1, Pj2>);
        }

        {
            using Bad_Pr = int;
            static_assert(input_iterator<I1>);
            static_assert(input_iterator<I2>);
            static_assert(weakly_incrementable<O>);
            static_assert(indirectly_copyable<I1, O>);
            static_assert(indirectly_copyable<I2, O>);
            static_assert(!indirect_strict_weak_order<Bad_Pr, projected<I1, Pj1>, projected<I2, Pj2>>);
            static_assert(!mergeable<I1, I2, O, Bad_Pr, Pj1, Pj2>);
        }

        static_assert(input_iterator<I1>);
        static_assert(input_iterator<I2>);
        static_assert(weakly_incrementable<O>);
        static_assert(indirectly_copyable<I1, O>);
        static_assert(indirectly_copyable<I2, O>);
        static_assert(indirect_strict_weak_order<Pr, projected<I1, Pj1>, projected<I2, Pj2>>);
        static_assert(mergeable<I1, I2, O, Pr, Pj1, Pj2>);
    }
} // namespace mergeable_test

namespace sortable_test {
    using ranges::less;
    using std::identity, std::indirect_strict_weak_order, std::permutable, std::projected, std::sortable;

    void test() {
        {
            using I = int const*; // not permutable
            using C = less;
            using P = identity;
            static_assert(!permutable<I>);
            static_assert(indirect_strict_weak_order<C, projected<I, P>>);
            static_assert(!sortable<I, C, P>);
        }

        {
            using I = int*;
            using C = void; // not an indirect_strict_weak_order
            using P = identity;
            static_assert(permutable<I>);
            static_assert(!indirect_strict_weak_order<C, projected<I, P>>);
            static_assert(!sortable<I, C, P>);
        }

        {
            using I = int*;
            using C = less;
            using P = identity;
            static_assert(permutable<I>);
            static_assert(indirect_strict_weak_order<C, projected<I, P>>);
            static_assert(sortable<I, C, P>);
        }
    }
} // namespace sortable_test

namespace gh_1089 {
    // Defend against regression of GH-1089: "_Pass_fn/_Ref_fn interferes with the Ranges invoke protocol"
    // The _Pass_fn protocol would previously assume that anything larger than a pointer was a function object that it
    // could call with `()` and not a pointer-to-member that requires the `invoke` protocol.

    void test() {
        {
            struct Base {
                virtual int purr() = 0;
            };

            struct Derived1 : virtual Base {
                int purr() override {
                    return 1729;
                }
            };

            struct Derived2 : virtual Base {};

            struct MostDerived : Derived1, Derived2 {
                int purr() override {
                    return 2020;
                }
            };


            static_assert(sizeof(&Derived1::purr) > sizeof(void*)); // NB: relies on non-portable platform properties

            Derived1 a[2];
            MostDerived b[3];
            Derived1* pointers[] = {&b[0], &a[0], &b[1], &a[1], &b[2]};

            (void) ranges::count(pointers, 2020, &Derived1::purr);
        }
        {
            struct Cat;

            using PMD_Cat = int Cat::*;
            // Quantum effects: we must observe the size before defining Cat or it will become smaller.
            static_assert(sizeof(PMD_Cat) > sizeof(void*));

            struct Cat {
                int x = 42;
            };

            static_assert(sizeof(&Cat::x) > sizeof(void*)); // NB: relies on non-portable platform properties

            Cat cats[42];

            (void) ranges::count(cats, 42, &Cat::x);
        }
    }
} // namespace gh_1089

namespace special_memory_concepts {
    // Validate the concepts from [special.mem.concepts] used to constrain the specialized memory algorithms
    // NB: Non-portable tests of internal machinery
    using ranges::_No_throw_input_iterator, ranges::_No_throw_sentinel_for, ranges::_No_throw_input_range,
        ranges::_No_throw_forward_iterator, ranges::_No_throw_forward_range;
    using std::forward_iterator, std::input_iterator, std::sentinel_for;

    enum class iterator_status : int { not_input, not_lvalue_reference, different_reference_and_value, input, forward };

    template <iterator_status I>
    struct iterator_archetype {
        using iterator_concept = std::conditional_t<I == iterator_status::not_input, std::output_iterator_tag,
            std::conditional_t<I == iterator_status::forward, std::forward_iterator_tag, std::input_iterator_tag>>;
        using difference_type  = int;
        using value_type       = std::conditional_t<I == iterator_status::different_reference_and_value, long, int>;

        int operator*() const
            requires (I == iterator_status::not_lvalue_reference);
        int& operator*() const
            requires (I != iterator_status::not_lvalue_reference);

        iterator_archetype& operator++();
        void operator++(int)
            requires (I != iterator_status::forward);
        iterator_archetype operator++(int)
            requires (I == iterator_status::forward);

        bool operator==(std::default_sentinel_t) const;
        bool operator==(iterator_archetype const&) const
            requires (I == iterator_status::forward);
    };
    // Verify iterator_archetype
    static_assert(!input_iterator<iterator_archetype<iterator_status::not_input>>);
    static_assert(input_iterator<iterator_archetype<iterator_status::not_lvalue_reference>>);
    static_assert(input_iterator<iterator_archetype<iterator_status::different_reference_and_value>>);
    static_assert(input_iterator<iterator_archetype<iterator_status::input>>);
    static_assert(!forward_iterator<iterator_archetype<iterator_status::input>>);
    static_assert(forward_iterator<iterator_archetype<iterator_status::forward>>);

    template <class I>
    constexpr bool has_lvalue_reference = std::is_lvalue_reference_v<std::iter_reference_t<I>>;
    static_assert(has_lvalue_reference<iterator_archetype<iterator_status::not_input>>);
    static_assert(!has_lvalue_reference<iterator_archetype<iterator_status::not_lvalue_reference>>);
    static_assert(has_lvalue_reference<iterator_archetype<iterator_status::different_reference_and_value>>);
    static_assert(has_lvalue_reference<iterator_archetype<iterator_status::input>>);
    static_assert(has_lvalue_reference<iterator_archetype<iterator_status::forward>>);

    template <class I>
    constexpr bool same_reference_value =
        std::same_as<std::remove_cvref_t<std::iter_reference_t<I>>, std::iter_value_t<I>>;
    static_assert(same_reference_value<iterator_archetype<iterator_status::not_input>>);
    static_assert(same_reference_value<iterator_archetype<iterator_status::not_lvalue_reference>>);
    static_assert(!same_reference_value<iterator_archetype<iterator_status::different_reference_and_value>>);
    static_assert(same_reference_value<iterator_archetype<iterator_status::input>>);
    static_assert(same_reference_value<iterator_archetype<iterator_status::forward>>);

    // Validate _No_throw_input_iterator
    static_assert(!_No_throw_input_iterator<iterator_archetype<iterator_status::not_input>>);
    static_assert(!_No_throw_input_iterator<iterator_archetype<iterator_status::not_lvalue_reference>>);
    static_assert(!_No_throw_input_iterator<iterator_archetype<iterator_status::different_reference_and_value>>);
    static_assert(_No_throw_input_iterator<iterator_archetype<iterator_status::input>>);
    static_assert(_No_throw_input_iterator<iterator_archetype<iterator_status::forward>>);

    enum class sentinel_status : int { no, yes };

    template <sentinel_status I>
    struct sentinel_archetype {
        template <iterator_status S>
        bool operator==(iterator_archetype<S> const&) const
            requires (I != sentinel_status::no);
    };
    // Verify sentinel_archetype
    static_assert(!sentinel_for<sentinel_archetype<sentinel_status::no>, iterator_archetype<iterator_status::input>>);
    static_assert(sentinel_for<sentinel_archetype<sentinel_status::yes>, iterator_archetype<iterator_status::input>>);

    // Validate _No_throw_sentinel_for
    static_assert(
        !_No_throw_sentinel_for<sentinel_archetype<sentinel_status::no>, iterator_archetype<iterator_status::input>>);
    static_assert(
        _No_throw_sentinel_for<sentinel_archetype<sentinel_status::yes>, iterator_archetype<iterator_status::input>>);
    static_assert(!_No_throw_sentinel_for<iterator_archetype<iterator_status::input>,
        iterator_archetype<iterator_status::input>>);
    static_assert(_No_throw_sentinel_for<iterator_archetype<iterator_status::forward>,
        iterator_archetype<iterator_status::forward>>);

    // Validate _No_throw_forward_iterator
    static_assert(!_No_throw_forward_iterator<iterator_archetype<iterator_status::not_input>>);
    static_assert(!_No_throw_forward_iterator<iterator_archetype<iterator_status::not_lvalue_reference>>);
    static_assert(!_No_throw_forward_iterator<iterator_archetype<iterator_status::different_reference_and_value>>);
    static_assert(!_No_throw_forward_iterator<iterator_archetype<iterator_status::input>>);
    static_assert(_No_throw_forward_iterator<iterator_archetype<iterator_status::forward>>);

    enum class range_status : int { not_range, not_input, input, forward };

    template <range_status I>
    struct range_archetype {
        using It = std::conditional_t<I == range_status::not_range, void,
            std::conditional_t<I == range_status::not_input, iterator_archetype<iterator_status::not_input>,
                std::conditional_t<I == range_status::forward, iterator_archetype<iterator_status::forward>,
                    iterator_archetype<iterator_status::input>>>>;
        using Se = std::conditional_t<I == range_status::not_range, void,
            std::conditional_t<I == range_status::forward, iterator_archetype<iterator_status::forward>,
                std::default_sentinel_t>>;

        It begin() const;
        Se end() const;
    };
    // Verify range_archetype
    static_assert(!ranges::range<range_archetype<range_status::not_range>>);
    static_assert(ranges::range<range_archetype<range_status::not_input>>);
    static_assert(ranges::range<range_archetype<range_status::input>>);
    static_assert(ranges::range<range_archetype<range_status::forward>>);

    // Validate _No_throw_input_range; note that the distinction between range<R> and
    // no-throw-sentinel-for<sentinel_t<R>, iterator_t<R>> is purely semantic, so we can't test them separately.
    static_assert(!_No_throw_input_range<range_archetype<range_status::not_range>>);
    static_assert(!_No_throw_input_range<range_archetype<range_status::not_input>>);
    static_assert(_No_throw_input_range<range_archetype<range_status::input>>);
    static_assert(_No_throw_input_range<range_archetype<range_status::forward>>);

    // Validate _No_throw_forward_range
    static_assert(!_No_throw_forward_range<range_archetype<range_status::not_range>>);
    static_assert(!_No_throw_forward_range<range_archetype<range_status::not_input>>);
    static_assert(!_No_throw_forward_range<range_archetype<range_status::input>>);
    static_assert(_No_throw_forward_range<range_archetype<range_status::forward>>);
} // namespace special_memory_concepts
