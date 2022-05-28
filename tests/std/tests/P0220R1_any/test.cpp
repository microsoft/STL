// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Organization of this file:
// * a short header (including this comment)
// * `// LLVM SOURCES BEGIN`
// * The contents of several libc++ `test/std/utilities/any` test files, each delimited by `// -- BEGIN/END: <filename>`
//   comments. These contents have been modified to merge many tests into one by:
//     (1) changing `int main(int, char**)` to `int run_test()`, and
//     (2) wrapping everything other than comments and includes in a unique namespace per-file, using namespace nesting
//         to reflect the hierarchical structure of the upstream sources.
// * `// LLVM SOURCES END`
// * Several more MSVC-STL-specific test cases laid out in a nested namespace structure within `msvc`.
// * The `main` function, which simply calls each of the `run_test` functions in both the adapted libc++ test cases and
//   the MSVC-specific test cases.
//
// The LLVM sources are updated manually:
// 1. Navigate a bash prompt to `libcxx` in an LLVM monorepo.
// 2. Redirect the output of the bash loop:
//      for f in $(find test/std/utilities/any -name '*.pass.cpp');
//        do echo "// -- BEGIN: $f";
//        sed -e 's/int main(int, char\*\*)/int run_test()/; s/FIXME/TODO/g' < $f;
//        echo -e "// -- END: $f\n";
//      done
//    into a file.
// 3. Replicate the namespace structure from here into that file, use its content to replace everything between the
//    "LLVM SOURCES BEGIN"/"END" delimiters, and ensure that `main` properly calls each of the `run_test` functions.
//
// Yes, this is an awkward hand process; notably the required headers can change without notice. We should investigate
// running the libc++ tests directly in all of our configurations so we needn't replicate this subset of files.

#define _HAS_DEPRECATED_IS_LITERAL_TYPE 1
#define _SILENCE_CXX17_IS_LITERAL_TYPE_DEPRECATION_WARNING
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
#define _LIBCXX_IN_DEVCRT
#include <msvc_stdlib_force_include.h> // Must precede any other libc++ headers
#include <stdlib.h>

#pragma warning(push)
#pragma warning(disable : 4365) // 'initializing': conversion from 'int' to 'const size_t', signed/unsigned mismatch
#include "count_new.h"
#pragma warning(pop)

// clang-format off
// LLVM SOURCES BEGIN
// -- BEGIN: test/std/utilities/any/any.class/any.assign/copy.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_any_cast && !libcpp-no-exceptions

// <any>

// any& operator=(any const &);

// Test copy assignment

#include <any>
#include <cassert>

#include "any_helpers.h"
#include "count_new.h"
#include "test_macros.h"

namespace assign::copy {
using std::any;
using std::any_cast;

template <class LHS, class RHS>
void test_copy_assign() {
    assert(LHS::count == 0);
    assert(RHS::count == 0);
    LHS::reset();
    RHS::reset();
    {
        any lhs(LHS(1));
        any const rhs(RHS(2));

        assert(LHS::count == 1);
        assert(RHS::count == 1);
        assert(RHS::copied == 0);

        lhs = rhs;

        assert(RHS::copied == 1);
        assert(LHS::count == 0);
        assert(RHS::count == 2);

        assertContains<RHS>(lhs, 2);
        assertContains<RHS>(rhs, 2);
    }
    assert(LHS::count == 0);
    assert(RHS::count == 0);
}

template <class LHS>
void test_copy_assign_empty() {
    assert(LHS::count == 0);
    LHS::reset();
    {
        any lhs;
        any const rhs(LHS(42));

        assert(LHS::count == 1);
        assert(LHS::copied == 0);

        lhs = rhs;

        assert(LHS::copied == 1);
        assert(LHS::count == 2);

        assertContains<LHS>(lhs, 42);
        assertContains<LHS>(rhs, 42);
    }
    assert(LHS::count == 0);
    LHS::reset();
    {
        any lhs(LHS(1));
        any const rhs;

        assert(LHS::count == 1);
        assert(LHS::copied == 0);

        lhs = rhs;

        assert(LHS::copied == 0);
        assert(LHS::count == 0);

        assertEmpty<LHS>(lhs);
        assertEmpty(rhs);
    }
    assert(LHS::count == 0);
}

void test_copy_assign_self() {
    // empty
    {
        any a;
        a = (any &)a;
        assertEmpty(a);
        assert(globalMemCounter.checkOutstandingNewEq(0));
    }
    assert(globalMemCounter.checkOutstandingNewEq(0));
    // small
    {
        any a((small(1)));
        assert(small::count == 1);

        a = (any &)a;

        assert(small::count == 1);
        assertContains<small>(a, 1);
        assert(globalMemCounter.checkOutstandingNewEq(0));
    }
    assert(small::count == 0);
    assert(globalMemCounter.checkOutstandingNewEq(0));
    // large
    {
        any a(large(1));
        assert(large::count == 1);

        a = (any &)a;

        assert(large::count == 1);
        assertContains<large>(a, 1);
        assert(globalMemCounter.checkOutstandingNewEq(1));
    }
    assert(large::count == 0);
    assert(globalMemCounter.checkOutstandingNewEq(0));
}

template <class Tp>
void test_copy_assign_throws()
{
#if !defined(TEST_HAS_NO_EXCEPTIONS)
    auto try_throw =
    [](any& lhs, any const& rhs) {
        try {
            lhs = rhs;
            abort();
        } catch (my_any_exception const &) {
            // do nothing
        }
    };
    // const lvalue to empty
    {
        any lhs;
        any const rhs((Tp(1)));
        assert(Tp::count == 1);

        try_throw(lhs, rhs);

        assert(Tp::count == 1);
        assertEmpty<Tp>(lhs);
        assertContains<Tp>(rhs, 1);
    }
    {
        any lhs((small(2)));
        any const rhs((Tp(1)));
        assert(small::count == 1);
        assert(Tp::count == 1);

        try_throw(lhs, rhs);

        assert(small::count == 1);
        assert(Tp::count == 1);
        assertContains<small>(lhs, 2);
        assertContains<Tp>(rhs, 1);
    }
    {
        any lhs((large(2)));
        any const rhs((Tp(1)));
        assert(large::count == 1);
        assert(Tp::count == 1);

        try_throw(lhs, rhs);

        assert(large::count == 1);
        assert(Tp::count == 1);
        assertContains<large>(lhs, 2);
        assertContains<Tp>(rhs, 1);
    }
#endif
}

int run_test() {
    test_copy_assign<small1, small2>();
    test_copy_assign<large1, large2>();
    test_copy_assign<small, large>();
    test_copy_assign<large, small>();
    test_copy_assign_empty<small>();
    test_copy_assign_empty<large>();
    test_copy_assign_self();
    test_copy_assign_throws<small_throws_on_copy>();
    test_copy_assign_throws<large_throws_on_copy>();

  return 0;
}
} // namespace assign::copy
// -- END: test/std/utilities/any/any.class/any.assign/copy.pass.cpp

// -- BEGIN: test/std/utilities/any/any.class/any.assign/move.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_any_cast && !libcpp-no-exceptions

// <any>

// any& operator=(any &&);

// Test move assignment.

#include <any>
#include <cassert>

#include "any_helpers.h"
#include "test_macros.h"

namespace assign::move {
using std::any;
using std::any_cast;

template <class LHS, class RHS>
void test_move_assign() {
    assert(LHS::count == 0);
    assert(RHS::count == 0);
    {
        LHS const s1(1);
        any a(s1);
        RHS const s2(2);
        any a2(s2);

        assert(LHS::count == 2);
        assert(RHS::count == 2);

        a = std::move(a2);

        assert(LHS::count == 1);
        assert(RHS::count == 2 + a2.has_value());
        LIBCPP_ASSERT(RHS::count == 2); // libc++ leaves the object empty

        assertContains<RHS>(a, 2);
        if (a2.has_value())
            assertContains<RHS>(a2, 0);
        LIBCPP_ASSERT(!a2.has_value());
    }
    assert(LHS::count == 0);
    assert(RHS::count == 0);
}

template <class LHS>
void test_move_assign_empty() {
    assert(LHS::count == 0);
    {
        any a;
        any a2((LHS(1)));

        assert(LHS::count == 1);

        a = std::move(a2);

        assert(LHS::count == 1 + a2.has_value());
        LIBCPP_ASSERT(LHS::count == 1);

        assertContains<LHS>(a, 1);
        if (a2.has_value())
            assertContains<LHS>(a2, 0);
        LIBCPP_ASSERT(!a2.has_value());
    }
    assert(LHS::count == 0);
    {
        any a((LHS(1)));
        any a2;

        assert(LHS::count == 1);

        a = std::move(a2);

        assert(LHS::count == 0);

        assertEmpty<LHS>(a);
        assertEmpty(a2);
    }
    assert(LHS::count == 0);
}

void test_move_assign_noexcept() {
    any a1;
    any a2;
    static_assert(
        noexcept(a1 = std::move(a2))
      , "any & operator=(any &&) must be noexcept"
      );
}

int run_test() {
    test_move_assign_noexcept();
    test_move_assign<small1, small2>();
    test_move_assign<large1, large2>();
    test_move_assign<small, large>();
    test_move_assign<large, small>();
    test_move_assign_empty<small>();
    test_move_assign_empty<large>();

  return 0;
}
} // namespace assign::move
// -- END: test/std/utilities/any/any.class/any.assign/move.pass.cpp

// -- BEGIN: test/std/utilities/any/any.class/any.assign/value.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_any_cast && !libcpp-no-exceptions

// <any>

// template <class ValueType>
// any& operator=(ValueType&&);

// Test value copy and move assignment.

#include <any>
#include <cassert>

#include "any_helpers.h"
#include "count_new.h"
#include "test_macros.h"

