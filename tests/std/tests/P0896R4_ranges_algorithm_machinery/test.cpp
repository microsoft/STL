// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

namespace ranges = std::ranges;

int main() {} // COMPILE-ONLY

namespace detail {
    static constexpr bool permissive() {
        return false;
    }

    template <class>
    struct DependentBase {
        static constexpr bool permissive() {
            return true;
        }
    };

    template <class T>
    struct Derived : DependentBase<T> {
        static constexpr bool test() {
            return permissive();
        }
    };
} // namespace detail
constexpr bool is_permissive = detail::Derived<int>::test();

template <bool>
struct borrowed { // borrowed<true> is a borrowed_range; borrowed<false> is not
    int* begin() const;
    int* end() const;
};

template <>
inline constexpr bool std::ranges::enable_borrowed_range<borrowed<true>> = true;

struct simple_reference {
    operator int() const;
};

struct simple_common_reference {
    simple_common_reference(simple_reference);
    simple_common_reference(int);
};

template <int I>
struct simple_iter_archetype {
    using value_type = int;

    // 0: not indirectly_readable
    simple_reference operator*() const requires(I != 0);

    friend void iter_swap(simple_iter_archetype const&, simple_iter_archetype const&) {}
};
STATIC_ASSERT(!std::indirectly_readable<simple_iter_archetype<0>>);
STATIC_ASSERT(std::indirectly_readable<simple_iter_archetype<1>>);

template <template <class> class TQuals, template <class> class UQuals>
struct std::basic_common_reference<int, ::simple_reference, TQuals, UQuals> {
    using type = ::simple_common_reference;
};
template <template <class> class TQuals, template <class> class UQuals>
struct std::basic_common_reference<::simple_reference, int, TQuals, UQuals> {
    using type = ::simple_common_reference;
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
        void operator()(int&) const requires(I == 1) = delete;
        // 2: not invocable<Fn&, iter_reference_t<simple_iter_archetype>>
        void operator()(simple_reference) const requires(I == 2) = delete;
        // 3: not invocable<Fn&, iter_common_reference_t<simple_iter_archetype>>
        void operator()(simple_common_reference) const requires(I == 3) = delete;

        // 4 : not common_reference_with<invoke_result_t<Fn&, iter_value_t<simple_iter_archetype>&>,
        //                               invoke_result_t<Fn&, iter_reference_t<simple_iter_archetype>>>;
        void operator()(int&) const requires(I == 4);
        int operator()(simple_reference) const requires(I == 4);

