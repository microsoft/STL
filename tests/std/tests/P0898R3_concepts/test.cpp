// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <assert.h>
#include <bitset>
#include <concepts>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <initializer_list>
#include <memory>
#include <random>
#include <stdlib.h>
#include <type_traits>
#include <utility>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <class, class = void>
constexpr bool is_trait = false;
template <class T>
constexpr bool is_trait<T, std::void_t<typename T::type>> = true;

template <class>
constexpr bool always_false = false;

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

struct IncompleteClass;
union IncompleteUnion;

template <class T>
struct DoNotInstantiate {
    STATIC_ASSERT(always_false<T>);
};

struct Immobile {
    Immobile()                = default;
    Immobile(Immobile const&) = delete;
    Immobile& operator=(Immobile const&) = delete;
};

struct MoveOnly {
    MoveOnly()           = default;
    MoveOnly(MoveOnly&&) = default;
    MoveOnly& operator=(MoveOnly&&) = default;
};

struct CopyOnlyAbomination {
    CopyOnlyAbomination()                           = default;
    CopyOnlyAbomination(CopyOnlyAbomination const&) = default;
    CopyOnlyAbomination(CopyOnlyAbomination&&)      = delete;
    CopyOnlyAbomination& operator=(CopyOnlyAbomination const&) = default;
    CopyOnlyAbomination& operator=(CopyOnlyAbomination&&) = delete;
};

struct CopyableType {
    CopyableType() = delete;
};

struct SemiregularType {};

struct RegularType {
    friend constexpr bool operator==(RegularType const&, RegularType const&) {
        return true;
    }
    friend constexpr bool operator!=(RegularType const&, RegularType const&) {
        return false;
    }
};

struct Indestructible {
    ~Indestructible() = delete;
};

struct NonTriviallyDestructible {
    ~NonTriviallyDestructible();
};

class PrivateDestructor {
public:
    PrivateDestructor(int) {}

private:
    ~PrivateDestructor() {}
};

struct ThrowingDestructor {
    ~ThrowingDestructor() noexcept(false);
};

class PurePublicDestructor {
public:
    virtual ~PurePublicDestructor() = 0;
};
class PureProtectedDestructor {
protected:
    virtual ~PureProtectedDestructor() = 0;
};
class PurePrivateDestructor {
private:
    virtual ~PurePrivateDestructor() = 0;
};

struct NotDefaultConstructible {
    NotDefaultConstructible(int) {}
};

class PublicAbstract {
public:
    virtual void meow() = 0;
};
class ProtectedAbstract {
protected:
    virtual void meow() = 0;
};
class PrivateAbstract {
private:
    virtual void meow() = 0;
};

struct ExplicitDefault {
    explicit ExplicitDefault() = default;
};

struct AggregatesExplicitDefault {
    ExplicitDefault meow;
};

struct DeletedDefault {
    DeletedDefault() = delete;
};

struct ExplicitMoveAbomination {
    ExplicitMoveAbomination()                                   = default;
    explicit ExplicitMoveAbomination(ExplicitMoveAbomination&&) = default;
};

struct ExplicitCopyAbomination {
    ExplicitCopyAbomination()                                        = default;
    ExplicitCopyAbomination(ExplicitCopyAbomination&&)               = default;
    explicit ExplicitCopyAbomination(ExplicitCopyAbomination const&) = default;
};

struct EmptyClass {};
union EmptyUnion {};

struct NotEmpty {
    virtual ~NotEmpty() = default;
};

struct BitZero {
    int : 0;
};

template <class First, class Second = IncompleteClass>
struct ConvertsFrom {
    ConvertsFrom() = default;
    constexpr ConvertsFrom(First) noexcept {}
    // clang-format off
    constexpr ConvertsFrom(Second) noexcept requires (!std::is_same_v<IncompleteClass, Second>) {}
    // clang-format on
};

template <int>
struct Interconvertible {
    Interconvertible() = default;
    template <int N>
    explicit Interconvertible(Interconvertible<N>) {}
};

struct Omniconvertible {
    template <class T>
    Omniconvertible(T&&);
};

template <class To>
struct ImplicitTo {
    operator To() const;
};
template <class To>
struct ExplicitTo {
    explicit operator To() const;
};

template <class... Ts>
struct DerivesFrom : Ts... {};

struct SimpleBase {};
using SimpleDerived = DerivesFrom<SimpleBase>;

struct MovableFriendSwap {
    friend void swap(MovableFriendSwap&, MovableFriendSwap&) {}
};
struct ImmobileFriendSwap : Immobile {
    friend void swap(ImmobileFriendSwap&, ImmobileFriendSwap&) noexcept {}
};

struct ImmobileNonMemberSwap : Immobile {};
void swap(ImmobileNonMemberSwap&, ImmobileNonMemberSwap&) {}

template <>
struct std::common_type<Interconvertible<0>, Interconvertible<1>> {
    using type = Interconvertible<2>;
};
template <>
struct std::common_type<Interconvertible<1>, Interconvertible<0>> {
    using type = Interconvertible<2>;
};

template <>
struct std::common_type<ExplicitTo<EmptyClass>, EmptyClass> {
    using type = EmptyClass;
};
template <>
struct std::common_type<EmptyClass, ExplicitTo<EmptyClass>> {
    using type = EmptyClass;
};

namespace test_same_as {
    using std::same_as;

    template <class T, class U>
    constexpr bool test() {
        STATIC_ASSERT(same_as<T, U>);
        STATIC_ASSERT(same_as<T const, U const>);
        STATIC_ASSERT(same_as<T volatile, U volatile>);
        STATIC_ASSERT(same_as<T const volatile, U const volatile>);

        constexpr bool is_ref = std::is_reference_v<T>;

        STATIC_ASSERT(same_as<T const, U> == is_ref);
        STATIC_ASSERT(same_as<T volatile, U> == is_ref);
        STATIC_ASSERT(same_as<T const volatile, U> == is_ref);
        STATIC_ASSERT(same_as<T, U const> == is_ref);
        STATIC_ASSERT(same_as<T, U volatile> == is_ref);
        STATIC_ASSERT(same_as<T, U const volatile> == is_ref);

        if constexpr (!std::is_void_v<T>) {
            STATIC_ASSERT(same_as<T&, U&>);
            STATIC_ASSERT(same_as<T const&, U&> == is_ref);
            STATIC_ASSERT(same_as<T&&, U&&>);
            STATIC_ASSERT(same_as<T const&&, U&&> == is_ref);
        }

        if constexpr (!is_ref) {
            STATIC_ASSERT(same_as<T*, U*>);
            STATIC_ASSERT(same_as<T**, U**>);
        }

        return true;
    }

    STATIC_ASSERT(test<void, void>());
    STATIC_ASSERT(test<int, int>());
    STATIC_ASSERT(!same_as<int, void>);
    STATIC_ASSERT(test<double, double>());
    STATIC_ASSERT(!same_as<double, int>);
    STATIC_ASSERT(!same_as<int, double>);

    STATIC_ASSERT(test<int[42], int[42]>());
    STATIC_ASSERT(test<int[], int[]>());

    STATIC_ASSERT(test<NonTriviallyDestructible, NonTriviallyDestructible>());
    STATIC_ASSERT(!same_as<void, NonTriviallyDestructible>);
    STATIC_ASSERT(!same_as<NonTriviallyDestructible, int*>);

    STATIC_ASSERT(test<int&, int&>());
    STATIC_ASSERT(!same_as<int*, int&>);
    STATIC_ASSERT(!same_as<int&, int>);
    STATIC_ASSERT(test<int (&)[42], int (&)[42]>());
    STATIC_ASSERT(test<int (&)(), int (&)()>());

    STATIC_ASSERT(same_as<void() const, void() const>);
    STATIC_ASSERT(same_as<void() &, void() &>);

    // clang-format off
    // Verify that `same_as<T, U>` subsumes `same_as<U, T>` (note reversed argument order)
    template <class T, class U>
        requires same_as<U, T>
    constexpr bool f() {
        return false;
    }

    template <class T, class U>
        requires same_as<T, U> && std::is_integral_v<T>
    constexpr bool f() {
        return true;
    }
    // clang-format on

    STATIC_ASSERT(!f<int*, int*>());
    STATIC_ASSERT(!f<void, void>());

    STATIC_ASSERT(f<int, int>()); // ambiguous if second overload's requirements do not subsume the first's
    STATIC_ASSERT(f<long, long>()); // Ditto
} // namespace test_same_as

namespace test_derived_from {
    using std::derived_from;

    STATIC_ASSERT(!derived_from<int, int>);
    STATIC_ASSERT(!derived_from<void, void>);
    STATIC_ASSERT(!derived_from<void(), void()>);
    STATIC_ASSERT(!derived_from<void() const, void() const>);
    STATIC_ASSERT(!derived_from<int[42], int[42]>);
    STATIC_ASSERT(!derived_from<int&, int>);
    STATIC_ASSERT(!derived_from<int, int&>);
    STATIC_ASSERT(!derived_from<int&, int&>);

    template <class Derived, class Base>
    constexpr bool test() {
        STATIC_ASSERT(derived_from<Derived, Base>);
        STATIC_ASSERT(derived_from<Derived const, Base>);
        STATIC_ASSERT(derived_from<Derived volatile, Base>);
        STATIC_ASSERT(derived_from<Derived const volatile, Base>);
        STATIC_ASSERT(derived_from<Derived, Base const>);
        STATIC_ASSERT(derived_from<Derived const, Base const>);
        STATIC_ASSERT(derived_from<Derived volatile, Base const>);
        STATIC_ASSERT(derived_from<Derived const volatile, Base const>);
        STATIC_ASSERT(derived_from<Derived, Base volatile>);
        STATIC_ASSERT(derived_from<Derived const, Base volatile>);
        STATIC_ASSERT(derived_from<Derived volatile, Base volatile>);
        STATIC_ASSERT(derived_from<Derived const volatile, Base volatile>);
        STATIC_ASSERT(derived_from<Derived, Base const volatile>);
        STATIC_ASSERT(derived_from<Derived const, Base const volatile>);
        STATIC_ASSERT(derived_from<Derived volatile, Base const volatile>);
        STATIC_ASSERT(derived_from<Derived const volatile, Base const volatile>);
        return true;
    }

    STATIC_ASSERT(test<SimpleBase, SimpleBase>());
    STATIC_ASSERT(!derived_from<SimpleBase&, SimpleBase&>);

    template <int>
    struct Middle : SimpleBase {};

    STATIC_ASSERT(test<Middle<0>, SimpleBase>());
    STATIC_ASSERT(!derived_from<SimpleBase, Middle<0>>);
    STATIC_ASSERT(!derived_from<Middle<0>&, SimpleBase&>);
    STATIC_ASSERT(!derived_from<Middle<0>*, SimpleBase*>);
    STATIC_ASSERT(!derived_from<Middle<0>[42], SimpleBase[42]>);

    STATIC_ASSERT(test<Middle<1>, SimpleBase>());
    STATIC_ASSERT(!derived_from<SimpleBase, Middle<1>>);

    STATIC_ASSERT(test<DerivesFrom<Middle<0>, Middle<1>>, Middle<0>>());
    STATIC_ASSERT(test<DerivesFrom<Middle<0>, Middle<1>>, Middle<1>>());
    STATIC_ASSERT(!derived_from<DerivesFrom<Middle<0>, Middle<1>>, SimpleBase>);

    class PrivateDerived : private Middle<0>, private Middle<1> {
    public:
        void f();
    };

    void PrivateDerived::f() {
        // Check these in a member to verify that access doesn't depend on context
        STATIC_ASSERT(!derived_from<PrivateDerived, Middle<0>>);
        STATIC_ASSERT(!derived_from<PrivateDerived, Middle<1>>);
    }

    STATIC_ASSERT(!derived_from<PrivateDerived, SimpleBase>);

    STATIC_ASSERT(test<IncompleteClass, IncompleteClass>());

    STATIC_ASSERT(!derived_from<IncompleteUnion, IncompleteUnion>);
    STATIC_ASSERT(!derived_from<IncompleteClass, IncompleteUnion>);
    STATIC_ASSERT(!derived_from<IncompleteUnion, IncompleteClass>);
} // namespace test_derived_from

namespace test_convertible_to {
    using std::convertible_to;

    using char_array = char[42];
    using fn         = void();
    using const_fn   = void() const;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class From, class To>
    constexpr bool test() {
        constexpr bool result = convertible_to<From, To>;
        STATIC_ASSERT(convertible_to<From const, To> == result);
        STATIC_ASSERT(convertible_to<From, To const> == result);
        STATIC_ASSERT(convertible_to<From const, To const> == result);
        return result;
    }
#pragma warning(pop)

    // void
    STATIC_ASSERT(test<void, void>());
    STATIC_ASSERT(!test<void, fn>());
    STATIC_ASSERT(!test<void, fn&>());
    STATIC_ASSERT(!test<void, fn*>());
    STATIC_ASSERT(!test<void, char_array>());
    STATIC_ASSERT(!test<void, char_array&>());
    STATIC_ASSERT(!test<void, char>());
    STATIC_ASSERT(!test<void, char&>());
    STATIC_ASSERT(!test<void, char*>());
    STATIC_ASSERT(!test<char, void>());

    // fn
    STATIC_ASSERT(!test<fn, void>());
    STATIC_ASSERT(!test<fn, fn>());
    STATIC_ASSERT(test<fn, fn&>());
    STATIC_ASSERT(test<fn, fn*>());
    STATIC_ASSERT(test<fn, fn* const>());

    STATIC_ASSERT(convertible_to<fn, fn&&>);

    STATIC_ASSERT(!test<fn, char_array>());
    STATIC_ASSERT(!test<fn, char_array&>());
    STATIC_ASSERT(!test<fn, char>());
    STATIC_ASSERT(!test<fn, char&>());
    STATIC_ASSERT(!test<fn, char*>());

    // fn&
    STATIC_ASSERT(!test<fn&, void>());
    STATIC_ASSERT(!test<fn&, fn>());
    STATIC_ASSERT(test<fn&, fn&>());

    STATIC_ASSERT(test<fn&, fn*>());
    STATIC_ASSERT(!test<fn&, char_array>());
    STATIC_ASSERT(!test<fn&, char_array&>());
    STATIC_ASSERT(!test<fn&, char>());
    STATIC_ASSERT(!test<fn&, char&>());
    STATIC_ASSERT(!test<fn&, char*>());

    // fn*
    STATIC_ASSERT(!test<fn*, void>());
    STATIC_ASSERT(!test<fn*, fn>());
    STATIC_ASSERT(!test<fn*, fn&>());
    STATIC_ASSERT(test<fn*, fn*>());

    STATIC_ASSERT(!test<fn*, char_array>());
    STATIC_ASSERT(!test<fn*, char_array&>());
    STATIC_ASSERT(!test<fn*, char>());
    STATIC_ASSERT(!test<fn*, char&>());
    STATIC_ASSERT(!test<fn*, char*>());

    // Abominable function
    STATIC_ASSERT(!convertible_to<const_fn, fn>);
    STATIC_ASSERT(!convertible_to<const_fn, fn*>);
    STATIC_ASSERT(!convertible_to<const_fn, fn&>);
    STATIC_ASSERT(!convertible_to<const_fn, fn&&>);
    STATIC_ASSERT(!convertible_to<fn*, const_fn>);
    STATIC_ASSERT(!convertible_to<fn&, const_fn>);
    STATIC_ASSERT(!convertible_to<const_fn, const_fn>);
    STATIC_ASSERT(!convertible_to<const_fn, void>);

    // char_array
    STATIC_ASSERT(!test<char_array, void>());
    STATIC_ASSERT(!test<char_array, fn>());
    STATIC_ASSERT(!test<char_array, fn&>());
    STATIC_ASSERT(!test<char_array, fn*>());
    STATIC_ASSERT(!test<char_array, char_array>());

    STATIC_ASSERT(!convertible_to<char_array, char_array&>);
    STATIC_ASSERT(convertible_to<char_array, char_array const&>);
    STATIC_ASSERT(!convertible_to<char_array, char_array const volatile&>);

    STATIC_ASSERT(!convertible_to<char_array const, char_array&>);
    STATIC_ASSERT(convertible_to<char_array const, char_array const&>);
    STATIC_ASSERT(!convertible_to<char_array, char_array volatile&>);
    STATIC_ASSERT(!convertible_to<char_array, char_array const volatile&>);

    STATIC_ASSERT(convertible_to<char_array, char_array&&>);
    STATIC_ASSERT(convertible_to<char_array, char_array const&&>);
    STATIC_ASSERT(convertible_to<char_array, char_array volatile&&>);
    STATIC_ASSERT(convertible_to<char_array, char_array const volatile&&>);
    STATIC_ASSERT(convertible_to<char_array const, char_array const&&>);
    STATIC_ASSERT(!convertible_to<char_array&, char_array&&>);
    STATIC_ASSERT(!convertible_to<char_array&&, char_array&>);

    STATIC_ASSERT(!test<char_array, char>());
    STATIC_ASSERT(!test<char_array, char&>());

    STATIC_ASSERT(convertible_to<char_array, char*>);
    STATIC_ASSERT(convertible_to<char_array, char const*>);
    STATIC_ASSERT(convertible_to<char_array, char* const>);
    STATIC_ASSERT(convertible_to<char_array, char* const volatile>);

    STATIC_ASSERT(!convertible_to<char_array const, char*>);
    STATIC_ASSERT(convertible_to<char_array const, char const*>);

    STATIC_ASSERT(!convertible_to<char[42][42], char*>);
    STATIC_ASSERT(!convertible_to<char[][1], char*>);

    // char_array&
    STATIC_ASSERT(!test<char_array&, void>());
    STATIC_ASSERT(!test<char_array&, fn>());
    STATIC_ASSERT(!test<char_array&, fn&>());
    STATIC_ASSERT(!test<char_array&, fn*>());
    STATIC_ASSERT(!test<char_array&, char_array>());

    STATIC_ASSERT(convertible_to<char_array&, char_array&>);
    STATIC_ASSERT(convertible_to<char_array&, char_array const&>);
    STATIC_ASSERT(!convertible_to<char_array const&, char_array&>);
    STATIC_ASSERT(convertible_to<char_array const&, char_array const&>);

    STATIC_ASSERT(!test<char_array&, char>());
    STATIC_ASSERT(!test<char_array&, char&>());

    STATIC_ASSERT(convertible_to<char_array&, char*>);
    STATIC_ASSERT(convertible_to<char_array&, char const*>);
    STATIC_ASSERT(!convertible_to<char_array const&, char*>);
    STATIC_ASSERT(convertible_to<char_array const&, char const*>);

    STATIC_ASSERT(convertible_to<char_array, ConvertsFrom<char const*>>);
    STATIC_ASSERT(convertible_to<char (&)[], ConvertsFrom<char const*>>);

    // char
    STATIC_ASSERT(!test<char, void>());
    STATIC_ASSERT(!test<char, fn>());
    STATIC_ASSERT(!test<char, fn&>());
    STATIC_ASSERT(!test<char, fn*>());
    STATIC_ASSERT(!test<char, char_array>());
    STATIC_ASSERT(!test<char, char_array&>());

    STATIC_ASSERT(test<char, char>());

    STATIC_ASSERT(!convertible_to<char, char&>);
    STATIC_ASSERT(convertible_to<char, char const&>);
    STATIC_ASSERT(!convertible_to<char const, char&>);
    STATIC_ASSERT(convertible_to<char const, char const&>);

    STATIC_ASSERT(!test<char, char*>());

    // char&
    STATIC_ASSERT(!test<char&, void>());
    STATIC_ASSERT(!test<char&, fn>());
    STATIC_ASSERT(!test<char&, fn&>());
    STATIC_ASSERT(!test<char&, fn*>());
    STATIC_ASSERT(!test<char&, char_array>());
    STATIC_ASSERT(!test<char&, char_array&>());

    STATIC_ASSERT(test<char&, char>());