namespace assign::value {
using std::any;
using std::any_cast;

template <class LHS, class RHS>
void test_assign_value() {
    assert(LHS::count == 0);
    assert(RHS::count == 0);
    LHS::reset();
    RHS::reset();
    {
        any lhs(LHS(1));
        any const rhs(RHS(2));

        assert(LHS::count == 1);
        assert(RHS::count == 1);
        assert(RHS::copied == 0);

        lhs = rhs;

        assert(RHS::copied == 1);
        assert(LHS::count == 0);
        assert(RHS::count == 2);

        assertContains<RHS>(lhs, 2);
        assertContains<RHS>(rhs, 2);
    }
    assert(LHS::count == 0);
    assert(RHS::count == 0);
    LHS::reset();
    RHS::reset();
    {
        any lhs(LHS(1));
        any rhs(RHS(2));

        assert(LHS::count == 1);
        assert(RHS::count == 1);
        assert(RHS::moved == 1);

        lhs = std::move(rhs);

        assert(RHS::moved >= 1);
        assert(RHS::copied == 0);
        assert(LHS::count == 0);
        assert(RHS::count == 1 + rhs.has_value());
        LIBCPP_ASSERT(!rhs.has_value());

        assertContains<RHS>(lhs, 2);
        if (rhs.has_value())
            assertContains<RHS>(rhs, 0);
    }
    assert(LHS::count == 0);
    assert(RHS::count == 0);
}

template <class RHS>
void test_assign_value_empty() {
    assert(RHS::count == 0);
    RHS::reset();
    {
        any lhs;
        RHS rhs(42);
        assert(RHS::count == 1);
        assert(RHS::copied == 0);

        lhs = rhs;

        assert(RHS::count == 2);
        assert(RHS::copied == 1);
        assert(RHS::moved >= 0);
        assertContains<RHS>(lhs, 42);
    }
    assert(RHS::count == 0);
    RHS::reset();
    {
        any lhs;
        RHS rhs(42);
        assert(RHS::count == 1);
        assert(RHS::moved == 0);

        lhs = std::move(rhs);

        assert(RHS::count == 2);
        assert(RHS::copied == 0);
        assert(RHS::moved >= 1);
        assertContains<RHS>(lhs, 42);
    }
    assert(RHS::count == 0);
    RHS::reset();
}


template <class Tp, bool Move = false>
void test_assign_throws() {
#if !defined(TEST_HAS_NO_EXCEPTIONS)
    auto try_throw =
    [](any& lhs, Tp& rhs) {
        try {
            Move ? lhs = std::move(rhs)
                 : lhs = rhs;
            abort();
        } catch (my_any_exception const &) {
            // do nothing
        }
    };
    // const lvalue to empty
    {
        any lhs;
        Tp rhs(1);
        assert(Tp::count == 1);

        try_throw(lhs, rhs);

        assert(Tp::count == 1);
        assertEmpty<Tp>(lhs);
    }
    {
        any lhs((small(2)));
        Tp  rhs(1);
        assert(small::count == 1);
        assert(Tp::count == 1);

        try_throw(lhs, rhs);

        assert(small::count == 1);
        assert(Tp::count == 1);
        assertContains<small>(lhs, 2);
    }
    {
        any lhs((large(2)));
        Tp rhs(1);
        assert(large::count == 1);
        assert(Tp::count == 1);

        try_throw(lhs, rhs);

        assert(large::count == 1);
        assert(Tp::count == 1);
        assertContains<large>(lhs, 2);
    }
#endif
}


// Test that any& operator=(ValueType&&) is *never* selected for:
// * std::in_place type.
// * Non-copyable types
void test_sfinae_constraints() {
    { // Only the constructors are required to SFINAE on in_place_t
        using Tag = std::in_place_type_t<int>;
        using RawTag = std::remove_reference_t<Tag>;
        static_assert(std::is_assignable<std::any, RawTag&&>::value, "");
    }
    {
        struct Dummy { Dummy() = delete; };
        using T = std::in_place_type_t<Dummy>;
        static_assert(std::is_assignable<std::any, T>::value, "");
    }
    {
        // Test that the ValueType&& constructor SFINAE's away when the
        // argument is non-copyable
        struct NoCopy {
          NoCopy() = default;
          NoCopy(NoCopy const&) = delete;
          NoCopy(NoCopy&&) = default;
        };
        static_assert(!std::is_assignable<std::any, NoCopy>::value, "");
        static_assert(!std::is_assignable<std::any, NoCopy&>::value, "");
    }
}

int run_test() {
    test_assign_value<small1, small2>();
    test_assign_value<large1, large2>();
    test_assign_value<small, large>();
    test_assign_value<large, small>();
    test_assign_value_empty<small>();
    test_assign_value_empty<large>();
    test_assign_throws<small_throws_on_copy>();
    test_assign_throws<large_throws_on_copy>();
    test_assign_throws<throws_on_move, /* Move = */ true>();
    test_sfinae_constraints();

  return 0;
}
} // namespace assign::value
// -- END: test/std/utilities/any/any.class/any.assign/value.pass.cpp

// -- BEGIN: test/std/utilities/any/any.class/any.cons/copy.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_any_cast && !libcpp-no-exceptions

// <any>

// any(any const &);

#include <any>
#include <cassert>

#include "any_helpers.h"
#include "count_new.h"
#include "test_macros.h"

namespace ctor::copy {
using std::any;
using std::any_cast;

template <class Type>
void test_copy_throws() {
#if !defined(TEST_HAS_NO_EXCEPTIONS)
    assert(Type::count == 0);
    {
        any const a((Type(42)));
        assert(Type::count == 1);
        try {
            any const a2(a);
            abort();
        } catch (my_any_exception const &) {
            // do nothing
        }
        assert(Type::count == 1);
        assertContains<Type>(a, 42);
    }
    assert(Type::count == 0);
#endif
}

void test_copy_empty() {
    DisableAllocationGuard g; ((void)g); // No allocations should occur.
    any a1;
    any a2(a1);

    assertEmpty(a1);
    assertEmpty(a2);
}

template <class Type>
void test_copy()
{
    // Copying small types should not perform any allocations.
    DisableAllocationGuard g(isSmallType<Type>()); ((void)g);
    assert(Type::count == 0);
    Type::reset();
    {
        any a((Type(42)));
        assert(Type::count == 1);
        assert(Type::copied == 0);

        any a2(a);

        assert(Type::copied == 1);
        assert(Type::count == 2);
        assertContains<Type>(a, 42);
        assertContains<Type>(a2, 42);

        // Modify a and check that a2 is unchanged
        modifyValue<Type>(a, -1);
        assertContains<Type>(a, -1);
        assertContains<Type>(a2, 42);

        // modify a2 and check that a is unchanged
        modifyValue<Type>(a2, 999);
        assertContains<Type>(a, -1);
        assertContains<Type>(a2, 999);

        // clear a and check that a2 is unchanged
        a.reset();
        assertEmpty(a);
        assertContains<Type>(a2, 999);
    }
    assert(Type::count == 0);
}

int run_test() {
    test_copy<small>();
    test_copy<large>();
    test_copy_empty();
    test_copy_throws<small_throws_on_copy>();
    test_copy_throws<large_throws_on_copy>();

  return 0;
}
} // namespace ctor::copy
// -- END: test/std/utilities/any/any.class/any.cons/copy.pass.cpp

// -- BEGIN: test/std/utilities/any/any.class/any.cons/default.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <any>

// any() noexcept;

#include <any>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "any_helpers.h"
#include "count_new.h"

namespace ctor::default_ {
int run_test()
{
    using std::any;
    {
        static_assert(
            std::is_nothrow_default_constructible<any>::value
          , "Must be default constructible"
          );
    }
    {
        struct TestConstexpr : public std::any {
          constexpr TestConstexpr() : std::any() {}
        };
#ifdef _LIBCPP_SAFE_STATIC
        _LIBCPP_SAFE_STATIC static std::any a;
        ((void)a);
#endif
    }
    {
        DisableAllocationGuard g; ((void)g);
        any const a;
        assertEmpty(a);
    }

  return 0;
}
} // namespace ctor::default_
// -- END: test/std/utilities/any/any.class/any.cons/default.pass.cpp

// -- BEGIN: test/std/utilities/any/any.class/any.cons/in_place_type.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_any_cast && !libcpp-no-exceptions

// <any>

// template <class T, class ...Args> any(in_place_type_t<T>, Args&&...);
// template <class T, class U, class ...Args>
// any(in_place_type_t<T>, initializer_list<U>, Args&&...);

// Test construction from a value.
// Concerns:
// ---------
// 1. The value is properly move/copied depending on the value category.
// 2. Both small and large values are properly handled.


#include <any>
#include <cassert>

#include "any_helpers.h"
#include "count_new.h"
#include "test_macros.h"
#include "test_convertible.h"

