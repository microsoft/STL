// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Organization of this file:
// * a short header (including this comment)
// * `// LLVM SOURCES BEGIN`
// * The contents of several libc++ test files, each delimited by `// -- BEGIN/END: <filename>`
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
// 1. Navigate a bash prompt to `llvm-project/libcxx`.
// 2. Redirect the output of:
//      ../../tools/scripts/transform_llvm.sh test/std/utilities/variant
//    into a file.
// 3. Replicate the namespace structure from here into that file, use its content to replace everything between the
//    "LLVM SOURCES BEGIN"/"END" delimiters, and ensure that `main` properly calls each of the `run_test` functions.
// 4. Fix the specializations of std::hash and std::get by closing/reopening namespaces and qualifying types.
// 5. Restore the TRANSITION-commented workarounds.
// 6. Restore the _HAS_CXX20 guards for relops::three_way and visit::return_type.
// 7. Restore the guards for P2637R3 Member visit.
// 8. Restore the is_permissive workarounds.
//
// Yes, this is an awkward hand process; notably the required headers can change without notice. We should investigate
// running the libc++ tests directly in all of our configurations so we needn't replicate this subset of files.

#if !defined(_PREFAST_) || !defined(_M_IX86) // TRANSITION, VSO-1639191

#define _SILENCE_CXX20_VOLATILE_DEPRECATION_WARNING
#define _LIBCXX_IN_DEVCRT
#include <msvc_stdlib_force_include.h> // Must precede any other libc++ headers

#include <is_permissive.hpp>

// clang-format off: avoid diverging from external code
// LLVM SOURCES BEGIN
// -- BEGIN: test/std/utilities/variant/variant.bad_variant_access/bad_variant_access.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

/*

 class bad_variant_access : public exception {
public:
  bad_variant_access() noexcept;
  virtual const char* what() const noexcept;
};

*/

#include <cassert>
#include <exception>
#include <type_traits>
#include <variant>

#include "test_macros.h"

namespace bad_variant_access {
int run_test() {
  static_assert(std::is_base_of<std::exception, std::bad_variant_access>::value,
                "");
  static_assert(noexcept(std::bad_variant_access{}), "must be noexcept");
  static_assert(noexcept(std::bad_variant_access{}.what()), "must be noexcept");
  std::bad_variant_access ex;
  assert(ex.what());

  return 0;
}
} // namespace bad_variant_access
// -- END: test/std/utilities/variant/variant.bad_variant_access/bad_variant_access.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.get/get_if_index.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <size_t I, class... Types>
//  constexpr add_pointer_t<variant_alternative_t<I, variant<Types...>>>
//   get_if(variant<Types...>* v) noexcept;
// template <size_t I, class... Types>
//  constexpr add_pointer_t<const variant_alternative_t<I, variant<Types...>>>
//   get_if(const variant<Types...>* v) noexcept;

#include "test_macros.h"
#include "variant_test_helpers.h"
#include <cassert>
#include <memory>
#include <variant>

namespace get::get_if_index {
void test_const_get_if() {
  {
    using V = std::variant<int>;
    constexpr const V *v = nullptr;
    static_assert(std::get_if<0>(v) == nullptr, "");
  }
  {
    using V = std::variant<int, const long>;
    constexpr V v(42);
    ASSERT_NOEXCEPT(std::get_if<0>(&v));
    ASSERT_SAME_TYPE(decltype(std::get_if<0>(&v)), const int *);
    static_assert(*std::get_if<0>(&v) == 42, "");
    static_assert(std::get_if<1>(&v) == nullptr, "");
  }
  {
    using V = std::variant<int, const long>;
    constexpr V v(42l);
    ASSERT_SAME_TYPE(decltype(std::get_if<1>(&v)), const long *);
    static_assert(*std::get_if<1>(&v) == 42, "");
    static_assert(std::get_if<0>(&v) == nullptr, "");
  }
}

void test_get_if() {
  {
    using V = std::variant<int>;
    V *v = nullptr;
    assert(std::get_if<0>(v) == nullptr);
  }
  {
    using V = std::variant<int, long>;
    V v(42);
    ASSERT_NOEXCEPT(std::get_if<0>(&v));
    ASSERT_SAME_TYPE(decltype(std::get_if<0>(&v)), int *);
    assert(*std::get_if<0>(&v) == 42);
    assert(std::get_if<1>(&v) == nullptr);
  }
  {
    using V = std::variant<int, const long>;
    V v(42l);
    ASSERT_SAME_TYPE(decltype(std::get_if<1>(&v)), const long *);
    assert(*std::get_if<1>(&v) == 42);
    assert(std::get_if<0>(&v) == nullptr);
  }
}

int run_test() {
  test_const_get_if();
  test_get_if();

  return 0;
}
} // namespace get::get_if_index
// -- END: test/std/utilities/variant/variant.get/get_if_index.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.get/get_if_type.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

//  template <class T, class... Types>
//  constexpr add_pointer_t<T> get_if(variant<Types...>* v) noexcept;
// template <class T, class... Types>
//  constexpr add_pointer_t<const T> get_if(const variant<Types...>* v)
//  noexcept;

#include "test_macros.h"
#include "variant_test_helpers.h"
#include <cassert>
#include <variant>

namespace get::get_if_type {
void test_const_get_if() {
  {
    using V = std::variant<int>;
    constexpr const V *v = nullptr;
    static_assert(std::get_if<int>(v) == nullptr, "");
  }
  {
    using V = std::variant<int, const long>;
    constexpr V v(42);
    ASSERT_NOEXCEPT(std::get_if<int>(&v));
    ASSERT_SAME_TYPE(decltype(std::get_if<int>(&v)), const int *);
    static_assert(*std::get_if<int>(&v) == 42, "");
    static_assert(std::get_if<const long>(&v) == nullptr, "");
  }
  {
    using V = std::variant<int, const long>;
    constexpr V v(42l);
    ASSERT_SAME_TYPE(decltype(std::get_if<const long>(&v)), const long *);
    static_assert(*std::get_if<const long>(&v) == 42, "");
    static_assert(std::get_if<int>(&v) == nullptr, "");
  }
}

void test_get_if() {
  {
    using V = std::variant<int>;
    V *v = nullptr;
    assert(std::get_if<int>(v) == nullptr);
  }
  {
    using V = std::variant<int, const long>;
    V v(42);
    ASSERT_NOEXCEPT(std::get_if<int>(&v));
    ASSERT_SAME_TYPE(decltype(std::get_if<int>(&v)), int *);
    assert(*std::get_if<int>(&v) == 42);
    assert(std::get_if<const long>(&v) == nullptr);
  }
  {
    using V = std::variant<int, const long>;
    V v(42l);
    ASSERT_SAME_TYPE(decltype(std::get_if<const long>(&v)), const long *);
    assert(*std::get_if<const long>(&v) == 42);
    assert(std::get_if<int>(&v) == nullptr);
  }
}

int run_test() {
  test_const_get_if();
  test_get_if();

  return 0;
}
} // namespace get::get_if_type
// -- END: test/std/utilities/variant/variant.get/get_if_type.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.get/get_index.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <size_t I, class... Types>
//   constexpr variant_alternative_t<I, variant<Types...>>&
//   get(variant<Types...>& v);
// template <size_t I, class... Types>
//   constexpr variant_alternative_t<I, variant<Types...>>&&
//   get(variant<Types...>&& v);
// template <size_t I, class... Types>
//   constexpr variant_alternative_t<I, variant<Types...>> const& get(const
//   variant<Types...>& v);
// template <size_t I, class... Types>
//  constexpr variant_alternative_t<I, variant<Types...>> const&& get(const
//  variant<Types...>&& v);

#include "test_macros.h"
#include "test_workarounds.h"
#include "variant_test_helpers.h"
#include <cassert>
#include <type_traits>
#include <utility>
#include <variant>

namespace get::get_index {
void test_const_lvalue_get() {
  {
    using V = std::variant<int, const long>;
    constexpr V v(42);
    ASSERT_NOT_NOEXCEPT(std::get<0>(v));
    ASSERT_SAME_TYPE(decltype(std::get<0>(v)), const int &);
    static_assert(std::get<0>(v) == 42, "");
  }
  {
    using V = std::variant<int, const long>;
    const V v(42);
    ASSERT_NOT_NOEXCEPT(std::get<0>(v));
    ASSERT_SAME_TYPE(decltype(std::get<0>(v)), const int &);
    assert(std::get<0>(v) == 42);
  }
  {
    using V = std::variant<int, const long>;
    constexpr V v(42l);
    ASSERT_NOT_NOEXCEPT(std::get<1>(v));
    ASSERT_SAME_TYPE(decltype(std::get<1>(v)), const long &);
    static_assert(std::get<1>(v) == 42, "");
  }
  {
    using V = std::variant<int, const long>;
    const V v(42l);
    ASSERT_NOT_NOEXCEPT(std::get<1>(v));
    ASSERT_SAME_TYPE(decltype(std::get<1>(v)), const long &);
    assert(std::get<1>(v) == 42);
  }
}

void test_lvalue_get() {
  {
    using V = std::variant<int, const long>;
    V v(42);
    ASSERT_NOT_NOEXCEPT(std::get<0>(v));
    ASSERT_SAME_TYPE(decltype(std::get<0>(v)), int &);
    assert(std::get<0>(v) == 42);
  }
  {
    using V = std::variant<int, const long>;
    V v(42l);
    ASSERT_SAME_TYPE(decltype(std::get<1>(v)), const long &);
    assert(std::get<1>(v) == 42);
  }
}

void test_rvalue_get() {
  {
    using V = std::variant<int, const long>;
    V v(42);
    ASSERT_NOT_NOEXCEPT(std::get<0>(std::move(v)));
    ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), int &&);
    assert(std::get<0>(std::move(v)) == 42);
  }
  {
    using V = std::variant<int, const long>;
    V v(42l);
    ASSERT_SAME_TYPE(decltype(std::get<1>(std::move(v))), const long &&);
    assert(std::get<1>(std::move(v)) == 42);
  }
}

void test_const_rvalue_get() {
  {
    using V = std::variant<int, const long>;
    const V v(42);
    ASSERT_NOT_NOEXCEPT(std::get<0>(std::move(v)));
    ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), const int &&);
    assert(std::get<0>(std::move(v)) == 42);
  }
  {
    using V = std::variant<int, const long>;
    const V v(42l);
    ASSERT_SAME_TYPE(decltype(std::get<1>(std::move(v))), const long &&);
    assert(std::get<1>(std::move(v)) == 42);
  }
}

template <std::size_t I> using Idx = std::integral_constant<std::size_t, I>;

void test_throws_for_all_value_categories() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  using V = std::variant<int, long>;
  V v0(42);
  const V &cv0 = v0;
  assert(v0.index() == 0);
  V v1(42l);
  const V &cv1 = v1;
  assert(v1.index() == 1);
  std::integral_constant<std::size_t, 0> zero;
  std::integral_constant<std::size_t, 1> one;
  auto test = [](auto idx, auto &&v) {
    using Idx = decltype(idx);
    try {
      TEST_IGNORE_NODISCARD std::get<Idx::value>(std::forward<decltype(v)>(v));
    } catch (const std::bad_variant_access &) {
      return true;
    } catch (...) { /* ... */
    }
    return false;
  };
  { // lvalue test cases
    assert(test(one, v0));
    assert(test(zero, v1));
  }
  { // const lvalue test cases
    assert(test(one, cv0));
    assert(test(zero, cv1));
  }
  { // rvalue test cases
    assert(test(one, std::move(v0)));
    assert(test(zero, std::move(v1)));
  }
  { // const rvalue test cases
    assert(test(one, std::move(cv0)));
    assert(test(zero, std::move(cv1)));
  }
#endif
}

int run_test() {
  test_const_lvalue_get();
  test_lvalue_get();
  test_rvalue_get();
  test_const_rvalue_get();
  test_throws_for_all_value_categories();

  return 0;
}
} // namespace get::get_index
// -- END: test/std/utilities/variant/variant.get/get_index.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.get/get_type.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class T, class... Types> constexpr T& get(variant<Types...>& v);
// template <class T, class... Types> constexpr T&& get(variant<Types...>&& v);
// template <class T, class... Types> constexpr const T& get(const
// variant<Types...>& v);
// template <class T, class... Types> constexpr const T&& get(const
// variant<Types...>&& v);

#include "test_macros.h"
#include "test_workarounds.h"
#include "variant_test_helpers.h"
#include <cassert>
#include <type_traits>
#include <utility>
#include <variant>

namespace get::get_type {
void test_const_lvalue_get() {
  {
    using V = std::variant<int, const long>;
    constexpr V v(42);
    ASSERT_NOT_NOEXCEPT(std::get<int>(v));
    ASSERT_SAME_TYPE(decltype(std::get<int>(v)), const int &);
    static_assert(std::get<int>(v) == 42, "");
  }
  {
    using V = std::variant<int, const long>;
    const V v(42);
    ASSERT_NOT_NOEXCEPT(std::get<int>(v));
    ASSERT_SAME_TYPE(decltype(std::get<int>(v)), const int &);
    assert(std::get<int>(v) == 42);
  }
  {
    using V = std::variant<int, const long>;
    constexpr V v(42l);
    ASSERT_NOT_NOEXCEPT(std::get<const long>(v));
    ASSERT_SAME_TYPE(decltype(std::get<const long>(v)), const long &);
    static_assert(std::get<const long>(v) == 42, "");
  }
  {
    using V = std::variant<int, const long>;
    const V v(42l);
    ASSERT_NOT_NOEXCEPT(std::get<const long>(v));
    ASSERT_SAME_TYPE(decltype(std::get<const long>(v)), const long &);
    assert(std::get<const long>(v) == 42);
  }
}

void test_lvalue_get() {
  {
    using V = std::variant<int, const long>;
    V v(42);
    ASSERT_NOT_NOEXCEPT(std::get<int>(v));
    ASSERT_SAME_TYPE(decltype(std::get<int>(v)), int &);
    assert(std::get<int>(v) == 42);
  }
  {
    using V = std::variant<int, const long>;
    V v(42l);
    ASSERT_SAME_TYPE(decltype(std::get<const long>(v)), const long &);
    assert(std::get<const long>(v) == 42);
  }
}

void test_rvalue_get() {
  {
    using V = std::variant<int, const long>;
    V v(42);
    ASSERT_NOT_NOEXCEPT(std::get<int>(std::move(v)));
    ASSERT_SAME_TYPE(decltype(std::get<int>(std::move(v))), int &&);
    assert(std::get<int>(std::move(v)) == 42);
  }
  {
    using V = std::variant<int, const long>;
    V v(42l);
    ASSERT_SAME_TYPE(decltype(std::get<const long>(std::move(v))),
                     const long &&);
    assert(std::get<const long>(std::move(v)) == 42);
  }
}

void test_const_rvalue_get() {
  {
    using V = std::variant<int, const long>;
    const V v(42);
    ASSERT_NOT_NOEXCEPT(std::get<int>(std::move(v)));
    ASSERT_SAME_TYPE(decltype(std::get<int>(std::move(v))), const int &&);
    assert(std::get<int>(std::move(v)) == 42);
  }
  {
    using V = std::variant<int, const long>;
    const V v(42l);
    ASSERT_SAME_TYPE(decltype(std::get<const long>(std::move(v))),
                     const long &&);
    assert(std::get<const long>(std::move(v)) == 42);
  }
}

template <class Tp> struct identity { using type = Tp; };

void test_throws_for_all_value_categories() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  using V = std::variant<int, long>;
  V v0(42);
  const V &cv0 = v0;
  assert(v0.index() == 0);
  V v1(42l);
  const V &cv1 = v1;
  assert(v1.index() == 1);
  identity<int> zero;
  identity<long> one;
  auto test = [](auto idx, auto &&v) {
    using Idx = decltype(idx);
    try {
      TEST_IGNORE_NODISCARD std::get<typename Idx::type>(std::forward<decltype(v)>(v));
    } catch (const std::bad_variant_access &) {
      return true;
    } catch (...) { /* ... */
    }
    return false;
  };
  { // lvalue test cases
    assert(test(one, v0));
    assert(test(zero, v1));
  }
  { // const lvalue test cases
    assert(test(one, cv0));
    assert(test(zero, cv1));
  }
  { // rvalue test cases
    assert(test(one, std::move(v0)));
    assert(test(zero, std::move(v1)));
  }
  { // const rvalue test cases
    assert(test(one, std::move(cv0)));
    assert(test(zero, std::move(cv1)));
  }
#endif
}

int run_test() {
  test_const_lvalue_get();
  test_lvalue_get();
  test_rvalue_get();
  test_const_rvalue_get();
  test_throws_for_all_value_categories();

  return 0;
}
} // namespace get::get_type
// -- END: test/std/utilities/variant/variant.get/get_type.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.get/holds_alternative.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class T, class... Types>
// constexpr bool holds_alternative(const variant<Types...>& v) noexcept;

#include "test_macros.h"
#include <variant>

namespace get::holds_alternative {
int run_test() {
  {
    using V = std::variant<int>;
    constexpr V v;
    static_assert(std::holds_alternative<int>(v), "");
  }
  {
    using V = std::variant<int, long>;
    constexpr V v;
    static_assert(std::holds_alternative<int>(v), "");
    static_assert(!std::holds_alternative<long>(v), "");
  }
  { // noexcept test
    using V = std::variant<int>;
    const V v;
    ASSERT_NOEXCEPT(std::holds_alternative<int>(v));
  }

  return 0;
}
} // namespace get::holds_alternative
// -- END: test/std/utilities/variant/variant.get/holds_alternative.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.hash/enabled_hash.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// Test that <variant> provides all of the arithmetic, enum, and pointer
// hash specializations.

#include <variant>

#include "poisoned_hash_helper.h"

#include "test_macros.h"

namespace hash::enabled_hash {
int run_test() {
  test_library_hash_specializations_available();

  return 0;
}
} // namespace hash::enabled_hash
// -- END: test/std/utilities/variant/variant.hash/enabled_hash.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.hash/hash.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class... Types> struct hash<variant<Types...>>;
// template <> struct hash<monostate>;

#include <cassert>
#include <type_traits>
#include <variant>

#include "test_macros.h"
#include "variant_test_helpers.h"
#include "poisoned_hash_helper.h"

#ifndef TEST_HAS_NO_EXCEPTIONS
namespace std {
template <> struct hash<::MakeEmptyT> {
  std::size_t operator()(const ::MakeEmptyT &) const {
    assert(false);
    return 0;
  }
};
}
#endif

namespace hash {
void test_hash_variant() {
  {
    using V = std::variant<int, long, int>;
    using H = std::hash<V>;
    const V v(std::in_place_index<0>, 42);
    const V v_copy = v;
    V v2(std::in_place_index<0>, 100);
    const H h{};
    assert(h(v) == h(v));
    assert(h(v) != h(v2));
    assert(h(v) == h(v_copy));
    {
      ASSERT_SAME_TYPE(decltype(h(v)), std::size_t);
      static_assert(std::is_copy_constructible<H>::value, "");
    }
  }
  {
    using V = std::variant<std::monostate, int, long, const char *>;
    using H = std::hash<V>;
    const char *str = "hello";
    const V v0;
    const V v0_other;
    const V v1(42);
    const V v1_other(100);
    V v2(100l);
    V v2_other(999l);
    V v3(str);
    V v3_other("not hello");
    const H h{};
    assert(h(v0) == h(v0));
    assert(h(v0) == h(v0_other));
    assert(h(v1) == h(v1));
    assert(h(v1) != h(v1_other));
    assert(h(v2) == h(v2));
    assert(h(v2) != h(v2_other));
    assert(h(v3) == h(v3));
    assert(h(v3) != h(v3_other));
    assert(h(v0) != h(v1));
    assert(h(v0) != h(v2));
    assert(h(v0) != h(v3));
    assert(h(v1) != h(v2));
    assert(h(v1) != h(v3));
    assert(h(v2) != h(v3));
  }
#ifndef TEST_HAS_NO_EXCEPTIONS
  {
    using V = std::variant<int, MakeEmptyT>;
    using H = std::hash<V>;
    V v;
    makeEmpty(v);
    V v2;
    makeEmpty(v2);
    const H h{};
    assert(h(v) == h(v2));
  }
#endif
}

void test_hash_monostate() {
  using H = std::hash<std::monostate>;
  const H h{};
  std::monostate m1{};
  const std::monostate m2{};
  assert(h(m1) == h(m1));
  assert(h(m2) == h(m2));
  assert(h(m1) == h(m2));
  {
    ASSERT_SAME_TYPE(decltype(h(m1)), std::size_t);
    ASSERT_NOEXCEPT(h(m1));
    static_assert(std::is_copy_constructible<H>::value, "");
  }
  {
    test_hash_enabled<std::monostate>();
  }
}

void test_hash_variant_duplicate_elements() {
    // Test that the index of the alternative participates in the hash value.
    using V = std::variant<std::monostate, std::monostate>;
    using H = std::hash<V>;
    H h{};
    const V v1(std::in_place_index<0>);
    const V v2(std::in_place_index<1>);
    assert(h(v1) == h(v1));
    assert(h(v2) == h(v2));
    LIBCPP_ASSERT(h(v1) != h(v2));
}

struct A {};
struct B {};
} // namespace hash

namespace std {

template <>
struct hash<::hash::B> {
  std::size_t operator()(::hash::B const&) const {
    return 0;
  }
};

}

namespace hash {
void test_hash_variant_enabled() {
  {
    test_hash_enabled<std::variant<int> >();
    test_hash_enabled<std::variant<int*, long, double, const int> >();
  }
  {
    test_hash_disabled<std::variant<int, A>>();
    test_hash_disabled<std::variant<const A, void*>>();
  }
  {
    test_hash_enabled<std::variant<int, B>>();
    test_hash_enabled<std::variant<const B, int>>();
  }
}

int run_test() {
  test_hash_variant();
  test_hash_variant_duplicate_elements();
  test_hash_monostate();
  test_hash_variant_enabled();

  return 0;
}
} // namespace hash
// -- END: test/std/utilities/variant/variant.hash/hash.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.helpers/variant_alternative.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <size_t I, class T> struct variant_alternative; // undefined
// template <size_t I, class T> struct variant_alternative<I, const T>;
// template <size_t I, class T> struct variant_alternative<I, volatile T>;
// template <size_t I, class T> struct variant_alternative<I, const volatile T>;
// template <size_t I, class T>
//   using variant_alternative_t = typename variant_alternative<I, T>::type;
//
// template <size_t I, class... Types>
//    struct variant_alternative<I, variant<Types...>>;

#include <memory>
#include <type_traits>
#include <variant>

#include "test_macros.h"
#include "variant_test_helpers.h"

namespace helpers::variant_alternative {
template <class V, std::size_t I, class E> void test() {
  static_assert(
      std::is_same_v<typename std::variant_alternative<I, V>::type, E>, "");
  static_assert(
      std::is_same_v<typename std::variant_alternative<I, const V>::type,
                     const E>,
      "");
  static_assert(
      std::is_same_v<typename std::variant_alternative<I, volatile V>::type,
                     volatile E>,
      "");
  static_assert(
      std::is_same_v<
          typename std::variant_alternative<I, const volatile V>::type,
          const volatile E>,
      "");
  static_assert(std::is_same_v<std::variant_alternative_t<I, V>, E>, "");
  static_assert(std::is_same_v<std::variant_alternative_t<I, const V>, const E>,
                "");
  static_assert(
      std::is_same_v<std::variant_alternative_t<I, volatile V>, volatile E>,
      "");
  static_assert(std::is_same_v<std::variant_alternative_t<I, const volatile V>,
                               const volatile E>,
                "");
}

int run_test() {
  {
    using V = std::variant<int, void *, const void *, long double>;
    test<V, 0, int>();
    test<V, 1, void *>();
    test<V, 2, const void *>();
    test<V, 3, long double>();
  }

  return 0;
}
} // namespace helpers::variant_alternative
// -- END: test/std/utilities/variant/variant.helpers/variant_alternative.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.helpers/variant_size.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class T> struct variant_size; // undefined
// template <class T> struct variant_size<const T>;
// template <class T> struct variant_size<volatile T>;
// template <class T> struct variant_size<const volatile T>;
// template <class T> constexpr size_t variant_size_v
//     = variant_size<T>::value;

#include <memory>
#include <type_traits>
#include <variant>

#include "test_macros.h"

namespace helpers::variant_size {
template <class V, std::size_t E> void test() {
  static_assert(std::variant_size<V>::value == E, "");
  static_assert(std::variant_size<const V>::value == E, "");
  static_assert(std::variant_size<volatile V>::value == E, "");
  static_assert(std::variant_size<const volatile V>::value == E, "");
  static_assert(std::variant_size_v<V> == E, "");
  static_assert(std::variant_size_v<const V> == E, "");
  static_assert(std::variant_size_v<volatile V> == E, "");
  static_assert(std::variant_size_v<const volatile V> == E, "");
  static_assert(std::is_base_of<std::integral_constant<std::size_t, E>,
                                std::variant_size<V>>::value,
                "");
};

int run_test() {
  test<std::variant<>, 0>();
  test<std::variant<void *>, 1>();
  test<std::variant<long, long, void *, double>, 4>();

  return 0;
}
} // namespace helpers::variant_size
// -- END: test/std/utilities/variant/variant.helpers/variant_size.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.monostate/monostate.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// struct monostate {};

#include <type_traits>
#include <variant>

#include "test_macros.h"

namespace monostate::properties {
int run_test() {
  using M = std::monostate;
  static_assert(std::is_trivially_default_constructible<M>::value, "");
  static_assert(std::is_trivially_copy_constructible<M>::value, "");
  static_assert(std::is_trivially_copy_assignable<M>::value, "");
  static_assert(std::is_trivially_destructible<M>::value, "");
  constexpr M m{};
  ((void)m);

  return 0;
}
} // namespace monostate::properties
// -- END: test/std/utilities/variant/variant.monostate/monostate.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.monostate.relops/relops.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// constexpr bool operator<(monostate, monostate) noexcept { return false; }
// constexpr bool operator>(monostate, monostate) noexcept { return false; }
// constexpr bool operator<=(monostate, monostate) noexcept { return true; }
// constexpr bool operator>=(monostate, monostate) noexcept { return true; }
// constexpr bool operator==(monostate, monostate) noexcept { return true; }
// constexpr bool operator!=(monostate, monostate) noexcept { return false; }
// constexpr strong_ordering operator<=>(monostate, monostate) noexcept { return strong_ordering::equal; } // C++20

#include <cassert>
#include <variant>

#include "test_comparisons.h"
#include "test_macros.h"

namespace monostate::relops {
constexpr bool test() {
  using M = std::monostate;
  constexpr M m1{};
  constexpr M m2{};
  assert(testComparisons(m1, m2, /*isEqual*/ true, /*isLess*/ false));
  AssertComparisonsAreNoexcept<M>();

#if TEST_STD_VER > 17
  assert(testOrder(m1, m2, std::strong_ordering::equal));
  AssertOrderAreNoexcept<M>();
#endif // TEST_STD_VER > 17

  return true;
}

int run_test() {
  test();
  static_assert(test());

  return 0;
}
} // namespace monostate::relops
// -- END: test/std/utilities/variant/variant.monostate.relops/relops.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.relops/relops.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types>
// constexpr bool
// operator==(variant<Types...> const&, variant<Types...> const&) noexcept;
//
// template <class ...Types>
// constexpr bool
// operator!=(variant<Types...> const&, variant<Types...> const&) noexcept;
//
// template <class ...Types>
// constexpr bool
// operator<(variant<Types...> const&, variant<Types...> const&) noexcept;
//
// template <class ...Types>
// constexpr bool
// operator>(variant<Types...> const&, variant<Types...> const&) noexcept;
//
// template <class ...Types>
// constexpr bool
// operator<=(variant<Types...> const&, variant<Types...> const&) noexcept;
//
// template <class ...Types>
// constexpr bool
// operator>=(variant<Types...> const&, variant<Types...> const&) noexcept;

#include <cassert>
#include <type_traits>
#include <utility>
#include <variant>

#include "test_macros.h"

