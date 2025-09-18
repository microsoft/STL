// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <bitset>
#include <cassert>
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

#include <is_permissive.hpp>

template <class, class = void>
constexpr bool is_trait = false;
template <class T>
constexpr bool is_trait<T, std::void_t<typename T::type>> = true;

struct IncompleteClass;
union IncompleteUnion;

template <class T>
struct DoNotInstantiate {
    static_assert(false);
};

struct Immobile {
    Immobile()                           = default;
    Immobile(Immobile const&)            = delete;
    Immobile& operator=(Immobile const&) = delete;
};

struct MoveOnly {
    MoveOnly()                      = default;
    MoveOnly(MoveOnly&&)            = default;
    MoveOnly& operator=(MoveOnly&&) = default;
};

struct CopyOnlyAbomination {
    CopyOnlyAbomination()                                      = default;
    CopyOnlyAbomination(CopyOnlyAbomination const&)            = default;
    CopyOnlyAbomination(CopyOnlyAbomination&&)                 = delete;
    CopyOnlyAbomination& operator=(CopyOnlyAbomination const&) = default;
    CopyOnlyAbomination& operator=(CopyOnlyAbomination&&)      = delete;
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
    constexpr ConvertsFrom(Second) noexcept
        requires (!std::is_same_v<IncompleteClass, Second>)
    {}
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
        static_assert(same_as<T, U>);
        static_assert(same_as<T const, U const>);
        static_assert(same_as<T volatile, U volatile>);
        static_assert(same_as<T const volatile, U const volatile>);

        constexpr bool is_ref = std::is_reference_v<T>;

        static_assert(same_as<T const, U> == is_ref);
        static_assert(same_as<T volatile, U> == is_ref);
        static_assert(same_as<T const volatile, U> == is_ref);
        static_assert(same_as<T, U const> == is_ref);
        static_assert(same_as<T, U volatile> == is_ref);
        static_assert(same_as<T, U const volatile> == is_ref);

        if constexpr (!std::is_void_v<T>) {
            static_assert(same_as<T&, U&>);
            static_assert(same_as<T const&, U&> == is_ref);
            static_assert(same_as<T&&, U&&>);
            static_assert(same_as<T const&&, U&&> == is_ref);
        }

        if constexpr (!is_ref) {
            static_assert(same_as<T*, U*>);
            static_assert(same_as<T**, U**>);
        }

        return true;
    }

    static_assert(test<void, void>());
    static_assert(test<int, int>());
    static_assert(!same_as<int, void>);
    static_assert(test<double, double>());
    static_assert(!same_as<double, int>);
    static_assert(!same_as<int, double>);

    static_assert(test<int[42], int[42]>());
    static_assert(test<int[], int[]>());

    static_assert(test<NonTriviallyDestructible, NonTriviallyDestructible>());
    static_assert(!same_as<void, NonTriviallyDestructible>);
    static_assert(!same_as<NonTriviallyDestructible, int*>);

    static_assert(test<int&, int&>());
    static_assert(!same_as<int*, int&>);
    static_assert(!same_as<int&, int>);
    static_assert(test<int (&)[42], int (&)[42]>());
    static_assert(test<int (&)(), int (&)()>());

    static_assert(same_as<void() const, void() const>);
    static_assert(same_as<void() &, void() &>);

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

    static_assert(!f<int*, int*>());
    static_assert(!f<void, void>());

    static_assert(f<int, int>()); // ambiguous if second overload's requirements do not subsume the first's
    static_assert(f<long, long>()); // Ditto
} // namespace test_same_as

namespace test_derived_from {
    using std::derived_from;

    static_assert(!derived_from<int, int>);
    static_assert(!derived_from<void, void>);
    static_assert(!derived_from<void(), void()>);
    static_assert(!derived_from<void() const, void() const>);
    static_assert(!derived_from<int[42], int[42]>);
    static_assert(!derived_from<int&, int>);
    static_assert(!derived_from<int, int&>);
    static_assert(!derived_from<int&, int&>);

    template <class Derived, class Base>
    constexpr bool test() {
        static_assert(derived_from<Derived, Base>);
        static_assert(derived_from<Derived const, Base>);
        static_assert(derived_from<Derived volatile, Base>);
        static_assert(derived_from<Derived const volatile, Base>);
        static_assert(derived_from<Derived, Base const>);
        static_assert(derived_from<Derived const, Base const>);
        static_assert(derived_from<Derived volatile, Base const>);
        static_assert(derived_from<Derived const volatile, Base const>);
        static_assert(derived_from<Derived, Base volatile>);
        static_assert(derived_from<Derived const, Base volatile>);
        static_assert(derived_from<Derived volatile, Base volatile>);
        static_assert(derived_from<Derived const volatile, Base volatile>);
        static_assert(derived_from<Derived, Base const volatile>);
        static_assert(derived_from<Derived const, Base const volatile>);
        static_assert(derived_from<Derived volatile, Base const volatile>);
        static_assert(derived_from<Derived const volatile, Base const volatile>);
        return true;
    }

    static_assert(test<SimpleBase, SimpleBase>());
    static_assert(!derived_from<SimpleBase&, SimpleBase&>);

    template <int>
    struct Middle : SimpleBase {};

    static_assert(test<Middle<0>, SimpleBase>());
    static_assert(!derived_from<SimpleBase, Middle<0>>);
    static_assert(!derived_from<Middle<0>&, SimpleBase&>);
    static_assert(!derived_from<Middle<0>*, SimpleBase*>);
    static_assert(!derived_from<Middle<0>[42], SimpleBase[42]>);

    static_assert(test<Middle<1>, SimpleBase>());
    static_assert(!derived_from<SimpleBase, Middle<1>>);

    static_assert(test<DerivesFrom<Middle<0>, Middle<1>>, Middle<0>>());
    static_assert(test<DerivesFrom<Middle<0>, Middle<1>>, Middle<1>>());
    static_assert(!derived_from<DerivesFrom<Middle<0>, Middle<1>>, SimpleBase>);

    class PrivateDerived : private Middle<0>, private Middle<1> {
    public:
        void f();
    };

    void PrivateDerived::f() {
        // Check these in a member to verify that access doesn't depend on context
#ifndef __EDG__ // TRANSITION, VSO-1898937
        static_assert(!derived_from<PrivateDerived, Middle<0>>);
        static_assert(!derived_from<PrivateDerived, Middle<1>>);
#endif // ^^^ no workaround ^^^
    }

    static_assert(!derived_from<PrivateDerived, SimpleBase>);

    static_assert(test<IncompleteClass, IncompleteClass>());

    static_assert(!derived_from<IncompleteUnion, IncompleteUnion>);
    static_assert(!derived_from<IncompleteClass, IncompleteUnion>);
    static_assert(!derived_from<IncompleteUnion, IncompleteClass>);
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
        static_assert(convertible_to<From const, To> == result);
        static_assert(convertible_to<From, To const> == result);
        static_assert(convertible_to<From const, To const> == result);
        return result;
    }
#pragma warning(pop)

    // void
    static_assert(test<void, void>());
    static_assert(!test<void, fn>());
    static_assert(!test<void, fn&>());
    static_assert(!test<void, fn*>());
    static_assert(!test<void, char_array>());
    static_assert(!test<void, char_array&>());
    static_assert(!test<void, char>());
    static_assert(!test<void, char&>());
    static_assert(!test<void, char*>());
    static_assert(!test<char, void>());

    // fn
    static_assert(!test<fn, void>());
    static_assert(!test<fn, fn>());
    static_assert(test<fn, fn&>());
    static_assert(test<fn, fn*>());
    static_assert(test<fn, fn* const>());

    static_assert(convertible_to<fn, fn&&>);

    static_assert(!test<fn, char_array>());
    static_assert(!test<fn, char_array&>());
    static_assert(!test<fn, char>());
    static_assert(!test<fn, char&>());
    static_assert(!test<fn, char*>());

    // fn&
    static_assert(!test<fn&, void>());
    static_assert(!test<fn&, fn>());
    static_assert(test<fn&, fn&>());

    static_assert(test<fn&, fn*>());
    static_assert(!test<fn&, char_array>());
    static_assert(!test<fn&, char_array&>());
    static_assert(!test<fn&, char>());
    static_assert(!test<fn&, char&>());
    static_assert(!test<fn&, char*>());

    // fn*
    static_assert(!test<fn*, void>());
    static_assert(!test<fn*, fn>());
    static_assert(!test<fn*, fn&>());
    static_assert(test<fn*, fn*>());

    static_assert(!test<fn*, char_array>());
    static_assert(!test<fn*, char_array&>());
    static_assert(!test<fn*, char>());
    static_assert(!test<fn*, char&>());
    static_assert(!test<fn*, char*>());

    // Abominable function
    static_assert(!convertible_to<const_fn, fn>);
    static_assert(!convertible_to<const_fn, fn*>);
    static_assert(!convertible_to<const_fn, fn&>);
    static_assert(!convertible_to<const_fn, fn&&>);
    static_assert(!convertible_to<fn*, const_fn>);
    static_assert(!convertible_to<fn&, const_fn>);
    static_assert(!convertible_to<const_fn, const_fn>);
    static_assert(!convertible_to<const_fn, void>);

    // char_array
    static_assert(!test<char_array, void>());
    static_assert(!test<char_array, fn>());
    static_assert(!test<char_array, fn&>());
    static_assert(!test<char_array, fn*>());
    static_assert(!test<char_array, char_array>());

    static_assert(!convertible_to<char_array, char_array&>);
    static_assert(convertible_to<char_array, char_array const&>);
    static_assert(!convertible_to<char_array, char_array const volatile&>);

    static_assert(!convertible_to<char_array const, char_array&>);
    static_assert(convertible_to<char_array const, char_array const&>);
    static_assert(!convertible_to<char_array, char_array volatile&>);
    static_assert(!convertible_to<char_array, char_array const volatile&>);

    static_assert(convertible_to<char_array, char_array&&>);
    static_assert(convertible_to<char_array, char_array const&&>);
    static_assert(convertible_to<char_array, char_array volatile&&>);
    static_assert(convertible_to<char_array, char_array const volatile&&>);
    static_assert(convertible_to<char_array const, char_array const&&>);
    static_assert(!convertible_to<char_array&, char_array&&>);
    static_assert(!convertible_to<char_array&&, char_array&>);

    static_assert(!test<char_array, char>());
    static_assert(!test<char_array, char&>());

    static_assert(convertible_to<char_array, char*>);
    static_assert(convertible_to<char_array, char const*>);
    static_assert(convertible_to<char_array, char* const>);
    static_assert(convertible_to<char_array, char* const volatile>);

    static_assert(!convertible_to<char_array const, char*>);
    static_assert(convertible_to<char_array const, char const*>);

    static_assert(!convertible_to<char[42][42], char*>);
    static_assert(!convertible_to<char[][1], char*>);

    // char_array&
    static_assert(!test<char_array&, void>());
    static_assert(!test<char_array&, fn>());
    static_assert(!test<char_array&, fn&>());
    static_assert(!test<char_array&, fn*>());
    static_assert(!test<char_array&, char_array>());

    static_assert(convertible_to<char_array&, char_array&>);
    static_assert(convertible_to<char_array&, char_array const&>);
    static_assert(!convertible_to<char_array const&, char_array&>);
    static_assert(convertible_to<char_array const&, char_array const&>);

    static_assert(!test<char_array&, char>());
    static_assert(!test<char_array&, char&>());

    static_assert(convertible_to<char_array&, char*>);
    static_assert(convertible_to<char_array&, char const*>);
    static_assert(!convertible_to<char_array const&, char*>);
    static_assert(convertible_to<char_array const&, char const*>);

    static_assert(convertible_to<char_array, ConvertsFrom<char const*>>);
    static_assert(convertible_to<char (&)[], ConvertsFrom<char const*>>);

    // volatile array glvalues
    static_assert(convertible_to<int volatile (&)[42], int volatile (&)[42]>);
    static_assert(convertible_to<int volatile (&)[42][13], int volatile (&)[42][13]>);
    static_assert(convertible_to<int volatile (&&)[42], int volatile (&&)[42]>);
    static_assert(convertible_to<int volatile (&&)[42][13], int volatile (&&)[42][13]>);


    // char
    static_assert(!test<char, void>());
    static_assert(!test<char, fn>());
    static_assert(!test<char, fn&>());
    static_assert(!test<char, fn*>());
    static_assert(!test<char, char_array>());
    static_assert(!test<char, char_array&>());

    static_assert(test<char, char>());

    static_assert(!convertible_to<char, char&>);
    static_assert(convertible_to<char, char const&>);
    static_assert(!convertible_to<char const, char&>);
    static_assert(convertible_to<char const, char const&>);

    static_assert(!test<char, char*>());

    // char&
    static_assert(!test<char&, void>());
    static_assert(!test<char&, fn>());
    static_assert(!test<char&, fn&>());
    static_assert(!test<char&, fn*>());
    static_assert(!test<char&, char_array>());
    static_assert(!test<char&, char_array&>());

    static_assert(test<char&, char>());

    static_assert(convertible_to<char&, char&>);
    static_assert(convertible_to<char&, char const&>);
    static_assert(!convertible_to<char const&, char&>);
    static_assert(convertible_to<char const&, char const&>);

    static_assert(!test<char&, char*>());

    // char*
    static_assert(!test<char*, void>());
    static_assert(!test<char*, fn>());
    static_assert(!test<char*, fn&>());
    static_assert(!test<char*, fn*>());
    static_assert(!test<char*, char_array>());
    static_assert(!test<char*, char_array&>());

    static_assert(!test<char*, char>());
    static_assert(!test<char*, char&>());

    static_assert(convertible_to<char*, char*>);
    static_assert(convertible_to<char*, char const*>);
    static_assert(!convertible_to<char const*, char*>);
    static_assert(convertible_to<char const*, char const*>);

    static_assert(convertible_to<Immobile&, Immobile&>);
    static_assert(convertible_to<Immobile&, Immobile const&>);
    static_assert(convertible_to<Immobile&, Immobile const volatile&>);
    static_assert(convertible_to<Immobile&, Immobile volatile&>);
    static_assert(convertible_to<Immobile const&, Immobile const&>);
    static_assert(convertible_to<Immobile const&, Immobile const volatile&>);
    static_assert(convertible_to<Immobile volatile&, Immobile const volatile&>);
    static_assert(convertible_to<Immobile const volatile&, Immobile const volatile&>);
    static_assert(!convertible_to<Immobile const&, Immobile&>);

    static_assert(!test<Immobile&, Immobile>());
    static_assert(!test<Immobile const&, Immobile>());
    static_assert(!test<Immobile, Immobile>());

    // Ensure that DoNotInstantiate is not instantiated by is_convertible when it is not needed.
    // (For example, DoNotInstantiate is instantiated by ADL lookup for arguments of type DoNotInstantiate*.)
    static_assert(convertible_to<DoNotInstantiate<int>*, DoNotInstantiate<int>*>);

    static_assert(test<SimpleBase, SimpleBase>());
    static_assert(test<SimpleDerived, SimpleDerived>());
    static_assert(test<SimpleDerived, SimpleBase>());
    static_assert(!test<SimpleBase, SimpleDerived>());

    struct ImplicitConversionOnly;
    struct Target {
        Target()                                = default;
        explicit Target(ImplicitConversionOnly) = delete;
    };
    struct ImplicitConversionOnly {
        operator Target() const;
    };
    static_assert(test<ImplicitTo<Target>, Target>());
    static_assert(!test<ExplicitTo<Target>, Target>());
    static_assert(!test<ImplicitConversionOnly, Target>());

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

        static_assert(f(ConvertsFrom<int>{}) == result::exact); // the most specialized overload is chosen
        constexpr ConvertsFrom<int> cfi{};
        static_assert(f(cfi) == result::exact); // Ditto with qualification conversion
        static_assert(f(42) == result::convertible); // the more-constrained overload is chosen
        static_assert(f("meow") == result::unrelated); // everything isn't completely broken
    } // namespace overloading
} // namespace test_convertible_to