namespace ctor::in_place {
using std::any;
using std::any_cast;

template <class Type>
void test_in_place_type() {
    // constructing from a small type should perform no allocations.
    DisableAllocationGuard g(isSmallType<Type>()); ((void)g);
    assert(Type::count == 0);
    Type::reset();
    {
        any a(std::in_place_type<Type>);

        assert(Type::count == 1);
        assert(Type::copied == 0);
        assert(Type::moved == 0);
        assertContains<Type>(a, 0);
    }
    assert(Type::count == 0);
    Type::reset();
    { // Test that the in_place argument is properly decayed
        any a(std::in_place_type<Type&>);

        assert(Type::count == 1);
        assert(Type::copied == 0);
        assert(Type::moved == 0);
        assertContains<Type>(a, 0);
    }
    assert(Type::count == 0);
    Type::reset();
    {
        any a(std::in_place_type<Type>, 101);

        assert(Type::count == 1);
        assert(Type::copied == 0);
        assert(Type::moved == 0);
        assertContains<Type>(a, 101);
    }
    assert(Type::count == 0);
    Type::reset();
    {
        any a(std::in_place_type<Type>, -1, 42, -1);

        assert(Type::count == 1);
        assert(Type::copied == 0);
        assert(Type::moved == 0);
        assertContains<Type>(a, 42);
    }
    assert(Type::count == 0);
    Type::reset();
}

template <class Type>
void test_in_place_type_tracked() {
    // constructing from a small type should perform no allocations.
    DisableAllocationGuard g(isSmallType<Type>()); ((void)g);
    {
        any a(std::in_place_type<Type>);
        assertArgsMatch<Type>(a);
    }
    {
        any a(std::in_place_type<Type>, -1, 42, -1);
        assertArgsMatch<Type, int, int, int>(a);
    }
    // initializer_list constructor tests
    {
        any a(std::in_place_type<Type>, {-1, 42, -1});
        assertArgsMatch<Type, std::initializer_list<int>>(a);
    }
    {
        int x = 42;
        any a(std::in_place_type<Type&>, {-1, 42, -1}, x);
        assertArgsMatch<Type, std::initializer_list<int>, int&>(a);
    }
}

void test_func() {}

void test_in_place_type_decayed() {
    {
        using Type = decltype(test_func);
        using DecayT = void(*)();
        any a(std::in_place_type<Type>, test_func);
        assert(containsType<DecayT>(a));
        assert(any_cast<DecayT>(a) == test_func);
    }
    {
        int my_arr[5];
        using Type = int(&)[5];
        using DecayT = int*;
        any a(std::in_place_type<Type>, my_arr);
        assert(containsType<DecayT>(a));
        assert(any_cast<DecayT>(a) == my_arr);
    }
    {
        using Type = int[5];
        using DecayT = int*;
        any a(std::in_place_type<Type>);
        assert(containsType<DecayT>(a));
        assert(any_cast<DecayT>(a) == nullptr);
    }
}

void test_ctor_sfinae() {
    {
        // Test that the init-list ctor SFINAE's away properly when
        // construction would be ill-formed.
        using IL = std::initializer_list<int>;
        static_assert(!std::is_constructible<std::any,
                      std::in_place_type_t<int>, IL>::value, "");
        static_assert(std::is_constructible<std::any,
            std::in_place_type_t<small_tracked_t>, IL>::value, "");
    }
    {
        // Test that the tagged dispatch constructor SFINAE's away when the
        // argument is non-copyable
        struct NoCopy {
          NoCopy() = default;
          NoCopy(NoCopy const&) = delete;
          NoCopy(int) {}
          NoCopy(std::initializer_list<int>, int) {}
        };
        using Tag = std::in_place_type_t<NoCopy>;
        using RefTag = std::in_place_type_t<NoCopy&>;
        using IL = std::initializer_list<int>;
        static_assert(!std::is_constructible<std::any, Tag>::value, "");
        static_assert(!std::is_constructible<std::any, Tag, int>::value, "");
        static_assert(!std::is_constructible<std::any, Tag, IL, int>::value, "");
        static_assert(!std::is_constructible<std::any, RefTag>::value, "");
        static_assert(!std::is_constructible<std::any, RefTag, int>::value, "");
        static_assert(!std::is_constructible<std::any, RefTag, IL, int>::value, "");
    }
}

struct Implicit {
  Implicit(int) {}
  Implicit(int, int, int) {}
  Implicit(std::initializer_list<int>, int) {}
};

void test_constructor_explicit() {
    using I = Implicit;
    using IT = std::in_place_type_t<I>;
    static_assert(!test_convertible<std::any, IT, int>(), "");
    static_assert(std::is_constructible<std::any, IT, int>::value, "");
    static_assert(!test_convertible<std::any, IT, int, int, int>(), "");
    static_assert(std::is_constructible<std::any, IT, int, int, int>::value, "");
    static_assert(!test_convertible<std::any, IT, std::initializer_list<int>&, int>(), "");
    static_assert(std::is_constructible<std::any, IT, std::initializer_list<int>&, int>::value, "");
}

int run_test() {
    test_in_place_type<small>();
    test_in_place_type<large>();
    test_in_place_type<small_throws_on_copy>();
    test_in_place_type<large_throws_on_copy>();
    test_in_place_type<throws_on_move>();
    test_in_place_type_tracked<small_tracked_t>();
    test_in_place_type_tracked<large_tracked_t>();
    test_in_place_type_decayed();
    test_ctor_sfinae();
    test_constructor_explicit();

  return 0;
}
} // namespace ctor::in_place
// -- END: test/std/utilities/any/any.class/any.cons/in_place_type.pass.cpp

// -- BEGIN: test/std/utilities/any/any.class/any.cons/move.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_any_cast && !libcpp-no-exceptions

// <any>

// any(any &&) noexcept;

#include <any>
#include <utility>
#include <type_traits>
#include <cassert>

#include "any_helpers.h"
#include "count_new.h"
#include "test_macros.h"

namespace ctor::move {
using std::any;
using std::any_cast;

// Moves are always noexcept. The throws_on_move object
// must be stored dynamically so the pointer is moved and
// not the stored object.
void test_move_does_not_throw() noexcept
{
#if !defined(TEST_HAS_NO_EXCEPTIONS)
    assert(throws_on_move::count == 0);
    {
        throws_on_move v(42);
        any a(v);
        assert(throws_on_move::count == 2);
        // No allocations should be performed after this point.
        DisableAllocationGuard g; ((void)g);
        {
            any const a2(std::move(a));
            assertEmpty(a);
            assertContains<throws_on_move>(a2, 42);
        }

        assert(throws_on_move::count == 1);
        assertEmpty(a);
    }
    assert(throws_on_move::count == 0);
#endif
}

void test_move_empty() {
    DisableAllocationGuard g; ((void)g); // no allocations should be performed.

    any a1;
    any a2(std::move(a1));

    assertEmpty(a1);
    assertEmpty(a2);
}

template <class Type>
void test_move() {
    assert(Type::count == 0);
    Type::reset();
    {
        any a((Type(42)));
        assert(Type::count == 1);
        assert(Type::copied == 0);
        assert(Type::moved == 1);

        // Moving should not perform allocations since it must be noexcept.
        DisableAllocationGuard g; ((void)g);

        any a2(std::move(a));

        assert(Type::moved == 1 || Type::moved == 2); // zero or more move operations can be performed.
        assert(Type::copied == 0); // no copies can be performed.
        assert(Type::count == 1 + a.has_value());
        assertContains<Type>(a2, 42);
        LIBCPP_ASSERT(!a.has_value()); // Moves are always destructive.
        if (a.has_value())
            assertContains<Type>(a, 0);
    }
    assert(Type::count == 0);
}

int run_test()
{
    // noexcept test
    {
        static_assert(
            std::is_nothrow_move_constructible<any>::value
          , "any must be nothrow move constructible"
          );
    }
    test_move<small>();
    test_move<large>();
    test_move_empty();
    test_move_does_not_throw();

  return 0;
}
} // namespace ctor::move
// -- END: test/std/utilities/any/any.class/any.cons/move.pass.cpp

// -- BEGIN: test/std/utilities/any/any.class/any.cons/value.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_any_cast && !libcpp-no-exceptions

// <any>

// template <class Value> any(Value &&)

// Test construction from a value.
// Concerns:
// ---------
// 1. The value is properly move/copied depending on the value category.
// 2. Both small and large values are properly handled.


#include <any>
#include <cassert>

#include "any_helpers.h"
#include "count_new.h"
#include "test_macros.h"

namespace ctor::value {
using std::any;
using std::any_cast;

template <class Type>
void test_copy_value_throws()
{
#if !defined(TEST_HAS_NO_EXCEPTIONS)
    assert(Type::count == 0);
    {
        Type const t(42);
        assert(Type::count == 1);
        try {
            any const a2(t);
            abort();
        } catch (my_any_exception const &) {
            // do nothing
        }

        assert(Type::count == 1);
        assert(t.value == 42);
    }
    assert(Type::count == 0);
#endif
}

void test_move_value_throws()
{
#if !defined(TEST_HAS_NO_EXCEPTIONS)
    assert(throws_on_move::count == 0);
    {
        throws_on_move v;
        assert(throws_on_move::count == 1);
        try {
            any const a(std::move(v));
            abort();
        } catch (my_any_exception const &) {
            // do nothing
        }
        assert(throws_on_move::count == 1);
    }
    assert(throws_on_move::count == 0);
#endif
}

template <class Type>
void test_copy_move_value() {
    // constructing from a small type should perform no allocations.
    DisableAllocationGuard g(isSmallType<Type>()); ((void)g);
    assert(Type::count == 0);
    Type::reset();
    {
        Type t(42);
        assert(Type::count == 1);

        any a(t);

        assert(Type::count == 2);
        assert(Type::copied == 1);
        assert(Type::moved == 0);
        assertContains<Type>(a, 42);
    }
    assert(Type::count == 0);
    Type::reset();
    {
        Type t(42);
        assert(Type::count == 1);

        any a(std::move(t));

        assert(Type::count == 2);
        assert(Type::copied == 0);
        assert(Type::moved == 1);
        assertContains<Type>(a, 42);
    }
}

// Test that any(ValueType&&) is *never* selected for a std::in_place_type_t specialization.
void test_sfinae_constraints() {
    using BadTag = std::in_place_type_t<int>;
    using OKTag = std::in_place_t;
    // Test that the tag type is properly handled in SFINAE
    BadTag t = std::in_place_type<int>;
    OKTag ot = std::in_place;
    {
        std::any a(t);
        assertContains<int>(a, 0);
    }
    {
        std::any a(std::move(t));
        assertContains<int>(a, 0);
    }
    {
        std::any a(ot);
        assert(containsType<OKTag>(a));
    }
    {
        struct Dummy { Dummy() = delete; };
        using T = std::in_place_type_t<Dummy>;
        static_assert(!std::is_constructible<std::any, T>::value, "");
    }
    {
        // Test that the ValueType&& constructor SFINAE's away when the
        // argument is non-copyable
        struct NoCopy {
          NoCopy() = default;
          NoCopy(NoCopy const&) = delete;
          NoCopy(int) {}
        };
        static_assert(!std::is_constructible<std::any, NoCopy>::value, "");
        static_assert(!std::is_constructible<std::any, NoCopy&>::value, "");
        static_assert(!std::is_convertible<NoCopy, std::any>::value, "");
    }
}

int run_test() {
    test_copy_move_value<small>();
    test_copy_move_value<large>();
    test_copy_value_throws<small_throws_on_copy>();
    test_copy_value_throws<large_throws_on_copy>();
    test_move_value_throws();
    test_sfinae_constraints();

  return 0;
}
} // namespace ctor::value
// -- END: test/std/utilities/any/any.class/any.cons/value.pass.cpp

// -- BEGIN: test/std/utilities/any/any.class/any.modifiers/emplace.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_any_cast && !libcpp-no-exceptions

// <any>

// template <class T, class ...Args> T& emplace(Args&&...);
// template <class T, class U, class ...Args>
// T& emplace(initializer_list<U>, Args&&...);

#include <any>
#include <cassert>

#include "any_helpers.h"
#include "count_new.h"
#include "test_macros.h"