    STATIC_ASSERT(convertible_to<char&, char&>);
    STATIC_ASSERT(convertible_to<char&, char const&>);
    STATIC_ASSERT(!convertible_to<char const&, char&>);
    STATIC_ASSERT(convertible_to<char const&, char const&>);

    STATIC_ASSERT(!test<char&, char*>());

    // char*
    STATIC_ASSERT(!test<char*, void>());
    STATIC_ASSERT(!test<char*, fn>());
    STATIC_ASSERT(!test<char*, fn&>());
    STATIC_ASSERT(!test<char*, fn*>());
    STATIC_ASSERT(!test<char*, char_array>());
    STATIC_ASSERT(!test<char*, char_array&>());

    STATIC_ASSERT(!test<char*, char>());
    STATIC_ASSERT(!test<char*, char&>());

    STATIC_ASSERT(convertible_to<char*, char*>);
    STATIC_ASSERT(convertible_to<char*, char const*>);
    STATIC_ASSERT(!convertible_to<char const*, char*>);
    STATIC_ASSERT(convertible_to<char const*, char const*>);

    STATIC_ASSERT(convertible_to<Immobile&, Immobile&>);
    STATIC_ASSERT(convertible_to<Immobile&, Immobile const&>);
    STATIC_ASSERT(convertible_to<Immobile&, Immobile const volatile&>);
    STATIC_ASSERT(convertible_to<Immobile&, Immobile volatile&>);
    STATIC_ASSERT(convertible_to<Immobile const&, Immobile const&>);
    STATIC_ASSERT(convertible_to<Immobile const&, Immobile const volatile&>);
    STATIC_ASSERT(convertible_to<Immobile volatile&, Immobile const volatile&>);
    STATIC_ASSERT(convertible_to<Immobile const volatile&, Immobile const volatile&>);
    STATIC_ASSERT(!convertible_to<Immobile const&, Immobile&>);

    STATIC_ASSERT(!test<Immobile&, Immobile>());
    STATIC_ASSERT(!test<Immobile const&, Immobile>());
    STATIC_ASSERT(!test<Immobile, Immobile>());

    // Ensure that DoNotInstantiate is not instantiated by is_convertible when it is not needed.
    // (For example, DoNotInstantiate is instantiated by ADL lookup for arguments of type DoNotInstantiate*.)
    STATIC_ASSERT(convertible_to<DoNotInstantiate<int>*, DoNotInstantiate<int>*>);

    STATIC_ASSERT(test<SimpleBase, SimpleBase>());
    STATIC_ASSERT(test<SimpleDerived, SimpleDerived>());
    STATIC_ASSERT(test<SimpleDerived, SimpleBase>());
    STATIC_ASSERT(!test<SimpleBase, SimpleDerived>());

    struct ImplicitConversionOnly;
    struct Target {
        Target()                                = default;
        explicit Target(ImplicitConversionOnly) = delete;
    };
    struct ImplicitConversionOnly {
        operator Target() const;
    };
    STATIC_ASSERT(test<ImplicitTo<Target>, Target>());
    STATIC_ASSERT(!test<ExplicitTo<Target>, Target>());
    STATIC_ASSERT(!test<ImplicitConversionOnly, Target>());

    namespace overloading { // a test of overload resolution moreso than the concept itself
        enum class result { exact, convertible, unrelated };

        constexpr result f(ConvertsFrom<int>) {
            return result::exact;
        }

        template <convertible_to<ConvertsFrom<int>> T>
        constexpr result f(T&&) {
            return result::convertible;
        }

        template <class T>
        constexpr result f(T&&) {
            return result::unrelated;
        }

        STATIC_ASSERT(f(ConvertsFrom<int>{}) == result::exact); // the most specialized overload is chosen
        constexpr ConvertsFrom<int> cfi{};
        STATIC_ASSERT(f(cfi) == result::exact); // Ditto with qualification conversion
        STATIC_ASSERT(f(42) == result::convertible); // the more-constrained overload is chosen
        STATIC_ASSERT(f("meow") == result::unrelated); // everything isn't completely broken
    } // namespace overloading
} // namespace test_convertible_to

namespace test_common_reference_with {
    using std::common_reference_t, std::common_reference_with;

    template <class T, class U>
    constexpr bool test() {
        constexpr bool result = common_reference_with<T, U>;
        STATIC_ASSERT(common_reference_with<U, T> == result);
        return result;
    }

    STATIC_ASSERT(test<int, int>());
    STATIC_ASSERT(test<int, double>());
    STATIC_ASSERT(test<double, int>());
    STATIC_ASSERT(test<double, double>());
    STATIC_ASSERT(!test<void, int>());
    STATIC_ASSERT(!test<int*, int>());
    STATIC_ASSERT(test<void*, int*>());
    STATIC_ASSERT(test<double, long long>());
    STATIC_ASSERT(test<void, void>());

    // common_reference_t<<ExplicitTo<EmptyClass>, EmptyClass> is EmptyClass, to which ExplicitTo<EmptyClass> is
    // not implicitly convertible
    STATIC_ASSERT(!test<ExplicitTo<EmptyClass>, EmptyClass>());

    STATIC_ASSERT(test<Immobile&, DerivesFrom<Immobile>&>());
    STATIC_ASSERT(test<Immobile&&, DerivesFrom<Immobile>&&>());
    STATIC_ASSERT(!test<Immobile, DerivesFrom<Immobile>>());

    STATIC_ASSERT(test<SimpleBase&, SimpleDerived&>());
    STATIC_ASSERT(test<SimpleBase&, SimpleDerived const&>());
    STATIC_ASSERT(test<SimpleBase const&, SimpleDerived&>());
    STATIC_ASSERT(test<SimpleBase const&, SimpleDerived const&>());

    STATIC_ASSERT(test<SimpleBase&&, SimpleDerived&&>());
    STATIC_ASSERT(test<SimpleBase&&, SimpleDerived const&&>());
    STATIC_ASSERT(test<SimpleBase const&&, SimpleDerived&&>());
    STATIC_ASSERT(test<SimpleBase const&&, SimpleDerived const&&>());

    STATIC_ASSERT(test<SimpleBase&, SimpleDerived&&>());
    STATIC_ASSERT(test<SimpleBase&, SimpleDerived const&&>());
    STATIC_ASSERT(test<SimpleBase const&, SimpleDerived&&>());
    STATIC_ASSERT(test<SimpleBase const&, SimpleDerived const&&>());

    STATIC_ASSERT(test<SimpleBase&&, SimpleDerived&>());
    STATIC_ASSERT(test<SimpleBase&&, SimpleDerived const&>());
    STATIC_ASSERT(test<SimpleBase const&&, SimpleDerived&>());
    STATIC_ASSERT(test<SimpleBase const&&, SimpleDerived const&>());

    // https://github.com/ericniebler/stl2/issues/338
    STATIC_ASSERT(test<int&, ConvertsFrom<int&>>());

    STATIC_ASSERT(!test<MoveOnly const&, MoveOnly>());
    STATIC_ASSERT(test<MoveOnly const&, MoveOnly&>());
    STATIC_ASSERT(!test<DerivesFrom<MoveOnly> const&, MoveOnly>());
    STATIC_ASSERT(test<DerivesFrom<MoveOnly> const&, MoveOnly&&>());
    STATIC_ASSERT(!test<MoveOnly const&, DerivesFrom<MoveOnly>>());
    STATIC_ASSERT(test<MoveOnly const&, DerivesFrom<MoveOnly> const&>());

    STATIC_ASSERT(
        std::is_same_v<Interconvertible<2>, common_reference_t<Interconvertible<0>&, Interconvertible<1> const&>>);
    STATIC_ASSERT(!test<Interconvertible<0>&, Interconvertible<1> const&>());

    STATIC_ASSERT(test<SimpleBase, ConvertsFrom<int, SimpleBase>>());
} // namespace test_common_reference_with

namespace test_common_with {
    template <class T>
    struct ExplicitFromConstLvalue {
        explicit ExplicitFromConstLvalue(T const&) {}
    };
} // namespace test_common_with

template <class T>
struct std::common_type<T, test_common_with::ExplicitFromConstLvalue<T>> {
    using type = test_common_with::ExplicitFromConstLvalue<T>;
};
template <class T>
struct std::common_type<test_common_with::ExplicitFromConstLvalue<T>, T> {
    using type = test_common_with::ExplicitFromConstLvalue<T>;
};

namespace test_common_with {
    using std::common_with, std::common_type_t, std::is_same_v;

    template <class T, class U>
    constexpr bool test() {
        constexpr bool result = common_with<T, U>;
        STATIC_ASSERT(common_with<U, T> == result);
        return result;
    }

    STATIC_ASSERT(test<int, double>());
    STATIC_ASSERT(test<double, int>());
    STATIC_ASSERT(test<double, double>());
    STATIC_ASSERT(test<void*, int*>());
    STATIC_ASSERT(test<double, long long>());

    STATIC_ASSERT(test<int, int>());
    STATIC_ASSERT(test<int, int const>());
    STATIC_ASSERT(test<int const, int const>());

    STATIC_ASSERT(test<long, int const>());
    STATIC_ASSERT(test<long const, int>());
    STATIC_ASSERT(test<long, int volatile>());
    STATIC_ASSERT(test<long volatile, int>());
    STATIC_ASSERT(test<long const, int const>());

    STATIC_ASSERT(test<double, char>());
    STATIC_ASSERT(test<short, char>());

    STATIC_ASSERT(test<void, void>());
    STATIC_ASSERT(test<void, void const>());
    STATIC_ASSERT(test<void const, void>());
    STATIC_ASSERT(test<void, void volatile>());
    STATIC_ASSERT(test<void volatile, void>());
    STATIC_ASSERT(test<void const, void const>());

    STATIC_ASSERT(!test<void, int>());
    STATIC_ASSERT(!test<int*, int>());

    STATIC_ASSERT(test<std::reference_wrapper<int>, int>());

    STATIC_ASSERT(test<ExplicitFromConstLvalue<int>, ExplicitFromConstLvalue<int>>());
    STATIC_ASSERT(!test<int, ExplicitFromConstLvalue<int>>());

    STATIC_ASSERT(test<void*, IncompleteClass*>());
    STATIC_ASSERT(test<void*, DoNotInstantiate<int>*>());

    STATIC_ASSERT(is_same_v<common_type_t<ExplicitTo<EmptyClass>, EmptyClass>, EmptyClass>);
    STATIC_ASSERT(is_same_v<common_type_t<EmptyClass, ExplicitTo<EmptyClass>>, EmptyClass>);
    STATIC_ASSERT(!test<ExplicitTo<EmptyClass>, EmptyClass>());

    STATIC_ASSERT(test<SimpleBase, SimpleDerived>());
    STATIC_ASSERT(test<SimpleBase, ConvertsFrom<SimpleBase, int>>());

    STATIC_ASSERT(is_same_v<common_type_t<Immobile, DerivesFrom<Immobile>>, Immobile>);
    STATIC_ASSERT(is_same_v<common_type_t<DerivesFrom<Immobile>, Immobile>, Immobile>);
    STATIC_ASSERT(!test<Immobile, DerivesFrom<Immobile>>());
} // namespace test_common_with

namespace test_integral_concepts {
    using std::integral, std::signed_integral, std::unsigned_integral;

    namespace subsumption {
        enum class is { not_integral, integral, signed_integral, unsigned_integral, ull };

        template <class T>
        constexpr is f(T&&) {
            return is::not_integral;
        }
        template <integral T>
        constexpr is f(T) {
            return is::integral;
        }
        template <signed_integral T>
        constexpr is f(T) {
            return is::signed_integral;
        }
        template <unsigned_integral T>
        constexpr is f(T) {
            return is::unsigned_integral;
        }
        constexpr is f(unsigned long long) {
            return is::ull;
        }

        constexpr bool test_subsumption() {
            STATIC_ASSERT(f(0.0f) == is::not_integral);
            STATIC_ASSERT(f(0.0) == is::not_integral);
            STATIC_ASSERT(f(0) == is::signed_integral);
            STATIC_ASSERT(f(0u) == is::unsigned_integral);
            STATIC_ASSERT(f('a') == is::signed_integral || f('a') == is::unsigned_integral);
            STATIC_ASSERT(f(nullptr) == is::not_integral);
            STATIC_ASSERT(f(0ull) == is::ull);
            STATIC_ASSERT(f(static_cast<int*>(nullptr)) == is::not_integral);
            struct A {};
            STATIC_ASSERT(f(static_cast<int A::*>(nullptr)) == is::not_integral);
            STATIC_ASSERT(f(static_cast<int (A::*)()>(nullptr)) == is::not_integral);
            return true;
        }
        STATIC_ASSERT(test_subsumption());
    } // namespace subsumption

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    enum class is_signed { no, yes, NA };

    template <class T, is_signed S>
    constexpr bool test_integral() {
        constexpr bool result = integral<T>;
        STATIC_ASSERT(integral<T const> == result);
        STATIC_ASSERT(integral<T volatile> == result);
        STATIC_ASSERT(integral<T const volatile> == result);
        if constexpr (!result) {
            STATIC_ASSERT(S == is_signed::NA);
            STATIC_ASSERT(!signed_integral<T>);
            STATIC_ASSERT(!signed_integral<T const>);
            STATIC_ASSERT(!signed_integral<T volatile>);
            STATIC_ASSERT(!signed_integral<T const volatile>);
            STATIC_ASSERT(!unsigned_integral<T>);
            STATIC_ASSERT(!unsigned_integral<T const>);
            STATIC_ASSERT(!unsigned_integral<T volatile>);
            STATIC_ASSERT(!unsigned_integral<T const volatile>);
        } else if constexpr (S == is_signed::yes) {
            STATIC_ASSERT(signed_integral<T>);
            STATIC_ASSERT(signed_integral<T const>);
            STATIC_ASSERT(signed_integral<T volatile>);
            STATIC_ASSERT(signed_integral<T const volatile>);
            STATIC_ASSERT(!unsigned_integral<T>);
            STATIC_ASSERT(!unsigned_integral<T const>);
            STATIC_ASSERT(!unsigned_integral<T volatile>);
            STATIC_ASSERT(!unsigned_integral<T const volatile>);
        } else {
            STATIC_ASSERT(!signed_integral<T>);
            STATIC_ASSERT(!signed_integral<T const>);
            STATIC_ASSERT(!signed_integral<T volatile>);
            STATIC_ASSERT(!signed_integral<T const volatile>);
            STATIC_ASSERT(unsigned_integral<T>);
            STATIC_ASSERT(unsigned_integral<T const>);
            STATIC_ASSERT(unsigned_integral<T volatile>);
            STATIC_ASSERT(unsigned_integral<T const volatile>);
        }
        return result;
    }
#pragma warning(pop)

    STATIC_ASSERT(test_integral<signed char, is_signed::yes>());
    STATIC_ASSERT(test_integral<short, is_signed::yes>());
    STATIC_ASSERT(test_integral<int, is_signed::yes>());
    STATIC_ASSERT(test_integral<long, is_signed::yes>());
    STATIC_ASSERT(test_integral<long long, is_signed::yes>());

    STATIC_ASSERT(test_integral<unsigned char, is_signed::no>());
    STATIC_ASSERT(test_integral<unsigned short, is_signed::no>());
    STATIC_ASSERT(test_integral<unsigned int, is_signed::no>());
    STATIC_ASSERT(test_integral<unsigned long, is_signed::no>());
    STATIC_ASSERT(test_integral<unsigned long long, is_signed::no>());

    STATIC_ASSERT(test_integral<bool, is_signed::no>());
    STATIC_ASSERT(test_integral<char, is_signed{std::is_signed_v<char>}>());
    STATIC_ASSERT(test_integral<wchar_t, is_signed{std::is_signed_v<wchar_t>}>());
    STATIC_ASSERT(test_integral<char16_t, is_signed::no>());
    STATIC_ASSERT(test_integral<char32_t, is_signed::no>());

    STATIC_ASSERT(!test_integral<float, is_signed::NA>());
    STATIC_ASSERT(!test_integral<double, is_signed::NA>());
    STATIC_ASSERT(!test_integral<long double, is_signed::NA>());

    STATIC_ASSERT(!test_integral<void, is_signed::NA>());
    STATIC_ASSERT(!test_integral<std::nullptr_t, is_signed::NA>());

    STATIC_ASSERT(!test_integral<int[42], is_signed::NA>());
    STATIC_ASSERT(!test_integral<int (&)[42], is_signed::NA>());
    STATIC_ASSERT(!test_integral<int(int), is_signed::NA>());
    STATIC_ASSERT(!test_integral<int*, is_signed::NA>());
    STATIC_ASSERT(!test_integral<int&, is_signed::NA>());

    STATIC_ASSERT(!test_integral<IncompleteClass, is_signed::NA>());
    STATIC_ASSERT(!test_integral<IncompleteUnion, is_signed::NA>());
    STATIC_ASSERT(!test_integral<DoNotInstantiate<int>, is_signed::NA>());
    STATIC_ASSERT(!test_integral<void (IncompleteClass::*)(), is_signed::NA>());

    enum int_enum : int {};
    STATIC_ASSERT(!test_integral<int_enum, is_signed::NA>());

    STATIC_ASSERT(test_integral<std::ptrdiff_t, is_signed::yes>());
    STATIC_ASSERT(test_integral<std::size_t, is_signed::no>());
} // namespace test_integral_concepts

namespace test_floating_point {
    using std::floating_point;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T>
    constexpr bool test_floating_point() {
        constexpr bool result = floating_point<T>;
        STATIC_ASSERT(floating_point<T const> == result);
        STATIC_ASSERT(floating_point<T volatile> == result);
        STATIC_ASSERT(floating_point<T const volatile> == result);
        return result;
    }
#pragma warning(pop)

    STATIC_ASSERT(!test_floating_point<signed char>());
    STATIC_ASSERT(!test_floating_point<short>());
    STATIC_ASSERT(!test_floating_point<int>());
    STATIC_ASSERT(!test_floating_point<long>());
    STATIC_ASSERT(!test_floating_point<long long>());

    STATIC_ASSERT(!test_floating_point<unsigned char>());
    STATIC_ASSERT(!test_floating_point<unsigned short>());
    STATIC_ASSERT(!test_floating_point<unsigned int>());
    STATIC_ASSERT(!test_floating_point<unsigned long>());
    STATIC_ASSERT(!test_floating_point<unsigned long long>());

    STATIC_ASSERT(!test_floating_point<bool>());
    STATIC_ASSERT(!test_floating_point<char>());
    STATIC_ASSERT(!test_floating_point<wchar_t>());
    STATIC_ASSERT(!test_floating_point<char16_t>());
    STATIC_ASSERT(!test_floating_point<char32_t>());

    STATIC_ASSERT(test_floating_point<float>());
    STATIC_ASSERT(test_floating_point<double>());
    STATIC_ASSERT(test_floating_point<long double>());

    STATIC_ASSERT(!test_floating_point<void>());
    STATIC_ASSERT(!test_floating_point<std::nullptr_t>());

    STATIC_ASSERT(!test_floating_point<double[42]>());
    STATIC_ASSERT(!test_floating_point<double (&)[42]>());
    STATIC_ASSERT(!test_floating_point<double(double)>());
    STATIC_ASSERT(!test_floating_point<double*>());
    STATIC_ASSERT(!test_floating_point<double&>());

    STATIC_ASSERT(!test_floating_point<IncompleteClass>());
    STATIC_ASSERT(!test_floating_point<IncompleteUnion>());
    STATIC_ASSERT(!test_floating_point<DoNotInstantiate<int>>());
    STATIC_ASSERT(!test_floating_point<void (IncompleteClass::*)()>());

    enum int_enum : int {};
    STATIC_ASSERT(!test_floating_point<int_enum>());

    STATIC_ASSERT(!test_floating_point<std::ptrdiff_t>());
    STATIC_ASSERT(!test_floating_point<std::size_t>());
} // namespace test_floating_point

namespace test_assignable_from {
    using std::assignable_from;