namespace test_common_reference_with {
    using std::common_reference_t, std::common_reference_with;

    template <class T, class U>
    constexpr bool test() {
        constexpr bool result = common_reference_with<T, U>;
        static_assert(common_reference_with<U, T> == result);
        return result;
    }

    static_assert(test<int, int>());
    static_assert(test<int, double>());
    static_assert(test<double, int>());
    static_assert(test<double, double>());
    static_assert(!test<void, int>());
    static_assert(!test<int*, int>());
    static_assert(test<void*, int*>());
    static_assert(test<double, long long>());
    static_assert(test<void, void>());

    // common_reference_t<<ExplicitTo<EmptyClass>, EmptyClass> is EmptyClass, to which ExplicitTo<EmptyClass> is
    // not implicitly convertible
    static_assert(!test<ExplicitTo<EmptyClass>, EmptyClass>());

    static_assert(test<Immobile&, DerivesFrom<Immobile>&>());
    static_assert(test<Immobile&&, DerivesFrom<Immobile>&&>());
    static_assert(!test<Immobile, DerivesFrom<Immobile>>());

    static_assert(test<SimpleBase&, SimpleDerived&>());
    static_assert(test<SimpleBase&, SimpleDerived const&>());
    static_assert(test<SimpleBase const&, SimpleDerived&>());
    static_assert(test<SimpleBase const&, SimpleDerived const&>());

    static_assert(test<SimpleBase&&, SimpleDerived&&>());
    static_assert(test<SimpleBase&&, SimpleDerived const&&>());
    static_assert(test<SimpleBase const&&, SimpleDerived&&>());
    static_assert(test<SimpleBase const&&, SimpleDerived const&&>());

    static_assert(test<SimpleBase&, SimpleDerived&&>());
    static_assert(test<SimpleBase&, SimpleDerived const&&>());
    static_assert(test<SimpleBase const&, SimpleDerived&&>());
    static_assert(test<SimpleBase const&, SimpleDerived const&&>());

    static_assert(test<SimpleBase&&, SimpleDerived&>());
    static_assert(test<SimpleBase&&, SimpleDerived const&>());
    static_assert(test<SimpleBase const&&, SimpleDerived&>());
    static_assert(test<SimpleBase const&&, SimpleDerived const&>());

    // https://github.com/ericniebler/stl2/issues/338
    static_assert(test<int&, ConvertsFrom<int&>>());

    static_assert(!test<MoveOnly const&, MoveOnly>());
    static_assert(test<MoveOnly const&, MoveOnly&>());
    static_assert(!test<DerivesFrom<MoveOnly> const&, MoveOnly>());
    static_assert(test<DerivesFrom<MoveOnly> const&, MoveOnly&&>());
    static_assert(!test<MoveOnly const&, DerivesFrom<MoveOnly>>());
    static_assert(test<MoveOnly const&, DerivesFrom<MoveOnly> const&>());

    static_assert(
        std::is_same_v<Interconvertible<2>, common_reference_t<Interconvertible<0>&, Interconvertible<1> const&>>);
    static_assert(!test<Interconvertible<0>&, Interconvertible<1> const&>());

    static_assert(test<SimpleBase, ConvertsFrom<int, SimpleBase>>());

    static_assert(test<int volatile&, int volatile&>());
    static_assert(test<int volatile (&)[42], int volatile (&)[42]>());
    static_assert(test<int volatile (&)[42][13], int volatile (&)[42][13]>());
    static_assert(test<int volatile (&&)[42], int volatile (&&)[42]>());
    static_assert(test<int volatile (&&)[42][13], int volatile (&&)[42][13]>());
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
        static_assert(common_with<U, T> == result);
        return result;
    }

    static_assert(test<int, double>());
    static_assert(test<double, int>());
    static_assert(test<double, double>());
    static_assert(test<void*, int*>());
    static_assert(test<double, long long>());

    static_assert(test<int, int>());
    static_assert(test<int, int const>());
    static_assert(test<int const, int const>());

    static_assert(test<long, int const>());
    static_assert(test<long const, int>());
    static_assert(test<long, int volatile>());
    static_assert(test<long volatile, int>());
    static_assert(test<long const, int const>());

    static_assert(test<double, char>());
    static_assert(test<short, char>());

    static_assert(test<void, void>());
    static_assert(test<void, void const>());
    static_assert(test<void const, void>());
    static_assert(test<void, void volatile>());
    static_assert(test<void volatile, void>());
    static_assert(test<void const, void const>());

    static_assert(!test<void, int>());
    static_assert(!test<int*, int>());

    static_assert(test<std::reference_wrapper<int>, int>());

    static_assert(test<ExplicitFromConstLvalue<int>, ExplicitFromConstLvalue<int>>());
    static_assert(!test<int, ExplicitFromConstLvalue<int>>());

    static_assert(test<void*, IncompleteClass*>());
    static_assert(test<void*, DoNotInstantiate<int>*>());

    static_assert(is_same_v<common_type_t<ExplicitTo<EmptyClass>, EmptyClass>, EmptyClass>);
    static_assert(is_same_v<common_type_t<EmptyClass, ExplicitTo<EmptyClass>>, EmptyClass>);
    static_assert(!test<ExplicitTo<EmptyClass>, EmptyClass>());

    static_assert(test<SimpleBase, SimpleDerived>());
    static_assert(test<SimpleBase, ConvertsFrom<SimpleBase, int>>());

    static_assert(is_same_v<common_type_t<Immobile, DerivesFrom<Immobile>>, Immobile>);
    static_assert(is_same_v<common_type_t<DerivesFrom<Immobile>, Immobile>, Immobile>);
    static_assert(!test<Immobile, DerivesFrom<Immobile>>());
} // namespace test_common_with

namespace test_integral_concepts {
    using std::integral, std::signed_integral, std::unsigned_integral;

    namespace subsumption {
        enum class is { not_integral, integral, signed_integral, unsigned_integral, ull };

        template <class T>
        constexpr is f(T) {
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
            static_assert(f(0.0f) == is::not_integral);
            static_assert(f(0.0) == is::not_integral);
            static_assert(f(0) == is::signed_integral);
            static_assert(f(0u) == is::unsigned_integral);
            static_assert(f('a') == is::signed_integral || f('a') == is::unsigned_integral);
            static_assert(f(nullptr) == is::not_integral);
            static_assert(f(0ull) == is::ull);
            static_assert(f(static_cast<int*>(nullptr)) == is::not_integral);
            struct A {};
            static_assert(f(static_cast<int A::*>(nullptr)) == is::not_integral);
            static_assert(f(static_cast<int (A::*)()>(nullptr)) == is::not_integral);
            return true;
        }
        static_assert(test_subsumption());
    } // namespace subsumption

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    enum class is_signed { no, yes, NA };

    template <class T, is_signed S>
    constexpr bool test_integral() {
        constexpr bool result = integral<T>;
        static_assert(integral<T const> == result);
        static_assert(integral<T volatile> == result);
        static_assert(integral<T const volatile> == result);
        if constexpr (!result) {
            static_assert(S == is_signed::NA);
            static_assert(!signed_integral<T>);
            static_assert(!signed_integral<T const>);
            static_assert(!signed_integral<T volatile>);
            static_assert(!signed_integral<T const volatile>);
            static_assert(!unsigned_integral<T>);
            static_assert(!unsigned_integral<T const>);
            static_assert(!unsigned_integral<T volatile>);
            static_assert(!unsigned_integral<T const volatile>);
        } else if constexpr (S == is_signed::yes) {
            static_assert(signed_integral<T>);
            static_assert(signed_integral<T const>);
            static_assert(signed_integral<T volatile>);
            static_assert(signed_integral<T const volatile>);
            static_assert(!unsigned_integral<T>);
            static_assert(!unsigned_integral<T const>);
            static_assert(!unsigned_integral<T volatile>);
            static_assert(!unsigned_integral<T const volatile>);
        } else {
            static_assert(!signed_integral<T>);
            static_assert(!signed_integral<T const>);
            static_assert(!signed_integral<T volatile>);
            static_assert(!signed_integral<T const volatile>);
            static_assert(unsigned_integral<T>);
            static_assert(unsigned_integral<T const>);
            static_assert(unsigned_integral<T volatile>);
            static_assert(unsigned_integral<T const volatile>);
        }
        return result;
    }
#pragma warning(pop)

    static_assert(test_integral<signed char, is_signed::yes>());
    static_assert(test_integral<short, is_signed::yes>());
    static_assert(test_integral<int, is_signed::yes>());
    static_assert(test_integral<long, is_signed::yes>());
    static_assert(test_integral<long long, is_signed::yes>());

    static_assert(test_integral<unsigned char, is_signed::no>());
    static_assert(test_integral<unsigned short, is_signed::no>());
    static_assert(test_integral<unsigned int, is_signed::no>());
    static_assert(test_integral<unsigned long, is_signed::no>());
    static_assert(test_integral<unsigned long long, is_signed::no>());

    static_assert(test_integral<bool, is_signed::no>());
    static_assert(test_integral<char, is_signed{std::is_signed_v<char>}>());
    static_assert(test_integral<wchar_t, is_signed{std::is_signed_v<wchar_t>}>());
    static_assert(test_integral<char16_t, is_signed::no>());
    static_assert(test_integral<char32_t, is_signed::no>());

    static_assert(!test_integral<float, is_signed::NA>());
    static_assert(!test_integral<double, is_signed::NA>());
    static_assert(!test_integral<long double, is_signed::NA>());

    static_assert(!test_integral<void, is_signed::NA>());
    static_assert(!test_integral<std::nullptr_t, is_signed::NA>());

    static_assert(!test_integral<int[42], is_signed::NA>());
    static_assert(!test_integral<int (&)[42], is_signed::NA>());
    static_assert(!test_integral<int(int), is_signed::NA>());
    static_assert(!test_integral<int*, is_signed::NA>());
    static_assert(!test_integral<int&, is_signed::NA>());

    static_assert(!test_integral<IncompleteClass, is_signed::NA>());
    static_assert(!test_integral<IncompleteUnion, is_signed::NA>());
    static_assert(!test_integral<DoNotInstantiate<int>, is_signed::NA>());
    static_assert(!test_integral<void (IncompleteClass::*)(), is_signed::NA>());

    enum int_enum : int {};
    static_assert(!test_integral<int_enum, is_signed::NA>());

    static_assert(test_integral<std::ptrdiff_t, is_signed::yes>());
    static_assert(test_integral<std::size_t, is_signed::no>());
} // namespace test_integral_concepts

namespace test_floating_point {
    using std::floating_point;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T>
    constexpr bool test_floating_point() {
        constexpr bool result = floating_point<T>;
        static_assert(floating_point<T const> == result);
        static_assert(floating_point<T volatile> == result);
        static_assert(floating_point<T const volatile> == result);
        return result;
    }
#pragma warning(pop)

    static_assert(!test_floating_point<signed char>());
    static_assert(!test_floating_point<short>());
    static_assert(!test_floating_point<int>());
    static_assert(!test_floating_point<long>());
    static_assert(!test_floating_point<long long>());

    static_assert(!test_floating_point<unsigned char>());
    static_assert(!test_floating_point<unsigned short>());
    static_assert(!test_floating_point<unsigned int>());
    static_assert(!test_floating_point<unsigned long>());
    static_assert(!test_floating_point<unsigned long long>());

    static_assert(!test_floating_point<bool>());
    static_assert(!test_floating_point<char>());
    static_assert(!test_floating_point<wchar_t>());
    static_assert(!test_floating_point<char16_t>());
    static_assert(!test_floating_point<char32_t>());

    static_assert(test_floating_point<float>());
    static_assert(test_floating_point<double>());
    static_assert(test_floating_point<long double>());

    static_assert(!test_floating_point<void>());
    static_assert(!test_floating_point<std::nullptr_t>());

    static_assert(!test_floating_point<double[42]>());
    static_assert(!test_floating_point<double (&)[42]>());
    static_assert(!test_floating_point<double(double)>());
    static_assert(!test_floating_point<double*>());
    static_assert(!test_floating_point<double&>());

    static_assert(!test_floating_point<IncompleteClass>());
    static_assert(!test_floating_point<IncompleteUnion>());
    static_assert(!test_floating_point<DoNotInstantiate<int>>());
    static_assert(!test_floating_point<void (IncompleteClass::*)()>());

    enum int_enum : int {};
    static_assert(!test_floating_point<int_enum>());

    static_assert(!test_floating_point<std::ptrdiff_t>());
    static_assert(!test_floating_point<std::size_t>());
} // namespace test_floating_point

namespace test_assignable_from {
    using std::assignable_from;

    static_assert(!assignable_from<int, int>);
    static_assert(!assignable_from<int, int&>);
    static_assert(!assignable_from<int, int&&>);
    static_assert(!assignable_from<int, int const>);
    static_assert(!assignable_from<int, int const&>);
    static_assert(!assignable_from<int, int const&&>);

    static_assert(assignable_from<int&, int>);
    static_assert(assignable_from<int&, int&>);
    static_assert(assignable_from<int&, int&&>);
    static_assert(assignable_from<int&, int const>);
    static_assert(assignable_from<int&, int const&>);
    static_assert(assignable_from<int&, int const&&>);

    static_assert(!assignable_from<int&&, int>);
    static_assert(!assignable_from<int&&, int&>);
    static_assert(!assignable_from<int&&, int&&>);
    static_assert(!assignable_from<int&&, int const>);
    static_assert(!assignable_from<int&&, int const&>);
    static_assert(!assignable_from<int&&, int const&&>);

    static_assert(assignable_from<int&, double>);
    static_assert(assignable_from<void*&, void*>);

    static_assert(!assignable_from<EmptyClass, EmptyClass>);
    static_assert(!assignable_from<EmptyClass, EmptyClass&>);
    static_assert(!assignable_from<EmptyClass, EmptyClass&&>);
    static_assert(!assignable_from<EmptyClass, EmptyClass const>);
    static_assert(!assignable_from<EmptyClass, EmptyClass const&>);
    static_assert(!assignable_from<EmptyClass, EmptyClass const&&>);