namespace modifiers::emplace {
using std::any;
using std::any_cast;

struct Tracked {
  static int count;
  Tracked()  {++count;}
  ~Tracked() { --count; }
};
int Tracked::count = 0;

template <class Type>
void test_emplace_type() {
    // constructing from a small type should perform no allocations.
    DisableAllocationGuard g(isSmallType<Type>()); ((void)g);
    assert(Type::count == 0);
    Type::reset();
    {
        any a(std::in_place_type<Tracked>);
        assert(Tracked::count == 1);

        auto &v = a.emplace<Type>();
        static_assert( std::is_same_v<Type&, decltype(v)>, "" );
        assert(&v == std::any_cast<Type>(&a));

        assert(Tracked::count == 0);
        assert(Type::count == 1);
        assert(Type::copied == 0);
        assert(Type::moved == 0);
        assertContains<Type>(a, 0);
    }
    assert(Type::count == 0);
    Type::reset();
    {
        any a(std::in_place_type<Tracked>);
        assert(Tracked::count == 1);

        auto &v = a.emplace<Type>(101);
        static_assert( std::is_same_v<Type&, decltype(v)>, "" );
        assert(&v == std::any_cast<Type>(&a));

        assert(Tracked::count == 0);
        assert(Type::count == 1);
        assert(Type::copied == 0);
        assert(Type::moved == 0);
        assertContains<Type>(a, 101);
    }
    assert(Type::count == 0);
    Type::reset();
    {
        any a(std::in_place_type<Tracked>);
        assert(Tracked::count == 1);

        auto &v = a.emplace<Type>(-1, 42, -1);
        static_assert( std::is_same_v<Type&, decltype(v)>, "" );
        assert(&v == std::any_cast<Type>(&a));

        assert(Tracked::count == 0);
        assert(Type::count == 1);
        assert(Type::copied == 0);
        assert(Type::moved == 0);
        assertContains<Type>(a, 42);
    }
    assert(Type::count == 0);
    Type::reset();
}

template <class Type>
void test_emplace_type_tracked() {
    // constructing from a small type should perform no allocations.
    DisableAllocationGuard g(isSmallType<Type>()); ((void)g);
    {
        any a(std::in_place_type<Tracked>);
        assert(Tracked::count == 1);
        auto &v = a.emplace<Type>();
        static_assert( std::is_same_v<Type&, decltype(v)>, "" );
        assert(&v == std::any_cast<Type>(&a));

        assert(Tracked::count == 0);
        assertArgsMatch<Type>(a);
    }
    {
        any a(std::in_place_type<Tracked>);
        assert(Tracked::count == 1);
        auto &v = a.emplace<Type>(-1, 42, -1);
        static_assert( std::is_same_v<Type&, decltype(v)>, "" );
        assert(&v == std::any_cast<Type>(&a));

        assert(Tracked::count == 0);
        assertArgsMatch<Type, int, int, int>(a);
    }
    // initializer_list constructor tests
    {
        any a(std::in_place_type<Tracked>);
        assert(Tracked::count == 1);
        auto &v = a.emplace<Type>({-1, 42, -1});
        static_assert( std::is_same_v<Type&, decltype(v)>, "" );
        assert(&v == std::any_cast<Type>(&a));

        assert(Tracked::count == 0);
        assertArgsMatch<Type, std::initializer_list<int>>(a);
    }
    {
        int x = 42;
        any a(std::in_place_type<Tracked>);
        assert(Tracked::count == 1);
        auto &v = a.emplace<Type>({-1, 42, -1}, x);
        static_assert( std::is_same_v<Type&, decltype(v)>, "" );
        assert(&v == std::any_cast<Type>(&a));

        assert(Tracked::count == 0);
        assertArgsMatch<Type, std::initializer_list<int>, int&>(a);
    }
}

#ifndef TEST_HAS_NO_EXCEPTIONS

struct SmallThrows {
  SmallThrows(int) { throw 42; }
  SmallThrows(std::initializer_list<int>, int) { throw 42; }
};
static_assert(IsSmallObject<SmallThrows>::value, "");

struct LargeThrows {
  LargeThrows(int) { throw 42; }
  LargeThrows(std::initializer_list<int>, int) { throw 42; }
  int data[sizeof(std::any)];
};
static_assert(!IsSmallObject<LargeThrows>::value, "");

template <class Type>
void test_emplace_throws()
{
    // any stores small type
    {
        std::any a(small{42});
        assert(small::count == 1);
        try {
            auto &v = a.emplace<Type>(101);
            static_assert( std::is_same_v<Type&, decltype(v)>, "" );
            abort();
        } catch (int const&) {
        }
        assert(small::count == 0);
    }
    {
        std::any a(small{42});
        assert(small::count == 1);
        try {
            auto &v = a.emplace<Type>({1, 2, 3}, 101);
            static_assert( std::is_same_v<Type&, decltype(v)>, "" );
            abort();
        } catch (int const&) {
        }
        assert(small::count == 0);
    }
    // any stores large type
    {
        std::any a(large{42});
        assert(large::count == 1);
        try {
            auto &v = a.emplace<Type>(101);
            static_assert( std::is_same_v<Type&, decltype(v)>, "" );
            abort();
        } catch (int const&) {
        }
        assert(large::count == 0);
    }
    {
        std::any a(large{42});
        assert(large::count == 1);
        try {
            auto &v = a.emplace<Type>({1, 2, 3}, 101);
            static_assert( std::is_same_v<Type&, decltype(v)>, "" );
            abort();
        } catch (int const&) {
        }
        assert(large::count == 0);
    }
}

#endif

template <class T, class ...Args>
constexpr auto has_emplace(int)
    -> decltype(std::any{}.emplace<T>(std::declval<Args>()...), true) { return true; }

template <class ...Args>
constexpr bool has_emplace(long) { return false; }

template <class ...Args>
constexpr bool has_emplace() { return has_emplace<Args...>(0); }


template <class T, class IT, class ...Args>
constexpr auto has_emplace_init_list(int)
    -> decltype(std::any{}.emplace<T>(
        {std::declval<IT>(), std::declval<IT>(), std::declval<IT>()},
        std::declval<Args>()...), true) { return true; }

template <class ...Args>
constexpr bool has_emplace_init_list(long) { return false; }

template <class ...Args>
constexpr bool has_emplace_init_list() { return has_emplace_init_list<Args...>(0); }


void test_emplace_sfinae_constraints() {
    {
        static_assert(has_emplace<int>(), "");
        static_assert(has_emplace<int, int>(), "");
        static_assert(!has_emplace<int, int, int>(), "not constructible");
        static_assert(!has_emplace_init_list<int, int>(), "not constructible from il");
    }
    {
        static_assert(has_emplace<small>(), "");
        static_assert(has_emplace<large>(), "");
        static_assert(!has_emplace<small, void*>(), "");
        static_assert(!has_emplace<large, void*>(), "");

        static_assert(has_emplace_init_list<small, int>(), "");
        static_assert(has_emplace_init_list<large, int>(), "");
        static_assert(!has_emplace_init_list<small, void*>(), "");
        static_assert(!has_emplace_init_list<large, void*>(), "");
    }
    {
        // Test that the emplace SFINAE's away when the
        // argument is non-copyable
        struct NoCopy {
          NoCopy() = default;
          NoCopy(NoCopy const&) = delete;
          NoCopy(int) {}
          NoCopy(std::initializer_list<int>, int, int) {}
        };
        static_assert(!has_emplace<NoCopy>(), "");
        static_assert(!has_emplace<NoCopy, int>(), "");
        static_assert(!has_emplace_init_list<NoCopy, int, int, int>(), "");
        static_assert(!has_emplace<NoCopy&>(), "");
        static_assert(!has_emplace<NoCopy&, int>(), "");
        static_assert(!has_emplace_init_list<NoCopy&, int, int, int>(), "");
        static_assert(!has_emplace<NoCopy&&>(), "");
        static_assert(!has_emplace<NoCopy&&, int>(), "");
        static_assert(!has_emplace_init_list<NoCopy&&, int, int, int>(), "");

    }
}

int run_test() {
    test_emplace_type<small>();
    test_emplace_type<large>();
    test_emplace_type<small_throws_on_copy>();
    test_emplace_type<large_throws_on_copy>();
    test_emplace_type<throws_on_move>();
    test_emplace_type_tracked<small_tracked_t>();
    test_emplace_type_tracked<large_tracked_t>();
    test_emplace_sfinae_constraints();
#ifndef TEST_HAS_NO_EXCEPTIONS
    test_emplace_throws<SmallThrows>();
    test_emplace_throws<LargeThrows>();
#endif

  return 0;
}
} // namespace modifiers::emplace
// -- END: test/std/utilities/any/any.class/any.modifiers/emplace.pass.cpp

// -- BEGIN: test/std/utilities/any/any.class/any.modifiers/reset.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_any_cast && !libcpp-no-exceptions

// <any>

// any::reset() noexcept

#include <any>
#include <cassert>

#include "test_macros.h"
#include "any_helpers.h"

namespace modifiers::reset {
int run_test()
{
    using std::any;
    using std::any_cast;
    // empty
    {
        any a;

        // noexcept check
        static_assert(
            noexcept(a.reset())
          , "any.reset() must be noexcept"
          );

        assertEmpty(a);

        a.reset();

        assertEmpty(a);
    }
    // small object
    {
        any a((small(1)));
        assert(small::count == 1);
        assertContains<small>(a, 1);

        a.reset();

        assertEmpty<small>(a);
        assert(small::count == 0);
    }
    // large object
    {
        any a(large(1));
        assert(large::count == 1);
        assertContains<large>(a, 1);

        a.reset();

        assertEmpty<large>(a);
        assert(large::count == 0);
    }

  return 0;
}
} // namespace modifiers::reset
// -- END: test/std/utilities/any/any.class/any.modifiers/reset.pass.cpp

// -- BEGIN: test/std/utilities/any/any.class/any.modifiers/swap.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_any_cast && !libcpp-no-exceptions

// <any>

// any::swap(any &) noexcept

// Test swap(large, small) and swap(small, large)

#include <any>
#include <cassert>

#include "test_macros.h"
#include "any_helpers.h"