    STATIC_ASSERT(!assignable_from<int, int>);
    STATIC_ASSERT(!assignable_from<int, int&>);
    STATIC_ASSERT(!assignable_from<int, int&&>);
    STATIC_ASSERT(!assignable_from<int, int const>);
    STATIC_ASSERT(!assignable_from<int, int const&>);
    STATIC_ASSERT(!assignable_from<int, int const&&>);

    STATIC_ASSERT(assignable_from<int&, int>);
    STATIC_ASSERT(assignable_from<int&, int&>);
    STATIC_ASSERT(assignable_from<int&, int&&>);
    STATIC_ASSERT(assignable_from<int&, int const>);
    STATIC_ASSERT(assignable_from<int&, int const&>);
    STATIC_ASSERT(assignable_from<int&, int const&&>);

    STATIC_ASSERT(!assignable_from<int&&, int>);
    STATIC_ASSERT(!assignable_from<int&&, int&>);
    STATIC_ASSERT(!assignable_from<int&&, int&&>);
    STATIC_ASSERT(!assignable_from<int&&, int const>);
    STATIC_ASSERT(!assignable_from<int&&, int const&>);
    STATIC_ASSERT(!assignable_from<int&&, int const&&>);

    STATIC_ASSERT(assignable_from<int&, double>);
    STATIC_ASSERT(assignable_from<void*&, void*>);

    STATIC_ASSERT(!assignable_from<EmptyClass, EmptyClass>);
    STATIC_ASSERT(!assignable_from<EmptyClass, EmptyClass&>);
    STATIC_ASSERT(!assignable_from<EmptyClass, EmptyClass&&>);
    STATIC_ASSERT(!assignable_from<EmptyClass, EmptyClass const>);
    STATIC_ASSERT(!assignable_from<EmptyClass, EmptyClass const&>);
    STATIC_ASSERT(!assignable_from<EmptyClass, EmptyClass const&&>);

    STATIC_ASSERT(assignable_from<EmptyClass&, EmptyClass>);
    STATIC_ASSERT(assignable_from<EmptyClass&, EmptyClass&>);
    STATIC_ASSERT(assignable_from<EmptyClass&, EmptyClass&&>);
    STATIC_ASSERT(assignable_from<EmptyClass&, EmptyClass const>);
    STATIC_ASSERT(assignable_from<EmptyClass&, EmptyClass const&>);
    STATIC_ASSERT(assignable_from<EmptyClass&, EmptyClass const&&>);

    STATIC_ASSERT(!assignable_from<EmptyClass&&, EmptyClass>);
    STATIC_ASSERT(!assignable_from<EmptyClass&&, EmptyClass&>);
    STATIC_ASSERT(!assignable_from<EmptyClass&&, EmptyClass&&>);
    STATIC_ASSERT(!assignable_from<EmptyClass&&, EmptyClass const>);
    STATIC_ASSERT(!assignable_from<EmptyClass&&, EmptyClass const&>);
    STATIC_ASSERT(!assignable_from<EmptyClass&&, EmptyClass const&&>);

    // assignment operator exists, but no common_reference_with
    struct AssignButUncommon {
        AssignButUncommon& operator=(EmptyClass const&);
    };
    STATIC_ASSERT(!assignable_from<AssignButUncommon, EmptyClass>);
    STATIC_ASSERT(!assignable_from<AssignButUncommon, EmptyClass&>);
    STATIC_ASSERT(!assignable_from<AssignButUncommon, EmptyClass&&>);
    STATIC_ASSERT(!assignable_from<AssignButUncommon, EmptyClass const>);
    STATIC_ASSERT(!assignable_from<AssignButUncommon, EmptyClass const&>);
    STATIC_ASSERT(!assignable_from<AssignButUncommon, EmptyClass const&&>);

    STATIC_ASSERT(!assignable_from<AssignButUncommon&, EmptyClass>);
    STATIC_ASSERT(!assignable_from<AssignButUncommon&, EmptyClass&>);
    STATIC_ASSERT(!assignable_from<AssignButUncommon&, EmptyClass&&>);
    STATIC_ASSERT(!assignable_from<AssignButUncommon&, EmptyClass const>);
    STATIC_ASSERT(!assignable_from<AssignButUncommon&, EmptyClass const&>);
    STATIC_ASSERT(!assignable_from<AssignButUncommon&, EmptyClass const&&>);

    STATIC_ASSERT(!assignable_from<AssignButUncommon&&, EmptyClass>);
    STATIC_ASSERT(!assignable_from<AssignButUncommon&&, EmptyClass&>);
    STATIC_ASSERT(!assignable_from<AssignButUncommon&&, EmptyClass&&>);
    STATIC_ASSERT(!assignable_from<AssignButUncommon&&, EmptyClass const>);
    STATIC_ASSERT(!assignable_from<AssignButUncommon&&, EmptyClass const&>);
    STATIC_ASSERT(!assignable_from<AssignButUncommon&&, EmptyClass const&&>);

    // assignment operator exists, with common_reference_with
    struct AssignAndCommon {
        AssignAndCommon(EmptyClass const&);
        AssignAndCommon& operator=(EmptyClass const&) &;
    };
    STATIC_ASSERT(!assignable_from<AssignAndCommon, EmptyClass>);
    STATIC_ASSERT(!assignable_from<AssignAndCommon, EmptyClass&>);
    STATIC_ASSERT(!assignable_from<AssignAndCommon, EmptyClass&&>);
    STATIC_ASSERT(!assignable_from<AssignAndCommon, EmptyClass const>);
    STATIC_ASSERT(!assignable_from<AssignAndCommon, EmptyClass const&>);
    STATIC_ASSERT(!assignable_from<AssignAndCommon, EmptyClass const&&>);

    STATIC_ASSERT(assignable_from<AssignAndCommon&, EmptyClass>);
    STATIC_ASSERT(assignable_from<AssignAndCommon&, EmptyClass&>);
    STATIC_ASSERT(assignable_from<AssignAndCommon&, EmptyClass&&>);
    STATIC_ASSERT(assignable_from<AssignAndCommon&, EmptyClass const>);
    STATIC_ASSERT(assignable_from<AssignAndCommon&, EmptyClass const&>);
    STATIC_ASSERT(assignable_from<AssignAndCommon&, EmptyClass const&&>);

    STATIC_ASSERT(!assignable_from<AssignAndCommon&&, EmptyClass>);
    STATIC_ASSERT(!assignable_from<AssignAndCommon&&, EmptyClass&>);
    STATIC_ASSERT(!assignable_from<AssignAndCommon&&, EmptyClass&&>);
    STATIC_ASSERT(!assignable_from<AssignAndCommon&&, EmptyClass const>);
    STATIC_ASSERT(!assignable_from<AssignAndCommon&&, EmptyClass const&>);
    STATIC_ASSERT(!assignable_from<AssignAndCommon&&, EmptyClass const&&>);

    struct VoidReturn {
        void operator=(EmptyClass);
    };
    STATIC_ASSERT(!assignable_from<VoidReturn, EmptyClass>);
    STATIC_ASSERT(!assignable_from<EmptyClass, VoidReturn>);

    struct EvilReturn {
        struct EvilCommaOverload {
            template <class U>
            IncompleteClass operator,(U&&);
        };

        EvilCommaOverload operator=(int);
    };
    STATIC_ASSERT(!assignable_from<EvilReturn, int>);

    STATIC_ASSERT(!assignable_from<void, void const>);
    STATIC_ASSERT(!assignable_from<void const, void const>);
    STATIC_ASSERT(!assignable_from<int(), int>);

    STATIC_ASSERT(assignable_from<DoNotInstantiate<int>*&, DoNotInstantiate<int>*>);
} // namespace test_assignable_from

namespace test_destructible {
    using std::destructible;

    using void_function = void();

    class PublicDestructor {
    public:
        ~PublicDestructor() {}
    };
    class ProtectedDestructor {
    protected:
        ~ProtectedDestructor() {}
    };

    class VirtualPublicDestructor {
    public:
        virtual ~VirtualPublicDestructor() {}
    };
    class VirtualProtectedDestructor {
    protected:
        virtual ~VirtualProtectedDestructor() {}
    };
    class VirtualPrivateDestructor {
    private:
        virtual ~VirtualPrivateDestructor() {}
    };
    class DeletedPublicDestructor {
    public:
        ~DeletedPublicDestructor() = delete;
    };
    class DeletedProtectedDestructor {
    protected:
        ~DeletedProtectedDestructor() = delete;
    };
    class DeletedPrivateDestructor {
    private:
        ~DeletedPrivateDestructor() = delete;
    };

    class DeletedVirtualPublicDestructor {
    public:
        virtual ~DeletedVirtualPublicDestructor() = delete;
    };
    class DeletedVirtualProtectedDestructor {
    protected:
        virtual ~DeletedVirtualProtectedDestructor() = delete;
    };
    class DeletedVirtualPrivateDestructor {
    private:
        virtual ~DeletedVirtualPrivateDestructor() = delete;
    };

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T>
    constexpr bool test() {
        constexpr bool result = destructible<T>;
        STATIC_ASSERT(destructible<T const> == result);
        STATIC_ASSERT(destructible<T volatile> == result);
        STATIC_ASSERT(destructible<T const volatile> == result);
        return result;
    }
#pragma warning(pop)

    STATIC_ASSERT(test<int>());
    STATIC_ASSERT(test<int const>());
    STATIC_ASSERT(test<int&>());
    STATIC_ASSERT(test<void (*)()>());
    STATIC_ASSERT(test<void (&)()>());
    STATIC_ASSERT(test<int[2]>());
    STATIC_ASSERT(test<int (*)[2]>());
    STATIC_ASSERT(test<int (&)[2]>());
    STATIC_ASSERT(test<CopyableType>());
    STATIC_ASSERT(test<MoveOnly>());
    STATIC_ASSERT(test<CopyOnlyAbomination>());
    STATIC_ASSERT(test<Immobile>());

    STATIC_ASSERT(!test<void()>());
    STATIC_ASSERT(!test<int[]>());
    STATIC_ASSERT(!test<Indestructible>());
    STATIC_ASSERT(!test<ThrowingDestructor>());
    STATIC_ASSERT(!test<void>());

    STATIC_ASSERT(test<NonTriviallyDestructible>());
    STATIC_ASSERT(test<int&>());
    STATIC_ASSERT(test<EmptyUnion>());
    STATIC_ASSERT(test<EmptyClass>());
    STATIC_ASSERT(test<int>());
    STATIC_ASSERT(test<double>());
    STATIC_ASSERT(test<int*>());
    STATIC_ASSERT(test<int const*>());
    STATIC_ASSERT(test<char[3]>());
    STATIC_ASSERT(test<BitZero>());
    STATIC_ASSERT(test<int[3]>());

    STATIC_ASSERT(test<ProtectedAbstract>());
    STATIC_ASSERT(test<PublicAbstract>());
    STATIC_ASSERT(test<PrivateAbstract>());
    STATIC_ASSERT(test<PublicDestructor>());
    STATIC_ASSERT(test<VirtualPublicDestructor>());
    STATIC_ASSERT(test<PurePublicDestructor>());

    STATIC_ASSERT(!test<int[]>());
    STATIC_ASSERT(!test<void>());
    STATIC_ASSERT(!test<void_function>());

    // Test inaccessible destructors
    STATIC_ASSERT(!test<ProtectedDestructor>());
    STATIC_ASSERT(!test<PrivateDestructor>());
    STATIC_ASSERT(!test<VirtualProtectedDestructor>());
    STATIC_ASSERT(!test<VirtualPrivateDestructor>());
    STATIC_ASSERT(!test<PureProtectedDestructor>());
    STATIC_ASSERT(!test<PurePrivateDestructor>());

    // Test deleted constructors
    STATIC_ASSERT(!test<DeletedPublicDestructor>());
    STATIC_ASSERT(!test<DeletedProtectedDestructor>());
    STATIC_ASSERT(!test<DeletedPrivateDestructor>());
    STATIC_ASSERT(!test<DeletedVirtualPublicDestructor>());
    STATIC_ASSERT(!test<DeletedVirtualProtectedDestructor>());
    STATIC_ASSERT(!test<DeletedVirtualPrivateDestructor>());
} // namespace test_destructible

namespace test_constructible_from {
    using std::constructible_from, std::initializer_list;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T, class... Args>
    constexpr bool test() {
        constexpr bool result = constructible_from<T, Args...>;
        STATIC_ASSERT(constructible_from<T const, Args...> == result);
        STATIC_ASSERT(constructible_from<T volatile, Args...> == result);
        STATIC_ASSERT(constructible_from<T const volatile, Args...> == result);
        return result;
    }
#pragma warning(pop)

    STATIC_ASSERT(!test<void>());
    STATIC_ASSERT(!test<void const>());
    STATIC_ASSERT(!test<void volatile>());
    STATIC_ASSERT(!test<void const volatile>());

    STATIC_ASSERT(test<int>());
    STATIC_ASSERT(test<int, int>());
    STATIC_ASSERT(test<int, int&>());
    STATIC_ASSERT(test<int, int&&>());
    STATIC_ASSERT(test<int, int const>());
    STATIC_ASSERT(test<int, int const&>());
    STATIC_ASSERT(test<int, int const&&>());
    STATIC_ASSERT(!test<int, int (&)[3]>());
    STATIC_ASSERT(!test<int, void()>());
    STATIC_ASSERT(!test<int, void (&)()>());
    STATIC_ASSERT(!test<int, void() const>());

    STATIC_ASSERT(test<int*>());
    STATIC_ASSERT(test<int*, int*>());
    STATIC_ASSERT(test<int*, int* const>());
    STATIC_ASSERT(test<int*, int*&>());
    STATIC_ASSERT(test<int*, int* const&>());
    STATIC_ASSERT(test<int*, int*&&>());
    STATIC_ASSERT(test<int*, int* const&&>());

    STATIC_ASSERT(!test<int&>());
    STATIC_ASSERT(!test<int&, int>());
    STATIC_ASSERT(test<int&, int&>());
    STATIC_ASSERT(!test<int&, int&&>());
    STATIC_ASSERT(!test<int&, int const>());
    STATIC_ASSERT(!test<int&, int const&>());
    STATIC_ASSERT(!test<int&, int const&&>());

    STATIC_ASSERT(!test<int&, long&>()); // https://github.com/ericniebler/stl2/issues/301
    STATIC_ASSERT(!test<int&, void>());
    STATIC_ASSERT(!test<int&, void()>());
    STATIC_ASSERT(!test<int&, void() const>());
    STATIC_ASSERT(!test<int&, void (&)()>());

    STATIC_ASSERT(!test<int const&>());
    STATIC_ASSERT(test<int const&, int>());
    STATIC_ASSERT(test<int const&, int&>());
    STATIC_ASSERT(test<int const&, int&&>());
    STATIC_ASSERT(test<int const&, int const>());
    STATIC_ASSERT(test<int const&, int const&>());
    STATIC_ASSERT(test<int const&, int const&&>());

    STATIC_ASSERT(test<int&&, int>());
    STATIC_ASSERT(!test<int&&, int&>());
    STATIC_ASSERT(test<int&&, int&&>());
    STATIC_ASSERT(!test<int&&, int const>());
    STATIC_ASSERT(!test<int&&, int const&>());
    STATIC_ASSERT(!test<int&&, int const&&>());

    STATIC_ASSERT(test<int const&&, int>());
    STATIC_ASSERT(!test<int const&&, int&>());
    STATIC_ASSERT(test<int const&&, int&&>());
    STATIC_ASSERT(test<int const&&, int const>());
    STATIC_ASSERT(!test<int const&&, int const&>());
    STATIC_ASSERT(test<int const&&, int const&&>());

    STATIC_ASSERT(!test<int()>());
    STATIC_ASSERT(!test<int (&)()>());
    STATIC_ASSERT(!test<int[]>());
    STATIC_ASSERT(test<int[5]>());
    STATIC_ASSERT(test<int const (&)[5], int (&)[5]>());

    STATIC_ASSERT(!test<void()>());
    STATIC_ASSERT(!test<void() const>());
    STATIC_ASSERT(!test<void() const, void*>());
    STATIC_ASSERT(!test<void() const, void() const>());
    STATIC_ASSERT(!test<void() volatile>());
    STATIC_ASSERT(!test<void() &>());
    STATIC_ASSERT(!test<void() &&>());

    STATIC_ASSERT(test<void (&)(), void()>());
    STATIC_ASSERT(test<void (&)(), void (&)()>());
    STATIC_ASSERT(test<void (&)(), void(&&)()>());
    STATIC_ASSERT(test<void(&&)(), void()>());
    STATIC_ASSERT(test<void(&&)(), void (&)()>());
    STATIC_ASSERT(test<void(&&)(), void(&&)()>());

    STATIC_ASSERT(test<int&&, double&>());

    STATIC_ASSERT(test<initializer_list<int>>());

    STATIC_ASSERT(!test<CopyableType>());
    STATIC_ASSERT(test<CopyableType, CopyableType>());
    STATIC_ASSERT(test<CopyableType, CopyableType&>());
    STATIC_ASSERT(test<CopyableType, CopyableType&&>());
    STATIC_ASSERT(test<CopyableType, CopyableType const>());
    STATIC_ASSERT(test<CopyableType, CopyableType const&>());
    STATIC_ASSERT(test<CopyableType, CopyableType const&&>());

    STATIC_ASSERT(test<MoveOnly>());
    STATIC_ASSERT(test<MoveOnly, MoveOnly>());
    STATIC_ASSERT(!test<MoveOnly, MoveOnly&>());
    STATIC_ASSERT(test<MoveOnly, MoveOnly&&>());
    STATIC_ASSERT(!test<MoveOnly, MoveOnly const>());
    STATIC_ASSERT(!test<MoveOnly, MoveOnly const&>());
    STATIC_ASSERT(!test<MoveOnly, MoveOnly const&&>());

    STATIC_ASSERT(test<CopyOnlyAbomination>());
    STATIC_ASSERT(!test<CopyOnlyAbomination, CopyOnlyAbomination>());
    STATIC_ASSERT(test<CopyOnlyAbomination, CopyOnlyAbomination&>());
    STATIC_ASSERT(!test<CopyOnlyAbomination, CopyOnlyAbomination&&>());
    STATIC_ASSERT(test<CopyOnlyAbomination, CopyOnlyAbomination const>());
    STATIC_ASSERT(test<CopyOnlyAbomination, CopyOnlyAbomination const&>());
    STATIC_ASSERT(test<CopyOnlyAbomination, CopyOnlyAbomination const&&>());

    STATIC_ASSERT(test<Immobile>());
    STATIC_ASSERT(!test<Immobile, Immobile>());
    STATIC_ASSERT(!test<Immobile, Immobile&>());
    STATIC_ASSERT(!test<Immobile, Immobile&&>());
    STATIC_ASSERT(!test<Immobile, Immobile const>());
    STATIC_ASSERT(!test<Immobile, Immobile const&>());
    STATIC_ASSERT(!test<Immobile, Immobile const&&>());

    STATIC_ASSERT(!test<NotDefaultConstructible>());
    STATIC_ASSERT(test<NotDefaultConstructible, NotDefaultConstructible>());
    STATIC_ASSERT(test<NotDefaultConstructible, NotDefaultConstructible&>());
    STATIC_ASSERT(test<NotDefaultConstructible, NotDefaultConstructible&&>());
    STATIC_ASSERT(test<NotDefaultConstructible, NotDefaultConstructible const>());
    STATIC_ASSERT(test<NotDefaultConstructible, NotDefaultConstructible const&>());
    STATIC_ASSERT(test<NotDefaultConstructible, NotDefaultConstructible const&&>());

    STATIC_ASSERT(!test<Indestructible>());
    STATIC_ASSERT(!test<Indestructible, Indestructible>());
    STATIC_ASSERT(!test<Indestructible, Indestructible&>());
    STATIC_ASSERT(!test<Indestructible, Indestructible&&>());
    STATIC_ASSERT(!test<Indestructible, Indestructible const>());
    STATIC_ASSERT(!test<Indestructible, Indestructible const&>());
    STATIC_ASSERT(!test<Indestructible, Indestructible const&&>());