    static_assert(assignable_from<EmptyClass&, EmptyClass>);
    static_assert(assignable_from<EmptyClass&, EmptyClass&>);
    static_assert(assignable_from<EmptyClass&, EmptyClass&&>);
    static_assert(assignable_from<EmptyClass&, EmptyClass const>);
    static_assert(assignable_from<EmptyClass&, EmptyClass const&>);
    static_assert(assignable_from<EmptyClass&, EmptyClass const&&>);

    static_assert(!assignable_from<EmptyClass&&, EmptyClass>);
    static_assert(!assignable_from<EmptyClass&&, EmptyClass&>);
    static_assert(!assignable_from<EmptyClass&&, EmptyClass&&>);
    static_assert(!assignable_from<EmptyClass&&, EmptyClass const>);
    static_assert(!assignable_from<EmptyClass&&, EmptyClass const&>);
    static_assert(!assignable_from<EmptyClass&&, EmptyClass const&&>);

    // assignment operator exists, but no common_reference_with
    struct AssignButUncommon {
        AssignButUncommon& operator=(EmptyClass const&);
    };
    static_assert(!assignable_from<AssignButUncommon, EmptyClass>);
    static_assert(!assignable_from<AssignButUncommon, EmptyClass&>);
    static_assert(!assignable_from<AssignButUncommon, EmptyClass&&>);
    static_assert(!assignable_from<AssignButUncommon, EmptyClass const>);
    static_assert(!assignable_from<AssignButUncommon, EmptyClass const&>);
    static_assert(!assignable_from<AssignButUncommon, EmptyClass const&&>);

    static_assert(!assignable_from<AssignButUncommon&, EmptyClass>);
    static_assert(!assignable_from<AssignButUncommon&, EmptyClass&>);
    static_assert(!assignable_from<AssignButUncommon&, EmptyClass&&>);
    static_assert(!assignable_from<AssignButUncommon&, EmptyClass const>);
    static_assert(!assignable_from<AssignButUncommon&, EmptyClass const&>);
    static_assert(!assignable_from<AssignButUncommon&, EmptyClass const&&>);

    static_assert(!assignable_from<AssignButUncommon&&, EmptyClass>);
    static_assert(!assignable_from<AssignButUncommon&&, EmptyClass&>);
    static_assert(!assignable_from<AssignButUncommon&&, EmptyClass&&>);
    static_assert(!assignable_from<AssignButUncommon&&, EmptyClass const>);
    static_assert(!assignable_from<AssignButUncommon&&, EmptyClass const&>);
    static_assert(!assignable_from<AssignButUncommon&&, EmptyClass const&&>);

    // assignment operator exists, with common_reference_with
    struct AssignAndCommon {
        AssignAndCommon(EmptyClass const&);
        AssignAndCommon& operator=(EmptyClass const&) &;
    };
    static_assert(!assignable_from<AssignAndCommon, EmptyClass>);
    static_assert(!assignable_from<AssignAndCommon, EmptyClass&>);
    static_assert(!assignable_from<AssignAndCommon, EmptyClass&&>);
    static_assert(!assignable_from<AssignAndCommon, EmptyClass const>);
    static_assert(!assignable_from<AssignAndCommon, EmptyClass const&>);
    static_assert(!assignable_from<AssignAndCommon, EmptyClass const&&>);

    static_assert(assignable_from<AssignAndCommon&, EmptyClass>);
    static_assert(assignable_from<AssignAndCommon&, EmptyClass&>);
    static_assert(assignable_from<AssignAndCommon&, EmptyClass&&>);
    static_assert(assignable_from<AssignAndCommon&, EmptyClass const>);
    static_assert(assignable_from<AssignAndCommon&, EmptyClass const&>);
    static_assert(assignable_from<AssignAndCommon&, EmptyClass const&&>);

    static_assert(!assignable_from<AssignAndCommon&&, EmptyClass>);
    static_assert(!assignable_from<AssignAndCommon&&, EmptyClass&>);
    static_assert(!assignable_from<AssignAndCommon&&, EmptyClass&&>);
    static_assert(!assignable_from<AssignAndCommon&&, EmptyClass const>);
    static_assert(!assignable_from<AssignAndCommon&&, EmptyClass const&>);
    static_assert(!assignable_from<AssignAndCommon&&, EmptyClass const&&>);

    struct VoidReturn {
        void operator=(EmptyClass);
    };
    static_assert(!assignable_from<VoidReturn, EmptyClass>);
    static_assert(!assignable_from<EmptyClass, VoidReturn>);

    struct EvilReturn {
        struct EvilCommaOverload {
            template <class U>
            IncompleteClass operator,(U&&);
        };

        EvilCommaOverload operator=(int);
    };
    static_assert(!assignable_from<EvilReturn, int>);

    static_assert(!assignable_from<void, void const>);
    static_assert(!assignable_from<void const, void const>);
    static_assert(!assignable_from<int(), int>);

    static_assert(assignable_from<DoNotInstantiate<int>*&, DoNotInstantiate<int>*>);
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
        static_assert(destructible<T const> == result);
        static_assert(destructible<T volatile> == result);
        static_assert(destructible<T const volatile> == result);
        return result;
    }
#pragma warning(pop)

    static_assert(test<int>());
    static_assert(test<int const>());
    static_assert(test<int&>());
    static_assert(test<void (*)()>());
    static_assert(test<void (&)()>());
    static_assert(test<int[2]>());
    static_assert(test<int (*)[2]>());
    static_assert(test<int (&)[2]>());
    static_assert(test<CopyableType>());
    static_assert(test<MoveOnly>());
    static_assert(test<CopyOnlyAbomination>());
    static_assert(test<Immobile>());

    static_assert(!test<void()>());
    static_assert(!test<int[]>());
    static_assert(!test<Indestructible>());
    static_assert(!test<ThrowingDestructor>());
    static_assert(!test<void>());

    static_assert(test<NonTriviallyDestructible>());
    static_assert(test<int&>());
    static_assert(test<EmptyUnion>());
    static_assert(test<EmptyClass>());
    static_assert(test<int>());
    static_assert(test<double>());
    static_assert(test<int*>());
    static_assert(test<int const*>());
    static_assert(test<char[3]>());
    static_assert(test<BitZero>());
    static_assert(test<int[3]>());

    static_assert(test<ProtectedAbstract>());
    static_assert(test<PublicAbstract>());
    static_assert(test<PrivateAbstract>());
    static_assert(test<PublicDestructor>());
    static_assert(test<VirtualPublicDestructor>());
    static_assert(test<PurePublicDestructor>());

    static_assert(!test<int[]>());
    static_assert(!test<void>());
    static_assert(!test<void_function>());

    // Test inaccessible destructors
    static_assert(!test<ProtectedDestructor>());
    static_assert(!test<PrivateDestructor>());
    static_assert(!test<VirtualProtectedDestructor>());
    static_assert(!test<VirtualPrivateDestructor>());
    static_assert(!test<PureProtectedDestructor>());
    static_assert(!test<PurePrivateDestructor>());

    // Test deleted constructors
    static_assert(!test<DeletedPublicDestructor>());
    static_assert(!test<DeletedProtectedDestructor>());
    static_assert(!test<DeletedPrivateDestructor>());
    static_assert(!test<DeletedVirtualPublicDestructor>());
    static_assert(!test<DeletedVirtualProtectedDestructor>());
    static_assert(!test<DeletedVirtualPrivateDestructor>());
} // namespace test_destructible

namespace test_constructible_from {
    using std::constructible_from, std::initializer_list;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T, class... Args>
    constexpr bool test() {
        constexpr bool result = constructible_from<T, Args...>;
        static_assert(constructible_from<T const, Args...> == result);
        static_assert(constructible_from<T volatile, Args...> == result);
        static_assert(constructible_from<T const volatile, Args...> == result);
        return result;
    }
#pragma warning(pop)

    static_assert(!test<void>());
    static_assert(!test<void const>());
    static_assert(!test<void volatile>());
    static_assert(!test<void const volatile>());

    static_assert(test<int>());
    static_assert(test<int, int>());
    static_assert(test<int, int&>());
    static_assert(test<int, int&&>());
    static_assert(test<int, int const>());
    static_assert(test<int, int const&>());
    static_assert(test<int, int const&&>());
    static_assert(!test<int, int (&)[3]>());
    static_assert(!test<int, void()>());
    static_assert(!test<int, void (&)()>());
    static_assert(!test<int, void() const>());

    static_assert(test<int*>());
    static_assert(test<int*, int*>());
    static_assert(test<int*, int* const>());
    static_assert(test<int*, int*&>());
    static_assert(test<int*, int* const&>());
    static_assert(test<int*, int*&&>());
    static_assert(test<int*, int* const&&>());

    static_assert(!test<int&>());
    static_assert(!test<int&, int>());
    static_assert(test<int&, int&>());
    static_assert(!test<int&, int&&>());
    static_assert(!test<int&, int const>());
    static_assert(!test<int&, int const&>());
    static_assert(!test<int&, int const&&>());

    static_assert(!test<int&, long&>()); // https://github.com/ericniebler/stl2/issues/301
    static_assert(!test<int&, void>());
    static_assert(!test<int&, void()>());
    static_assert(!test<int&, void() const>());
    static_assert(!test<int&, void (&)()>());

    static_assert(!test<int const&>());
    static_assert(test<int const&, int>());
    static_assert(test<int const&, int&>());
    static_assert(test<int const&, int&&>());
    static_assert(test<int const&, int const>());
    static_assert(test<int const&, int const&>());
    static_assert(test<int const&, int const&&>());

    static_assert(test<int&&, int>());
    static_assert(!test<int&&, int&>());
    static_assert(test<int&&, int&&>());
    static_assert(!test<int&&, int const>());
    static_assert(!test<int&&, int const&>());
    static_assert(!test<int&&, int const&&>());

    static_assert(test<int const&&, int>());
    static_assert(!test<int const&&, int&>());
    static_assert(test<int const&&, int&&>());
    static_assert(test<int const&&, int const>());
    static_assert(!test<int const&&, int const&>());
    static_assert(test<int const&&, int const&&>());

    static_assert(!test<int()>());
    static_assert(!test<int (&)()>());
    static_assert(!test<int[]>());
    static_assert(test<int[5]>());
    static_assert(test<int const (&)[5], int (&)[5]>());

    static_assert(!test<void()>());
    static_assert(!test<void() const>());
    static_assert(!test<void() const, void*>());
    static_assert(!test<void() const, void() const>());
    static_assert(!test<void() volatile>());
    static_assert(!test<void() &>());
    static_assert(!test<void() &&>());

    static_assert(test<void (&)(), void()>());
    static_assert(test<void (&)(), void (&)()>());
    static_assert(test<void (&)(), void (&&)()>());
    static_assert(test<void (&&)(), void()>());
    static_assert(test<void (&&)(), void (&)()>());
    static_assert(test<void (&&)(), void (&&)()>());

    static_assert(test<int&&, double&>());

    static_assert(test<initializer_list<int>>());

    static_assert(!test<CopyableType>());
    static_assert(test<CopyableType, CopyableType>());
    static_assert(test<CopyableType, CopyableType&>());
    static_assert(test<CopyableType, CopyableType&&>());
    static_assert(test<CopyableType, CopyableType const>());
    static_assert(test<CopyableType, CopyableType const&>());
    static_assert(test<CopyableType, CopyableType const&&>());

    static_assert(test<MoveOnly>());
    static_assert(test<MoveOnly, MoveOnly>());
    static_assert(!test<MoveOnly, MoveOnly&>());
    static_assert(test<MoveOnly, MoveOnly&&>());
    static_assert(!test<MoveOnly, MoveOnly const>());
    static_assert(!test<MoveOnly, MoveOnly const&>());
    static_assert(!test<MoveOnly, MoveOnly const&&>());

    static_assert(test<CopyOnlyAbomination>());
    static_assert(!test<CopyOnlyAbomination, CopyOnlyAbomination>());
    static_assert(test<CopyOnlyAbomination, CopyOnlyAbomination&>());
    static_assert(!test<CopyOnlyAbomination, CopyOnlyAbomination&&>());
    static_assert(test<CopyOnlyAbomination, CopyOnlyAbomination const>());
    static_assert(test<CopyOnlyAbomination, CopyOnlyAbomination const&>());
    static_assert(test<CopyOnlyAbomination, CopyOnlyAbomination const&&>());

    static_assert(test<Immobile>());
    static_assert(!test<Immobile, Immobile>());
    static_assert(!test<Immobile, Immobile&>());
    static_assert(!test<Immobile, Immobile&&>());
    static_assert(!test<Immobile, Immobile const>());
    static_assert(!test<Immobile, Immobile const&>());
    static_assert(!test<Immobile, Immobile const&&>());

    static_assert(!test<NotDefaultConstructible>());
    static_assert(test<NotDefaultConstructible, NotDefaultConstructible>());
    static_assert(test<NotDefaultConstructible, NotDefaultConstructible&>());
    static_assert(test<NotDefaultConstructible, NotDefaultConstructible&&>());
    static_assert(test<NotDefaultConstructible, NotDefaultConstructible const>());
    static_assert(test<NotDefaultConstructible, NotDefaultConstructible const&>());
    static_assert(test<NotDefaultConstructible, NotDefaultConstructible const&&>());

    static_assert(!test<Indestructible>());
    static_assert(!test<Indestructible, Indestructible>());
    static_assert(!test<Indestructible, Indestructible&>());
    static_assert(!test<Indestructible, Indestructible&&>());
    static_assert(!test<Indestructible, Indestructible const>());
    static_assert(!test<Indestructible, Indestructible const&>());
    static_assert(!test<Indestructible, Indestructible const&&>());

    static_assert(!test<ThrowingDestructor>());
    static_assert(!test<ThrowingDestructor, ThrowingDestructor>());
    static_assert(!test<ThrowingDestructor, ThrowingDestructor&>());
    static_assert(!test<ThrowingDestructor, ThrowingDestructor&&>());
    static_assert(!test<ThrowingDestructor, ThrowingDestructor const>());
    static_assert(!test<ThrowingDestructor, ThrowingDestructor const&>());
    static_assert(!test<ThrowingDestructor, ThrowingDestructor const&&>());

    // Indestructible types are not constructible
    static_assert(test<PrivateDestructor&, PrivateDestructor&>());
    static_assert(!test<PrivateDestructor, int>());
    static_assert(!test<PurePrivateDestructor>());

    static_assert(test<ExplicitDefault>());
    static_assert(test<ExplicitDefault, ExplicitDefault>());
    static_assert(test<ExplicitDefault, ExplicitDefault&>());
    static_assert(test<ExplicitDefault, ExplicitDefault&&>());
    static_assert(test<ExplicitDefault, ExplicitDefault const>());
    static_assert(test<ExplicitDefault, ExplicitDefault const&>());
    static_assert(test<ExplicitDefault, ExplicitDefault const&&>());