namespace modifiers::swap_ {
using std::any;
using std::any_cast;

template <class LHS, class RHS>
void test_swap() {
    assert(LHS::count == 0);
    assert(RHS::count == 0);
    {
        any a1((LHS(1)));
        any a2(RHS{2});
        assert(LHS::count == 1);
        assert(RHS::count == 1);

        a1.swap(a2);

        assert(LHS::count == 1);
        assert(RHS::count == 1);

        assertContains<RHS>(a1, 2);
        assertContains<LHS>(a2, 1);
    }
    assert(LHS::count == 0);
    assert(RHS::count == 0);
    assert(LHS::copied == 0);
    assert(RHS::copied == 0);
}

template <class Tp>
void test_swap_empty() {
    assert(Tp::count == 0);
    {
        any a1((Tp(1)));
        any a2;
        assert(Tp::count == 1);

        a1.swap(a2);

        assert(Tp::count == 1);

        assertContains<Tp>(a2, 1);
        assertEmpty(a1);
    }
    assert(Tp::count == 0);
    {
        any a1((Tp(1)));
        any a2;
        assert(Tp::count == 1);

        a2.swap(a1);

        assert(Tp::count == 1);

        assertContains<Tp>(a2, 1);
        assertEmpty(a1);
    }
    assert(Tp::count == 0);
    assert(Tp::copied == 0);
}

void test_noexcept()
{
    any a1;
    any a2;
    static_assert(
        noexcept(a1.swap(a2))
      , "any::swap(any&) must be noexcept"
      );
}

void test_self_swap() {
    {
        // empty
        any a;
        a.swap(a);
        assertEmpty(a);
    }
    { // small
        using T = small;
        any a{T{42}};
        T::reset();
        a.swap(a);
        assertContains<T>(a, 42);
        assert(T::count == 1);
        assert(T::copied == 0);
        LIBCPP_ASSERT(T::moved == 0);
    }
    assert(small::count == 0);
    { // large
        using T = large;
        any a{T{42}};
        T::reset();
        a.swap(a);
        assertContains<T>(a, 42);
        assert(T::count == 1);
        assert(T::copied == 0);
        LIBCPP_ASSERT(T::moved == 0);
    }
    assert(large::count == 0);
}

int run_test()
{
    test_noexcept();
    test_swap_empty<small>();
    test_swap_empty<large>();
    test_swap<small1, small2>();
    test_swap<large1, large2>();
    test_swap<small, large>();
    test_swap<large, small>();
    test_self_swap();

  return 0;
}
} // namespace modifiers::swap_
// -- END: test/std/utilities/any/any.class/any.modifiers/swap.pass.cpp

// -- BEGIN: test/std/utilities/any/any.class/any.observers/has_value.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <any>

// any::has_value() noexcept

#include <any>
#include <cassert>

#include "test_macros.h"
#include "any_helpers.h"

namespace observers::has_value {
int run_test()
{
    using std::any;
    // noexcept test
    {
        any a;
        static_assert(noexcept(a.has_value()), "any::has_value() must be noexcept");
    }
    // empty
    {
        any a;
        assert(!a.has_value());

        a.reset();
        assert(!a.has_value());

        a = 42;
        assert(a.has_value());
    }
    // small object
    {
        small const s(1);
        any a(s);
        assert(a.has_value());

        a.reset();
        assert(!a.has_value());

        a = s;
        assert(a.has_value());
    }
    // large object
    {
        large const l(1);
        any a(l);
        assert(a.has_value());

        a.reset();
        assert(!a.has_value());

        a = l;
        assert(a.has_value());
    }

  return 0;
}
} // namespace observers::has_value
// -- END: test/std/utilities/any/any.class/any.observers/has_value.pass.cpp

// -- BEGIN: test/std/utilities/any/any.class/any.observers/type.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: libcpp-no-rtti

// <any>

// any::type() noexcept

#include <any>
#include <cassert>

#include "test_macros.h"
#include "any_helpers.h"

namespace observers::type {
int run_test()
{
    using std::any;
    {
        any const a;
        assert(a.type() == typeid(void));
        ASSERT_NOEXCEPT(a.type());
    }
    {
        small const s(1);
        any const a(s);
        assert(a.type() == typeid(small));
    }
    {
        large const l(1);
        any const a(l);
        assert(a.type() == typeid(large));
    }
    {
        int arr[3];
        any const a(arr);
        assert(a.type() == typeid(int*));  // ensure that it is decayed
    }

  return 0;
}
} // namespace observers::type
// -- END: test/std/utilities/any/any.class/any.observers/type.pass.cpp

// -- BEGIN: test/std/utilities/any/any.class/not_literal_type.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <any>

// [Note any is a not a literal type --end note]

#include <any>
#include <type_traits>

#include "test_macros.h"

namespace not_literal {
int run_test() {
    static_assert(!std::is_literal_type<std::any>::value, "");

  return 0;
}
} // namespace not_literal
// -- END: test/std/utilities/any/any.class/not_literal_type.pass.cpp

// -- BEGIN: test/std/utilities/any/any.nonmembers/any.cast/any_cast_pointer.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_any_cast && !libcpp-no-exceptions

// <any>

// template <class ValueType>
// ValueType const* any_cast(any const *) noexcept;
//
// template <class ValueType>
// ValueType * any_cast(any *) noexcept;

#include <any>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "any_helpers.h"

namespace nonmembers::cast::pointer {
using std::any;
using std::any_cast;

// Test that the operators are properly noexcept.
void test_cast_is_noexcept() {
    any a;
    ASSERT_NOEXCEPT(any_cast<int>(&a));

    any const& ca = a;
    ASSERT_NOEXCEPT(any_cast<int>(&ca));
}

// Test that the return type of any_cast is correct.
void test_cast_return_type() {
    any a;
    ASSERT_SAME_TYPE(decltype(any_cast<int>(&a)),       int*);
    ASSERT_SAME_TYPE(decltype(any_cast<int const>(&a)), int const*);

    any const& ca = a;
    ASSERT_SAME_TYPE(decltype(any_cast<int>(&ca)),       int const*);
    ASSERT_SAME_TYPE(decltype(any_cast<int const>(&ca)), int const*);
}

// Test that any_cast handles null pointers.
void test_cast_nullptr() {
    any* a = nullptr;
    assert(nullptr == any_cast<int>(a));
    assert(nullptr == any_cast<int const>(a));

    any const* ca = nullptr;
    assert(nullptr == any_cast<int>(ca));
    assert(nullptr == any_cast<int const>(ca));
}

// Test casting an empty object.
void test_cast_empty() {
    {
        any a;
        assert(nullptr == any_cast<int>(&a));
        assert(nullptr == any_cast<int const>(&a));

        any const& ca = a;
        assert(nullptr == any_cast<int>(&ca));
        assert(nullptr == any_cast<int const>(&ca));
    }
    // Create as non-empty, then make empty and run test.
    {
        any a(42);
        a.reset();
        assert(nullptr == any_cast<int>(&a));
        assert(nullptr == any_cast<int const>(&a));

        any const& ca = a;
        assert(nullptr == any_cast<int>(&ca));
        assert(nullptr == any_cast<int const>(&ca));
    }
}

template <class Type>
void test_cast() {
    assert(Type::count == 0);
    Type::reset();
    {
        any a((Type(42)));
        any const& ca = a;
        assert(Type::count == 1);
        assert(Type::copied == 0);
        assert(Type::moved == 1);

        // Try a cast to a bad type.
        // NOTE: Type cannot be an int.
        assert(any_cast<int>(&a) == nullptr);
        assert(any_cast<int const>(&a) == nullptr);
        assert(any_cast<int const volatile>(&a) == nullptr);

        // Try a cast to the right type, but as a pointer.
        assert(any_cast<Type*>(&a) == nullptr);
        assert(any_cast<Type const*>(&a) == nullptr);

        // Check getting a unqualified type from a non-const any.
        Type* v = any_cast<Type>(&a);
        assert(v != nullptr);
        assert(v->value == 42);

        // change the stored value and later check for the new value.
        v->value = 999;

        // Check getting a const qualified type from a non-const any.
        Type const* cv = any_cast<Type const>(&a);
        assert(cv != nullptr);
        assert(cv == v);
        assert(cv->value == 999);

        // Check getting a unqualified type from a const any.
        cv = any_cast<Type>(&ca);
        assert(cv != nullptr);
        assert(cv == v);
        assert(cv->value == 999);

        // Check getting a const-qualified type from a const any.
        cv = any_cast<Type const>(&ca);
        assert(cv != nullptr);
        assert(cv == v);
        assert(cv->value == 999);

        // Check that no more objects were created, copied or moved.
        assert(Type::count == 1);
        assert(Type::copied == 0);
        assert(Type::moved == 1);
    }
    assert(Type::count == 0);
}

void test_cast_non_copyable_type()
{
    // Even though 'any' never stores non-copyable types
    // we still need to support any_cast<NoCopy>(ptr)
    struct NoCopy { NoCopy(NoCopy const&) = delete; };
    std::any a(42);
    std::any const& ca = a;
    assert(std::any_cast<NoCopy>(&a) == nullptr);
    assert(std::any_cast<NoCopy>(&ca) == nullptr);
}

void test_cast_array() {
    int arr[3];
    std::any a(arr);
    assert(a.type() == typeid(int*)); // contained value is decayed
//  We can't get an array out
    int (*p)[3] = std::any_cast<int[3]>(&a);
    assert(p == nullptr);
}

void test_fn() {}

void test_cast_function_pointer() {
    using T = void(*)();
    std::any a(test_fn);
    // An any can never store a function type, but we should at least be able
    // to ask.
    assert(std::any_cast<void()>(&a) == nullptr);
    T fn_ptr = std::any_cast<T>(a);
    assert(fn_ptr == test_fn);
}

int run_test() {
    test_cast_is_noexcept();
    test_cast_return_type();
    test_cast_nullptr();
    test_cast_empty();
    test_cast<small>();
    test_cast<large>();
    test_cast_non_copyable_type();
    test_cast_array();
    test_cast_function_pointer();

  return 0;
}
} // namespace nonmembers::cast::pointer
// -- END: test/std/utilities/any/any.nonmembers/any.cast/any_cast_pointer.pass.cpp

// -- BEGIN: test/std/utilities/any/any.nonmembers/any.cast/any_cast_reference.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_any_cast && !libcpp-no-exceptions

// <any>

// template <class ValueType>
// ValueType const any_cast(any const&);
//
// template <class ValueType>
// ValueType any_cast(any &);
//
// template <class ValueType>
// ValueType any_cast(any &&);

#include <any>
#include <type_traits>
#include <cassert>

#include "any_helpers.h"
#include "count_new.h"
#include "test_macros.h"