    STATIC_ASSERT(!test<ThrowingDestructor>());
    STATIC_ASSERT(!test<ThrowingDestructor, ThrowingDestructor>());
    STATIC_ASSERT(!test<ThrowingDestructor, ThrowingDestructor&>());
    STATIC_ASSERT(!test<ThrowingDestructor, ThrowingDestructor&&>());
    STATIC_ASSERT(!test<ThrowingDestructor, ThrowingDestructor const>());
    STATIC_ASSERT(!test<ThrowingDestructor, ThrowingDestructor const&>());
    STATIC_ASSERT(!test<ThrowingDestructor, ThrowingDestructor const&&>());

    // Indestructible types are not constructible
    STATIC_ASSERT(test<PrivateDestructor&, PrivateDestructor&>());
    STATIC_ASSERT(!test<PrivateDestructor, int>());
    STATIC_ASSERT(!test<PurePrivateDestructor>());

    STATIC_ASSERT(test<ExplicitDefault>());
    STATIC_ASSERT(test<ExplicitDefault, ExplicitDefault>());
    STATIC_ASSERT(test<ExplicitDefault, ExplicitDefault&>());
    STATIC_ASSERT(test<ExplicitDefault, ExplicitDefault&&>());
    STATIC_ASSERT(test<ExplicitDefault, ExplicitDefault const>());
    STATIC_ASSERT(test<ExplicitDefault, ExplicitDefault const&>());
    STATIC_ASSERT(test<ExplicitDefault, ExplicitDefault const&&>());

    STATIC_ASSERT(!test<DeletedDefault>());
    STATIC_ASSERT(test<DeletedDefault, DeletedDefault>());
    STATIC_ASSERT(test<DeletedDefault, DeletedDefault&>());
    STATIC_ASSERT(test<DeletedDefault, DeletedDefault&&>());
    STATIC_ASSERT(test<DeletedDefault, DeletedDefault const>());
    STATIC_ASSERT(test<DeletedDefault, DeletedDefault const&>());
    STATIC_ASSERT(test<DeletedDefault, DeletedDefault const&&>());

    STATIC_ASSERT(test<SimpleBase, SimpleDerived>());
    STATIC_ASSERT(test<SimpleBase&, SimpleDerived&>());
    STATIC_ASSERT(!test<SimpleDerived&, SimpleBase&>());
    STATIC_ASSERT(test<SimpleBase const&, SimpleDerived const&>());
    STATIC_ASSERT(!test<SimpleDerived const&, SimpleBase const&>());
    STATIC_ASSERT(!test<SimpleDerived const&, SimpleBase>());

    STATIC_ASSERT(test<SimpleBase&&, SimpleDerived>());
    STATIC_ASSERT(test<SimpleBase&&, SimpleDerived&&>());
    STATIC_ASSERT(!test<SimpleDerived&&, SimpleBase&&>());
    STATIC_ASSERT(!test<SimpleDerived&&, SimpleBase>());
    STATIC_ASSERT(!test<SimpleBase&&, SimpleBase&>());
    STATIC_ASSERT(!test<SimpleBase&&, SimpleDerived&>());

    STATIC_ASSERT(!test<PrivateAbstract>());

    STATIC_ASSERT(test<int&, ImplicitTo<int&>>());
    STATIC_ASSERT(test<int const&, ImplicitTo<int&&>>());
    STATIC_ASSERT(test<int&&, ImplicitTo<int&&>>());
    STATIC_ASSERT(test<int const&, ImplicitTo<int>>());
    STATIC_ASSERT(test<int const&, ImplicitTo<int&>>());
    STATIC_ASSERT(test<int const&, ImplicitTo<int&>&>());

    STATIC_ASSERT(test<SimpleBase&&, ImplicitTo<SimpleDerived&&>>());
    STATIC_ASSERT(test<SimpleBase&&, ImplicitTo<SimpleDerived&&>&>());

    STATIC_ASSERT(test<int&, ExplicitTo<int&>>());
    STATIC_ASSERT(test<int const&, ExplicitTo<int&>>());
    STATIC_ASSERT(test<int const&, ExplicitTo<int&>&>());

    struct Multiparameter {
        explicit Multiparameter(int);
        Multiparameter(int, double);
        Multiparameter(int, long, double);
        Multiparameter(char) = delete;
    };
    STATIC_ASSERT(!test<Multiparameter>());
    STATIC_ASSERT(test<Multiparameter, int>());
    STATIC_ASSERT(!test<Multiparameter, long>() || is_permissive);
    STATIC_ASSERT(!test<Multiparameter, double>());
    STATIC_ASSERT(!test<Multiparameter, char>());
    STATIC_ASSERT(!test<Multiparameter, void>());
    STATIC_ASSERT(test<Multiparameter, int, double>());
    STATIC_ASSERT(test<Multiparameter, char, float>());
    STATIC_ASSERT(test<Multiparameter, int, long, double>());
    STATIC_ASSERT(test<Multiparameter, double, double, int>());

    struct ExplicitToDeduced {
        template <class T>
        explicit operator T() const;
    };
    STATIC_ASSERT(test<int, ExplicitToDeduced>());
    STATIC_ASSERT(!test<void, ExplicitToDeduced>());
    STATIC_ASSERT(!test<int&, ExplicitToDeduced>());

    // Binding through reference-compatible type is required to perform
    // direct-initialization as described in N4849 [over.match.ref]/1.1:
    STATIC_ASSERT(test<int&, ExplicitTo<int&>>());
    STATIC_ASSERT(test<int&&, ExplicitTo<int&&>>());

    // Binding through temporary behaves like copy-initialization,
    // see N4849 [dcl.init.ref]/5.4:
#ifndef __clang__ // TRANSITION, LLVM-44688
    STATIC_ASSERT(!test<int const&, ExplicitTo<int>>());
    STATIC_ASSERT(!test<int&&, ExplicitTo<int>>());
#endif // TRANSITION, LLVM-44688
    STATIC_ASSERT(!test<int const&, ExplicitTo<double&&>>());
    STATIC_ASSERT(!test<int&&, ExplicitTo<double&&>>());

    struct ImmobileExplicitFromInt {
        ImmobileExplicitFromInt()                          = default;
        ImmobileExplicitFromInt(ImmobileExplicitFromInt&&) = delete;
        explicit ImmobileExplicitFromInt(int);
    };
    STATIC_ASSERT(test<ImmobileExplicitFromInt>());
    STATIC_ASSERT(test<ImmobileExplicitFromInt, int>());
    STATIC_ASSERT(!test<ImmobileExplicitFromInt, ImmobileExplicitFromInt>());
    STATIC_ASSERT(!test<ImmobileExplicitFromInt, ImmobileExplicitFromInt&>());
    STATIC_ASSERT(!test<ImmobileExplicitFromInt, ImmobileExplicitFromInt&&>());
    STATIC_ASSERT(!test<ImmobileExplicitFromInt, ImmobileExplicitFromInt const>());
    STATIC_ASSERT(!test<ImmobileExplicitFromInt, ImmobileExplicitFromInt const&>());
    STATIC_ASSERT(!test<ImmobileExplicitFromInt, ImmobileExplicitFromInt const&&>());
} // namespace test_constructible_from

namespace test_default_initializable {
    using std::default_initializable, std::initializer_list;

    STATIC_ASSERT(default_initializable<int>);
    STATIC_ASSERT(!default_initializable<int const>);
    STATIC_ASSERT(default_initializable<int volatile>);
    STATIC_ASSERT(!default_initializable<int const volatile>);
    STATIC_ASSERT(default_initializable<double>);
    STATIC_ASSERT(!default_initializable<void>);

    STATIC_ASSERT(default_initializable<int*>);
    STATIC_ASSERT(default_initializable<int const*>);

    STATIC_ASSERT(default_initializable<int[2]>);
    STATIC_ASSERT(default_initializable<char[3]>);
    STATIC_ASSERT(default_initializable<char[5][3]>);
    STATIC_ASSERT(!default_initializable<int[]>);
    STATIC_ASSERT(!default_initializable<char[]>);
    STATIC_ASSERT(!default_initializable<char[][3]>);
    STATIC_ASSERT(!default_initializable<int const[2]>);

    STATIC_ASSERT(!default_initializable<int&>);
    STATIC_ASSERT(!default_initializable<int const&>);
    STATIC_ASSERT(!default_initializable<int&&>);

    STATIC_ASSERT(!default_initializable<int()>);
    STATIC_ASSERT(!default_initializable<int (&)()>);

    STATIC_ASSERT(!default_initializable<void()>);
    STATIC_ASSERT(!default_initializable<void() const>);
    STATIC_ASSERT(!default_initializable<void() volatile>);
    STATIC_ASSERT(!default_initializable<void() &>);
    STATIC_ASSERT(!default_initializable<void() &&>);

    STATIC_ASSERT(default_initializable<EmptyClass>);
    STATIC_ASSERT(default_initializable<EmptyClass const>);
    STATIC_ASSERT(default_initializable<EmptyUnion>);
    STATIC_ASSERT(default_initializable<EmptyUnion const>);

    STATIC_ASSERT(default_initializable<std::initializer_list<int>>);

    STATIC_ASSERT(default_initializable<NotEmpty>);
    STATIC_ASSERT(default_initializable<BitZero>);

    STATIC_ASSERT(default_initializable<ExplicitDefault>);
    STATIC_ASSERT(default_initializable<ExplicitMoveAbomination>);
    STATIC_ASSERT(default_initializable<ExplicitCopyAbomination>);

    STATIC_ASSERT(!default_initializable<NotDefaultConstructible>);
    STATIC_ASSERT(!default_initializable<DeletedDefault>);

    STATIC_ASSERT(!default_initializable<PurePublicDestructor>);
    STATIC_ASSERT(!default_initializable<NotDefaultConstructible>);

    class PrivateDefault {
        PrivateDefault();
    };
    STATIC_ASSERT(!default_initializable<PrivateDefault>);

    struct S {
        int x;
    };
    STATIC_ASSERT(default_initializable<S>);
    STATIC_ASSERT(!default_initializable<S const>);

    // Also test GH-1603 "default_initializable accepts types that are not default-initializable"
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-1326684
    STATIC_ASSERT(!default_initializable<AggregatesExplicitDefault>);
#else // ^^^ no workaround / assert bug so we'll notice when it's fixed vvv
    STATIC_ASSERT(default_initializable<AggregatesExplicitDefault>);
#endif // TRANSITION, DevCom-1326684
} // namespace test_default_initializable

namespace test_move_constructible {
    using std::move_constructible;

    STATIC_ASSERT(move_constructible<int>);
    STATIC_ASSERT(move_constructible<int const>);
    STATIC_ASSERT(move_constructible<double>);
    STATIC_ASSERT(!move_constructible<void>);

    STATIC_ASSERT(move_constructible<int*>);
    STATIC_ASSERT(move_constructible<int const*>);

    STATIC_ASSERT(!move_constructible<int[4]>);
    STATIC_ASSERT(!move_constructible<char[3]>);
    STATIC_ASSERT(!move_constructible<char[]>);

    STATIC_ASSERT(move_constructible<int&>);
    STATIC_ASSERT(move_constructible<int&&>);
    STATIC_ASSERT(move_constructible<int const&>);
    STATIC_ASSERT(move_constructible<int const&&>);

    STATIC_ASSERT(!move_constructible<void()>);

    STATIC_ASSERT(move_constructible<EmptyClass>);
    STATIC_ASSERT(move_constructible<EmptyUnion>);
    STATIC_ASSERT(move_constructible<NotEmpty>);
    STATIC_ASSERT(move_constructible<CopyableType>);
    STATIC_ASSERT(move_constructible<MoveOnly>);
    STATIC_ASSERT(!move_constructible<CopyOnlyAbomination>);
    STATIC_ASSERT(!move_constructible<Immobile>);
    STATIC_ASSERT(!move_constructible<ExplicitMoveAbomination>);
    STATIC_ASSERT(move_constructible<ExplicitCopyAbomination>);
    STATIC_ASSERT(move_constructible<BitZero>);

    STATIC_ASSERT(move_constructible<NotDefaultConstructible>);
    STATIC_ASSERT(!move_constructible<Indestructible>);
    STATIC_ASSERT(!move_constructible<ThrowingDestructor>);
    STATIC_ASSERT(move_constructible<ExplicitDefault>);
    STATIC_ASSERT(move_constructible<DeletedDefault>);

    STATIC_ASSERT(move_constructible<Immobile&>);
    STATIC_ASSERT(move_constructible<Immobile&&>);
    STATIC_ASSERT(move_constructible<Immobile const&>);
    STATIC_ASSERT(move_constructible<Immobile const&&>);

    STATIC_ASSERT(!move_constructible<PurePublicDestructor>);

    struct ImplicitlyDeletedMove {
        ImplicitlyDeletedMove(ImplicitlyDeletedMove const&);
    };
    STATIC_ASSERT(move_constructible<ImplicitlyDeletedMove>);
} // namespace test_move_constructible

namespace test_ranges_swap {
    namespace ranges = std::ranges;

    // clang-format off
    template <class T, class U = T>
    constexpr bool can_swap = false;
    template <class T, class U>
        requires requires { ranges::swap(std::declval<T>(), std::declval<U>()); }
    constexpr bool can_swap<T, U> = true;

    template <class T, class U = T>
    constexpr bool can_nothrow_swap = false;
    template <class T, class U>
        requires can_swap<T, U>
    constexpr bool can_nothrow_swap<T, U> = noexcept(ranges::swap(std::declval<T>(), std::declval<U>()));
    // clang-format on

    STATIC_ASSERT(!can_swap<void>);
    STATIC_ASSERT(!can_swap<void const>);
    STATIC_ASSERT(!can_swap<void volatile>);
    STATIC_ASSERT(!can_swap<void const volatile>);

    STATIC_ASSERT(can_nothrow_swap<int&>);
    STATIC_ASSERT(!can_swap<int const&>);
    STATIC_ASSERT(can_nothrow_swap<int volatile&>);
    STATIC_ASSERT(!can_swap<int const volatile&>);

    STATIC_ASSERT(!can_swap<int&&>);
    STATIC_ASSERT(!can_swap<int const&&>);
    STATIC_ASSERT(!can_swap<int volatile&&>);
    STATIC_ASSERT(!can_swap<int const volatile&&>);

    STATIC_ASSERT(!can_swap<int()>);
    STATIC_ASSERT(!can_swap<int() const>);
    STATIC_ASSERT(!can_swap<int (&)()>);
    STATIC_ASSERT(!can_swap<int(&&)()>);

    STATIC_ASSERT(!can_swap<int (&)[]>);
    STATIC_ASSERT(!can_swap<int(&&)[]>);

    STATIC_ASSERT(can_nothrow_swap<int (&)[42]>);
    STATIC_ASSERT(can_nothrow_swap<int (&)[42][13]>);
    STATIC_ASSERT(!can_swap<int (&)[42][13], int (&)[13][42]>);
    STATIC_ASSERT(!can_swap<int const (&)[42]>);
    STATIC_ASSERT(can_nothrow_swap<int volatile (&)[42]>);
    STATIC_ASSERT(!can_swap<int const volatile (&)[42]>);
    STATIC_ASSERT(!can_swap<int const (&)[42][13]>);
    STATIC_ASSERT(can_nothrow_swap<int volatile (&)[42][13]>);
    STATIC_ASSERT(!can_swap<int const volatile (&)[42][13]>);

    STATIC_ASSERT(!can_swap<int(&&)[42]>);
    STATIC_ASSERT(!can_swap<int(&&)[42][13]>);

    STATIC_ASSERT(!can_swap<int(&&)[42][13], int(&&)[13][42]>);

    struct SemithrowCopyOnly {
        SemithrowCopyOnly()                                  = default;
        SemithrowCopyOnly(SemithrowCopyOnly const&) noexcept = default;

        SemithrowCopyOnly& operator=(SemithrowCopyOnly const&) noexcept(false) {
            return *this;
        }
    };
    STATIC_ASSERT(can_swap<SemithrowCopyOnly&>);
    STATIC_ASSERT(!can_nothrow_swap<SemithrowCopyOnly&>);
    STATIC_ASSERT(can_swap<SemithrowCopyOnly (&)[42]>);
    STATIC_ASSERT(!can_nothrow_swap<SemithrowCopyOnly (&)[42]>);

    struct SemithrowMoveOnly {
        SemithrowMoveOnly() = default;
        SemithrowMoveOnly(SemithrowMoveOnly&&) noexcept(false) {}
        SemithrowMoveOnly& operator=(SemithrowMoveOnly&&) noexcept = default;
    };
    STATIC_ASSERT(can_swap<SemithrowMoveOnly&>);
    STATIC_ASSERT(!can_nothrow_swap<SemithrowMoveOnly&>);
    STATIC_ASSERT(can_swap<SemithrowMoveOnly (&)[42]>);
    STATIC_ASSERT(!can_nothrow_swap<SemithrowMoveOnly (&)[42]>);

    struct NothrowMoveOnly {
        NothrowMoveOnly()                           = default;
        NothrowMoveOnly(NothrowMoveOnly&&) noexcept = default;
        NothrowMoveOnly& operator=(NothrowMoveOnly&&) noexcept = default;
    };
    STATIC_ASSERT(can_nothrow_swap<NothrowMoveOnly&>);
    STATIC_ASSERT(can_nothrow_swap<NothrowMoveOnly (&)[42]>);

    struct NotMoveConstructible {
        NotMoveConstructible()                       = default;
        NotMoveConstructible(NotMoveConstructible&&) = delete;
        NotMoveConstructible& operator=(NotMoveConstructible&&) = default;
    };
    STATIC_ASSERT(!can_swap<NotMoveConstructible&>);
    STATIC_ASSERT(!can_swap<NotMoveConstructible (&)[42]>);

    struct NotMoveAssignable {
        NotMoveAssignable(NotMoveAssignable&&) = default;
        NotMoveAssignable& operator=(NotMoveAssignable&&) = delete;
    };
    STATIC_ASSERT(!can_swap<NotMoveAssignable&>);
    STATIC_ASSERT(!can_swap<NotMoveAssignable (&)[42]>);

    struct ImmobileNothrowSwap {
        ImmobileNothrowSwap()                      = default;
        ImmobileNothrowSwap(ImmobileNothrowSwap&&) = delete;
        ImmobileNothrowSwap& operator=(ImmobileNothrowSwap&&) = delete;
        friend void swap(ImmobileNothrowSwap&, ImmobileNothrowSwap&) noexcept {}
    };
    STATIC_ASSERT(can_nothrow_swap<ImmobileNothrowSwap&>);
    STATIC_ASSERT(can_nothrow_swap<ImmobileNothrowSwap (&)[42]>);

    struct HasThrowingSwap {
        friend void swap(HasThrowingSwap&, HasThrowingSwap&) {}
    };
    STATIC_ASSERT(can_swap<HasThrowingSwap&>);
    STATIC_ASSERT(!can_nothrow_swap<HasThrowingSwap&>);
    STATIC_ASSERT(can_swap<HasThrowingSwap (&)[42]>);
    STATIC_ASSERT(!can_nothrow_swap<HasThrowingSwap (&)[42]>);

    // Derives from type in std with overloaded swap to validate the poison pill
    struct Unswappable : std::pair<int, int> {
        Unswappable()                   = default;
        Unswappable(Unswappable const&) = delete;
        Unswappable(Unswappable&&)      = delete;
    };
    STATIC_ASSERT(!can_swap<Unswappable&>);

    STATIC_ASSERT(!can_swap<Unswappable (&)[42]>);
    STATIC_ASSERT(!can_swap<Unswappable (&)[42][13]>);
    STATIC_ASSERT(!can_swap<Unswappable (&)[42][13], Unswappable (&)[13][42]>);

    STATIC_ASSERT(!can_swap<Unswappable(&&)[42]>);
    STATIC_ASSERT(!can_swap<Unswappable(&&)[42][13]>);
    STATIC_ASSERT(!can_swap<Unswappable(&&)[42][13], Unswappable(&&)[13][42]>);