    static_assert(!test<DeletedDefault>());
    static_assert(test<DeletedDefault, DeletedDefault>());
    static_assert(test<DeletedDefault, DeletedDefault&>());
    static_assert(test<DeletedDefault, DeletedDefault&&>());
    static_assert(test<DeletedDefault, DeletedDefault const>());
    static_assert(test<DeletedDefault, DeletedDefault const&>());
    static_assert(test<DeletedDefault, DeletedDefault const&&>());

    static_assert(test<SimpleBase, SimpleDerived>());
    static_assert(test<SimpleBase&, SimpleDerived&>());
    static_assert(!test<SimpleDerived&, SimpleBase&>());
    static_assert(test<SimpleBase const&, SimpleDerived const&>());
    static_assert(!test<SimpleDerived const&, SimpleBase const&>());
    static_assert(!test<SimpleDerived const&, SimpleBase>());

    static_assert(test<SimpleBase&&, SimpleDerived>());
    static_assert(test<SimpleBase&&, SimpleDerived&&>());
    static_assert(!test<SimpleDerived&&, SimpleBase&&>());
    static_assert(!test<SimpleDerived&&, SimpleBase>());
    static_assert(!test<SimpleBase&&, SimpleBase&>());
    static_assert(!test<SimpleBase&&, SimpleDerived&>());

    static_assert(!test<PrivateAbstract>());

    static_assert(test<int&, ImplicitTo<int&>>());
    static_assert(test<int const&, ImplicitTo<int&&>>());
    static_assert(test<int&&, ImplicitTo<int&&>>());
    static_assert(test<int const&, ImplicitTo<int>>());
    static_assert(test<int const&, ImplicitTo<int&>>());
    static_assert(test<int const&, ImplicitTo<int&>&>());

    static_assert(test<SimpleBase&&, ImplicitTo<SimpleDerived&&>>());
    static_assert(test<SimpleBase&&, ImplicitTo<SimpleDerived&&>&>());

    static_assert(test<int&, ExplicitTo<int&>>());
    static_assert(test<int const&, ExplicitTo<int&>>());
    static_assert(test<int const&, ExplicitTo<int&>&>());

    struct Multiparameter {
        explicit Multiparameter(int);
        Multiparameter(int, double);
        Multiparameter(int, long, double);
        Multiparameter(char) = delete;
    };
    static_assert(!test<Multiparameter>());
    static_assert(test<Multiparameter, int>());
#ifndef __EDG__ // TRANSITION, VSO-1898939
    static_assert(test<Multiparameter, long>() == is_permissive);
#endif // ^^^ no workaround ^^^
    static_assert(!test<Multiparameter, double>());
    static_assert(!test<Multiparameter, char>());
    static_assert(!test<Multiparameter, void>());
    static_assert(test<Multiparameter, int, double>());
    static_assert(test<Multiparameter, char, float>());
    static_assert(test<Multiparameter, int, long, double>());
    static_assert(test<Multiparameter, double, double, int>());

    struct ExplicitToDeduced {
        template <class T>
        explicit operator T() const;
    };
    static_assert(test<int, ExplicitToDeduced>());
    static_assert(!test<void, ExplicitToDeduced>());
    static_assert(!test<int&, ExplicitToDeduced>());

    // Binding through reference-compatible type is required to perform
    // direct-initialization as described in N4849 [over.match.ref]/1.1:
    static_assert(test<int&, ExplicitTo<int&>>());
    static_assert(test<int&&, ExplicitTo<int&&>>());

    // Binding through temporary behaves like copy-initialization,
    // see N4849 [dcl.init.ref]/5.4:
#ifndef __clang__ // TRANSITION, LLVM-44688
    static_assert(!test<int const&, ExplicitTo<int>>());
    static_assert(!test<int&&, ExplicitTo<int>>());
#endif // ^^^ no workaround ^^^
    static_assert(!test<int const&, ExplicitTo<double&&>>());
    static_assert(!test<int&&, ExplicitTo<double&&>>());

    struct ImmobileExplicitFromInt {
        ImmobileExplicitFromInt()                          = default;
        ImmobileExplicitFromInt(ImmobileExplicitFromInt&&) = delete;
        explicit ImmobileExplicitFromInt(int);
    };
    static_assert(test<ImmobileExplicitFromInt>());
    static_assert(test<ImmobileExplicitFromInt, int>());
    static_assert(!test<ImmobileExplicitFromInt, ImmobileExplicitFromInt>());
    static_assert(!test<ImmobileExplicitFromInt, ImmobileExplicitFromInt&>());
    static_assert(!test<ImmobileExplicitFromInt, ImmobileExplicitFromInt&&>());
    static_assert(!test<ImmobileExplicitFromInt, ImmobileExplicitFromInt const>());
    static_assert(!test<ImmobileExplicitFromInt, ImmobileExplicitFromInt const&>());
    static_assert(!test<ImmobileExplicitFromInt, ImmobileExplicitFromInt const&&>());
} // namespace test_constructible_from

namespace test_default_initializable {
    using std::default_initializable, std::initializer_list;

    static_assert(default_initializable<int>);
#ifndef __EDG__ // TRANSITION, VSO-1898941
    static_assert(!default_initializable<int const>);
#endif // ^^^ no workaround ^^^
    static_assert(default_initializable<int volatile>);
#ifndef __EDG__ // TRANSITION, VSO-1898941
    static_assert(!default_initializable<int const volatile>);
#endif // ^^^ no workaround ^^^
    static_assert(default_initializable<double>);
    static_assert(!default_initializable<void>);

    static_assert(default_initializable<int*>);
    static_assert(default_initializable<int const*>);

    static_assert(default_initializable<int[2]>);
    static_assert(default_initializable<char[3]>);
    static_assert(default_initializable<char[5][3]>);
    static_assert(!default_initializable<int[]>);
    static_assert(!default_initializable<char[]>);
    static_assert(!default_initializable<char[][3]>);
#ifndef __EDG__ // TRANSITION, VSO-1898941
    static_assert(!default_initializable<int const[2]>);
#endif // ^^^ no workaround ^^^

    static_assert(!default_initializable<int&>);
    static_assert(!default_initializable<int const&>);
    static_assert(!default_initializable<int&&>);

    static_assert(!default_initializable<int()>);
    static_assert(!default_initializable<int (&)()>);

    static_assert(!default_initializable<void()>);
    static_assert(!default_initializable<void() const>);
    static_assert(!default_initializable<void() volatile>);
    static_assert(!default_initializable<void() &>);
    static_assert(!default_initializable<void() &&>);

    static_assert(default_initializable<EmptyClass>);
    static_assert(default_initializable<EmptyClass const>);
    static_assert(default_initializable<EmptyUnion>);
    static_assert(default_initializable<EmptyUnion const>);

    static_assert(default_initializable<std::initializer_list<int>>);

    static_assert(default_initializable<NotEmpty>);
    static_assert(default_initializable<BitZero>);

    static_assert(default_initializable<ExplicitDefault>);
    static_assert(default_initializable<ExplicitMoveAbomination>);
    static_assert(default_initializable<ExplicitCopyAbomination>);

    static_assert(!default_initializable<NotDefaultConstructible>);
    static_assert(!default_initializable<DeletedDefault>);

    static_assert(!default_initializable<PurePublicDestructor>);
    static_assert(!default_initializable<NotDefaultConstructible>);

    class PrivateDefault {
        PrivateDefault();
    };
    static_assert(!default_initializable<PrivateDefault>);

    struct S {
        int x;
    };
    static_assert(default_initializable<S>);
#ifndef __EDG__ // TRANSITION, VSO-1898941
    static_assert(!default_initializable<S const>);
#endif // ^^^ no workaround ^^^

    // Also test GH-1603 "default_initializable accepts types that are not default-initializable"
#ifndef __EDG__ // TRANSITION, VSO-1898945
    static_assert(!default_initializable<AggregatesExplicitDefault>);
#endif // ^^^ no workaround ^^^
} // namespace test_default_initializable

namespace test_move_constructible {
    using std::move_constructible;

    static_assert(move_constructible<int>);
    static_assert(move_constructible<int const>);
    static_assert(move_constructible<double>);
    static_assert(!move_constructible<void>);

    static_assert(move_constructible<int*>);
    static_assert(move_constructible<int const*>);

    static_assert(!move_constructible<int[4]>);
    static_assert(!move_constructible<char[3]>);
    static_assert(!move_constructible<char[]>);

    static_assert(move_constructible<int&>);
    static_assert(move_constructible<int&&>);
    static_assert(move_constructible<int const&>);
    static_assert(move_constructible<int const&&>);

    static_assert(!move_constructible<void()>);

    static_assert(move_constructible<EmptyClass>);
    static_assert(move_constructible<EmptyUnion>);
    static_assert(move_constructible<NotEmpty>);
    static_assert(move_constructible<CopyableType>);
    static_assert(move_constructible<MoveOnly>);
    static_assert(!move_constructible<CopyOnlyAbomination>);
    static_assert(!move_constructible<Immobile>);
    static_assert(!move_constructible<ExplicitMoveAbomination>);
    static_assert(move_constructible<ExplicitCopyAbomination>);
    static_assert(move_constructible<BitZero>);

    static_assert(move_constructible<NotDefaultConstructible>);
    static_assert(!move_constructible<Indestructible>);
    static_assert(!move_constructible<ThrowingDestructor>);
    static_assert(move_constructible<ExplicitDefault>);
    static_assert(move_constructible<DeletedDefault>);

    static_assert(move_constructible<Immobile&>);
    static_assert(move_constructible<Immobile&&>);
    static_assert(move_constructible<Immobile const&>);
    static_assert(move_constructible<Immobile const&&>);

    static_assert(!move_constructible<PurePublicDestructor>);

    struct ImplicitlyDeletedMove {
        ImplicitlyDeletedMove(ImplicitlyDeletedMove const&);
    };
    static_assert(move_constructible<ImplicitlyDeletedMove>);
} // namespace test_move_constructible

namespace test_ranges_swap {
    namespace ranges = std::ranges;

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

    static_assert(!can_swap<void>);
    static_assert(!can_swap<void const>);
    static_assert(!can_swap<void volatile>);
    static_assert(!can_swap<void const volatile>);

    static_assert(can_nothrow_swap<int&>);
    static_assert(!can_swap<int const&>);
    static_assert(can_nothrow_swap<int volatile&>);
    static_assert(!can_swap<int const volatile&>);

    static_assert(!can_swap<int&&>);
    static_assert(!can_swap<int const&&>);
    static_assert(!can_swap<int volatile&&>);
    static_assert(!can_swap<int const volatile&&>);

    static_assert(!can_swap<int()>);
    static_assert(!can_swap<int() const>);
    static_assert(!can_swap<int (&)()>);
    static_assert(!can_swap<int (&&)()>);

    static_assert(!can_swap<int (&)[]>);
    static_assert(!can_swap<int (&&)[]>);

    static_assert(can_nothrow_swap<int (&)[42]>);
    static_assert(can_nothrow_swap<int (&)[42][13]>);
    static_assert(!can_swap<int (&)[42][13], int (&)[13][42]>);
    static_assert(!can_swap<int const (&)[42]>);
    static_assert(can_nothrow_swap<int volatile (&)[42]>);
    static_assert(!can_swap<int const volatile (&)[42]>);
    static_assert(!can_swap<int const (&)[42][13]>);
    static_assert(can_nothrow_swap<int volatile (&)[42][13]>);
    static_assert(!can_swap<int const volatile (&)[42][13]>);

    static_assert(!can_swap<int (&&)[42]>);
    static_assert(!can_swap<int (&&)[42][13]>);

    static_assert(!can_swap<int (&&)[42][13], int (&&)[13][42]>);

    struct SemithrowCopyOnly {
        SemithrowCopyOnly()                                  = default;
        SemithrowCopyOnly(SemithrowCopyOnly const&) noexcept = default;

        SemithrowCopyOnly& operator=(SemithrowCopyOnly const&) noexcept(false) {
            return *this;
        }
    };
    static_assert(can_swap<SemithrowCopyOnly&>);
    static_assert(!can_nothrow_swap<SemithrowCopyOnly&>);
    static_assert(can_swap<SemithrowCopyOnly (&)[42]>);
    static_assert(!can_nothrow_swap<SemithrowCopyOnly (&)[42]>);

    struct SemithrowMoveOnly {
        SemithrowMoveOnly() = default;
        SemithrowMoveOnly(SemithrowMoveOnly&&) noexcept(false) {}
        SemithrowMoveOnly& operator=(SemithrowMoveOnly&&) noexcept = default;
    };
    static_assert(can_swap<SemithrowMoveOnly&>);
    static_assert(!can_nothrow_swap<SemithrowMoveOnly&>);
    static_assert(can_swap<SemithrowMoveOnly (&)[42]>);
    static_assert(!can_nothrow_swap<SemithrowMoveOnly (&)[42]>);

    struct NothrowMoveOnly {
        NothrowMoveOnly()                                      = default;
        NothrowMoveOnly(NothrowMoveOnly&&) noexcept            = default;
        NothrowMoveOnly& operator=(NothrowMoveOnly&&) noexcept = default;
    };
    static_assert(can_nothrow_swap<NothrowMoveOnly&>);
    static_assert(can_nothrow_swap<NothrowMoveOnly (&)[42]>);

    struct NotMoveConstructible {
        NotMoveConstructible()                                  = default;
        NotMoveConstructible(NotMoveConstructible&&)            = delete;
        NotMoveConstructible& operator=(NotMoveConstructible&&) = default;
    };
    static_assert(!can_swap<NotMoveConstructible&>);
    static_assert(!can_swap<NotMoveConstructible (&)[42]>);

    struct NotMoveAssignable {
        NotMoveAssignable(NotMoveAssignable&&)            = default;
        NotMoveAssignable& operator=(NotMoveAssignable&&) = delete;
    };
    static_assert(!can_swap<NotMoveAssignable&>);
    static_assert(!can_swap<NotMoveAssignable (&)[42]>);

    struct ImmobileNothrowSwap {
        ImmobileNothrowSwap()                                 = default;
        ImmobileNothrowSwap(ImmobileNothrowSwap&&)            = delete;
        ImmobileNothrowSwap& operator=(ImmobileNothrowSwap&&) = delete;
        friend void swap(ImmobileNothrowSwap&, ImmobileNothrowSwap&) noexcept {}
    };
    static_assert(can_nothrow_swap<ImmobileNothrowSwap&>);
    static_assert(can_nothrow_swap<ImmobileNothrowSwap (&)[42]>);

    struct HasThrowingSwap {
        friend void swap(HasThrowingSwap&, HasThrowingSwap&) {}
    };
    static_assert(can_swap<HasThrowingSwap&>);
    static_assert(!can_nothrow_swap<HasThrowingSwap&>);
    static_assert(can_swap<HasThrowingSwap (&)[42]>);
    static_assert(!can_nothrow_swap<HasThrowingSwap (&)[42]>);