        int operator()(int&) const requires(I != 1 && I != 4);
        int operator()(simple_reference) const requires(I != 2 && I != 4);
        int operator()(simple_common_reference) const requires(I != 3 && I != 4);
    };

    template <class F, class I>
    constexpr bool test() {
        constexpr bool result = std::indirectly_unary_invocable<F, I>;
        STATIC_ASSERT(result == std::indirectly_regular_unary_invocable<F, I>);
        return result;
    }
    STATIC_ASSERT(!test<Fn<0>, simple_iter_archetype<1>>());
    STATIC_ASSERT(!test<Fn<1>, simple_iter_archetype<1>>());
    STATIC_ASSERT(!test<Fn<2>, simple_iter_archetype<1>>());
    STATIC_ASSERT(!test<Fn<3>, simple_iter_archetype<1>>());
    STATIC_ASSERT(!test<Fn<4>, simple_iter_archetype<1>>());
    STATIC_ASSERT(!test<Fn<5>, simple_iter_archetype<0>>());
    STATIC_ASSERT(test<Fn<5>, simple_iter_archetype<1>>());

    STATIC_ASSERT(std::same_as<std::indirect_result_t<Fn<5>, simple_iter_archetype<1>>, int>);
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
        void operator()(int&) const requires(I == 1);
        // 2: not predicate<Fn&, iter_reference_t<simple_iter_archetype>>
        void operator()(simple_reference) const requires(I == 2) = delete;
        // 3: not predicate<Fn&, iter_common_reference_t<simple_iter_archetype>>
        void operator()(simple_common_reference) const requires(I == 3) = delete;

        int operator()(int&) const requires(I != 1 && I != 4);
        int operator()(simple_reference) const requires(I != 2 && I != 4);
        int operator()(simple_common_reference) const requires(I != 3 && I != 4);
    };

    STATIC_ASSERT(!indirect_unary_predicate<Fn<0>, simple_iter_archetype<1>>);
    STATIC_ASSERT(!indirect_unary_predicate<Fn<1>, simple_iter_archetype<1>>);
    STATIC_ASSERT(!indirect_unary_predicate<Fn<2>, simple_iter_archetype<1>>);
    STATIC_ASSERT(!indirect_unary_predicate<Fn<3>, simple_iter_archetype<1>>);
    STATIC_ASSERT(!indirect_unary_predicate<Fn<4>, simple_iter_archetype<1>>);
    STATIC_ASSERT(!indirect_unary_predicate<Fn<5>, simple_iter_archetype<0>>);
    STATIC_ASSERT(indirect_unary_predicate<Fn<5>, simple_iter_archetype<1>>);

    STATIC_ASSERT(std::same_as<std::indirect_result_t<Fn<5>, simple_iter_archetype<1>>, int>);
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
        void operator()(int&, int&) const requires(I == 1);
        // 2: not predicate<Fn&, iter_value_t<simple_iter_archetype>&, iter_reference_t<simple_iter_archetype>>
        void operator()(int&, simple_reference) const requires(I == 2);
        // 3: not predicate<Fn&, iter_reference_t<simple_iter_archetype>, iter_value_t<simple_iter_archetype>&>
        void operator()(simple_reference, int&) const requires(I == 3);
        // 4: not predicate<Fn&, iter_reference_t<simple_iter_archetype>, iter_reference_t<simple_iter_archetype>>
        void operator()(simple_reference, simple_reference) const requires(I == 4);
        // 5: not predicate<Fn&, iter_common_reference_t</**/>, iter_common_reference_t</**/>>
        void operator()(simple_common_reference, simple_common_reference) const requires(I == 5);

        bool operator()(int&, int&) const requires(I != 1);
        int operator()(int&, simple_reference) const requires(I != 2);
        int* operator()(simple_reference, int&) const requires(I != 3);
        std::true_type operator()(simple_reference, simple_reference) const requires(I != 4);
        std::false_type operator()(simple_common_reference, simple_common_reference) const requires(I != 5);
    };

    template <int FuncSelector, int IterSelector1, int IterSelector2>
    constexpr bool test() {
        using std::indirect_binary_predicate, std::indirect_equivalence_relation, std::indirect_strict_weak_order;
        using F               = Fn<FuncSelector>;
        using I1              = simple_iter_archetype<IterSelector1>;
        using I2              = simple_iter_archetype<IterSelector2>;
        constexpr bool result = indirect_binary_predicate<F, I1, I2>;
        STATIC_ASSERT(indirect_equivalence_relation<F, I1, I2> == result);
        STATIC_ASSERT(indirect_strict_weak_order<F, I1, I2> == result);
        return result;
    }

    STATIC_ASSERT(!test<0, 1, 1>());
    STATIC_ASSERT(!test<1, 1, 1>());
    STATIC_ASSERT(!test<2, 1, 1>());
    STATIC_ASSERT(!test<3, 1, 1>());
    STATIC_ASSERT(!test<4, 1, 1>());
    STATIC_ASSERT(!test<5, 1, 1>());

    STATIC_ASSERT(!test<6, 0, 1>());
    STATIC_ASSERT(!test<6, 1, 0>());
    STATIC_ASSERT(test<6, 1, 1>());

    STATIC_ASSERT(std::same_as<std::indirect_result_t<Fn<6>, simple_iter_archetype<1>, simple_iter_archetype<1>>,
        std::true_type>);
} // namespace indirect_binary_predicate_test