    // The wording allows customization of swap for unions as well
    union U {
        U(U const&) = delete;
        U& operator=(U const&) = delete;

        friend void swap(U&, U&) {}
    };
    STATIC_ASSERT(can_swap<U&>);

    namespace adl_barrier {
        struct ConstrainedSwappable {
            ConstrainedSwappable()                            = default;
            ConstrainedSwappable(ConstrainedSwappable const&) = default;
            ConstrainedSwappable(ConstrainedSwappable&&)      = default;
        };
        // ensure equally-specialized but more-constrained non-member swap is preferred to the poison pill
        template <std::same_as<ConstrainedSwappable> T>
        void swap(T&, T&) {}
    } // namespace adl_barrier
    STATIC_ASSERT(can_swap<adl_barrier::ConstrainedSwappable&>);
    STATIC_ASSERT(!can_swap<adl_barrier::ConstrainedSwappable const volatile&>);

    struct MyInt {
        int i = 42;
    };
    STATIC_ASSERT(can_nothrow_swap<MyInt&>);

    struct YourInt {
        int i = 13;
    };
    STATIC_ASSERT(can_nothrow_swap<YourInt&>);

    constexpr void swap(MyInt& x, YourInt& y) noexcept {
        ranges::swap(x.i, y.i);
    }
    constexpr void swap(YourInt& x, MyInt& y) noexcept {
        ranges::swap(x.i, y.i);
    }
    STATIC_ASSERT(can_nothrow_swap<MyInt&, YourInt&>);
    STATIC_ASSERT(can_nothrow_swap<YourInt&, MyInt&>);

    struct DoNotUseFallback {
        DoNotUseFallback() = default;
        DoNotUseFallback(DoNotUseFallback&&) noexcept {
            abort();
        }
        DoNotUseFallback& operator=(DoNotUseFallback&&) noexcept {
            return *this;
        }
        friend constexpr void swap(DoNotUseFallback&, DoNotUseFallback&) noexcept {}
    };

    constexpr auto for_each_232 = [](auto (&array)[2][3][2], auto f) {
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 3; ++j) {
                for (int k = 0; k < 2; ++k) {
                    f(array[i][j][k]);
                }
            }
        }
    };

    constexpr bool compile_time_tests() {
        using ranges::swap;

        {
            DoNotUseFallback x, y;
            swap(x, y);
        }
        {
            // Non-array swap
            int i = 1;
            int j = 2;
            swap(i, j);
            STATIC_ASSERT(noexcept(swap(i, j)));
            assert(i == 2);
            assert(j == 1);
        }
        {
            // Array swap
            int a[3] = {1, 2, 3};
            int b[3] = {4, 5, 6};
            swap(a, b);
            STATIC_ASSERT(noexcept(swap(a, b)));
            assert(a[0] == 4);
            assert(a[1] == 5);
            assert(a[2] == 6);
            assert(b[0] == 1);
            assert(b[1] == 2);
            assert(b[2] == 3);
        }
        {
            // Multidimensional array
            int a[2][3][2] = {
                {{0, 1}, {2, 3}, {4, 5}},
                {{6, 7}, {8, 9}, {10, 11}},
            };
            int b[2][3][2] = {
                {{12, 13}, {14, 15}, {16, 17}},
                {{18, 19}, {20, 21}, {22, 23}},
            };
            swap(a, b);
            STATIC_ASSERT(noexcept(swap(a, b)));
            int counter = 0;
            auto check  = [&counter](int const& i) {
                assert(i == counter);
                ++counter;
            };
            for_each_232(b, check);
            for_each_232(a, check);
        }
        {
            // Cross-type multidimensional array swap
            MyInt a[2][3][2] = {
                {{{0}, {1}}, {{2}, {3}}, {{4}, {5}}},
                {{{6}, {7}}, {{8}, {9}}, {{10}, {11}}},
            };
            YourInt b[2][3][2] = {
                {{{12}, {13}}, {{14}, {15}}, {{16}, {17}}},
                {{{18}, {19}}, {{20}, {21}}, {{22}, {23}}},
            };
            swap(a, b);
            STATIC_ASSERT(noexcept(swap(a, b)));
            int counter = 0;
            auto check  = [&counter](auto const& e) {
                assert(e.i == counter);
                ++counter;
            };
            for_each_232(b, check);
            for_each_232(a, check);
        }
        return true;
    }
    STATIC_ASSERT(compile_time_tests());

    void runtime_tests() {
        using ranges::swap;
        compile_time_tests();

        {
            // Non-array swap
            auto i = std::make_unique<int>(1);
            auto j = std::make_unique<int>(2);
            swap(i, j);
            STATIC_ASSERT(noexcept(swap(i, j)));
            assert(*i == 2);
            assert(*j == 1);
        }
        {
            // Array swap
            std::unique_ptr<int> a[3];
            for (int i = 0; i < 3; ++i) {
                a[i] = std::make_unique<int>(i + 1);
            }
            std::unique_ptr<int> b[3];
            for (int i = 0; i < 3; ++i) {
                b[i] = std::make_unique<int>(i + 4);
            }
            swap(a, b);
            STATIC_ASSERT(noexcept(swap(a, b)));
            assert(*a[0] == 4);
            assert(*a[1] == 5);
            assert(*a[2] == 6);
            assert(*b[0] == 1);
            assert(*b[1] == 2);
            assert(*b[2] == 3);
        }
        {
            // Multidimensional array
            std::unique_ptr<int> a[2][3][2];
            std::unique_ptr<int> b[2][3][2];

            int counter = 0;
            auto init   = [&counter](std::unique_ptr<int>& e) { e = std::make_unique<int>(counter++); };
            auto check  = [&counter](std::unique_ptr<int> const& e) {
                assert(*e == counter);
                ++counter;
            };

            for_each_232(a, init);
            for_each_232(b, init);
            swap(a, b);
            STATIC_ASSERT(noexcept(swap(a, b)));
            counter = 0;
            for_each_232(b, check);
            for_each_232(a, check);
        }
    }
} // namespace test_ranges_swap

namespace test_swappable {
    using std::swappable;

    STATIC_ASSERT(swappable<int>);
    STATIC_ASSERT(swappable<int[4]>);
    STATIC_ASSERT(!swappable<int[]>);
    STATIC_ASSERT(!swappable<int[][4]>);
    STATIC_ASSERT(swappable<int[3][1][4][1][5][9]>);

    STATIC_ASSERT(swappable<EmptyClass>);
    STATIC_ASSERT(swappable<EmptyUnion>);

    STATIC_ASSERT(!swappable<Immobile>);
    STATIC_ASSERT(!swappable<Immobile&>);
    STATIC_ASSERT(!swappable<Immobile&&>);

    STATIC_ASSERT(swappable<MovableFriendSwap>);
    // It may not be a great idea that swappable admits reference types and treats them as lvalues, but that's what
    // is_swappable does, so:
    STATIC_ASSERT(swappable<MovableFriendSwap&>);
    STATIC_ASSERT(swappable<MovableFriendSwap&&>);

    STATIC_ASSERT(swappable<ImmobileFriendSwap>);
    STATIC_ASSERT(swappable<ImmobileFriendSwap&>);
    STATIC_ASSERT(swappable<ImmobileFriendSwap&&>);

    // test non-referenceable types
    STATIC_ASSERT(!swappable<void>);
    STATIC_ASSERT(!swappable<int() const>);
    STATIC_ASSERT(!swappable<int() &>);

    STATIC_ASSERT(swappable<ImmobileNonMemberSwap>);
    STATIC_ASSERT(swappable<ImmobileNonMemberSwap&>);
    STATIC_ASSERT(swappable<ImmobileNonMemberSwap&&>);

    namespace non_member {
        struct OnlyRvalueSwappable : Immobile {};
        void swap(OnlyRvalueSwappable&&, OnlyRvalueSwappable&&) {}
        // swappable only cares about lvalue swaps
        STATIC_ASSERT(!swappable<OnlyRvalueSwappable>);
        STATIC_ASSERT(!swappable<OnlyRvalueSwappable&>);
        STATIC_ASSERT(!swappable<OnlyRvalueSwappable&&>);

        struct DeletedSwap {
            friend void swap(DeletedSwap&, DeletedSwap&) = delete;
        };
        // a deleted swap overload is correctly ignored
        STATIC_ASSERT(swappable<DeletedSwap>);
        STATIC_ASSERT(swappable<DeletedSwap&>);
        STATIC_ASSERT(swappable<DeletedSwap&&>);
    } // namespace non_member

    namespace underconstrained {
        struct AmbiguousSwap {};

        template <class T>
        void swap(T&, T&);

        // a swap overload that isn't more specialized or constrained than the poison pill is ignored
        STATIC_ASSERT(swappable<AmbiguousSwap>);
        STATIC_ASSERT(swappable<AmbiguousSwap&>);
        STATIC_ASSERT(swappable<AmbiguousSwap&&>);
    } // namespace underconstrained
} // namespace test_swappable

namespace test_swappable_with {
    using std::swappable_with;

    template <class T, class U>
    constexpr bool test() {
        STATIC_ASSERT(swappable_with<T, U> == swappable_with<U, T>);
        return swappable_with<T, U>;
    }

    STATIC_ASSERT(!test<void, int>());
    STATIC_ASSERT(!test<int, void>());
    STATIC_ASSERT(!test<void const, void const volatile>());

    STATIC_ASSERT(!test<int, int>());
    STATIC_ASSERT(test<int&, int&>());
    STATIC_ASSERT(!test<int&&, int&&>());

    STATIC_ASSERT(test<int (&)[4], int (&)[4]>());
    STATIC_ASSERT(!test<int, int>());
    STATIC_ASSERT(!test<int&&, int&&>());
    STATIC_ASSERT(!test<int&, double&>());
    STATIC_ASSERT(!test<int (&)[4], bool (&)[4]>());

    STATIC_ASSERT(test<int (&)[3][4], int (&)[3][4]>());
    STATIC_ASSERT(test<int (&)[3][4][1][2], int (&)[3][4][1][2]>());
    STATIC_ASSERT(!test<int (&)[3][4][1][2], int (&)[4][4][1][2]>());

    STATIC_ASSERT(test<int (&)[2][2], int (&)[2][2]>());

    STATIC_ASSERT(test<MovableFriendSwap, MovableFriendSwap>() == is_permissive);
    STATIC_ASSERT(test<MovableFriendSwap&, MovableFriendSwap&>());
    STATIC_ASSERT(test<MovableFriendSwap&&, MovableFriendSwap&&>() == is_permissive);

    STATIC_ASSERT(!test<ImmobileFriendSwap, ImmobileFriendSwap>());
    STATIC_ASSERT(test<ImmobileFriendSwap&, ImmobileFriendSwap&>());
    STATIC_ASSERT(test<ImmobileFriendSwap&&, ImmobileFriendSwap&&>() == is_permissive);

    STATIC_ASSERT(!test<ImmobileNonMemberSwap, ImmobileNonMemberSwap>());
    STATIC_ASSERT(test<ImmobileNonMemberSwap&, ImmobileNonMemberSwap&>());
    STATIC_ASSERT(test<ImmobileNonMemberSwap&&, ImmobileNonMemberSwap&&>() == is_permissive);

    struct LvalueAndRvalueSwappable {};
    void swap(LvalueAndRvalueSwappable&&, LvalueAndRvalueSwappable&&) {}
    STATIC_ASSERT(test<LvalueAndRvalueSwappable, LvalueAndRvalueSwappable>());
    STATIC_ASSERT(test<LvalueAndRvalueSwappable&, LvalueAndRvalueSwappable&>());
    STATIC_ASSERT(test<LvalueAndRvalueSwappable&&, LvalueAndRvalueSwappable&&>());

    template <int>
    struct ImmobileAndSwappable : Immobile {
        friend void swap(ImmobileAndSwappable&, ImmobileAndSwappable&) {}
    };
    template <int I, int J>
    void swap(ImmobileAndSwappable<I>&, ImmobileAndSwappable<J>&) {}

    struct ImmobileAndNotSwappable : Immobile {};
    void swap(ImmobileNonMemberSwap&, ImmobileAndNotSwappable&) {}
    void swap(ImmobileAndNotSwappable&, ImmobileNonMemberSwap&) {}

    struct CanSwapButUncommon {};
    void swap(ImmobileNonMemberSwap&, CanSwapButUncommon&) {}
    void swap(CanSwapButUncommon&, ImmobileNonMemberSwap&) {}

    template <int>
    struct AsymmetricallySwappable {};
    void swap(AsymmetricallySwappable<0>&, AsymmetricallySwappable<1>&) {}
} // namespace test_swappable_with

// common_type specializations to satisfy swappable_with's common_reference_with requirement
template <>
struct std::common_type<ImmobileNonMemberSwap, test_swappable_with::ImmobileAndNotSwappable> {
    using type = Omniconvertible;
};
template <>
struct std::common_type<test_swappable_with::ImmobileAndNotSwappable, ImmobileNonMemberSwap> {
    using type = Omniconvertible;
};

template <int I, int J>
struct std::common_type<test_swappable_with::AsymmetricallySwappable<I>,
    test_swappable_with::AsymmetricallySwappable<J>> {
    using type = Omniconvertible;
};

template <int I, int J>
struct std::common_type<test_swappable_with::ImmobileAndSwappable<I>, test_swappable_with::ImmobileAndSwappable<J>> {
    using type = Omniconvertible;
};

namespace test_swappable_with {
    // Not swappable_with: a constituent type is not swappable
    STATIC_ASSERT(!test<ImmobileNonMemberSwap&, ImmobileAndNotSwappable&>());
    // Not swappable_with: common_reference_with requirement is not satisfied
    STATIC_ASSERT(!test<ImmobileNonMemberSwap&, CanSwapButUncommon&>());
    // Not swappable_with: asymmetric
    STATIC_ASSERT(!test<AsymmetricallySwappable<0>&, AsymmetricallySwappable<1>&>());
    // swappable_with: all requirements met
    STATIC_ASSERT(test<ImmobileAndSwappable<0>&, ImmobileAndSwappable<1>&>());

    namespace example {
        // The example from [concept.swappable] with changes per the proposed resolution of LWG-3175:
        // clang-format off
        namespace ranges = std::ranges;

        template <class T, std::swappable_with<T> U>
        void value_swap(T&& t, U&& u) {
          ranges::swap(std::forward<T>(t), std::forward<U>(u));
        }

        template <std::swappable T>
        void lv_swap(T& t1, T& t2) {
          ranges::swap(t1, t2);
        }

        namespace N {
          struct A { int m; };
          struct Proxy {
            A* a;
            Proxy(A& a) : a{&a} {}
            friend void swap(Proxy x, Proxy y) {
              ranges::swap(*x.a, *y.a);
            }
          };
          Proxy proxy(A& a) { return Proxy{a}; }
        }

        void test() {
          int i = 1, j = 2;
          lv_swap(i, j);
          assert(i == 2 && j == 1);

          N::A a1 = { 5 };
          N::A a2 = { -5 };
          value_swap(a1, proxy(a2));
          assert(a1.m == -5 && a2.m == 5);

          // additional test cases not from [concept.swappable] for completeness:
          STATIC_ASSERT(std::is_same_v<std::common_reference_t<N::Proxy, N::A&>, N::Proxy>);
          STATIC_ASSERT(swappable_with<N::A&, N::Proxy>);

          value_swap(proxy(a1), a2);
          assert(a1.m == 5 && a2.m == -5);

          value_swap(proxy(a1), proxy(a2));
          assert(a1.m == -5 && a2.m == 5);

          value_swap(a1, a2);
          assert(a1.m == 5 && a2.m == -5);
          // end additional test cases
        }
        // clang-format on
    } // namespace example
} // namespace test_swappable_with

namespace test_copy_constructible {
    using std::copy_constructible;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T>
    constexpr bool test() {
        constexpr bool result = copy_constructible<T>;
        STATIC_ASSERT(copy_constructible<T const> == result);
        if constexpr (std::is_scalar_v<T> || std::is_reference_v<T>) {
            STATIC_ASSERT(copy_constructible<T volatile> == result);
            STATIC_ASSERT(copy_constructible<T const volatile> == result);
        } else {
            STATIC_ASSERT(!copy_constructible<T volatile>);
            STATIC_ASSERT(!copy_constructible<T const volatile>);
        }
        return result;
    }
#pragma warning(pop)

    STATIC_ASSERT(!test<void>());

    STATIC_ASSERT(test<int>());
    STATIC_ASSERT(test<int const>());
    STATIC_ASSERT(test<double>());

    STATIC_ASSERT(test<int*>());
    STATIC_ASSERT(test<int const*>());

    STATIC_ASSERT(!test<int[4]>());
    STATIC_ASSERT(!test<int[]>());

    STATIC_ASSERT(test<int&>());
    STATIC_ASSERT(!test<int&&>());
    STATIC_ASSERT(test<int const&>());
    STATIC_ASSERT(!test<int const&&>());

    STATIC_ASSERT(!test<void()>());

    STATIC_ASSERT(test<EmptyUnion>());
    STATIC_ASSERT(test<EmptyClass>());
    STATIC_ASSERT(test<NotEmpty>());
    STATIC_ASSERT(test<BitZero>());

    STATIC_ASSERT(test<CopyableType>());
    STATIC_ASSERT(!test<MoveOnly>());
    STATIC_ASSERT(!copy_constructible<CopyOnlyAbomination>);
    STATIC_ASSERT(copy_constructible<CopyOnlyAbomination const>);
    STATIC_ASSERT(!test<Immobile>());
    STATIC_ASSERT(!test<ExplicitMoveAbomination>());
    STATIC_ASSERT(!test<ExplicitCopyAbomination>());

    STATIC_ASSERT(!test<PurePublicDestructor>());
    STATIC_ASSERT(!test<Indestructible>());
    STATIC_ASSERT(!test<ThrowingDestructor>());

    STATIC_ASSERT(test<NotDefaultConstructible>());
    STATIC_ASSERT(test<ExplicitDefault>());
    STATIC_ASSERT(test<DeletedDefault>());

    STATIC_ASSERT(test<Immobile&>());
    STATIC_ASSERT(!test<Immobile&&>());
    STATIC_ASSERT(test<Immobile const&>());
    STATIC_ASSERT(!test<Immobile const&&>());

#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-119526
    // https://github.com/ericniebler/stl2/issues/301
    struct NotMutableRef {
        NotMutableRef()                     = default;
        NotMutableRef(NotMutableRef const&) = default;
        NotMutableRef(NotMutableRef&)       = delete;
        NotMutableRef(NotMutableRef&&)      = default;
    };
    struct NotConstRefRef {
        NotConstRefRef()                       = default;
        NotConstRefRef(NotConstRefRef const&)  = default;
        NotConstRefRef(NotConstRefRef&&)       = default;
        NotConstRefRef(NotConstRefRef const&&) = delete;
    };

    STATIC_ASSERT(!copy_constructible<NotMutableRef>);
    STATIC_ASSERT(!copy_constructible<NotConstRefRef>);
#endif // TRANSITION, VSO-119526

    struct UserProvidedCopy {
        UserProvidedCopy(UserProvidedCopy const&);
    };
    STATIC_ASSERT(test<UserProvidedCopy>());

    class PrivateCopy {
    private:
        PrivateCopy(PrivateCopy const&);
    };
    STATIC_ASSERT(!test<PrivateCopy>());

    struct MutatingCopyAbomination {
        MutatingCopyAbomination(MutatingCopyAbomination&); // NB: not const&
        void operator=(MutatingCopyAbomination&); // NB: not const&
    };
    STATIC_ASSERT(!test<MutatingCopyAbomination>());
} // namespace test_copy_constructible

namespace test_object_concepts {
    using std::copyable, std::movable, std::regular, std::semiregular;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T>
    constexpr bool test_movable() {
        constexpr bool result = movable<T>;
        STATIC_ASSERT(!movable<T const>);
        STATIC_ASSERT(movable<T volatile> == std::is_scalar_v<T>);
        STATIC_ASSERT(!movable<T const volatile>);
        return result;
    }