    // Derives from type in std with overloaded swap to validate the poison pill
    struct Unswappable : std::pair<int, int> {
        Unswappable()                   = default;
        Unswappable(Unswappable const&) = delete;
        Unswappable(Unswappable&&)      = delete;
    };
    static_assert(!can_swap<Unswappable&>);

    static_assert(!can_swap<Unswappable (&)[42]>);
    static_assert(!can_swap<Unswappable (&)[42][13]>);
    static_assert(!can_swap<Unswappable (&)[42][13], Unswappable (&)[13][42]>);

    static_assert(!can_swap<Unswappable (&&)[42]>);
    static_assert(!can_swap<Unswappable (&&)[42][13]>);
    static_assert(!can_swap<Unswappable (&&)[42][13], Unswappable (&&)[13][42]>);

    // The wording allows customization of swap for unions as well
    union U {
        U(U const&)            = delete;
        U& operator=(U const&) = delete;

        friend void swap(U&, U&) {}
    };
    static_assert(can_swap<U&>);

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
    static_assert(can_swap<adl_barrier::ConstrainedSwappable&>);
    static_assert(!can_swap<adl_barrier::ConstrainedSwappable const volatile&>);

    struct MyInt {
        int i = 42;
    };
    static_assert(can_nothrow_swap<MyInt&>);

    struct YourInt {
        int i = 13;
    };
    static_assert(can_nothrow_swap<YourInt&>);

    constexpr void swap(MyInt& x, YourInt& y) noexcept {
        ranges::swap(x.i, y.i);
    }
    constexpr void swap(YourInt& x, MyInt& y) noexcept {
        ranges::swap(x.i, y.i);
    }
    static_assert(can_nothrow_swap<MyInt&, YourInt&>);
    static_assert(can_nothrow_swap<YourInt&, MyInt&>);

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
            static_assert(noexcept(swap(i, j)));
            assert(i == 2);
            assert(j == 1);
        }
        {
            // Array swap
            int a[3] = {1, 2, 3};
            int b[3] = {4, 5, 6};
            swap(a, b);
            static_assert(noexcept(swap(a, b)));
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
            static_assert(noexcept(swap(a, b)));
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
            static_assert(noexcept(swap(a, b)));
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
    static_assert(compile_time_tests());

    void runtime_tests() {
        using ranges::swap;
        compile_time_tests();

        {
            // Non-array swap
            auto i = std::make_unique<int>(1);
            auto j = std::make_unique<int>(2);
            swap(i, j);
            static_assert(noexcept(swap(i, j)));
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
            static_assert(noexcept(swap(a, b)));
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
            static_assert(noexcept(swap(a, b)));
            counter = 0;
            for_each_232(b, check);
            for_each_232(a, check);
        }
    }
} // namespace test_ranges_swap

namespace test_swappable {
    using std::swappable;

    static_assert(swappable<int>);
    static_assert(swappable<int[4]>);
    static_assert(!swappable<int[]>);
    static_assert(!swappable<int[][4]>);
    static_assert(swappable<int[3][1][4][1][5][9]>);

    static_assert(swappable<EmptyClass>);
    static_assert(swappable<EmptyUnion>);

    static_assert(!swappable<Immobile>);
    static_assert(!swappable<Immobile&>);
    static_assert(!swappable<Immobile&&>);

    static_assert(swappable<MovableFriendSwap>);
    // It may not be a great idea that swappable admits reference types and treats them as lvalues, but that's what
    // is_swappable does, so:
    static_assert(swappable<MovableFriendSwap&>);
    static_assert(swappable<MovableFriendSwap&&>);

    static_assert(swappable<ImmobileFriendSwap>);
    static_assert(swappable<ImmobileFriendSwap&>);
    static_assert(swappable<ImmobileFriendSwap&&>);

    // test non-referenceable types
    static_assert(!swappable<void>);
    static_assert(!swappable<int() const>);
    static_assert(!swappable<int() &>);

    static_assert(swappable<ImmobileNonMemberSwap>);
    static_assert(swappable<ImmobileNonMemberSwap&>);
    static_assert(swappable<ImmobileNonMemberSwap&&>);

    namespace non_member {
        struct OnlyRvalueSwappable : Immobile {};
        void swap(OnlyRvalueSwappable&&, OnlyRvalueSwappable&&) {}
        // swappable only cares about lvalue swaps
        static_assert(!swappable<OnlyRvalueSwappable>);
        static_assert(!swappable<OnlyRvalueSwappable&>);
        static_assert(!swappable<OnlyRvalueSwappable&&>);

        struct DeletedSwap {
            friend void swap(DeletedSwap&, DeletedSwap&) = delete;
        };
        // a deleted swap overload is correctly ignored
        static_assert(swappable<DeletedSwap>);
        static_assert(swappable<DeletedSwap&>);
        static_assert(swappable<DeletedSwap&&>);
    } // namespace non_member

    namespace underconstrained {
        struct AmbiguousSwap {};

        template <class T>
        void swap(T&, T&);

        // a swap overload that isn't more specialized or constrained than the poison pill is ignored
        static_assert(swappable<AmbiguousSwap>);
        static_assert(swappable<AmbiguousSwap&>);
        static_assert(swappable<AmbiguousSwap&&>);
    } // namespace underconstrained
} // namespace test_swappable

namespace test_swappable_with {
    using std::swappable_with;

    template <class T, class U>
    constexpr bool test() {
        static_assert(swappable_with<T, U> == swappable_with<U, T>);
        return swappable_with<T, U>;
    }

    static_assert(!test<void, int>());
    static_assert(!test<int, void>());
    static_assert(!test<void const, void const volatile>());

    static_assert(!test<int, int>());
    static_assert(test<int&, int&>());
    static_assert(!test<int&&, int&&>());

    static_assert(test<int (&)[4], int (&)[4]>());
    static_assert(!test<int, int>());
    static_assert(!test<int&&, int&&>());
    static_assert(!test<int&, double&>());
    static_assert(!test<int (&)[4], bool (&)[4]>());

    static_assert(test<int (&)[3][4], int (&)[3][4]>());
    static_assert(test<int (&)[3][4][1][2], int (&)[3][4][1][2]>());
    static_assert(!test<int (&)[3][4][1][2], int (&)[4][4][1][2]>());

    static_assert(test<int (&)[2][2], int (&)[2][2]>());

    static_assert(test<int volatile (&)[4], int volatile (&)[4]>());
    static_assert(test<int volatile (&)[3][4], int volatile (&)[3][4]>());

    static_assert(test<MovableFriendSwap, MovableFriendSwap>() == is_permissive);
    static_assert(test<MovableFriendSwap&, MovableFriendSwap&>());
    static_assert(test<MovableFriendSwap&&, MovableFriendSwap&&>() == is_permissive);

    static_assert(!test<ImmobileFriendSwap, ImmobileFriendSwap>());
    static_assert(test<ImmobileFriendSwap&, ImmobileFriendSwap&>());
    static_assert(test<ImmobileFriendSwap&&, ImmobileFriendSwap&&>() == is_permissive);

    static_assert(!test<ImmobileNonMemberSwap, ImmobileNonMemberSwap>());
    static_assert(test<ImmobileNonMemberSwap&, ImmobileNonMemberSwap&>());
    static_assert(test<ImmobileNonMemberSwap&&, ImmobileNonMemberSwap&&>() == is_permissive);

    struct LvalueAndRvalueSwappable {};
    void swap(LvalueAndRvalueSwappable&&, LvalueAndRvalueSwappable&&) {}
    static_assert(test<LvalueAndRvalueSwappable, LvalueAndRvalueSwappable>());
    static_assert(test<LvalueAndRvalueSwappable&, LvalueAndRvalueSwappable&>());
    static_assert(test<LvalueAndRvalueSwappable&&, LvalueAndRvalueSwappable&&>());

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
    static_assert(!test<ImmobileNonMemberSwap&, ImmobileAndNotSwappable&>());
    // Not swappable_with: common_reference_with requirement is not satisfied
    static_assert(!test<ImmobileNonMemberSwap&, CanSwapButUncommon&>());
    // Not swappable_with: asymmetric
    static_assert(!test<AsymmetricallySwappable<0>&, AsymmetricallySwappable<1>&>());
    // swappable_with: all requirements met
    static_assert(test<ImmobileAndSwappable<0>&, ImmobileAndSwappable<1>&>());

    namespace example {
        // The example from [concept.swappable] with changes per the proposed resolution of LWG-3175:
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
            struct A {
                int m;
            };
            struct Proxy {
                A* a;
                Proxy(A& a) : a{&a} {}
                friend void swap(Proxy x, Proxy y) {
                    ranges::swap(*x.a, *y.a);
                }
            };
            Proxy proxy(A& a) {
                return Proxy{a};
            }
        } // namespace N

        void test() {
            int i = 1, j = 2;
            lv_swap(i, j);
            assert(i == 2 && j == 1);

            N::A a1 = {5};
            N::A a2 = {-5};
            value_swap(a1, proxy(a2));
            assert(a1.m == -5 && a2.m == 5);

            // additional test cases not from [concept.swappable] for completeness:
            static_assert(std::is_same_v<std::common_reference_t<N::Proxy, N::A&>, N::Proxy>);
            static_assert(swappable_with<N::A&, N::Proxy>);

            value_swap(proxy(a1), a2);
            assert(a1.m == 5 && a2.m == -5);

            value_swap(proxy(a1), proxy(a2));
            assert(a1.m == -5 && a2.m == 5);

            value_swap(a1, a2);
            assert(a1.m == 5 && a2.m == -5);
            // end additional test cases
        }
    } // namespace example
} // namespace test_swappable_with

namespace test_copy_constructible {
    using std::copy_constructible;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T>
    constexpr bool test() {
        constexpr bool result = copy_constructible<T>;
        static_assert(copy_constructible<T const> == result);
        if constexpr (std::is_scalar_v<T> || std::is_reference_v<T>) {
            static_assert(copy_constructible<T volatile> == result);
            static_assert(copy_constructible<T const volatile> == result);
        } else {
            static_assert(!copy_constructible<T volatile>);
            static_assert(!copy_constructible<T const volatile>);
        }
        return result;
    }
#pragma warning(pop)

    static_assert(!test<void>());

    static_assert(test<int>());
    static_assert(test<int const>());
    static_assert(test<double>());

    static_assert(test<int*>());
    static_assert(test<int const*>());

    static_assert(!test<int[4]>());
    static_assert(!test<int[]>());

    static_assert(test<int&>());
    static_assert(!test<int&&>());
    static_assert(test<int const&>());
    static_assert(!test<int const&&>());

    static_assert(!test<void()>());

    static_assert(test<EmptyUnion>());
    static_assert(test<EmptyClass>());
    static_assert(test<NotEmpty>());
    static_assert(test<BitZero>());

    static_assert(test<CopyableType>());
    static_assert(!test<MoveOnly>());
    static_assert(!copy_constructible<CopyOnlyAbomination>);
    static_assert(copy_constructible<CopyOnlyAbomination const>);
    static_assert(!test<Immobile>());
    static_assert(!test<ExplicitMoveAbomination>());
    static_assert(!test<ExplicitCopyAbomination>());

    static_assert(!test<PurePublicDestructor>());
    static_assert(!test<Indestructible>());
    static_assert(!test<ThrowingDestructor>());

    static_assert(test<NotDefaultConstructible>());
    static_assert(test<ExplicitDefault>());
    static_assert(test<DeletedDefault>());

    static_assert(test<Immobile&>());
    static_assert(!test<Immobile&&>());
    static_assert(test<Immobile const&>());
    static_assert(!test<Immobile const&&>());

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

    static_assert(!copy_constructible<NotMutableRef>);
    static_assert(!copy_constructible<NotConstRefRef>);

    struct UserProvidedCopy {
        UserProvidedCopy(UserProvidedCopy const&);
    };
    static_assert(test<UserProvidedCopy>());

    class PrivateCopy {
    private:
        PrivateCopy(PrivateCopy const&);
    };
    static_assert(!test<PrivateCopy>());

    struct MutatingCopyAbomination {
        MutatingCopyAbomination(MutatingCopyAbomination&); // NB: not const&
        void operator=(MutatingCopyAbomination&); // NB: not const&
    };
    static_assert(!test<MutatingCopyAbomination>());
} // namespace test_copy_constructible

namespace test_object_concepts {
    using std::copyable, std::movable, std::regular, std::semiregular;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T>
    constexpr bool test_movable() {
        constexpr bool result = movable<T>;
        static_assert(!movable<T const>);
        static_assert(movable<T volatile> == std::is_scalar_v<T>);
        static_assert(!movable<T const volatile>);
        return result;
    }

    template <class T>
    constexpr bool test_copyable() {
        constexpr bool result = copyable<T>;
        static_assert(!copyable<T const>);
        static_assert(copyable<T volatile> == std::is_scalar_v<T>);
        static_assert(!copyable<T const volatile>);
        return result;
    }

    template <class T>
    constexpr bool test_semiregular() {
        constexpr bool result = semiregular<T>;
        static_assert(!semiregular<T const>);
        static_assert(semiregular<T volatile> == std::is_scalar_v<T>);
        static_assert(!semiregular<T const volatile>);
        return result;
    }

    template <class T>
    constexpr bool test_regular() {
        constexpr bool result = regular<T>;
        static_assert(!regular<T const>);
        static_assert(regular<T volatile> == std::is_scalar_v<T>);
        static_assert(!regular<T const volatile>);
        return result;
    }
#pragma warning(pop)

    static_assert(!test_movable<void>());

    static_assert(test_movable<int>());
    static_assert(test_movable<double>());

    static_assert(!test_movable<int&>());
    static_assert(!test_movable<int&&>()); // https://github.com/ericniebler/stl2/issues/310

    static_assert(!test_movable<int[42]>());
    static_assert(!test_movable<int[]>());

    static_assert(!test_movable<int(int)>());

    static_assert(test_movable<CopyableType>());
    static_assert(test_movable<MoveOnly>());
    static_assert(!test_movable<CopyOnlyAbomination>());
    static_assert(!test_movable<Immobile>());

    static_assert(!test_copyable<void>());

    static_assert(test_copyable<int>());
    static_assert(test_copyable<double>());

    static_assert(!test_copyable<int[42]>());
    static_assert(!test_copyable<int[]>());

    static_assert(!test_copyable<int&>());
    static_assert(!test_copyable<int&&>());

    static_assert(!test_copyable<int(int)>());

    static_assert(test_copyable<CopyableType>());
    static_assert(!test_copyable<MoveOnly>());
    static_assert(!test_copyable<CopyOnlyAbomination>());
    static_assert(!test_copyable<Immobile>());

    static_assert(!test_semiregular<void>());

    static_assert(test_semiregular<int>());
    static_assert(test_semiregular<double>());

    static_assert(!test_semiregular<int&>());