namespace nonmembers::cast::reference {
using std::any;
using std::any_cast;
using std::bad_any_cast;


// Test that the operators are NOT marked noexcept.
void test_cast_is_not_noexcept() {
    any a;
    static_assert(!noexcept(any_cast<int>(static_cast<any&>(a))), "");
    static_assert(!noexcept(any_cast<int>(static_cast<any const&>(a))), "");
    static_assert(!noexcept(any_cast<int>(static_cast<any &&>(a))), "");
}

// Test that the return type of any_cast is correct.
void test_cast_return_type() {
    any a;
    static_assert(std::is_same<decltype(any_cast<int>(a)), int>::value, "");
    static_assert(std::is_same<decltype(any_cast<int const>(a)), int>::value, "");
    static_assert(std::is_same<decltype(any_cast<int&>(a)), int&>::value, "");
    static_assert(std::is_same<decltype(any_cast<int const&>(a)), int const&>::value, "");

    static_assert(std::is_same<decltype(any_cast<int&&>(a)), int&&>::value, "");
    static_assert(std::is_same<decltype(any_cast<int const&&>(a)), int const&&>::value, "");

    static_assert(std::is_same<decltype(any_cast<int>(std::move(a))), int>::value, "");
    static_assert(std::is_same<decltype(any_cast<int const>(std::move(a))), int>::value, "");
    static_assert(std::is_same<decltype(any_cast<int&>(std::move(a))), int&>::value, "");
    static_assert(std::is_same<decltype(any_cast<int const&>(std::move(a))), int const&>::value, "");

    static_assert(std::is_same<decltype(any_cast<int&&>(std::move(a))), int&&>::value, "");
    static_assert(std::is_same<decltype(any_cast<int const&&>(std::move(a))), int const&&>::value, "");

    any const& ca = a;
    static_assert(std::is_same<decltype(any_cast<int>(ca)), int>::value, "");
    static_assert(std::is_same<decltype(any_cast<int const>(ca)), int>::value, "");
    static_assert(std::is_same<decltype(any_cast<int const&>(ca)), int const&>::value, "");

    static_assert(std::is_same<decltype(any_cast<int const&&>(ca)), int const&&>::value, "");
}

template <class Type, class ConstT = Type>
void checkThrows(any& a)
{
#if !defined(TEST_HAS_NO_EXCEPTIONS)
    try {
        TEST_IGNORE_NODISCARD any_cast<Type>(a);
        abort();
    } catch (bad_any_cast const &) {
            // do nothing
    }

    try {
        TEST_IGNORE_NODISCARD any_cast<ConstT>(static_cast<any const&>(a));
        abort();
    } catch (bad_any_cast const &) {
            // do nothing
    }

    try {
        using RefType = typename std::conditional<
            std::is_lvalue_reference<Type>::value,
            typename std::remove_reference<Type>::type&&,
            Type
        >::type;
        TEST_IGNORE_NODISCARD any_cast<RefType>(static_cast<any&&>(a));
        abort();
    } catch (bad_any_cast const &) {
            // do nothing
    }
#else
    (TEST_IGNORE_NODISCARD a);
#endif
}

void test_cast_empty() {
    // None of these operations should allocate.
    DisableAllocationGuard g; (TEST_IGNORE_NODISCARD g);
    any a;
    checkThrows<int>(a);
}

template <class Type>
void test_cast_to_reference() {
    assert(Type::count == 0);
    Type::reset();
    {
        any a((Type(42)));
        any const& ca = a;
        assert(Type::count == 1);
        assert(Type::copied == 0);
        assert(Type::moved == 1);

        // Try a cast to a bad type.
        // NOTE: Type cannot be an int.
        checkThrows<int>(a);
        checkThrows<int&, int const&>(a);
        checkThrows<Type*, Type const*>(a);
        checkThrows<Type const*>(a);

        // Check getting a type by reference from a non-const lvalue any.
        {
            Type& v = any_cast<Type&>(a);
            assert(v.value == 42);

            Type const &cv = any_cast<Type const&>(a);
            assert(&cv == &v);
        }
        // Check getting a type by reference from a const lvalue any.
        {
            Type const& v = any_cast<Type const&>(ca);
            assert(v.value == 42);

            Type const &cv = any_cast<Type const&>(ca);
            assert(&cv == &v);
        }
        // Check getting a type by reference from a const rvalue any.
        {
            Type const& v = any_cast<Type const&>(std::move(ca));
            assert(v.value == 42);

            Type const &cv = any_cast<Type const&>(std::move(ca));
            assert(&cv == &v);
        }
        // Check getting a type by reference from a const rvalue any.
        {
            Type&& v = any_cast<Type&&>(std::move(a));
            assert(v.value == 42);
            assert(any_cast<Type&>(a).value == 42);

            Type&& cv = any_cast<Type&&>(std::move(a));
            assert(&cv == &v);
            assert(any_cast<Type&>(a).value == 42);
        }
        // Check getting a type by reference from a const rvalue any.
        {
            Type const&& v = any_cast<Type const&&>(std::move(a));
            assert(v.value == 42);
            assert(any_cast<Type&>(a).value == 42);

            Type const&& cv = any_cast<Type const&&>(std::move(a));
            assert(&cv == &v);
            assert(any_cast<Type&>(a).value == 42);
        }
        // Check that the original object hasn't been changed.
        assertContains<Type>(a, 42);

        // Check that no objects have been created/copied/moved.
        assert(Type::count == 1);
        assert(Type::copied == 0);
        assert(Type::moved == 1);
    }
    assert(Type::count == 0);
}

template <class Type>
void test_cast_to_value() {
    assert(Type::count == 0);
    Type::reset();
    {
        any a((Type(42)));
        assert(Type::count == 1);
        assert(Type::copied == 0);
        assert(Type::moved == 1);

        // Try a cast to a bad type.
        // NOTE: Type cannot be an int.
        checkThrows<int>(a);
        checkThrows<int&, int const&>(a);
        checkThrows<Type*, Type const*>(a);
        checkThrows<Type const*>(a);

        Type::reset(); // NOTE: reset does not modify Type::count
        // Check getting Type by value from a non-const lvalue any.
        // This should cause the non-const copy constructor to be called.
        {
            Type t = any_cast<Type>(a);

            assert(Type::count == 2);
            assert(Type::copied == 1);
            assert(Type::const_copied == 0);
            assert(Type::non_const_copied == 1);
            assert(Type::moved == 0);
            assert(t.value == 42);
        }
        assert(Type::count == 1);
        Type::reset();
        // Check getting const Type by value from a non-const lvalue any.
        // This should cause the const copy constructor to be called.
        {
            Type t = any_cast<Type const>(a);

            assert(Type::count == 2);
            assert(Type::copied == 1);
            assert(Type::const_copied == 0);
            assert(Type::non_const_copied == 1);
            assert(Type::moved == 0);
            assert(t.value == 42);
        }
        assert(Type::count == 1);
        Type::reset();
        // Check getting Type by value from a non-const lvalue any.
        // This should cause the const copy constructor to be called.
        {
            Type t = any_cast<Type>(static_cast<any const&>(a));

            assert(Type::count == 2);
            assert(Type::copied == 1);
            assert(Type::const_copied == 1);
            assert(Type::non_const_copied == 0);
            assert(Type::moved == 0);
            assert(t.value == 42);
        }
        assert(Type::count == 1);
        Type::reset();
        // Check getting Type by value from a non-const rvalue any.
        // This should cause the non-const copy constructor to be called.
        {
            Type t = any_cast<Type>(static_cast<any &&>(a));

            assert(Type::count == 2);
            assert(Type::moved == 1);
            assert(Type::copied == 0);
            assert(Type::const_copied == 0);
            assert(Type::non_const_copied == 0);
            assert(t.value == 42);
            assert(any_cast<Type&>(a).value == 0);
            any_cast<Type&>(a).value = 42; // reset the value
        }
        assert(Type::count == 1);
        Type::reset();
        // Check getting const Type by value from a non-const rvalue any.
        // This should cause the const copy constructor to be called.
        {
            Type t = any_cast<Type const>(static_cast<any &&>(a));

            assert(Type::count == 2);
            assert(Type::copied == 0);
            assert(Type::const_copied == 0);
            assert(Type::non_const_copied == 0);
            assert(Type::moved == 1);
            assert(t.value == 42);
            assert(any_cast<Type&>(a).value == 0);
            any_cast<Type&>(a).value = 42; // reset the value
        }
        assert(Type::count == 1);
        Type::reset();
        // Check getting Type by value from a const rvalue any.
        // This should cause the const copy constructor to be called.
        {
            Type t = any_cast<Type>(static_cast<any const&&>(a));

            assert(Type::count == 2);
            assert(Type::copied == 1);
            assert(Type::const_copied == 1);
            assert(Type::non_const_copied == 0);
            assert(Type::moved == 0);
            assert(t.value == 42);
            assert(any_cast<Type&>(a).value == 42);
        }
        // Ensure we still only have 1 Type object alive.
        assert(Type::count == 1);

        // Check that the original object hasn't been changed.
        assertContains<Type>(a, 42);
    }
    assert(Type::count == 0);
}

int run_test() {
    test_cast_is_not_noexcept();
    test_cast_return_type();
    test_cast_empty();
    test_cast_to_reference<small>();
    test_cast_to_reference<large>();
    test_cast_to_value<small>();
    test_cast_to_value<large>();

  return 0;
}
} // namespace nonmembers::cast::reference
// -- END: test/std/utilities/any/any.nonmembers/any.cast/any_cast_reference.pass.cpp

// -- BEGIN: test/std/utilities/any/any.nonmembers/make_any.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_any_cast && !libcpp-no-exceptions

// <any>

// template <class T, class ...Args> any make_any(Args&&...);
// template <class T, class U, class ...Args>
// any make_any(initializer_list<U>, Args&&...);

#include <any>
#include <cassert>

#include "any_helpers.h"
#include "count_new.h"
#include "test_macros.h"