namespace relops::relops {
#ifndef TEST_HAS_NO_EXCEPTIONS
struct MakeEmptyT {
  MakeEmptyT() = default;
  MakeEmptyT(MakeEmptyT &&) { throw 42; }
  MakeEmptyT &operator=(MakeEmptyT &&) { throw 42; }
};
inline bool operator==(const MakeEmptyT &, const MakeEmptyT &) {
  assert(false);
  return false;
}
inline bool operator!=(const MakeEmptyT &, const MakeEmptyT &) {
  assert(false);
  return false;
}
inline bool operator<(const MakeEmptyT &, const MakeEmptyT &) {
  assert(false);
  return false;
}
inline bool operator<=(const MakeEmptyT &, const MakeEmptyT &) {
  assert(false);
  return false;
}
inline bool operator>(const MakeEmptyT &, const MakeEmptyT &) {
  assert(false);
  return false;
}
inline bool operator>=(const MakeEmptyT &, const MakeEmptyT &) {
  assert(false);
  return false;
}

template <class Variant> void makeEmpty(Variant &v) {
  Variant v2(std::in_place_type<MakeEmptyT>);
  try {
    v = std::move(v2);
    assert(false);
  } catch (...) {
    assert(v.valueless_by_exception());
  }
}
#endif // TEST_HAS_NO_EXCEPTIONS

struct MyBool {
  bool value;
  constexpr explicit MyBool(bool v) : value(v) {}
  constexpr operator bool() const noexcept { return value; }
};

struct ComparesToMyBool {
  int value = 0;
};
inline constexpr MyBool operator==(const ComparesToMyBool& LHS, const ComparesToMyBool& RHS) noexcept {
  return MyBool(LHS.value == RHS.value);
}
inline constexpr MyBool operator!=(const ComparesToMyBool& LHS, const ComparesToMyBool& RHS) noexcept {
  return MyBool(LHS.value != RHS.value);
}
inline constexpr MyBool operator<(const ComparesToMyBool& LHS, const ComparesToMyBool& RHS) noexcept {
  return MyBool(LHS.value < RHS.value);
}
inline constexpr MyBool operator<=(const ComparesToMyBool& LHS, const ComparesToMyBool& RHS) noexcept {
  return MyBool(LHS.value <= RHS.value);
}
inline constexpr MyBool operator>(const ComparesToMyBool& LHS, const ComparesToMyBool& RHS) noexcept {
  return MyBool(LHS.value > RHS.value);
}
inline constexpr MyBool operator>=(const ComparesToMyBool& LHS, const ComparesToMyBool& RHS) noexcept {
  return MyBool(LHS.value >= RHS.value);
}

template <class T1, class T2>
void test_equality_basic() {
  {
    using V = std::variant<T1, T2>;
    constexpr V v1(std::in_place_index<0>, T1{42});
    constexpr V v2(std::in_place_index<0>, T1{42});
    static_assert(v1 == v2, "");
    static_assert(v2 == v1, "");
    static_assert(!(v1 != v2), "");
    static_assert(!(v2 != v1), "");
  }
  {
    using V = std::variant<T1, T2>;
    constexpr V v1(std::in_place_index<0>, T1{42});
    constexpr V v2(std::in_place_index<0>, T1{43});
    static_assert(!(v1 == v2), "");
    static_assert(!(v2 == v1), "");
    static_assert(v1 != v2, "");
    static_assert(v2 != v1, "");
  }
  {
    using V = std::variant<T1, T2>;
    constexpr V v1(std::in_place_index<0>, T1{42});
    constexpr V v2(std::in_place_index<1>, T2{42});
    static_assert(!(v1 == v2), "");
    static_assert(!(v2 == v1), "");
    static_assert(v1 != v2, "");
    static_assert(v2 != v1, "");
  }
  {
    using V = std::variant<T1, T2>;
    constexpr V v1(std::in_place_index<1>, T2{42});
    constexpr V v2(std::in_place_index<1>, T2{42});
    static_assert(v1 == v2, "");
    static_assert(v2 == v1, "");
    static_assert(!(v1 != v2), "");
    static_assert(!(v2 != v1), "");
  }
}

void test_equality() {
  test_equality_basic<int, long>();
  test_equality_basic<ComparesToMyBool, int>();
  test_equality_basic<int, ComparesToMyBool>();
  test_equality_basic<ComparesToMyBool, ComparesToMyBool>();
#ifndef TEST_HAS_NO_EXCEPTIONS
  {
    using V = std::variant<int, MakeEmptyT>;
    V v1;
    V v2;
    makeEmpty(v2);
    assert(!(v1 == v2));
    assert(!(v2 == v1));
    assert(v1 != v2);
    assert(v2 != v1);
  }
  {
    using V = std::variant<int, MakeEmptyT>;
    V v1;
    makeEmpty(v1);
    V v2;
    assert(!(v1 == v2));
    assert(!(v2 == v1));
    assert(v1 != v2);
    assert(v2 != v1);
  }
  {
    using V = std::variant<int, MakeEmptyT>;
    V v1;
    makeEmpty(v1);
    V v2;
    makeEmpty(v2);
    assert(v1 == v2);
    assert(v2 == v1);
    assert(!(v1 != v2));
    assert(!(v2 != v1));
  }
#endif
}

template <class Var>
constexpr bool test_less(const Var &l, const Var &r, bool expect_less,
                         bool expect_greater) {
  static_assert(std::is_same_v<decltype(l < r), bool>, "");
  static_assert(std::is_same_v<decltype(l <= r), bool>, "");
  static_assert(std::is_same_v<decltype(l > r), bool>, "");
  static_assert(std::is_same_v<decltype(l >= r), bool>, "");

  return ((l < r) == expect_less) && (!(l >= r) == expect_less) &&
         ((l > r) == expect_greater) && (!(l <= r) == expect_greater);
}

template <class T1, class T2>
void test_relational_basic() {
  { // same index, same value
    using V = std::variant<T1, T2>;
    constexpr V v1(std::in_place_index<0>, T1{1});
    constexpr V v2(std::in_place_index<0>, T1{1});
    static_assert(test_less(v1, v2, false, false), "");
  }
  { // same index, value < other_value
    using V = std::variant<T1, T2>;
    constexpr V v1(std::in_place_index<0>, T1{0});
    constexpr V v2(std::in_place_index<0>, T1{1});
    static_assert(test_less(v1, v2, true, false), "");
  }
  { // same index, value > other_value
    using V = std::variant<T1, T2>;
    constexpr V v1(std::in_place_index<0>, T1{1});
    constexpr V v2(std::in_place_index<0>, T1{0});
    static_assert(test_less(v1, v2, false, true), "");
  }
  { // LHS.index() < RHS.index()
    using V = std::variant<T1, T2>;
    constexpr V v1(std::in_place_index<0>, T1{0});
    constexpr V v2(std::in_place_index<1>, T2{0});
    static_assert(test_less(v1, v2, true, false), "");
  }
  { // LHS.index() > RHS.index()
    using V = std::variant<T1, T2>;
    constexpr V v1(std::in_place_index<1>, T2{0});
    constexpr V v2(std::in_place_index<0>, T1{0});
    static_assert(test_less(v1, v2, false, true), "");
  }
}

void test_relational() {
  test_relational_basic<int, long>();
  test_relational_basic<ComparesToMyBool, int>();
  test_relational_basic<int, ComparesToMyBool>();
  test_relational_basic<ComparesToMyBool, ComparesToMyBool>();
#ifndef TEST_HAS_NO_EXCEPTIONS
  { // LHS.index() < RHS.index(), RHS is empty
    using V = std::variant<int, MakeEmptyT>;
    V v1;
    V v2;
    makeEmpty(v2);
    assert(test_less(v1, v2, false, true));
  }
  { // LHS.index() > RHS.index(), LHS is empty
    using V = std::variant<int, MakeEmptyT>;
    V v1;
    makeEmpty(v1);
    V v2;
    assert(test_less(v1, v2, true, false));
  }
  { // LHS.index() == RHS.index(), LHS and RHS are empty
    using V = std::variant<int, MakeEmptyT>;
    V v1;
    makeEmpty(v1);
    V v2;
    makeEmpty(v2);
    assert(test_less(v1, v2, false, false));
  }
#endif
}

int run_test() {
  test_equality();
  test_relational();

  return 0;
}
} // namespace relops::relops
// -- END: test/std/utilities/variant/variant.relops/relops.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.relops/three_way.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class... Types> class variant;

// template <class... Types> requires (three_way_comparable<Types> && ...)
//   constexpr std::common_comparison_category_t<
//     std::compare_three_way_result_t<Types>...>
//   operator<=>(const variant<Types...>& t, const variant<Types...>& u);

#include <cassert>
#include <limits>
#include <type_traits>
#include <utility>
#include <variant>

#include "test_macros.h"
#include "test_comparisons.h"

namespace relops::three_way {
#if _HAS_CXX20
#ifndef TEST_HAS_NO_EXCEPTIONS
// MakeEmptyT throws in operator=(&&), so we can move to it to create valueless-by-exception variants.
struct MakeEmptyT {
  MakeEmptyT() = default;
  MakeEmptyT(MakeEmptyT&&) { throw 42; }
  MakeEmptyT& operator=(MakeEmptyT&&) { throw 42; }
};
inline bool operator==(const MakeEmptyT&, const MakeEmptyT&) {
  assert(false);
  return false;
}
inline std::weak_ordering operator<=>(const MakeEmptyT&, const MakeEmptyT&) {
  assert(false);
  return std::weak_ordering::equivalent;
}

template <class Variant>
void makeEmpty(Variant& v) {
  Variant v2(std::in_place_type<MakeEmptyT>);
  try {
    v = std::move(v2);
    assert(false);
  } catch (...) {
    assert(v.valueless_by_exception());
  }
}

void test_empty() {
  {
    using V = std::variant<int, MakeEmptyT>;
    V v1;
    V v2;
    makeEmpty(v2);
    assert(testOrder(v1, v2, std::weak_ordering::greater));
  }
  {
    using V = std::variant<int, MakeEmptyT>;
    V v1;
    makeEmpty(v1);
    V v2;
    assert(testOrder(v1, v2, std::weak_ordering::less));
  }
  {
    using V = std::variant<int, MakeEmptyT>;
    V v1;
    makeEmpty(v1);
    V v2;
    makeEmpty(v2);
    assert(testOrder(v1, v2, std::weak_ordering::equivalent));
  }
}
#endif // TEST_HAS_NO_EXCEPTIONS

template <class T1, class T2, class Order>
constexpr bool test_with_types() {
  using V = std::variant<T1, T2>;
  AssertOrderReturn<Order, V>();
  { // same index, same value
    constexpr V v1(std::in_place_index<0>, T1{1});
    constexpr V v2(std::in_place_index<0>, T1{1});
    assert(testOrder(v1, v2, Order::equivalent));
  }
  { // same index, value < other_value
    constexpr V v1(std::in_place_index<0>, T1{0});
    constexpr V v2(std::in_place_index<0>, T1{1});
    assert(testOrder(v1, v2, Order::less));
  }
  { // same index, value > other_value
    constexpr V v1(std::in_place_index<0>, T1{1});
    constexpr V v2(std::in_place_index<0>, T1{0});
    assert(testOrder(v1, v2, Order::greater));
  }
  { // LHS.index() < RHS.index()
    constexpr V v1(std::in_place_index<0>, T1{0});
    constexpr V v2(std::in_place_index<1>, T2{0});
    assert(testOrder(v1, v2, Order::less));
  }
  { // LHS.index() > RHS.index()
    constexpr V v1(std::in_place_index<1>, T2{0});
    constexpr V v2(std::in_place_index<0>, T1{0});
    assert(testOrder(v1, v2, Order::greater));
  }

  return true;
}

constexpr bool test_three_way() {
  assert((test_with_types<int, double, std::partial_ordering>()));
  assert((test_with_types<int, long, std::strong_ordering>()));

  {
    using V              = std::variant<int, double>;
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    {
      constexpr V v1(std::in_place_type<int>, 1);
      constexpr V v2(std::in_place_type<double>, nan);
      assert(testOrder(v1, v2, std::partial_ordering::less));
    }
    {
      constexpr V v1(std::in_place_type<double>, nan);
      constexpr V v2(std::in_place_type<int>, 2);
      assert(testOrder(v1, v2, std::partial_ordering::greater));
    }
#if !defined(__clang__) // TRANSITION, DevCom-1626139 (MSVC), EDG says "floating-point values cannot be compared"
    if (!std::is_constant_evaluated())
#endif // ^^^ workaround ^^^
    {
      constexpr V v1(std::in_place_type<double>, nan);
      constexpr V v2(std::in_place_type<double>, nan);
      assert(testOrder(v1, v2, std::partial_ordering::unordered));
    }
  }

  return true;
}

// SFINAE tests
template <class T, class U = T>
concept has_three_way_op = requires (T& t, U& u) { t <=> u; };

// std::three_way_comparable is a more stringent requirement that demands
// operator== and a few other things.
using std::three_way_comparable;

struct HasSimpleOrdering {
  constexpr bool operator==(const HasSimpleOrdering&) const;
  constexpr bool operator<(const HasSimpleOrdering&) const;
};

struct HasOnlySpaceship {
  constexpr bool operator==(const HasOnlySpaceship&) const = delete;
  constexpr std::weak_ordering operator<=>(const HasOnlySpaceship&) const;
};

#ifdef __clang__ // TRANSITION, not yet investigated
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundefined-inline"
#endif // ^^^ workaround ^^^

struct HasFullOrdering {
  constexpr bool operator==(const HasFullOrdering&) const;
  constexpr std::weak_ordering operator<=>(const HasFullOrdering&) const;
};

#ifdef __clang__ // TRANSITION, not yet investigated
#pragma clang diagnostic pop
#endif // ^^^ workaround ^^^

// operator<=> must resolve the return types of all its union types'
// operator<=>s to determine its own return type, so it is detectable by SFINAE
static_assert(!has_three_way_op<HasSimpleOrdering>);
static_assert(!has_three_way_op<std::variant<int, HasSimpleOrdering>>);

static_assert(!three_way_comparable<HasSimpleOrdering>);
static_assert(!three_way_comparable<std::variant<int, HasSimpleOrdering>>);

static_assert(has_three_way_op<HasOnlySpaceship>);
static_assert(!has_three_way_op<std::variant<int, HasOnlySpaceship>>);

static_assert(!three_way_comparable<HasOnlySpaceship>);
static_assert(!three_way_comparable<std::variant<int, HasOnlySpaceship>>);

static_assert( has_three_way_op<HasFullOrdering>);
static_assert( has_three_way_op<std::variant<int, HasFullOrdering>>);

static_assert( three_way_comparable<HasFullOrdering>);
static_assert( three_way_comparable<std::variant<int, HasFullOrdering>>);

int run_test() {
  test_three_way();
  static_assert(test_three_way());

#ifndef TEST_HAS_NO_EXCEPTIONS
  test_empty();
#endif // TEST_HAS_NO_EXCEPTIONS

  return 0;
}
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
int run_test() {
  return 0;
}
#endif // ^^^ !_HAS_CXX20 ^^^
} // namespace relops::three_way
// -- END: test/std/utilities/variant/variant.relops/three_way.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.synopsis/variant_npos.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// constexpr size_t variant_npos = -1;

#include <variant>

#include "test_macros.h"

namespace npos {
int run_test() {
  static_assert(std::variant_npos == static_cast<std::size_t>(-1), "");

  return 0;
}
} // namespace npos
// -- END: test/std/utilities/variant/variant.synopsis/variant_npos.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/implicit_ctad.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// Make sure that the implicitly-generated CTAD works.

// We make sure that it is not ill-formed, however we still produce a warning for
// this one because explicit construction from a variant using CTAD is ambiguous
// (in the sense that the programmer intent is not clear).
// ADDITIONAL_COMPILE_FLAGS(gcc-style-warnings): -Wno-ctad-maybe-unsupported

#include <variant>

#include "test_macros.h"

namespace implicit_ctad {
int run_test() {
  // This is the motivating example from P0739R0
  {
    std::variant<int, double> v1(3);
    std::variant v2 = v1;
    ASSERT_SAME_TYPE(decltype(v2), std::variant<int, double>);
  }

  {
    std::variant<int, double> v1(3);
    std::variant v2 = std::variant(v1); // Technically valid, but intent is ambiguous!
    ASSERT_SAME_TYPE(decltype(v2), std::variant<int, double>);
  }

  return 0;
}
} // namespace implicit_ctad
// -- END: test/std/utilities/variant/variant.variant/implicit_ctad.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.assign/conv.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// template <class T>
// variant& operator=(T&&) noexcept(see below);

#include <variant>
#include <string>
#include <memory>

#include "variant_test_helpers.h"

namespace assign::conv {
int run_test()
{
  static_assert(!std::is_assignable<std::variant<int, int>, int>::value, "");
  static_assert(!std::is_assignable<std::variant<long, long long>, int>::value, "");
  static_assert(!std::is_assignable<std::variant<char>, int>::value, "");

  static_assert(!std::is_assignable<std::variant<std::string, float>, int>::value, "");
  static_assert(!std::is_assignable<std::variant<std::string, double>, int>::value, "");
  static_assert(!std::is_assignable<std::variant<std::string, bool>, int>::value, "");

  static_assert(!std::is_assignable<std::variant<int, bool>, decltype("meow")>::value, "");
  static_assert(!std::is_assignable<std::variant<int, const bool>, decltype("meow")>::value, "");

  static_assert(std::is_assignable<std::variant<bool>, std::true_type>::value, "");
  static_assert(!std::is_assignable<std::variant<bool>, std::unique_ptr<char> >::value, "");
  static_assert(std::is_assignable<std::variant<bool>, decltype(nullptr)>::value == is_permissive, "");

  return 0;
}
} // namespace assign::conv
// -- END: test/std/utilities/variant/variant.variant/variant.assign/conv.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.assign/copy.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// constexpr variant& operator=(variant const&);

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>

#include "test_macros.h"

namespace assign::copy {
struct NoCopy {
  NoCopy(const NoCopy&)            = delete;
  NoCopy& operator=(const NoCopy&) = default;
};

struct CopyOnly {
  CopyOnly(const CopyOnly&)            = default;
  CopyOnly(CopyOnly&&)                 = delete;
  CopyOnly& operator=(const CopyOnly&) = default;
  CopyOnly& operator=(CopyOnly&&)      = delete;
};

struct MoveOnly {
  MoveOnly(const MoveOnly&)            = delete;
  MoveOnly(MoveOnly&&)                 = default;
  MoveOnly& operator=(const MoveOnly&) = default;
};

struct MoveOnlyNT {
  MoveOnlyNT(const MoveOnlyNT&) = delete;
  MoveOnlyNT(MoveOnlyNT&&) {}
  MoveOnlyNT& operator=(const MoveOnlyNT&) = default;
};

struct CopyAssign {
  constexpr CopyAssign(int v, int* alv, int* cpy_ctr, int* cpy_assi, int* move_ctr, int* move_assi)
      : value(v),
        alive(alv),
        copy_construct(cpy_ctr),
        copy_assign(cpy_assi),
        move_construct(move_ctr),
        move_assign(move_assi) {
    ++*alive;
  }
  constexpr CopyAssign(const CopyAssign& o)
      : value(o.value),
        alive(o.alive),
        copy_construct(o.copy_construct),
        copy_assign(o.copy_assign),
        move_construct(o.move_construct),
        move_assign(o.move_assign) {
    ++*alive;
    ++*copy_construct;
  }
  constexpr CopyAssign(CopyAssign&& o) noexcept
      : value(o.value),
        alive(o.alive),
        copy_construct(o.copy_construct),
        copy_assign(o.copy_assign),
        move_construct(o.move_construct),
        move_assign(o.move_assign) {
    o.value = -1;
    ++*alive;
    ++*move_construct;
  }
  constexpr CopyAssign& operator=(const CopyAssign& o) {
    value          = o.value;
    alive          = o.alive;
    copy_construct = o.copy_construct;
    copy_assign    = o.copy_assign;
    move_construct = o.move_construct;
    move_assign    = o.move_assign;
    ++*copy_assign;
    return *this;
  }
  constexpr CopyAssign& operator=(CopyAssign&& o) noexcept {
    value          = o.value;
    alive          = o.alive;
    copy_construct = o.copy_construct;
    copy_assign    = o.copy_assign;
    move_construct = o.move_construct;
    move_assign    = o.move_assign;
    o.value        = -1;
    ++*move_assign;
    return *this;
  }
  TEST_CONSTEXPR_CXX20 ~CopyAssign() { --*alive; }
  int value;
  int* alive;
  int* copy_construct;
  int* copy_assign;
  int* move_construct;
  int* move_assign;
};

struct CopyMaybeThrows {
  CopyMaybeThrows(const CopyMaybeThrows&);
  CopyMaybeThrows& operator=(const CopyMaybeThrows&);
};
struct CopyDoesThrow {
  CopyDoesThrow(const CopyDoesThrow&) noexcept(false);
  CopyDoesThrow& operator=(const CopyDoesThrow&) noexcept(false);
};

struct NTCopyAssign {
  constexpr NTCopyAssign(int v) : value(v) {}
  NTCopyAssign(const NTCopyAssign&) = default;
  NTCopyAssign(NTCopyAssign&&)      = default;
  NTCopyAssign& operator=(const NTCopyAssign& that) {
    value = that.value;
    return *this;
  };
  NTCopyAssign& operator=(NTCopyAssign&&) = delete;
  int value;
};

static_assert(!std::is_trivially_copy_assignable<NTCopyAssign>::value, "");
static_assert(std::is_copy_assignable<NTCopyAssign>::value, "");

struct TCopyAssign {
  constexpr TCopyAssign(int v) : value(v) {}
  TCopyAssign(const TCopyAssign&)            = default;
  TCopyAssign(TCopyAssign&&)                 = default;
  TCopyAssign& operator=(const TCopyAssign&) = default;
  TCopyAssign& operator=(TCopyAssign&&)      = delete;
  int value;
};

static_assert(std::is_trivially_copy_assignable<TCopyAssign>::value, "");

struct TCopyAssignNTMoveAssign {
  constexpr TCopyAssignNTMoveAssign(int v) : value(v) {}
  TCopyAssignNTMoveAssign(const TCopyAssignNTMoveAssign&)            = default;
  TCopyAssignNTMoveAssign(TCopyAssignNTMoveAssign&&)                 = default;
  TCopyAssignNTMoveAssign& operator=(const TCopyAssignNTMoveAssign&) = default;
  TCopyAssignNTMoveAssign& operator=(TCopyAssignNTMoveAssign&& that) {
    value      = that.value;
    that.value = -1;
    return *this;
  }
  int value;
};

static_assert(std::is_trivially_copy_assignable_v<TCopyAssignNTMoveAssign>, "");

#ifndef TEST_HAS_NO_EXCEPTIONS
struct CopyThrows {
  CopyThrows() = default;
  CopyThrows(const CopyThrows&) { throw 42; }
  CopyThrows& operator=(const CopyThrows&) { throw 42; }
};

struct CopyCannotThrow {
  static int alive;
  CopyCannotThrow() { ++alive; }
  CopyCannotThrow(const CopyCannotThrow&) noexcept { ++alive; }
  CopyCannotThrow(CopyCannotThrow&&) noexcept { assert(false); }
  CopyCannotThrow& operator=(const CopyCannotThrow&) noexcept = default;
  CopyCannotThrow& operator=(CopyCannotThrow&&) noexcept {
    assert(false);
    return *this;
  }
};

int CopyCannotThrow::alive = 0;

struct MoveThrows {
  static int alive;
  MoveThrows() { ++alive; }
  MoveThrows(const MoveThrows&) { ++alive; }
  MoveThrows(MoveThrows&&) { throw 42; }
  MoveThrows& operator=(const MoveThrows&) { return *this; }
  MoveThrows& operator=(MoveThrows&&) { throw 42; }
  ~MoveThrows() { --alive; }
};

int MoveThrows::alive = 0;

struct MakeEmptyT {
  static int alive;
  MakeEmptyT() { ++alive; }
  MakeEmptyT(const MakeEmptyT&) {
    ++alive;
    // Don't throw from the copy constructor since variant's assignment
    // operator performs a copy before committing to the assignment.
  }
  MakeEmptyT(MakeEmptyT&&) { throw 42; }
  MakeEmptyT& operator=(const MakeEmptyT&) { throw 42; }
  MakeEmptyT& operator=(MakeEmptyT&&) { throw 42; }
  ~MakeEmptyT() { --alive; }
};

int MakeEmptyT::alive = 0;

template <class Variant>
void makeEmpty(Variant& v) {
  Variant v2(std::in_place_type<MakeEmptyT>);
  try {
    v = std::move(v2);
    assert(false);
  } catch (...) {
    assert(v.valueless_by_exception());
  }
}
#endif // TEST_HAS_NO_EXCEPTIONS

constexpr void test_copy_assignment_not_noexcept() {
  {
    using V = std::variant<CopyMaybeThrows>;
    static_assert(!std::is_nothrow_copy_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, CopyDoesThrow>;
    static_assert(!std::is_nothrow_copy_assignable<V>::value, "");
  }
}

constexpr void test_copy_assignment_sfinae() {
  {
    using V = std::variant<int, long>;
    static_assert(std::is_copy_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, CopyOnly>;
    static_assert(std::is_copy_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, NoCopy>;
    static_assert(!std::is_copy_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, MoveOnly>;
    static_assert(!std::is_copy_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, MoveOnlyNT>;
    static_assert(!std::is_copy_assignable<V>::value, "");
  }

  // Make sure we properly propagate triviality (see P0602R4).
  {
    using V = std::variant<int, long>;
    static_assert(std::is_trivially_copy_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, NTCopyAssign>;
    static_assert(!std::is_trivially_copy_assignable<V>::value, "");
    static_assert(std::is_copy_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, TCopyAssign>;
    static_assert(std::is_trivially_copy_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, TCopyAssignNTMoveAssign>;
    static_assert(std::is_trivially_copy_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, CopyOnly>;
    static_assert(std::is_trivially_copy_assignable<V>::value, "");
  }
}

void test_copy_assignment_empty_empty() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  using MET = MakeEmptyT;
  {
    using V = std::variant<int, long, MET>;
    V v1(std::in_place_index<0>);
    makeEmpty(v1);
    V v2(std::in_place_index<0>);
    makeEmpty(v2);
    V& vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.valueless_by_exception());
    assert(v1.index() == std::variant_npos);
  }
#endif // TEST_HAS_NO_EXCEPTIONS
}

void test_copy_assignment_non_empty_empty() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  using MET = MakeEmptyT;
  {
    using V = std::variant<int, MET>;
    V v1(std::in_place_index<0>, 42);
    V v2(std::in_place_index<0>);
    makeEmpty(v2);
    V& vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.valueless_by_exception());
    assert(v1.index() == std::variant_npos);
  }
  {
    using V = std::variant<int, MET, std::string>;
    V v1(std::in_place_index<2>, "hello");
    V v2(std::in_place_index<0>);
    makeEmpty(v2);
    V& vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.valueless_by_exception());
    assert(v1.index() == std::variant_npos);
  }
#endif // TEST_HAS_NO_EXCEPTIONS
}

void test_copy_assignment_empty_non_empty() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  using MET = MakeEmptyT;
  {
    using V = std::variant<int, MET>;
    V v1(std::in_place_index<0>);
    makeEmpty(v1);
    V v2(std::in_place_index<0>, 42);
    V& vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 0);
    assert(std::get<0>(v1) == 42);
  }
  {
    using V = std::variant<int, MET, std::string>;
    V v1(std::in_place_index<0>);
    makeEmpty(v1);
    V v2(std::in_place_type<std::string>, "hello");
    V& vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 2);
    assert(std::get<2>(v1) == "hello");
  }
#endif // TEST_HAS_NO_EXCEPTIONS
}

template <typename T>
struct Result {
  std::size_t index;
  T value;
};

TEST_CONSTEXPR_CXX20 void test_copy_assignment_same_index() {
  {
    using V = std::variant<int>;
    V v1(43);
    V v2(42);
    V& vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 0);
    assert(std::get<0>(v1) == 42);
  }
  {
    using V = std::variant<int, long, unsigned>;
    V v1(43l);
    V v2(42l);
    V& vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 1);
    assert(std::get<1>(v1) == 42);
  }
  {
    using V            = std::variant<int, CopyAssign, unsigned>;
    int alive          = 0;
    int copy_construct = 0;
    int copy_assign    = 0;
    int move_construct = 0;
    int move_assign    = 0;
    V v1(std::in_place_type<CopyAssign>, 43, &alive, &copy_construct, &copy_assign, &move_construct, &move_assign);
    V v2(std::in_place_type<CopyAssign>, 42, &alive, &copy_construct, &copy_assign, &move_construct, &move_assign);
    V& vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 1);
    assert(std::get<1>(v1).value == 42);
    assert(copy_construct == 0);
    assert(move_construct == 0);
    assert(copy_assign == 1);
  }

  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
  {
    struct {
      constexpr Result<int> operator()() const {
        using V = std::variant<int>;
        V v(43);
        V v2(42);
        v = v2;
        return {v.index(), std::get<0>(v)};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 0, "");
    static_assert(result.value == 42, "");
  }
  {
    struct {
      constexpr Result<long> operator()() const {
        using V = std::variant<int, long, unsigned>;
        V v(43l);
        V v2(42l);
        v = v2;
        return {v.index(), std::get<1>(v)};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 1, "");
    static_assert(result.value == 42l, "");
  }
  {
    struct {
      constexpr Result<int> operator()() const {
        using V = std::variant<int, TCopyAssign, unsigned>;
        V v(std::in_place_type<TCopyAssign>, 43);
        V v2(std::in_place_type<TCopyAssign>, 42);
        v = v2;
        return {v.index(), std::get<1>(v).value};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 1, "");
    static_assert(result.value == 42, "");
  }
  {
    struct {
      constexpr Result<int> operator()() const {
        using V = std::variant<int, TCopyAssignNTMoveAssign, unsigned>;
        V v(std::in_place_type<TCopyAssignNTMoveAssign>, 43);
        V v2(std::in_place_type<TCopyAssignNTMoveAssign>, 42);
        v = v2;
        return {v.index(), std::get<1>(v).value};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 1, "");
    static_assert(result.value == 42, "");
  }
}

TEST_CONSTEXPR_CXX20 void test_copy_assignment_different_index() {
  {
    using V = std::variant<int, long, unsigned>;
    V v1(43);
    V v2(42l);
    V& vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 1);
    assert(std::get<1>(v1) == 42);
  }
  {
    using V            = std::variant<int, CopyAssign, unsigned>;
    int alive          = 0;
    int copy_construct = 0;
    int copy_assign    = 0;
    int move_construct = 0;
    int move_assign    = 0;
    V v1(std::in_place_type<unsigned>, 43u);
    V v2(std::in_place_type<CopyAssign>, 42, &alive, &copy_construct, &copy_assign, &move_construct, &move_assign);
    assert(copy_construct == 0);
    assert(move_construct == 0);
    assert(alive == 1);
    V& vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 1);
    assert(std::get<1>(v1).value == 42);
    assert(alive == 2);
    assert(copy_construct == 1);
    assert(move_construct == 1);
    assert(copy_assign == 0);
  }

  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
  {
    struct {
      constexpr Result<long> operator()() const {
        using V = std::variant<int, long, unsigned>;
        V v(43);
        V v2(42l);
        v = v2;
        return {v.index(), std::get<1>(v)};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 1, "");
    static_assert(result.value == 42l, "");
  }
  {
    struct {
      constexpr Result<int> operator()() const {
        using V = std::variant<int, TCopyAssign, unsigned>;
        V v(std::in_place_type<unsigned>, 43u);
        V v2(std::in_place_type<TCopyAssign>, 42);
        v = v2;
        return {v.index(), std::get<1>(v).value};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 1, "");
    static_assert(result.value == 42, "");
  }
}

void test_assignment_throw() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  using MET = MakeEmptyT;
  // same index
  {
    using V = std::variant<int, MET, std::string>;
    V v1(std::in_place_type<MET>);
    MET& mref = std::get<1>(v1);
    V v2(std::in_place_type<MET>);
    try {
      v1 = v2;
      assert(false);
    } catch (...) {
    }
    assert(v1.index() == 1);
    assert(&std::get<1>(v1) == &mref);
  }

  // difference indices
  {
    using V = std::variant<int, CopyThrows, std::string>;
    V v1(std::in_place_type<std::string>, "hello");
    V v2(std::in_place_type<CopyThrows>);
    try {
      v1 = v2;
      assert(false);
    } catch (...) { /* ... */
    }
    // Test that copy construction is used directly if move construction may throw,
    // resulting in a valueless variant if copy throws.
    assert(v1.valueless_by_exception());
  }
  {
    using V = std::variant<int, MoveThrows, std::string>;
    V v1(std::in_place_type<std::string>, "hello");
    V v2(std::in_place_type<MoveThrows>);
    assert(MoveThrows::alive == 1);
    // Test that copy construction is used directly if move construction may throw.
    v1 = v2;
    assert(v1.index() == 1);
    assert(v2.index() == 1);
    assert(MoveThrows::alive == 2);
  }
  {
    // Test that direct copy construction is preferred when it cannot throw.
    using V = std::variant<int, CopyCannotThrow, std::string>;
    V v1(std::in_place_type<std::string>, "hello");
    V v2(std::in_place_type<CopyCannotThrow>);
    assert(CopyCannotThrow::alive == 1);
    v1 = v2;
    assert(v1.index() == 1);
    assert(v2.index() == 1);
    assert(CopyCannotThrow::alive == 2);
  }
  {
    using V = std::variant<int, CopyThrows, std::string>;
    V v1(std::in_place_type<CopyThrows>);
    V v2(std::in_place_type<std::string>, "hello");
    V& vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 2);
    assert(std::get<2>(v1) == "hello");
    assert(v2.index() == 2);
    assert(std::get<2>(v2) == "hello");
  }
  {
    using V = std::variant<int, MoveThrows, std::string>;
    V v1(std::in_place_type<MoveThrows>);
    V v2(std::in_place_type<std::string>, "hello");
    V& vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 2);
    assert(std::get<2>(v1) == "hello");
    assert(v2.index() == 2);
    assert(std::get<2>(v2) == "hello");
  }
#endif // TEST_HAS_NO_EXCEPTIONS
}