    static_assert(!test_semiregular<MoveOnly>());
    static_assert(!test_semiregular<Immobile>());
    static_assert(!test_semiregular<ExplicitMoveAbomination>());
    static_assert(!test_semiregular<ExplicitCopyAbomination>());
    static_assert(!test_semiregular<CopyOnlyAbomination>());
    static_assert(test_semiregular<SemiregularType>());
    static_assert(test_semiregular<RegularType>());

    static_assert(!test_regular<void>());

    static_assert(test_regular<int>());
    static_assert(test_regular<double>());

    static_assert(!test_regular<int&>());

    static_assert(!test_regular<MoveOnly>());
    static_assert(!test_regular<Immobile>());
    static_assert(!test_regular<ExplicitMoveAbomination>());
    static_assert(!test_regular<ExplicitCopyAbomination>());
    static_assert(!test_regular<CopyOnlyAbomination>());
    static_assert(!test_regular<CopyableType>());
    static_assert(!test_regular<SemiregularType>());
    static_assert(test_regular<RegularType>());
} // namespace test_object_concepts

namespace test_boolean_testable {
    // Note: other than knowing the secret internal concept name, this is a portable test.
    using std::_Boolean_testable;

    // Better have these four, since we use them as examples in the Standard.
    static_assert(_Boolean_testable<bool>);
    static_assert(_Boolean_testable<std::true_type>);
    static_assert(_Boolean_testable<int*>);
    static_assert(_Boolean_testable<std::bitset<42>::reference>);

    static_assert(_Boolean_testable<std::false_type>);

    static_assert(_Boolean_testable<int>);
    static_assert(_Boolean_testable<void*>);

    enum unscoped_boolish : bool { No, Yes };
    static_assert(_Boolean_testable<unscoped_boolish>);

    enum class scoped_boolish : bool { No, Yes };
    static_assert(!_Boolean_testable<scoped_boolish>);

    static_assert(!_Boolean_testable<EmptyClass>);

    static_assert(_Boolean_testable<ImplicitTo<bool>>);
    static_assert(!_Boolean_testable<ExplicitTo<bool>>);

    struct MutatingBoolConversion {
        operator bool();
    };
    static_assert(_Boolean_testable<MutatingBoolConversion>);

    template <unsigned int Select> // values in [0, Archetype_max) select a requirement to violate
    struct Archetype {
        operator bool() const
            requires (Select != 0); // Archetype<0> is not implicitly convertible to bool
        explicit operator bool() const
            requires (Select < 2); // Archetype<1> is not explicitly convertible to bool (ambiguity)
        void operator!() const
            requires (Select == 2); // !Archetype<2> does not model _Boolean_testable_impl
    };

    static_assert(!_Boolean_testable<Archetype<0>>);
    static_assert(!_Boolean_testable<Archetype<1>>);
    static_assert(!_Boolean_testable<Archetype<2>>);
    static_assert(_Boolean_testable<Archetype<3>>);
} // namespace test_boolean_testable

namespace test_equality_comparable {
    using std::equality_comparable;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T>
    constexpr bool test() {
        constexpr bool result = equality_comparable<T>;
        static_assert(equality_comparable<T const> == result);
        if constexpr (!std::is_same_v<T, std::add_lvalue_reference_t<T>>) {
            static_assert(equality_comparable<T&> == result);
            static_assert(equality_comparable<T const&> == result);
            static_assert(equality_comparable<T&&> == result);
            static_assert(equality_comparable<T const&&> == result);
        }
        return result;
    }
#pragma warning(pop)

    static_assert(!test<void>());

    static_assert(test<int>());
    static_assert(test<double>());
    static_assert(test<std::nullptr_t>());
    static_assert(test<int[42]>());
    static_assert(test<int(int)>());

    static_assert(!test<EmptyClass>());

    template <unsigned int> // selects one requirement to violate
    struct Archetype {};

    template <unsigned int Select>
    bool operator==(Archetype<Select> const&, Archetype<Select> const&);
    void operator==(Archetype<0> const&, Archetype<0> const&); // Archetype<0> == Archetype<0> is not _Boolean_testable

    template <unsigned int Select>
    bool operator!=(Archetype<Select> const&, Archetype<Select> const&);
    void operator!=(Archetype<1> const&, Archetype<1> const&); // Archetype<1> != Archetype<1> is not _Boolean_testable

    static_assert(!test<Archetype<0>>());
    static_assert(!test<Archetype<1>>());
    static_assert(test<Archetype<2>>());
} // namespace test_equality_comparable

namespace test_equality_comparable_with {
    using std::equality_comparable_with;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T, class U = T>
    constexpr bool test() {
        using std::is_same_v, std::add_lvalue_reference_t;

        constexpr bool result = equality_comparable_with<T, U>;
        static_assert(equality_comparable_with<U, T> == result);

        if constexpr (!is_same_v<T, add_lvalue_reference_t<T>>) {
            static_assert(equality_comparable_with<T&, U> == result);
            static_assert(equality_comparable_with<T const&, U> == result);
            static_assert(equality_comparable_with<T&&, U> == result);
            static_assert(equality_comparable_with<T const&&, U> == result);
        }

        if constexpr (!is_same_v<U, add_lvalue_reference_t<U>>) {
            static_assert(equality_comparable_with<T, U&> == result);
            static_assert(equality_comparable_with<T, U const&> == result);
            static_assert(equality_comparable_with<T, U&&> == result);
            static_assert(equality_comparable_with<T, U const&&> == result);
        }

        if constexpr (!is_same_v<T, add_lvalue_reference_t<T>> && !is_same_v<U, add_lvalue_reference_t<U>>) {
            static_assert(equality_comparable_with<T&, U&> == result);
            static_assert(equality_comparable_with<T const&, U&> == result);
            static_assert(equality_comparable_with<T&&, U&> == result);
            static_assert(equality_comparable_with<T const&&, U&> == result);

            static_assert(equality_comparable_with<T&, U const&> == result);
            static_assert(equality_comparable_with<T const&, U const&> == result);
            static_assert(equality_comparable_with<T&&, U const&> == result);
            static_assert(equality_comparable_with<T const&&, U const&> == result);

            static_assert(equality_comparable_with<T&, U&&> == result);
            static_assert(equality_comparable_with<T const&, U&&> == result);
            static_assert(equality_comparable_with<T&&, U&&> == result);
            static_assert(equality_comparable_with<T const&&, U&&> == result);

            static_assert(equality_comparable_with<T&, U const&&> == result);
            static_assert(equality_comparable_with<T const&, U const&&> == result);
            static_assert(equality_comparable_with<T&&, U const&&> == result);
            static_assert(equality_comparable_with<T const&&, U const&&> == result);
        }

        return result;
    }
#pragma warning(pop)

    static_assert(!test<void>());
    static_assert(!test<int, void>());

    static_assert(test<int>());
    static_assert(test<double>());
    static_assert(test<int, double>());
    static_assert(test<std::nullptr_t>());
    static_assert(test<int*, void*>());
    static_assert(test<int[42]>());
    static_assert(test<int(int)>());

    template <int>
    struct CrossSpecializationComparable {
        bool operator==(CrossSpecializationComparable const&) const;
        bool operator!=(CrossSpecializationComparable const&) const;
    };
    static_assert(test<CrossSpecializationComparable<0>, CrossSpecializationComparable<0>>());

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
    static_assert(test<Common, Common>());
    static_assert(!test<CrossSpecializationComparable<0>, Common>());
    static_assert(test<CrossSpecializationComparable<1>, Common>());
    static_assert(test<CrossSpecializationComparable<2>, Common>());
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
    static_assert(!test<CrossSpecializationComparable<0>, CrossSpecializationComparable<1>>());
    // CrossSpecializationComparable<1> / <2> *do* satisfy common_reference_with.
    static_assert(test<CrossSpecializationComparable<1>, CrossSpecializationComparable<2>>());
} // namespace test_equality_comparable_with

namespace test_totally_ordered {
    using std::totally_ordered;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T>
    constexpr bool test() {
        constexpr bool result = totally_ordered<T>;
        static_assert(totally_ordered<T const> == result);
        if constexpr (!std::is_same_v<T, std::add_lvalue_reference_t<T>>) {
            static_assert(totally_ordered<T&> == result);
            static_assert(totally_ordered<T const&> == result);
            static_assert(totally_ordered<T&&> == result);
            static_assert(totally_ordered<T const&&> == result);
        }
        return result;
    }
#pragma warning(pop)

    static_assert(!test<void>());

    static_assert(test<int>());
    static_assert(test<double>());
    static_assert(test<void*>());
    static_assert(test<int*>());
    static_assert(test<int[42]>());
    static_assert(test<int(int)>());

#ifndef __EDG__ // TRANSITION, VSO-1898947
    static_assert(!test<std::nullptr_t>());
#endif // ^^^ no workaround ^^^
    static_assert(!test<EmptyClass>());

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
        static_assert(!test<Archetype<I>>());
    }

    template <std::size_t... Is>
    constexpr bool test_Archetype(std::index_sequence<Is...>) {
        static_assert(std::is_same_v<std::index_sequence<Is...>, std::make_index_sequence<Archetype_max>>);
        (test_archetype_single<Is>(), ...);
        static_assert(test<Archetype<Archetype_max>>());
        return true;
    }
    static_assert(test_Archetype(std::make_index_sequence<Archetype_max>{}));
} // namespace test_totally_ordered

namespace test_totally_ordered_with {
    using std::totally_ordered_with;

#pragma warning(push)
#pragma warning(disable : 4180) // qualifier applied to function type has no meaning; ignored
    template <class T, class U = T>
    constexpr bool test() {
        constexpr bool result = totally_ordered_with<T, U>;
        static_assert(totally_ordered_with<U, T> == result);
        if constexpr (!std::is_same_v<T, std::add_lvalue_reference_t<T>>) {
            static_assert(totally_ordered_with<T&, U> == result);
            static_assert(totally_ordered_with<T const&, U> == result);
            static_assert(totally_ordered_with<T&&, U> == result);
            static_assert(totally_ordered_with<T const&&, U> == result);
        }

        if constexpr (!std::is_same_v<U, std::add_lvalue_reference_t<U>>) {
            static_assert(totally_ordered_with<T, U&> == result);
            static_assert(totally_ordered_with<T, U const&> == result);
            static_assert(totally_ordered_with<T, U&&> == result);
            static_assert(totally_ordered_with<T, U const&&> == result);
        }

        if constexpr (!std::is_same_v<T, std::add_lvalue_reference_t<T>>
                      && !std::is_same_v<U, std::add_lvalue_reference_t<U>>) {
            static_assert(totally_ordered_with<T&, U&> == result);
            static_assert(totally_ordered_with<T const&, U&> == result);
            static_assert(totally_ordered_with<T&&, U&> == result);
            static_assert(totally_ordered_with<T const&&, U&> == result);

            static_assert(totally_ordered_with<T&, U const&> == result);
            static_assert(totally_ordered_with<T const&, U const&> == result);
            static_assert(totally_ordered_with<T&&, U const&> == result);
            static_assert(totally_ordered_with<T const&&, U const&> == result);

            static_assert(totally_ordered_with<T&, U&&> == result);
            static_assert(totally_ordered_with<T const&, U&&> == result);
            static_assert(totally_ordered_with<T&&, U&&> == result);
            static_assert(totally_ordered_with<T const&&, U&&> == result);

            static_assert(totally_ordered_with<T&, U const&&> == result);
            static_assert(totally_ordered_with<T const&, U const&&> == result);
            static_assert(totally_ordered_with<T&&, U const&&> == result);
            static_assert(totally_ordered_with<T const&&, U const&&> == result);
        }

        return result;
    }
#pragma warning(pop)

    static_assert(!test<void>());
    static_assert(!test<int, void>());

    static_assert(test<int>());
    static_assert(test<double>());
    static_assert(test<int, double>());
#ifndef __EDG__ // TRANSITION, VSO-1898947
    static_assert(!test<std::nullptr_t>());
#endif // ^^^ no workaround ^^^

    static_assert(test<void*>());
    static_assert(test<int*>());
    static_assert(test<int*, void*>());

    static_assert(test<int[42]>());

    static_assert(test<int(int)>());

    template <int>
    struct Ordered {
        bool operator==(Ordered const&) const;
        bool operator!=(Ordered const&) const;
        bool operator<(Ordered const&) const;
        bool operator>(Ordered const&) const;
        bool operator<=(Ordered const&) const;
        bool operator>=(Ordered const&) const;
    };
    static_assert(test<Ordered<0>, Ordered<0>>());
    static_assert(test<Ordered<1>, Ordered<1>>());
    static_assert(test<Ordered<2>, Ordered<2>>());

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
    static_assert(test<Common, Common>());
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
    static_assert(!test<Ordered<0>, Ordered<1>>());
    // Common is the common type of (Ordered<1>, Common) and (Ordered<2>, Common)
    static_assert(test<Ordered<1>, Common>());
    static_assert(test<Ordered<2>, Common>());
    // Ordered<1> / <2> have all the necessary operators, and model common_reference_with
    static_assert(test<Ordered<1>, Ordered<2>>());
} // namespace test_totally_ordered_with

namespace test_invocable_concepts {
    // Tests both invocable and regular_invocable concepts (since they're syntactically identical)

    using std::invocable, std::regular_invocable;

    template <class F, class... Args>
    constexpr bool test() {
        constexpr bool result = invocable<F, Args...>;
        static_assert(regular_invocable<F, Args...> == result);
        return result;
    }

    struct tag {};
    struct NotDerived {};