namespace nonmembers::make_any {
using std::any;
using std::any_cast;


template <class Type>
void test_make_any_type() {
    // constructing from a small type should perform no allocations.
    DisableAllocationGuard g(isSmallType<Type>()); ((void)g);
    assert(Type::count == 0);
    Type::reset();
    {
        any a = std::make_any<Type>();

        assert(Type::count == 1);
        assert(Type::copied == 0);
        assert(Type::moved == 0);
        assertContains<Type>(a, 0);
    }
    assert(Type::count == 0);
    Type::reset();
    {
        any a = std::make_any<Type>(101);

        assert(Type::count == 1);
        assert(Type::copied == 0);
        assert(Type::moved == 0);
        assertContains<Type>(a, 101);
    }
    assert(Type::count == 0);
    Type::reset();
    {
        any a = std::make_any<Type>(-1, 42, -1);

        assert(Type::count == 1);
        assert(Type::copied == 0);
        assert(Type::moved == 0);
        assertContains<Type>(a, 42);
    }
    assert(Type::count == 0);
    Type::reset();
}

template <class Type>
void test_make_any_type_tracked() {
    // constructing from a small type should perform no allocations.
    DisableAllocationGuard g(isSmallType<Type>()); ((void)g);
    {
        any a = std::make_any<Type>();
        assertArgsMatch<Type>(a);
    }
    {
        any a = std::make_any<Type>(-1, 42, -1);
        assertArgsMatch<Type, int, int, int>(a);
    }
    // initializer_list constructor tests
    {
        any a = std::make_any<Type>({-1, 42, -1});
        assertArgsMatch<Type, std::initializer_list<int>>(a);
    }
    {
        int x = 42;
        any a  = std::make_any<Type>({-1, 42, -1}, x);
        assertArgsMatch<Type, std::initializer_list<int>, int&>(a);
    }
}

#ifndef TEST_HAS_NO_EXCEPTIONS

struct SmallThrows {
  SmallThrows(int) { throw 42; }
  SmallThrows(std::initializer_list<int>, int) { throw 42; }
};
static_assert(IsSmallObject<SmallThrows>::value, "");

struct LargeThrows {
  LargeThrows(int) { throw 42; }
  LargeThrows(std::initializer_list<int>, int) { throw 42; }
  int data[sizeof(std::any)];
};
static_assert(!IsSmallObject<LargeThrows>::value, "");

template <class Type>
void test_make_any_throws()
{
    {
        try {
            TEST_IGNORE_NODISCARD std::make_any<Type>(101);
            abort();
        } catch (int const&) {
        }
    }
    {
        try {
            TEST_IGNORE_NODISCARD std::make_any<Type>({1, 2, 3}, 101);
            abort();
        } catch (int const&) {
        }
    }
}

#endif

int run_test() {
    test_make_any_type<small>();
    test_make_any_type<large>();
    test_make_any_type<small_throws_on_copy>();
    test_make_any_type<large_throws_on_copy>();
    test_make_any_type<throws_on_move>();
    test_make_any_type_tracked<small_tracked_t>();
    test_make_any_type_tracked<large_tracked_t>();
#ifndef TEST_HAS_NO_EXCEPTIONS
    test_make_any_throws<SmallThrows>();
    test_make_any_throws<LargeThrows>();

#endif

  return 0;
}
} // namespace nonmembers::make_any
// -- END: test/std/utilities/any/any.nonmembers/make_any.pass.cpp

// -- BEGIN: test/std/utilities/any/any.nonmembers/swap.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_any_cast && !libcpp-no-exceptions

// <any>

// void swap(any &, any &) noexcept

// swap(...) just wraps any::swap(...). That function is tested elsewhere.

#include <any>
#include <cassert>

#include "test_macros.h"
#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

namespace nonmembers::swap_ {
using std::any;
using std::any_cast;

int run_test()
{

    { // test noexcept
        any a;
        static_assert(noexcept(swap(a, a)), "swap(any&, any&) must be noexcept");
    }
    {
        any a1(1);
        any a2(2);

        swap(a1, a2);

        assert(any_cast<int>(a1) == 2);
        assert(any_cast<int>(a2) == 1);
    }

  return 0;
}
} // namespace nonmembers::swap_
// -- END: test/std/utilities/any/any.nonmembers/swap.pass.cpp
// LLVM SOURCES END
// clang-format on

#include <any>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>


namespace msvc {
    using std::any;
    using std::any_cast;
    using std::bad_any_cast;

    namespace ctor {
        namespace in_place {
            void run_test() {
                struct S {
                    S(std::initializer_list<int>&) {}
                };

                (void) std::make_any<S>({1, 2, 3});
            }
        } // namespace in_place

        namespace value {
            struct bad_new_type {
                char large[42 * sizeof(any)];

                static void* operator new(std::size_t); // undefined
                static void operator delete(void*, std::size_t); // undefined
            };

            void run_test() {
                any a{std::in_place_type<bad_new_type>};
            }
        } // namespace value
    } // namespace ctor

    namespace assign {
        namespace emplace {
            void run_test() {
                struct S {
                    S(std::initializer_list<int>&) {}
                };

                std::any{}.emplace<S>({1, 2, 3});
            }
        } // namespace emplace

        namespace move {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
#endif // __clang__
            void run_test() {
                // test that self-move-assignment doesn't fail
                {
                    // empty
                    any a;
                    a = std::move(a);
                }
                {
                    // small
                    any a{small{42}};
                    a = std::move(a);
                }
                {
                    // large
                    any a{large{42}};
                    a = std::move(a);
                }
                {
                    // trivial
                    any a{int{42}};
                    a = std::move(a);
                }
            }
#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__
        } // namespace move
    } // namespace assign

    namespace modifiers {
        namespace swap_ {
            void run_test() {
                // test trivial self-swap
                any a{42};
                swap(a, a);
                assertContains<int>(a, 42);
            }
        } // namespace swap_
    } // namespace modifiers

    namespace overaligned {
        template <std::size_t shift>
        void test_one_alignment() {
#ifndef _M_CEE // If alignas doesn't work, we don't need to support overaligned types.
            constexpr std::size_t align = __STDCPP_DEFAULT_NEW_ALIGNMENT__ << (1 + shift);

            struct aligned_type {
                alignas(align) unsigned char space[align];

                ~aligned_type() noexcept {
                    assert(reinterpret_cast<std::uintptr_t>(this) % align == 0);
                }
            };

            globalMemCounter.reset();

            constexpr std::size_t iterations = 256;

            std::vector<std::any> data(iterations);

            for (std::size_t i = 0; i < iterations; ++i) {
                data[i] = aligned_type{};
                assert(static_cast<std::size_t>(globalMemCounter.aligned_new_called) == i + 1);
                assert(globalMemCounter.last_new_size == sizeof(aligned_type));
                assert(globalMemCounter.last_new_align == alignof(aligned_type));
            }

            for (std::size_t i = 0; i < iterations; ++i) {
                data[i].reset();
                assert(static_cast<std::size_t>(globalMemCounter.aligned_delete_called) == i + 1);
                assert(globalMemCounter.last_delete_align == alignof(aligned_type));
            }
#endif // !_M_CEE
        }

        void run_test() {
            test_one_alignment<0>();
            test_one_alignment<1>();
            test_one_alignment<2>();
            test_one_alignment<3>();
        }
    } // namespace overaligned

    namespace size_and_alignment {
        void run_test() {
            static_assert(alignof(std::type_info) >= 4, "Can't steal two bits from type_info*");
            static_assert(
                sizeof(any) == std::_Small_object_num_ptrs * sizeof(void*), "any has unintended internal padding");
            static_assert(
                alignof(any) >= alignof(std::max_align_t), "any should be suitably aligned for any fundamental type");
            static_assert(std::_Any_is_small<std::string>, "any should hold a string without allocating");
        }
    } // namespace size_and_alignment

    namespace small_type {
        constexpr std::size_t BufferSize      = sizeof(std::any) - sizeof(void*);
        constexpr std::size_t BufferAlignment = alignof(std::any);

        [[nodiscard]] constexpr bool is_pow2(std::size_t n) noexcept {
            return n != 0 && (n & (n - 1)) == 0;
        }

        template <std::size_t Align>
        [[nodiscard]] constexpr std::size_t align_to(std::size_t n) noexcept {
            static_assert(is_pow2(Align), "Alignment must be a power of two.");
            return n & ~(Align - 1);
        }

        template <std::size_t Size, std::size_t Align>
        struct SizeAndAlignType {
            alignas(Align) unsigned char space[align_to<Align>(Size + Align - 1)];
        };

        template <class T>
        constexpr bool IsBig = !(std::_Any_is_small<T> || std::_Any_is_trivial<T>);

        void run_test() {
            STATIC_ASSERT(!IsBig<small>);
            STATIC_ASSERT(!IsBig<void*>);
            STATIC_ASSERT(IsBig<large>);
            {
                // Verify that a type that meets the size requirement *exactly* and has a lesser alignment requirement
                // is considered small.
                using T = SizeAndAlignType<BufferSize, 1>;
                STATIC_ASSERT(sizeof(T) == BufferSize);
                STATIC_ASSERT(alignof(T) < BufferAlignment);
                STATIC_ASSERT(!IsBig<T>);
            }
            {
                // Verify that a type that meets the alignment requirement *exactly* and has a lesser size is considered
                // small.
                using T = SizeAndAlignType<BufferAlignment, BufferAlignment>;
                STATIC_ASSERT(sizeof(T) <= BufferSize);
                STATIC_ASSERT(alignof(T) == BufferAlignment);
                STATIC_ASSERT(!IsBig<T>);
            }
            {
                // Verify that a type that meets the size and alignment requirements *exactly* is considered small.
                using T = SizeAndAlignType<align_to<BufferAlignment>(BufferSize), BufferAlignment>;
                STATIC_ASSERT(sizeof(T) <= BufferSize);
                STATIC_ASSERT(alignof(T) == BufferAlignment);
                STATIC_ASSERT(!IsBig<T>);
            }
            {
                // Verify that a type that meets the alignment requirements but is over-sized is not considered small.
                using T = SizeAndAlignType<BufferSize + 1, 1>;
                STATIC_ASSERT(sizeof(T) > BufferSize);
                STATIC_ASSERT(alignof(T) < BufferAlignment);
                STATIC_ASSERT(IsBig<T>);
            }
            {
                // Verify that a type that meets the size requirements but is over-aligned is not considered small.
                using T = SizeAndAlignType<BufferAlignment * 2, BufferAlignment * 2>;
                STATIC_ASSERT(alignof(T) >= BufferSize || sizeof(T) < BufferSize);
                STATIC_ASSERT(alignof(T) > BufferAlignment);
                STATIC_ASSERT(IsBig<T>);
            }
            {
                // Verify that a type that exceeds both the size and alignment requirements is not considered small.
                using T = SizeAndAlignType<BufferSize + 1, BufferAlignment * 2>;
                STATIC_ASSERT(sizeof(T) > BufferSize);
                STATIC_ASSERT(alignof(T) > BufferAlignment);
                STATIC_ASSERT(IsBig<T>);
            }
        }
    } // namespace small_type

    namespace trivial {
        template <class T>
        struct wrap {
            static_assert(
                IsSmallObject<T>::value && std::is_trivially_copyable_v<T>, "wrap instantiated with non-trivial type");

            T value{};

            constexpr wrap() = default;
            wrap(T t) : value(std::move(t)) {}
            template <class U, std::enable_if_t<std::is_convertible_v<U, T>, int> = 0>
            wrap(U&& u) : value(std::forward<U>(u)) {}

            friend bool operator==(const wrap& x, const wrap& y) {
                return x.value == y.value;
            }
            friend bool operator!=(const wrap& x, const wrap& y) {
                return !(x == y);
            }
        };