namespace projected_test {
    template <class Iter, class Proj, class Value, class Reference>
    constexpr bool test() {
        STATIC_ASSERT(std::indirectly_readable<Iter>);

        using P = std::projected<Iter, Proj>;
        STATIC_ASSERT(std::indirectly_readable<P>);
        STATIC_ASSERT(std::same_as<std::iter_value_t<P>, Value>);
        STATIC_ASSERT(std::same_as<std::iter_reference_t<P>, Reference>);
        return true;
    }
    STATIC_ASSERT(test<int*, std::identity, int, int&>());
    STATIC_ASSERT(test<int const*, std::identity, int, int const&>());

    struct S {};
    STATIC_ASSERT(test<S*, int(S::*), int, int&>());
    STATIC_ASSERT(test<S const*, int(S::*), int, int const&>());

    struct iter {
        using value_type = int;
        struct reference {
            operator value_type() const;
        };
        reference operator*() const;
    };
    STATIC_ASSERT(test<iter, std::identity, iter::reference, iter::reference&&>());
    STATIC_ASSERT(test<iter, double (*)(int), double, double>());
} // namespace projected_test

namespace indirectly_copyable_test {
    using std::indirectly_copyable;

    template <int I>
    struct out_archetype {
        out_archetype& operator*() const;
        // 0: not indirectly_writable<simple_reference>
        void operator=(int) const requires(I != 0);
    };

    STATIC_ASSERT(!indirectly_copyable<simple_iter_archetype<0>, out_archetype<1>>);
    STATIC_ASSERT(!indirectly_copyable<simple_iter_archetype<1>, out_archetype<0>>);
    STATIC_ASSERT(indirectly_copyable<simple_iter_archetype<1>, out_archetype<1>>);
} // namespace indirectly_copyable_test

namespace indirectly_swappable_test {
    using std::indirectly_readable, std::indirectly_swappable;

    template <int I>
    struct archetype {
        using value_type = int;

        struct reference {
            operator int() const;
        };

        reference operator*() const noexcept requires(I != 0);

        friend constexpr void iter_swap(archetype const&, archetype const&) requires(I != 1) {}

        // clang-format off
        template <int J>
            requires(I != J && I != 2 && J != 2)
        friend constexpr void iter_swap(archetype const&, archetype<J> const&) {}
        // clang-format on
    };

    // We have to be careful to avoid checking some !indirectly_swappable cases that fail when the Evil Extension is
    // active (i.e., in permissive mode).
    template <bool IsPermissive>
    constexpr bool compile_only() {
        STATIC_ASSERT(!indirectly_readable<archetype<0>>); // <0> is not indirectly_readable
        STATIC_ASSERT(indirectly_readable<archetype<1>>); // <1+> is indirectly_readable
        STATIC_ASSERT(indirectly_readable<archetype<2>>);
        STATIC_ASSERT(indirectly_readable<archetype<3>>);
        STATIC_ASSERT(indirectly_readable<archetype<4>>);

        STATIC_ASSERT(!indirectly_swappable<archetype<0>, archetype<0>>); // <0> is (still) not indirectly_readable
        if constexpr (!IsPermissive) {
            STATIC_ASSERT(!indirectly_swappable<archetype<1>, archetype<1>>); // <1> is not self-indirectly_swappable
        }
        STATIC_ASSERT(indirectly_swappable<archetype<2>, archetype<2>>); // <2+> is self-indirectly_swappable
        STATIC_ASSERT(indirectly_swappable<archetype<3>, archetype<3>>);
        STATIC_ASSERT(indirectly_swappable<archetype<4>, archetype<4>>);

        STATIC_ASSERT(!indirectly_swappable<archetype<0>, archetype<4>>); // <0> is not indirectly_readable
        STATIC_ASSERT(!indirectly_swappable<archetype<4>, archetype<0>>); // <0> is not indirectly_readable
        if constexpr (!IsPermissive) {
            STATIC_ASSERT(!indirectly_swappable<archetype<1>, archetype<4>>); // <1> is not self-indirectly_swappable
            STATIC_ASSERT(!indirectly_swappable<archetype<4>, archetype<1>>); // <1> is not self-indirectly_swappable
        }
        STATIC_ASSERT(!indirectly_swappable<archetype<2>, archetype<4>>); // <2> & <4> aren't cross-indirectly_swappable
        STATIC_ASSERT(!indirectly_swappable<archetype<4>, archetype<2>>); // <2> & <4> aren't cross-indirectly_swappable

        STATIC_ASSERT(indirectly_swappable<archetype<3>, archetype<4>>);
        STATIC_ASSERT(indirectly_swappable<archetype<4>, archetype<3>>);

        return true;
    }
    STATIC_ASSERT(compile_only<is_permissive>());
} // namespace indirectly_swappable_test