    void cc_independent_tests() {
        // PMD tests, which don't depend on calling conventions
        {
            using Fn = int tag::*;
            static_assert(!test<Fn>());
            {
                // N4849 [func.require]/1.4: "... N == 1 and f is a pointer to data member of a class T and
                // is_base_of_v<T, remove_reference_t<decltype(t_1)>> is true"
                static_assert(test<Fn, tag&>());
                static_assert(test<Fn, DerivesFrom<tag>&>());
                static_assert(test<Fn, tag&&>());
                static_assert(test<Fn, tag const&>());
            }
            {
                // N4849 [func.require]/1.5: "... N == 1 and f is a pointer to data member of a class T and
                // remove_cvref_t<decltype(t_1)> is a specialization of reference_wrapper"
                using T  = std::reference_wrapper<tag>;
                using DT = std::reference_wrapper<DerivesFrom<tag>>;
                using CT = std::reference_wrapper<tag const>;
                static_assert(test<Fn, T&>());
                static_assert(test<Fn, DT&>());
                static_assert(test<Fn, T const&>());
                static_assert(test<Fn, T&&>());
                static_assert(test<Fn, CT&>());
            }
            {
                // N4849 [func.require]/1.6: "... N == 1 and f is a pointer to data member of a class T and t_1 does not
                // satisfy the previous two items"
                using T  = tag*;
                using DT = DerivesFrom<tag>*;
                using CT = tag const*;
                using ST = std::unique_ptr<tag>;
                static_assert(test<Fn, T&>());
                static_assert(test<Fn, DT&>());
                static_assert(test<Fn, T const&>());
                static_assert(test<Fn, T&&>());
                static_assert(test<Fn, ST>());
                static_assert(test<Fn, CT&>());
            }
        }
        { // pointer to member data
            struct S {};
            using PMD = char S::*;
            static_assert(test<PMD, S&>());
            static_assert(test<PMD, S*>());
            static_assert(test<PMD, S* const>());
            static_assert(test<PMD, S const&>());
            static_assert(test<PMD, S const*>());
            static_assert(test<PMD, S volatile&>());
            static_assert(test<PMD, S volatile*>());
            static_assert(test<PMD, S const volatile&>());
            static_assert(test<PMD, S const volatile*>());
            static_assert(test<PMD, DerivesFrom<S>&>());
            static_assert(test<PMD, DerivesFrom<S> const&>());
            static_assert(test<PMD, DerivesFrom<S>*>());
            static_assert(test<PMD, DerivesFrom<S> const*>());
            static_assert(test<PMD, std::unique_ptr<S>>());
            static_assert(test<PMD, std::unique_ptr<S const>>());
            static_assert(test<PMD, std::reference_wrapper<S>>());
            static_assert(test<PMD, std::reference_wrapper<S const>>());
            static_assert(!test<PMD, NotDerived&>());
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

#ifndef _M_CEE // avoid warning C4561: '__fastcall' incompatible with the '/clr' option: converting to '__stdcall'
#define NAME      test_fast_fast
#define CALLCONV  __fastcall
#define MCALLCONV __fastcall
#include "invocable_cc.hpp"
#endif // _M_CEE

#define NAME      test_std_std
#define CALLCONV  __stdcall
#define MCALLCONV __stdcall
#include "invocable_cc.hpp"

#define NAME test_this
#define CALLCONV
#define MCALLCONV __thiscall
#include "invocable_cc.hpp"

#ifndef _M_CEE // avoid warning C4575: '__vectorcall' incompatible with the '/clr' option: converting to '__stdcall'
#if !defined(_M_ARM64) && !defined(_M_ARM64EC)
#define NAME      test_vector_vector
#define CALLCONV  __vectorcall
#define MCALLCONV __vectorcall
#include "invocable_cc.hpp"
#endif // ^^^ !ARM64 && !ARM64EC ^^^
#endif // _M_CEE

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
            using RFn = Bool (tag::*)(int) &&;
            {
                // N4849 [func.require]/1.1: "... f is a pointer to member function of a class T and
                // is_base_of_v<T, remove_reference_t<decltype(t_1)>> is true"
                static_assert(predicate<Fn, tag&, int>);
                static_assert(predicate<Fn, DerivesFrom<tag>&, int>);
                static_assert(predicate<RFn, tag&&, int>);
                static_assert(!predicate<RFn, tag&, int>);
                static_assert(!predicate<Fn, tag&>);
                static_assert(!predicate<Fn, tag const&, int>);
            }
            {
                // N4849 [func.require]/1.2: "... f is a pointer to a member function of a class T and
                // remove_cvref_t<decltype(t_1)> is a specialization of reference_wrapper"
                using T  = std::reference_wrapper<tag>;
                using DT = std::reference_wrapper<DerivesFrom<tag>>;
                using CT = std::reference_wrapper<tag const>;
                static_assert(predicate<Fn, T&, int>);
                static_assert(predicate<Fn, DT&, int>);
                static_assert(predicate<Fn, T const&, int>);
                static_assert(predicate<Fn, T&&, int>);
                static_assert(!predicate<Fn, CT&, int>);
                static_assert(!predicate<RFn, T, int>);
            }
            {
                // N4849 [func.require]/1.3: "... f is a pointer to a member function of a class T and t_1 does not
                // satisfy the previous two items"
                using T  = tag*;
                using DT = DerivesFrom<tag>*;
                using CT = tag const*;
                using ST = std::unique_ptr<tag>;
                static_assert(predicate<Fn, T&, int>);
                static_assert(predicate<Fn, DT&, int>);
                static_assert(predicate<Fn, T const&, int>);
                static_assert(predicate<Fn, T&&, int>);
                static_assert(predicate<Fn, ST, int>);
                static_assert(!predicate<Fn, CT&, int>);
                static_assert(!predicate<RFn, T, int>);
            }
        }
        {
            using Fn = Bool(tag::*);
            static_assert(!predicate<Fn>);
            {
                // N4849 [func.require]/1.4: "... N == 1 and f is a pointer to data member of a class T and
                // is_base_of_v<T, remove_reference_t<decltype(t_1)>> is true"
                static_assert(predicate<Fn, tag&>);
                static_assert(predicate<Fn, DerivesFrom<tag>&>);
                static_assert(predicate<Fn, tag&&>);
                static_assert(predicate<Fn, tag const&>);
            }
            {
                // N4849 [func.require]/1.5: "... N == 1 and f is a pointer to data member of a class T and
                // remove_cvref_t<decltype(t_1)> is a specialization of reference_wrapper"
                using T  = std::reference_wrapper<tag>;
                using DT = std::reference_wrapper<DerivesFrom<tag>>;
                using CT = std::reference_wrapper<tag const>;
                static_assert(predicate<Fn, T&>);
                static_assert(predicate<Fn, DT&>);
                static_assert(predicate<Fn, T const&>);
                static_assert(predicate<Fn, T&&>);
                static_assert(predicate<Fn, CT&>);
            }
            {
                // N4849 [func.require]/1.6: "... N == 1 and f is a pointer to data member of a class T and t_1 does not
                // satisfy the previous two items"
                using T  = tag*;
                using DT = DerivesFrom<tag>*;
                using CT = tag const*;
                using ST = std::unique_ptr<tag>;
                static_assert(predicate<Fn, T&>);
                static_assert(predicate<Fn, DT&>);
                static_assert(predicate<Fn, T const&>);
                static_assert(predicate<Fn, T&&>);
                static_assert(predicate<Fn, ST>);
                static_assert(predicate<Fn, CT&>);
            }
        }
        { // N4849 [func.require]/1.7: "f(t_1, t_2, ..., t_N) in all other cases"
            { // function pointer
                using Fp = Bool (*)(tag&, int);
                static_assert(predicate<Fp, tag&, int>);
                static_assert(predicate<Fp, DerivesFrom<tag>&, int>);
                static_assert(!predicate<Fp, tag const&, int>);
                static_assert(!predicate<Fp>);
                static_assert(!predicate<Fp, tag&>);
            }
            { // function reference
                using Fp = Bool (&)(tag&, int);
                static_assert(predicate<Fp, tag&, int>);
                static_assert(predicate<Fp, DerivesFrom<tag>&, int>);
                static_assert(!predicate<Fp, tag const&, int>);
                static_assert(!predicate<Fp>);
                static_assert(!predicate<Fp, tag&>);
            }
            { // function object
                using Fn = NotCallableWithInt;
                static_assert(predicate<Fn, tag>);
                static_assert(!predicate<Fn, int>);
            }
        }

        { // function object
            static_assert(predicate<S, int>);
            static_assert(predicate<S&, unsigned char, int&>);
            static_assert(predicate<S const&, unsigned char, int&>);
            static_assert(!predicate<S volatile&, unsigned char, int&>);
            static_assert(!predicate<S const volatile&, unsigned char, int&>);

            static_assert(predicate<ImplicitTo<bool (*)(long)>, int>);
            static_assert(predicate<ImplicitTo<bool (*)(long)>, char>);
            static_assert(predicate<ImplicitTo<bool (*)(long)>, float>);
            static_assert(!predicate<ImplicitTo<bool (*)(long)>, char const*>);
            static_assert(!predicate<ImplicitTo<bool (*)(long)>, S const&>);

            auto omega_mu = [x = 42](int i, double) { return x == i; };
            static_assert(predicate<decltype((omega_mu)), int, double>);
            static_assert(predicate<decltype((omega_mu)), double, int>);
            static_assert(predicate<decltype((omega_mu)), char, char>);
            static_assert(!predicate<decltype((omega_mu))>);
            static_assert(!predicate<decltype((omega_mu)), char const*, double>);
            static_assert(!predicate<decltype((omega_mu)), double, char const*>);
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
            static_assert(predicate<RF0>);
            static_assert(predicate<RF1, int>);
            static_assert(predicate<RF2, int, long>);
            static_assert(predicate<RF3, int, long, int>);
            static_assert(predicate<RF4, int, float, void*>);
            static_assert(predicate<PF0>);
            static_assert(predicate<PF1, int>);
            static_assert(predicate<PF2, int, long>);
            static_assert(predicate<PF3, int, long, int>);
            static_assert(predicate<PF4, int, float, void*>);
            static_assert(predicate<RPF0>);
            static_assert(predicate<RPF1, int>);
            static_assert(predicate<RPF2, int, long>);
            static_assert(predicate<RPF3, int, long, int>);
            static_assert(predicate<RPF4, int, float, void*>);
        }

        { // pointer to member function
            using PMF0  = Bool (S::*)();
            using PMF1  = Bool (S::*)(long);
            using PMF2  = Bool& (S::*) (long, int);
            using PMF1P = Bool const& (S::*) (int, ...);
            static_assert(predicate<PMF0, S>);
            static_assert(predicate<PMF0, S&>);
            static_assert(predicate<PMF0, S*>);
            static_assert(predicate<PMF0, S*&>);
            static_assert(predicate<PMF0, std::reference_wrapper<S>>);
            static_assert(predicate<PMF0, std::reference_wrapper<S> const&>);
            static_assert(predicate<PMF0, std::reference_wrapper<DerivesFrom<S>>>);
            static_assert(predicate<PMF0, std::reference_wrapper<DerivesFrom<S>> const&>);
            static_assert(predicate<PMF0, std::unique_ptr<S>>);
            static_assert(predicate<PMF0, std::unique_ptr<DerivesFrom<S>>>);
            static_assert(!predicate<PMF0, S const&>);
            static_assert(!predicate<PMF0, S volatile&>);
            static_assert(!predicate<PMF0, S const volatile&>);
            static_assert(!predicate<PMF0, NotDerived&>);
            static_assert(!predicate<PMF0, NotDerived const&>);
            static_assert(!predicate<PMF0, std::unique_ptr<S const>>);
            static_assert(!predicate<PMF0, std::reference_wrapper<S const>>);
            static_assert(!predicate<PMF0, std::reference_wrapper<NotDerived>>);
            static_assert(!predicate<PMF0, std::unique_ptr<NotDerived>>);

            static_assert(predicate<PMF1, S, int>);
            static_assert(predicate<PMF1, S&, int>);
            static_assert(predicate<PMF1, S*, int>);
            static_assert(predicate<PMF1, S*&, int>);
            static_assert(predicate<PMF1, std::unique_ptr<S>, int>);
            static_assert(predicate<PMF1, std::unique_ptr<DerivesFrom<S>>, int>);
            static_assert(predicate<PMF1, std::reference_wrapper<S>, int>);
            static_assert(predicate<PMF1, std::reference_wrapper<S> const&, int>);
            static_assert(predicate<PMF1, std::reference_wrapper<DerivesFrom<S>>, int>);
            static_assert(predicate<PMF1, std::reference_wrapper<DerivesFrom<S>> const&, int>);
            static_assert(!predicate<PMF1, S const&, int>);
            static_assert(!predicate<PMF1, S volatile&, int>);
            static_assert(!predicate<PMF1, S const volatile&, int>);
            static_assert(!predicate<PMF1, NotDerived&, int>);
            static_assert(!predicate<PMF1, NotDerived const&, int>);
            static_assert(!predicate<PMF1, std::unique_ptr<S const>, int>);
            static_assert(!predicate<PMF1, std::reference_wrapper<S const>, int>);
            static_assert(!predicate<PMF1, std::reference_wrapper<NotDerived>, int>);
            static_assert(!predicate<PMF1, std::unique_ptr<NotDerived>, int>);

            static_assert(predicate<PMF2, S, int, int>);
            static_assert(predicate<PMF2, S&, int, int>);
            static_assert(predicate<PMF2, S*, int, int>);
            static_assert(predicate<PMF2, S*&, int, int>);
            static_assert(predicate<PMF2, std::unique_ptr<S>, int, int>);
            static_assert(predicate<PMF2, std::unique_ptr<DerivesFrom<S>>, int, int>);
            static_assert(predicate<PMF2, std::reference_wrapper<S>, int, int>);
            static_assert(predicate<PMF2, std::reference_wrapper<S> const&, int, int>);
            static_assert(predicate<PMF2, std::reference_wrapper<DerivesFrom<S>>, int, int>);
            static_assert(predicate<PMF2, std::reference_wrapper<DerivesFrom<S>> const&, int, int>);
            static_assert(!predicate<PMF2, S const&, int, int>);
            static_assert(!predicate<PMF2, S volatile&, int, int>);
            static_assert(!predicate<PMF2, S const volatile&, int, int>);
            static_assert(!predicate<PMF2, std::unique_ptr<S const>, int, int>);
            static_assert(!predicate<PMF2, std::reference_wrapper<S const>, int, int>);
            static_assert(!predicate<PMF2, NotDerived const&, int, int>);
            static_assert(!predicate<PMF2, std::reference_wrapper<NotDerived>, int, int>);
            static_assert(!predicate<PMF2, std::unique_ptr<NotDerived>, int, int>);

            static_assert(predicate<PMF1P, S&, int>);
            static_assert(predicate<PMF1P, S&, int, long>);

            using PMF0C  = bool (S::*)() const;
            using PMF1C  = bool (S::*)(long) const;
            using PMF2C  = bool (S::*)(long, int) const;
            using PMF1PC = bool const& (S::*) (int, ...) const;
            static_assert(predicate<PMF0C, S>);
            static_assert(predicate<PMF0C, S&>);
            static_assert(predicate<PMF0C, S const&>);
            static_assert(predicate<PMF0C, S*>);
            static_assert(predicate<PMF0C, S const*>);
            static_assert(predicate<PMF0C, S*&>);
            static_assert(predicate<PMF0C, S const*&>);
            static_assert(predicate<PMF0C, std::unique_ptr<S>>);
            static_assert(predicate<PMF0C, std::unique_ptr<DerivesFrom<S>>>);
            static_assert(predicate<PMF0C, std::reference_wrapper<S>>);
            static_assert(predicate<PMF0C, std::reference_wrapper<S const>>);
            static_assert(predicate<PMF0C, std::reference_wrapper<S> const&>);
            static_assert(predicate<PMF0C, std::reference_wrapper<S const> const&>);
            static_assert(predicate<PMF0C, std::reference_wrapper<DerivesFrom<S>>>);
            static_assert(predicate<PMF0C, std::reference_wrapper<DerivesFrom<S> const>>);
            static_assert(predicate<PMF0C, std::reference_wrapper<DerivesFrom<S>> const&>);
            static_assert(predicate<PMF0C, std::reference_wrapper<DerivesFrom<S> const> const&>);
            static_assert(!predicate<PMF0C, S volatile&>);
            static_assert(!predicate<PMF0C, S const volatile&>);

            static_assert(predicate<PMF1C, S, int>);
            static_assert(predicate<PMF1C, S&, int>);
            static_assert(predicate<PMF1C, S const&, int>);
            static_assert(predicate<PMF1C, S*, int>);
            static_assert(predicate<PMF1C, S const*, int>);
            static_assert(predicate<PMF1C, S*&, int>);
            static_assert(predicate<PMF1C, S const*&, int>);
            static_assert(predicate<PMF1C, std::unique_ptr<S>, int>);
            static_assert(!predicate<PMF1C, S volatile&, int>);
            static_assert(!predicate<PMF1C, S const volatile&, int>);

            static_assert(predicate<PMF2C, S, int, int>);
            static_assert(predicate<PMF2C, S&, int, int>);
            static_assert(predicate<PMF2C, S const&, int, int>);
            static_assert(predicate<PMF2C, S*, int, int>);
            static_assert(predicate<PMF2C, S const*, int, int>);
            static_assert(predicate<PMF2C, S*&, int, int>);
            static_assert(predicate<PMF2C, S const*&, int, int>);
            static_assert(predicate<PMF2C, std::unique_ptr<S>, int, int>);
            static_assert(!predicate<PMF2C, S volatile&, int, int>);
            static_assert(!predicate<PMF2C, S const volatile&, int, int>);

            static_assert(predicate<PMF1PC, S&, int>);
            static_assert(predicate<PMF1PC, S&, int, long>);

            using PMF0V  = bool (S::*)() volatile;
            using PMF1V  = bool (S::*)(long) volatile;
            using PMF2V  = bool (S::*)(long, int) volatile;
            using PMF1PV = bool const& (S::*) (int, ...) volatile;
            static_assert(predicate<PMF0V, S>);
            static_assert(predicate<PMF0V, S&>);
            static_assert(predicate<PMF0V, S volatile&>);
            static_assert(predicate<PMF0V, S*>);
            static_assert(predicate<PMF0V, S volatile*>);
            static_assert(predicate<PMF0V, S*&>);
            static_assert(predicate<PMF0V, S volatile*&>);
            static_assert(predicate<PMF0V, std::unique_ptr<S>>);
            static_assert(!predicate<PMF0V, S const&>);
            static_assert(!predicate<PMF0V, S const volatile&>);

            static_assert(predicate<PMF1V, S, int>);
            static_assert(predicate<PMF1V, S&, int>);
            static_assert(predicate<PMF1V, S volatile&, int>);
            static_assert(predicate<PMF1V, S*, int>);
            static_assert(predicate<PMF1V, S volatile*, int>);
            static_assert(predicate<PMF1V, S*&, int>);
            static_assert(predicate<PMF1V, S volatile*&, int>);
            static_assert(predicate<PMF1V, std::unique_ptr<S>, int>);
            static_assert(!predicate<PMF1V, S const&, int>);
            static_assert(!predicate<PMF1V, S const volatile&, int>);

            static_assert(predicate<PMF2V, S, int, int>);
            static_assert(predicate<PMF2V, S&, int, int>);
            static_assert(predicate<PMF2V, S volatile&, int, int>);
            static_assert(predicate<PMF2V, S*, int, int>);
            static_assert(predicate<PMF2V, S volatile*, int, int>);
            static_assert(predicate<PMF2V, S*&, int, int>);
            static_assert(predicate<PMF2V, S volatile*&, int, int>);
            static_assert(predicate<PMF2V, std::unique_ptr<S>, int, int>);
            static_assert(!predicate<PMF2V, S const&, int, int>);
            static_assert(!predicate<PMF2V, S const volatile&, int, int>);

            static_assert(predicate<PMF1PV, S&, int>);
            static_assert(predicate<PMF1PV, S&, int, long>);

            using PMF0CV  = Bool (S::*)() const volatile;
            using PMF1CV  = Bool (S::*)(long) const volatile;
            using PMF2CV  = Bool (S::*)(long, int) const volatile;
            using PMF1PCV = Bool const& (S::*) (int, ...) const volatile;
            static_assert(predicate<PMF0CV, S>);
            static_assert(predicate<PMF0CV, S&>);
            static_assert(predicate<PMF0CV, S const&>);
            static_assert(predicate<PMF0CV, S volatile&>);
            static_assert(predicate<PMF0CV, S const volatile&>);
            static_assert(predicate<PMF0CV, S*>);
            static_assert(predicate<PMF0CV, S const*>);
            static_assert(predicate<PMF0CV, S volatile*>);
            static_assert(predicate<PMF0CV, S const volatile*>);
            static_assert(predicate<PMF0CV, S*&>);
            static_assert(predicate<PMF0CV, S const*&>);
            static_assert(predicate<PMF0CV, S volatile*&>);
            static_assert(predicate<PMF0CV, S const volatile*&>);
            static_assert(predicate<PMF0CV, std::unique_ptr<S>>);

            static_assert(predicate<PMF1CV, S, int>);
            static_assert(predicate<PMF1CV, S&, int>);
            static_assert(predicate<PMF1CV, S const&, int>);
            static_assert(predicate<PMF1CV, S volatile&, int>);
            static_assert(predicate<PMF1CV, S const volatile&, int>);
            static_assert(predicate<PMF1CV, S*, int>);
            static_assert(predicate<PMF1CV, S const*, int>);
            static_assert(predicate<PMF1CV, S volatile*, int>);
            static_assert(predicate<PMF1CV, S const volatile*, int>);
            static_assert(predicate<PMF1CV, S*&, int>);
            static_assert(predicate<PMF1CV, S const*&, int>);
            static_assert(predicate<PMF1CV, S volatile*&, int>);
            static_assert(predicate<PMF1CV, S const volatile*&, int>);
            static_assert(predicate<PMF1CV, std::unique_ptr<S>, int>);

            static_assert(predicate<PMF2CV, S, int, int>);
            static_assert(predicate<PMF2CV, S&, int, int>);
            static_assert(predicate<PMF2CV, S const&, int, int>);
            static_assert(predicate<PMF2CV, S volatile&, int, int>);
            static_assert(predicate<PMF2CV, S const volatile&, int, int>);
            static_assert(predicate<PMF2CV, S*, int, int>);
            static_assert(predicate<PMF2CV, S const*, int, int>);
            static_assert(predicate<PMF2CV, S volatile*, int, int>);
            static_assert(predicate<PMF2CV, S const volatile*, int, int>);
            static_assert(predicate<PMF2CV, S*&, int, int>);
            static_assert(predicate<PMF2CV, S const*&, int, int>);
            static_assert(predicate<PMF2CV, S volatile*&, int, int>);
            static_assert(predicate<PMF2CV, S const volatile*&, int, int>);
            static_assert(predicate<PMF2CV, std::unique_ptr<S>, int, int>);

            static_assert(predicate<PMF1PCV, S&, int>);
            static_assert(predicate<PMF1PCV, S&, int, long>);
        }

        { // pointer to member data
            using PMD = bool S::*;
            static_assert(predicate<PMD, S&>);
            static_assert(predicate<PMD, S*>);
            static_assert(predicate<PMD, S* const>);
            static_assert(predicate<PMD, S const&>);
            static_assert(predicate<PMD, S const*>);
            static_assert(predicate<PMD, S volatile&>);
            static_assert(predicate<PMD, S volatile*>);
            static_assert(predicate<PMD, S const volatile&>);
            static_assert(predicate<PMD, S const volatile*>);
            static_assert(predicate<PMD, DerivesFrom<S>&>);
            static_assert(predicate<PMD, DerivesFrom<S> const&>);
            static_assert(predicate<PMD, DerivesFrom<S>*>);
            static_assert(predicate<PMD, DerivesFrom<S> const*>);
            static_assert(predicate<PMD, std::unique_ptr<S>>);
            static_assert(predicate<PMD, std::unique_ptr<S const>>);
            static_assert(predicate<PMD, std::reference_wrapper<S>>);
            static_assert(predicate<PMD, std::reference_wrapper<S const>>);
            static_assert(!predicate<PMD, NotDerived&>);
        }
    }
} // namespace test_predicate