        template <class Type>
        void test_copy() {
            any a((Type(42)));

            any a2(a);

            assertContains<Type>(a, 42);
            assertContains<Type>(a2, 42);

            // Modify a and check that a2 is unchanged
            modifyValue<Type>(a, -1);
            assertContains<Type>(a, -1);
            assertContains<Type>(a2, 42);

            // modify a2 and check that a is unchanged
            modifyValue<Type>(a2, 999);
            assertContains<Type>(a, -1);
            assertContains<Type>(a2, 999);

            // clear a and check that a2 is unchanged
            a.reset();
            assertEmpty(a);
            assertContains<Type>(a2, 999);
        }

        template <class Type>
        void test_move() {
            any a((Type(42)));
            any a2(std::move(a));

            assertContains<Type>(a2, 42);
            assertContains<Type>(a, 42);
        }

        template <class Type>
        void test_copy_move_value() {
            {
                Type t(42);
                any a(t);
                assertContains<Type>(a, 42);
            }
            {
                Type t(42);
                any a(std::move(t));
                assertContains<Type>(a, 42);
            }
        }

        template <class Type>
        void test_in_place_type() {
            {
                any a(std::in_place_type<Type>);
                assertContains<Type>(a, 0);
            }
            {
                any a(std::in_place_type<Type>, 101);
                assertContains<Type>(a, 101);
            }
        }

        struct trivial_tracked_t {
            trivial_tracked_t() : arg_types(&makeArgumentID<>()) {}
            template <class... Args>
            explicit trivial_tracked_t(Args&&...) : arg_types(&makeArgumentID<Args...>()) {}
            template <class... Args>
            explicit trivial_tracked_t(std::initializer_list<int>, Args&&...)
                : arg_types(&makeArgumentID<std::initializer_list<int>, Args...>()) {}

            TypeID const* arg_types;
        };
        static_assert(IsSmallObject<trivial_tracked_t>::value, "must be small");
        static_assert(std::is_trivially_copyable_v<trivial_tracked_t>, "must be trivially copyable");

        template <class Type>
        void test_in_place_type_tracked() {
            {
                any a(std::in_place_type<Type>);
                assertArgsMatch<Type>(a);
            }
            {
                any a(std::in_place_type<Type>, -1, 42, -1);
                assertArgsMatch<Type, int, int, int>(a);
            }
            // initializer_list constructor tests
            {
                any a(std::in_place_type<Type>, {-1, 42, -1});
                assertArgsMatch<Type, std::initializer_list<int>>(a);
            }
            {
                int x = 42;
                any a(std::in_place_type<Type>, {-1, 42, -1}, x);
                assertArgsMatch<Type, std::initializer_list<int>, int&>(a);
            }
        }

        template <class LHS, class RHS>
        void test_copy_assign() {
            any lhs(LHS(1));
            any const rhs(RHS(2));

            lhs = rhs;

            assertContains<RHS>(lhs, 2);
            assertContains<RHS>(rhs, 2);
        }

        template <class LHS>
        void test_copy_assign_empty() {
            {
                any lhs;
                any const rhs(LHS(42));

                lhs = rhs;

                assertContains<LHS>(lhs, 42);
                assertContains<LHS>(rhs, 42);
            }
            {
                any lhs(LHS(1));
                any const rhs;

                lhs = rhs;

                assertEmpty<LHS>(lhs);
                assertEmpty(rhs);
            }
        }

        void test_copy_assign_self() {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif // __clang__

            // trivial
            any a(int(1));

            a = a;

            assertContains<int>(a, 1);

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__
        }

        template <class LHS, class RHS>
        void test_move_assign() {
            LHS const s1(1);
            any a(s1);
            RHS const s2(2);
            any a2(s2);

            a = std::move(a2);

            assertContains<RHS>(a, 2);
            assertContains<RHS>(a2, 2);
        }

        template <class LHS>
        void test_move_assign_empty() {
            {
                any a;
                any a2((LHS(1)));

                a = std::move(a2);

                assertContains<LHS>(a, 1);
                assertContains<LHS>(a2, 1);
            }
            {
                any a((LHS(1)));
                any a2;

                a = std::move(a2);

                assertEmpty<LHS>(a);
                assertEmpty(a2);
            }
        }

        template <class LHS, class RHS>
        void test_assign_value() {
            {
                any lhs(LHS(1));
                any const rhs(RHS(2));

                lhs = rhs;

                assertContains<RHS>(lhs, 2);
                assertContains<RHS>(rhs, 2);
            }
            {
                any lhs(LHS(1));
                any rhs(RHS(2));

                lhs = std::move(rhs);

                assertContains<RHS>(lhs, 2);
                assertContains<RHS>(rhs, 2);
            }
        }

        template <class RHS>
        void test_assign_value_empty() {
            {
                any lhs;
                RHS rhs(42);

                lhs = rhs;

                assertContains<RHS>(lhs, 42);
            }
            {
                any lhs;
                RHS rhs(42);

                lhs = std::move(rhs);

                assertContains<RHS>(lhs, 42);
            }
        }

        struct Tracked {
            static int count;
            Tracked() {
                ++count;
            }
            ~Tracked() {
                --count;
            }
        };
        int Tracked::count = 0;

        template <class Type>
        void test_emplace_type() {
            assert(Tracked::count == 0);
            {
                any a(std::in_place_type<Tracked>);
                assert(Tracked::count == 1);

                a.emplace<Type>();

                assert(Tracked::count == 0);
                assertContains<Type>(a, 0);
            }
            {
                any a(std::in_place_type<Tracked>);
                assert(Tracked::count == 1);

                a.emplace<Type>(101);

                assert(Tracked::count == 0);
                assertContains<Type>(a, 101);
            }
        }

        template <class Type>
        void test_emplace_type_tracked() {
            {
                any a(std::in_place_type<Tracked>);
                assert(Tracked::count == 1);
                a.emplace<Type>();
                assert(Tracked::count == 0);
                assertArgsMatch<Type>(a);
            }
            // initializer_list constructor tests
            {
                any a(std::in_place_type<Tracked>);
                assert(Tracked::count == 1);
                a.emplace<Type>({-1, 42, -1});
                assert(Tracked::count == 0);
                assertArgsMatch<Type, std::initializer_list<int>>(a);
            }
        }

        void test_reset() {
            any a((int(1)));
            assertContains<int>(a, 1);
            a.reset();
            assertEmpty<int>(a);
        }

        template <class LHS, class RHS>
        void test_swap() {
            any a1(LHS{1});
            any a2(RHS{2});

            a1.swap(a2);

            assertContains<RHS>(a1, 2);
            assertContains<LHS>(a2, 1);
        }

        template <class Tp>
        void test_swap_empty() {
            {
                any a1((Tp(1)));
                any a2;

                a1.swap(a2);

                assertContains<Tp>(a2, 1);
                assertEmpty(a1);
            }
            {
                any a1((Tp(1)));
                any a2;

                a2.swap(a1);

                assertContains<Tp>(a2, 1);
                assertEmpty(a1);
            }
        }

        void test_has_value() {
            int s(1);
            any a(s);
            assert(a.has_value());

            a.reset();
            assert(!a.has_value());

            a = s;
            assert(a.has_value());
        }

        template <class Tp>
        void test_type() {
            Tp const s(1);
            any const a(s);
            assert(a.type() == typeid(Tp));
        }

#pragma warning(push)
#pragma warning(disable : 4242 4244) // 'initializing': conversion from '_Ty' to 'T', possible loss of data
        void run_test() {
            globalMemCounter.disable_allocations = true;

            test_copy<wrap<int>>();
            test_copy<wrap<short>>();
            test_move<wrap<int>>();
            test_move<wrap<short>>();
            test_copy_move_value<wrap<int>>();
            test_copy_move_value<wrap<short>>();
            test_in_place_type<wrap<int>>();
            test_in_place_type<wrap<short>>();
            test_in_place_type_tracked<trivial_tracked_t>();

            test_copy_assign<wrap<int>, wrap<int>>();
            test_copy_assign<wrap<int>, wrap<short>>();
            test_copy_assign<wrap<short>, wrap<int>>();
            test_copy_assign_empty<wrap<int>>();
            test_copy_assign_empty<wrap<short>>();
            test_copy_assign_self();
            test_move_assign<wrap<int>, wrap<int>>();
            test_move_assign<wrap<int>, wrap<short>>();
            test_move_assign<wrap<short>, wrap<int>>();
            test_move_assign_empty<wrap<int>>();
            test_move_assign_empty<wrap<short>>();
            test_assign_value<wrap<int>, wrap<int>>();
            test_assign_value<wrap<int>, wrap<short>>();
            test_assign_value<wrap<short>, wrap<int>>();
            test_assign_value_empty<wrap<int>>();
            test_assign_value_empty<wrap<short>>();

            test_emplace_type<wrap<int>>();
            test_emplace_type<wrap<short>>();
            test_emplace_type_tracked<trivial_tracked_t>();
            test_reset();

            test_swap<wrap<int>, wrap<int>>();
            test_swap<wrap<int>, wrap<short>>();
            test_swap<wrap<short>, wrap<int>>();
            test_swap<wrap<int>, small>();
            test_swap<small, wrap<int>>();

            globalMemCounter.disable_allocations = false;
            test_swap<wrap<int>, large>();
            test_swap<large, wrap<int>>();
            globalMemCounter.disable_allocations = true;

            test_swap_empty<wrap<int>>();
            test_swap_empty<wrap<short>>();

            test_has_value();
            test_type<wrap<int>>();
            test_type<wrap<short>>();
        }
#pragma warning(pop)
    } // namespace trivial
} // namespace msvc

int main() {
    assign::copy::run_test();
    assign::move::run_test();
    assign::value::run_test();

    ctor::copy::run_test();
    ctor::default_::run_test();
    ctor::in_place::run_test();
    ctor::move::run_test();
    ctor::value::run_test();

    modifiers::emplace::run_test();
    modifiers::reset::run_test();
    modifiers::swap_::run_test();

    observers::has_value::run_test();
    observers::type::run_test();

    not_literal::run_test();

    nonmembers::cast::pointer::run_test();
    nonmembers::cast::reference::run_test();
    nonmembers::make_any::run_test();
    nonmembers::swap_::run_test();

    msvc::ctor::in_place::run_test();
    msvc::ctor::value::run_test();
    msvc::assign::emplace::run_test();
    msvc::assign::move::run_test();
    msvc::modifiers::swap_::run_test();
    msvc::overaligned::run_test();
    msvc::size_and_alignment::run_test();
    msvc::small_type::run_test();
    msvc::trivial::run_test();
}
