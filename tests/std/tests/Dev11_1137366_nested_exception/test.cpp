// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

STATIC_ASSERT(is_polymorphic_v<nested_exception>);
STATIC_ASSERT(has_virtual_destructor_v<nested_exception>);
STATIC_ASSERT(!is_final_v<nested_exception>);

STATIC_ASSERT(is_nothrow_default_constructible_v<nested_exception>);
STATIC_ASSERT(is_nothrow_copy_constructible_v<nested_exception>);
STATIC_ASSERT(is_nothrow_move_constructible_v<nested_exception>);
STATIC_ASSERT(is_nothrow_copy_assignable_v<nested_exception>);
STATIC_ASSERT(is_nothrow_move_assignable_v<nested_exception>);
STATIC_ASSERT(is_nothrow_destructible_v<nested_exception>);

STATIC_ASSERT(noexcept(nested_exception().nested_ptr()));

template <typename Exception, typename Func, typename Checker>
void assert_thrown(Func func, Checker checker) {

    try {
        func();
        abort();
    } catch (const Exception& e) {
        checker(e);
    }
}

struct Final final {};

STATIC_ASSERT(is_final_v<Final>);

struct AlreadyNested : nested_exception {};

struct PolymorphicInt {
    explicit PolymorphicInt(const int n) : m_n(n) {}

    PolymorphicInt(const PolymorphicInt&)            = default;
    PolymorphicInt& operator=(const PolymorphicInt&) = default;
    virtual ~PolymorphicInt()                        = default;

    int m_n;
};

int cube(const int n) {
    return n * n * n;
}

struct Nonpolymorphic {};

class PrivateNested : private nested_exception {};

struct AmbiguousNested1 : nested_exception {};
struct AmbiguousNested2 : nested_exception {};
struct AmbiguousNested : AmbiguousNested1, AmbiguousNested2 {};

struct Icky : PolymorphicInt, PrivateNested {
    Icky() : PolymorphicInt(-1), PrivateNested() {}
};

struct Yucky : PolymorphicInt, AmbiguousNested {
    Yucky() : PolymorphicInt(-2), AmbiguousNested() {}
};

struct GoodNested : PolymorphicInt, nested_exception {
    GoodNested() : PolymorphicInt(343), nested_exception() {}
};

int main() {
    {
        nested_exception outer;

        assert(!outer.nested_ptr());

        try {
            throw runtime_error("WOOF");
        } catch (...) {
            nested_exception inner;
            outer = inner;
        }

        assert(!!outer.nested_ptr());

        auto woof_checker = [](const runtime_error& e) { assert(e.what() == "WOOF"s); };

        assert_thrown<runtime_error>([&] { rethrow_exception(outer.nested_ptr()); }, woof_checker);

        assert_thrown<runtime_error>([&] { outer.rethrow_nested(); }, woof_checker);

        nested_exception dupe(outer);
        assert(dupe.nested_ptr() == outer.nested_ptr());
    }

    {
        assert_thrown<int>([] { throw_with_nested(1729); }, [](const int e) { assert(e == 1729); });

        assert_thrown<Final>([] { throw_with_nested(Final()); }, [](const Final&) { assert(true); });

        assert_thrown<AlreadyNested>([] { throw_with_nested(AlreadyNested()); },
            [](const AlreadyNested& e) { assert(typeid(e) == typeid(AlreadyNested)); });

        assert_thrown<nested_exception>([] { throw_with_nested(nested_exception()); },
            [](const nested_exception& e) { assert(typeid(e) == typeid(nested_exception)); });

        assert_thrown<PolymorphicInt>([] { throw_with_nested(PolymorphicInt(123)); },
            [](const PolymorphicInt& e) {
                assert(e.m_n == 123);
                assert(typeid(e) != typeid(PolymorphicInt));
                const auto p = dynamic_cast<const nested_exception*>(&e);
                assert(p != nullptr);
            });

        // LWG-2855 "std::throw_with_nested("string_literal")"
        assert_thrown<const char*>(
            [] { throw_with_nested("meow"); }, [](const char* const p) { assert(p == "meow"s); });

        using FP = int (*)(int);

        assert_thrown<FP>([] { throw_with_nested(cube); }, [](const FP fp) { assert(fp(7) == 343); });
    }

    {
        // NON-POLYMORPHIC INPUTS:

        // Non-classes can't possibly derive from nested_exception.
        rethrow_if_nested(19937);

        // Classes might derive from nested_exception, but if we're given
        // a non-polymorphic input, we can't possibly sense nested_exception.
        rethrow_if_nested(Nonpolymorphic());


        // POLYMORPHIC INPUTS:

        Icky icky;
        PolymorphicInt& icky_pi_base = icky;

        Yucky yucky;
        PolymorphicInt& yucky_pi_base = yucky;


        GoodNested good;
        PolymorphicInt& good_pi_base   = good;
        nested_exception& good_ne_base = good;

        try {
            throw runtime_error("HISS");
        } catch (...) {
            nested_exception inner;
            good_ne_base = inner;
        }

        auto hiss_checker = [](const runtime_error& e) { assert(e.what() == "HISS"s); };


        nested_exception ne = good_ne_base;


        // 1a. Statically unrelated, dynamically unrelated:
        rethrow_if_nested(PolymorphicInt(256));

        // 1b. Statically unrelated, dynamically derives badly (inaccessible or ambiguous):
        rethrow_if_nested(icky_pi_base);
        rethrow_if_nested(yucky_pi_base);

        // 1c. Statically unrelated, dynamically derives well (public and unambiguous):
        assert_thrown<runtime_error>([&] { rethrow_if_nested(good_pi_base); }, hiss_checker);

        // 2. Statically derives badly:
        rethrow_if_nested(PrivateNested());
        rethrow_if_nested(AmbiguousNested());

        // 3a. Statically derives well, dynamically derives badly:
        // Can't detect this case.

        // 3b. Statically derives well, dynamically derives well:
        assert_thrown<runtime_error>([&] { rethrow_if_nested(good); }, hiss_checker);

        // 4a. Statically nested_exception itself, dynamically derives badly:
        // Can't detect this case.

        // 4b. Statically nested_exception itself, dynamically derives well:
        assert_thrown<runtime_error>([&] { rethrow_if_nested(good_ne_base); }, hiss_checker);

        // 4c. Statically nested_exception itself, dynamically nested_exception itself:
        assert_thrown<runtime_error>([&] { rethrow_if_nested(ne); }, hiss_checker);
    }
}