template <std::size_t NewIdx, class T, class ValueType>
constexpr void test_constexpr_assign_imp(T&& v, ValueType&& new_value) {
  using Variant = std::decay_t<T>;
  const Variant cp(std::forward<ValueType>(new_value));
  v = cp;
  assert(v.index() == NewIdx);
  assert(std::get<NewIdx>(v) == std::get<NewIdx>(cp));
}

constexpr void test_constexpr_copy_assignment_trivial() {
  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
  using V = std::variant<long, void*, int>;
  static_assert(std::is_trivially_copyable<V>::value, "");
  static_assert(std::is_trivially_copy_assignable<V>::value, "");
  test_constexpr_assign_imp<0>(V(42l), 101l);
  test_constexpr_assign_imp<0>(V(nullptr), 101l);
  test_constexpr_assign_imp<1>(V(42l), nullptr);
  test_constexpr_assign_imp<2>(V(42l), 101);
}

struct NonTrivialCopyAssign {
  int i = 0;
  constexpr NonTrivialCopyAssign(int ii) : i(ii) {}
  constexpr NonTrivialCopyAssign(const NonTrivialCopyAssign& other) : i(other.i) {}
  constexpr NonTrivialCopyAssign& operator=(const NonTrivialCopyAssign& o) {
    i = o.i;
    return *this;
  }
  TEST_CONSTEXPR_CXX20 ~NonTrivialCopyAssign() = default;
  friend constexpr bool operator==(const NonTrivialCopyAssign& x, const NonTrivialCopyAssign& y) { return x.i == y.i; }
};

constexpr void test_constexpr_copy_assignment_non_trivial() {
  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
  using V = std::variant<long, void*, NonTrivialCopyAssign>;
  static_assert(!std::is_trivially_copyable<V>::value, "");
  static_assert(!std::is_trivially_copy_assignable<V>::value, "");
  test_constexpr_assign_imp<0>(V(42l), 101l);
  test_constexpr_assign_imp<0>(V(nullptr), 101l);
  test_constexpr_assign_imp<1>(V(42l), nullptr);
  test_constexpr_assign_imp<2>(V(42l), NonTrivialCopyAssign(5));
  test_constexpr_assign_imp<2>(V(NonTrivialCopyAssign(3)), NonTrivialCopyAssign(5));
}

void non_constexpr_test() {
  test_copy_assignment_empty_empty();
  test_copy_assignment_non_empty_empty();
  test_copy_assignment_empty_non_empty();
  test_assignment_throw();
}

constexpr bool cxx17_constexpr_test() {
  test_copy_assignment_sfinae();
  test_copy_assignment_not_noexcept();
  test_constexpr_copy_assignment_trivial();

  return true;
}

TEST_CONSTEXPR_CXX20 bool cxx20_constexpr_test() {
  test_copy_assignment_same_index();
  test_copy_assignment_different_index();
  test_constexpr_copy_assignment_non_trivial();

  return true;
}

int run_test() {
  non_constexpr_test();
  cxx17_constexpr_test();
  cxx20_constexpr_test();

  static_assert(cxx17_constexpr_test());
#if TEST_STD_VER >= 20
  static_assert(cxx20_constexpr_test());
#endif
  return 0;
}
} // namespace assign::copy
// -- END: test/std/utilities/variant/variant.variant/variant.assign/copy.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.assign/move.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// constexpr variant& operator=(variant&&) noexcept(see below);

#include <cassert>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

#include "test_macros.h"
#include "variant_test_helpers.h"