    template <class T>
    constexpr bool test_copyable() {
        constexpr bool result = copyable<T>;
        STATIC_ASSERT(!copyable<T const>);
        STATIC_ASSERT(copyable<T volatile> == std::is_scalar_v<T>);
        STATIC_ASSERT(!copyable<T const volatile>);
        return result;
    }

    template <class T>
    constexpr bool test_semiregular() {
        constexpr bool result = semiregular<T>;
        STATIC_ASSERT(!semiregular<T const>);
        STATIC_ASSERT(semiregular<T volatile> == std::is_scalar_v<T>);
        STATIC_ASSERT(!semiregular<T const volatile>);
        return result;
    }

    template <class T>
    constexpr bool test_regular() {
        constexpr bool result = regular<T>;
        STATIC_ASSERT(!regular<T const>);
        STATIC_ASSERT(regular<T volatile> == std::is_scalar_v<T>);
        STATIC_ASSERT(!regular<T const volatile>);
        return result;
    }
#pragma warning(pop)

    STATIC_ASSERT(!test_movable<void>());

    STATIC_ASSERT(test_movable<int>());
    STATIC_ASSERT(test_movable<double>());

    STATIC_ASSERT(!test_movable<int&>());
    STATIC_ASSERT(!test_movable<int&&>()); // https://github.com/ericniebler/stl2/issues/310

    STATIC_ASSERT(!test_movable<int[42]>());
    STATIC_ASSERT(!test_movable<int[]>());

    STATIC_ASSERT(!test_movable<int(int)>());

    STATIC_ASSERT(test_movable<CopyableType>());
    STATIC_ASSERT(test_movable<MoveOnly>());
    STATIC_ASSERT(!test_movable<CopyOnlyAbomination>());
    STATIC_ASSERT(!test_movable<Immobile>());

    STATIC_ASSERT(!test_copyable<void>());

    STATIC_ASSERT(test_copyable<int>());
    STATIC_ASSERT(test_copyable<double>());

    STATIC_ASSERT(!test_copyable<int[42]>());
    STATIC_ASSERT(!test_copyable<int[]>());

    STATIC_ASSERT(!test_copyable<int&>());
    STATIC_ASSERT(!test_copyable<int&&>());

    STATIC_ASSERT(!test_copyable<int(int)>());

    STATIC_ASSERT(test_copyable<CopyableType>());
    STATIC_ASSERT(!test_copyable<MoveOnly>());
    STATIC_ASSERT(!test_copyable<CopyOnlyAbomination>());
    STATIC_ASSERT(!test_copyable<Immobile>());

    STATIC_ASSERT(!test_semiregular<void>());

    STATIC_ASSERT(test_semiregular<int>());
    STATIC_ASSERT(test_semiregular<double>());

    STATIC_ASSERT(!test_semiregular<int&>());

    STATIC_ASSERT(!test_semiregular<MoveOnly>());
    STATIC_ASSERT(!test_semiregular<Immobile>());
    STATIC_ASSERT(!test_semiregular<ExplicitMoveAbomination>());
    STATIC_ASSERT(!test_semiregular<ExplicitCopyAbomination>());
    STATIC_ASSERT(!test_semiregular<CopyOnlyAbomination>());
    STATIC_ASSERT(test_semiregular<SemiregularType>());
    STATIC_ASSERT(test_semiregular<RegularType>());

    STATIC_ASSERT(!test_regular<void>());

    STATIC_ASSERT(test_regular<int>());
    STATIC_ASSERT(test_regular<double>());

    STATIC_ASSERT(!test_regular<int&>());

    STATIC_ASSERT(!test_regular<MoveOnly>());
    STATIC_ASSERT(!test_regular<Immobile>());
    STATIC_ASSERT(!test_regular<ExplicitMoveAbomination>());
    STATIC_ASSERT(!test_regular<ExplicitCopyAbomination>());
    STATIC_ASSERT(!test_regular<CopyOnlyAbomination>());
    STATIC_ASSERT(!test_regular<CopyableType>());
    STATIC_ASSERT(!test_regular<SemiregularType>());
    STATIC_ASSERT(test_regular<RegularType>());
} // namespace test_object_concepts

namespace test_boolean_testable {
    // Note: other than knowing the secret internal concept name, this is a portable test.
    using std::_Boolean_testable;

    // Better have these four, since we use them as examples in the Standard.
    STATIC_ASSERT(_Boolean_testable<bool>);
    STATIC_ASSERT(_Boolean_testable<std::true_type>);
    STATIC_ASSERT(_Boolean_testable<int*>);
    STATIC_ASSERT(_Boolean_testable<std::bitset<42>::reference>);

    STATIC_ASSERT(_Boolean_testable<std::false_type>);

    STATIC_ASSERT(_Boolean_testable<int>);
    STATIC_ASSERT(_Boolean_testable<void*>);

    enum unscoped_boolish : bool { No, Yes };
    STATIC_ASSERT(_Boolean_testable<unscoped_boolish>);

    enum class scoped_boolish : bool { No, Yes };
    STATIC_ASSERT(!_Boolean_testable<scoped_boolish>);

    STATIC_ASSERT(!_Boolean_testable<EmptyClass>);

    STATIC_ASSERT(_Boolean_testable<ImplicitTo<bool>>);
    STATIC_ASSERT(!_Boolean_testable<ExplicitTo<bool>>);

    struct MutatingBoolConversion {
        operator bool();
    };
    STATIC_ASSERT(_Boolean_testable<MutatingBoolConversion>);

    template <unsigned int Select> // values in [0, Archetype_max) select a requirement to violate
    struct Archetype {
        // clang-format off
        operator bool() const requires (Select != 0); // Archetype<0> is not implicitly convertible to bool
        explicit operator bool() const requires (Select < 2); // Archetype<1> is not explicitly convertible
                                                              // to bool (ambiguity)
        void operator!() const requires (Select == 2); // !Archetype<2> does not model _Boolean_testable_impl
        // clang-format on
    };

    STATIC_ASSERT(!_Boolean_testable<Archetype<0>>);
    STATIC_ASSERT(!_Boolean_testable<Archetype<1>>);
    STATIC_ASSERT(!_Boolean_testable<Archetype<2>>);
    STATIC_ASSERT(_Boolean_testable<Archetype<3>>);
} // namespace test_boolean_testable

namespace test_equality_comparable {
    using std::equality_comparable;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T>
    constexpr bool test() {
        constexpr bool result = equality_comparable<T>;
        STATIC_ASSERT(equality_comparable<T const> == result);
        if constexpr (!std::is_same_v<T, std::add_lvalue_reference_t<T>>) {
            STATIC_ASSERT(equality_comparable<T&> == result);
            STATIC_ASSERT(equality_comparable<T const&> == result);
            STATIC_ASSERT(equality_comparable<T&&> == result);
            STATIC_ASSERT(equality_comparable<T const&&> == result);
        }
        return result;
    }
#pragma warning(pop)

    STATIC_ASSERT(!test<void>());

    STATIC_ASSERT(test<int>());
    STATIC_ASSERT(test<double>());
    STATIC_ASSERT(test<std::nullptr_t>());
    STATIC_ASSERT(test<int[42]>());
    STATIC_ASSERT(test<int(int)>());

    STATIC_ASSERT(!test<EmptyClass>());

    template <unsigned int> // selects one requirement to violate
    struct Archetype {};

    template <unsigned int Select>
    bool operator==(Archetype<Select> const&, Archetype<Select> const&);
    void operator==(Archetype<0> const&, Archetype<0> const&); // Archetype<0> == Archetype<0> is not _Boolean_testable

    template <unsigned int Select>
    bool operator!=(Archetype<Select> const&, Archetype<Select> const&);
    void operator!=(Archetype<1> const&, Archetype<1> const&); // Archetype<1> != Archetype<1> is not _Boolean_testable

    STATIC_ASSERT(!test<Archetype<0>>());
    STATIC_ASSERT(!test<Archetype<1>>());
    STATIC_ASSERT(test<Archetype<2>>());
} // namespace test_equality_comparable

namespace test_equality_comparable_with {
    using std::equality_comparable_with;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T, class U = T>
    constexpr bool test() {
        using std::is_same_v, std::add_lvalue_reference_t;

        constexpr bool result = equality_comparable_with<T, U>;
        STATIC_ASSERT(equality_comparable_with<U, T> == result);

        if constexpr (!is_same_v<T, add_lvalue_reference_t<T>>) {
            STATIC_ASSERT(equality_comparable_with<T&, U> == result);
            STATIC_ASSERT(equality_comparable_with<T const&, U> == result);
            STATIC_ASSERT(equality_comparable_with<T&&, U> == result);
            STATIC_ASSERT(equality_comparable_with<T const&&, U> == result);
        }

        if constexpr (!is_same_v<U, add_lvalue_reference_t<U>>) {
            STATIC_ASSERT(equality_comparable_with<T, U&> == result);
            STATIC_ASSERT(equality_comparable_with<T, U const&> == result);
            STATIC_ASSERT(equality_comparable_with<T, U&&> == result);
            STATIC_ASSERT(equality_comparable_with<T, U const&&> == result);
        }

        if constexpr (!is_same_v<T, add_lvalue_reference_t<T>> && !is_same_v<U, add_lvalue_reference_t<U>>) {
            STATIC_ASSERT(equality_comparable_with<T&, U&> == result);
            STATIC_ASSERT(equality_comparable_with<T const&, U&> == result);
            STATIC_ASSERT(equality_comparable_with<T&&, U&> == result);
            STATIC_ASSERT(equality_comparable_with<T const&&, U&> == result);

            STATIC_ASSERT(equality_comparable_with<T&, U const&> == result);
            STATIC_ASSERT(equality_comparable_with<T const&, U const&> == result);
            STATIC_ASSERT(equality_comparable_with<T&&, U const&> == result);
            STATIC_ASSERT(equality_comparable_with<T const&&, U const&> == result);

            STATIC_ASSERT(equality_comparable_with<T&, U&&> == result);
            STATIC_ASSERT(equality_comparable_with<T const&, U&&> == result);
            STATIC_ASSERT(equality_comparable_with<T&&, U&&> == result);
            STATIC_ASSERT(equality_comparable_with<T const&&, U&&> == result);

            STATIC_ASSERT(equality_comparable_with<T&, U const&&> == result);
            STATIC_ASSERT(equality_comparable_with<T const&, U const&&> == result);
            STATIC_ASSERT(equality_comparable_with<T&&, U const&&> == result);
            STATIC_ASSERT(equality_comparable_with<T const&&, U const&&> == result);
        }

        return result;
    }
#pragma warning(pop)

    STATIC_ASSERT(!test<void>());
    STATIC_ASSERT(!test<int, void>());

    STATIC_ASSERT(test<int>());
    STATIC_ASSERT(test<double>());
    STATIC_ASSERT(test<int, double>());
    STATIC_ASSERT(test<std::nullptr_t>());
    STATIC_ASSERT(test<int*, void*>());
    STATIC_ASSERT(test<int[42]>());
    STATIC_ASSERT(test<int(int)>());

    template <int>
    struct CrossSpecializationComparable {
        bool operator==(CrossSpecializationComparable const&) const;
        bool operator!=(CrossSpecializationComparable const&) const;
    };
    STATIC_ASSERT(test<CrossSpecializationComparable<0>, CrossSpecializationComparable<0>>());

    template <int I, int J>
    bool operator==(CrossSpecializationComparable<I> const&, CrossSpecializationComparable<J> const&);
    template <int I, int J>
    bool operator!=(CrossSpecializationComparable<I> const&, CrossSpecializationComparable<J> const&);

    struct Common { // A common type for CrossSpecializationComparable<1> / <2>
        Common(CrossSpecializationComparable<1>);
        Common(CrossSpecializationComparable<2>);

        friend bool operator==(Common const&, Common const&) {
            return true;
        }
        friend bool operator!=(Common const&, Common const&) {
            return false;
        }
    };
    STATIC_ASSERT(test<Common, Common>());
    STATIC_ASSERT(!test<CrossSpecializationComparable<0>, Common>());
    STATIC_ASSERT(test<CrossSpecializationComparable<1>, Common>());
    STATIC_ASSERT(test<CrossSpecializationComparable<2>, Common>());
} // namespace test_equality_comparable_with

template <>
struct std::common_type<test_equality_comparable_with::CrossSpecializationComparable<1>,
    test_equality_comparable_with::CrossSpecializationComparable<2>> {
    using type = test_equality_comparable_with::Common;
};
template <>
struct std::common_type<test_equality_comparable_with::CrossSpecializationComparable<2>,
    test_equality_comparable_with::CrossSpecializationComparable<1>> {
    using type = test_equality_comparable_with::Common;
};

namespace test_equality_comparable_with {
    // CrossSpecializationComparable<0> / <1> have all the necessary operators, but fail common_reference_with.
    STATIC_ASSERT(!test<CrossSpecializationComparable<0>, CrossSpecializationComparable<1>>());
    // CrossSpecializationComparable<1> / <2> *do* satisfy common_reference_with.
    STATIC_ASSERT(test<CrossSpecializationComparable<1>, CrossSpecializationComparable<2>>());
} // namespace test_equality_comparable_with

namespace test_totally_ordered {
    using std::totally_ordered;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T>
    constexpr bool test() {
        constexpr bool result = totally_ordered<T>;
        STATIC_ASSERT(totally_ordered<T const> == result);
        if constexpr (!std::is_same_v<T, std::add_lvalue_reference_t<T>>) {
            STATIC_ASSERT(totally_ordered<T&> == result);
            STATIC_ASSERT(totally_ordered<T const&> == result);
            STATIC_ASSERT(totally_ordered<T&&> == result);
            STATIC_ASSERT(totally_ordered<T const&&> == result);
        }
        return result;
    }
#pragma warning(pop)

    STATIC_ASSERT(!test<void>());

    STATIC_ASSERT(test<int>());
    STATIC_ASSERT(test<double>());
    STATIC_ASSERT(test<void*>());
    STATIC_ASSERT(test<int*>());
    STATIC_ASSERT(test<int[42]>());
    STATIC_ASSERT(test<int(int)>());

    STATIC_ASSERT(!test<std::nullptr_t>());
    STATIC_ASSERT(!test<EmptyClass>());

    constexpr unsigned int Archetype_max = 6;
    template <unsigned int> // values in [0, Archetype_max) select a requirement to violate
    struct Archetype {};

    template <unsigned int Select>
    bool operator==(Archetype<Select> const&, Archetype<Select> const&);
    void operator==(Archetype<0> const&, Archetype<0> const&);

    template <unsigned int Select>
    bool operator!=(Archetype<Select> const&, Archetype<Select> const&);
    void operator!=(Archetype<1> const&, Archetype<1> const&);

    template <unsigned int Select>
    bool operator<(Archetype<Select> const&, Archetype<Select> const&);
    void operator<(Archetype<2> const&, Archetype<2> const&);

    template <unsigned int Select>
    bool operator>(Archetype<Select> const&, Archetype<Select> const&);
    void operator>(Archetype<3> const&, Archetype<3> const&);

    template <unsigned int Select>
    bool operator<=(Archetype<Select> const&, Archetype<Select> const&);
    void operator<=(Archetype<4> const&, Archetype<4> const&);

    template <unsigned int Select>
    bool operator>=(Archetype<Select> const&, Archetype<Select> const&);
    void operator>=(Archetype<5> const&, Archetype<5> const&);

    template <std::size_t I>
    constexpr void test_archetype_single() {
        STATIC_ASSERT(!test<Archetype<I>>());
    }

    template <std::size_t... Is>
    constexpr bool test_Archetype(std::index_sequence<Is...>) {
        STATIC_ASSERT(std::is_same_v<std::index_sequence<Is...>, std::make_index_sequence<Archetype_max>>);
        (test_archetype_single<Is>(), ...);
        STATIC_ASSERT(test<Archetype<Archetype_max>>());
        return true;
    }
    STATIC_ASSERT(test_Archetype(std::make_index_sequence<Archetype_max>{}));
} // namespace test_totally_ordered

namespace test_totally_ordered_with {
    using std::totally_ordered_with;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T, class U = T>
    constexpr bool test() {
        constexpr bool result = totally_ordered_with<T, U>;
        STATIC_ASSERT(totally_ordered_with<U, T> == result);
        if constexpr (!std::is_same_v<T, std::add_lvalue_reference_t<T>>) {
            STATIC_ASSERT(totally_ordered_with<T&, U> == result);
            STATIC_ASSERT(totally_ordered_with<T const&, U> == result);
            STATIC_ASSERT(totally_ordered_with<T&&, U> == result);
            STATIC_ASSERT(totally_ordered_with<T const&&, U> == result);
        }

        if constexpr (!std::is_same_v<U, std::add_lvalue_reference_t<U>>) {
            STATIC_ASSERT(totally_ordered_with<T, U&> == result);
            STATIC_ASSERT(totally_ordered_with<T, U const&> == result);
            STATIC_ASSERT(totally_ordered_with<T, U&&> == result);
            STATIC_ASSERT(totally_ordered_with<T, U const&&> == result);
        }

        if constexpr (!std::is_same_v<T,
                          std::add_lvalue_reference_t<T>> && !std::is_same_v<U, std::add_lvalue_reference_t<U>>) {
            STATIC_ASSERT(totally_ordered_with<T&, U&> == result);
            STATIC_ASSERT(totally_ordered_with<T const&, U&> == result);
            STATIC_ASSERT(totally_ordered_with<T&&, U&> == result);
            STATIC_ASSERT(totally_ordered_with<T const&&, U&> == result);

            STATIC_ASSERT(totally_ordered_with<T&, U const&> == result);
            STATIC_ASSERT(totally_ordered_with<T const&, U const&> == result);
            STATIC_ASSERT(totally_ordered_with<T&&, U const&> == result);
            STATIC_ASSERT(totally_ordered_with<T const&&, U const&> == result);

            STATIC_ASSERT(totally_ordered_with<T&, U&&> == result);
            STATIC_ASSERT(totally_ordered_with<T const&, U&&> == result);
            STATIC_ASSERT(totally_ordered_with<T&&, U&&> == result);
            STATIC_ASSERT(totally_ordered_with<T const&&, U&&> == result);

            STATIC_ASSERT(totally_ordered_with<T&, U const&&> == result);
            STATIC_ASSERT(totally_ordered_with<T const&, U const&&> == result);
            STATIC_ASSERT(totally_ordered_with<T&&, U const&&> == result);
            STATIC_ASSERT(totally_ordered_with<T const&&, U const&&> == result);
        }

        return result;
    }
#pragma warning(pop)

    STATIC_ASSERT(!test<void>());
    STATIC_ASSERT(!test<int, void>());

    STATIC_ASSERT(test<int>());
    STATIC_ASSERT(test<double>());
    STATIC_ASSERT(test<int, double>());
    STATIC_ASSERT(!test<std::nullptr_t>());

    STATIC_ASSERT(test<void*>());
    STATIC_ASSERT(test<int*>());
    STATIC_ASSERT(test<int*, void*>());

    STATIC_ASSERT(test<int[42]>());

    STATIC_ASSERT(test<int(int)>());

    template <int>
    struct Ordered {
        bool operator==(Ordered const&) const;
        bool operator!=(Ordered const&) const;
        bool operator<(Ordered const&) const;
        bool operator>(Ordered const&) const;
        bool operator<=(Ordered const&) const;
        bool operator>=(Ordered const&) const;
    };
    STATIC_ASSERT(test<Ordered<0>, Ordered<0>>());
    STATIC_ASSERT(test<Ordered<1>, Ordered<1>>());
    STATIC_ASSERT(test<Ordered<2>, Ordered<2>>());

    template <int I, int J>
    bool operator==(Ordered<I> const&, Ordered<J> const&);
    template <int I, int J>
    bool operator!=(Ordered<I> const&, Ordered<J> const&);
    template <int I, int J>
    bool operator<(Ordered<I> const&, Ordered<J> const&);
    template <int I, int J>
    bool operator>(Ordered<I> const&, Ordered<J> const&);
    template <int I, int J>
    bool operator<=(Ordered<I> const&, Ordered<J> const&);
    template <int I, int J>
    bool operator>=(Ordered<I> const&, Ordered<J> const&);