namespace indirectly_comparable_test {
    // Also validate indirect_result_t
    using std::indirectly_comparable;

    using Proj    = int (&)(simple_common_reference);
    using BadProj = char const* (&) (simple_common_reference);

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
        void operator()(int, int) const requires(I == 1);
        void* operator()(int, int) const requires(I != 1);
    };

    STATIC_ASSERT(!indirectly_comparable<simple_iter_archetype<1>, simple_iter_archetype<1>, Fn<0>, Proj, Proj>);
    STATIC_ASSERT(!indirectly_comparable<simple_iter_archetype<1>, simple_iter_archetype<1>, Fn<1>, Proj, Proj>);
    STATIC_ASSERT(!indirectly_comparable<simple_iter_archetype<0>, simple_iter_archetype<1>, Fn<2>, Proj, Proj>);
    STATIC_ASSERT(!indirectly_comparable<simple_iter_archetype<1>, simple_iter_archetype<0>, Fn<2>, Proj, Proj>);
    STATIC_ASSERT(!indirectly_comparable<simple_iter_archetype<1>, simple_iter_archetype<1>, Fn<2>, BadProj, Proj>);
    STATIC_ASSERT(!indirectly_comparable<simple_iter_archetype<1>, simple_iter_archetype<1>, Fn<2>, Proj, BadProj>);
    STATIC_ASSERT(indirectly_comparable<simple_iter_archetype<1>, simple_iter_archetype<1>, Fn<2>, Proj, Proj>);

    using Projected = std::projected<simple_iter_archetype<1>, Proj>;
    STATIC_ASSERT(std::same_as<std::indirect_result_t<Fn<2>, Projected, Projected>, void*>);
} // namespace indirectly_comparable_test

namespace dangling_test {
    // Also test borrowed_iterator_t and borrowed_subrange_t
    using ranges::dangling, ranges::borrowed_iterator_t, ranges::borrowed_subrange_t;
    using std::is_nothrow_constructible_v, std::same_as;

    STATIC_ASSERT(std::is_class_v<dangling>);
    STATIC_ASSERT(std::semiregular<dangling>);
    STATIC_ASSERT(std::is_trivial_v<dangling>); // not guaranteed, but likely portable nonetheless

    // dangling is constructible from any sequence of arguments without throwing
    STATIC_ASSERT(is_nothrow_constructible_v<dangling>);
    STATIC_ASSERT(is_nothrow_constructible_v<dangling, int>);
    STATIC_ASSERT(is_nothrow_constructible_v<dangling, int*>);
    STATIC_ASSERT(is_nothrow_constructible_v<dangling, int[42]>);
    STATIC_ASSERT(is_nothrow_constructible_v<dangling, int (*)()>);
    STATIC_ASSERT(is_nothrow_constructible_v<dangling, const int (*)[42]>);
    STATIC_ASSERT(is_nothrow_constructible_v<dangling, int, int*, int[42], int (*)(), const int (*)[42]>);

    STATIC_ASSERT(same_as<borrowed_iterator_t<borrowed<false>>, dangling>);
    STATIC_ASSERT(same_as<borrowed_iterator_t<borrowed<true>>, int*>);

    STATIC_ASSERT(same_as<borrowed_subrange_t<borrowed<false>>, dangling>);
    STATIC_ASSERT(same_as<borrowed_subrange_t<borrowed<true>>, ranges::subrange<int*>>);
} // namespace dangling_test