namespace assign::move {
struct NoCopy {
  NoCopy(const NoCopy&)            = delete;
  NoCopy& operator=(const NoCopy&) = default;
};

struct CopyOnly {
  CopyOnly(const CopyOnly&)            = default;
  CopyOnly(CopyOnly&&)                 = delete;
  CopyOnly& operator=(const CopyOnly&) = default;
  CopyOnly& operator=(CopyOnly&&)      = delete;
};

struct MoveOnly {
  MoveOnly(const MoveOnly&)            = delete;
  MoveOnly(MoveOnly&&)                 = default;
  MoveOnly& operator=(const MoveOnly&) = delete;
  MoveOnly& operator=(MoveOnly&&)      = default;
};

struct MoveOnlyNT {
  MoveOnlyNT(const MoveOnlyNT&) = delete;
  MoveOnlyNT(MoveOnlyNT&&) {}
  MoveOnlyNT& operator=(const MoveOnlyNT&) = delete;
  MoveOnlyNT& operator=(MoveOnlyNT&&)      = default;
};

struct MoveOnlyOddNothrow {
  MoveOnlyOddNothrow(MoveOnlyOddNothrow&&) noexcept(false) {}
  MoveOnlyOddNothrow(const MoveOnlyOddNothrow&)                = delete;
  MoveOnlyOddNothrow& operator=(MoveOnlyOddNothrow&&) noexcept = default;
  MoveOnlyOddNothrow& operator=(const MoveOnlyOddNothrow&)     = delete;
};

struct MoveAssignOnly {
  MoveAssignOnly(MoveAssignOnly&&)            = delete;
  MoveAssignOnly& operator=(MoveAssignOnly&&) = default;
};

struct MoveAssign {
  constexpr MoveAssign(int v, int* move_ctor, int* move_assi)
      : value(v), move_construct(move_ctor), move_assign(move_assi) {}
  constexpr MoveAssign(MoveAssign&& o) : value(o.value), move_construct(o.move_construct), move_assign(o.move_assign) {
    ++*move_construct;
    o.value = -1;
  }
  constexpr MoveAssign& operator=(MoveAssign&& o) {
    value          = o.value;
    move_construct = o.move_construct;
    move_assign    = o.move_assign;
    ++*move_assign;
    o.value = -1;
    return *this;
  }
  int value;
  int* move_construct;
  int* move_assign;
};

struct NTMoveAssign {
  constexpr NTMoveAssign(int v) : value(v) {}
  NTMoveAssign(const NTMoveAssign&)                 = default;
  NTMoveAssign(NTMoveAssign&&)                      = default;
  NTMoveAssign& operator=(const NTMoveAssign& that) = default;
  NTMoveAssign& operator=(NTMoveAssign&& that) {
    value      = that.value;
    that.value = -1;
    return *this;
  };
  int value;
};

static_assert(!std::is_trivially_move_assignable<NTMoveAssign>::value, "");
static_assert(std::is_move_assignable<NTMoveAssign>::value, "");

struct TMoveAssign {
  constexpr TMoveAssign(int v) : value(v) {}
  TMoveAssign(const TMoveAssign&)            = delete;
  TMoveAssign(TMoveAssign&&)                 = default;
  TMoveAssign& operator=(const TMoveAssign&) = delete;
  TMoveAssign& operator=(TMoveAssign&&)      = default;
  int value;
};

static_assert(std::is_trivially_move_assignable<TMoveAssign>::value, "");

struct TMoveAssignNTCopyAssign {
  constexpr TMoveAssignNTCopyAssign(int v) : value(v) {}
  TMoveAssignNTCopyAssign(const TMoveAssignNTCopyAssign&) = default;
  TMoveAssignNTCopyAssign(TMoveAssignNTCopyAssign&&)      = default;
  TMoveAssignNTCopyAssign& operator=(const TMoveAssignNTCopyAssign& that) {
    value = that.value;
    return *this;
  }
  TMoveAssignNTCopyAssign& operator=(TMoveAssignNTCopyAssign&&) = default;
  int value;
};

static_assert(std::is_trivially_move_assignable_v<TMoveAssignNTCopyAssign>, "");

struct TrivialCopyNontrivialMove {
  TrivialCopyNontrivialMove(TrivialCopyNontrivialMove const&) = default;
  TrivialCopyNontrivialMove(TrivialCopyNontrivialMove&&) noexcept {}
  TrivialCopyNontrivialMove& operator=(TrivialCopyNontrivialMove const&) = default;
  TrivialCopyNontrivialMove& operator=(TrivialCopyNontrivialMove&&) noexcept { return *this; }
};

static_assert(std::is_trivially_copy_assignable_v<TrivialCopyNontrivialMove>, "");
static_assert(!std::is_trivially_move_assignable_v<TrivialCopyNontrivialMove>, "");

constexpr void test_move_assignment_noexcept() {
  {
    using V = std::variant<int>;
    static_assert(std::is_nothrow_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<MoveOnly>;
    static_assert(std::is_nothrow_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, long>;
    static_assert(std::is_nothrow_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, MoveOnly>;
    static_assert(std::is_nothrow_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<MoveOnlyNT>;
    static_assert(!std::is_nothrow_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<MoveOnlyOddNothrow>;
    static_assert(!std::is_nothrow_move_assignable<V>::value, "");
  }
}

constexpr void test_move_assignment_sfinae() {
  {
    using V = std::variant<int, long>;
    static_assert(std::is_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, CopyOnly>;
    static_assert(std::is_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, NoCopy>;
    static_assert(!std::is_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, MoveOnly>;
    static_assert(std::is_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, MoveOnlyNT>;
    static_assert(std::is_move_assignable<V>::value, "");
  }
  {
    // variant only provides move assignment when the types also provide
    // a move constructor.
    using V = std::variant<int, MoveAssignOnly>;
    static_assert(!std::is_move_assignable<V>::value, "");
  }

  // Make sure we properly propagate triviality (see P0602R4).
  {
    using V = std::variant<int, long>;
    static_assert(std::is_trivially_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, NTMoveAssign>;
    static_assert(!std::is_trivially_move_assignable<V>::value, "");
    static_assert(std::is_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, TMoveAssign>;
    static_assert(std::is_trivially_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, TMoveAssignNTCopyAssign>;
    static_assert(std::is_trivially_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, TrivialCopyNontrivialMove>;
    static_assert(!std::is_trivially_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, CopyOnly>;
    static_assert(std::is_trivially_move_assignable<V>::value, "");
  }
}

void test_move_assignment_empty_empty() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  using MET = MakeEmptyT;
  {
    using V = std::variant<int, long, MET>;
    V v1(std::in_place_index<0>);
    makeEmpty(v1);
    V v2(std::in_place_index<0>);
    makeEmpty(v2);
    V& vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.valueless_by_exception());
    assert(v1.index() == std::variant_npos);
  }
#endif // TEST_HAS_NO_EXCEPTIONS
}

void test_move_assignment_non_empty_empty() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  using MET = MakeEmptyT;
  {
    using V = std::variant<int, MET>;
    V v1(std::in_place_index<0>, 42);
    V v2(std::in_place_index<0>);
    makeEmpty(v2);
    V& vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.valueless_by_exception());
    assert(v1.index() == std::variant_npos);
  }
  {
    using V = std::variant<int, MET, std::string>;
    V v1(std::in_place_index<2>, "hello");
    V v2(std::in_place_index<0>);
    makeEmpty(v2);
    V& vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.valueless_by_exception());
    assert(v1.index() == std::variant_npos);
  }
#endif // TEST_HAS_NO_EXCEPTIONS
}

void test_move_assignment_empty_non_empty() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  using MET = MakeEmptyT;
  {
    using V = std::variant<int, MET>;
    V v1(std::in_place_index<0>);
    makeEmpty(v1);
    V v2(std::in_place_index<0>, 42);
    V& vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.index() == 0);
    assert(std::get<0>(v1) == 42);
  }
  {
    using V = std::variant<int, MET, std::string>;
    V v1(std::in_place_index<0>);
    makeEmpty(v1);
    V v2(std::in_place_type<std::string>, "hello");
    V& vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.index() == 2);
    assert(std::get<2>(v1) == "hello");
  }
#endif // TEST_HAS_NO_EXCEPTIONS
}

template <typename T>
struct Result {
  std::size_t index;
  T value;
};

TEST_CONSTEXPR_CXX20 void test_move_assignment_same_index() {
  {
    using V = std::variant<int>;
    V v1(43);
    V v2(42);
    V& vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.index() == 0);
    assert(std::get<0>(v1) == 42);
  }
  {
    using V = std::variant<int, long, unsigned>;
    V v1(43l);
    V v2(42l);
    V& vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.index() == 1);
    assert(std::get<1>(v1) == 42);
  }
  {
    using V            = std::variant<int, MoveAssign, unsigned>;
    int move_construct = 0;
    int move_assign    = 0;
    V v1(std::in_place_type<MoveAssign>, 43, &move_construct, &move_assign);
    V v2(std::in_place_type<MoveAssign>, 42, &move_construct, &move_assign);
    V& vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.index() == 1);
    assert(std::get<1>(v1).value == 42);
    assert(move_construct == 0);
    assert(move_assign == 1);
  }

  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
  {
    struct {
      constexpr Result<int> operator()() const {
        using V = std::variant<int>;
        V v(43);
        V v2(42);
        v = std::move(v2);
        return {v.index(), std::get<0>(v)};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 0, "");
    static_assert(result.value == 42, "");
  }
  {
    struct {
      constexpr Result<long> operator()() const {
        using V = std::variant<int, long, unsigned>;
        V v(43l);
        V v2(42l);
        v = std::move(v2);
        return {v.index(), std::get<1>(v)};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 1, "");
    static_assert(result.value == 42l, "");
  }
  {
    struct {
      constexpr Result<int> operator()() const {
        using V = std::variant<int, TMoveAssign, unsigned>;
        V v(std::in_place_type<TMoveAssign>, 43);
        V v2(std::in_place_type<TMoveAssign>, 42);
        v = std::move(v2);
        return {v.index(), std::get<1>(v).value};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 1, "");
    static_assert(result.value == 42, "");
  }
}

TEST_CONSTEXPR_CXX20 void test_move_assignment_different_index() {
  {
    using V = std::variant<int, long, unsigned>;
    V v1(43);
    V v2(42l);
    V& vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.index() == 1);
    assert(std::get<1>(v1) == 42);
  }
  {
    using V            = std::variant<int, MoveAssign, unsigned>;
    int move_construct = 0;
    int move_assign    = 0;
    V v1(std::in_place_type<unsigned>, 43u);
    V v2(std::in_place_type<MoveAssign>, 42, &move_construct, &move_assign);
    V& vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.index() == 1);
    assert(std::get<1>(v1).value == 42);
    assert(move_construct == 1);
    assert(move_assign == 0);
  }

  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
  {
    struct {
      constexpr Result<long> operator()() const {
        using V = std::variant<int, long, unsigned>;
        V v(43);
        V v2(42l);
        v = std::move(v2);
        return {v.index(), std::get<1>(v)};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 1, "");
    static_assert(result.value == 42l, "");
  }
  {
    struct {
      constexpr Result<long> operator()() const {
        using V = std::variant<int, TMoveAssign, unsigned>;
        V v(std::in_place_type<unsigned>, 43u);
        V v2(std::in_place_type<TMoveAssign>, 42);
        v = std::move(v2);
        return {v.index(), std::get<1>(v).value};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 1, "");
    static_assert(result.value == 42, "");
  }
}

void test_assignment_throw() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  using MET = MakeEmptyT;
  // same index
  {
    using V = std::variant<int, MET, std::string>;
    V v1(std::in_place_type<MET>);
    MET& mref = std::get<1>(v1);
    V v2(std::in_place_type<MET>);
    try {
      v1 = std::move(v2);
      assert(false);
    } catch (...) {
    }
    assert(v1.index() == 1);
    assert(&std::get<1>(v1) == &mref);
  }

  // different indices
  {
    using V = std::variant<int, MET, std::string>;
    V v1(std::in_place_type<int>);
    V v2(std::in_place_type<MET>);
    try {
      v1 = std::move(v2);
      assert(false);
    } catch (...) {
    }
    assert(v1.valueless_by_exception());
    assert(v1.index() == std::variant_npos);
  }
  {
    using V = std::variant<int, MET, std::string>;
    V v1(std::in_place_type<MET>);
    V v2(std::in_place_type<std::string>, "hello");
    V& vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.index() == 2);
    assert(std::get<2>(v1) == "hello");
  }
#endif // TEST_HAS_NO_EXCEPTIONS
}

template <std::size_t NewIdx, class T, class ValueType>
constexpr void test_constexpr_assign_imp(T&& v, ValueType&& new_value) {
  using Variant = std::decay_t<T>;
  Variant v2(std::forward<ValueType>(new_value));
  const auto cp = v2;
  v             = std::move(v2);
  assert(v.index() == NewIdx);
  assert(std::get<NewIdx>(v) == std::get<NewIdx>(cp));
}

constexpr void test_constexpr_move_assignment_trivial() {
  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
  using V = std::variant<long, void*, int>;
  static_assert(std::is_trivially_copyable<V>::value, "");
  static_assert(std::is_trivially_move_assignable<V>::value, "");
  test_constexpr_assign_imp<0>(V(42l), 101l);
  test_constexpr_assign_imp<0>(V(nullptr), 101l);
  test_constexpr_assign_imp<1>(V(42l), nullptr);
  test_constexpr_assign_imp<2>(V(42l), 101);
}

struct NonTrivialMoveAssign {
  int i = 0;
  constexpr NonTrivialMoveAssign(int ii) : i(ii) {}
  constexpr NonTrivialMoveAssign(const NonTrivialMoveAssign& other) = default;
  constexpr NonTrivialMoveAssign(NonTrivialMoveAssign&& other) : i(other.i) {}
  constexpr NonTrivialMoveAssign& operator=(const NonTrivialMoveAssign&) = default;
  constexpr NonTrivialMoveAssign& operator=(NonTrivialMoveAssign&& o) {
    i = o.i;
    return *this;
  }
  TEST_CONSTEXPR_CXX20 ~NonTrivialMoveAssign() = default;
  friend constexpr bool operator==(const NonTrivialMoveAssign& x, const NonTrivialMoveAssign& y) { return x.i == y.i; }
};

TEST_CONSTEXPR_CXX20 void test_constexpr_move_assignment_non_trivial() {
  using V = std::variant<long, void*, NonTrivialMoveAssign>;
  static_assert(!std::is_trivially_copyable<V>::value);
  static_assert(!std::is_trivially_move_assignable<V>::value);
  test_constexpr_assign_imp<0>(V(42l), 101l);
  test_constexpr_assign_imp<0>(V(nullptr), 101l);
  test_constexpr_assign_imp<1>(V(42l), nullptr);
  test_constexpr_assign_imp<2>(V(42l), NonTrivialMoveAssign(5));
  test_constexpr_assign_imp<2>(V(NonTrivialMoveAssign(3)), NonTrivialMoveAssign(5));
}

void non_constexpr_test() {
  test_move_assignment_empty_empty();
  test_move_assignment_non_empty_empty();
  test_move_assignment_empty_non_empty();
  test_assignment_throw();
}

constexpr bool cxx17_constexpr_test() {
  test_move_assignment_sfinae();
  test_move_assignment_noexcept();
  test_constexpr_move_assignment_trivial();

  return true;
}

TEST_CONSTEXPR_CXX20 bool cxx20_constexpr_test() {
  test_move_assignment_same_index();
  test_move_assignment_different_index();
  test_constexpr_move_assignment_non_trivial();

  return true;
}

int run_test() {
  non_constexpr_test();
  cxx17_constexpr_test();
  cxx20_constexpr_test();

  static_assert(cxx17_constexpr_test());
#if TEST_STD_VER >= 20
  static_assert(cxx20_constexpr_test());
#endif
  return 0;
}
} // namespace assign::move
// -- END: test/std/utilities/variant/variant.variant/variant.assign/move.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.assign/T.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// template <class T>
// variant& operator=(T&&) noexcept(see below);

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>
#include <memory>

#include "test_macros.h"
#include "variant_test_helpers.h"

namespace assign::T {
namespace MetaHelpers {

struct Dummy {
  Dummy() = default;
};

struct ThrowsCtorT {
  ThrowsCtorT(int) noexcept(false) {}
  ThrowsCtorT& operator=(int) noexcept { return *this; }
};

struct ThrowsAssignT {
  ThrowsAssignT(int) noexcept {}
  ThrowsAssignT& operator=(int) noexcept(false) { return *this; }
};

struct NoThrowT {
  NoThrowT(int) noexcept {}
  NoThrowT& operator=(int) noexcept { return *this; }
};

} // namespace MetaHelpers

namespace RuntimeHelpers {
#ifndef TEST_HAS_NO_EXCEPTIONS

struct ThrowsCtorT {
  int value;
  ThrowsCtorT() : value(0) {}
  ThrowsCtorT(int) noexcept(false) { throw 42; }
  ThrowsCtorT& operator=(int v) noexcept {
    value = v;
    return *this;
  }
};

struct MoveCrashes {
  int value;
  MoveCrashes(int v = 0) noexcept : value{v} {}
  MoveCrashes(MoveCrashes&&) noexcept { assert(false); }
  MoveCrashes& operator=(MoveCrashes&&) noexcept {
    assert(false);
    return *this;
  }
  MoveCrashes& operator=(int v) noexcept {
    value = v;
    return *this;
  }
};

struct ThrowsCtorTandMove {
  int value;
  ThrowsCtorTandMove() : value(0) {}
  ThrowsCtorTandMove(int) noexcept(false) { throw 42; }
  ThrowsCtorTandMove(ThrowsCtorTandMove&&) noexcept(false) { assert(false); }
  ThrowsCtorTandMove& operator=(int v) noexcept {
    value = v;
    return *this;
  }
};

struct ThrowsAssignT {
  int value;
  ThrowsAssignT() : value(0) {}
  ThrowsAssignT(int v) noexcept : value(v) {}
  ThrowsAssignT& operator=(int) noexcept(false) { throw 42; }
};

struct NoThrowT {
  int value;
  NoThrowT() : value(0) {}
  NoThrowT(int v) noexcept : value(v) {}
  NoThrowT& operator=(int v) noexcept {
    value = v;
    return *this;
  }
};

#endif // !defined(TEST_HAS_NO_EXCEPTIONS)
} // namespace RuntimeHelpers

constexpr void test_T_assignment_noexcept() {
  using namespace MetaHelpers;
  {
    using V = std::variant<Dummy, NoThrowT>;
    static_assert(std::is_nothrow_assignable<V, int>::value, "");
  }
  {
    using V = std::variant<Dummy, ThrowsCtorT>;
    static_assert(!std::is_nothrow_assignable<V, int>::value, "");
  }
  {
    using V = std::variant<Dummy, ThrowsAssignT>;
    static_assert(!std::is_nothrow_assignable<V, int>::value, "");
  }
}

constexpr void test_T_assignment_sfinae() {
  {
    using V = std::variant<long, long long>;
    static_assert(!std::is_assignable<V, int>::value, "ambiguous");
  }
  {
    using V = std::variant<std::string, std::string>;
    static_assert(!std::is_assignable<V, const char*>::value, "ambiguous");
  }
  {
    using V = std::variant<std::string, void*>;
    static_assert(!std::is_assignable<V, int>::value, "no matching operator=");
  }
  {
    using V = std::variant<std::string, float>;
    static_assert(!std::is_assignable<V, int>::value, "no matching operator=");
  }
  {
    using V = std::variant<std::unique_ptr<int>, bool>;
    static_assert(!std::is_assignable<V, std::unique_ptr<char>>::value, "no explicit bool in operator=");
    struct X {
      operator void*();
    };
#ifndef __EDG__ // TRANSITION, VSO-1327220
    static_assert(!std::is_assignable<V, X>::value, "no boolean conversion in operator=");
#endif // ^^^ no workaround ^^^
    static_assert(std::is_assignable<V, std::false_type>::value, "converted to bool in operator=");
  }
  {
    struct X {};
    struct Y {
      operator X();
    };
    using V = std::variant<X>;
    static_assert(std::is_assignable<V, Y>::value, "regression on user-defined conversions in operator=");
  }
}

TEST_CONSTEXPR_CXX20 void test_T_assignment_basic() {
  {
    std::variant<int> v(43);
    v = 42;
    assert(v.index() == 0);
    assert(std::get<0>(v) == 42);
  }
  {
    std::variant<int, long> v(43l);
    v = 42;
    assert(v.index() == 0);
    assert(std::get<0>(v) == 42);
    v = 43l;
    assert(v.index() == 1);
    assert(std::get<1>(v) == 43);
  }
  {
    std::variant<unsigned, long> v;
    v = 42;
    assert(v.index() == 1);
    assert(std::get<1>(v) == 42);
    v = 43u;
    assert(v.index() == 0);
    assert(std::get<0>(v) == 43);
  }
  {
    std::variant<std::string, bool> v = true;
    v                                 = "bar";
    assert(v.index() == 0);
    assert(std::get<0>(v) == "bar");
  }
}

void test_T_assignment_basic_no_constexpr() {
  std::variant<bool, std::unique_ptr<int>> v;
  v = nullptr;
  if constexpr (is_permissive) {
    assert(v.index() == 0);
    assert(std::get<0>(v) == false);
  } else {
    assert(v.index() == 1);
    assert(std::get<1>(v) == nullptr);
  }
}

struct TraceStat {
  int construct      = 0;
  int copy_construct = 0;
  int copy_assign    = 0;
  int move_construct = 0;
  int move_assign    = 0;
  int T_copy_assign  = 0;
  int T_move_assign  = 0;
  int destroy        = 0;
};

template <bool CtorNoexcept, bool MoveCtorNoexcept>
struct Trace {
  struct T {};

  constexpr Trace(TraceStat* s) noexcept(CtorNoexcept) : stat(s) { ++s->construct; }
  constexpr Trace(T) noexcept(CtorNoexcept) : stat(nullptr) {}
  constexpr Trace(const Trace& o) : stat(o.stat) { ++stat->copy_construct; }
  constexpr Trace(Trace&& o) noexcept(MoveCtorNoexcept) : stat(o.stat) { ++stat->move_construct; }
  constexpr Trace& operator=(const Trace&) {
    ++stat->copy_assign;
    return *this;
  }
  constexpr Trace& operator=(Trace&&) noexcept {
    ++stat->move_assign;
    return *this;
  }

  constexpr Trace& operator=(const T&) {
    ++stat->T_copy_assign;
    return *this;
  }
  constexpr Trace& operator=(T&&) noexcept {
    ++stat->T_move_assign;
    return *this;
  }
  TEST_CONSTEXPR_CXX20 ~Trace() { ++stat->destroy; }

  TraceStat* stat;
};

TEST_CONSTEXPR_CXX20 void test_T_assignment_performs_construction() {
  {
    using V = std::variant<int, Trace<false, false>>;
    TraceStat stat;
    V v{1};
    v = &stat;
    assert(stat.construct == 1);
    assert(stat.copy_construct == 0);
    assert(stat.move_construct == 0);
    assert(stat.copy_assign == 0);
    assert(stat.move_assign == 0);
    assert(stat.destroy == 0);
  }
  {
    using V = std::variant<int, Trace<false, true>>;
    TraceStat stat;
    V v{1};
    v = &stat;
    assert(stat.construct == 1);
    assert(stat.copy_construct == 0);
    assert(stat.move_construct == 1);
    assert(stat.copy_assign == 0);
    assert(stat.move_assign == 0);
    assert(stat.destroy == 1);
  }

  {
    using V = std::variant<int, Trace<true, false>>;
    TraceStat stat;
    V v{1};
    v = &stat;
    assert(stat.construct == 1);
    assert(stat.copy_construct == 0);
    assert(stat.move_construct == 0);
    assert(stat.copy_assign == 0);
    assert(stat.move_assign == 0);
    assert(stat.destroy == 0);
  }

  {
    using V = std::variant<int, Trace<true, true>>;
    TraceStat stat;
    V v{1};
    v = &stat;
    assert(stat.construct == 1);
    assert(stat.copy_construct == 0);
    assert(stat.move_construct == 0);
    assert(stat.copy_assign == 0);
    assert(stat.move_assign == 0);
    assert(stat.destroy == 0);
  }
}

TEST_CONSTEXPR_CXX20 void test_T_assignment_performs_assignment() {
  {
    using V = std::variant<int, Trace<false, false>>;
    TraceStat stat;
    V v{&stat};
    v = Trace<false, false>::T{};
    assert(stat.construct == 1);
    assert(stat.copy_construct == 0);
    assert(stat.move_construct == 0);
    assert(stat.copy_assign == 0);
    assert(stat.move_assign == 0);
    assert(stat.T_copy_assign == 0);
    assert(stat.T_move_assign == 1);
    assert(stat.destroy == 0);
  }
  {
    using V = std::variant<int, Trace<false, false>>;
    TraceStat stat;
    V v{&stat};
    Trace<false, false>::T t;
    v = t;
    assert(stat.construct == 1);
    assert(stat.copy_construct == 0);
    assert(stat.move_construct == 0);
    assert(stat.copy_assign == 0);
    assert(stat.move_assign == 0);
    assert(stat.T_copy_assign == 1);
    assert(stat.T_move_assign == 0);
    assert(stat.destroy == 0);
  }
}

void test_T_assignment_performs_construction_throw() {
  using namespace RuntimeHelpers;
#ifndef TEST_HAS_NO_EXCEPTIONS
  {
    using V = std::variant<std::string, ThrowsCtorT>;
    V v(std::in_place_type<std::string>, "hello");
    try {
      v = 42;
      assert(false);
    } catch (...) { /* ... */
    }
    assert(v.index() == 0);
    assert(std::get<0>(v) == "hello");
  }
  {
    using V = std::variant<ThrowsAssignT, std::string>;
    V v(std::in_place_type<std::string>, "hello");
    v = 42;
    assert(v.index() == 0);
    assert(std::get<0>(v).value == 42);
  }
#endif // TEST_HAS_NO_EXCEPTIONS
}

void test_T_assignment_performs_assignment_throw() {
  using namespace RuntimeHelpers;
#ifndef TEST_HAS_NO_EXCEPTIONS
  {
    using V = std::variant<ThrowsCtorT>;
    V v;
    v = 42;
    assert(v.index() == 0);
    assert(std::get<0>(v).value == 42);
  }
  {
    using V = std::variant<ThrowsCtorT, std::string>;
    V v;
    v = 42;
    assert(v.index() == 0);
    assert(std::get<0>(v).value == 42);
  }
  {
    using V = std::variant<ThrowsAssignT>;
    V v(100);
    try {
      v = 42;
      assert(false);
    } catch (...) { /* ... */
    }
    assert(v.index() == 0);
    assert(std::get<0>(v).value == 100);
  }
  {
    using V = std::variant<std::string, ThrowsAssignT>;
    V v(100);
    try {
      v = 42;
      assert(false);
    } catch (...) { /* ... */
    }
    assert(v.index() == 1);
    assert(std::get<1>(v).value == 100);
  }
#endif // TEST_HAS_NO_EXCEPTIONS
}

TEST_CONSTEXPR_CXX20 void test_T_assignment_vector_bool() {
  std::vector<bool> vec = {true};
  std::variant<bool, int> v;
  v = vec[0];
  assert(v.index() == 0);
  assert(std::get<0>(v) == true);
}

void non_constexpr_test() {
  test_T_assignment_basic_no_constexpr();
  test_T_assignment_performs_construction_throw();
  test_T_assignment_performs_assignment_throw();
}

TEST_CONSTEXPR_CXX20 bool test() {
  test_T_assignment_basic();
  test_T_assignment_performs_construction();
  test_T_assignment_performs_assignment();
  test_T_assignment_noexcept();
  test_T_assignment_sfinae();
  test_T_assignment_vector_bool();

  return true;
}

int run_test() {
  test();
  non_constexpr_test();

#if TEST_STD_VER >= 20
  static_assert(test());
#endif
  return 0;
}
} // namespace assign::T
// -- END: test/std/utilities/variant/variant.variant/variant.assign/T.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.ctor/conv.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// template <class T> constexpr variant(T&&) noexcept(see below);

#include <variant>
#include <string>
#include <memory>

#include "variant_test_helpers.h"

namespace ctor::conv {
int run_test()
{
  static_assert(!std::is_constructible<std::variant<int, int>, int>::value, "");
  static_assert(!std::is_constructible<std::variant<long, long long>, int>::value, "");
  static_assert(!std::is_constructible<std::variant<char>, int>::value, "");

  static_assert(!std::is_constructible<std::variant<std::string, float>, int>::value, "");
  static_assert(!std::is_constructible<std::variant<std::string, double>, int>::value, "");
  static_assert(!std::is_constructible<std::variant<std::string, bool>, int>::value, "");

  static_assert(!std::is_constructible<std::variant<int, bool>, decltype("meow")>::value, "");
  static_assert(!std::is_constructible<std::variant<int, const bool>, decltype("meow")>::value, "");

  static_assert(std::is_constructible<std::variant<bool>, std::true_type>::value, "");
  static_assert(!std::is_constructible<std::variant<bool>, std::unique_ptr<char> >::value, "");
  static_assert(std::is_constructible<std::variant<bool>, decltype(nullptr)>::value == is_permissive, "");

  return 0;
}
} // namespace ctor::conv
// -- END: test/std/utilities/variant/variant.variant/variant.ctor/conv.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.ctor/copy.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// constexpr variant(variant const&);

#include <cassert>
#include <type_traits>
#include <variant>

#include "test_macros.h"
#include "test_workarounds.h"

namespace ctor::copy {
struct NonT {
  constexpr NonT(int v) : value(v) {}
  constexpr NonT(const NonT& o) : value(o.value) {}
  int value;
};
static_assert(!std::is_trivially_copy_constructible<NonT>::value, "");

struct NoCopy {
  NoCopy(const NoCopy&) = delete;
};

struct MoveOnly {
  MoveOnly(const MoveOnly&) = delete;
  MoveOnly(MoveOnly&&)      = default;
};

struct MoveOnlyNT {
  MoveOnlyNT(const MoveOnlyNT&) = delete;
  MoveOnlyNT(MoveOnlyNT&&) {}
};

struct NTCopy {
  constexpr NTCopy(int v) : value(v) {}
  NTCopy(const NTCopy& that) : value(that.value) {}
  NTCopy(NTCopy&&) = delete;
  int value;
};

static_assert(!std::is_trivially_copy_constructible<NTCopy>::value, "");
static_assert(std::is_copy_constructible<NTCopy>::value, "");

struct TCopy {
  constexpr TCopy(int v) : value(v) {}
  TCopy(TCopy const&) = default;
  TCopy(TCopy&&)      = delete;
  int value;
};

static_assert(std::is_trivially_copy_constructible<TCopy>::value, "");

struct TCopyNTMove {
  constexpr TCopyNTMove(int v) : value(v) {}
  TCopyNTMove(const TCopyNTMove&) = default;
  TCopyNTMove(TCopyNTMove&& that) : value(that.value) { that.value = -1; }
  int value;
};

static_assert(std::is_trivially_copy_constructible<TCopyNTMove>::value, "");

#ifndef TEST_HAS_NO_EXCEPTIONS
struct MakeEmptyT {
  static int alive;
  MakeEmptyT() { ++alive; }
  MakeEmptyT(const MakeEmptyT&) {
    ++alive;
    // Don't throw from the copy constructor since variant's assignment
    // operator performs a copy before committing to the assignment.
  }
  MakeEmptyT(MakeEmptyT&&) { throw 42; }
  MakeEmptyT& operator=(const MakeEmptyT&) { throw 42; }
  MakeEmptyT& operator=(MakeEmptyT&&) { throw 42; }
  ~MakeEmptyT() { --alive; }
};

int MakeEmptyT::alive = 0;

template <class Variant>
void makeEmpty(Variant& v) {
  Variant v2(std::in_place_type<MakeEmptyT>);
  try {
    v = std::move(v2);
    assert(false);
  } catch (...) {
    assert(v.valueless_by_exception());
  }
}
#endif // TEST_HAS_NO_EXCEPTIONS

constexpr void test_copy_ctor_sfinae() {
  {
    using V = std::variant<int, long>;
    static_assert(std::is_copy_constructible<V>::value, "");
  }
  {
    using V = std::variant<int, NoCopy>;
    static_assert(!std::is_copy_constructible<V>::value, "");
  }
  {
    using V = std::variant<int, MoveOnly>;
    static_assert(!std::is_copy_constructible<V>::value, "");
  }
  {
    using V = std::variant<int, MoveOnlyNT>;
    static_assert(!std::is_copy_constructible<V>::value, "");
  }

  // Make sure we properly propagate triviality (see P0602R4).
  {
    using V = std::variant<int, long>;
    static_assert(std::is_trivially_copy_constructible<V>::value, "");
  }
  {
    using V = std::variant<int, NTCopy>;
    static_assert(!std::is_trivially_copy_constructible<V>::value, "");
    static_assert(std::is_copy_constructible<V>::value, "");
  }
  {
    using V = std::variant<int, TCopy>;
    static_assert(std::is_trivially_copy_constructible<V>::value, "");
  }
  {
    using V = std::variant<int, TCopyNTMove>;
    static_assert(std::is_trivially_copy_constructible<V>::value, "");
  }
}

TEST_CONSTEXPR_CXX20 void test_copy_ctor_basic() {
  {
    std::variant<int> v(std::in_place_index<0>, 42);
    std::variant<int> v2 = v;
    assert(v2.index() == 0);
    assert(std::get<0>(v2) == 42);
  }
  {
    std::variant<int, long> v(std::in_place_index<1>, 42);
    std::variant<int, long> v2 = v;
    assert(v2.index() == 1);
    assert(std::get<1>(v2) == 42);
  }
  {
    std::variant<NonT> v(std::in_place_index<0>, 42);
    assert(v.index() == 0);
    std::variant<NonT> v2(v);
    assert(v2.index() == 0);
    assert(std::get<0>(v2).value == 42);
  }
  {
    std::variant<int, NonT> v(std::in_place_index<1>, 42);
    assert(v.index() == 1);
    std::variant<int, NonT> v2(v);
    assert(v2.index() == 1);
    assert(std::get<1>(v2).value == 42);
  }

  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
  {
    constexpr std::variant<int> v(std::in_place_index<0>, 42);
    static_assert(v.index() == 0, "");
    constexpr std::variant<int> v2 = v;
    static_assert(v2.index() == 0, "");
    static_assert(std::get<0>(v2) == 42, "");
  }
  {
    constexpr std::variant<int, long> v(std::in_place_index<1>, 42);
    static_assert(v.index() == 1, "");
    constexpr std::variant<int, long> v2 = v;
    static_assert(v2.index() == 1, "");
    static_assert(std::get<1>(v2) == 42, "");
  }
  {
    constexpr std::variant<TCopy> v(std::in_place_index<0>, 42);
    static_assert(v.index() == 0, "");
    constexpr std::variant<TCopy> v2(v);
    static_assert(v2.index() == 0, "");
    static_assert(std::get<0>(v2).value == 42, "");
  }
  {
    constexpr std::variant<int, TCopy> v(std::in_place_index<1>, 42);
    static_assert(v.index() == 1, "");
    constexpr std::variant<int, TCopy> v2(v);
    static_assert(v2.index() == 1, "");
    static_assert(std::get<1>(v2).value == 42, "");
  }
  {
    constexpr std::variant<TCopyNTMove> v(std::in_place_index<0>, 42);
    static_assert(v.index() == 0, "");
    constexpr std::variant<TCopyNTMove> v2(v);
    static_assert(v2.index() == 0, "");
    static_assert(std::get<0>(v2).value == 42, "");
  }
  {
    constexpr std::variant<int, TCopyNTMove> v(std::in_place_index<1>, 42);
    static_assert(v.index() == 1, "");
    constexpr std::variant<int, TCopyNTMove> v2(v);
    static_assert(v2.index() == 1, "");
    static_assert(std::get<1>(v2).value == 42, "");
  }
}

void test_copy_ctor_valueless_by_exception() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  using V = std::variant<int, MakeEmptyT>;
  V v1;
  makeEmpty(v1);
  const V& cv1 = v1;
  V v(cv1);
  assert(v.valueless_by_exception());
#endif // TEST_HAS_NO_EXCEPTIONS
}

template <std::size_t Idx, class T>
constexpr void test_constexpr_copy_ctor_imp(const T& v) {
  auto v2 = v;
  assert(v2.index() == v.index());
  assert(v2.index() == Idx);
  assert(std::get<Idx>(v2) == std::get<Idx>(v));
}

constexpr void test_constexpr_copy_ctor_trivial() {
  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
  using V = std::variant<long, void*, const int>;
#ifdef TEST_WORKAROUND_MSVC_BROKEN_IS_TRIVIALLY_COPYABLE
  static_assert(std::is_trivially_destructible<V>::value, "");
  static_assert(std::is_trivially_copy_constructible<V>::value, "");
  static_assert(std::is_trivially_move_constructible<V>::value, "");
  static_assert(!std::is_copy_assignable<V>::value, "");
  static_assert(!std::is_move_assignable<V>::value, "");
#else  // TEST_WORKAROUND_MSVC_BROKEN_IS_TRIVIALLY_COPYABLE
  static_assert(std::is_trivially_copyable<V>::value, "");
#endif // TEST_WORKAROUND_MSVC_BROKEN_IS_TRIVIALLY_COPYABLE
  static_assert(std::is_trivially_copy_constructible<V>::value, "");
  test_constexpr_copy_ctor_imp<0>(V(42l));
  test_constexpr_copy_ctor_imp<1>(V(nullptr));
  test_constexpr_copy_ctor_imp<2>(V(101));
}

struct NonTrivialCopyCtor {
  int i = 0;
  constexpr NonTrivialCopyCtor(int ii) : i(ii) {}
  constexpr NonTrivialCopyCtor(const NonTrivialCopyCtor& other) : i(other.i) {}
  constexpr NonTrivialCopyCtor(NonTrivialCopyCtor&& other) = default;
  TEST_CONSTEXPR_CXX20 ~NonTrivialCopyCtor()               = default;
  friend constexpr bool operator==(const NonTrivialCopyCtor& x, const NonTrivialCopyCtor& y) { return x.i == y.i; }
};

TEST_CONSTEXPR_CXX20 void test_constexpr_copy_ctor_non_trivial() {
  // Test !is_trivially_move_constructible
  using V = std::variant<long, NonTrivialCopyCtor, void*>;
  static_assert(!std::is_trivially_copy_constructible<V>::value, "");
  test_constexpr_copy_ctor_imp<0>(V(42l));
  test_constexpr_copy_ctor_imp<1>(V(NonTrivialCopyCtor(5)));
  test_constexpr_copy_ctor_imp<2>(V(nullptr));
}

void non_constexpr_test() { test_copy_ctor_valueless_by_exception(); }

constexpr bool cxx17_constexpr_test() {
  test_copy_ctor_sfinae();
  test_constexpr_copy_ctor_trivial();

  return true;
}

TEST_CONSTEXPR_CXX20 bool cxx20_constexpr_test() {
  test_copy_ctor_basic();
  test_constexpr_copy_ctor_non_trivial();

  return true;
}

int run_test() {
  non_constexpr_test();
  cxx17_constexpr_test();
  cxx20_constexpr_test();

  static_assert(cxx17_constexpr_test());
#if TEST_STD_VER >= 20
  static_assert(cxx20_constexpr_test());
#endif
  return 0;
}
} // namespace ctor::copy
// -- END: test/std/utilities/variant/variant.variant/variant.ctor/copy.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.ctor/default.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// constexpr variant() noexcept(see below);

#include <cassert>
#include <type_traits>
#include <variant>
#include <string>

#include "test_macros.h"
#include "variant_test_helpers.h"

namespace ctor::default_ {
struct NonDefaultConstructible {
  constexpr NonDefaultConstructible(int) {}
};

struct NotNoexcept {
  NotNoexcept() noexcept(false) {}
};

#ifndef TEST_HAS_NO_EXCEPTIONS
struct DefaultCtorThrows {
  DefaultCtorThrows() { throw 42; }
};
#endif

constexpr void test_default_ctor_sfinae() {
  {
    using V = std::variant<std::monostate, int>;
    static_assert(std::is_default_constructible<V>::value, "");
  }
  {
    using V = std::variant<NonDefaultConstructible, int>;
    static_assert(!std::is_default_constructible<V>::value, "");
  }
}

constexpr void test_default_ctor_noexcept() {
  {
    using V = std::variant<int>;
    static_assert(std::is_nothrow_default_constructible<V>::value, "");
  }
  {
    using V = std::variant<NotNoexcept>;
    static_assert(!std::is_nothrow_default_constructible<V>::value, "");
  }
}

void test_default_ctor_throws() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  using V = std::variant<DefaultCtorThrows, int>;
  try {
    V v;
    assert(false);
  } catch (const int& ex) {
    assert(ex == 42);
  } catch (...) {
    assert(false);
  }
#endif
}

constexpr void test_default_ctor_basic() {
  {
    std::variant<int> v;
    assert(v.index() == 0);
    assert(std::get<0>(v) == 0);
  }
  {
    std::variant<int, long> v;
    assert(v.index() == 0);
    assert(std::get<0>(v) == 0);
  }
  {
    std::variant<int, NonDefaultConstructible> v;
    assert(v.index() == 0);
    assert(std::get<0>(v) == 0);
  }
  {
    using V = std::variant<int, long>;
    constexpr V v;
    static_assert(v.index() == 0, "");
    static_assert(std::get<0>(v) == 0, "");
  }
  {
    using V = std::variant<int, long>;
    constexpr V v;
    static_assert(v.index() == 0, "");
    static_assert(std::get<0>(v) == 0, "");
  }
  {
    using V = std::variant<int, NonDefaultConstructible>;
    constexpr V v;
    static_assert(v.index() == 0, "");
    static_assert(std::get<0>(v) == 0, "");
  }
}

constexpr void issue_86686() {
#if TEST_STD_VER >= 20
  static_assert(std::variant<std::string>{}.index() == 0);
#endif
}

constexpr bool test() {
  test_default_ctor_basic();
  test_default_ctor_sfinae();
  test_default_ctor_noexcept();
  issue_86686();

  return true;
}

int run_test() {
  test();
  test_default_ctor_throws();
  static_assert(test());
  return 0;
}
} // namespace ctor::default_
// -- END: test/std/utilities/variant/variant.variant/variant.ctor/default.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.ctor/in_place_index_args.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// template <size_t I, class ...Args>
// constexpr explicit variant(in_place_index_t<I>, Args&&...);

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>

#include "test_convertible.h"
#include "test_macros.h"

namespace ctor::in_place_index_args {
void test_ctor_sfinae() {
  {
    using V = std::variant<int>;
    static_assert(
        std::is_constructible<V, std::in_place_index_t<0>, int>::value, "");
    static_assert(!test_convertible<V, std::in_place_index_t<0>, int>(), "");
  }
  {
    using V = std::variant<int, long, long long>;
    static_assert(
        std::is_constructible<V, std::in_place_index_t<1>, int>::value, "");
    static_assert(!test_convertible<V, std::in_place_index_t<1>, int>(), "");
  }
  {
    using V = std::variant<int, long, int *>;
    static_assert(
        std::is_constructible<V, std::in_place_index_t<2>, int *>::value, "");
    static_assert(!test_convertible<V, std::in_place_index_t<2>, int *>(), "");
  }
  { // args not convertible to type
    using V = std::variant<int, long, int *>;
    static_assert(
        !std::is_constructible<V, std::in_place_index_t<0>, int *>::value, "");
    static_assert(!test_convertible<V, std::in_place_index_t<0>, int *>(), "");
  }
  { // index not in variant
    using V = std::variant<int, long, int *>;
    static_assert(
        !std::is_constructible<V, std::in_place_index_t<3>, int>::value, "");
    static_assert(!test_convertible<V, std::in_place_index_t<3>, int>(), "");
  }
}

void test_ctor_basic() {
  {
    constexpr std::variant<int> v(std::in_place_index<0>, 42);
    static_assert(v.index() == 0, "");
    static_assert(std::get<0>(v) == 42, "");
  }
  {
    constexpr std::variant<int, long, long> v(std::in_place_index<1>, 42);
    static_assert(v.index() == 1, "");
    static_assert(std::get<1>(v) == 42, "");
  }
  {
    constexpr std::variant<int, const int, long> v(std::in_place_index<1>, 42);
    static_assert(v.index() == 1, "");
    static_assert(std::get<1>(v) == 42, "");
  }
  {
    using V = std::variant<const int, volatile int, int>;
    int x = 42;
    V v(std::in_place_index<0>, x);
    assert(v.index() == 0);
    assert(std::get<0>(v) == x);
  }
  {
    using V = std::variant<const int, volatile int, int>;
    int x = 42;
    V v(std::in_place_index<1>, x);
    assert(v.index() == 1);
    assert(std::get<1>(v) == x);
  }
  {
    using V = std::variant<const int, volatile int, int>;
    int x = 42;
    V v(std::in_place_index<2>, x);
    assert(v.index() == 2);
    assert(std::get<2>(v) == x);
  }
}

int run_test() {
  test_ctor_basic();
  test_ctor_sfinae();

  return 0;
}
} // namespace ctor::in_place_index_args
// -- END: test/std/utilities/variant/variant.variant/variant.ctor/in_place_index_args.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.ctor/in_place_index_init_list_args.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// template <size_t I, class Up, class ...Args>
// constexpr explicit
// variant(in_place_index_t<I>, initializer_list<Up>, Args&&...);

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>

#include "test_convertible.h"
#include "test_macros.h"

namespace ctor::in_place_index_init_list_args {
struct InitList {
  std::size_t size;
  constexpr InitList(std::initializer_list<int> il) : size(il.size()) {}
};

struct InitListArg {
  std::size_t size;
  int value;
  constexpr InitListArg(std::initializer_list<int> il, int v)
      : size(il.size()), value(v) {}
};

void test_ctor_sfinae() {
  using IL = std::initializer_list<int>;
  { // just init list
    using V = std::variant<InitList, InitListArg, int>;
    static_assert(std::is_constructible<V, std::in_place_index_t<0>, IL>::value,
                  "");
    static_assert(!test_convertible<V, std::in_place_index_t<0>, IL>(), "");
  }
  { // too many arguments
    using V = std::variant<InitList, InitListArg, int>;
    static_assert(
        !std::is_constructible<V, std::in_place_index_t<0>, IL, int>::value,
        "");
    static_assert(!test_convertible<V, std::in_place_index_t<0>, IL, int>(),
                  "");
  }
  { // too few arguments
    using V = std::variant<InitList, InitListArg, int>;
    static_assert(
        !std::is_constructible<V, std::in_place_index_t<1>, IL>::value, "");
    static_assert(!test_convertible<V, std::in_place_index_t<1>, IL>(), "");
  }
  { // init list and arguments
    using V = std::variant<InitList, InitListArg, int>;
    static_assert(
        std::is_constructible<V, std::in_place_index_t<1>, IL, int>::value, "");
    static_assert(!test_convertible<V, std::in_place_index_t<1>, IL, int>(),
                  "");
  }
  { // not constructible from arguments
    using V = std::variant<InitList, InitListArg, int>;
    static_assert(
        !std::is_constructible<V, std::in_place_index_t<2>, IL>::value, "");
    static_assert(!test_convertible<V, std::in_place_index_t<2>, IL>(), "");
  }
  { // index not in variant
    using V = std::variant<InitList, InitListArg, int>;
    static_assert(
        !std::is_constructible<V, std::in_place_index_t<3>, IL>::value, "");
    static_assert(!test_convertible<V, std::in_place_index_t<3>, IL>(), "");
  }
}

void test_ctor_basic() {
  {
    constexpr std::variant<InitList, InitListArg, InitList> v(
        std::in_place_index<0>, {1, 2, 3});
    static_assert(v.index() == 0, "");
    static_assert(std::get<0>(v).size == 3, "");
  }
  {
    constexpr std::variant<InitList, InitListArg, InitList> v(
        std::in_place_index<2>, {1, 2, 3});
    static_assert(v.index() == 2, "");
    static_assert(std::get<2>(v).size == 3, "");
  }
  {
    constexpr std::variant<InitList, InitListArg, InitListArg> v(
        std::in_place_index<1>, {1, 2, 3, 4}, 42);
    static_assert(v.index() == 1, "");
    static_assert(std::get<1>(v).size == 4, "");
    static_assert(std::get<1>(v).value == 42, "");
  }
}

int run_test() {
  test_ctor_basic();
  test_ctor_sfinae();

  return 0;
}
} // namespace ctor::in_place_index_init_list_args
// -- END: test/std/utilities/variant/variant.variant/variant.ctor/in_place_index_init_list_args.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.ctor/in_place_type_args.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// template <class Tp, class ...Args>
// constexpr explicit variant(in_place_type_t<Tp>, Args&&...);

#include <cassert>
#include <type_traits>
#include <variant>

#include "test_convertible.h"
#include "test_macros.h"

namespace ctor::in_place_type_args {
void test_ctor_sfinae() {
  {
    using V = std::variant<int>;
    static_assert(
        std::is_constructible<V, std::in_place_type_t<int>, int>::value, "");
    static_assert(!test_convertible<V, std::in_place_type_t<int>, int>(), "");
  }
  {
    using V = std::variant<int, long, long long>;
    static_assert(
        std::is_constructible<V, std::in_place_type_t<long>, int>::value, "");
    static_assert(!test_convertible<V, std::in_place_type_t<long>, int>(), "");
  }
  {
    using V = std::variant<int, long, int *>;
    static_assert(
        std::is_constructible<V, std::in_place_type_t<int *>, int *>::value,
        "");
    static_assert(!test_convertible<V, std::in_place_type_t<int *>, int *>(),
                  "");
  }
  { // duplicate type
    using V = std::variant<int, long, int>;
    static_assert(
        !std::is_constructible<V, std::in_place_type_t<int>, int>::value, "");
    static_assert(!test_convertible<V, std::in_place_type_t<int>, int>(), "");
  }
  { // args not convertible to type
    using V = std::variant<int, long, int *>;
    static_assert(
        !std::is_constructible<V, std::in_place_type_t<int>, int *>::value, "");
    static_assert(!test_convertible<V, std::in_place_type_t<int>, int *>(), "");
  }
  { // type not in variant
    using V = std::variant<int, long, int *>;
    static_assert(
        !std::is_constructible<V, std::in_place_type_t<long long>, int>::value,
        "");
    static_assert(!test_convertible<V, std::in_place_type_t<long long>, int>(),
                  "");
  }
}

void test_ctor_basic() {
  {
    constexpr std::variant<int> v(std::in_place_type<int>, 42);
    static_assert(v.index() == 0, "");
    static_assert(std::get<0>(v) == 42, "");
  }
  {
    constexpr std::variant<int, long> v(std::in_place_type<long>, 42);
    static_assert(v.index() == 1, "");
    static_assert(std::get<1>(v) == 42, "");
  }
  {
    constexpr std::variant<int, const int, long> v(
        std::in_place_type<const int>, 42);
    static_assert(v.index() == 1, "");
    static_assert(std::get<1>(v) == 42, "");
  }
  {
    using V = std::variant<const int, volatile int, int>;
    int x = 42;
    V v(std::in_place_type<const int>, x);
    assert(v.index() == 0);
    assert(std::get<0>(v) == x);
  }
  {
    using V = std::variant<const int, volatile int, int>;
    int x = 42;
    V v(std::in_place_type<volatile int>, x);
    assert(v.index() == 1);
    assert(std::get<1>(v) == x);
  }
  {
    using V = std::variant<const int, volatile int, int>;
    int x = 42;
    V v(std::in_place_type<int>, x);
    assert(v.index() == 2);
    assert(std::get<2>(v) == x);
  }
}

int run_test() {
  test_ctor_basic();
  test_ctor_sfinae();

  return 0;
}
} // namespace ctor::in_place_type_args
// -- END: test/std/utilities/variant/variant.variant/variant.ctor/in_place_type_args.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.ctor/in_place_type_init_list_args.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// template <class Tp, class Up, class ...Args>
// constexpr explicit
// variant(in_place_type_t<Tp>, initializer_list<Up>, Args&&...);

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>

#include "test_convertible.h"
#include "test_macros.h"

namespace ctor::in_place_type_init_list_args {
struct InitList {
  std::size_t size;
  constexpr InitList(std::initializer_list<int> il) : size(il.size()) {}
};

struct InitListArg {
  std::size_t size;
  int value;
  constexpr InitListArg(std::initializer_list<int> il, int v)
      : size(il.size()), value(v) {}
};

void test_ctor_sfinae() {
  using IL = std::initializer_list<int>;
  { // just init list
    using V = std::variant<InitList, InitListArg, int>;
    static_assert(
        std::is_constructible<V, std::in_place_type_t<InitList>, IL>::value,
        "");
    static_assert(!test_convertible<V, std::in_place_type_t<InitList>, IL>(),
                  "");
  }
  { // too many arguments
    using V = std::variant<InitList, InitListArg, int>;
    static_assert(!std::is_constructible<V, std::in_place_type_t<InitList>, IL,
                                         int>::value,
                  "");
    static_assert(
        !test_convertible<V, std::in_place_type_t<InitList>, IL, int>(), "");
  }
  { // too few arguments
    using V = std::variant<InitList, InitListArg, int>;
    static_assert(
        !std::is_constructible<V, std::in_place_type_t<InitListArg>, IL>::value,
        "");
    static_assert(!test_convertible<V, std::in_place_type_t<InitListArg>, IL>(),
                  "");
  }
  { // init list and arguments
    using V = std::variant<InitList, InitListArg, int>;
    static_assert(std::is_constructible<V, std::in_place_type_t<InitListArg>,
                                        IL, int>::value,
                  "");
    static_assert(
        !test_convertible<V, std::in_place_type_t<InitListArg>, IL, int>(), "");
  }
  { // not constructible from arguments
    using V = std::variant<InitList, InitListArg, int>;
    static_assert(
        !std::is_constructible<V, std::in_place_type_t<int>, IL>::value, "");
    static_assert(!test_convertible<V, std::in_place_type_t<int>, IL>(), "");
  }
  { // duplicate types in variant
    using V = std::variant<InitListArg, InitListArg, int>;
    static_assert(!std::is_constructible<V, std::in_place_type_t<InitListArg>,
                                         IL, int>::value,
                  "");
    static_assert(
        !test_convertible<V, std::in_place_type_t<InitListArg>, IL, int>(), "");
  }
}

void test_ctor_basic() {
  {
    constexpr std::variant<InitList, InitListArg> v(
        std::in_place_type<InitList>, {1, 2, 3});
    static_assert(v.index() == 0, "");
    static_assert(std::get<0>(v).size == 3, "");
  }
  {
    constexpr std::variant<InitList, InitListArg> v(
        std::in_place_type<InitListArg>, {1, 2, 3, 4}, 42);
    static_assert(v.index() == 1, "");
    static_assert(std::get<1>(v).size == 4, "");
    static_assert(std::get<1>(v).value == 42, "");
  }
}

int run_test() {
  test_ctor_basic();
  test_ctor_sfinae();

  return 0;
}
} // namespace ctor::in_place_type_init_list_args
// -- END: test/std/utilities/variant/variant.variant/variant.ctor/in_place_type_init_list_args.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.ctor/move.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// constexpr variant(variant&&) noexcept(see below);

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>

#include "test_macros.h"
#include "test_workarounds.h"

namespace ctor::move {
struct ThrowsMove {
  ThrowsMove(ThrowsMove&&) noexcept(false) {}
};

struct NoCopy {
  NoCopy(const NoCopy&) = delete;
};

struct MoveOnly {
  int value;
  constexpr MoveOnly(int v) : value(v) {}
  MoveOnly(const MoveOnly&) = delete;
  MoveOnly(MoveOnly&&)      = default;
};

struct MoveOnlyNT {
  int value;
  constexpr MoveOnlyNT(int v) : value(v) {}
  MoveOnlyNT(const MoveOnlyNT&) = delete;
  constexpr MoveOnlyNT(MoveOnlyNT&& other) : value(other.value) { other.value = -1; }
};

struct NTMove {
  constexpr NTMove(int v) : value(v) {}
  NTMove(const NTMove&) = delete;
  NTMove(NTMove&& that) : value(that.value) { that.value = -1; }
  int value;
};

static_assert(!std::is_trivially_move_constructible<NTMove>::value, "");
static_assert(std::is_move_constructible<NTMove>::value, "");

struct TMove {
  constexpr TMove(int v) : value(v) {}
  TMove(const TMove&) = delete;
  TMove(TMove&&)      = default;
  int value;
};

static_assert(std::is_trivially_move_constructible<TMove>::value, "");

struct TMoveNTCopy {
  constexpr TMoveNTCopy(int v) : value(v) {}
  TMoveNTCopy(const TMoveNTCopy& that) : value(that.value) {}
  TMoveNTCopy(TMoveNTCopy&&) = default;
  int value;
};

static_assert(std::is_trivially_move_constructible<TMoveNTCopy>::value, "");

#ifndef TEST_HAS_NO_EXCEPTIONS
struct MakeEmptyT {
  static int alive;
  MakeEmptyT() { ++alive; }
  MakeEmptyT(const MakeEmptyT&) {
    ++alive;
    // Don't throw from the copy constructor since variant's assignment
    // operator performs a copy before committing to the assignment.
  }
  MakeEmptyT(MakeEmptyT&&) { throw 42; }
  MakeEmptyT& operator=(const MakeEmptyT&) { throw 42; }
  MakeEmptyT& operator=(MakeEmptyT&&) { throw 42; }
  ~MakeEmptyT() { --alive; }
};

int MakeEmptyT::alive = 0;

template <class Variant>
void makeEmpty(Variant& v) {
  Variant v2(std::in_place_type<MakeEmptyT>);
  try {
    v = std::move(v2);
    assert(false);
  } catch (...) {
    assert(v.valueless_by_exception());
  }
}
#endif // TEST_HAS_NO_EXCEPTIONS

constexpr void test_move_noexcept() {
  {
    using V = std::variant<int, long>;
    static_assert(std::is_nothrow_move_constructible<V>::value, "");
  }
  {
    using V = std::variant<int, MoveOnly>;
    static_assert(std::is_nothrow_move_constructible<V>::value, "");
  }
  {
    using V = std::variant<int, MoveOnlyNT>;
    static_assert(!std::is_nothrow_move_constructible<V>::value, "");
  }
  {
    using V = std::variant<int, ThrowsMove>;
    static_assert(!std::is_nothrow_move_constructible<V>::value, "");
  }
}

constexpr void test_move_ctor_sfinae() {
  {
    using V = std::variant<int, long>;
    static_assert(std::is_move_constructible<V>::value, "");
  }
  {
    using V = std::variant<int, MoveOnly>;
    static_assert(std::is_move_constructible<V>::value, "");
  }
  {
    using V = std::variant<int, MoveOnlyNT>;
    static_assert(std::is_move_constructible<V>::value, "");
  }
  {
    using V = std::variant<int, NoCopy>;
    static_assert(!std::is_move_constructible<V>::value, "");
  }

  // Make sure we properly propagate triviality (see P0602R4).
  {
    using V = std::variant<int, long>;
    static_assert(std::is_trivially_move_constructible<V>::value, "");
  }
  {
    using V = std::variant<int, NTMove>;
    static_assert(!std::is_trivially_move_constructible<V>::value, "");
    static_assert(std::is_move_constructible<V>::value, "");
  }
  {
    using V = std::variant<int, TMove>;
    static_assert(std::is_trivially_move_constructible<V>::value, "");
  }
  {
    using V = std::variant<int, TMoveNTCopy>;
    static_assert(std::is_trivially_move_constructible<V>::value, "");
  }
}

template <typename T>
struct Result {
  std::size_t index;
  T value;
};

TEST_CONSTEXPR_CXX20 void test_move_ctor_basic() {
  {
    std::variant<int> v(std::in_place_index<0>, 42);
    std::variant<int> v2 = std::move(v);
    assert(v2.index() == 0);
    assert(std::get<0>(v2) == 42);
  }
  {
    std::variant<int, long> v(std::in_place_index<1>, 42);
    std::variant<int, long> v2 = std::move(v);
    assert(v2.index() == 1);
    assert(std::get<1>(v2) == 42);
  }
  {
    std::variant<MoveOnly> v(std::in_place_index<0>, 42);
    assert(v.index() == 0);
    std::variant<MoveOnly> v2(std::move(v));
    assert(v2.index() == 0);
    assert(std::get<0>(v2).value == 42);
  }
  {
    std::variant<int, MoveOnly> v(std::in_place_index<1>, 42);
    assert(v.index() == 1);
    std::variant<int, MoveOnly> v2(std::move(v));
    assert(v2.index() == 1);
    assert(std::get<1>(v2).value == 42);
  }
  {
    std::variant<MoveOnlyNT> v(std::in_place_index<0>, 42);
    assert(v.index() == 0);
    std::variant<MoveOnlyNT> v2(std::move(v));
    assert(v2.index() == 0);
    assert(std::get<0>(v).value == -1);
    assert(std::get<0>(v2).value == 42);
  }
  {
    std::variant<int, MoveOnlyNT> v(std::in_place_index<1>, 42);
    assert(v.index() == 1);
    std::variant<int, MoveOnlyNT> v2(std::move(v));
    assert(v2.index() == 1);
    assert(std::get<1>(v).value == -1);
    assert(std::get<1>(v2).value == 42);
  }

  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
  {
    struct {
      constexpr Result<int> operator()() const {
        std::variant<int> v(std::in_place_index<0>, 42);
        std::variant<int> v2 = std::move(v);
        return {v2.index(), std::get<0>(std::move(v2))};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 0, "");
    static_assert(result.value == 42, "");
  }
  {
    struct {
      constexpr Result<long> operator()() const {
        std::variant<int, long> v(std::in_place_index<1>, 42);
        std::variant<int, long> v2 = std::move(v);
        return {v2.index(), std::get<1>(std::move(v2))};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 1, "");
    static_assert(result.value == 42, "");
  }
  {
    struct {
      constexpr Result<TMove> operator()() const {
        std::variant<TMove> v(std::in_place_index<0>, 42);
        std::variant<TMove> v2(std::move(v));
        return {v2.index(), std::get<0>(std::move(v2))};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 0, "");
    static_assert(result.value.value == 42, "");
  }
  {
    struct {
      constexpr Result<TMove> operator()() const {
        std::variant<int, TMove> v(std::in_place_index<1>, 42);
        std::variant<int, TMove> v2(std::move(v));
        return {v2.index(), std::get<1>(std::move(v2))};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 1, "");
    static_assert(result.value.value == 42, "");
  }
  {
    struct {
      constexpr Result<TMoveNTCopy> operator()() const {
        std::variant<TMoveNTCopy> v(std::in_place_index<0>, 42);
        std::variant<TMoveNTCopy> v2(std::move(v));
        return {v2.index(), std::get<0>(std::move(v2))};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 0, "");
    static_assert(result.value.value == 42, "");
  }
  {
    struct {
      constexpr Result<TMoveNTCopy> operator()() const {
        std::variant<int, TMoveNTCopy> v(std::in_place_index<1>, 42);
        std::variant<int, TMoveNTCopy> v2(std::move(v));
        return {v2.index(), std::get<1>(std::move(v2))};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 1, "");
    static_assert(result.value.value == 42, "");
  }
}

void test_move_ctor_valueless_by_exception() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  using V = std::variant<int, MakeEmptyT>;
  V v1;
  makeEmpty(v1);
  V v(std::move(v1));
  assert(v.valueless_by_exception());
#endif // TEST_HAS_NO_EXCEPTIONS
}

template <std::size_t Idx, class T>
constexpr void test_constexpr_ctor_imp(const T& v) {
  auto copy = v;
  auto v2   = std::move(copy);
  assert(v2.index() == v.index());
  assert(v2.index() == Idx);
  assert(std::get<Idx>(v2) == std::get<Idx>(v));
}

constexpr void test_constexpr_move_ctor_trivial() {
  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
  using V = std::variant<long, void*, const int>;
#ifdef TEST_WORKAROUND_MSVC_BROKEN_IS_TRIVIALLY_COPYABLE
  static_assert(std::is_trivially_destructible<V>::value, "");
  static_assert(std::is_trivially_copy_constructible<V>::value, "");
  static_assert(std::is_trivially_move_constructible<V>::value, "");
  static_assert(!std::is_copy_assignable<V>::value, "");
  static_assert(!std::is_move_assignable<V>::value, "");
#else  // TEST_WORKAROUND_MSVC_BROKEN_IS_TRIVIALLY_COPYABLE
  static_assert(std::is_trivially_copyable<V>::value, "");
#endif // TEST_WORKAROUND_MSVC_BROKEN_IS_TRIVIALLY_COPYABLE
  static_assert(std::is_trivially_move_constructible<V>::value, "");
  test_constexpr_ctor_imp<0>(V(42l));
  test_constexpr_ctor_imp<1>(V(nullptr));
  test_constexpr_ctor_imp<2>(V(101));
}

struct NonTrivialMoveCtor {
  int i = 0;
  constexpr NonTrivialMoveCtor(int ii) : i(ii) {}
  constexpr NonTrivialMoveCtor(const NonTrivialMoveCtor& other) = default;
  constexpr NonTrivialMoveCtor(NonTrivialMoveCtor&& other) : i(other.i) {}
  TEST_CONSTEXPR_CXX20 ~NonTrivialMoveCtor() = default;
  friend constexpr bool operator==(const NonTrivialMoveCtor& x, const NonTrivialMoveCtor& y) { return x.i == y.i; }
};

TEST_CONSTEXPR_CXX20 void test_constexpr_move_ctor_non_trivial() {
  using V = std::variant<long, NonTrivialMoveCtor, void*>;
  static_assert(!std::is_trivially_move_constructible<V>::value, "");
  test_constexpr_ctor_imp<0>(V(42l));
  test_constexpr_ctor_imp<1>(V(NonTrivialMoveCtor(5)));
  test_constexpr_ctor_imp<2>(V(nullptr));
}

void non_constexpr_test() { test_move_ctor_valueless_by_exception(); }

constexpr bool cxx17_constexpr_test() {
  test_move_noexcept();
  test_move_ctor_sfinae();
  test_constexpr_move_ctor_trivial();

  return true;
}

TEST_CONSTEXPR_CXX20 bool cxx20_constexpr_test() {
  test_move_ctor_basic();
  test_constexpr_move_ctor_non_trivial();

  return true;
}

int run_test() {
  non_constexpr_test();
  cxx17_constexpr_test();
  cxx20_constexpr_test();

  static_assert(cxx17_constexpr_test());
#if TEST_STD_VER >= 20
  static_assert(cxx20_constexpr_test());
#endif

  return 0;
}
} // namespace ctor::move
// -- END: test/std/utilities/variant/variant.variant/variant.ctor/move.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.ctor/T.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// template <class T> constexpr variant(T&&) noexcept(see below);

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>
#include <memory>
#include <vector>

#include "test_macros.h"
#include "variant_test_helpers.h"

namespace ctor::T {
struct Dummy {
  Dummy() = default;
};

struct ThrowsT {
  ThrowsT(int) noexcept(false) {}
};

struct NoThrowT {
  NoThrowT(int) noexcept(true) {}
};

struct AnyConstructible {
  template <typename T>
  AnyConstructible(T&&) {}
};
struct NoConstructible {
  NoConstructible() = delete;
};
template <class T>
struct RValueConvertibleFrom {
  RValueConvertibleFrom(T&&) {}
};

void test_T_ctor_noexcept() {
  {
    using V = std::variant<Dummy, NoThrowT>;
    static_assert(std::is_nothrow_constructible<V, int>::value, "");
  }
  {
    using V = std::variant<Dummy, ThrowsT>;
    static_assert(!std::is_nothrow_constructible<V, int>::value, "");
  }
}

void test_T_ctor_sfinae() {
  {
    using V = std::variant<long, long long>;
    static_assert(!std::is_constructible<V, int>::value, "ambiguous");
  }
  {
    using V = std::variant<std::string, std::string>;
    static_assert(!std::is_constructible<V, const char*>::value, "ambiguous");
  }
  {
    using V = std::variant<std::string, void*>;
    static_assert(!std::is_constructible<V, int>::value, "no matching constructor");
  }
  {
    using V = std::variant<std::string, float>;
    static_assert(!std::is_constructible<V, int>::value, "no matching constructor");
  }
  {
    using V = std::variant<std::unique_ptr<int>, bool>;
    static_assert(!std::is_constructible<V, std::unique_ptr<char>>::value, "no explicit bool in constructor");
    struct X {
      operator void*();
    };
#ifndef __EDG__ // TRANSITION, VSO-1327220
    static_assert(!std::is_constructible<V, X>::value, "no boolean conversion in constructor");
#endif // ^^^ no workaround ^^^
    static_assert(std::is_constructible<V, std::false_type>::value, "converted to bool in constructor");
  }
  {
    struct X {};
    struct Y {
      operator X();
    };
    using V = std::variant<X>;
    static_assert(std::is_constructible<V, Y>::value, "regression on user-defined conversions in constructor");
  }
  {
    using V = std::variant<AnyConstructible, NoConstructible>;
    static_assert(!std::is_constructible<V, std::in_place_type_t<NoConstructible>>::value, "no matching constructor");
    static_assert(!std::is_constructible<V, std::in_place_index_t<1>>::value, "no matching constructor");
  }
}

void test_T_ctor_basic() {
  {
    constexpr std::variant<int> v(42);
    static_assert(v.index() == 0, "");
    static_assert(std::get<0>(v) == 42, "");
  }
  {
    constexpr std::variant<int, long> v(42l);
    static_assert(v.index() == 1, "");
    static_assert(std::get<1>(v) == 42, "");
  }
  {
    constexpr std::variant<unsigned, long> v(42);
    static_assert(v.index() == 1, "");
    static_assert(std::get<1>(v) == 42, "");
  }
  {
    std::variant<std::string, bool const> v = "foo";
    assert(v.index() == 0);
    assert(std::get<0>(v) == "foo");
  }
  {
    std::variant<bool, std::unique_ptr<int>> v = nullptr;
    if constexpr (is_permissive) {
      assert(v.index() == 0);
      assert(std::get<0>(v) == false);
    } else {
      assert(v.index() == 1);
      assert(std::get<1>(v) == nullptr);
    }
  }
  {
    std::variant<bool const, int> v = true;
    assert(v.index() == 0);
    assert(std::get<0>(v));
  }
  {
    std::variant<RValueConvertibleFrom<int>> v1 = 42;
    assert(v1.index() == 0);

    int x                                                         = 42;
    std::variant<RValueConvertibleFrom<int>, AnyConstructible> v2 = x;
    assert(v2.index() == 1);
  }
}

struct BoomOnAnything {
  template <class T>
  constexpr BoomOnAnything(T) {
    static_assert(!std::is_same<T, T>::value, "");
  }
};

void test_no_narrowing_check_for_class_types() {
#ifndef __clang__ // TRANSITION, not yet investigated
  using V = std::variant<int, BoomOnAnything>;
  V v(42);
  assert(v.index() == 0);
  assert(std::get<0>(v) == 42);
#endif // ^^^ no workaround ^^^
}

struct Bar {};
struct Baz {};
void test_construction_with_repeated_types() {
  using V = std::variant<int, Bar, Baz, int, Baz, int, int>;
  static_assert(!std::is_constructible<V, int>::value, "");
  static_assert(!std::is_constructible<V, Baz>::value, "");
  // OK, the selected type appears only once and so it shouldn't
  // be affected by the duplicate types.
  static_assert(std::is_constructible<V, Bar>::value, "");
}

void test_vector_bool() {
  std::vector<bool> vec = {true};
  std::variant<bool, int> v = vec[0];
  assert(v.index() == 0);
  assert(std::get<0>(v) == true);
}

int run_test() {
  test_T_ctor_basic();
  test_T_ctor_noexcept();
  test_T_ctor_sfinae();
  test_no_narrowing_check_for_class_types();
  test_construction_with_repeated_types();
  test_vector_bool();
  return 0;
}
} // namespace ctor::T
// -- END: test/std/utilities/variant/variant.variant/variant.ctor/T.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.dtor/dtor.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// ~variant();

#include <cassert>
#include <type_traits>
#include <variant>

#include "test_macros.h"

namespace dtor {
struct NonTDtor {
  int* count;
  constexpr NonTDtor(int* a, int*) : count(a) {}
  TEST_CONSTEXPR_CXX20 ~NonTDtor() { ++*count; }
};
static_assert(!std::is_trivially_destructible<NonTDtor>::value, "");

struct NonTDtor1 {
  int* count;
  constexpr NonTDtor1(int*, int* b) : count(b) {}
  TEST_CONSTEXPR_CXX20 ~NonTDtor1() { ++*count; }
};
static_assert(!std::is_trivially_destructible<NonTDtor1>::value, "");

struct TDtor {
  constexpr TDtor() = default;
  constexpr TDtor(const TDtor&) {} // non-trivial copy
  TEST_CONSTEXPR_CXX20 ~TDtor() = default;
};
static_assert(!std::is_trivially_copy_constructible<TDtor>::value, "");
static_assert(std::is_trivially_destructible<TDtor>::value, "");

TEST_CONSTEXPR_CXX20 bool test() {
  {
    using V = std::variant<int, long, TDtor>;
    static_assert(std::is_trivially_destructible<V>::value, "");
    [[maybe_unused]] V v(std::in_place_index<2>);
  }
  {
    using V = std::variant<NonTDtor, int, NonTDtor1>;
    static_assert(!std::is_trivially_destructible<V>::value, "");
    {
      int count0 = 0;
      int count1 = 0;
      {
        V v(std::in_place_index<0>, &count0, &count1);
        assert(count0 == 0);
        assert(count1 == 0);
      }
      assert(count0 == 1);
      assert(count1 == 0);
    }
    {
      int count0 = 0;
      int count1 = 0;
      { V v(std::in_place_index<1>); }
      assert(count0 == 0);
      assert(count1 == 0);
    }
    {
      int count0 = 0;
      int count1 = 0;
      {
        V v(std::in_place_index<2>, &count0, &count1);
        assert(count0 == 0);
        assert(count1 == 0);
      }
      assert(count0 == 0);
      assert(count1 == 1);
    }
  }

  return true;
}

int run_test() {
  test();

#if TEST_STD_VER >= 20
  static_assert(test());
#endif

  return 0;
}
} // namespace dtor
// -- END: test/std/utilities/variant/variant.variant/variant.dtor/dtor.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.mod/emplace_index_args.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// template <size_t I, class ...Args>
//   variant_alternative_t<I, variant<Types...>>& emplace(Args&&... args);

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>

#include "archetypes.h"
#include "test_convertible.h"
#include "test_macros.h"
#include "variant_test_helpers.h"

namespace emplace::index_args {
template <class Var, std::size_t I, class... Args>
constexpr auto test_emplace_exists_imp(int)
    -> decltype(std::declval<Var>().template emplace<I>(std::declval<Args>()...), true) {
  return true;
}

template <class, std::size_t, class...>
constexpr auto test_emplace_exists_imp(long) -> bool {
  return false;
}

template <class Var, std::size_t I, class... Args>
constexpr bool emplace_exists() {
  return test_emplace_exists_imp<Var, I, Args...>(0);
}

constexpr void test_emplace_sfinae() {
  {
    using V = std::variant<int, void*, const void*, TestTypes::NoCtors>;
    static_assert(emplace_exists<V, 0>(), "");
    static_assert(emplace_exists<V, 0, int>(), "");
    static_assert(!emplace_exists<V, 0, decltype(nullptr)>(), "cannot construct");
    static_assert(emplace_exists<V, 1, decltype(nullptr)>(), "");
    static_assert(emplace_exists<V, 1, int*>(), "");
    static_assert(!emplace_exists<V, 1, const int*>(), "");
    static_assert(!emplace_exists<V, 1, int>(), "cannot construct");
    static_assert(emplace_exists<V, 2, const int*>(), "");
    static_assert(emplace_exists<V, 2, int*>(), "");
    static_assert(!emplace_exists<V, 3>(), "cannot construct");
  }
}

struct NoCtor {
  NoCtor() = delete;
};

TEST_CONSTEXPR_CXX20 void test_basic() {
  {
    using V = std::variant<int>;
    V v(42);
    auto& ref1 = v.emplace<0>();
    static_assert(std::is_same_v<int&, decltype(ref1)>, "");
    assert(std::get<0>(v) == 0);
    assert(&ref1 == &std::get<0>(v));
    auto& ref2 = v.emplace<0>(42);
    static_assert(std::is_same_v<int&, decltype(ref2)>, "");
    assert(std::get<0>(v) == 42);
    assert(&ref2 == &std::get<0>(v));
  }

  {
    using V     = std::variant<int, long, const void*, NoCtor, std::string>;
    const int x = 100;
    V v(std::in_place_index<0>, -1);
    // default emplace a value
    auto& ref1 = v.emplace<1>();
    static_assert(std::is_same_v<long&, decltype(ref1)>, "");
    assert(std::get<1>(v) == 0);
    assert(&ref1 == &std::get<1>(v));
    auto& ref2 = v.emplace<2>(&x);
    static_assert(std::is_same_v<const void*&, decltype(ref2)>, "");
    assert(std::get<2>(v) == &x);
    assert(&ref2 == &std::get<2>(v));
    // emplace with multiple args
    auto& ref3 = v.emplace<4>(3u, 'a');
    static_assert(std::is_same_v<std::string&, decltype(ref3)>, "");
    assert(std::get<4>(v) == "aaa");
    assert(&ref3 == &std::get<4>(v));
  }
}

TEST_CONSTEXPR_CXX20 bool test() {
  test_basic();
  test_emplace_sfinae();

  return true;
}

int run_test() {
  test();

#if TEST_STD_VER >= 20
  static_assert(test());
#endif

  return 0;
}
} // namespace emplace::index_args
// -- END: test/std/utilities/variant/variant.variant/variant.mod/emplace_index_args.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.mod/emplace_index_init_list_args.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// template <size_t I, class U, class ...Args>
//   variant_alternative_t<I, variant<Types...>>& emplace(initializer_list<U> il,Args&&... args);

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>

#include "archetypes.h"
#include "test_convertible.h"
#include "test_macros.h"

namespace emplace::index_init_list_args {
struct InitList {
  std::size_t size;
  constexpr InitList(std::initializer_list<int> il) : size(il.size()) {}
};

struct InitListArg {
  std::size_t size;
  int value;
  constexpr InitListArg(std::initializer_list<int> il, int v) : size(il.size()), value(v) {}
};

template <class Var, std::size_t I, class... Args>
constexpr auto test_emplace_exists_imp(int)
    -> decltype(std::declval<Var>().template emplace<I>(std::declval<Args>()...), true) {
  return true;
}

template <class, std::size_t, class...>
constexpr auto test_emplace_exists_imp(long) -> bool {
  return false;
}

template <class Var, std::size_t I, class... Args>
constexpr bool emplace_exists() {
  return test_emplace_exists_imp<Var, I, Args...>(0);
}

constexpr void test_emplace_sfinae() {
  using V  = std::variant<int, TestTypes::NoCtors, InitList, InitListArg, long, long>;
  using IL = std::initializer_list<int>;
  static_assert(!emplace_exists<V, 1, IL>(), "no such constructor");
  static_assert(emplace_exists<V, 2, IL>(), "");
  static_assert(!emplace_exists<V, 2, int>(), "args don't match");
  static_assert(!emplace_exists<V, 2, IL, int>(), "too many args");
  static_assert(emplace_exists<V, 3, IL, int>(), "");
  static_assert(!emplace_exists<V, 3, int>(), "args don't match");
  static_assert(!emplace_exists<V, 3, IL>(), "too few args");
  static_assert(!emplace_exists<V, 3, IL, int, int>(), "too many args");
}

struct NoCtor {
  NoCtor() = delete;
};

TEST_CONSTEXPR_CXX20 void test_basic() {
  using V = std::variant<int, InitList, InitListArg, NoCtor>;
  V v;
  auto& ref1 = v.emplace<1>({1, 2, 3});
  static_assert(std::is_same_v<InitList&, decltype(ref1)>, "");
  assert(std::get<1>(v).size == 3);
  assert(&ref1 == &std::get<1>(v));
  auto& ref2 = v.emplace<2>({1, 2, 3, 4}, 42);
  static_assert(std::is_same_v<InitListArg&, decltype(ref2)>, "");
  assert(std::get<2>(v).size == 4);
  assert(std::get<2>(v).value == 42);
  assert(&ref2 == &std::get<2>(v));
  auto& ref3 = v.emplace<1>({1});
  static_assert(std::is_same_v<InitList&, decltype(ref3)>, "");
  assert(std::get<1>(v).size == 1);
  assert(&ref3 == &std::get<1>(v));
}

TEST_CONSTEXPR_CXX20 bool test() {
  test_basic();
  test_emplace_sfinae();

  return true;
}

int run_test() {
  test();

#if TEST_STD_VER >= 20
  static_assert(test());
#endif

  return 0;
}
} // namespace emplace::index_init_list_args
// -- END: test/std/utilities/variant/variant.variant/variant.mod/emplace_index_init_list_args.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.mod/emplace_type_args.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// template <class T, class ...Args> T& emplace(Args&&... args);

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>

#include "archetypes.h"
#include "test_convertible.h"
#include "test_macros.h"
#include "variant_test_helpers.h"

namespace emplace::type_args {
template <class Var, class T, class... Args>
constexpr auto test_emplace_exists_imp(int)
    -> decltype(std::declval<Var>().template emplace<T>(std::declval<Args>()...), true) {
  return true;
}

template <class, class, class...>
constexpr auto test_emplace_exists_imp(long) -> bool {
  return false;
}

template <class... Args>
constexpr bool emplace_exists() {
  return test_emplace_exists_imp<Args...>(0);
}

constexpr void test_emplace_sfinae() {
  {
    using V = std::variant<int, void*, const void*, TestTypes::NoCtors>;
    static_assert(emplace_exists<V, int>(), "");
    static_assert(emplace_exists<V, int, int>(), "");
    static_assert(!emplace_exists<V, int, decltype(nullptr)>(), "cannot construct");
    static_assert(emplace_exists<V, void*, decltype(nullptr)>(), "");
    static_assert(!emplace_exists<V, void*, int>(), "cannot construct");
    static_assert(emplace_exists<V, void*, int*>(), "");
    static_assert(!emplace_exists<V, void*, const int*>(), "");
    static_assert(emplace_exists<V, const void*, const int*>(), "");
    static_assert(emplace_exists<V, const void*, int*>(), "");
    static_assert(!emplace_exists<V, TestTypes::NoCtors>(), "cannot construct");
  }
}

struct NoCtor {
  NoCtor() = delete;
};

TEST_CONSTEXPR_CXX20 void test_basic() {
  {
    using V = std::variant<int>;
    V v(42);
    auto& ref1 = v.emplace<int>();
    static_assert(std::is_same_v<int&, decltype(ref1)>, "");
    assert(std::get<0>(v) == 0);
    assert(&ref1 == &std::get<0>(v));
    auto& ref2 = v.emplace<int>(42);
    static_assert(std::is_same_v<int&, decltype(ref2)>, "");
    assert(std::get<0>(v) == 42);
    assert(&ref2 == &std::get<0>(v));
  }
  {
    using V     = std::variant<int, long, const void*, NoCtor, std::string>;
    const int x = 100;
    V v(std::in_place_type<int>, -1);
    // default emplace a value
    auto& ref1 = v.emplace<long>();
    static_assert(std::is_same_v<long&, decltype(ref1)>, "");
    assert(std::get<1>(v) == 0);
    assert(&ref1 == &std::get<1>(v));
    auto& ref2 = v.emplace<const void*>(&x);
    static_assert(std::is_same_v<const void*&, decltype(ref2)>, "");
    assert(std::get<2>(v) == &x);
    assert(&ref2 == &std::get<2>(v));
    // emplace with multiple args
    auto& ref3 = v.emplace<std::string>(3u, 'a');
    static_assert(std::is_same_v<std::string&, decltype(ref3)>, "");
    assert(std::get<4>(v) == "aaa");
    assert(&ref3 == &std::get<4>(v));
  }
}

TEST_CONSTEXPR_CXX20 bool test() {
  test_basic();
  test_emplace_sfinae();

  return true;
}

int run_test() {
  test();

#if TEST_STD_VER >= 20
  static_assert(test());
#endif

  return 0;
}
} // namespace emplace::type_args
// -- END: test/std/utilities/variant/variant.variant/variant.mod/emplace_type_args.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.mod/emplace_type_init_list_args.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// template <class T, class U, class ...Args>
//   T& emplace(initializer_list<U> il,Args&&... args);

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>

#include "archetypes.h"
#include "test_convertible.h"
#include "test_macros.h"

namespace emplace::type_init_list_args {
struct InitList {
  std::size_t size;
  constexpr InitList(std::initializer_list<int> il) : size(il.size()) {}
};

struct InitListArg {
  std::size_t size;
  int value;
  constexpr InitListArg(std::initializer_list<int> il, int v) : size(il.size()), value(v) {}
};

template <class Var, class T, class... Args>
constexpr auto test_emplace_exists_imp(int)
    -> decltype(std::declval<Var>().template emplace<T>(std::declval<Args>()...), true) {
  return true;
}

template <class, class, class...>
constexpr auto test_emplace_exists_imp(long) -> bool {
  return false;
}

template <class... Args>
constexpr bool emplace_exists() {
  return test_emplace_exists_imp<Args...>(0);
}

constexpr void test_emplace_sfinae() {
  using V  = std::variant<int, TestTypes::NoCtors, InitList, InitListArg, long, long>;
  using IL = std::initializer_list<int>;
  static_assert(emplace_exists<V, InitList, IL>(), "");
  static_assert(!emplace_exists<V, InitList, int>(), "args don't match");
  static_assert(!emplace_exists<V, InitList, IL, int>(), "too many args");
  static_assert(emplace_exists<V, InitListArg, IL, int>(), "");
  static_assert(!emplace_exists<V, InitListArg, int>(), "args don't match");
  static_assert(!emplace_exists<V, InitListArg, IL>(), "too few args");
  static_assert(!emplace_exists<V, InitListArg, IL, int, int>(), "too many args");
}

struct NoCtor {
  NoCtor() = delete;
};

TEST_CONSTEXPR_CXX20 void test_basic() {
  using V = std::variant<int, InitList, InitListArg, NoCtor>;
  V v;
  auto& ref1 = v.emplace<InitList>({1, 2, 3});
  static_assert(std::is_same_v<InitList&, decltype(ref1)>, "");
  assert(std::get<InitList>(v).size == 3);
  assert(&ref1 == &std::get<InitList>(v));
  auto& ref2 = v.emplace<InitListArg>({1, 2, 3, 4}, 42);
  static_assert(std::is_same_v<InitListArg&, decltype(ref2)>, "");
  assert(std::get<InitListArg>(v).size == 4);
  assert(std::get<InitListArg>(v).value == 42);
  assert(&ref2 == &std::get<InitListArg>(v));
  auto& ref3 = v.emplace<InitList>({1});
  static_assert(std::is_same_v<InitList&, decltype(ref3)>, "");
  assert(std::get<InitList>(v).size == 1);
  assert(&ref3 == &std::get<InitList>(v));
}

TEST_CONSTEXPR_CXX20 bool test() {
  test_basic();
  test_emplace_sfinae();

  return true;
}

int run_test() {
  test();

#if TEST_STD_VER >= 20
  static_assert(test());
#endif

  return 0;
}
} // namespace emplace::type_init_list_args
// -- END: test/std/utilities/variant/variant.variant/variant.mod/emplace_type_init_list_args.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.status/index.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// constexpr size_t index() const noexcept;

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>

#include "archetypes.h"
#include "test_macros.h"
#include "variant_test_helpers.h"


namespace status::index {
int run_test() {
  {
    using V = std::variant<int, long>;
    constexpr V v;
    static_assert(v.index() == 0, "");
  }
  {
    using V = std::variant<int, long>;
    V v;
    assert(v.index() == 0);
  }
  {
    using V = std::variant<int, long>;
    constexpr V v(std::in_place_index<1>);
    static_assert(v.index() == 1, "");
  }
  {
    using V = std::variant<int, std::string>;
    V v("abc");
    assert(v.index() == 1);
    v = 42;
    assert(v.index() == 0);
  }
#ifndef TEST_HAS_NO_EXCEPTIONS
  {
    using V = std::variant<int, MakeEmptyT>;
    V v;
    assert(v.index() == 0);
    makeEmpty(v);
    assert(v.index() == std::variant_npos);
  }
#endif

  return 0;
}
} // namespace status::index
// -- END: test/std/utilities/variant/variant.variant/variant.status/index.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.status/valueless_by_exception.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// constexpr bool valueless_by_exception() const noexcept;

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>

#include "archetypes.h"
#include "test_macros.h"
#include "variant_test_helpers.h"


namespace status::valueless_by_exception {
int run_test() {
  {
    using V = std::variant<int, long>;
    constexpr V v;
    static_assert(!v.valueless_by_exception(), "");
  }
  {
    using V = std::variant<int, long>;
    V v;
    assert(!v.valueless_by_exception());
  }
  {
    using V = std::variant<int, long, std::string>;
    const V v("abc");
    assert(!v.valueless_by_exception());
  }
#ifndef TEST_HAS_NO_EXCEPTIONS
  {
    using V = std::variant<int, MakeEmptyT>;
    V v;
    assert(!v.valueless_by_exception());
    makeEmpty(v);
    assert(v.valueless_by_exception());
  }
#endif

  return 0;
}
} // namespace status::valueless_by_exception
// -- END: test/std/utilities/variant/variant.variant/variant.status/valueless_by_exception.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.swap/swap.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>

// template <class ...Types> class variant;

// void swap(variant& rhs) noexcept(see below)

#include <cassert>
#include <cstdlib>
#include <string>
#include <type_traits>
#include <variant>

#include "test_convertible.h"
#include "test_macros.h"
#include "variant_test_helpers.h"

namespace member_swap {
struct NotSwappable {};
void swap(NotSwappable&, NotSwappable&) = delete;

struct NotCopyable {
  NotCopyable()                              = default;
  NotCopyable(const NotCopyable&)            = delete;
  NotCopyable& operator=(const NotCopyable&) = delete;
};

struct NotCopyableWithSwap {
  NotCopyableWithSwap()                                      = default;
  NotCopyableWithSwap(const NotCopyableWithSwap&)            = delete;
  NotCopyableWithSwap& operator=(const NotCopyableWithSwap&) = delete;
};
constexpr void swap(NotCopyableWithSwap&, NotCopyableWithSwap) {}

struct NotMoveAssignable {
  NotMoveAssignable()                               = default;
  NotMoveAssignable(NotMoveAssignable&&)            = default;
  NotMoveAssignable& operator=(NotMoveAssignable&&) = delete;
};

struct NotMoveAssignableWithSwap {
  NotMoveAssignableWithSwap()                                       = default;
  NotMoveAssignableWithSwap(NotMoveAssignableWithSwap&&)            = default;
  NotMoveAssignableWithSwap& operator=(NotMoveAssignableWithSwap&&) = delete;
};
constexpr void swap(NotMoveAssignableWithSwap&, NotMoveAssignableWithSwap&) noexcept {}

template <bool Throws>
constexpr void do_throw() {}

template <>
void do_throw<true>() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  throw 42;
#else
  std::abort();
#endif
}

template <bool NT_Copy, bool NT_Move, bool NT_CopyAssign, bool NT_MoveAssign, bool NT_Swap, bool EnableSwap = true>
struct NothrowTypeImp {
  int value;
  int* move_called;
  int* move_assign_called;
  int* swap_called;

  constexpr NothrowTypeImp(int v, int* mv_ctr, int* mv_assign, int* swap)
      : value(v), move_called(mv_ctr), move_assign_called(mv_assign), swap_called(swap) {}

  NothrowTypeImp(const NothrowTypeImp& o) noexcept(NT_Copy) : value(o.value) { assert(false); } // never called by test

  constexpr NothrowTypeImp(NothrowTypeImp&& o) noexcept(NT_Move)
      : value(o.value),
        move_called(o.move_called),
        move_assign_called(o.move_assign_called),
        swap_called(o.swap_called) {
    ++*move_called;
    do_throw<!NT_Move>();
    o.value = -1;
  }

  NothrowTypeImp& operator=(const NothrowTypeImp&) noexcept(NT_CopyAssign) {
    assert(false);
    return *this;
  } // never called by the tests

  constexpr NothrowTypeImp& operator=(NothrowTypeImp&& o) noexcept(NT_MoveAssign) {
    ++*move_assign_called;
    do_throw<!NT_MoveAssign>();
    value   = o.value;
    o.value = -1;
    return *this;
  }
};

template <bool NT_Copy, bool NT_Move, bool NT_CopyAssign, bool NT_MoveAssign, bool NT_Swap>
constexpr void
swap(NothrowTypeImp<NT_Copy, NT_Move, NT_CopyAssign, NT_MoveAssign, NT_Swap, true>& lhs,
     NothrowTypeImp<NT_Copy, NT_Move, NT_CopyAssign, NT_MoveAssign, NT_Swap, true>& rhs) noexcept(NT_Swap) {
  ++*lhs.swap_called;
  do_throw<!NT_Swap>();
  std::swap(lhs.value, rhs.value);
}

// throwing copy, nothrow move ctor/assign, no swap provided
using NothrowMoveable = NothrowTypeImp<false, true, false, true, false, false>;
// throwing copy and move assign, nothrow move ctor, no swap provided
using NothrowMoveCtor = NothrowTypeImp<false, true, false, false, false, false>;
// nothrow move ctor, throwing move assignment, swap provided
using NothrowMoveCtorWithThrowingSwap = NothrowTypeImp<false, true, false, false, false, true>;
// throwing move ctor, nothrow move assignment, no swap provided
using ThrowingMoveCtor = NothrowTypeImp<false, false, false, true, false, false>;
// throwing special members, nothrowing swap
using ThrowingTypeWithNothrowSwap = NothrowTypeImp<false, false, false, false, true, true>;
using NothrowTypeWithThrowingSwap = NothrowTypeImp<true, true, true, true, false, true>;
// throwing move assign with nothrow move and nothrow swap
using ThrowingMoveAssignNothrowMoveCtorWithSwap = NothrowTypeImp<false, true, false, false, true, true>;
// throwing move assign with nothrow move but no swap.
using ThrowingMoveAssignNothrowMoveCtor = NothrowTypeImp<false, true, false, false, false, false>;

struct NonThrowingNonNoexceptType {
  int value;
  int* move_called;
  constexpr NonThrowingNonNoexceptType(int v, int* mv_called) : value(v), move_called(mv_called) {}
  constexpr NonThrowingNonNoexceptType(NonThrowingNonNoexceptType&& o) noexcept(false)
      : value(o.value), move_called(o.move_called) {
    ++*move_called;
    o.value = -1;
  }
  NonThrowingNonNoexceptType& operator=(NonThrowingNonNoexceptType&&) noexcept(false) {
    assert(false); // never called by the tests.
    return *this;
  }
};

struct ThrowsOnSecondMove {
  int value;
  int move_count;
  ThrowsOnSecondMove(int v) : value(v), move_count(0) {}
  ThrowsOnSecondMove(ThrowsOnSecondMove&& o) noexcept(false) : value(o.value), move_count(o.move_count + 1) {
    if (move_count == 2)
      do_throw<true>();
    o.value = -1;
  }
  ThrowsOnSecondMove& operator=(ThrowsOnSecondMove&&) {
    assert(false); // not called by test
    return *this;
  }
};

void test_swap_valueless_by_exception() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  using V = std::variant<int, MakeEmptyT>;
  { // both empty
    V v1;
    makeEmpty(v1);
    V v2;
    makeEmpty(v2);
    assert(MakeEmptyT::alive == 0);
    { // member swap
      v1.swap(v2);
      assert(v1.valueless_by_exception());
      assert(v2.valueless_by_exception());
      assert(MakeEmptyT::alive == 0);
    }
    { // non-member swap
      swap(v1, v2);
      assert(v1.valueless_by_exception());
      assert(v2.valueless_by_exception());
      assert(MakeEmptyT::alive == 0);
    }
  }
  { // only one empty
    V v1(42);
    V v2;
    makeEmpty(v2);
    { // member swap
      v1.swap(v2);
      assert(v1.valueless_by_exception());
      assert(std::get<0>(v2) == 42);
      // swap again
      v2.swap(v1);
      assert(v2.valueless_by_exception());
      assert(std::get<0>(v1) == 42);
    }
    { // non-member swap
      swap(v1, v2);
      assert(v1.valueless_by_exception());
      assert(std::get<0>(v2) == 42);
      // swap again
      swap(v1, v2);
      assert(v2.valueless_by_exception());
      assert(std::get<0>(v1) == 42);
    }
  }
#endif
}

TEST_CONSTEXPR_CXX20 void test_swap_same_alternative() {
  {
    using V                = std::variant<ThrowingTypeWithNothrowSwap, int>;
    int move_called        = 0;
    int move_assign_called = 0;
    int swap_called        = 0;
    V v1(std::in_place_index<0>, 42, &move_called, &move_assign_called, &swap_called);
    V v2(std::in_place_index<0>, 100, &move_called, &move_assign_called, &swap_called);
    v1.swap(v2);
    assert(swap_called == 1);
    assert(std::get<0>(v1).value == 100);
    assert(std::get<0>(v2).value == 42);
    swap(v1, v2);
    assert(swap_called == 2);
    assert(std::get<0>(v1).value == 42);
    assert(std::get<0>(v2).value == 100);

    assert(move_called == 0);
    assert(move_assign_called == 0);
  }
  {
    using V                = std::variant<NothrowMoveable, int>;
    int move_called        = 0;
    int move_assign_called = 0;
    int swap_called        = 0;
    V v1(std::in_place_index<0>, 42, &move_called, &move_assign_called, &swap_called);
    V v2(std::in_place_index<0>, 100, &move_called, &move_assign_called, &swap_called);
    v1.swap(v2);
    assert(swap_called == 0);
    assert(move_called == 1);
    assert(move_assign_called == 2);
    assert(std::get<0>(v1).value == 100);
    assert(std::get<0>(v2).value == 42);

    move_called        = 0;
    move_assign_called = 0;
    swap_called        = 0;

    swap(v1, v2);
    assert(swap_called == 0);
    assert(move_called == 1);
    assert(move_assign_called == 2);
    assert(std::get<0>(v1).value == 42);
    assert(std::get<0>(v2).value == 100);
  }
}

void test_swap_same_alternative_throws(){
#ifndef TEST_HAS_NO_EXCEPTIONS
    {using V = std::variant<NothrowTypeWithThrowingSwap, int>;
int move_called        = 0;
int move_assign_called = 0;
int swap_called        = 0;
V v1(std::in_place_index<0>, 42, &move_called, &move_assign_called, &swap_called);
V v2(std::in_place_index<0>, 100, &move_called, &move_assign_called, &swap_called);
try {
  v1.swap(v2);
  assert(false);
} catch (int) {
}
assert(swap_called == 1);
assert(move_called == 0);
assert(move_assign_called == 0);
assert(std::get<0>(v1).value == 42);
assert(std::get<0>(v2).value == 100);
}

{
  using V                = std::variant<ThrowingMoveCtor, int>;
  int move_called        = 0;
  int move_assign_called = 0;
  int swap_called        = 0;
  V v1(std::in_place_index<0>, 42, &move_called, &move_assign_called, &swap_called);
  V v2(std::in_place_index<0>, 100, &move_called, &move_assign_called, &swap_called);
  try {
    v1.swap(v2);
    assert(false);
  } catch (int) {
  }
  assert(move_called == 1); // call threw
  assert(move_assign_called == 0);
  assert(swap_called == 0);
  assert(std::get<0>(v1).value == 42); // throw happened before v1 was moved from
  assert(std::get<0>(v2).value == 100);
}
{
  using V                = std::variant<ThrowingMoveAssignNothrowMoveCtor, int>;
  int move_called        = 0;
  int move_assign_called = 0;
  int swap_called        = 0;
  V v1(std::in_place_index<0>, 42, &move_called, &move_assign_called, &swap_called);
  V v2(std::in_place_index<0>, 100, &move_called, &move_assign_called, &swap_called);
  try {
    v1.swap(v2);
    assert(false);
  } catch (int) {
  }
  assert(move_called == 1);
  assert(move_assign_called == 1); // call threw and didn't complete
  assert(swap_called == 0);
  assert(std::get<0>(v1).value == -1); // v1 was moved from
  assert(std::get<0>(v2).value == 100);
}
#endif
}

TEST_CONSTEXPR_CXX20 void test_swap_different_alternatives() {
  {
    using V                = std::variant<NothrowMoveCtorWithThrowingSwap, int>;
    int move_called        = 0;
    int move_assign_called = 0;
    int swap_called        = 0;
    V v1(std::in_place_index<0>, 42, &move_called, &move_assign_called, &swap_called);
    V v2(std::in_place_index<1>, 100);
    v1.swap(v2);
    assert(swap_called == 0);
    // The libc++ implementation double copies the argument, and not
    // the variant swap is called on.
    LIBCPP_ASSERT(move_called == 1);
    assert(move_called <= 2);
    assert(move_assign_called == 0);
    assert(std::get<1>(v1) == 100);
    assert(std::get<0>(v2).value == 42);

    move_called        = 0;
    move_assign_called = 0;
    swap_called        = 0;

    swap(v1, v2);
    assert(swap_called == 0);
    LIBCPP_ASSERT(move_called == 2);
    assert(move_called <= 2);
    assert(move_assign_called == 0);
    assert(std::get<0>(v1).value == 42);
    assert(std::get<1>(v2) == 100);
  }
}

void test_swap_different_alternatives_throws() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  {
    using V                 = std::variant<ThrowingTypeWithNothrowSwap, NonThrowingNonNoexceptType>;
    int move_called1        = 0;
    int move_assign_called1 = 0;
    int swap_called1        = 0;
    int move_called2        = 0;
    V v1(std::in_place_index<0>, 42, &move_called1, &move_assign_called1, &swap_called1);
    V v2(std::in_place_index<1>, 100, &move_called2);
    try {
      v1.swap(v2);
      assert(false);
    } catch (int) {
    }
    assert(swap_called1 == 0);
    assert(move_called1 == 1); // throws
    assert(move_assign_called1 == 0);
    // TODO: libc++ shouldn't move from T2 here.
    LIBCPP_ASSERT(move_called2 == 1);
    assert(move_called2 <= 1);
    assert(std::get<0>(v1).value == 42);
    if (move_called2 != 0)
      assert(v2.valueless_by_exception());
    else
      assert(std::get<1>(v2).value == 100);
  }
  {
    using V                 = std::variant<NonThrowingNonNoexceptType, ThrowingTypeWithNothrowSwap>;
    int move_called1        = 0;
    int move_called2        = 0;
    int move_assign_called2 = 0;
    int swap_called2        = 0;
    V v1(std::in_place_index<0>, 42, &move_called1);
    V v2(std::in_place_index<1>, 100, &move_called2, &move_assign_called2, &swap_called2);
    try {
      v1.swap(v2);
      assert(false);
    } catch (int) {
    }
    LIBCPP_ASSERT(move_called1 == 0);
    assert(move_called1 <= 1);
    assert(swap_called2 == 0);
    assert(move_called2 == 1); // throws
    assert(move_assign_called2 == 0);
    if (move_called1 != 0)
      assert(v1.valueless_by_exception());
    else
      assert(std::get<0>(v1).value == 42);
    assert(std::get<1>(v2).value == 100);
  }
// TODO: The tests below are just very libc++ specific
#  ifdef _LIBCPP_VERSION
  {
    using V         = std::variant<ThrowsOnSecondMove, NonThrowingNonNoexceptType>;
    int move_called = 0;
    V v1(std::in_place_index<0>, 42);
    V v2(std::in_place_index<1>, 100, &move_called);
    v1.swap(v2);
    assert(move_called == 2);
    assert(std::get<1>(v1).value == 100);
    assert(std::get<0>(v2).value == 42);
    assert(std::get<0>(v2).move_count == 1);
  }
  {
    using V         = std::variant<NonThrowingNonNoexceptType, ThrowsOnSecondMove>;
    int move_called = 0;
    V v1(std::in_place_index<0>, 42, &move_called);
    V v2(std::in_place_index<1>, 100);
    try {
      v1.swap(v2);
      assert(false);
    } catch (int) {
    }
    assert(move_called == 1);
    assert(v1.valueless_by_exception());
    assert(std::get<0>(v2).value == 42);
  }
#  endif
  // testing libc++ extension. If either variant stores a nothrow move
  // constructible type v1.swap(v2) provides the strong exception safety
  // guarantee.
#  ifdef _LIBCPP_VERSION
  {
    using V                 = std::variant<ThrowingTypeWithNothrowSwap, NothrowMoveable>;
    int move_called1        = 0;
    int move_assign_called1 = 0;
    int swap_called1        = 0;
    int move_called2        = 0;
    int move_assign_called2 = 0;
    int swap_called2        = 0;
    V v1(std::in_place_index<0>, 42, &move_called1, &move_assign_called1, &swap_called1);
    V v2(std::in_place_index<1>, 100, &move_called2, &move_assign_called2, &swap_called2);
    try {
      v1.swap(v2);
      assert(false);
    } catch (int) {
    }
    assert(swap_called1 == 0);
    assert(move_called1 == 1);
    assert(move_assign_called1 == 0);
    assert(swap_called2 == 0);
    assert(move_called2 == 2);
    assert(move_assign_called2 == 0);
    assert(std::get<0>(v1).value == 42);
    assert(std::get<1>(v2).value == 100);
    // swap again, but call v2's swap.

    move_called1        = 0;
    move_assign_called1 = 0;
    swap_called1        = 0;
    move_called2        = 0;
    move_assign_called2 = 0;
    swap_called2        = 0;

    try {
      v2.swap(v1);
      assert(false);
    } catch (int) {
    }
    assert(swap_called1 == 0);
    assert(move_called1 == 1);
    assert(move_assign_called1 == 0);
    assert(swap_called2 == 0);
    assert(move_called2 == 2);
    assert(move_assign_called2 == 0);
    assert(std::get<0>(v1).value == 42);
    assert(std::get<1>(v2).value == 100);
  }
#  endif // _LIBCPP_VERSION
#endif
}

template <class Var>
constexpr auto has_swap_member_imp(int) -> decltype(std::declval<Var&>().swap(std::declval<Var&>()), true) {
  return true;
}

template <class Var>
constexpr auto has_swap_member_imp(long) -> bool {
  return false;
}

template <class Var>
constexpr bool has_swap_member() {
  return has_swap_member_imp<Var>(0);
}

constexpr void test_swap_sfinae() {
  {
    // This variant type does not provide either a member or non-member swap
    // but is still swappable via the generic swap algorithm, since the
    // variant is move constructible and move assignable.
    using V = std::variant<int, NotSwappable>;
    LIBCPP_STATIC_ASSERT(!has_swap_member<V>(), "");
    static_assert(std::is_swappable_v<V>, "");
  }
  {
    using V = std::variant<int, NotCopyable>;
    LIBCPP_STATIC_ASSERT(!has_swap_member<V>(), "");
    static_assert(!std::is_swappable_v<V>, "");
  }
  {
    using V = std::variant<int, NotCopyableWithSwap>;
    LIBCPP_STATIC_ASSERT(!has_swap_member<V>(), "");
    static_assert(!std::is_swappable_v<V>, "");
  }
  {
    using V = std::variant<int, NotMoveAssignable>;
    LIBCPP_STATIC_ASSERT(!has_swap_member<V>(), "");
    static_assert(!std::is_swappable_v<V>, "");
  }
}

TEST_CONSTEXPR_CXX20 void test_swap_noexcept() {
  {
    using V = std::variant<int, NothrowMoveable>;
    static_assert(std::is_swappable_v<V> && has_swap_member<V>(), "");
    static_assert(std::is_nothrow_swappable_v<V>, "");
    // instantiate swap
    V v1, v2;
    v1.swap(v2);
    swap(v1, v2);
  }
  {
    using V = std::variant<int, NothrowMoveCtor>;
    static_assert(std::is_swappable_v<V> && has_swap_member<V>(), "");
    static_assert(!std::is_nothrow_swappable_v<V>, "");
    // instantiate swap
    V v1, v2;
    v1.swap(v2);
    swap(v1, v2);
  }
  {
    using V = std::variant<int, ThrowingTypeWithNothrowSwap>;
    static_assert(std::is_swappable_v<V> && has_swap_member<V>(), "");
    static_assert(!std::is_nothrow_swappable_v<V>, "");
    // instantiate swap
    V v1, v2;
    v1.swap(v2);
    swap(v1, v2);
  }
  {
    using V = std::variant<int, ThrowingMoveAssignNothrowMoveCtor>;
    static_assert(std::is_swappable_v<V> && has_swap_member<V>(), "");
    static_assert(!std::is_nothrow_swappable_v<V>, "");
    // instantiate swap
    V v1, v2;
    v1.swap(v2);
    swap(v1, v2);
  }
  {
    using V = std::variant<int, ThrowingMoveAssignNothrowMoveCtorWithSwap>;
    static_assert(std::is_swappable_v<V> && has_swap_member<V>(), "");
    static_assert(std::is_nothrow_swappable_v<V>, "");
    // instantiate swap
    V v1, v2;
    v1.swap(v2);
    swap(v1, v2);
  }
  {
    using V = std::variant<int, NotMoveAssignableWithSwap>;
    static_assert(std::is_swappable_v<V> && has_swap_member<V>(), "");
    static_assert(std::is_nothrow_swappable_v<V>, "");
    // instantiate swap
    V v1, v2;
    v1.swap(v2);
    swap(v1, v2);
  }
  {
    // This variant type does not provide either a member or non-member swap
    // but is still swappable via the generic swap algorithm, since the
    // variant is move constructible and move assignable.
    using V = std::variant<int, NotSwappable>;
    LIBCPP_STATIC_ASSERT(!has_swap_member<V>(), "");
    static_assert(std::is_swappable_v<V>, "");
    static_assert(std::is_nothrow_swappable_v<V>, "");
    V v1, v2;
    swap(v1, v2);
  }
}

#ifdef _LIBCPP_VERSION
// This is why variant should SFINAE member swap. :-)
template class std::variant<int, NotSwappable>;
#endif

void non_constexpr_test() {
  test_swap_valueless_by_exception();
  test_swap_same_alternative_throws();
  test_swap_different_alternatives_throws();
}

TEST_CONSTEXPR_CXX20 bool test() {
  test_swap_same_alternative();
  test_swap_different_alternatives();
  test_swap_sfinae();
  test_swap_noexcept();

  return true;
}

int run_test() {
  non_constexpr_test();
  test();

#if TEST_STD_VER >= 20
  static_assert(test());
#endif

  return 0;
}
} // namespace member_swap
// -- END: test/std/utilities/variant/variant.variant/variant.swap/swap.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.visit/robust_against_adl.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>
// template <class Visitor, class... Variants>
// constexpr see below visit(Visitor&& vis, Variants&&... vars);

#include <variant>

#include "test_macros.h"

namespace visit::robust_against_adl {
struct Incomplete;
template<class T> struct Holder { T t; };

constexpr bool test(bool do_it)
{
    if (do_it) {
#ifndef _M_CEE // TRANSITION, VSO-1659496
        std::variant<Holder<Incomplete>*, int> v = nullptr;
        std::visit([](auto){}, v);
        std::visit([](auto) -> Holder<Incomplete>* { return nullptr; }, v);
#if TEST_STD_VER > 17
        std::visit<void>([](auto){}, v);
        std::visit<void*>([](auto) -> Holder<Incomplete>* { return nullptr; }, v);
#endif
#endif // ^^^ no workaround ^^^
    }
    return true;
}

int run_test()
{
    test(true);
#if TEST_STD_VER > 17
    static_assert(test(true));
#endif
    return 0;
}
} // namespace visit::robust_against_adl
// -- END: test/std/utilities/variant/variant.visit/robust_against_adl.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.visit/visit.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>
// template <class Visitor, class... Variants>
// constexpr see below visit(Visitor&& vis, Variants&&... vars);

#include <cassert>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#include "test_macros.h"
#include "variant_test_helpers.h"

namespace visit {
void test_call_operator_forwarding() {
  using Fn = ForwardingCallObject;
  Fn obj{};
  const Fn &cobj = obj;
  { // test call operator forwarding - no variant
    std::visit(obj);
    assert(Fn::check_call<>(CT_NonConst | CT_LValue));
    std::visit(cobj);
    assert(Fn::check_call<>(CT_Const | CT_LValue));
    std::visit(std::move(obj));
    assert(Fn::check_call<>(CT_NonConst | CT_RValue));
    std::visit(std::move(cobj));
    assert(Fn::check_call<>(CT_Const | CT_RValue));
  }
  { // test call operator forwarding - single variant, single arg
    using V = std::variant<int>;
    V v(42);
    std::visit(obj, v);
    assert(Fn::check_call<int &>(CT_NonConst | CT_LValue));
    std::visit(cobj, v);
    assert(Fn::check_call<int &>(CT_Const | CT_LValue));
    std::visit(std::move(obj), v);
    assert(Fn::check_call<int &>(CT_NonConst | CT_RValue));
    std::visit(std::move(cobj), v);
    assert(Fn::check_call<int &>(CT_Const | CT_RValue));
  }
  { // test call operator forwarding - single variant, multi arg
    using V = std::variant<int, long, double>;
    V v(42l);
    std::visit(obj, v);
    assert(Fn::check_call<long &>(CT_NonConst | CT_LValue));
    std::visit(cobj, v);
    assert(Fn::check_call<long &>(CT_Const | CT_LValue));
    std::visit(std::move(obj), v);
    assert(Fn::check_call<long &>(CT_NonConst | CT_RValue));
    std::visit(std::move(cobj), v);
    assert(Fn::check_call<long &>(CT_Const | CT_RValue));
  }
  { // test call operator forwarding - multi variant, multi arg
    using V = std::variant<int, long, double>;
    using V2 = std::variant<int *, std::string>;
    V v(42l);
    V2 v2("hello");
    std::visit(obj, v, v2);
    assert((Fn::check_call<long &, std::string &>(CT_NonConst | CT_LValue)));
    std::visit(cobj, v, v2);
    assert((Fn::check_call<long &, std::string &>(CT_Const | CT_LValue)));
    std::visit(std::move(obj), v, v2);
    assert((Fn::check_call<long &, std::string &>(CT_NonConst | CT_RValue)));
    std::visit(std::move(cobj), v, v2);
    assert((Fn::check_call<long &, std::string &>(CT_Const | CT_RValue)));
  }
  {
    using V = std::variant<int, long, double, std::string>;
    V v1(42l), v2("hello"), v3(101), v4(1.1);
    std::visit(obj, v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int &, double &>(CT_NonConst | CT_LValue)));
    std::visit(cobj, v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int &, double &>(CT_Const | CT_LValue)));
    std::visit(std::move(obj), v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int &, double &>(CT_NonConst | CT_RValue)));
    std::visit(std::move(cobj), v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int &, double &>(CT_Const | CT_RValue)));
  }
  {
    using V = std::variant<int, long, double, int*, std::string>;
    V v1(42l), v2("hello"), v3(nullptr), v4(1.1);
    std::visit(obj, v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int *&, double &>(CT_NonConst | CT_LValue)));
    std::visit(cobj, v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int *&, double &>(CT_Const | CT_LValue)));
    std::visit(std::move(obj), v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int *&, double &>(CT_NonConst | CT_RValue)));
    std::visit(std::move(cobj), v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int *&, double &>(CT_Const | CT_RValue)));
  }
}