    struct Common { // a common type for Ordered<1> / <2>
        Common(Ordered<1>);
        Common(Ordered<2>);
        friend bool operator==(Common const&, Common const&);
        friend bool operator!=(Common const&, Common const&);
        friend bool operator<(Common const&, Common const&);
        friend bool operator>(Common const&, Common const&);
        friend bool operator<=(Common const&, Common const&);
        friend bool operator>=(Common const&, Common const&);
    };
    STATIC_ASSERT(test<Common, Common>());
} // namespace test_totally_ordered_with

template <>
struct std::common_type<test_totally_ordered_with::Ordered<1>, test_totally_ordered_with::Ordered<2>> {
    using type = test_totally_ordered_with::Common;
};
template <>
struct std::common_type<test_totally_ordered_with::Ordered<2>, test_totally_ordered_with::Ordered<1>> {
    using type = test_totally_ordered_with::Common;
};

namespace test_totally_ordered_with {
    // Ordered<0> / <1> have all the necessary operators, but fail common_reference_with
    STATIC_ASSERT(!test<Ordered<0>, Ordered<1>>());
    // Common is the common type of (Ordered<1>, Common) and (Ordered<2>, Common)
    STATIC_ASSERT(test<Ordered<1>, Common>());
    STATIC_ASSERT(test<Ordered<2>, Common>());
    // Ordered<1> / <2> have all the necessary operators, and model common_reference_with
    STATIC_ASSERT(test<Ordered<1>, Ordered<2>>());
} // namespace test_totally_ordered_with

namespace test_invocable_concepts {
    // Tests both invocable and regular_invocable concepts (since they're syntactically identical)

    using std::invocable, std::regular_invocable;

    template <class F, class... Args>
    constexpr bool test() {
        constexpr bool result = invocable<F, Args...>;
        STATIC_ASSERT(regular_invocable<F, Args...> == result);
        return result;
    }

    struct tag {};
    struct NotDerived {};

    void cc_independent_tests() {
        // PMD tests, which don't depend on calling conventions
        {
            using Fn = int tag::*;
            STATIC_ASSERT(!test<Fn>());
            {
                // N4849 [func.require]/1.4: "... N == 1 and f is a pointer to data member of a class T and
                // is_base_of_v<T, remove_reference_t<decltype(t_1)>> is true"
                STATIC_ASSERT(test<Fn, tag&>());
                STATIC_ASSERT(test<Fn, DerivesFrom<tag>&>());
                STATIC_ASSERT(test<Fn, tag&&>());
                STATIC_ASSERT(test<Fn, tag const&>());
            }
            {
                // N4849 [func.require]/1.5: "... N == 1 and f is a pointer to data member of a class T and
                // remove_cvref_t<decltype(t_1)> is a specialization of reference_wrapper"
                using T  = std::reference_wrapper<tag>;
                using DT = std::reference_wrapper<DerivesFrom<tag>>;
                using CT = std::reference_wrapper<tag const>;
                STATIC_ASSERT(test<Fn, T&>());
                STATIC_ASSERT(test<Fn, DT&>());
                STATIC_ASSERT(test<Fn, T const&>());
                STATIC_ASSERT(test<Fn, T&&>());
                STATIC_ASSERT(test<Fn, CT&>());
            }
            {
                // N4849 [func.require]/1.6: "... N == 1 and f is a pointer to data member of a class T and t_1 does not
                // satisfy the previous two items"
                using T  = tag*;
                using DT = DerivesFrom<tag>*;
                using CT = tag const*;
                using ST = std::unique_ptr<tag>;
                STATIC_ASSERT(test<Fn, T&>());
                STATIC_ASSERT(test<Fn, DT&>());
                STATIC_ASSERT(test<Fn, T const&>());
                STATIC_ASSERT(test<Fn, T&&>());
                STATIC_ASSERT(test<Fn, ST>());
                STATIC_ASSERT(test<Fn, CT&>());
            }
        }
        { // pointer to member data
            struct S {};
            using PMD = char S::*;
            STATIC_ASSERT(test<PMD, S&>());
            STATIC_ASSERT(test<PMD, S*>());
            STATIC_ASSERT(test<PMD, S* const>());
            STATIC_ASSERT(test<PMD, S const&>());
            STATIC_ASSERT(test<PMD, S const*>());
            STATIC_ASSERT(test<PMD, S volatile&>());
            STATIC_ASSERT(test<PMD, S volatile*>());
            STATIC_ASSERT(test<PMD, S const volatile&>());
            STATIC_ASSERT(test<PMD, S const volatile*>());
            STATIC_ASSERT(test<PMD, DerivesFrom<S>&>());
            STATIC_ASSERT(test<PMD, DerivesFrom<S> const&>());
            STATIC_ASSERT(test<PMD, DerivesFrom<S>*>());
            STATIC_ASSERT(test<PMD, DerivesFrom<S> const*>());
            STATIC_ASSERT(test<PMD, std::unique_ptr<S>>());
            STATIC_ASSERT(test<PMD, std::unique_ptr<S const>>());
            STATIC_ASSERT(test<PMD, std::reference_wrapper<S>>());
            STATIC_ASSERT(test<PMD, std::reference_wrapper<S const>>());
            STATIC_ASSERT(!test<PMD, NotDerived&>());
        }
    }

#define NAME      test_default_cc
#define CALLCONV  /* default convention */
#define MCALLCONV /* default convention */
#include "invocable_cc.hpp"

#define NAME      test_cdecl_cdecl
#define CALLCONV  __cdecl
#define MCALLCONV __cdecl
#include "invocable_cc.hpp"

#define NAME      test_fast_fast
#define CALLCONV  __fastcall
#define MCALLCONV __fastcall
#include "invocable_cc.hpp"

#define NAME      test_std_std
#define CALLCONV  __stdcall
#define MCALLCONV __stdcall
#include "invocable_cc.hpp"

#define NAME test_this
#define CALLCONV
#define MCALLCONV __thiscall
#include "invocable_cc.hpp"

#if !defined(_M_ARM) && !defined(_M_ARM64)
#define NAME      test_vector_vector
#define CALLCONV  __vectorcall
#define MCALLCONV __vectorcall
#include "invocable_cc.hpp"
#endif // ^^^ !ARM && !ARM64 ^^^

} // namespace test_invocable_concepts

namespace test_predicate {
    using std::predicate;

    struct Bool {
        operator bool() const;
    };

    struct tag {};

    struct NotCallableWithInt {
        int operator()(int) = delete;
        Bool operator()(tag);
    };

    struct S {
        using FreeFunc = Bool (*)(long);
        operator FreeFunc() const;
        Bool operator()(char, int&);
        Bool const& operator()(char, int&) const;
        Bool volatile& operator()(char, int&) volatile;
        Bool const volatile& operator()(char, int&) const volatile;
    };

    struct NotDerived {};

    void test() {
        {
            using Fn  = Bool (tag::*)(int);
            using RFn = Bool (tag::*)(int)&&;
            {
                // N4849 [func.require]/1.1: "... f is a pointer to member function of a class T and
                // is_base_of_v<T, remove_reference_t<decltype(t_1)>> is true"
                STATIC_ASSERT(predicate<Fn, tag&, int>);
                STATIC_ASSERT(predicate<Fn, DerivesFrom<tag>&, int>);
                STATIC_ASSERT(predicate<RFn, tag&&, int>);
                STATIC_ASSERT(!predicate<RFn, tag&, int>);
                STATIC_ASSERT(!predicate<Fn, tag&>);
                STATIC_ASSERT(!predicate<Fn, tag const&, int>);
            }
            {
                // N4849 [func.require]/1.2: "... f is a pointer to a member function of a class T and
                // remove_cvref_t<decltype(t_1)> is a specialization of reference_wrapper"
                using T  = std::reference_wrapper<tag>;
                using DT = std::reference_wrapper<DerivesFrom<tag>>;
                using CT = std::reference_wrapper<tag const>;
                STATIC_ASSERT(predicate<Fn, T&, int>);
                STATIC_ASSERT(predicate<Fn, DT&, int>);
                STATIC_ASSERT(predicate<Fn, T const&, int>);
                STATIC_ASSERT(predicate<Fn, T&&, int>);
                STATIC_ASSERT(!predicate<Fn, CT&, int>);
                STATIC_ASSERT(!predicate<RFn, T, int>);
            }
            {
                // N4849 [func.require]/1.3: "... f is a pointer to a member function of a class T and t_1 does not
                // satisfy the previous two items"
                using T  = tag*;
                using DT = DerivesFrom<tag>*;
                using CT = tag const*;
                using ST = std::unique_ptr<tag>;
                STATIC_ASSERT(predicate<Fn, T&, int>);
                STATIC_ASSERT(predicate<Fn, DT&, int>);
                STATIC_ASSERT(predicate<Fn, T const&, int>);
                STATIC_ASSERT(predicate<Fn, T&&, int>);
                STATIC_ASSERT(predicate<Fn, ST, int>);
                STATIC_ASSERT(!predicate<Fn, CT&, int>);
                STATIC_ASSERT(!predicate<RFn, T, int>);
            }
        }
        {
            using Fn = Bool(tag::*);
            STATIC_ASSERT(!predicate<Fn>);
            {
                // N4849 [func.require]/1.4: "... N == 1 and f is a pointer to data member of a class T and
                // is_base_of_v<T, remove_reference_t<decltype(t_1)>> is true"
                STATIC_ASSERT(predicate<Fn, tag&>);
                STATIC_ASSERT(predicate<Fn, DerivesFrom<tag>&>);
                STATIC_ASSERT(predicate<Fn, tag&&>);
                STATIC_ASSERT(predicate<Fn, tag const&>);
            }
            {
                // N4849 [func.require]/1.5: "... N == 1 and f is a pointer to data member of a class T and
                // remove_cvref_t<decltype(t_1)> is a specialization of reference_wrapper"
                using T  = std::reference_wrapper<tag>;
                using DT = std::reference_wrapper<DerivesFrom<tag>>;
                using CT = std::reference_wrapper<tag const>;
                STATIC_ASSERT(predicate<Fn, T&>);
                STATIC_ASSERT(predicate<Fn, DT&>);
                STATIC_ASSERT(predicate<Fn, T const&>);
                STATIC_ASSERT(predicate<Fn, T&&>);
                STATIC_ASSERT(predicate<Fn, CT&>);
            }
            {
                // N4849 [func.require]/1.6: "... N == 1 and f is a pointer to data member of a class T and t_1 does not
                // satisfy the previous two items"
                using T  = tag*;
                using DT = DerivesFrom<tag>*;
                using CT = tag const*;
                using ST = std::unique_ptr<tag>;
                STATIC_ASSERT(predicate<Fn, T&>);
                STATIC_ASSERT(predicate<Fn, DT&>);
                STATIC_ASSERT(predicate<Fn, T const&>);
                STATIC_ASSERT(predicate<Fn, T&&>);
                STATIC_ASSERT(predicate<Fn, ST>);
                STATIC_ASSERT(predicate<Fn, CT&>);
            }
        }
        { // N4849 [func.require]/1.7: "f(t_1, t_2, ..., t_N) in all other cases"
            { // function pointer
                using Fp = Bool (*)(tag&, int);
                STATIC_ASSERT(predicate<Fp, tag&, int>);
                STATIC_ASSERT(predicate<Fp, DerivesFrom<tag>&, int>);
                STATIC_ASSERT(!predicate<Fp, tag const&, int>);
                STATIC_ASSERT(!predicate<Fp>);
                STATIC_ASSERT(!predicate<Fp, tag&>);
            }
            { // function reference
                using Fp = Bool (&)(tag&, int);
                STATIC_ASSERT(predicate<Fp, tag&, int>);
                STATIC_ASSERT(predicate<Fp, DerivesFrom<tag>&, int>);
                STATIC_ASSERT(!predicate<Fp, tag const&, int>);
                STATIC_ASSERT(!predicate<Fp>);
                STATIC_ASSERT(!predicate<Fp, tag&>);
            }
            { // function object
                using Fn = NotCallableWithInt;
                STATIC_ASSERT(predicate<Fn, tag>);
                STATIC_ASSERT(!predicate<Fn, int>);
            }; // TRANSITION, semicolon needed to make clang-format happy (LLVM-48305)
        }

        { // function object
            STATIC_ASSERT(predicate<S, int>);
            STATIC_ASSERT(predicate<S&, unsigned char, int&>);
            STATIC_ASSERT(predicate<S const&, unsigned char, int&>);
            STATIC_ASSERT(!predicate<S volatile&, unsigned char, int&>);
            STATIC_ASSERT(!predicate<S const volatile&, unsigned char, int&>);

            STATIC_ASSERT(predicate<ImplicitTo<bool (*)(long)>, int>);
            STATIC_ASSERT(predicate<ImplicitTo<bool (*)(long)>, char>);
            STATIC_ASSERT(predicate<ImplicitTo<bool (*)(long)>, float>);
            STATIC_ASSERT(!predicate<ImplicitTo<bool (*)(long)>, char const*>);
            STATIC_ASSERT(!predicate<ImplicitTo<bool (*)(long)>, S const&>);

            auto omega_mu = [x = 42](int i, double) { return x == i; };
            STATIC_ASSERT(predicate<decltype((omega_mu)), int, double>);
            STATIC_ASSERT(predicate<decltype((omega_mu)), double, int>);
            STATIC_ASSERT(predicate<decltype((omega_mu)), char, char>);
            STATIC_ASSERT(!predicate<decltype((omega_mu))>);
            STATIC_ASSERT(!predicate<decltype((omega_mu)), char const*, double>);
            STATIC_ASSERT(!predicate<decltype((omega_mu)), double, char const*>);
        }

        { // pointer to function
            using RF0  = bool (&)();
            using RF1  = bool* (&) (int);
            using RF2  = bool& (&) (int, int);
            using RF3  = bool const& (&) (int, int, int);
            using RF4  = bool (&)(int, ...);
            using PF0  = bool (*)();
            using PF1  = bool* (*) (int);
            using PF2  = bool& (*) (int, int);
            using PF3  = bool const& (*) (int, int, int);
            using PF4  = bool (*)(int, ...);
            using RPF0 = bool (*&)();
            using RPF1 = bool* (*&) (int);
            using RPF2 = bool& (*&) (int, int);
            using RPF3 = bool const& (*&) (int, int, int);
            using RPF4 = bool (*&)(int, ...);
            STATIC_ASSERT(predicate<RF0>);
            STATIC_ASSERT(predicate<RF1, int>);
            STATIC_ASSERT(predicate<RF2, int, long>);
            STATIC_ASSERT(predicate<RF3, int, long, int>);
            STATIC_ASSERT(predicate<RF4, int, float, void*>);
            STATIC_ASSERT(predicate<PF0>);
            STATIC_ASSERT(predicate<PF1, int>);
            STATIC_ASSERT(predicate<PF2, int, long>);
            STATIC_ASSERT(predicate<PF3, int, long, int>);
            STATIC_ASSERT(predicate<PF4, int, float, void*>);
            STATIC_ASSERT(predicate<RPF0>);
            STATIC_ASSERT(predicate<RPF1, int>);
            STATIC_ASSERT(predicate<RPF2, int, long>);
            STATIC_ASSERT(predicate<RPF3, int, long, int>);
            STATIC_ASSERT(predicate<RPF4, int, float, void*>);
        }

        { // pointer to member function
            using PMF0  = Bool (S::*)();
            using PMF1  = Bool (S::*)(long);
            using PMF2  = Bool& (S::*) (long, int);
            using PMF1P = Bool const& (S::*) (int, ...);
            STATIC_ASSERT(predicate<PMF0, S>);
            STATIC_ASSERT(predicate<PMF0, S&>);
            STATIC_ASSERT(predicate<PMF0, S*>);
            STATIC_ASSERT(predicate<PMF0, S*&>);
            STATIC_ASSERT(predicate<PMF0, std::reference_wrapper<S>>);
            STATIC_ASSERT(predicate<PMF0, std::reference_wrapper<S> const&>);
            STATIC_ASSERT(predicate<PMF0, std::reference_wrapper<DerivesFrom<S>>>);
            STATIC_ASSERT(predicate<PMF0, std::reference_wrapper<DerivesFrom<S>> const&>);
            STATIC_ASSERT(predicate<PMF0, std::unique_ptr<S>>);
            STATIC_ASSERT(predicate<PMF0, std::unique_ptr<DerivesFrom<S>>>);
            STATIC_ASSERT(!predicate<PMF0, S const&>);
            STATIC_ASSERT(!predicate<PMF0, S volatile&>);
            STATIC_ASSERT(!predicate<PMF0, S const volatile&>);
            STATIC_ASSERT(!predicate<PMF0, NotDerived&>);
            STATIC_ASSERT(!predicate<PMF0, NotDerived const&>);
            STATIC_ASSERT(!predicate<PMF0, std::unique_ptr<S const>>);
            STATIC_ASSERT(!predicate<PMF0, std::reference_wrapper<S const>>);
            STATIC_ASSERT(!predicate<PMF0, std::reference_wrapper<NotDerived>>);
            STATIC_ASSERT(!predicate<PMF0, std::unique_ptr<NotDerived>>);

            STATIC_ASSERT(predicate<PMF1, S, int>);
            STATIC_ASSERT(predicate<PMF1, S&, int>);
            STATIC_ASSERT(predicate<PMF1, S*, int>);
            STATIC_ASSERT(predicate<PMF1, S*&, int>);
            STATIC_ASSERT(predicate<PMF1, std::unique_ptr<S>, int>);
            STATIC_ASSERT(predicate<PMF1, std::unique_ptr<DerivesFrom<S>>, int>);
            STATIC_ASSERT(predicate<PMF1, std::reference_wrapper<S>, int>);
            STATIC_ASSERT(predicate<PMF1, std::reference_wrapper<S> const&, int>);
            STATIC_ASSERT(predicate<PMF1, std::reference_wrapper<DerivesFrom<S>>, int>);
            STATIC_ASSERT(predicate<PMF1, std::reference_wrapper<DerivesFrom<S>> const&, int>);
            STATIC_ASSERT(!predicate<PMF1, S const&, int>);
            STATIC_ASSERT(!predicate<PMF1, S volatile&, int>);
            STATIC_ASSERT(!predicate<PMF1, S const volatile&, int>);
            STATIC_ASSERT(!predicate<PMF1, NotDerived&, int>);
            STATIC_ASSERT(!predicate<PMF1, NotDerived const&, int>);
            STATIC_ASSERT(!predicate<PMF1, std::unique_ptr<S const>, int>);
            STATIC_ASSERT(!predicate<PMF1, std::reference_wrapper<S const>, int>);
            STATIC_ASSERT(!predicate<PMF1, std::reference_wrapper<NotDerived>, int>);
            STATIC_ASSERT(!predicate<PMF1, std::unique_ptr<NotDerived>, int>);

            STATIC_ASSERT(predicate<PMF2, S, int, int>);
            STATIC_ASSERT(predicate<PMF2, S&, int, int>);
            STATIC_ASSERT(predicate<PMF2, S*, int, int>);
            STATIC_ASSERT(predicate<PMF2, S*&, int, int>);
            STATIC_ASSERT(predicate<PMF2, std::unique_ptr<S>, int, int>);
            STATIC_ASSERT(predicate<PMF2, std::unique_ptr<DerivesFrom<S>>, int, int>);
            STATIC_ASSERT(predicate<PMF2, std::reference_wrapper<S>, int, int>);
            STATIC_ASSERT(predicate<PMF2, std::reference_wrapper<S> const&, int, int>);
            STATIC_ASSERT(predicate<PMF2, std::reference_wrapper<DerivesFrom<S>>, int, int>);
            STATIC_ASSERT(predicate<PMF2, std::reference_wrapper<DerivesFrom<S>> const&, int, int>);
            STATIC_ASSERT(!predicate<PMF2, S const&, int, int>);
            STATIC_ASSERT(!predicate<PMF2, S volatile&, int, int>);
            STATIC_ASSERT(!predicate<PMF2, S const volatile&, int, int>);
            STATIC_ASSERT(!predicate<PMF2, std::unique_ptr<S const>, int, int>);
            STATIC_ASSERT(!predicate<PMF2, std::reference_wrapper<S const>, int, int>);
            STATIC_ASSERT(!predicate<PMF2, NotDerived const&, int, int>);
            STATIC_ASSERT(!predicate<PMF2, std::reference_wrapper<NotDerived>, int, int>);
            STATIC_ASSERT(!predicate<PMF2, std::unique_ptr<NotDerived>, int, int>);

            STATIC_ASSERT(predicate<PMF1P, S&, int>);
            STATIC_ASSERT(predicate<PMF1P, S&, int, long>);

            using PMF0C  = bool (S::*)() const;
            using PMF1C  = bool (S::*)(long) const;
            using PMF2C  = bool (S::*)(long, int) const;
            using PMF1PC = bool const& (S::*) (int, ...) const;
            STATIC_ASSERT(predicate<PMF0C, S>);
            STATIC_ASSERT(predicate<PMF0C, S&>);
            STATIC_ASSERT(predicate<PMF0C, S const&>);
            STATIC_ASSERT(predicate<PMF0C, S*>);
            STATIC_ASSERT(predicate<PMF0C, S const*>);
            STATIC_ASSERT(predicate<PMF0C, S*&>);
            STATIC_ASSERT(predicate<PMF0C, S const*&>);
            STATIC_ASSERT(predicate<PMF0C, std::unique_ptr<S>>);
            STATIC_ASSERT(predicate<PMF0C, std::unique_ptr<DerivesFrom<S>>>);
            STATIC_ASSERT(predicate<PMF0C, std::reference_wrapper<S>>);
            STATIC_ASSERT(predicate<PMF0C, std::reference_wrapper<S const>>);
            STATIC_ASSERT(predicate<PMF0C, std::reference_wrapper<S> const&>);
            STATIC_ASSERT(predicate<PMF0C, std::reference_wrapper<S const> const&>);
            STATIC_ASSERT(predicate<PMF0C, std::reference_wrapper<DerivesFrom<S>>>);
            STATIC_ASSERT(predicate<PMF0C, std::reference_wrapper<DerivesFrom<S> const>>);
            STATIC_ASSERT(predicate<PMF0C, std::reference_wrapper<DerivesFrom<S>> const&>);
            STATIC_ASSERT(predicate<PMF0C, std::reference_wrapper<DerivesFrom<S> const> const&>);
            STATIC_ASSERT(!predicate<PMF0C, S volatile&>);
            STATIC_ASSERT(!predicate<PMF0C, S const volatile&>);

            STATIC_ASSERT(predicate<PMF1C, S, int>);
            STATIC_ASSERT(predicate<PMF1C, S&, int>);
            STATIC_ASSERT(predicate<PMF1C, S const&, int>);
            STATIC_ASSERT(predicate<PMF1C, S*, int>);
            STATIC_ASSERT(predicate<PMF1C, S const*, int>);
            STATIC_ASSERT(predicate<PMF1C, S*&, int>);
            STATIC_ASSERT(predicate<PMF1C, S const*&, int>);
            STATIC_ASSERT(predicate<PMF1C, std::unique_ptr<S>, int>);
            STATIC_ASSERT(!predicate<PMF1C, S volatile&, int>);
            STATIC_ASSERT(!predicate<PMF1C, S const volatile&, int>);

            STATIC_ASSERT(predicate<PMF2C, S, int, int>);
            STATIC_ASSERT(predicate<PMF2C, S&, int, int>);
            STATIC_ASSERT(predicate<PMF2C, S const&, int, int>);
            STATIC_ASSERT(predicate<PMF2C, S*, int, int>);
            STATIC_ASSERT(predicate<PMF2C, S const*, int, int>);
            STATIC_ASSERT(predicate<PMF2C, S*&, int, int>);
            STATIC_ASSERT(predicate<PMF2C, S const*&, int, int>);
            STATIC_ASSERT(predicate<PMF2C, std::unique_ptr<S>, int, int>);
            STATIC_ASSERT(!predicate<PMF2C, S volatile&, int, int>);
            STATIC_ASSERT(!predicate<PMF2C, S const volatile&, int, int>);

            STATIC_ASSERT(predicate<PMF1PC, S&, int>);
            STATIC_ASSERT(predicate<PMF1PC, S&, int, long>);

            using PMF0V  = bool (S::*)() volatile;
            using PMF1V  = bool (S::*)(long) volatile;
            using PMF2V  = bool (S::*)(long, int) volatile;
            using PMF1PV = bool const& (S::*) (int, ...) volatile;
            STATIC_ASSERT(predicate<PMF0V, S>);
            STATIC_ASSERT(predicate<PMF0V, S&>);
            STATIC_ASSERT(predicate<PMF0V, S volatile&>);
            STATIC_ASSERT(predicate<PMF0V, S*>);
            STATIC_ASSERT(predicate<PMF0V, S volatile*>);
            STATIC_ASSERT(predicate<PMF0V, S*&>);
            STATIC_ASSERT(predicate<PMF0V, S volatile*&>);
            STATIC_ASSERT(predicate<PMF0V, std::unique_ptr<S>>);
            STATIC_ASSERT(!predicate<PMF0V, S const&>);
            STATIC_ASSERT(!predicate<PMF0V, S const volatile&>);

            STATIC_ASSERT(predicate<PMF1V, S, int>);
            STATIC_ASSERT(predicate<PMF1V, S&, int>);
            STATIC_ASSERT(predicate<PMF1V, S volatile&, int>);
            STATIC_ASSERT(predicate<PMF1V, S*, int>);
            STATIC_ASSERT(predicate<PMF1V, S volatile*, int>);
            STATIC_ASSERT(predicate<PMF1V, S*&, int>);
            STATIC_ASSERT(predicate<PMF1V, S volatile*&, int>);
            STATIC_ASSERT(predicate<PMF1V, std::unique_ptr<S>, int>);
            STATIC_ASSERT(!predicate<PMF1V, S const&, int>);
            STATIC_ASSERT(!predicate<PMF1V, S const volatile&, int>);

            STATIC_ASSERT(predicate<PMF2V, S, int, int>);
            STATIC_ASSERT(predicate<PMF2V, S&, int, int>);
            STATIC_ASSERT(predicate<PMF2V, S volatile&, int, int>);
            STATIC_ASSERT(predicate<PMF2V, S*, int, int>);
            STATIC_ASSERT(predicate<PMF2V, S volatile*, int, int>);
            STATIC_ASSERT(predicate<PMF2V, S*&, int, int>);
            STATIC_ASSERT(predicate<PMF2V, S volatile*&, int, int>);
            STATIC_ASSERT(predicate<PMF2V, std::unique_ptr<S>, int, int>);
            STATIC_ASSERT(!predicate<PMF2V, S const&, int, int>);
            STATIC_ASSERT(!predicate<PMF2V, S const volatile&, int, int>);

            STATIC_ASSERT(predicate<PMF1PV, S&, int>);
            STATIC_ASSERT(predicate<PMF1PV, S&, int, long>);

            using PMF0CV  = Bool (S::*)() const volatile;
            using PMF1CV  = Bool (S::*)(long) const volatile;
            using PMF2CV  = Bool (S::*)(long, int) const volatile;
            using PMF1PCV = Bool const& (S::*) (int, ...) const volatile;
            STATIC_ASSERT(predicate<PMF0CV, S>);
            STATIC_ASSERT(predicate<PMF0CV, S&>);
            STATIC_ASSERT(predicate<PMF0CV, S const&>);
            STATIC_ASSERT(predicate<PMF0CV, S volatile&>);
            STATIC_ASSERT(predicate<PMF0CV, S const volatile&>);
            STATIC_ASSERT(predicate<PMF0CV, S*>);
            STATIC_ASSERT(predicate<PMF0CV, S const*>);
            STATIC_ASSERT(predicate<PMF0CV, S volatile*>);
            STATIC_ASSERT(predicate<PMF0CV, S const volatile*>);
            STATIC_ASSERT(predicate<PMF0CV, S*&>);
            STATIC_ASSERT(predicate<PMF0CV, S const*&>);
            STATIC_ASSERT(predicate<PMF0CV, S volatile*&>);
            STATIC_ASSERT(predicate<PMF0CV, S const volatile*&>);
            STATIC_ASSERT(predicate<PMF0CV, std::unique_ptr<S>>);

            STATIC_ASSERT(predicate<PMF1CV, S, int>);
            STATIC_ASSERT(predicate<PMF1CV, S&, int>);
            STATIC_ASSERT(predicate<PMF1CV, S const&, int>);
            STATIC_ASSERT(predicate<PMF1CV, S volatile&, int>);
            STATIC_ASSERT(predicate<PMF1CV, S const volatile&, int>);
            STATIC_ASSERT(predicate<PMF1CV, S*, int>);
            STATIC_ASSERT(predicate<PMF1CV, S const*, int>);
            STATIC_ASSERT(predicate<PMF1CV, S volatile*, int>);
            STATIC_ASSERT(predicate<PMF1CV, S const volatile*, int>);
            STATIC_ASSERT(predicate<PMF1CV, S*&, int>);
            STATIC_ASSERT(predicate<PMF1CV, S const*&, int>);
            STATIC_ASSERT(predicate<PMF1CV, S volatile*&, int>);
            STATIC_ASSERT(predicate<PMF1CV, S const volatile*&, int>);
            STATIC_ASSERT(predicate<PMF1CV, std::unique_ptr<S>, int>);

            STATIC_ASSERT(predicate<PMF2CV, S, int, int>);
            STATIC_ASSERT(predicate<PMF2CV, S&, int, int>);
            STATIC_ASSERT(predicate<PMF2CV, S const&, int, int>);
            STATIC_ASSERT(predicate<PMF2CV, S volatile&, int, int>);
            STATIC_ASSERT(predicate<PMF2CV, S const volatile&, int, int>);
            STATIC_ASSERT(predicate<PMF2CV, S*, int, int>);
            STATIC_ASSERT(predicate<PMF2CV, S const*, int, int>);
            STATIC_ASSERT(predicate<PMF2CV, S volatile*, int, int>);
            STATIC_ASSERT(predicate<PMF2CV, S const volatile*, int, int>);
            STATIC_ASSERT(predicate<PMF2CV, S*&, int, int>);
            STATIC_ASSERT(predicate<PMF2CV, S const*&, int, int>);
            STATIC_ASSERT(predicate<PMF2CV, S volatile*&, int, int>);
            STATIC_ASSERT(predicate<PMF2CV, S const volatile*&, int, int>);
            STATIC_ASSERT(predicate<PMF2CV, std::unique_ptr<S>, int, int>);

            STATIC_ASSERT(predicate<PMF1PCV, S&, int>);
            STATIC_ASSERT(predicate<PMF1PCV, S&, int, long>);
        }

        { // pointer to member data
            using PMD = bool S::*;
            STATIC_ASSERT(predicate<PMD, S&>);
            STATIC_ASSERT(predicate<PMD, S*>);
            STATIC_ASSERT(predicate<PMD, S* const>);
            STATIC_ASSERT(predicate<PMD, S const&>);
            STATIC_ASSERT(predicate<PMD, S const*>);
            STATIC_ASSERT(predicate<PMD, S volatile&>);
            STATIC_ASSERT(predicate<PMD, S volatile*>);
            STATIC_ASSERT(predicate<PMD, S const volatile&>);
            STATIC_ASSERT(predicate<PMD, S const volatile*>);
            STATIC_ASSERT(predicate<PMD, DerivesFrom<S>&>);
            STATIC_ASSERT(predicate<PMD, DerivesFrom<S> const&>);
            STATIC_ASSERT(predicate<PMD, DerivesFrom<S>*>);
            STATIC_ASSERT(predicate<PMD, DerivesFrom<S> const*>);
            STATIC_ASSERT(predicate<PMD, std::unique_ptr<S>>);
            STATIC_ASSERT(predicate<PMD, std::unique_ptr<S const>>);
            STATIC_ASSERT(predicate<PMD, std::reference_wrapper<S>>);
            STATIC_ASSERT(predicate<PMD, std::reference_wrapper<S const>>);
            STATIC_ASSERT(!predicate<PMD, NotDerived&>);
        }
    }
} // namespace test_predicate