namespace result_test {
    using ranges::in_found_result, ranges::in_fun_result, ranges::in_in_result, ranges::in_out_result,
        ranges::in_in_out_result, ranges::in_out_out_result, ranges::min_max_result;
    using std::is_aggregate_v, std::is_convertible_v, std::is_trivial_v;

    // Validate the result types are:
    // * aggregates
    STATIC_ASSERT(is_aggregate_v<in_found_result<int>>);
    STATIC_ASSERT(is_aggregate_v<in_fun_result<int, int>>);
    STATIC_ASSERT(is_aggregate_v<in_in_result<int, int>>);
    STATIC_ASSERT(is_aggregate_v<in_out_result<int, int>>);
    STATIC_ASSERT(is_aggregate_v<in_in_out_result<int, int, int>>);
    STATIC_ASSERT(is_aggregate_v<in_out_out_result<int, int, int>>);
    STATIC_ASSERT(is_aggregate_v<min_max_result<int>>);

    // * trivial when parameter types are trivial
    STATIC_ASSERT(is_trivial_v<in_found_result<int>>);
    STATIC_ASSERT(is_trivial_v<in_fun_result<int, int>>);
    STATIC_ASSERT(is_trivial_v<in_in_result<int, int>>);
    STATIC_ASSERT(is_trivial_v<in_out_result<int, int>>);
    STATIC_ASSERT(is_trivial_v<in_in_out_result<int, int, int>>);
    STATIC_ASSERT(is_trivial_v<in_out_out_result<int, int, int>>);
    STATIC_ASSERT(is_trivial_v<min_max_result<int>>);

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

    STATIC_ASSERT(test_bindings_in_found_result());
    STATIC_ASSERT(test_bindings_2<in_fun_result<int, int>>());
    STATIC_ASSERT(test_bindings_2<in_in_result<int, int>>());
    STATIC_ASSERT(test_bindings_2<in_out_result<int, int>>());
    STATIC_ASSERT(test_bindings_3<in_in_out_result<int, int, int>>());
    STATIC_ASSERT(test_bindings_3<in_out_out_result<int, int, int>>());
    STATIC_ASSERT(test_bindings_2<min_max_result<int>>());

    // * appropriately lvalue and rvalue inter-specialization convertible
    template <class T>
    struct convertible_from {
        convertible_from(std::add_rvalue_reference_t<T>);
    };

    using CFI = convertible_from<int>;

    constexpr bool test_convertible_in_found_result() {
        STATIC_ASSERT(!is_convertible_v<in_found_result<int> const&, in_found_result<CFI>>);
        STATIC_ASSERT(is_convertible_v<in_found_result<int>, in_found_result<CFI>>);
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
    STATIC_ASSERT(test_convertible_in_found_result());
    constexpr bool test_convertible_min_max_result() {
        STATIC_ASSERT(!is_convertible_v<min_max_result<int> const&, min_max_result<CFI>>);
        STATIC_ASSERT(is_convertible_v<min_max_result<int>, min_max_result<CFI>>);
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
    STATIC_ASSERT(test_convertible_min_max_result());

    template <template <class, class> class R>
    constexpr bool test_convertible_2() {
        STATIC_ASSERT(!is_convertible_v<R<int, int> const&, R<CFI, CFI>>);
        STATIC_ASSERT(is_convertible_v<R<int, int>, R<CFI, CFI>>);
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
    STATIC_ASSERT(test_convertible_2<in_fun_result>());
    STATIC_ASSERT(test_convertible_2<in_in_result>());
    STATIC_ASSERT(test_convertible_2<in_out_result>());

    template <template <class, class, class> class R>
    constexpr bool test_convertible_3() {
        STATIC_ASSERT(!is_convertible_v<R<int, int, int> const&, R<CFI, CFI, CFI>>);
        STATIC_ASSERT(is_convertible_v<R<int, int, int>, R<CFI, CFI, CFI>>);
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
    STATIC_ASSERT(test_convertible_3<in_in_out_result>());
    STATIC_ASSERT(test_convertible_3<in_out_out_result>());
} // namespace result_test