void test_argument_forwarding() {
  using Fn = ForwardingCallObject;
  Fn obj{};
  const auto Val = CT_LValue | CT_NonConst;
  { // single argument - value type
    using V = std::variant<int>;
    V v(42);
    const V &cv = v;
    std::visit(obj, v);
    assert(Fn::check_call<int &>(Val));
    std::visit(obj, cv);
    assert(Fn::check_call<const int &>(Val));
    std::visit(obj, std::move(v));
    assert(Fn::check_call<int &&>(Val));
    std::visit(obj, std::move(cv));
    assert(Fn::check_call<const int &&>(Val));
  }
  { // multi argument - multi variant
    using V = std::variant<int, std::string, long>;
    V v1(42), v2("hello"), v3(43l);
    std::visit(obj, v1, v2, v3);
    assert((Fn::check_call<int &, std::string &, long &>(Val)));
    std::visit(obj, std::as_const(v1), std::as_const(v2), std::move(v3));
    assert((Fn::check_call<const int &, const std::string &, long &&>(Val)));
  }
  {
    using V = std::variant<int, long, double, std::string>;
    V v1(42l), v2("hello"), v3(101), v4(1.1);
    std::visit(obj, v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int &, double &>(Val)));
    std::visit(obj, std::as_const(v1), std::as_const(v2), std::move(v3), std::move(v4));
    assert((Fn::check_call<const long &, const std::string &, int &&, double &&>(Val)));
  }
  {
    using V = std::variant<int, long, double, int*, std::string>;
    V v1(42l), v2("hello"), v3(nullptr), v4(1.1);
    std::visit(obj, v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int *&, double &>(Val)));
    std::visit(obj, std::as_const(v1), std::as_const(v2), std::move(v3), std::move(v4));
    assert((Fn::check_call<const long &, const std::string &, int *&&, double &&>(Val)));
  }
}