namespace test_relation {
    // Tests relation, equivalence_relation, and strict_weak_order (since they are syntactically identical)

    using std::relation, std::equivalence_relation, std::strict_weak_order;

    template <class F, class T, class U = T>
    constexpr bool test() {
        constexpr bool result = relation<F, T, U>;
        STATIC_ASSERT(relation<F, U, T> == result);
        STATIC_ASSERT(equivalence_relation<F, T, U> == result);
        STATIC_ASSERT(equivalence_relation<F, U, T> == result);
        STATIC_ASSERT(strict_weak_order<F, T, U> == result);
        STATIC_ASSERT(strict_weak_order<F, U, T> == result);
        return result;
    }

    STATIC_ASSERT(test<std::equal_to<>, int, long>());
    STATIC_ASSERT(test<std::less<>, int*, void*>());

    struct Equivalent {
        // clang-format off
        template <class T, class U>
        constexpr decltype(auto) operator()(T&& t, U&& u) const
            requires requires { static_cast<T&&>(t) == static_cast<U&&>(u); } {
            return static_cast<T&&>(t) == static_cast<U&&>(u);
        }
        // clang-format on
    };
    STATIC_ASSERT(test<Equivalent, int, long>());

    struct Bool {
        operator bool() const;
    };

    template <unsigned int>
    struct A {};
    // clang-format off
    template <unsigned int U>
        requires (0 < U)
    Bool operator==(A<U>, A<U>); // A<0> == A<0> is invalid
    // clang-format on
    STATIC_ASSERT(!test<Equivalent, A<0>>());
    STATIC_ASSERT(test<Equivalent, A<1>>());

    template <unsigned int>
    struct B {};
    void operator==(B<1>, B<1>); // B<1> == B<1> does not model _Boolean_testable
    template <unsigned int U>
    bool operator==(B<U>, B<U>);
    STATIC_ASSERT(test<Equivalent, B<0>>());
    STATIC_ASSERT(!test<Equivalent, B<1>>());

    // clang-format off
    template <unsigned int I>
        requires (2 != I)
    bool operator==(A<I>, B<I>); // A<2> == B<2> rewrites to B<2> == A<2>
    template <unsigned int I>
        requires (3 != I)
    bool operator==(B<I>, A<I>); // B<3> == A<3> rewrites to A<3> == B<3>
    // clang-format on

    STATIC_ASSERT(!test<Equivalent, A<0>, B<0>>());
    STATIC_ASSERT(!test<Equivalent, A<1>, B<1>>());
    STATIC_ASSERT(test<Equivalent, A<2>, B<2>>());
    STATIC_ASSERT(test<Equivalent, A<3>, B<3>>());
    STATIC_ASSERT(test<Equivalent, A<4>, B<4>>());

    template <unsigned int I>
    struct C {};
    enum E : bool { No, Yes };
    E operator==(C<0>&, C<0>&); // const C<0> == const C<0> is invalid
    // clang-format off
    template <unsigned int I>
        requires (0 != I)
    E operator==(C<I>, C<I>);
    // clang-format on

    STATIC_ASSERT(!test<Equivalent, const C<0>>());
    STATIC_ASSERT(test<Equivalent, const C<1>>());
} // namespace test_relation

namespace test_uniform_random_bit_generator {
    using std::uniform_random_bit_generator;

    STATIC_ASSERT(uniform_random_bit_generator<std::minstd_rand0>);
    STATIC_ASSERT(uniform_random_bit_generator<std::minstd_rand>);
    STATIC_ASSERT(uniform_random_bit_generator<std::mt19937>);
    STATIC_ASSERT(uniform_random_bit_generator<std::mt19937_64>);
    STATIC_ASSERT(uniform_random_bit_generator<std::ranlux24_base>);
    STATIC_ASSERT(uniform_random_bit_generator<std::ranlux48_base>);
    STATIC_ASSERT(uniform_random_bit_generator<std::ranlux24>);
    STATIC_ASSERT(uniform_random_bit_generator<std::ranlux48>);
    STATIC_ASSERT(uniform_random_bit_generator<std::knuth_b>);
    STATIC_ASSERT(uniform_random_bit_generator<std::default_random_engine>);
    STATIC_ASSERT(uniform_random_bit_generator<std::random_device>);

    struct NoCall {
        static constexpr unsigned int min() {
            return 0;
        }
        static constexpr unsigned int max() {
            return 42;
        }
    };
    STATIC_ASSERT(!uniform_random_bit_generator<NoCall>);

    struct NoLvalueCall {
        static constexpr unsigned int min() {
            return 0;
        }
        static constexpr unsigned int max() {
            return 42;
        }
        unsigned int operator()() &&;
    };
    STATIC_ASSERT(!uniform_random_bit_generator<NoLvalueCall>);

    struct SignedValue {
        static constexpr int min() {
            return 0;
        }
        static constexpr int max() {
            return 42;
        }
        int operator()();
    };
    STATIC_ASSERT(!uniform_random_bit_generator<SignedValue>);

    struct NoMin {
        static constexpr unsigned int max() {
            return 42;
        }
        unsigned int operator()();
    };
    STATIC_ASSERT(!uniform_random_bit_generator<NoMin>);

    struct NonConstexprMin {
        static unsigned int min() {
            return 0;
        }
        static constexpr unsigned int max() {
            return 42;
        }
        unsigned int operator()();
    };
    STATIC_ASSERT(!uniform_random_bit_generator<NonConstexprMin>);

    struct BadMin {
        static constexpr int min() {
            return 0;
        }
        static constexpr unsigned int max() {
            return 42;
        }
        unsigned int operator()();
    };
    STATIC_ASSERT(!uniform_random_bit_generator<BadMin>);

    struct NoMax {
        static constexpr unsigned int min() {
            return 0;
        }
        unsigned int operator()();
    };
    STATIC_ASSERT(!uniform_random_bit_generator<NoMax>);

    struct NonConstexprMax {
        static constexpr unsigned int min() {
            return 0;
        }
        static unsigned int max() {
            return 42;
        }
        unsigned int operator()();
    };
    STATIC_ASSERT(!uniform_random_bit_generator<NonConstexprMax>);

    struct BadMax {
        static constexpr unsigned int min() {
            return 0;
        }
        static constexpr int max() {
            return 42;
        }
        unsigned int operator()();
    };
    STATIC_ASSERT(!uniform_random_bit_generator<BadMax>);

    struct EmptyRange {
        static constexpr unsigned int min() {
            return 0;
        }
        static constexpr unsigned int max() {
            return 0;
        }
        unsigned int operator()();
    };
    STATIC_ASSERT(!uniform_random_bit_generator<EmptyRange>);

    struct ReversedRange {
        static constexpr unsigned int min() {
            return 42;
        }
        static constexpr unsigned int max() {
            return 0;
        }
        unsigned int operator()();
    };
    STATIC_ASSERT(!uniform_random_bit_generator<ReversedRange>);

    struct URBG {
        static constexpr unsigned int min() {
            return 0;
        }
        static constexpr unsigned int max() {
            return 42;
        }
        unsigned int operator()();
    };
    STATIC_ASSERT(uniform_random_bit_generator<URBG>);
    STATIC_ASSERT(!uniform_random_bit_generator<const URBG>);

    struct ConstURBG {
        static constexpr unsigned int min() {
            return 0;
        }
        static constexpr unsigned int max() {
            return 42;
        }
        unsigned int operator()() const;
    };
    STATIC_ASSERT(uniform_random_bit_generator<ConstURBG>);
    STATIC_ASSERT(uniform_random_bit_generator<const ConstURBG>);
} // namespace test_uniform_random_bit_generator

int main() {
    test_ranges_swap::runtime_tests();
    test_swappable_with::example::test();
}