namespace test_relation {
    // Tests relation, equivalence_relation, and strict_weak_order (since they are syntactically identical)

    using std::relation, std::equivalence_relation, std::strict_weak_order;

    template <class F, class T, class U = T>
    constexpr bool test() {
        constexpr bool result = relation<F, T, U>;
        static_assert(relation<F, U, T> == result);
        static_assert(equivalence_relation<F, T, U> == result);
        static_assert(equivalence_relation<F, U, T> == result);
        static_assert(strict_weak_order<F, T, U> == result);
        static_assert(strict_weak_order<F, U, T> == result);
        return result;
    }

    static_assert(test<std::equal_to<>, int, long>());
    static_assert(test<std::less<>, int*, void*>());

    struct Equivalent {
        template <class T, class U>
        constexpr decltype(auto) operator()(T&& t, U&& u) const
            requires requires { static_cast<T &&>(t) == static_cast<U &&>(u); }
        {
            return static_cast<T&&>(t) == static_cast<U&&>(u);
        }
    };
    static_assert(test<Equivalent, int, long>());

    struct Bool {
        operator bool() const;
    };

    template <unsigned int>
    struct A {};
    template <unsigned int U>
        requires (0 < U)
    Bool operator==(A<U>, A<U>); // A<0> == A<0> is invalid
    static_assert(!test<Equivalent, A<0>>());
    static_assert(test<Equivalent, A<1>>());

    template <unsigned int>
    struct B {};
    void operator==(B<1>, B<1>); // B<1> == B<1> does not model _Boolean_testable
    template <unsigned int U>
    bool operator==(B<U>, B<U>);
    static_assert(test<Equivalent, B<0>>());
    static_assert(!test<Equivalent, B<1>>());

    template <unsigned int I>
        requires (2 != I)
    bool operator==(A<I>, B<I>); // A<2> == B<2> rewrites to B<2> == A<2>
    template <unsigned int I>
        requires (3 != I)
    bool operator==(B<I>, A<I>); // B<3> == A<3> rewrites to A<3> == B<3>

    static_assert(!test<Equivalent, A<0>, B<0>>());
    static_assert(!test<Equivalent, A<1>, B<1>>());
    static_assert(test<Equivalent, A<2>, B<2>>());
    static_assert(test<Equivalent, A<3>, B<3>>());
    static_assert(test<Equivalent, A<4>, B<4>>());

    template <unsigned int I>
    struct C {};
    enum E : bool { No, Yes };
    E operator==(C<0>&, C<0>&); // const C<0> == const C<0> is invalid
    template <unsigned int I>
        requires (0 != I)
    E operator==(C<I>, C<I>);

    static_assert(!test<Equivalent, const C<0>>());
    static_assert(test<Equivalent, const C<1>>());
} // namespace test_relation

namespace test_uniform_random_bit_generator {
    using std::uniform_random_bit_generator;

    static_assert(uniform_random_bit_generator<std::minstd_rand0>);
    static_assert(uniform_random_bit_generator<std::minstd_rand>);
    static_assert(uniform_random_bit_generator<std::mt19937>);
    static_assert(uniform_random_bit_generator<std::mt19937_64>);
    static_assert(uniform_random_bit_generator<std::ranlux24_base>);
    static_assert(uniform_random_bit_generator<std::ranlux48_base>);
    static_assert(uniform_random_bit_generator<std::ranlux24>);
    static_assert(uniform_random_bit_generator<std::ranlux48>);
    static_assert(uniform_random_bit_generator<std::knuth_b>);
    static_assert(uniform_random_bit_generator<std::default_random_engine>);
    static_assert(uniform_random_bit_generator<std::random_device>);

    struct NoCall {
        static constexpr unsigned int min() {
            return 0;
        }
        static constexpr unsigned int max() {
            return 42;
        }
    };
    static_assert(!uniform_random_bit_generator<NoCall>);

    struct NoLvalueCall {
        static constexpr unsigned int min() {
            return 0;
        }
        static constexpr unsigned int max() {
            return 42;
        }
        unsigned int operator()() &&;
    };
    static_assert(!uniform_random_bit_generator<NoLvalueCall>);

    struct SignedValue {
        static constexpr int min() {
            return 0;
        }
        static constexpr int max() {
            return 42;
        }
        int operator()();
    };
    static_assert(!uniform_random_bit_generator<SignedValue>);

    struct NoMin {
        static constexpr unsigned int max() {
            return 42;
        }
        unsigned int operator()();
    };
    static_assert(!uniform_random_bit_generator<NoMin>);

    struct NonConstexprMin {
        static unsigned int min() {
            return 0;
        }
        static constexpr unsigned int max() {
            return 42;
        }
        unsigned int operator()();
    };
    static_assert(!uniform_random_bit_generator<NonConstexprMin>);

    struct BadMin {
        static constexpr int min() {
            return 0;
        }
        static constexpr unsigned int max() {
            return 42;
        }
        unsigned int operator()();
    };
    static_assert(!uniform_random_bit_generator<BadMin>);

    struct NoMax {
        static constexpr unsigned int min() {
            return 0;
        }
        unsigned int operator()();
    };
    static_assert(!uniform_random_bit_generator<NoMax>);

    struct NonConstexprMax {
        static constexpr unsigned int min() {
            return 0;
        }
        static unsigned int max() {
            return 42;
        }
        unsigned int operator()();
    };
    static_assert(!uniform_random_bit_generator<NonConstexprMax>);

    struct BadMax {
        static constexpr unsigned int min() {
            return 0;
        }
        static constexpr int max() {
            return 42;
        }
        unsigned int operator()();
    };
    static_assert(!uniform_random_bit_generator<BadMax>);

    struct EmptyRange {
        static constexpr unsigned int min() {
            return 0;
        }
        static constexpr unsigned int max() {
            return 0;
        }
        unsigned int operator()();
    };
    static_assert(!uniform_random_bit_generator<EmptyRange>);

    struct ReversedRange {
        static constexpr unsigned int min() {
            return 42;
        }
        static constexpr unsigned int max() {
            return 0;
        }
        unsigned int operator()();
    };
    static_assert(!uniform_random_bit_generator<ReversedRange>);

    struct URBG {
        static constexpr unsigned int min() {
            return 0;
        }
        static constexpr unsigned int max() {
            return 42;
        }
        unsigned int operator()();
    };
    static_assert(uniform_random_bit_generator<URBG>);
    static_assert(!uniform_random_bit_generator<const URBG>);

    struct ConstURBG {
        static constexpr unsigned int min() {
            return 0;
        }
        static constexpr unsigned int max() {
            return 42;
        }
        unsigned int operator()() const;
    };
    static_assert(uniform_random_bit_generator<ConstURBG>);
    static_assert(uniform_random_bit_generator<const ConstURBG>);
} // namespace test_uniform_random_bit_generator

int main() {
    test_ranges_swap::runtime_tests();
    test_swappable_with::example::test();
}