void test_return_type() {
  using Fn = ForwardingCallObject;
  Fn obj{};
  const Fn &cobj = obj;
  { // test call operator forwarding - no variant
    static_assert(std::is_same_v<decltype(std::visit(obj)), Fn&>);
    static_assert(std::is_same_v<decltype(std::visit(cobj)), const Fn&>);
    static_assert(std::is_same_v<decltype(std::visit(std::move(obj))), Fn&&>);
    static_assert(std::is_same_v<decltype(std::visit(std::move(cobj))), const Fn&&>);
  }
  { // test call operator forwarding - single variant, single arg
    using V = std::variant<int>;
    V v(42);
    static_assert(std::is_same_v<decltype(std::visit(obj, v)), Fn&>);
    static_assert(std::is_same_v<decltype(std::visit(cobj, v)), const Fn&>);
    static_assert(std::is_same_v<decltype(std::visit(std::move(obj), v)), Fn&&>);
    static_assert(std::is_same_v<decltype(std::visit(std::move(cobj), v)), const Fn&&>);
  }
  { // test call operator forwarding - single variant, multi arg
    using V = std::variant<int, long, double>;
    V v(42l);
    static_assert(std::is_same_v<decltype(std::visit(obj, v)), Fn&>);
    static_assert(std::is_same_v<decltype(std::visit(cobj, v)), const Fn&>);
    static_assert(std::is_same_v<decltype(std::visit(std::move(obj), v)), Fn&&>);
    static_assert(std::is_same_v<decltype(std::visit(std::move(cobj), v)), const Fn&&>);
  }
  { // test call operator forwarding - multi variant, multi arg
    using V = std::variant<int, long, double>;
    using V2 = std::variant<int *, std::string>;
    V v(42l);
    V2 v2("hello");
    static_assert(std::is_same_v<decltype(std::visit(obj, v, v2)), Fn&>);
    static_assert(std::is_same_v<decltype(std::visit(cobj, v, v2)), const Fn&>);
    static_assert(std::is_same_v<decltype(std::visit(std::move(obj), v, v2)), Fn&&>);
    static_assert(std::is_same_v<decltype(std::visit(std::move(cobj), v, v2)), const Fn&&>);
  }
  {
    using V = std::variant<int, long, double, std::string>;
    V v1(42l), v2("hello"), v3(101), v4(1.1);
    static_assert(std::is_same_v<decltype(std::visit(obj, v1, v2, v3, v4)), Fn&>);
    static_assert(std::is_same_v<decltype(std::visit(cobj, v1, v2, v3, v4)), const Fn&>);
    static_assert(std::is_same_v<decltype(std::visit(std::move(obj), v1, v2, v3, v4)), Fn&&>);
    static_assert(std::is_same_v<decltype(std::visit(std::move(cobj), v1, v2, v3, v4)), const Fn&&>);
  }
  {
    using V = std::variant<int, long, double, int*, std::string>;
    V v1(42l), v2("hello"), v3(nullptr), v4(1.1);
    static_assert(std::is_same_v<decltype(std::visit(obj, v1, v2, v3, v4)), Fn&>);
    static_assert(std::is_same_v<decltype(std::visit(cobj, v1, v2, v3, v4)), const Fn&>);
    static_assert(std::is_same_v<decltype(std::visit(std::move(obj), v1, v2, v3, v4)), Fn&&>);
    static_assert(std::is_same_v<decltype(std::visit(std::move(cobj), v1, v2, v3, v4)), const Fn&&>);
  }
}

void test_constexpr() {
  constexpr ReturnFirst obj{};
  constexpr ReturnArity aobj{};
  {
    using V = std::variant<int>;
    constexpr V v(42);
    static_assert(std::visit(obj, v) == 42, "");
  }
  {
    using V = std::variant<short, long, char>;
    constexpr V v(42l);
    static_assert(std::visit(obj, v) == 42, "");
  }
  {
    using V1 = std::variant<int>;
    using V2 = std::variant<int, char *, long long>;
    using V3 = std::variant<bool, int, int>;
    constexpr V1 v1;
    constexpr V2 v2(nullptr);
    constexpr V3 v3;
    static_assert(std::visit(aobj, v1, v2, v3) == 3, "");
  }
  {
    using V1 = std::variant<int>;
    using V2 = std::variant<int, char *, long long>;
    using V3 = std::variant<void *, int, int>;
    constexpr V1 v1;
    constexpr V2 v2(nullptr);
    constexpr V3 v3;
    static_assert(std::visit(aobj, v1, v2, v3) == 3, "");
  }
  {
    using V = std::variant<int, long, double, int *>;
    constexpr V v1(42l), v2(101), v3(nullptr), v4(1.1);
    static_assert(std::visit(aobj, v1, v2, v3, v4) == 4, "");
  }
  {
    using V = std::variant<int, long, double, long long, int *>;
    constexpr V v1(42l), v2(101), v3(nullptr), v4(1.1);
    static_assert(std::visit(aobj, v1, v2, v3, v4) == 4, "");
  }
}

void test_exceptions() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  ReturnArity obj{};
  auto test = [&](auto &&... args) {
    try {
      std::visit(obj, args...);
    } catch (const std::bad_variant_access &) {
      return true;
    } catch (...) {
    }
    return false;
  };
  {
    using V = std::variant<int, MakeEmptyT>;
    V v;
    makeEmpty(v);
    assert(test(v));
  }
  {
    using V = std::variant<int, MakeEmptyT>;
    using V2 = std::variant<long, std::string, void *>;
    V v;
    makeEmpty(v);
    V2 v2("hello");
    assert(test(v, v2));
  }
  {
    using V = std::variant<int, MakeEmptyT>;
    using V2 = std::variant<long, std::string, void *>;
    V v;
    makeEmpty(v);
    V2 v2("hello");
    assert(test(v2, v));
  }
  {
    using V = std::variant<int, MakeEmptyT>;
    using V2 = std::variant<long, std::string, void *, MakeEmptyT>;
    V v;
    makeEmpty(v);
    V2 v2;
    makeEmpty(v2);
    assert(test(v, v2));
  }
  {
    using V = std::variant<int, long, double, MakeEmptyT>;
    V v1(42l), v2(101), v3(202), v4(1.1);
    makeEmpty(v1);
    assert(test(v1, v2, v3, v4));
  }
  {
    using V = std::variant<int, long, double, long long, MakeEmptyT>;
    V v1(42l), v2(101), v3(202), v4(1.1);
    makeEmpty(v1);
    makeEmpty(v2);
    makeEmpty(v3);
    makeEmpty(v4);
    assert(test(v1, v2, v3, v4));
  }
#endif
}

// See https://llvm.org/PR31916
void test_caller_accepts_nonconst() {
  struct A {};
  struct Visitor {
    void operator()(A&) {}
  };
  std::variant<A> v;
  std::visit(Visitor{}, v);
}

struct MyVariant : std::variant<short, long, float> {};
} // namespace visit

namespace std {
template <std::size_t Index>
void get(const ::visit::MyVariant&) {
  assert(false);
}
} // namespace std

namespace visit {
void test_derived_from_variant() {
  auto v1 = MyVariant{42};
  const auto cv1 = MyVariant{142};
  std::visit([](auto x) { assert(x == 42); }, v1);
  std::visit([](auto x) { assert(x == 142); }, cv1);
  std::visit([](auto x) { assert(x == -1.25f); }, MyVariant{-1.25f});
  std::visit([](auto x) { assert(x == 42); }, std::move(v1));
  std::visit([](auto x) { assert(x == 142); }, std::move(cv1));

  // Check that visit does not take index nor valueless_by_exception members from the base class.
  struct EvilVariantBase {
    int index;
    char valueless_by_exception;
  };

  struct EvilVariant1 : std::variant<int, long, double>,
                        std::tuple<int>,
                        EvilVariantBase {
    using std::variant<int, long, double>::variant;
  };

  std::visit([](auto x) { assert(x == 12); }, EvilVariant1{12});
  std::visit([](auto x) { assert(x == 12.3); }, EvilVariant1{12.3});

  // Check that visit unambiguously picks the variant, even if the other base has __impl member.
  struct ImplVariantBase {
    struct Callable {
      bool operator()() const { assert(false); return false; }
    };

    Callable __impl;
  };

  struct EvilVariant2 : std::variant<int, long, double>, ImplVariantBase {
    using std::variant<int, long, double>::variant;
  };

  std::visit([](auto x) { assert(x == 12); }, EvilVariant2{12});
  std::visit([](auto x) { assert(x == 12.3); }, EvilVariant2{12.3});
}

struct any_visitor {
  template <typename T>
  void operator()(const T&) const {}
};

template <typename T, typename = decltype(std::visit(
                          std::declval<any_visitor&>(), std::declval<T>()))>
constexpr bool has_visit(int) {
  return true;
}

template <typename T>
constexpr bool has_visit(...) {
  return false;
}

void test_sfinae() {
  struct BadVariant : std::variant<short>, std::variant<long, float> {};
  struct BadVariant2 : private std::variant<long, float> {};
  struct GoodVariant : std::variant<long, float> {};
  struct GoodVariant2 : GoodVariant {};

  static_assert(!has_visit<int>(0));
  static_assert(has_visit<BadVariant>(0) == is_permissive);
  static_assert(!has_visit<BadVariant2>(0));
  static_assert(has_visit<std::variant<int>>(0));
  static_assert(has_visit<GoodVariant>(0));
  static_assert(has_visit<GoodVariant2>(0));
}

int run_test() {
  test_call_operator_forwarding();
  test_argument_forwarding();
  test_return_type();
  test_constexpr();
  test_exceptions();
  test_caller_accepts_nonconst();
  test_derived_from_variant();
  test_sfinae();

  return 0;
}
} // namespace visit
// -- END: test/std/utilities/variant/variant.visit/visit.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.visit/visit_return_type.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <variant>
// template <class R, class Visitor, class... Variants>
// constexpr R visit(Visitor&& vis, Variants&&... vars);

#include <cassert>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#include "test_macros.h"
#include "variant_test_helpers.h"

namespace visit::return_type {
#if _HAS_CXX20
template <typename ReturnType>
void test_call_operator_forwarding() {
  using Fn = ForwardingCallObject;
  Fn obj{};
  const Fn &cobj = obj;
  { // test call operator forwarding - no variant
    std::visit<ReturnType>(obj);
    assert(Fn::check_call<>(CT_NonConst | CT_LValue));
    std::visit<ReturnType>(cobj);
    assert(Fn::check_call<>(CT_Const | CT_LValue));
    std::visit<ReturnType>(std::move(obj));
    assert(Fn::check_call<>(CT_NonConst | CT_RValue));
    std::visit<ReturnType>(std::move(cobj));
    assert(Fn::check_call<>(CT_Const | CT_RValue));
  }
  { // test call operator forwarding - single variant, single arg
    using V = std::variant<int>;
    V v(42);
    std::visit<ReturnType>(obj, v);
    assert(Fn::check_call<int &>(CT_NonConst | CT_LValue));
    std::visit<ReturnType>(cobj, v);
    assert(Fn::check_call<int &>(CT_Const | CT_LValue));
    std::visit<ReturnType>(std::move(obj), v);
    assert(Fn::check_call<int &>(CT_NonConst | CT_RValue));
    std::visit<ReturnType>(std::move(cobj), v);
    assert(Fn::check_call<int &>(CT_Const | CT_RValue));
  }
  { // test call operator forwarding - single variant, multi arg
    using V = std::variant<int, long, double>;
    V v(42l);
    std::visit<ReturnType>(obj, v);
    assert(Fn::check_call<long &>(CT_NonConst | CT_LValue));
    std::visit<ReturnType>(cobj, v);
    assert(Fn::check_call<long &>(CT_Const | CT_LValue));
    std::visit<ReturnType>(std::move(obj), v);
    assert(Fn::check_call<long &>(CT_NonConst | CT_RValue));
    std::visit<ReturnType>(std::move(cobj), v);
    assert(Fn::check_call<long &>(CT_Const | CT_RValue));
  }
  { // test call operator forwarding - multi variant, multi arg
    using V = std::variant<int, long, double>;
    using V2 = std::variant<int *, std::string>;
    V v(42l);
    V2 v2("hello");
    std::visit<int>(obj, v, v2);
    assert((Fn::check_call<long &, std::string &>(CT_NonConst | CT_LValue)));
    std::visit<ReturnType>(cobj, v, v2);
    assert((Fn::check_call<long &, std::string &>(CT_Const | CT_LValue)));
    std::visit<ReturnType>(std::move(obj), v, v2);
    assert((Fn::check_call<long &, std::string &>(CT_NonConst | CT_RValue)));
    std::visit<ReturnType>(std::move(cobj), v, v2);
    assert((Fn::check_call<long &, std::string &>(CT_Const | CT_RValue)));
  }
  {
    using V = std::variant<int, long, double, std::string>;
    V v1(42l), v2("hello"), v3(101), v4(1.1);
    std::visit<ReturnType>(obj, v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int &, double &>(CT_NonConst | CT_LValue)));
    std::visit<ReturnType>(cobj, v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int &, double &>(CT_Const | CT_LValue)));
    std::visit<ReturnType>(std::move(obj), v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int &, double &>(CT_NonConst | CT_RValue)));
    std::visit<ReturnType>(std::move(cobj), v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int &, double &>(CT_Const | CT_RValue)));
  }
  {
    using V = std::variant<int, long, double, int*, std::string>;
    V v1(42l), v2("hello"), v3(nullptr), v4(1.1);
    std::visit<ReturnType>(obj, v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int *&, double &>(CT_NonConst | CT_LValue)));
    std::visit<ReturnType>(cobj, v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int *&, double &>(CT_Const | CT_LValue)));
    std::visit<ReturnType>(std::move(obj), v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int *&, double &>(CT_NonConst | CT_RValue)));
    std::visit<ReturnType>(std::move(cobj), v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int *&, double &>(CT_Const | CT_RValue)));
  }
}

template <typename ReturnType>
void test_argument_forwarding() {
  using Fn = ForwardingCallObject;
  Fn obj{};
  const auto Val = CT_LValue | CT_NonConst;
  { // single argument - value type
    using V = std::variant<int>;
    V v(42);
    const V &cv = v;
    std::visit<ReturnType>(obj, v);
    assert(Fn::check_call<int &>(Val));
    std::visit<ReturnType>(obj, cv);
    assert(Fn::check_call<const int &>(Val));
    std::visit<ReturnType>(obj, std::move(v));
    assert(Fn::check_call<int &&>(Val));
    std::visit<ReturnType>(obj, std::move(cv));
    assert(Fn::check_call<const int &&>(Val));
  }
  { // multi argument - multi variant
    using V = std::variant<int, std::string, long>;
    V v1(42), v2("hello"), v3(43l);
    std::visit<ReturnType>(obj, v1, v2, v3);
    assert((Fn::check_call<int &, std::string &, long &>(Val)));
    std::visit<ReturnType>(obj, std::as_const(v1), std::as_const(v2), std::move(v3));
    assert((Fn::check_call<const int &, const std::string &, long &&>(Val)));
  }
  {
    using V = std::variant<int, long, double, std::string>;
    V v1(42l), v2("hello"), v3(101), v4(1.1);
    std::visit<ReturnType>(obj, v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int &, double &>(Val)));
    std::visit<ReturnType>(obj, std::as_const(v1), std::as_const(v2), std::move(v3), std::move(v4));
    assert((Fn::check_call<const long &, const std::string &, int &&, double &&>(Val)));
  }
  {
    using V = std::variant<int, long, double, int*, std::string>;
    V v1(42l), v2("hello"), v3(nullptr), v4(1.1);
    std::visit<ReturnType>(obj, v1, v2, v3, v4);
    assert((Fn::check_call<long &, std::string &, int *&, double &>(Val)));
    std::visit<ReturnType>(obj, std::as_const(v1), std::as_const(v2), std::move(v3), std::move(v4));
    assert((Fn::check_call<const long &, const std::string &, int *&&, double &&>(Val)));
  }
}

template <typename ReturnType>
void test_return_type() {
  using Fn = ForwardingCallObject;
  Fn obj{};
  const Fn &cobj = obj;
  { // test call operator forwarding - no variant
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(obj)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(cobj)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(std::move(obj))), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(std::move(cobj))), ReturnType>);
  }
  { // test call operator forwarding - single variant, single arg
    using V = std::variant<int>;
    V v(42);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(obj, v)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(cobj, v)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(std::move(obj), v)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(std::move(cobj), v)), ReturnType>);
  }
  { // test call operator forwarding - single variant, multi arg
    using V = std::variant<int, long, double>;
    V v(42l);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(obj, v)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(cobj, v)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(std::move(obj), v)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(std::move(cobj), v)), ReturnType>);
  }
  { // test call operator forwarding - multi variant, multi arg
    using V = std::variant<int, long, double>;
    using V2 = std::variant<int *, std::string>;
    V v(42l);
    V2 v2("hello");
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(obj, v, v2)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(cobj, v, v2)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(std::move(obj), v, v2)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(std::move(cobj), v, v2)), ReturnType>);
  }
  {
    using V = std::variant<int, long, double, std::string>;
    V v1(42l), v2("hello"), v3(101), v4(1.1);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(obj, v1, v2, v3, v4)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(cobj, v1, v2, v3, v4)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(std::move(obj), v1, v2, v3, v4)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(std::move(cobj), v1, v2, v3, v4)), ReturnType>);
  }
  {
    using V = std::variant<int, long, double, int*, std::string>;
    V v1(42l), v2("hello"), v3(nullptr), v4(1.1);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(obj, v1, v2, v3, v4)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(cobj, v1, v2, v3, v4)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(std::move(obj), v1, v2, v3, v4)), ReturnType>);
    static_assert(std::is_same_v<decltype(std::visit<ReturnType>(std::move(cobj), v1, v2, v3, v4)), ReturnType>);
  }
}

void test_constexpr_void() {
  constexpr ReturnFirst obj{};
  constexpr ReturnArity aobj{};
  {
    using V = std::variant<int>;
    constexpr V v(42);
    static_assert((std::visit<void>(obj, v), 42) == 42, "");
  }
  {
    using V = std::variant<short, long, char>;
    constexpr V v(42l);
    static_assert((std::visit<void>(obj, v), 42) == 42, "");
  }
  {
    using V1 = std::variant<int>;
    using V2 = std::variant<int, char *, long long>;
    using V3 = std::variant<bool, int, int>;
    constexpr V1 v1;
    constexpr V2 v2(nullptr);
    constexpr V3 v3;
    static_assert((std::visit<void>(aobj, v1, v2, v3), 3) == 3, "");
  }
  {
    using V1 = std::variant<int>;
    using V2 = std::variant<int, char *, long long>;
    using V3 = std::variant<void *, int, int>;
    constexpr V1 v1;
    constexpr V2 v2(nullptr);
    constexpr V3 v3;
    static_assert((std::visit<void>(aobj, v1, v2, v3), 3) == 3, "");
  }
  {
    using V = std::variant<int, long, double, int *>;
    constexpr V v1(42l), v2(101), v3(nullptr), v4(1.1);
    static_assert((std::visit<void>(aobj, v1, v2, v3, v4), 4) == 4, "");
  }
  {
    using V = std::variant<int, long, double, long long, int *>;
    constexpr V v1(42l), v2(101), v3(nullptr), v4(1.1);
    static_assert((std::visit<void>(aobj, v1, v2, v3, v4), 4) == 4, "");
  }
}

void test_constexpr_int() {
  constexpr ReturnFirst obj{};
  constexpr ReturnArity aobj{};
  {
    using V = std::variant<int>;
    constexpr V v(42);
    static_assert(std::visit<int>(obj, v) == 42, "");
  }
  {
    using V = std::variant<short, long, char>;
    constexpr V v(42l);
    static_assert(std::visit<int>(obj, v) == 42, "");
  }
  {
    using V1 = std::variant<int>;
    using V2 = std::variant<int, char *, long long>;
    using V3 = std::variant<bool, int, int>;
    constexpr V1 v1;
    constexpr V2 v2(nullptr);
    constexpr V3 v3;
    static_assert(std::visit<int>(aobj, v1, v2, v3) == 3, "");
  }
  {
    using V1 = std::variant<int>;
    using V2 = std::variant<int, char *, long long>;
    using V3 = std::variant<void *, int, int>;
    constexpr V1 v1;
    constexpr V2 v2(nullptr);
    constexpr V3 v3;
    static_assert(std::visit<int>(aobj, v1, v2, v3) == 3, "");
  }
  {
    using V = std::variant<int, long, double, int *>;
    constexpr V v1(42l), v2(101), v3(nullptr), v4(1.1);
    static_assert(std::visit<int>(aobj, v1, v2, v3, v4) == 4, "");
  }
  {
    using V = std::variant<int, long, double, long long, int *>;
    constexpr V v1(42l), v2(101), v3(nullptr), v4(1.1);
    static_assert(std::visit<int>(aobj, v1, v2, v3, v4) == 4, "");
  }
}

template <typename ReturnType>
void test_exceptions() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  ReturnArity obj{};
  auto test = [&](auto &&... args) {
    try {
      std::visit<ReturnType>(obj, args...);
    } catch (const std::bad_variant_access &) {
      return true;
    } catch (...) {
    }
    return false;
  };
  {
    using V = std::variant<int, MakeEmptyT>;
    V v;
    makeEmpty(v);
    assert(test(v));
  }
  {
    using V = std::variant<int, MakeEmptyT>;
    using V2 = std::variant<long, std::string, void *>;
    V v;
    makeEmpty(v);
    V2 v2("hello");
    assert(test(v, v2));
  }
  {
    using V = std::variant<int, MakeEmptyT>;
    using V2 = std::variant<long, std::string, void *>;
    V v;
    makeEmpty(v);
    V2 v2("hello");
    assert(test(v2, v));
  }
  {
    using V = std::variant<int, MakeEmptyT>;
    using V2 = std::variant<long, std::string, void *, MakeEmptyT>;
    V v;
    makeEmpty(v);
    V2 v2;
    makeEmpty(v2);
    assert(test(v, v2));
  }
  {
    using V = std::variant<int, long, double, MakeEmptyT>;
    V v1(42l), v2(101), v3(202), v4(1.1);
    makeEmpty(v1);
    assert(test(v1, v2, v3, v4));
  }
  {
    using V = std::variant<int, long, double, long long, MakeEmptyT>;
    V v1(42l), v2(101), v3(202), v4(1.1);
    makeEmpty(v1);
    makeEmpty(v2);
    makeEmpty(v3);
    makeEmpty(v4);
    assert(test(v1, v2, v3, v4));
  }
#endif
}

// See https://bugs.llvm.org/show_bug.cgi?id=31916
template <typename ReturnType>
void test_caller_accepts_nonconst() {
  struct A {};
  struct Visitor {
    auto operator()(A&) {
      if constexpr (!std::is_void_v<ReturnType>)
      {
        return ReturnType{};
      }
    }
  };
  std::variant<A> v;
  std::visit<ReturnType>(Visitor{}, v);
}

void test_constexpr_explicit_side_effect() {
  auto test_lambda = [](int arg) constexpr {
    std::variant<int> v = 101;
    std::visit<void>([arg](int& x) constexpr { x = arg; }, v);
    return std::get<int>(v);
  };

  static_assert(test_lambda(202) == 202, "");
}

void test_derived_from_variant() {
  struct MyVariant : std::variant<short, long, float> {};

  std::visit<bool>(
      [](auto x) {
        assert(x == 42);
        return true;
      },
      MyVariant{42});
  std::visit<bool>(
      [](auto x) {
        assert(x == -1.3f);
        return true;
      },
      MyVariant{-1.3f});

  // Check that visit does not take index nor valueless_by_exception members from the base class.
  struct EvilVariantBase {
    int index;
    char valueless_by_exception;
  };

  struct EvilVariant1 : std::variant<int, long, double>,
                        std::tuple<int>,
                        EvilVariantBase {
    using std::variant<int, long, double>::variant;
  };

  std::visit<bool>(
      [](auto x) {
        assert(x == 12);
        return true;
      },
      EvilVariant1{12});
  std::visit<bool>(
      [](auto x) {
        assert(x == 12.3);
        return true;
      },
      EvilVariant1{12.3});

  // Check that visit unambiguously picks the variant, even if the other base has __impl member.
  struct ImplVariantBase {
    struct Callable {
      bool operator()() const { assert(false); return false; }
    };

    Callable __impl;
  };

  struct EvilVariant2 : std::variant<int, long, double>, ImplVariantBase {
    using std::variant<int, long, double>::variant;
  };

  std::visit<bool>(
      [](auto x) {
        assert(x == 12);
        return true;
      },
      EvilVariant2{12});
  std::visit<bool>(
      [](auto x) {
        assert(x == 12.3);
        return true;
      },
      EvilVariant2{12.3});
}

struct any_visitor {
  template <typename T>
  bool operator()(const T&) {
    return true;
  }
};

template <typename T, typename = decltype(std::visit<bool>(
                          std::declval<any_visitor&>(), std::declval<T>()))>
constexpr bool has_visit(int) {
  return true;
}

template <typename T>
constexpr bool has_visit(...) {
  return false;
}

void test_sfinae() {
  struct BadVariant : std::variant<short>, std::variant<long, float> {};

  static_assert(has_visit<std::variant<int> >(int()));
  static_assert(has_visit<BadVariant>(int()) == is_permissive);
}

int run_test() {
  test_call_operator_forwarding<void>();
  test_argument_forwarding<void>();
  test_return_type<void>();
  test_constexpr_void();
  test_exceptions<void>();
  test_caller_accepts_nonconst<void>();
  test_call_operator_forwarding<int>();
  test_argument_forwarding<int>();
  test_return_type<int>();
  test_constexpr_int();
  test_exceptions<int>();
  test_caller_accepts_nonconst<int>();
  test_constexpr_explicit_side_effect();
  test_derived_from_variant();
  test_sfinae();

  return 0;
}
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
int run_test() {
  return 0;
}
#endif // ^^^ !_HAS_CXX20 ^^^
} // namespace visit::return_type
// -- END: test/std/utilities/variant/variant.visit/visit_return_type.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.visit.member/robust_against_adl.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// The tested functionality needs deducing this.



// <variant>

// class variant;
// template<class Self, class Visitor>
//   constexpr decltype(auto) visit(this Self&&, Visitor&&); // since C++26
// template<class R, class Self, class Visitor>
//   constexpr R visit(this Self&&, Visitor&&);              // since C++26

#include <variant>

#include "test_macros.h"

namespace member_visit::robust_against_adl {
#if __cpp_lib_variant >= 202306L
struct Incomplete;
template <class T>
struct Holder {
  T t;
};

constexpr bool test(bool do_it) {
  if (do_it) {
    std::variant<Holder<Incomplete>*, int> v = nullptr;

    v.visit([](auto) {});
    v.visit([](auto) -> Holder<Incomplete>* { return nullptr; });
    v.visit<void>([](auto) {});
    v.visit<void*>([](auto) -> Holder<Incomplete>* { return nullptr; });
  }
  return true;
}

int run_test() {
  test(true);
  static_assert(test(true));

  return 0;
}
#else // ^^^ P2637R3 available / P2637R3 unavailable vvv
int run_test() {
  return 0;
}
#endif // ^^^ P2637R3 unavailable ^^^
} // namespace member_visit::robust_against_adl
// -- END: test/std/utilities/variant/variant.visit.member/robust_against_adl.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.visit.member/visit.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// The tested functionality needs deducing this.



// <variant>

// class variant;

// template<class Self, class Visitor>
//   constexpr decltype(auto) visit(this Self&&, Visitor&&); // since C++26

#include <cassert>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#include "test_macros.h"
#include "variant_test_helpers.h"

namespace member_visit {
#if __cpp_lib_variant >= 202306L
void test_call_operator_forwarding() {
  using Fn = ForwardingCallObject;
  Fn obj{};
  const Fn& cobj = obj;

  { // test call operator forwarding - single variant, single arg
    using V = std::variant<int>;
    V v(42);

    v.visit(obj);
    assert(Fn::check_call<int&>(CT_NonConst | CT_LValue));
    v.visit(cobj);
    assert(Fn::check_call<int&>(CT_Const | CT_LValue));
    v.visit(std::move(obj));
    assert(Fn::check_call<int&>(CT_NonConst | CT_RValue));
    v.visit(std::move(cobj));
    assert(Fn::check_call<int&>(CT_Const | CT_RValue));
  }
  { // test call operator forwarding - single variant, multi arg
    using V = std::variant<int, long, double>;
    V v(42L);

    v.visit(obj);
    assert(Fn::check_call<long&>(CT_NonConst | CT_LValue));
    v.visit(cobj);
    assert(Fn::check_call<long&>(CT_Const | CT_LValue));
    v.visit(std::move(obj));
    assert(Fn::check_call<long&>(CT_NonConst | CT_RValue));
    v.visit(std::move(cobj));
    assert(Fn::check_call<long&>(CT_Const | CT_RValue));
  }
}

// Applies to non-member `std::visit` only.
void test_argument_forwarding() {
  using Fn = ForwardingCallObject;
  Fn obj{};
  const auto val = CT_LValue | CT_NonConst;

  { // single argument - value type
    using V = std::variant<int>;
    V v(42);
    const V& cv = v;

    v.visit(obj);
    assert(Fn::check_call<int&>(val));
    cv.visit(obj);
    assert(Fn::check_call<const int&>(val));
    std::move(v).visit(obj);
    assert(Fn::check_call<int&&>(val));
    std::move(cv).visit(obj);
    assert(Fn::check_call<const int&&>(val));
  }
}

void test_return_type() {
  using Fn = ForwardingCallObject;
  Fn obj{};
  const Fn& cobj = obj;

  { // test call operator forwarding - single variant, single arg
    using V = std::variant<int>;
    V v(42);

    static_assert(std::is_same_v<decltype(v.visit(obj)), Fn&>);
    static_assert(std::is_same_v<decltype(v.visit(cobj)), const Fn&>);
    static_assert(std::is_same_v<decltype(v.visit(std::move(obj))), Fn&&>);
    static_assert(std::is_same_v<decltype(v.visit(std::move(cobj))), const Fn&&>);
  }
  { // test call operator forwarding - single variant, multi arg
    using V = std::variant<int, long, double>;
    V v(42L);

    static_assert(std::is_same_v<decltype(v.visit(obj)), Fn&>);
    static_assert(std::is_same_v<decltype(v.visit(cobj)), const Fn&>);
    static_assert(std::is_same_v<decltype(v.visit(std::move(obj))), Fn&&>);
    static_assert(std::is_same_v<decltype(v.visit(std::move(cobj))), const Fn&&>);
  }
}

void test_constexpr() {
  constexpr ReturnFirst obj{};

  {
    using V = std::variant<int>;
    constexpr V v(42);

    static_assert(v.visit(obj) == 42);
  }
  {
    using V = std::variant<short, long, char>;
    constexpr V v(42L);

    static_assert(v.visit(obj) == 42);
  }
}

void test_exceptions() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  ReturnArity obj{};

  auto test = [&](auto&& v) {
    try {
      v.visit(obj);
    } catch (const std::bad_variant_access&) {
      return true;
    } catch (...) {
    }
    return false;
  };

  {
    using V = std::variant<int, MakeEmptyT>;
    V v;
    makeEmpty(v);

    assert(test(v));
  }
#endif
}

// See https://llvm.org/PR31916
void test_caller_accepts_nonconst() {
  struct A {};
  struct Visitor {
    void operator()(A&) {}
  };
  std::variant<A> v;

  v.visit(Visitor{});
}

struct MyVariant : std::variant<short, long, float> {};
} // namespace member_visit

namespace std {
template <std::size_t Index>
void get(const ::member_visit::MyVariant&) {
  assert(false);
}
} // namespace std

namespace member_visit {
void test_derived_from_variant() {
  auto v1        = MyVariant{42};
  const auto cv1 = MyVariant{142};

  v1.visit([](auto x) { assert(x == 42); });
  cv1.visit([](auto x) { assert(x == 142); });
  MyVariant{-1.25f}.visit([](auto x) { assert(x == -1.25f); });
  std::move(v1).visit([](auto x) { assert(x == 42); });
  std::move(cv1).visit([](auto x) { assert(x == 142); });

  // Check that visit does not take index nor valueless_by_exception members from the base class.
  struct EvilVariantBase {
    int index;
    char valueless_by_exception;
  };

  struct EvilVariant1 : std::variant<int, long, double>, std::tuple<int>, EvilVariantBase {
    using std::variant<int, long, double>::variant;
  };

  EvilVariant1{12}.visit([](auto x) { assert(x == 12); });
  EvilVariant1{12.3}.visit([](auto x) { assert(x == 12.3); });

  // Check that visit unambiguously picks the variant, even if the other base has __impl member.
  struct ImplVariantBase {
    struct Callable {
      bool operator()() const {
        assert(false);
        return false;
      }
    };

    Callable __impl;
  };

  struct EvilVariant2 : std::variant<int, long, double>, ImplVariantBase {
    using std::variant<int, long, double>::variant;
  };

  EvilVariant2{12}.visit([](auto x) { assert(x == 12); });
  EvilVariant2{12.3}.visit([](auto x) { assert(x == 12.3); });
}

int run_test() {
  test_call_operator_forwarding();
  test_argument_forwarding();
  test_return_type();
  test_constexpr();
  test_exceptions();
  test_caller_accepts_nonconst();
  test_derived_from_variant();

  return 0;
}
#else // ^^^ P2637R3 available / P2637R3 unavailable vvv
int run_test() {
  return 0;
}
#endif // ^^^ P2637R3 unavailable ^^^
} // namespace member_visit
// -- END: test/std/utilities/variant/variant.visit.member/visit.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.visit.member/visit_return_type.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// The tested functionality needs deducing this.



// <variant>

// class variant;

// template<class R, class Self, class Visitor>
//   constexpr R visit(this Self&&, Visitor&&);              // since C++26

#include <cassert>
#include <memory>
#include <string>
#include <type_traits>
#include <tuple>
#include <utility>
#include <variant>

#include "test_macros.h"
#include "variant_test_helpers.h"

namespace member_visit::return_type {
#if __cpp_lib_variant >= 202306L
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

void test_overload_ambiguity() {
  using V = std::variant<float, long, std::string>;
  using namespace std::string_literals;
  V v{"baba"s};

  v.visit(
      overloaded{[]([[maybe_unused]] auto x) { assert(false); }, [](const std::string& x) { assert(x == "baba"s); }});
  assert(std::get<std::string>(v) == "baba"s);

  // Test the constraint.
  v = std::move(v).visit<V>(overloaded{
      []([[maybe_unused]] auto x) {
        assert(false);
        return 0;
      },
      [](const std::string& x) {
        assert(x == "baba"s);
        return x + " zmt"s;
      }});
  assert(std::get<std::string>(v) == "baba zmt"s);
}

template <typename ReturnType>
void test_call_operator_forwarding() {
  using Fn = ForwardingCallObject;
  Fn obj{};
  const Fn& cobj = obj;

  { // test call operator forwarding - single variant, single arg
    using V = std::variant<int>;
    V v(42);

    v.visit<ReturnType>(obj);
    assert(Fn::check_call<int&>(CT_NonConst | CT_LValue));
    v.visit<ReturnType>(cobj);
    assert(Fn::check_call<int&>(CT_Const | CT_LValue));
    v.visit<ReturnType>(std::move(obj));
    assert(Fn::check_call<int&>(CT_NonConst | CT_RValue));
    v.visit<ReturnType>(std::move(cobj));
    assert(Fn::check_call<int&>(CT_Const | CT_RValue));
  }
  { // test call operator forwarding - single variant, multi arg
    using V = std::variant<int, long, double>;
    V v(42L);

    v.visit<ReturnType>(obj);
    assert(Fn::check_call<long&>(CT_NonConst | CT_LValue));
    v.visit<ReturnType>(cobj);
    assert(Fn::check_call<long&>(CT_Const | CT_LValue));
    v.visit<ReturnType>(std::move(obj));
    assert(Fn::check_call<long&>(CT_NonConst | CT_RValue));
    v.visit<ReturnType>(std::move(cobj));
    assert(Fn::check_call<long&>(CT_Const | CT_RValue));
  }
}

template <typename ReturnType>
void test_argument_forwarding() {
  using Fn = ForwardingCallObject;
  Fn obj{};
  const auto val = CT_LValue | CT_NonConst;

  { // single argument - value type
    using V = std::variant<int>;
    V v(42);
    const V& cv = v;

    v.visit<ReturnType>(obj);
    assert(Fn::check_call<int&>(val));
    cv.visit<ReturnType>(obj);
    assert(Fn::check_call<const int&>(val));
    std::move(v).visit<ReturnType>(obj);
    assert(Fn::check_call<int&&>(val));
    std::move(cv).visit<ReturnType>(obj);
    assert(Fn::check_call<const int&&>(val));
  }
}

template <typename ReturnType>
void test_return_type() {
  using Fn = ForwardingCallObject;
  Fn obj{};
  const Fn& cobj = obj;

  { // test call operator forwarding - no variant
    // non-member
    {
      static_assert(std::is_same_v<decltype(std::visit<ReturnType>(obj)), ReturnType>);
      static_assert(std::is_same_v<decltype(std::visit<ReturnType>(cobj)), ReturnType>);
      static_assert(std::is_same_v<decltype(std::visit<ReturnType>(std::move(obj))), ReturnType>);
      static_assert(std::is_same_v<decltype(std::visit<ReturnType>(std::move(cobj))), ReturnType>);
    }
  }
  { // test call operator forwarding - single variant, single arg
    using V = std::variant<int>;
    V v(42);

    static_assert(std::is_same_v<decltype(v.visit<ReturnType>(obj)), ReturnType>);
    static_assert(std::is_same_v<decltype(v.visit<ReturnType>(cobj)), ReturnType>);
    static_assert(std::is_same_v<decltype(v.visit<ReturnType>(std::move(obj))), ReturnType>);
    static_assert(std::is_same_v<decltype(v.visit<ReturnType>(std::move(cobj))), ReturnType>);
  }
  { // test call operator forwarding - single variant, multi arg
    using V = std::variant<int, long, double>;
    V v(42L);

    static_assert(std::is_same_v<decltype(v.visit<ReturnType>(obj)), ReturnType>);
    static_assert(std::is_same_v<decltype(v.visit<ReturnType>(cobj)), ReturnType>);
    static_assert(std::is_same_v<decltype(v.visit<ReturnType>(std::move(obj))), ReturnType>);
    static_assert(std::is_same_v<decltype(v.visit<ReturnType>(std::move(cobj))), ReturnType>);
  }
}

void test_constexpr_void() {
  constexpr ReturnFirst obj{};

  {
    using V = std::variant<int>;
    constexpr V v(42);

    static_assert((v.visit<void>(obj), 42) == 42);
  }
  {
    using V = std::variant<short, long, char>;
    constexpr V v(42L);

    static_assert((v.visit<void>(obj), 42) == 42);
  }
}

void test_constexpr_int() {
  constexpr ReturnFirst obj{};

  {
    using V = std::variant<int>;
    constexpr V v(42);

    static_assert(v.visit<int>(obj) == 42);
  }
  {
    using V = std::variant<short, long, char>;
    constexpr V v(42L);

    static_assert(v.visit<int>(obj) == 42);
  }
}

template <typename ReturnType>
void test_exceptions() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  ReturnArity obj{};

  auto test = [&](auto&& v) {
    try {
      v.template visit<ReturnType>(obj);
    } catch (const std::bad_variant_access&) {
      return true;
    } catch (...) {
    }
    return false;
  };

  {
    using V = std::variant<int, MakeEmptyT>;
    V v;
    makeEmpty(v);

    assert(test(v));
  }
#endif
}

// See https://bugs.llvm.org/show_bug.cgi?id=31916
template <typename ReturnType>
void test_caller_accepts_nonconst() {
  struct A {};
  struct Visitor {
    auto operator()(A&) {
      if constexpr (!std::is_void_v<ReturnType>) {
        return ReturnType{};
      }
    }
  };
  std::variant<A> v;

  v.template visit<ReturnType>(Visitor{});
}

void test_constexpr_explicit_side_effect() {
  auto test_lambda = [](int arg) constexpr {
    std::variant<int> v = 101;

    {
      v.template visit<void>([arg](int& x) constexpr { x = arg; });
    }

    return std::get<int>(v);
  };

  static_assert(test_lambda(202) == 202);
}

void test_derived_from_variant() {
  struct MyVariant : std::variant<short, long, float> {};

  MyVariant{42}.template visit<bool>([](auto x) {
    assert(x == 42);
    return true;
  });
  MyVariant{-1.3f}.template visit<bool>([](auto x) {
    assert(x == -1.3f);
    return true;
  });

  // Check that visit does not take index nor valueless_by_exception members from the base class.
  struct EvilVariantBase {
    int index;
    char valueless_by_exception;
  };

  struct EvilVariant1 : std::variant<int, long, double>, std::tuple<int>, EvilVariantBase {
    using std::variant<int, long, double>::variant;
  };

  EvilVariant1{12}.template visit<bool>([](auto x) {
    assert(x == 12);
    return true;
  });
  EvilVariant1{12.3}.template visit<bool>([](auto x) {
    assert(x == 12.3);
    return true;
  });

  // Check that visit unambiguously picks the variant, even if the other base has __impl member.
  struct ImplVariantBase {
    struct Callable {
      bool operator()() const {
        assert(false);
        return false;
      }
    };

    Callable __impl;
  };

  struct EvilVariant2 : std::variant<int, long, double>, ImplVariantBase {
    using std::variant<int, long, double>::variant;
  };

  EvilVariant2{12}.template visit<bool>([](auto x) {
    assert(x == 12);
    return true;
  });
  EvilVariant2{12.3}.template visit<bool>([](auto x) {
    assert(x == 12.3);
    return true;
  });
}

int run_test() {
  test_overload_ambiguity();
  test_call_operator_forwarding<void>();
  test_argument_forwarding<void>();
  test_return_type<void>();
  test_constexpr_void();
  test_exceptions<void>();
  test_caller_accepts_nonconst<void>();
  test_call_operator_forwarding<int>();
  test_argument_forwarding<int>();
  test_return_type<int>();
  test_constexpr_int();
  test_exceptions<int>();
  test_caller_accepts_nonconst<int>();
  test_constexpr_explicit_side_effect();
  test_derived_from_variant();

  return 0;
}
#else // ^^^ P2637R3 available / P2637R3 unavailable vvv
int run_test() {
  return 0;
}
#endif // ^^^ P2637R3 unavailable ^^^
} // namespace member_visit::return_type
// -- END: test/std/utilities/variant/variant.visit.member/visit_return_type.pass.cpp
// LLVM SOURCES END
// clang-format on

int main() {
    bad_variant_access::run_test();

    get::get_if_index::run_test();
    get::get_if_type::run_test();
    get::get_index::run_test();
    get::get_type::run_test();
    get::holds_alternative::run_test();

    hash::enabled_hash::run_test();
    hash::run_test();

    helpers::variant_alternative::run_test();
    helpers::variant_size::run_test();

    monostate::properties::run_test();
    monostate::relops::run_test();

    relops::relops::run_test();
    relops::three_way::run_test();

    npos::run_test();

    implicit_ctad::run_test();

    assign::conv::run_test();
    assign::copy::run_test();
    assign::move::run_test();
    assign::T::run_test();

    ctor::conv::run_test();
    ctor::copy::run_test();
    ctor::default_::run_test();
    ctor::in_place_index_args::run_test();
    ctor::in_place_index_init_list_args::run_test();
    ctor::in_place_type_args::run_test();
    ctor::in_place_type_init_list_args::run_test();
    ctor::move::run_test();
    ctor::T::run_test();

    dtor::run_test();

    emplace::index_args::run_test();
    emplace::index_init_list_args::run_test();
    emplace::type_args::run_test();
    emplace::type_init_list_args::run_test();

    status::index::run_test();
    status::valueless_by_exception::run_test();

    member_swap::run_test();

    visit::robust_against_adl::run_test();
    visit::run_test();
    visit::return_type::run_test();

    member_visit::robust_against_adl::run_test();
    member_visit::run_test();
    member_visit::return_type::run_test();
}
#else // ^^^ not x86 or not /analyze / x86 /analyze vvv
int main() {}
#endif // !defined(_PREFAST_) || !defined(_M_IX86)
