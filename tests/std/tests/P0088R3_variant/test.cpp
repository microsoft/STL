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
// 1. Navigate a bash prompt to `libcxx` in an LLVM monorepo.
// 2. Redirect the output of the bash loop:
//      for f in $(find test/std/utilities/variant -name '*.pass.cpp' -a -not -name '*nothing_to_do*');
//        do echo "// -- BEGIN: $f";
//        sed -e 's/int main(int, char\*\*)/int run_test()/; s/FIXME/TODO/g' < $f;
//        echo -e "// -- END: $f\n";
//      done
//    into a file.
// 3. Replicate the namespace structure from here into that file, use its content to replace everything between the
//    "LLVM SOURCES BEGIN"/"END" delimiters, and ensure that `main` properly calls each of the `run_test` functions.
// 4. You'll need to fixup the specializations of std::hash in test/std/utilities/variant/variant.hash/hash.pass.cpp,
//    and preserve the `// TRANSITION, P0608` conditionals if we still haven't implemented P0608.
//
// Yes, this is an awkward hand process; notably the required headers can change without notice. We should investigate
// running the libc++ tests directly in all of our configurations so we needn't replicate this subset of files.

#if !defined(_PREFAST_) || !defined(_M_IX86) // TRANSITION, VSO-1639191
#ifdef CONSTEXPR_NOTHROW
#define TEST_WORKAROUND_CONSTEXPR_IMPLIES_NOEXCEPT
#endif // CONSTEXPR_NOTHROW

#define _HAS_DEPRECATED_RESULT_OF 1
#define _SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
#define _SILENCE_CXX20_VOLATILE_DEPRECATION_WARNING
#define _SILENCE_CXX23_ALIGNED_UNION_DEPRECATION_WARNING
#define _LIBCXX_IN_DEVCRT
#include <msvc_stdlib_force_include.h> // Must precede any other libc++ headers

// clang-format off
// LLVM SOURCES BEGIN
// -- BEGIN: test/std/utilities/variant/variant.bad_variant_access/bad_variant_access.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_variant_access


// <variant>

/*

 class bad_variant_access : public exception {
public:
  bad_variant_access() noexcept;
  virtual const char* what() const noexcept;
};

*/

#include <stdlib.h>
#include <cassert>
#include <exception>
#include <type_traits>
#include <variant>

#include "test_macros.h"

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

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

namespace get_if::index {
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
// TODO: Remove these once reference support is reinstated
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int &>;
    int x = 42;
    const V v(x);
    ASSERT_SAME_TYPE(decltype(std::get_if<0>(&v)), int *);
    assert(std::get_if<0>(&v) == &x);
  }
  {
    using V = std::variant<int &&>;
    int x = 42;
    const V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get_if<0>(&v)), int *);
    assert(std::get_if<0>(&v) == &x);
  }
  {
    using V = std::variant<const int &&>;
    int x = 42;
    const V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get_if<0>(&v)), const int *);
    assert(std::get_if<0>(&v) == &x);
  }
#endif
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
// TODO: Remove these once reference support is reinstated
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int &>;
    int x = 42;
    V v(x);
    ASSERT_SAME_TYPE(decltype(std::get_if<0>(&v)), int *);
    assert(std::get_if<0>(&v) == &x);
  }
  {
    using V = std::variant<const int &>;
    int x = 42;
    V v(x);
    ASSERT_SAME_TYPE(decltype(std::get_if<0>(&v)), const int *);
    assert(std::get_if<0>(&v) == &x);
  }
  {
    using V = std::variant<int &&>;
    int x = 42;
    V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get_if<0>(&v)), int *);
    assert(std::get_if<0>(&v) == &x);
  }
  {
    using V = std::variant<const int &&>;
    int x = 42;
    V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get_if<0>(&v)), const int *);
    assert(std::get_if<0>(&v) == &x);
  }
#endif
}

int run_test() {
  test_const_get_if();
  test_get_if();

  return 0;
}
} // namespace get_if::index
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

namespace get_if::type {
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
// TODO: Remove these once reference support is reinstated
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int &>;
    int x = 42;
    const V v(x);
    ASSERT_SAME_TYPE(decltype(std::get_if<int &>(&v)), int *);
    assert(std::get_if<int &>(&v) == &x);
  }
  {
    using V = std::variant<int &&>;
    int x = 42;
    const V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get_if<int &&>(&v)), int *);
    assert(std::get_if<int &&>(&v) == &x);
  }
  {
    using V = std::variant<const int &&>;
    int x = 42;
    const V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get_if<const int &&>(&v)), const int *);
    assert(std::get_if<const int &&>(&v) == &x);
  }
#endif
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
// TODO: Remove these once reference support is reinstated
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int &>;
    int x = 42;
    V v(x);
    ASSERT_SAME_TYPE(decltype(std::get_if<int &>(&v)), int *);
    assert(std::get_if<int &>(&v) == &x);
  }
  {
    using V = std::variant<const int &>;
    int x = 42;
    V v(x);
    ASSERT_SAME_TYPE(decltype(std::get_if<const int &>(&v)), const int *);
    assert(std::get_if<const int &>(&v) == &x);
  }
  {
    using V = std::variant<int &&>;
    int x = 42;
    V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get_if<int &&>(&v)), int *);
    assert(std::get_if<int &&>(&v) == &x);
  }
  {
    using V = std::variant<const int &&>;
    int x = 42;
    V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get_if<const int &&>(&v)), const int *);
    assert(std::get_if<const int &&>(&v) == &x);
  }
#endif
}

int run_test() {
  test_const_get_if();
  test_get_if();

  return 0;
}
} // namespace get_if::type
// -- END: test/std/utilities/variant/variant.get/get_if_type.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.get/get_index.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

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

namespace get::index {
void test_const_lvalue_get() {
  {
    using V = std::variant<int, const long>;
    constexpr V v(42);
#ifdef TEST_WORKAROUND_CONSTEXPR_IMPLIES_NOEXCEPT
    ASSERT_NOEXCEPT(std::get<0>(v));
#else
    ASSERT_NOT_NOEXCEPT(std::get<0>(v));
#endif
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
#ifdef TEST_WORKAROUND_CONSTEXPR_IMPLIES_NOEXCEPT
    ASSERT_NOEXCEPT(std::get<1>(v));
#else
    ASSERT_NOT_NOEXCEPT(std::get<1>(v));
#endif
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
// TODO: Remove these once reference support is reinstated
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int &>;
    int x = 42;
    const V v(x);
    ASSERT_SAME_TYPE(decltype(std::get<0>(v)), int &);
    assert(&std::get<0>(v) == &x);
  }
  {
    using V = std::variant<int &&>;
    int x = 42;
    const V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get<0>(v)), int &);
    assert(&std::get<0>(v) == &x);
  }
  {
    using V = std::variant<const int &&>;
    int x = 42;
    const V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get<0>(v)), const int &);
    assert(&std::get<0>(v) == &x);
  }
#endif
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
// TODO: Remove these once reference support is reinstated
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int &>;
    int x = 42;
    V v(x);
    ASSERT_SAME_TYPE(decltype(std::get<0>(v)), int &);
    assert(&std::get<0>(v) == &x);
  }
  {
    using V = std::variant<const int &>;
    int x = 42;
    V v(x);
    ASSERT_SAME_TYPE(decltype(std::get<0>(v)), const int &);
    assert(&std::get<0>(v) == &x);
  }
  {
    using V = std::variant<int &&>;
    int x = 42;
    V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get<0>(v)), int &);
    assert(&std::get<0>(v) == &x);
  }
  {
    using V = std::variant<const int &&>;
    int x = 42;
    V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get<0>(v)), const int &);
    assert(&std::get<0>(v) == &x);
  }
#endif
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
// TODO: Remove these once reference support is reinstated
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int &>;
    int x = 42;
    V v(x);
    ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), int &);
    assert(&std::get<0>(std::move(v)) == &x);
  }
  {
    using V = std::variant<const int &>;
    int x = 42;
    V v(x);
    ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), const int &);
    assert(&std::get<0>(std::move(v)) == &x);
  }
  {
    using V = std::variant<int &&>;
    int x = 42;
    V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), int &&);
    int &&xref = std::get<0>(std::move(v));
    assert(&xref == &x);
  }
  {
    using V = std::variant<const int &&>;
    int x = 42;
    V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), const int &&);
    const int &&xref = std::get<0>(std::move(v));
    assert(&xref == &x);
  }
#endif
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
// TODO: Remove these once reference support is reinstated
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int &>;
    int x = 42;
    const V v(x);
    ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), int &);
    assert(&std::get<0>(std::move(v)) == &x);
  }
  {
    using V = std::variant<const int &>;
    int x = 42;
    const V v(x);
    ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), const int &);
    assert(&std::get<0>(std::move(v)) == &x);
  }
  {
    using V = std::variant<int &&>;
    int x = 42;
    const V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), int &&);
    int &&xref = std::get<0>(std::move(v));
    assert(&xref == &x);
  }
  {
    using V = std::variant<const int &&>;
    int x = 42;
    const V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), const int &&);
    const int &&xref = std::get<0>(std::move(v));
    assert(&xref == &x);
  }
#endif
}

template <std::size_t I> using Idx = std::integral_constant<size_t, I>;

void test_throws_for_all_value_categories() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  using V = std::variant<int, long>;
  V v0(42);
  const V &cv0 = v0;
  assert(v0.index() == 0);
  V v1(42l);
  const V &cv1 = v1;
  assert(v1.index() == 1);
  std::integral_constant<size_t, 0> zero;
  std::integral_constant<size_t, 1> one;
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
} // namespace get::index
// -- END: test/std/utilities/variant/variant.get/get_index.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.get/get_type.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

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

namespace get::type {
void test_const_lvalue_get() {
  {
    using V = std::variant<int, const long>;
    constexpr V v(42);
#ifdef TEST_WORKAROUND_CONSTEXPR_IMPLIES_NOEXCEPT
    ASSERT_NOEXCEPT(std::get<int>(v));
#else
    ASSERT_NOT_NOEXCEPT(std::get<int>(v));
#endif
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
#ifdef TEST_WORKAROUND_CONSTEXPR_IMPLIES_NOEXCEPT
    ASSERT_NOEXCEPT(std::get<const long>(v));
#else
    ASSERT_NOT_NOEXCEPT(std::get<const long>(v));
#endif
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
// TODO: Remove these once reference support is reinstated
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int &>;
    int x = 42;
    const V v(x);
    ASSERT_SAME_TYPE(decltype(std::get<int &>(v)), int &);
    assert(&std::get<int &>(v) == &x);
  }
  {
    using V = std::variant<int &&>;
    int x = 42;
    const V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get<int &&>(v)), int &);
    assert(&std::get<int &&>(v) == &x);
  }
  {
    using V = std::variant<const int &&>;
    int x = 42;
    const V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get<const int &&>(v)), const int &);
    assert(&std::get<const int &&>(v) == &x);
  }
#endif
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
// TODO: Remove these once reference support is reinstated
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int &>;
    int x = 42;
    V v(x);
    ASSERT_SAME_TYPE(decltype(std::get<int &>(v)), int &);
    assert(&std::get<int &>(v) == &x);
  }
  {
    using V = std::variant<const int &>;
    int x = 42;
    V v(x);
    ASSERT_SAME_TYPE(decltype(std::get<const int &>(v)), const int &);
    assert(&std::get<const int &>(v) == &x);
  }
  {
    using V = std::variant<int &&>;
    int x = 42;
    V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get<int &&>(v)), int &);
    assert(&std::get<int &&>(v) == &x);
  }
  {
    using V = std::variant<const int &&>;
    int x = 42;
    V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get<const int &&>(v)), const int &);
    assert(&std::get<const int &&>(v) == &x);
  }
#endif
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
// TODO: Remove these once reference support is reinstated
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int &>;
    int x = 42;
    V v(x);
    ASSERT_SAME_TYPE(decltype(std::get<int &>(std::move(v))), int &);
    assert(&std::get<int &>(std::move(v)) == &x);
  }
  {
    using V = std::variant<const int &>;
    int x = 42;
    V v(x);
    ASSERT_SAME_TYPE(decltype(std::get<const int &>(std::move(v))),
                     const int &);
    assert(&std::get<const int &>(std::move(v)) == &x);
  }
  {
    using V = std::variant<int &&>;
    int x = 42;
    V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get<int &&>(std::move(v))), int &&);
    int &&xref = std::get<int &&>(std::move(v));
    assert(&xref == &x);
  }
  {
    using V = std::variant<const int &&>;
    int x = 42;
    V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get<const int &&>(std::move(v))),
                     const int &&);
    const int &&xref = std::get<const int &&>(std::move(v));
    assert(&xref == &x);
  }
#endif
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
// TODO: Remove these once reference support is reinstated
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int &>;
    int x = 42;
    const V v(x);
    ASSERT_SAME_TYPE(decltype(std::get<int &>(std::move(v))), int &);
    assert(&std::get<int &>(std::move(v)) == &x);
  }
  {
    using V = std::variant<const int &>;
    int x = 42;
    const V v(x);
    ASSERT_SAME_TYPE(decltype(std::get<const int &>(std::move(v))),
                     const int &);
    assert(&std::get<const int &>(std::move(v)) == &x);
  }
  {
    using V = std::variant<int &&>;
    int x = 42;
    const V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get<int &&>(std::move(v))), int &&);
    int &&xref = std::get<int &&>(std::move(v));
    assert(&xref == &x);
  }
  {
    using V = std::variant<const int &&>;
    int x = 42;
    const V v(std::move(x));
    ASSERT_SAME_TYPE(decltype(std::get<const int &&>(std::move(v))),
                     const int &&);
    const int &&xref = std::get<const int &&>(std::move(v));
    assert(&xref == &x);
  }
#endif
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
} // namespace get::type
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

namespace holds_alternative {
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
} // namespace holds_alternative
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

namespace enabled_hash {
int run_test() {
  test_library_hash_specializations_available();

  return 0;
}
} // namespace enabled_hash
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
  size_t operator()(const ::MakeEmptyT &) const {
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
    test_hash_enabled_for_type<std::monostate>();
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
  size_t operator()(::hash::B const&) const {
    return 0;
  }
};

}

namespace hash {
void test_hash_variant_enabled() {
  {
#ifndef __EDG__ // TRANSITION, DevCom-10107834
    test_hash_enabled_for_type<std::variant<int> >();
    test_hash_enabled_for_type<std::variant<int*, long, double, const int> >();
#endif // TRANSITION, DevCom-10107834
  }
  {
    test_hash_disabled_for_type<std::variant<int, A>>();
    test_hash_disabled_for_type<std::variant<const A, void*>>();
  }
  {
#ifndef __EDG__ // TRANSITION, DevCom-10107834
    test_hash_enabled_for_type<std::variant<int, B>>();
    test_hash_enabled_for_type<std::variant<const B, int>>();
#endif // TRANSITION, DevCom-10107834
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
template <class V, size_t I, class E> void test() {
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
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int, int &, const int &, int &&, long double>;
    test<V, 0, int>();
    test<V, 1, int &>();
    test<V, 2, const int &>();
    test<V, 3, int &&>();
    test<V, 4, long double>();
  }
#endif

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
template <class V, size_t E> void test() {
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

#include "test_macros.h"
#include <cassert>
#include <type_traits>
#include <variant>

namespace monostate::relops {
int run_test() {
  using M = std::monostate;
  constexpr M m1{};
  constexpr M m2{};
  {
    static_assert((m1 < m2) == false, "");
    ASSERT_NOEXCEPT(m1 < m2);
  }
  {
    static_assert((m1 > m2) == false, "");
    ASSERT_NOEXCEPT(m1 > m2);
  }
  {
    static_assert((m1 <= m2) == true, "");
    ASSERT_NOEXCEPT(m1 <= m2);
  }
  {
    static_assert((m1 >= m2) == true, "");
    ASSERT_NOEXCEPT(m1 >= m2);
  }
  {
    static_assert((m1 == m2) == true, "");
    ASSERT_NOEXCEPT(m1 == m2);
  }
  {
    static_assert((m1 != m2) == false, "");
    ASSERT_NOEXCEPT(m1 != m2);
  }

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

namespace relops {
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
} // namespace relops
// -- END: test/std/utilities/variant/variant.relops/relops.pass.cpp

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
#if _HAS_CXX20
  static_assert(std::is_assignable<std::variant<char>, int>::value == VariantAllowsNarrowingConversions, "");

  static_assert(std::is_assignable<std::variant<std::string, float>, int>::value
    == VariantAllowsNarrowingConversions, "");
  static_assert(std::is_assignable<std::variant<std::string, double>, int>::value
    == VariantAllowsNarrowingConversions, "");
  static_assert(!std::is_assignable<std::variant<std::string, bool>, int>::value, "");

  static_assert(!std::is_assignable<std::variant<int, bool>, decltype("meow")>::value, "");
  static_assert(!std::is_assignable<std::variant<int, const bool>, decltype("meow")>::value, "");
  static_assert(!std::is_assignable<std::variant<int, const volatile bool>, decltype("meow")>::value, "");

  // libc++ is missing P1957R2
  static_assert(std::is_assignable<std::variant<bool>, std::true_type>::value, "");
  static_assert(!std::is_assignable<std::variant<bool>, std::unique_ptr<char> >::value, "");
#ifndef TEST_PERMISSIVE
  static_assert(!std::is_assignable<std::variant<bool>, decltype(nullptr)>::value, "");
#endif // !TEST_PERMISSIVE
#endif // _HAS_CXX20

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


// The following compilers don't generate constexpr special members correctly.
// XFAIL: clang-3.5, clang-3.6, clang-3.7, clang-3.8
// XFAIL: apple-clang-6, apple-clang-7, apple-clang-8.0

// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

// <variant>

// template <class ...Types> class variant;

// variant& operator=(variant const&); // constexpr in C++20

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>

#include "test_macros.h"

namespace assign::copy {
struct NoCopy {
  NoCopy(const NoCopy &) = delete;
  NoCopy &operator=(const NoCopy &) = default;
};

struct CopyOnly {
  CopyOnly(const CopyOnly &) = default;
  CopyOnly(CopyOnly &&) = delete;
  CopyOnly &operator=(const CopyOnly &) = default;
  CopyOnly &operator=(CopyOnly &&) = delete;
};

struct MoveOnly {
  MoveOnly(const MoveOnly &) = delete;
  MoveOnly(MoveOnly &&) = default;
  MoveOnly &operator=(const MoveOnly &) = default;
};

struct MoveOnlyNT {
  MoveOnlyNT(const MoveOnlyNT &) = delete;
  MoveOnlyNT(MoveOnlyNT &&) {}
  MoveOnlyNT &operator=(const MoveOnlyNT &) = default;
};

struct CopyAssign {
  static int alive;
  static int copy_construct;
  static int copy_assign;
  static int move_construct;
  static int move_assign;
  static void reset() {
    copy_construct = copy_assign = move_construct = move_assign = alive = 0;
  }
  CopyAssign(int v) : value(v) { ++alive; }
  CopyAssign(const CopyAssign &o) : value(o.value) {
    ++alive;
    ++copy_construct;
  }
  CopyAssign(CopyAssign &&o) noexcept : value(o.value) {
    o.value = -1;
    ++alive;
    ++move_construct;
  }
  CopyAssign &operator=(const CopyAssign &o) {
    value = o.value;
    ++copy_assign;
    return *this;
  }
  CopyAssign &operator=(CopyAssign &&o) noexcept {
    value = o.value;
    o.value = -1;
    ++move_assign;
    return *this;
  }
  ~CopyAssign() { --alive; }
  int value;
};

int CopyAssign::alive = 0;
int CopyAssign::copy_construct = 0;
int CopyAssign::copy_assign = 0;
int CopyAssign::move_construct = 0;
int CopyAssign::move_assign = 0;

struct CopyMaybeThrows {
  CopyMaybeThrows(const CopyMaybeThrows &);
  CopyMaybeThrows &operator=(const CopyMaybeThrows &);
};
struct CopyDoesThrow {
  CopyDoesThrow(const CopyDoesThrow &) noexcept(false);
  CopyDoesThrow &operator=(const CopyDoesThrow &) noexcept(false);
};


struct NTCopyAssign {
  constexpr NTCopyAssign(int v) : value(v) {}
  NTCopyAssign(const NTCopyAssign &) = default;
  NTCopyAssign(NTCopyAssign &&) = default;
  NTCopyAssign &operator=(const NTCopyAssign &that) {
    value = that.value;
    return *this;
  };
  NTCopyAssign &operator=(NTCopyAssign &&) = delete;
  int value;
};

static_assert(!std::is_trivially_copy_assignable<NTCopyAssign>::value, "");
static_assert(std::is_copy_assignable<NTCopyAssign>::value, "");

struct TCopyAssign {
  constexpr TCopyAssign(int v) : value(v) {}
  TCopyAssign(const TCopyAssign &) = default;
  TCopyAssign(TCopyAssign &&) = default;
  TCopyAssign &operator=(const TCopyAssign &) = default;
  TCopyAssign &operator=(TCopyAssign &&) = delete;
  int value;
};

static_assert(std::is_trivially_copy_assignable<TCopyAssign>::value, "");

struct TCopyAssignNTMoveAssign {
  constexpr TCopyAssignNTMoveAssign(int v) : value(v) {}
  TCopyAssignNTMoveAssign(const TCopyAssignNTMoveAssign &) = default;
  TCopyAssignNTMoveAssign(TCopyAssignNTMoveAssign &&) = default;
  TCopyAssignNTMoveAssign &operator=(const TCopyAssignNTMoveAssign &) = default;
  TCopyAssignNTMoveAssign &operator=(TCopyAssignNTMoveAssign &&that) {
    value = that.value;
    that.value = -1;
    return *this;
  }
  int value;
};

static_assert(std::is_trivially_copy_assignable_v<TCopyAssignNTMoveAssign>, "");

#ifndef TEST_HAS_NO_EXCEPTIONS
struct CopyThrows {
  CopyThrows() = default;
  CopyThrows(const CopyThrows &) { throw 42; }
  CopyThrows &operator=(const CopyThrows &) { throw 42; }
};

struct CopyCannotThrow {
  static int alive;
  CopyCannotThrow() { ++alive; }
  CopyCannotThrow(const CopyCannotThrow &) noexcept { ++alive; }
  CopyCannotThrow(CopyCannotThrow &&) noexcept { assert(false); }
  CopyCannotThrow &operator=(const CopyCannotThrow &) noexcept = default;
  CopyCannotThrow &operator=(CopyCannotThrow &&) noexcept { assert(false); return *this; }
};

int CopyCannotThrow::alive = 0;

struct MoveThrows {
  static int alive;
  MoveThrows() { ++alive; }
  MoveThrows(const MoveThrows &) { ++alive; }
  MoveThrows(MoveThrows &&) { throw 42; }
  MoveThrows &operator=(const MoveThrows &) { return *this; }
  MoveThrows &operator=(MoveThrows &&) { throw 42; }
  ~MoveThrows() { --alive; }
};

int MoveThrows::alive = 0;

struct MakeEmptyT {
  static int alive;
  MakeEmptyT() { ++alive; }
  MakeEmptyT(const MakeEmptyT &) {
    ++alive;
    // Don't throw from the copy constructor since variant's assignment
    // operator performs a copy before committing to the assignment.
  }
  MakeEmptyT(MakeEmptyT &&) { throw 42; }
  MakeEmptyT &operator=(const MakeEmptyT &) { throw 42; }
  MakeEmptyT &operator=(MakeEmptyT &&) { throw 42; }
  ~MakeEmptyT() { --alive; }
};

int MakeEmptyT::alive = 0;

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

void test_copy_assignment_not_noexcept() {
  {
    using V = std::variant<CopyMaybeThrows>;
    static_assert(!std::is_nothrow_copy_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, CopyDoesThrow>;
    static_assert(!std::is_nothrow_copy_assignable<V>::value, "");
  }
}

void test_copy_assignment_sfinae() {
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
#if TEST_STD_VER > 17
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
#endif // > C++17
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
    V &vref = (v1 = v2);
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
    V &vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.valueless_by_exception());
    assert(v1.index() == std::variant_npos);
  }
  {
    using V = std::variant<int, MET, std::string>;
    V v1(std::in_place_index<2>, "hello");
    V v2(std::in_place_index<0>);
    makeEmpty(v2);
    V &vref = (v1 = v2);
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
    V &vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 0);
    assert(std::get<0>(v1) == 42);
  }
  {
    using V = std::variant<int, MET, std::string>;
    V v1(std::in_place_index<0>);
    makeEmpty(v1);
    V v2(std::in_place_type<std::string>, "hello");
    V &vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 2);
    assert(std::get<2>(v1) == "hello");
  }
#endif // TEST_HAS_NO_EXCEPTIONS
}

template <typename T> struct Result { size_t index; T value; };

void test_copy_assignment_same_index() {
  {
    using V = std::variant<int>;
    V v1(43);
    V v2(42);
    V &vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 0);
    assert(std::get<0>(v1) == 42);
  }
  {
    using V = std::variant<int, long, unsigned>;
    V v1(43l);
    V v2(42l);
    V &vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 1);
    assert(std::get<1>(v1) == 42);
  }
  {
    using V = std::variant<int, CopyAssign, unsigned>;
    V v1(std::in_place_type<CopyAssign>, 43);
    V v2(std::in_place_type<CopyAssign>, 42);
    CopyAssign::reset();
    V &vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 1);
    assert(std::get<1>(v1).value == 42);
    assert(CopyAssign::copy_construct == 0);
    assert(CopyAssign::move_construct == 0);
    assert(CopyAssign::copy_assign == 1);
  }
#ifndef TEST_HAS_NO_EXCEPTIONS
  using MET = MakeEmptyT;
  {
    using V = std::variant<int, MET, std::string>;
    V v1(std::in_place_type<MET>);
    MET &mref = std::get<1>(v1);
    V v2(std::in_place_type<MET>);
    try {
      v1 = v2;
      assert(false);
    } catch (...) {
    }
    assert(v1.index() == 1);
    assert(&std::get<1>(v1) == &mref);
  }
#endif // TEST_HAS_NO_EXCEPTIONS

  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
#if TEST_STD_VER > 17
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
#endif // > C++17
}

void test_copy_assignment_different_index() {
  {
    using V = std::variant<int, long, unsigned>;
    V v1(43);
    V v2(42l);
    V &vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 1);
    assert(std::get<1>(v1) == 42);
  }
  {
    using V = std::variant<int, CopyAssign, unsigned>;
    CopyAssign::reset();
    V v1(std::in_place_type<unsigned>, 43u);
    V v2(std::in_place_type<CopyAssign>, 42);
    assert(CopyAssign::copy_construct == 0);
    assert(CopyAssign::move_construct == 0);
    assert(CopyAssign::alive == 1);
    V &vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 1);
    assert(std::get<1>(v1).value == 42);
    assert(CopyAssign::alive == 2);
    assert(CopyAssign::copy_construct == 1);
    assert(CopyAssign::move_construct == 1);
    assert(CopyAssign::copy_assign == 0);
  }
#ifndef TEST_HAS_NO_EXCEPTIONS
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
    V &vref = (v1 = v2);
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
    V &vref = (v1 = v2);
    assert(&vref == &v1);
    assert(v1.index() == 2);
    assert(std::get<2>(v1) == "hello");
    assert(v2.index() == 2);
    assert(std::get<2>(v2) == "hello");
  }
#endif // TEST_HAS_NO_EXCEPTIONS

  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
#if TEST_STD_VER > 17
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
#endif // > C++17
}

template <size_t NewIdx, class ValueType>
constexpr bool test_constexpr_assign_imp(
    std::variant<long, void*, int>&& v, ValueType&& new_value)
{
  const std::variant<long, void*, int> cp(
      std::forward<ValueType>(new_value));
  v = cp;
  return v.index() == NewIdx &&
        std::get<NewIdx>(v) == std::get<NewIdx>(cp);
}

void test_constexpr_copy_assignment() {
  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
#if TEST_STD_VER > 17
  using V = std::variant<long, void*, int>;
  static_assert(std::is_trivially_copyable<V>::value, "");
  static_assert(std::is_trivially_copy_assignable<V>::value, "");
  static_assert(test_constexpr_assign_imp<0>(V(42l), 101l), "");
  static_assert(test_constexpr_assign_imp<0>(V(nullptr), 101l), "");
  static_assert(test_constexpr_assign_imp<1>(V(42l), nullptr), "");
  static_assert(test_constexpr_assign_imp<2>(V(42l), 101), "");
#endif // > C++17
}

int run_test() {
  test_copy_assignment_empty_empty();
  test_copy_assignment_non_empty_empty();
  test_copy_assignment_empty_non_empty();
  test_copy_assignment_same_index();
  test_copy_assignment_different_index();
  test_copy_assignment_sfinae();
  test_copy_assignment_not_noexcept();
  test_constexpr_copy_assignment();

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


// The following compilers don't generate constexpr special members correctly.
// XFAIL: clang-3.5, clang-3.6, clang-3.7, clang-3.8
// XFAIL: apple-clang-6, apple-clang-7, apple-clang-8.0

// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions


// <variant>

// template <class ...Types> class variant;

// variant& operator=(variant&&) noexcept(see below); // constexpr in C++20

#include <cassert>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

#include "test_macros.h"
#include "variant_test_helpers.h"

namespace assign::move {
struct NoCopy {
  NoCopy(const NoCopy &) = delete;
  NoCopy &operator=(const NoCopy &) = default;
};

struct CopyOnly {
  CopyOnly(const CopyOnly &) = default;
  CopyOnly(CopyOnly &&) = delete;
  CopyOnly &operator=(const CopyOnly &) = default;
  CopyOnly &operator=(CopyOnly &&) = delete;
};

struct MoveOnly {
  MoveOnly(const MoveOnly &) = delete;
  MoveOnly(MoveOnly &&) = default;
  MoveOnly &operator=(const MoveOnly &) = delete;
  MoveOnly &operator=(MoveOnly &&) = default;
};

struct MoveOnlyNT {
  MoveOnlyNT(const MoveOnlyNT &) = delete;
  MoveOnlyNT(MoveOnlyNT &&) {}
  MoveOnlyNT &operator=(const MoveOnlyNT &) = delete;
  MoveOnlyNT &operator=(MoveOnlyNT &&) = default;
};

struct MoveOnlyOddNothrow {
  MoveOnlyOddNothrow(MoveOnlyOddNothrow &&) noexcept(false) {}
  MoveOnlyOddNothrow(const MoveOnlyOddNothrow &) = delete;
  MoveOnlyOddNothrow &operator=(MoveOnlyOddNothrow &&) noexcept = default;
  MoveOnlyOddNothrow &operator=(const MoveOnlyOddNothrow &) = delete;
};

struct MoveAssignOnly {
  MoveAssignOnly(MoveAssignOnly &&) = delete;
  MoveAssignOnly &operator=(MoveAssignOnly &&) = default;
};

struct MoveAssign {
  static int move_construct;
  static int move_assign;
  static void reset() { move_construct = move_assign = 0; }
  MoveAssign(int v) : value(v) {}
  MoveAssign(MoveAssign &&o) : value(o.value) {
    ++move_construct;
    o.value = -1;
  }
  MoveAssign &operator=(MoveAssign &&o) {
    value = o.value;
    ++move_assign;
    o.value = -1;
    return *this;
  }
  int value;
};

int MoveAssign::move_construct = 0;
int MoveAssign::move_assign = 0;

struct NTMoveAssign {
  constexpr NTMoveAssign(int v) : value(v) {}
  NTMoveAssign(const NTMoveAssign &) = default;
  NTMoveAssign(NTMoveAssign &&) = default;
  NTMoveAssign &operator=(const NTMoveAssign &that) = default;
  NTMoveAssign &operator=(NTMoveAssign &&that) {
    value = that.value;
    that.value = -1;
    return *this;
  };
  int value;
};

static_assert(!std::is_trivially_move_assignable<NTMoveAssign>::value, "");
static_assert(std::is_move_assignable<NTMoveAssign>::value, "");

struct TMoveAssign {
  constexpr TMoveAssign(int v) : value(v) {}
  TMoveAssign(const TMoveAssign &) = delete;
  TMoveAssign(TMoveAssign &&) = default;
  TMoveAssign &operator=(const TMoveAssign &) = delete;
  TMoveAssign &operator=(TMoveAssign &&) = default;
  int value;
};

static_assert(std::is_trivially_move_assignable<TMoveAssign>::value, "");

struct TMoveAssignNTCopyAssign {
  constexpr TMoveAssignNTCopyAssign(int v) : value(v) {}
  TMoveAssignNTCopyAssign(const TMoveAssignNTCopyAssign &) = default;
  TMoveAssignNTCopyAssign(TMoveAssignNTCopyAssign &&) = default;
  TMoveAssignNTCopyAssign &operator=(const TMoveAssignNTCopyAssign &that) {
    value = that.value;
    return *this;
  }
  TMoveAssignNTCopyAssign &operator=(TMoveAssignNTCopyAssign &&) = default;
  int value;
};

static_assert(std::is_trivially_move_assignable_v<TMoveAssignNTCopyAssign>, "");

struct TrivialCopyNontrivialMove {
  TrivialCopyNontrivialMove(TrivialCopyNontrivialMove const&) = default;
  TrivialCopyNontrivialMove(TrivialCopyNontrivialMove&&) noexcept {}
  TrivialCopyNontrivialMove& operator=(TrivialCopyNontrivialMove const&) = default;
  TrivialCopyNontrivialMove& operator=(TrivialCopyNontrivialMove&&) noexcept {
    return *this;
  }
};

static_assert(std::is_trivially_copy_assignable_v<TrivialCopyNontrivialMove>, "");
static_assert(!std::is_trivially_move_assignable_v<TrivialCopyNontrivialMove>, "");


void test_move_assignment_noexcept() {
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

void test_move_assignment_sfinae() {
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
#if TEST_STD_VER > 17
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
#endif // > C++17
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
    V &vref = (v1 = std::move(v2));
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
    V &vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.valueless_by_exception());
    assert(v1.index() == std::variant_npos);
  }
  {
    using V = std::variant<int, MET, std::string>;
    V v1(std::in_place_index<2>, "hello");
    V v2(std::in_place_index<0>);
    makeEmpty(v2);
    V &vref = (v1 = std::move(v2));
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
    V &vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.index() == 0);
    assert(std::get<0>(v1) == 42);
  }
  {
    using V = std::variant<int, MET, std::string>;
    V v1(std::in_place_index<0>);
    makeEmpty(v1);
    V v2(std::in_place_type<std::string>, "hello");
    V &vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.index() == 2);
    assert(std::get<2>(v1) == "hello");
  }
#endif // TEST_HAS_NO_EXCEPTIONS
}

template <typename T> struct Result { size_t index; T value; };

void test_move_assignment_same_index() {
  {
    using V = std::variant<int>;
    V v1(43);
    V v2(42);
    V &vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.index() == 0);
    assert(std::get<0>(v1) == 42);
  }
  {
    using V = std::variant<int, long, unsigned>;
    V v1(43l);
    V v2(42l);
    V &vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.index() == 1);
    assert(std::get<1>(v1) == 42);
  }
  {
    using V = std::variant<int, MoveAssign, unsigned>;
    V v1(std::in_place_type<MoveAssign>, 43);
    V v2(std::in_place_type<MoveAssign>, 42);
    MoveAssign::reset();
    V &vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.index() == 1);
    assert(std::get<1>(v1).value == 42);
    assert(MoveAssign::move_construct == 0);
    assert(MoveAssign::move_assign == 1);
  }
#ifndef TEST_HAS_NO_EXCEPTIONS
  using MET = MakeEmptyT;
  {
    using V = std::variant<int, MET, std::string>;
    V v1(std::in_place_type<MET>);
    MET &mref = std::get<1>(v1);
    V v2(std::in_place_type<MET>);
    try {
      v1 = std::move(v2);
      assert(false);
    } catch (...) {
    }
    assert(v1.index() == 1);
    assert(&std::get<1>(v1) == &mref);
  }
#endif // TEST_HAS_NO_EXCEPTIONS

  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
#if TEST_STD_VER > 17
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
#endif // > C++17
}

void test_move_assignment_different_index() {
  {
    using V = std::variant<int, long, unsigned>;
    V v1(43);
    V v2(42l);
    V &vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.index() == 1);
    assert(std::get<1>(v1) == 42);
  }
  {
    using V = std::variant<int, MoveAssign, unsigned>;
    V v1(std::in_place_type<unsigned>, 43u);
    V v2(std::in_place_type<MoveAssign>, 42);
    MoveAssign::reset();
    V &vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.index() == 1);
    assert(std::get<1>(v1).value == 42);
    assert(MoveAssign::move_construct == 1);
    assert(MoveAssign::move_assign == 0);
  }
#ifndef TEST_HAS_NO_EXCEPTIONS
  using MET = MakeEmptyT;
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
    V &vref = (v1 = std::move(v2));
    assert(&vref == &v1);
    assert(v1.index() == 2);
    assert(std::get<2>(v1) == "hello");
  }
#endif // TEST_HAS_NO_EXCEPTIONS

  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
#if TEST_STD_VER > 17
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
#endif // > C++17
}

template <size_t NewIdx, class ValueType>
constexpr bool test_constexpr_assign_imp(
    std::variant<long, void*, int>&& v, ValueType&& new_value)
{
  std::variant<long, void*, int> v2(
      std::forward<ValueType>(new_value));
  const auto cp = v2;
  v = std::move(v2);
  return v.index() == NewIdx &&
        std::get<NewIdx>(v) == std::get<NewIdx>(cp);
}

void test_constexpr_move_assignment() {
  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
#if TEST_STD_VER > 17
  using V = std::variant<long, void*, int>;
  static_assert(std::is_trivially_copyable<V>::value, "");
  static_assert(std::is_trivially_move_assignable<V>::value, "");
  static_assert(test_constexpr_assign_imp<0>(V(42l), 101l), "");
  static_assert(test_constexpr_assign_imp<0>(V(nullptr), 101l), "");
  static_assert(test_constexpr_assign_imp<1>(V(42l), nullptr), "");
  static_assert(test_constexpr_assign_imp<2>(V(42l), 101), "");
#endif // > C++17
}

int run_test() {
  test_move_assignment_empty_empty();
  test_move_assignment_non_empty_empty();
  test_move_assignment_empty_non_empty();
  test_move_assignment_same_index();
  test_move_assignment_different_index();
  test_move_assignment_sfinae();
  test_move_assignment_noexcept();
  test_constexpr_move_assignment();

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


// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

// <variant>

// template <class ...Types> class variant;

// template <class T>
// variant& operator=(T&&) noexcept(see below);

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>
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
  ThrowsCtorT &operator=(int) noexcept { return *this; }
};

struct ThrowsAssignT {
  ThrowsAssignT(int) noexcept {}
  ThrowsAssignT &operator=(int) noexcept(false) { return *this; }
};

struct NoThrowT {
  NoThrowT(int) noexcept {}
  NoThrowT &operator=(int) noexcept { return *this; }
};

} // namespace MetaHelpers

namespace RuntimeHelpers {
#ifndef TEST_HAS_NO_EXCEPTIONS

struct ThrowsCtorT {
  int value;
  ThrowsCtorT() : value(0) {}
  ThrowsCtorT(int) noexcept(false) { throw 42; }
  ThrowsCtorT &operator=(int v) noexcept {
    value = v;
    return *this;
  }
};

struct MoveCrashes {
  int value;
  MoveCrashes(int v = 0) noexcept : value{v} {}
  MoveCrashes(MoveCrashes &&) noexcept { assert(false); }
  MoveCrashes &operator=(MoveCrashes &&) noexcept { assert(false); return *this; }
  MoveCrashes &operator=(int v) noexcept {
    value = v;
    return *this;
  }
};

struct ThrowsCtorTandMove {
  int value;
  ThrowsCtorTandMove() : value(0) {}
  ThrowsCtorTandMove(int) noexcept(false) { throw 42; }
  ThrowsCtorTandMove(ThrowsCtorTandMove &&) noexcept(false) { assert(false); }
  ThrowsCtorTandMove &operator=(int v) noexcept {
    value = v;
    return *this;
  }
};

struct ThrowsAssignT {
  int value;
  ThrowsAssignT() : value(0) {}
  ThrowsAssignT(int v) noexcept : value(v) {}
  ThrowsAssignT &operator=(int) noexcept(false) { throw 42; }
};

struct NoThrowT {
  int value;
  NoThrowT() : value(0) {}
  NoThrowT(int v) noexcept : value(v) {}
  NoThrowT &operator=(int v) noexcept {
    value = v;
    return *this;
  }
};

#endif // !defined(TEST_HAS_NO_EXCEPTIONS)
} // namespace RuntimeHelpers

void test_T_assignment_noexcept() {
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

void test_T_assignment_sfinae() {
  {
    using V = std::variant<long, long long>;
    static_assert(!std::is_assignable<V, int>::value, "ambiguous");
  }
  {
    using V = std::variant<std::string, std::string>;
    static_assert(!std::is_assignable<V, const char *>::value, "ambiguous");
  }
  {
    using V = std::variant<std::string, void *>;
    static_assert(!std::is_assignable<V, int>::value, "no matching operator=");
  }
#if _HAS_CXX20
  {
    using V = std::variant<std::string, float>;
    static_assert(std::is_assignable<V, int>::value == VariantAllowsNarrowingConversions,
    "no matching operator=");
  }
  {
    using V = std::variant<std::unique_ptr<int>, bool>;
    static_assert(!std::is_assignable<V, std::unique_ptr<char>>::value,
                  "no explicit bool in operator=");
    struct X {
      operator void*();
    };
#ifndef __EDG__ // TRANSITION, VSO-1327220
    static_assert(!std::is_assignable<V, X>::value,
                  "no boolean conversion in operator=");
#endif // !__EDG__
    // libc++ is missing P1957R2
    static_assert(std::is_assignable<V, std::false_type>::value,
                  "no converted to bool in operator=");
  }
#endif // _HAS_CXX20
  {
    struct X {};
    struct Y {
      operator X();
    };
    using V = std::variant<X>;
    static_assert(std::is_assignable<V, Y>::value,
                  "regression on user-defined conversions in operator=");
  }
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int, int &&>;
    static_assert(!std::is_assignable<V, int>::value, "ambiguous");
  }
  {
    using V = std::variant<int, const int &>;
    static_assert(!std::is_assignable<V, int>::value, "ambiguous");
  }
#endif // TEST_VARIANT_HAS_NO_REFERENCES
}

void test_T_assignment_basic() {
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
#if _HAS_CXX20
#ifndef TEST_VARIANT_ALLOWS_NARROWING_CONVERSIONS
  {
    std::variant<unsigned, long> v;
    v = 42;
    assert(v.index() == 1);
    assert(std::get<1>(v) == 42);
    v = 43u;
    assert(v.index() == 0);
    assert(std::get<0>(v) == 43);
  }
#endif
  {
    std::variant<std::string, bool> v = true;
    v = "bar";
    assert(v.index() == 0);
    assert(std::get<0>(v) == "bar");
  }
#ifndef TEST_PERMISSIVE
  {
    std::variant<bool, std::unique_ptr<int>> v;
    v = nullptr;
    assert(v.index() == 1);
    assert(std::get<1>(v) == nullptr);
  }
#endif // !TEST_PERMISSIVE
#endif // _HAS_CXX20
  {
    std::variant<bool volatile, int> v = 42;
    v = false;
    assert(v.index() == 0);
    assert(!std::get<0>(v));
    bool lvt = true;
    v = lvt;
    assert(v.index() == 0);
    assert(std::get<0>(v));
  }
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int &, int &&, long>;
    int x = 42;
    V v(43l);
    v = x;
    assert(v.index() == 0);
    assert(&std::get<0>(v) == &x);
    v = std::move(x);
    assert(v.index() == 1);
    assert(&std::get<1>(v) == &x);
    // 'long' is selected by FUN(const int &) since 'const int &' cannot bind
    // to 'int&'.
    const int &cx = x;
    v = cx;
    assert(v.index() == 2);
    assert(std::get<2>(v) == 42);
  }
#endif // TEST_VARIANT_HAS_NO_REFERENCES
}

void test_T_assignment_performs_construction() {
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

void test_T_assignment_performs_assignment() {
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

int run_test() {
  test_T_assignment_basic();
  test_T_assignment_performs_construction();
  test_T_assignment_performs_assignment();
  test_T_assignment_noexcept();
  test_T_assignment_sfinae();

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
#if _HAS_CXX20
  static_assert(std::is_constructible<std::variant<char>, int>::value == VariantAllowsNarrowingConversions, "");

  static_assert(std::is_constructible<std::variant<std::string, float>, int>::value
    == VariantAllowsNarrowingConversions, "");
  static_assert(std::is_constructible<std::variant<std::string, double>, int>::value
    == VariantAllowsNarrowingConversions, "");
  static_assert(!std::is_constructible<std::variant<std::string, bool>, int>::value, "");

  static_assert(!std::is_constructible<std::variant<int, bool>, decltype("meow")>::value, "");
  static_assert(!std::is_constructible<std::variant<int, const bool>, decltype("meow")>::value, "");
  static_assert(!std::is_constructible<std::variant<int, const volatile bool>, decltype("meow")>::value, "");

  // libc++ is missing P1957R2
  static_assert(std::is_constructible<std::variant<bool>, std::true_type>::value, "");
  static_assert(!std::is_constructible<std::variant<bool>, std::unique_ptr<char> >::value, "");
#ifndef TEST_PERMISSIVE
  static_assert(!std::is_constructible<std::variant<bool>, decltype(nullptr)>::value, "");
#endif // !TEST_PERMISSIVE
#endif // _HAS_CXX20

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


// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

// <variant>

// template <class ...Types> class variant;

// variant(variant const&); // constexpr in C++20

#include <cassert>
#include <type_traits>
#include <variant>

#include "test_macros.h"
#include "test_workarounds.h"

namespace ctor::copy {
struct NonT {
  NonT(int v) : value(v) {}
  NonT(const NonT &o) : value(o.value) {}
  int value;
};
static_assert(!std::is_trivially_copy_constructible<NonT>::value, "");

struct NoCopy {
  NoCopy(const NoCopy &) = delete;
};

struct MoveOnly {
  MoveOnly(const MoveOnly &) = delete;
  MoveOnly(MoveOnly &&) = default;
};

struct MoveOnlyNT {
  MoveOnlyNT(const MoveOnlyNT &) = delete;
  MoveOnlyNT(MoveOnlyNT &&) {}
};

struct NTCopy {
  constexpr NTCopy(int v) : value(v) {}
  NTCopy(const NTCopy &that) : value(that.value) {}
  NTCopy(NTCopy &&) = delete;
  int value;
};

static_assert(!std::is_trivially_copy_constructible<NTCopy>::value, "");
static_assert(std::is_copy_constructible<NTCopy>::value, "");

struct TCopy {
  constexpr TCopy(int v) : value(v) {}
  TCopy(TCopy const &) = default;
  TCopy(TCopy &&) = delete;
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
  MakeEmptyT(const MakeEmptyT &) {
    ++alive;
    // Don't throw from the copy constructor since variant's assignment
    // operator performs a copy before committing to the assignment.
  }
  MakeEmptyT(MakeEmptyT &&) { throw 42; }
  MakeEmptyT &operator=(const MakeEmptyT &) { throw 42; }
  MakeEmptyT &operator=(MakeEmptyT &&) { throw 42; }
  ~MakeEmptyT() { --alive; }
};

int MakeEmptyT::alive = 0;

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

void test_copy_ctor_sfinae() {
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
#if TEST_STD_VER > 17
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
#endif // > C++17
}

void test_copy_ctor_basic() {
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
#if TEST_STD_VER > 17
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
#endif // > C++17
}

void test_copy_ctor_valueless_by_exception() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  using V = std::variant<int, MakeEmptyT>;
  V v1;
  makeEmpty(v1);
  const V &cv1 = v1;
  V v(cv1);
  assert(v.valueless_by_exception());
#endif // TEST_HAS_NO_EXCEPTIONS
}

template <size_t Idx>
constexpr bool test_constexpr_copy_ctor_imp(std::variant<long, void*, const int> const& v) {
  auto v2 = v;
  return v2.index() == v.index() &&
         v2.index() == Idx &&
         std::get<Idx>(v2) == std::get<Idx>(v);
}

void test_constexpr_copy_ctor() {
  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
#if TEST_STD_VER > 17
  using V = std::variant<long, void*, const int>;
#ifdef TEST_WORKAROUND_MSVC_BROKEN_IS_TRIVIALLY_COPYABLE
  static_assert(std::is_trivially_destructible<V>::value, "");
  static_assert(std::is_trivially_copy_constructible<V>::value, "");
  static_assert(std::is_trivially_move_constructible<V>::value, "");
  static_assert(!std::is_copy_assignable<V>::value, "");
  static_assert(!std::is_move_assignable<V>::value, "");
#else // TEST_WORKAROUND_MSVC_BROKEN_IS_TRIVIALLY_COPYABLE
  static_assert(std::is_trivially_copyable<V>::value, "");
#endif // TEST_WORKAROUND_MSVC_BROKEN_IS_TRIVIALLY_COPYABLE
  static_assert(test_constexpr_copy_ctor_imp<0>(V(42l)), "");
  static_assert(test_constexpr_copy_ctor_imp<1>(V(nullptr)), "");
  static_assert(test_constexpr_copy_ctor_imp<2>(V(101)), "");
#endif // > C++17
}

int run_test() {
  test_copy_ctor_basic();
  test_copy_ctor_valueless_by_exception();
  test_copy_ctor_sfinae();
  test_constexpr_copy_ctor();
#if 0
// disable this for the moment; it fails on older compilers.
//  Need to figure out which compilers will support it.
{ // This is the motivating example from P0739R0
  std::variant<int, double> v1(3);
  std::variant v2 = v1;
  (void) v2;
}
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


// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

// <variant>

// template <class ...Types> class variant;

// constexpr variant() noexcept(see below);

#include <cassert>
#include <type_traits>
#include <variant>

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

void test_default_ctor_sfinae() {
  {
    using V = std::variant<std::monostate, int>;
    static_assert(std::is_default_constructible<V>::value, "");
  }
  {
    using V = std::variant<NonDefaultConstructible, int>;
    static_assert(!std::is_default_constructible<V>::value, "");
  }
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int &, int>;
    static_assert(!std::is_default_constructible<V>::value, "");
  }
#endif
}

void test_default_ctor_noexcept() {
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
  } catch (const int &ex) {
    assert(ex == 42);
  } catch (...) {
    assert(false);
  }
#endif
}

void test_default_ctor_basic() {
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

int run_test() {
  test_default_ctor_basic();
  test_default_ctor_sfinae();
  test_default_ctor_noexcept();
  test_default_ctor_throws();

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


// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

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

// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

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


// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

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

// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

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


// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

// <variant>

// template <class ...Types> class variant;

// variant(variant&&) noexcept(see below); // constexpr in C++20

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>

#include "test_macros.h"
#include "test_workarounds.h"

namespace ctor::move {
struct ThrowsMove {
  ThrowsMove(ThrowsMove &&) noexcept(false) {}
};

struct NoCopy {
  NoCopy(const NoCopy &) = delete;
};

struct MoveOnly {
  int value;
  MoveOnly(int v) : value(v) {}
  MoveOnly(const MoveOnly &) = delete;
  MoveOnly(MoveOnly &&) = default;
};

struct MoveOnlyNT {
  int value;
  MoveOnlyNT(int v) : value(v) {}
  MoveOnlyNT(const MoveOnlyNT &) = delete;
  MoveOnlyNT(MoveOnlyNT &&other) : value(other.value) { other.value = -1; }
};

struct NTMove {
  constexpr NTMove(int v) : value(v) {}
  NTMove(const NTMove &) = delete;
  NTMove(NTMove &&that) : value(that.value) { that.value = -1; }
  int value;
};

static_assert(!std::is_trivially_move_constructible<NTMove>::value, "");
static_assert(std::is_move_constructible<NTMove>::value, "");

struct TMove {
  constexpr TMove(int v) : value(v) {}
  TMove(const TMove &) = delete;
  TMove(TMove &&) = default;
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
  MakeEmptyT(const MakeEmptyT &) {
    ++alive;
    // Don't throw from the copy constructor since variant's assignment
    // operator performs a copy before committing to the assignment.
  }
  MakeEmptyT(MakeEmptyT &&) { throw 42; }
  MakeEmptyT &operator=(const MakeEmptyT &) { throw 42; }
  MakeEmptyT &operator=(MakeEmptyT &&) { throw 42; }
  ~MakeEmptyT() { --alive; }
};

int MakeEmptyT::alive = 0;

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

void test_move_noexcept() {
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

void test_move_ctor_sfinae() {
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
#if TEST_STD_VER > 17
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
#endif // > C++17
}

template <typename T>
struct Result { size_t index; T value; };

void test_move_ctor_basic() {
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
#if TEST_STD_VER > 17
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
#endif // > C++17
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

template <size_t Idx>
constexpr bool test_constexpr_ctor_imp(std::variant<long, void*, const int> const& v) {
  auto copy = v;
  auto v2 = std::move(copy);
  return v2.index() == v.index() &&
         v2.index() == Idx &&
        std::get<Idx>(v2) == std::get<Idx>(v);
}

void test_constexpr_move_ctor() {
  // Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
#if TEST_STD_VER > 17
  using V = std::variant<long, void*, const int>;
#ifdef TEST_WORKAROUND_MSVC_BROKEN_IS_TRIVIALLY_COPYABLE
  static_assert(std::is_trivially_destructible<V>::value, "");
  static_assert(std::is_trivially_copy_constructible<V>::value, "");
  static_assert(std::is_trivially_move_constructible<V>::value, "");
  static_assert(!std::is_copy_assignable<V>::value, "");
  static_assert(!std::is_move_assignable<V>::value, "");
#else // TEST_WORKAROUND_MSVC_BROKEN_IS_TRIVIALLY_COPYABLE
  static_assert(std::is_trivially_copyable<V>::value, "");
#endif // TEST_WORKAROUND_MSVC_BROKEN_IS_TRIVIALLY_COPYABLE
  static_assert(std::is_trivially_move_constructible<V>::value, "");
  static_assert(test_constexpr_ctor_imp<0>(V(42l)), "");
  static_assert(test_constexpr_ctor_imp<1>(V(nullptr)), "");
  static_assert(test_constexpr_ctor_imp<2>(V(101)), "");
#endif // > C++17
}

int run_test() {
  test_move_ctor_basic();
  test_move_ctor_valueless_by_exception();
  test_move_noexcept();
  test_move_ctor_sfinae();
  test_constexpr_move_ctor();

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

// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

// <variant>

// template <class ...Types> class variant;

// template <class T> constexpr variant(T&&) noexcept(see below);

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>
#include <memory>

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

struct AnyConstructible { template <typename T> AnyConstructible(T&&) {} };
struct NoConstructible { NoConstructible() = delete; };
template <class T>
struct RValueConvertibleFrom { RValueConvertibleFrom(T&&) {} };

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
    static_assert(!std::is_constructible<V, const char *>::value, "ambiguous");
  }
  {
    using V = std::variant<std::string, void *>;
    static_assert(!std::is_constructible<V, int>::value,
                  "no matching constructor");
  }
#if _HAS_CXX20
  {
    using V = std::variant<std::string, float>;
    static_assert(std::is_constructible<V, int>::value == VariantAllowsNarrowingConversions,
                  "no matching constructor");
  }
  {
    using V = std::variant<std::unique_ptr<int>, bool>;
    static_assert(!std::is_constructible<V, std::unique_ptr<char>>::value,
                  "no explicit bool in constructor");
    struct X {
      operator void*();
    };
#ifndef __EDG__ // TRANSITION, VSO-1327220
    static_assert(!std::is_constructible<V, X>::value,
                  "no boolean conversion in constructor");
#endif // !__EDG__
    // libc++ is missing P1957R2
    static_assert(std::is_constructible<V, std::false_type>::value,
                  "no converted to bool in constructor");
  }
#endif // _HAS_CXX20
  {
    struct X {};
    struct Y {
      operator X();
    };
    using V = std::variant<X>;
    static_assert(std::is_constructible<V, Y>::value,
                  "regression on user-defined conversions in constructor");
  }
  {
    using V = std::variant<AnyConstructible, NoConstructible>;
    static_assert(
        !std::is_constructible<V, std::in_place_type_t<NoConstructible>>::value,
        "no matching constructor");
    static_assert(!std::is_constructible<V, std::in_place_index_t<1>>::value,
                  "no matching constructor");
  }



#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int, int &&>;
    static_assert(!std::is_constructible<V, int>::value, "ambiguous");
  }
  {
    using V = std::variant<int, const int &>;
    static_assert(!std::is_constructible<V, int>::value, "ambiguous");
  }
#endif
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
#if _HAS_CXX20
#ifndef TEST_VARIANT_ALLOWS_NARROWING_CONVERSIONS
  {
    constexpr std::variant<unsigned, long> v(42);
    static_assert(v.index() == 1, "");
    static_assert(std::get<1>(v) == 42, "");
  }
#endif
  {
    std::variant<std::string, bool const> v = "meow";
    assert(v.index() == 0);
    assert(std::get<0>(v) == "meow");
  }
#ifndef TEST_PERMISSIVE
  {
    std::variant<bool volatile, std::unique_ptr<int>> v = nullptr;
    assert(v.index() == 1);
    assert(std::get<1>(v) == nullptr);
  }
#endif // !TEST_PERMISSIVE
#endif // _HAS_CXX20
  {
    std::variant<bool volatile const, int> v = true;
    assert(v.index() == 0);
    assert(std::get<0>(v));
  }
  {
    std::variant<RValueConvertibleFrom<int>> v1 = 42;
    assert(v1.index() == 0);

    int x = 42;
    std::variant<RValueConvertibleFrom<int>, AnyConstructible> v2 = x;
    assert(v2.index() == 1);
  }
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<const int &, int &&, long>;
    static_assert(std::is_convertible<int &, V>::value, "must be implicit");
    int x = 42;
    V v(x);
    assert(v.index() == 0);
    assert(&std::get<0>(v) == &x);
  }
  {
    using V = std::variant<const int &, int &&, long>;
    static_assert(std::is_convertible<int, V>::value, "must be implicit");
    int x = 42;
    V v(std::move(x));
    assert(v.index() == 1);
    assert(&std::get<1>(v) == &x);
  }
#endif
}

#if !_HAS_CXX20 // Narrowing check occurs with P0608R3
struct FailOnAnything {
  template <class T>
  constexpr FailOnAnything(T) { static_assert(!std::is_same<T, T>::value, ""); }
};

void test_no_narrowing_check_for_class_types() {
  using V = std::variant<int, FailOnAnything>;
  V v(42);
  assert(v.index() == 0);
  assert(std::get<0>(v) == 42);
}
#endif // Narrowing check occurs with P0608R3

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

int run_test() {
  test_T_ctor_basic();
  test_T_ctor_noexcept();
  test_T_ctor_sfinae();
#if !_HAS_CXX20 // Narrowing check occurs with P0608R3
  test_no_narrowing_check_for_class_types();
#endif // Narrowing check occurs with P0608R3
  test_construction_with_repeated_types();
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
  static int count;
  NonTDtor() = default;
  ~NonTDtor() { ++count; }
};
int NonTDtor::count = 0;
static_assert(!std::is_trivially_destructible<NonTDtor>::value, "");

struct NonTDtor1 {
  static int count;
  NonTDtor1() = default;
  ~NonTDtor1() { ++count; }
};
int NonTDtor1::count = 0;
static_assert(!std::is_trivially_destructible<NonTDtor1>::value, "");

struct TDtor {
  TDtor(const TDtor &) {} // non-trivial copy
  ~TDtor() = default;
};
static_assert(!std::is_trivially_copy_constructible<TDtor>::value, "");
static_assert(std::is_trivially_destructible<TDtor>::value, "");

int run_test() {
  {
    using V = std::variant<int, long, TDtor>;
    static_assert(std::is_trivially_destructible<V>::value, "");
  }
  {
    using V = std::variant<NonTDtor, int, NonTDtor1>;
    static_assert(!std::is_trivially_destructible<V>::value, "");
    {
      V v(std::in_place_index<0>);
      assert(NonTDtor::count == 0);
      assert(NonTDtor1::count == 0);
    }
    assert(NonTDtor::count == 1);
    assert(NonTDtor1::count == 0);
    NonTDtor::count = 0;
    { V v(std::in_place_index<1>); }
    assert(NonTDtor::count == 0);
    assert(NonTDtor1::count == 0);
    {
      V v(std::in_place_index<2>);
      assert(NonTDtor::count == 0);
      assert(NonTDtor1::count == 0);
    }
    assert(NonTDtor::count == 0);
    assert(NonTDtor1::count == 1);
  }

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


// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

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

namespace emplace::index {
template <class Var, size_t I, class... Args>
constexpr auto test_emplace_exists_imp(int) -> decltype(
    std::declval<Var>().template emplace<I>(std::declval<Args>()...), true) {
  return true;
}

template <class, size_t, class...>
constexpr auto test_emplace_exists_imp(long) -> bool {
  return false;
}

template <class Var, size_t I, class... Args> constexpr bool emplace_exists() {
  return test_emplace_exists_imp<Var, I, Args...>(0);
}

void test_emplace_sfinae() {
  {
    using V = std::variant<int, void *, const void *, TestTypes::NoCtors>;
    static_assert(emplace_exists<V, 0>(), "");
    static_assert(emplace_exists<V, 0, int>(), "");
    static_assert(!emplace_exists<V, 0, decltype(nullptr)>(),
                  "cannot construct");
    static_assert(emplace_exists<V, 1, decltype(nullptr)>(), "");
    static_assert(emplace_exists<V, 1, int *>(), "");
    static_assert(!emplace_exists<V, 1, const int *>(), "");
    static_assert(!emplace_exists<V, 1, int>(), "cannot construct");
    static_assert(emplace_exists<V, 2, const int *>(), "");
    static_assert(emplace_exists<V, 2, int *>(), "");
    static_assert(!emplace_exists<V, 3>(), "cannot construct");
  }
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int, int &, const int &, int &&, TestTypes::NoCtors>;
    static_assert(emplace_exists<V, 0>(), "");
    static_assert(emplace_exists<V, 0, int>(), "");
    static_assert(emplace_exists<V, 0, long long>(), "");
    static_assert(!emplace_exists<V, 0, int, int>(), "too many args");
    static_assert(emplace_exists<V, 1, int &>(), "");
    static_assert(!emplace_exists<V, 1>(), "cannot default construct ref");
    static_assert(!emplace_exists<V, 1, const int &>(), "cannot bind ref");
    static_assert(!emplace_exists<V, 1, int &&>(), "cannot bind ref");
    static_assert(emplace_exists<V, 2, int &>(), "");
    static_assert(emplace_exists<V, 2, const int &>(), "");
    static_assert(emplace_exists<V, 2, int &&>(), "");
    static_assert(!emplace_exists<V, 2, void *>(),
                  "not constructible from void*");
    static_assert(emplace_exists<V, 3, int>(), "");
    static_assert(!emplace_exists<V, 3, int &>(), "cannot bind ref");
    static_assert(!emplace_exists<V, 3, const int &>(), "cannot bind ref");
    static_assert(!emplace_exists<V, 3, const int &&>(), "cannot bind ref");
    static_assert(!emplace_exists<V, 4>(), "no ctors");
  }
#endif
}

void test_basic() {
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
    using V =
        std::variant<int, long, const void *, TestTypes::NoCtors, std::string>;
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
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int, long, const int &, int &&, TestTypes::NoCtors,
                           std::string>;
    const int x = 100;
    int y = 42;
    int z = 43;
    V v(std::in_place_index<0>, -1);
    // default emplace a value
    auto& ref1 = v.emplace<1>();
    static_assert(std::is_same_v<long&, decltype(ref1)>, "");
    assert(std::get<1>(v) == 0);
    assert(&ref1 == &std::get<1>(v));
    // emplace a reference
    auto& ref2 = v.emplace<2>(x);
    static_assert(std::is_same_v<&, decltype(ref)>, "");
    assert(&std::get<2>(v) == &x);
    assert(&ref2 == &std::get<2>(v));
    // emplace an rvalue reference
    auto& ref3 = v.emplace<3>(std::move(y));
    static_assert(std::is_same_v<&, decltype(ref)>, "");
    assert(&std::get<3>(v) == &y);
    assert(&ref3 == &std::get<3>(v));
    // re-emplace a new reference over the active member
    auto& ref4 = v.emplace<3>(std::move(z));
    static_assert(std::is_same_v<&, decltype(ref)>, "");
    assert(&std::get<3>(v) == &z);
    assert(&ref4 == &std::get<3>(v));
    // emplace with multiple args
    auto& ref5 = v.emplace<5>(3u, 'a');
    static_assert(std::is_same_v<std::string&, decltype(ref5)>, "");
    assert(std::get<5>(v) == "aaa");
    assert(&ref5 == &std::get<5>(v));
  }
#endif
}

int run_test() {
  test_basic();
  test_emplace_sfinae();

  return 0;
}
} // namespace emplace::index
// -- END: test/std/utilities/variant/variant.variant/variant.mod/emplace_index_args.pass.cpp

// -- BEGIN: test/std/utilities/variant/variant.variant/variant.mod/emplace_index_init_list_args.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

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
  constexpr InitListArg(std::initializer_list<int> il, int v)
      : size(il.size()), value(v) {}
};

template <class Var, size_t I, class... Args>
constexpr auto test_emplace_exists_imp(int) -> decltype(
    std::declval<Var>().template emplace<I>(std::declval<Args>()...), true) {
  return true;
}

template <class, size_t, class...>
constexpr auto test_emplace_exists_imp(long) -> bool {
  return false;
}

template <class Var, size_t I, class... Args> constexpr bool emplace_exists() {
  return test_emplace_exists_imp<Var, I, Args...>(0);
}

void test_emplace_sfinae() {
  using V =
      std::variant<int, TestTypes::NoCtors, InitList, InitListArg, long, long>;
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

void test_basic() {
  using V = std::variant<int, InitList, InitListArg, TestTypes::NoCtors>;
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

int run_test() {
  test_basic();
  test_emplace_sfinae();

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


// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

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
constexpr auto test_emplace_exists_imp(int) -> decltype(
    std::declval<Var>().template emplace<T>(std::declval<Args>()...), true) {
  return true;
}

template <class, class, class...>
constexpr auto test_emplace_exists_imp(long) -> bool {
  return false;
}

template <class... Args> constexpr bool emplace_exists() {
  return test_emplace_exists_imp<Args...>(0);
}

void test_emplace_sfinae() {
  {
    using V = std::variant<int, void *, const void *, TestTypes::NoCtors>;
    static_assert(emplace_exists<V, int>(), "");
    static_assert(emplace_exists<V, int, int>(), "");
    static_assert(!emplace_exists<V, int, decltype(nullptr)>(),
                  "cannot construct");
    static_assert(emplace_exists<V, void *, decltype(nullptr)>(), "");
    static_assert(!emplace_exists<V, void *, int>(), "cannot construct");
    static_assert(emplace_exists<V, void *, int *>(), "");
    static_assert(!emplace_exists<V, void *, const int *>(), "");
    static_assert(emplace_exists<V, const void *, const int *>(), "");
    static_assert(emplace_exists<V, const void *, int *>(), "");
    static_assert(!emplace_exists<V, TestTypes::NoCtors>(), "cannot construct");
  }
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  using V = std::variant<int, int &, const int &, int &&, long, long,
                         TestTypes::NoCtors>;
  static_assert(emplace_exists<V, int>(), "");
  static_assert(emplace_exists<V, int, int>(), "");
  static_assert(emplace_exists<V, int, long long>(), "");
  static_assert(!emplace_exists<V, int, int, int>(), "too many args");
  static_assert(emplace_exists<V, int &, int &>(), "");
  static_assert(!emplace_exists<V, int &>(), "cannot default construct ref");
  static_assert(!emplace_exists<V, int &, const int &>(), "cannot bind ref");
  static_assert(!emplace_exists<V, int &, int &&>(), "cannot bind ref");
  static_assert(emplace_exists<V, const int &, int &>(), "");
  static_assert(emplace_exists<V, const int &, const int &>(), "");
  static_assert(emplace_exists<V, const int &, int &&>(), "");
  static_assert(!emplace_exists<V, const int &, void *>(),
                "not constructible from void*");
  static_assert(emplace_exists<V, int &&, int>(), "");
  static_assert(!emplace_exists<V, int &&, int &>(), "cannot bind ref");
  static_assert(!emplace_exists<V, int &&, const int &>(), "cannot bind ref");
  static_assert(!emplace_exists<V, int &&, const int &&>(), "cannot bind ref");
  static_assert(!emplace_exists<V, long, long>(), "ambiguous");
  static_assert(!emplace_exists<V, TestTypes::NoCtors>(),
                "cannot construct void");
#endif
}

void test_basic() {
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
    using V =
        std::variant<int, long, const void *, TestTypes::NoCtors, std::string>;
    const int x = 100;
    V v(std::in_place_type<int>, -1);
    // default emplace a value
    auto& ref1 = v.emplace<long>();
    static_assert(std::is_same_v<long&, decltype(ref1)>, "");
    assert(std::get<1>(v) == 0);
    assert(&ref1 == &std::get<1>(v));
    auto& ref2 = v.emplace<const void *>(&x);
    static_assert(std::is_same_v<const void *&, decltype(ref2)>, "");
    assert(std::get<2>(v) == &x);
    assert(&ref2 == &std::get<2>(v));
    // emplace with multiple args
    auto& ref3 = v.emplace<std::string>(3u, 'a');
    static_assert(std::is_same_v<std::string&, decltype(ref3)>, "");
    assert(std::get<4>(v) == "aaa");
    assert(&ref3 == &std::get<4>(v));
  }
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  {
    using V = std::variant<int, long, const int &, int &&, TestTypes::NoCtors,
                           std::string>;
    const int x = 100;
    int y = 42;
    int z = 43;
    V v(std::in_place_index<0>, -1);
    // default emplace a value
    auto& ref1 = v.emplace<long>();
    static_assert(std::is_same_v<long&, decltype(ref1)>, "");
    assert(std::get<long>(v) == 0);
    assert(&ref1 == &std::get<long>(v));
    // emplace a reference
    auto& ref2 = v.emplace<const int &>(x);
    static_assert(std::is_same_v<const int&, decltype(ref2)>, "");
    assert(&std::get<const int &>(v) == &x);
    assert(&ref2 == &std::get<const int &>(v));
    // emplace an rvalue reference
    auto& ref3 = v.emplace<int &&>(std::move(y));
    static_assert(std::is_same_v<int &&, decltype(ref3)>, "");
    assert(&std::get<int &&>(v) == &y);
    assert(&ref3 == &std::get<int &&>(v));
    // re-emplace a new reference over the active member
    auto& ref4 = v.emplace<int &&>(std::move(z));
    static_assert(std::is_same_v<int &, decltype(ref4)>, "");
    assert(&std::get<int &&>(v) == &z);
    assert(&ref4 == &std::get<int &&>(v));
    // emplace with multiple args
    auto& ref5 = v.emplace<std::string>(3u, 'a');
    static_assert(std::is_same_v<std::string&, decltype(ref5)>, "");
    assert(std::get<std::string>(v) == "aaa");
    assert(&ref5 == &std::get<std::string>(v));
  }
#endif
}

int run_test() {
  test_basic();
  test_emplace_sfinae();

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


// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

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
  constexpr InitListArg(std::initializer_list<int> il, int v)
      : size(il.size()), value(v) {}
};

template <class Var, class T, class... Args>
constexpr auto test_emplace_exists_imp(int) -> decltype(
    std::declval<Var>().template emplace<T>(std::declval<Args>()...), true) {
  return true;
}

template <class, class, class...>
constexpr auto test_emplace_exists_imp(long) -> bool {
  return false;
}

template <class... Args> constexpr bool emplace_exists() {
  return test_emplace_exists_imp<Args...>(0);
}

void test_emplace_sfinae() {
  using V =
      std::variant<int, TestTypes::NoCtors, InitList, InitListArg, long, long>;
  using IL = std::initializer_list<int>;
  static_assert(emplace_exists<V, InitList, IL>(), "");
  static_assert(!emplace_exists<V, InitList, int>(), "args don't match");
  static_assert(!emplace_exists<V, InitList, IL, int>(), "too many args");
  static_assert(emplace_exists<V, InitListArg, IL, int>(), "");
  static_assert(!emplace_exists<V, InitListArg, int>(), "args don't match");
  static_assert(!emplace_exists<V, InitListArg, IL>(), "too few args");
  static_assert(!emplace_exists<V, InitListArg, IL, int, int>(),
                "too many args");
}

void test_basic() {
  using V = std::variant<int, InitList, InitListArg, TestTypes::NoCtors>;
  V v;
  auto& ref1 = v.emplace<InitList>({1, 2, 3});
  static_assert(std::is_same_v<InitList&,decltype(ref1)>, "");
  assert(std::get<InitList>(v).size == 3);
  assert(&ref1 == &std::get<InitList>(v));
  auto& ref2 = v.emplace<InitListArg>({1, 2, 3, 4}, 42);
  static_assert(std::is_same_v<InitListArg&,decltype(ref2)>, "");
  assert(std::get<InitListArg>(v).size == 4);
  assert(std::get<InitListArg>(v).value == 42);
  assert(&ref2 == &std::get<InitListArg>(v));
  auto& ref3 = v.emplace<InitList>({1});
  static_assert(std::is_same_v<InitList&,decltype(ref3)>, "");
  assert(std::get<InitList>(v).size == 1);
  assert(&ref3 == &std::get<InitList>(v));
}

int run_test() {
  test_basic();
  test_emplace_sfinae();

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


// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

// <variant>

// template <class ...Types> class variant;

// void swap(variant& rhs) noexcept(see below)

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>

#include "test_convertible.h"
#include "test_macros.h"
#include "variant_test_helpers.h"

namespace member_swap {
struct NotSwappable {};
void swap(NotSwappable &, NotSwappable &) = delete;

struct NotCopyable {
  NotCopyable() = default;
  NotCopyable(const NotCopyable &) = delete;
  NotCopyable &operator=(const NotCopyable &) = delete;
};

struct NotCopyableWithSwap {
  NotCopyableWithSwap() = default;
  NotCopyableWithSwap(const NotCopyableWithSwap &) = delete;
  NotCopyableWithSwap &operator=(const NotCopyableWithSwap &) = delete;
};
void swap(NotCopyableWithSwap &, NotCopyableWithSwap) {}

struct NotMoveAssignable {
  NotMoveAssignable() = default;
  NotMoveAssignable(NotMoveAssignable &&) = default;
  NotMoveAssignable &operator=(NotMoveAssignable &&) = delete;
};

struct NotMoveAssignableWithSwap {
  NotMoveAssignableWithSwap() = default;
  NotMoveAssignableWithSwap(NotMoveAssignableWithSwap &&) = default;
  NotMoveAssignableWithSwap &operator=(NotMoveAssignableWithSwap &&) = delete;
};
void swap(NotMoveAssignableWithSwap &, NotMoveAssignableWithSwap &) noexcept {}

template <bool Throws> void do_throw() {}

template <> void do_throw<true>() {
#ifndef TEST_HAS_NO_EXCEPTIONS
  throw 42;
#else
  std::abort();
#endif
}

template <bool NT_Copy, bool NT_Move, bool NT_CopyAssign, bool NT_MoveAssign,
          bool NT_Swap, bool EnableSwap = true>
struct NothrowTypeImp {
  static int move_called;
  static int move_assign_called;
  static int swap_called;
  static void reset() { move_called = move_assign_called = swap_called = 0; }
  NothrowTypeImp() = default;
  explicit NothrowTypeImp(int v) : value(v) {}
  NothrowTypeImp(const NothrowTypeImp &o) noexcept(NT_Copy) : value(o.value) {
    assert(false);
  } // never called by test
  NothrowTypeImp(NothrowTypeImp &&o) noexcept(NT_Move) : value(o.value) {
    ++move_called;
    do_throw<!NT_Move>();
    o.value = -1;
  }
  NothrowTypeImp &operator=(const NothrowTypeImp &) noexcept(NT_CopyAssign) {
    assert(false);
    return *this;
  } // never called by the tests
  NothrowTypeImp &operator=(NothrowTypeImp &&o) noexcept(NT_MoveAssign) {
    ++move_assign_called;
    do_throw<!NT_MoveAssign>();
    value = o.value;
    o.value = -1;
    return *this;
  }
  int value;
};
template <bool NT_Copy, bool NT_Move, bool NT_CopyAssign, bool NT_MoveAssign,
          bool NT_Swap, bool EnableSwap>
int NothrowTypeImp<NT_Copy, NT_Move, NT_CopyAssign, NT_MoveAssign, NT_Swap,
                   EnableSwap>::move_called = 0;
template <bool NT_Copy, bool NT_Move, bool NT_CopyAssign, bool NT_MoveAssign,
          bool NT_Swap, bool EnableSwap>
int NothrowTypeImp<NT_Copy, NT_Move, NT_CopyAssign, NT_MoveAssign, NT_Swap,
                   EnableSwap>::move_assign_called = 0;
template <bool NT_Copy, bool NT_Move, bool NT_CopyAssign, bool NT_MoveAssign,
          bool NT_Swap, bool EnableSwap>
int NothrowTypeImp<NT_Copy, NT_Move, NT_CopyAssign, NT_MoveAssign, NT_Swap,
                   EnableSwap>::swap_called = 0;

template <bool NT_Copy, bool NT_Move, bool NT_CopyAssign, bool NT_MoveAssign,
          bool NT_Swap>
void swap(NothrowTypeImp<NT_Copy, NT_Move, NT_CopyAssign, NT_MoveAssign,
                         NT_Swap, true> &lhs,
          NothrowTypeImp<NT_Copy, NT_Move, NT_CopyAssign, NT_MoveAssign,
                         NT_Swap, true> &rhs) noexcept(NT_Swap) {
  lhs.swap_called++;
  do_throw<!NT_Swap>();
  int tmp = lhs.value;
  lhs.value = rhs.value;
  rhs.value = tmp;
}

// throwing copy, nothrow move ctor/assign, no swap provided
using NothrowMoveable = NothrowTypeImp<false, true, false, true, false, false>;
// throwing copy and move assign, nothrow move ctor, no swap provided
using NothrowMoveCtor = NothrowTypeImp<false, true, false, false, false, false>;
// nothrow move ctor, throwing move assignment, swap provided
using NothrowMoveCtorWithThrowingSwap =
    NothrowTypeImp<false, true, false, false, false, true>;
// throwing move ctor, nothrow move assignment, no swap provided
using ThrowingMoveCtor =
    NothrowTypeImp<false, false, false, true, false, false>;
// throwing special members, nothrowing swap
using ThrowingTypeWithNothrowSwap =
    NothrowTypeImp<false, false, false, false, true, true>;
using NothrowTypeWithThrowingSwap =
    NothrowTypeImp<true, true, true, true, false, true>;
// throwing move assign with nothrow move and nothrow swap
using ThrowingMoveAssignNothrowMoveCtorWithSwap =
    NothrowTypeImp<false, true, false, false, true, true>;
// throwing move assign with nothrow move but no swap.
using ThrowingMoveAssignNothrowMoveCtor =
    NothrowTypeImp<false, true, false, false, false, false>;

struct NonThrowingNonNoexceptType {
  static int move_called;
  static void reset() { move_called = 0; }
  NonThrowingNonNoexceptType() = default;
  NonThrowingNonNoexceptType(int v) : value(v) {}
  NonThrowingNonNoexceptType(NonThrowingNonNoexceptType &&o) noexcept(false)
      : value(o.value) {
    ++move_called;
    o.value = -1;
  }
  NonThrowingNonNoexceptType &
  operator=(NonThrowingNonNoexceptType &&) noexcept(false) {
    assert(false); // never called by the tests.
    return *this;
  }
  int value;
};
int NonThrowingNonNoexceptType::move_called = 0;

struct ThrowsOnSecondMove {
  int value;
  int move_count;
  ThrowsOnSecondMove(int v) : value(v), move_count(0) {}
  ThrowsOnSecondMove(ThrowsOnSecondMove &&o) noexcept(false)
      : value(o.value), move_count(o.move_count + 1) {
    if (move_count == 2)
      do_throw<true>();
    o.value = -1;
  }
  ThrowsOnSecondMove &operator=(ThrowsOnSecondMove &&) {
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

void test_swap_same_alternative() {
  {
    using T = ThrowingTypeWithNothrowSwap;
    using V = std::variant<T, int>;
    T::reset();
    V v1(std::in_place_index<0>, 42);
    V v2(std::in_place_index<0>, 100);
    v1.swap(v2);
    assert(T::swap_called == 1);
    assert(std::get<0>(v1).value == 100);
    assert(std::get<0>(v2).value == 42);
    swap(v1, v2);
    assert(T::swap_called == 2);
    assert(std::get<0>(v1).value == 42);
    assert(std::get<0>(v2).value == 100);
  }
  {
    using T = NothrowMoveable;
    using V = std::variant<T, int>;
    T::reset();
    V v1(std::in_place_index<0>, 42);
    V v2(std::in_place_index<0>, 100);
    v1.swap(v2);
    assert(T::swap_called == 0);
    assert(T::move_called == 1);
    assert(T::move_assign_called == 2);
    assert(std::get<0>(v1).value == 100);
    assert(std::get<0>(v2).value == 42);
    T::reset();
    swap(v1, v2);
    assert(T::swap_called == 0);
    assert(T::move_called == 1);
    assert(T::move_assign_called == 2);
    assert(std::get<0>(v1).value == 42);
    assert(std::get<0>(v2).value == 100);
  }
#ifndef TEST_HAS_NO_EXCEPTIONS
  {
    using T = NothrowTypeWithThrowingSwap;
    using V = std::variant<T, int>;
    T::reset();
    V v1(std::in_place_index<0>, 42);
    V v2(std::in_place_index<0>, 100);
    try {
      v1.swap(v2);
      assert(false);
    } catch (int) {
    }
    assert(T::swap_called == 1);
    assert(T::move_called == 0);
    assert(T::move_assign_called == 0);
    assert(std::get<0>(v1).value == 42);
    assert(std::get<0>(v2).value == 100);
  }
  {
    using T = ThrowingMoveCtor;
    using V = std::variant<T, int>;
    T::reset();
    V v1(std::in_place_index<0>, 42);
    V v2(std::in_place_index<0>, 100);
    try {
      v1.swap(v2);
      assert(false);
    } catch (int) {
    }
    assert(T::move_called == 1); // call threw
    assert(T::move_assign_called == 0);
    assert(std::get<0>(v1).value ==
           42); // throw happened before v1 was moved from
    assert(std::get<0>(v2).value == 100);
  }
  {
    using T = ThrowingMoveAssignNothrowMoveCtor;
    using V = std::variant<T, int>;
    T::reset();
    V v1(std::in_place_index<0>, 42);
    V v2(std::in_place_index<0>, 100);
    try {
      v1.swap(v2);
      assert(false);
    } catch (int) {
    }
    assert(T::move_called == 1);
    assert(T::move_assign_called == 1);  // call threw and didn't complete
    assert(std::get<0>(v1).value == -1); // v1 was moved from
    assert(std::get<0>(v2).value == 100);
  }
#endif
}

void test_swap_different_alternatives() {
  {
    using T = NothrowMoveCtorWithThrowingSwap;
    using V = std::variant<T, int>;
    T::reset();
    V v1(std::in_place_index<0>, 42);
    V v2(std::in_place_index<1>, 100);
    v1.swap(v2);
    assert(T::swap_called == 0);
    // The libc++ implementation double copies the argument, and not
    // the variant swap is called on.
    LIBCPP_ASSERT(T::move_called == 1);
    assert(T::move_called <= 2);
    assert(T::move_assign_called == 0);
    assert(std::get<1>(v1) == 100);
    assert(std::get<0>(v2).value == 42);
    T::reset();
    swap(v1, v2);
    assert(T::swap_called == 0);
    LIBCPP_ASSERT(T::move_called == 2);
    assert(T::move_called <= 2);
    assert(T::move_assign_called == 0);
    assert(std::get<0>(v1).value == 42);
    assert(std::get<1>(v2) == 100);
  }
#ifndef TEST_HAS_NO_EXCEPTIONS
  {
    using T1 = ThrowingTypeWithNothrowSwap;
    using T2 = NonThrowingNonNoexceptType;
    using V = std::variant<T1, T2>;
    T1::reset();
    T2::reset();
    V v1(std::in_place_index<0>, 42);
    V v2(std::in_place_index<1>, 100);
    try {
      v1.swap(v2);
      assert(false);
    } catch (int) {
    }
    assert(T1::swap_called == 0);
    assert(T1::move_called == 1); // throws
    assert(T1::move_assign_called == 0);
    // TODO: libc++ shouldn't move from T2 here.
    LIBCPP_ASSERT(T2::move_called == 1);
    assert(T2::move_called <= 1);
    assert(std::get<0>(v1).value == 42);
    if (T2::move_called != 0)
      assert(v2.valueless_by_exception());
    else
      assert(std::get<1>(v2).value == 100);
  }
  {
    using T1 = NonThrowingNonNoexceptType;
    using T2 = ThrowingTypeWithNothrowSwap;
    using V = std::variant<T1, T2>;
    T1::reset();
    T2::reset();
    V v1(std::in_place_index<0>, 42);
    V v2(std::in_place_index<1>, 100);
    try {
      v1.swap(v2);
      assert(false);
    } catch (int) {
    }
    LIBCPP_ASSERT(T1::move_called == 0);
    assert(T1::move_called <= 1);
    assert(T2::swap_called == 0);
    assert(T2::move_called == 1); // throws
    assert(T2::move_assign_called == 0);
    if (T1::move_called != 0)
      assert(v1.valueless_by_exception());
    else
      assert(std::get<0>(v1).value == 42);
    assert(std::get<1>(v2).value == 100);
  }
// TODO: The tests below are just very libc++ specific
#ifdef _LIBCPP_VERSION
  {
    using T1 = ThrowsOnSecondMove;
    using T2 = NonThrowingNonNoexceptType;
    using V = std::variant<T1, T2>;
    T2::reset();
    V v1(std::in_place_index<0>, 42);
    V v2(std::in_place_index<1>, 100);
    v1.swap(v2);
    assert(T2::move_called == 2);
    assert(std::get<1>(v1).value == 100);
    assert(std::get<0>(v2).value == 42);
    assert(std::get<0>(v2).move_count == 1);
  }
  {
    using T1 = NonThrowingNonNoexceptType;
    using T2 = ThrowsOnSecondMove;
    using V = std::variant<T1, T2>;
    T1::reset();
    V v1(std::in_place_index<0>, 42);
    V v2(std::in_place_index<1>, 100);
    try {
      v1.swap(v2);
      assert(false);
    } catch (int) {
    }
    assert(T1::move_called == 1);
    assert(v1.valueless_by_exception());
    assert(std::get<0>(v2).value == 42);
  }
#endif
// testing libc++ extension. If either variant stores a nothrow move
// constructible type v1.swap(v2) provides the strong exception safety
// guarantee.
#ifdef _LIBCPP_VERSION
  {

    using T1 = ThrowingTypeWithNothrowSwap;
    using T2 = NothrowMoveable;
    using V = std::variant<T1, T2>;
    T1::reset();
    T2::reset();
    V v1(std::in_place_index<0>, 42);
    V v2(std::in_place_index<1>, 100);
    try {
      v1.swap(v2);
      assert(false);
    } catch (int) {
    }
    assert(T1::swap_called == 0);
    assert(T1::move_called == 1);
    assert(T1::move_assign_called == 0);
    assert(T2::swap_called == 0);
    assert(T2::move_called == 2);
    assert(T2::move_assign_called == 0);
    assert(std::get<0>(v1).value == 42);
    assert(std::get<1>(v2).value == 100);
    // swap again, but call v2's swap.
    T1::reset();
    T2::reset();
    try {
      v2.swap(v1);
      assert(false);
    } catch (int) {
    }
    assert(T1::swap_called == 0);
    assert(T1::move_called == 1);
    assert(T1::move_assign_called == 0);
    assert(T2::swap_called == 0);
    assert(T2::move_called == 2);
    assert(T2::move_assign_called == 0);
    assert(std::get<0>(v1).value == 42);
    assert(std::get<1>(v2).value == 100);
  }
#endif // _LIBCPP_VERSION
#endif
}

template <class Var>
constexpr auto has_swap_member_imp(int)
    -> decltype(std::declval<Var &>().swap(std::declval<Var &>()), true) {
  return true;
}

template <class Var> constexpr auto has_swap_member_imp(long) -> bool {
  return false;
}

template <class Var> constexpr bool has_swap_member() {
  return has_swap_member_imp<Var>(0);
}

void test_swap_sfinae() {
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

void test_swap_noexcept() {
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

int run_test() {
  test_swap_valueless_by_exception();
  test_swap_same_alternative();
  test_swap_different_alternatives();
  test_swap_sfinae();
  test_swap_noexcept();

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


// XFAIL: dylib-has-no-bad_variant_access && !libcpp-no-exceptions

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
#endif // _M_CEE
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
// Throwing bad_variant_access is supported starting in macosx10.13
// XFAIL: use_system_cxx_lib && target={{.+}}-apple-macosx10.{{9|10|11|12}} && !no-exceptions

// <variant>
// template <class Visitor, class... Variants>
// constexpr see below visit(Visitor&& vis, Variants&&... vars);

#include <cassert>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

#include "test_macros.h"
#include "variant_test_helpers.h"

namespace visit {
#if _HAS_CXX20 && !defined(__EDG__) && !defined(TEST_PERMISSIVE)
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
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  { // single argument - lvalue reference
    using V = std::variant<int &>;
    int x = 42;
    V v(x);
    const V &cv = v;
    std::visit(obj, v);
    assert(Fn::check_call<int &>(Val));
    std::visit(obj, cv);
    assert(Fn::check_call<int &>(Val));
    std::visit(obj, std::move(v));
    assert(Fn::check_call<int &>(Val));
    std::visit(obj, std::move(cv));
    assert(Fn::check_call<int &>(Val));
  }
  { // single argument - rvalue reference
    using V = std::variant<int &&>;
    int x = 42;
    V v(std::move(x));
    const V &cv = v;
    std::visit(obj, v);
    assert(Fn::check_call<int &>(Val));
    std::visit(obj, cv);
    assert(Fn::check_call<int &>(Val));
    std::visit(obj, std::move(v));
    assert(Fn::check_call<int &&>(Val));
    std::visit(obj, std::move(cv));
    assert(Fn::check_call<int &&>(Val));
  }
#endif
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
template <size_t Index>
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
  static_assert(!has_visit<BadVariant>(0));
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
#else // ^^ real test / vv workaround
int run_test() {
  return 0;
}
#endif // _HAS_CXX20 && !defined(__EDG__) && !defined(TEST_PERMISSIVE)

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
// Throwing bad_variant_access is supported starting in macosx10.13
// XFAIL: use_system_cxx_lib && target={{.+}}-apple-macosx10.{{9|10|11|12}} && !no-exceptions

// <variant>
// template <class R, class Visitor, class... Variants>
// constexpr R visit(Visitor&& vis, Variants&&... vars);

#include <cassert>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

#include "test_macros.h"
#include "variant_test_helpers.h"

namespace visit::return_type {
#if _HAS_CXX20 && !defined(__EDG__) && !defined(TEST_PERMISSIVE)
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
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
  { // single argument - lvalue reference
    using V = std::variant<int &>;
    int x = 42;
    V v(x);
    const V &cv = v;
    std::visit<ReturnType>(obj, v);
    assert(Fn::check_call<int &>(Val));
    std::visit<ReturnType>(obj, cv);
    assert(Fn::check_call<int &>(Val));
    std::visit<ReturnType>(obj, std::move(v));
    assert(Fn::check_call<int &>(Val));
    std::visit<ReturnType>(obj, std::move(cv));
    assert(Fn::check_call<int &>(Val));
  }
  { // single argument - rvalue reference
    using V = std::variant<int &&>;
    int x = 42;
    V v(std::move(x));
    const V &cv = v;
    std::visit<ReturnType>(obj, v);
    assert(Fn::check_call<int &>(Val));
    std::visit<ReturnType>(obj, cv);
    assert(Fn::check_call<int &>(Val));
    std::visit<ReturnType>(obj, std::move(v));
    assert(Fn::check_call<int &&>(Val));
    std::visit<ReturnType>(obj, std::move(cv));
    assert(Fn::check_call<int &&>(Val));
  }
#endif
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
  static_assert(!has_visit<BadVariant>(int()));
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
#else // ^^ real tests / vv workaround
int run_test() {
  return 0;
}
#endif // _HAS_CXX20 && !defined(__EDG__) && !defined(TEST_PERMISSIVE)
} // namespace visit::return_type
// -- END: test/std/utilities/variant/variant.visit/visit_return_type.pass.cpp

// LLVM SOURCES END
// clang-format on

#include <cassert>
#include <cstddef>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

// Include llvm-project/libcxx/test/support headers:
#include <test_macros.h>
#include <type_id.h>
#include <variant_test_helpers.h>

namespace msvc {
    namespace size {
        template <class T>
        using element =
            std::conditional<std::is_reference<T>::value, std::reference_wrapper<std::remove_reference_t<T>>, T>;

        template <std::size_t N>
        using index_t =
            std::conditional_t<(N < static_cast<std::size_t>(std::numeric_limits<signed char>::max())), signed char,
                std::conditional_t<(N < static_cast<std::size_t>(std::numeric_limits<short>::max())), short, int>>;

        template <class... Ts>
        struct fake_variant {
            std::aligned_union_t<0, typename element<Ts>::type...> data_;
            index_t<sizeof...(Ts)> index_;
        };

        template <class... Ts>
        inline constexpr bool check_size = sizeof(std::variant<Ts...>) == sizeof(fake_variant<Ts...>);

        template <int>
        struct empty {};

        struct not_empty {
            int i;
        };
        struct __declspec(empty_bases) many_bases : empty<0>, empty<1>, empty<2>, empty<3> {};

        STATIC_ASSERT(check_size<bool>);
        STATIC_ASSERT(check_size<char>);
        STATIC_ASSERT(check_size<unsigned char>);
        STATIC_ASSERT(check_size<int>);
        STATIC_ASSERT(check_size<unsigned int>);
        STATIC_ASSERT(check_size<long>);
        STATIC_ASSERT(check_size<long long>);
        STATIC_ASSERT(check_size<float>);
        STATIC_ASSERT(check_size<double>);
        STATIC_ASSERT(check_size<void*>);
        STATIC_ASSERT(check_size<empty<0>>);
        STATIC_ASSERT(check_size<not_empty>);
        STATIC_ASSERT(check_size<many_bases>);

        STATIC_ASSERT(check_size<bool, char, short, int, long, long long, float, double, long double, void*, empty<0>,
            empty<1>, not_empty, many_bases>);
    } // namespace size

    namespace big_variant {
#ifdef __EDG__
        constexpr std::size_t big = 20;
#else // C1XX and Clang
        constexpr std::size_t big = 64;
#endif // tune value of "big" to a bit less than the largest variant the front-end can handle
        constexpr std::size_t n = 16;

        template <std::size_t Size, std::size_t I, std::size_t N>
        void test_gets() {
            using V = std::_Meta_repeat_n_c<Size, std::string, std::variant>;
            V v{std::in_place_index<I>, "Hello, world!"};
            assert(std::get<I>(v) == "Hello, world!");
            if constexpr (N != 0) {
                test_gets<Size, I + (Size - I - 1) / N, N - 1>();
            }
        }

        template <std::size_t Size>
        void test_size() {
            if constexpr (Size <= big) {
                using V = std::_Meta_repeat_n_c<Size, std::string, std::variant>;
                // test 0, and n equally spaced indices including big - 1
                test_gets < Size, 0, n<Size ? n : Size>();
                constexpr std::size_t i = Size / 2;
                V v1{std::in_place_index<i>, "Hello, world!"};
                V v2 = v1;
                assert(std::get<i>(v2) == "Hello, world!");
                V v3 = std::move(v1);
                assert(std::get<i>(v3) == "Hello, world!");
                constexpr std::size_t j = Size <= 2 ? 0 : i + 1;
                v2.template emplace<j>("Goodbye, world!");
                v1 = v2;
                assert(std::get<j>(v1) == "Goodbye, world!");
                v2 = std::move(v3);
                assert(std::get<i>(v2) == "Hello, world!");
                v1.swap(v2);
                assert(std::get<j>(v2) == "Goodbye, world!");
                assert(std::get<i>(v1) == "Hello, world!");
                auto visitor = [](const std::string& s) { return s; };
                assert(std::visit(visitor, v1) == "Hello, world!");
            }
        }

        void run_test() {
            // test with all of the power-of-four sizes used for the switch blocks in visit and _Variant_raw_visit
            test_size<1>();
            test_size<3>();
            test_size<15>();
            test_size<63>();
            test_size<255>();
            test_size<big>();
        }
    } // namespace big_variant

    namespace derived_variant {
        void run_test() {
            // Extension: std::visit accepts types derived from a specialization of variant.
            {
                struct my_variant : std::variant<int, char, double> {
                    using std::variant<int, char, double>::variant;
                };

                my_variant v1{42};
                my_variant v2{3.14};
                auto visitor1 = [](auto&& x) { return static_cast<double>(x); };
                assert(std::visit(visitor1, v1) == 42.0);
                assert(std::visit(visitor1, v2) == 3.14);
                auto visitor2 = [](auto&& x, auto&& y) { return static_cast<double>(x + y); };
                assert(std::visit(visitor2, v1, v2) == 45.14);
            }
            {
                struct MakeEmptyT {
                    MakeEmptyT() = default;
                    MakeEmptyT(MakeEmptyT&&) {
                        throw 42;
                    }
                    MakeEmptyT& operator=(MakeEmptyT&&) {
                        throw 42;
                    }
                };
                struct my_variant : std::variant<int, MakeEmptyT> {
                    using std::variant<int, MakeEmptyT>::variant;
                };

                my_variant v{42};
                try {
                    v = my_variant{std::in_place_type<MakeEmptyT>};
                    abort();
                } catch (int) {
                    assert(v.valueless_by_exception());
                }

                auto very_useful_visitor = [](auto&&...) { abort(); };
                try {
                    std::visit(very_useful_visitor, v);
                    abort();
                } catch (std::bad_variant_access&) {
                }
                try {
                    std::visit(very_useful_visitor, my_variant{42}, v);
                    abort();
                } catch (std::bad_variant_access&) {
                }
                try {
                    std::visit(very_useful_visitor, v, my_variant{42});
                    abort();
                } catch (std::bad_variant_access&) {
                }
            }
        }
    } // namespace derived_variant

    template <class Fn>
    struct mobile_visitor {
        mobile_visitor() = default;

        mobile_visitor(const mobile_visitor&) {
            abort();
        }
        mobile_visitor(mobile_visitor&&) {
            abort();
        }
        mobile_visitor& operator=(const mobile_visitor&) {
            abort();
            return *this;
        }
        mobile_visitor& operator=(mobile_visitor&&) {
            abort();
            return *this;
        }

        template <class... Args>
        constexpr decltype(auto) operator()(Args&&... args) const {
            return Fn{}(std::forward<Args>(args)...);
        }
    };

    template <class Fn>
    struct immobile_visitor : mobile_visitor<Fn> {
        immobile_visitor()                                   = default;
        immobile_visitor(const immobile_visitor&)            = delete;
        immobile_visitor& operator=(const immobile_visitor&) = delete;
    };

    template <class T>
    struct convert_to {
        template <class U, std::enable_if_t<std::is_convertible_v<U, T>, int> = 0>
        constexpr T operator()(U&& u) const {
            return std::forward<U>(u);
        }
    };

    namespace visit {
        void test_immobile_function() {
            {
                // Validate that visit need not copy or move the visitor
                using V = std::variant<int, double>;
                immobile_visitor<convert_to<double>> visitor{};
                assert(std::visit(visitor, V{42}) == 42.0);
                assert(std::visit(std::as_const(visitor), V{3.14}) == 3.14);
                assert(std::visit(std::move(visitor), V{1729}) == 1729.0);
                assert(std::visit(std::move(std::as_const(visitor)), V{1.414}) == 1.414);
            }
            {
                // Validate that visit does not copy or move the visitor
                using V = std::variant<int, double>;
                mobile_visitor<convert_to<double>> visitor{};
                assert(std::visit(visitor, V{42}) == 42.0);
                assert(std::visit(std::as_const(visitor), V{3.14}) == 3.14);
                assert(std::visit(std::move(visitor), V{1729}) == 1729.0);
                assert(std::visit(std::move(std::as_const(visitor)), V{1.414}) == 1.414);
            }
        }

        void run_test() {
            test_immobile_function();
        }
    } // namespace visit

    namespace visit_R {
#if _HAS_CXX20
        //===----------------------------------------------------------------------===//
        //
        // Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
        // See https://llvm.org/LICENSE.txt for license information.
        // SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
        //
        //===----------------------------------------------------------------------===//

        enum CallType : unsigned int { CT_None, CT_NonConst = 1, CT_Const = 2, CT_LValue = 4, CT_RValue = 8 };

        constexpr CallType operator|(CallType LHS, CallType RHS) {
            return static_cast<CallType>(static_cast<unsigned int>(LHS) | static_cast<unsigned int>(RHS));
        }

        struct ForwardingCallObject {
            template <class... Args>
            bool operator()(Args&&...) & {
                set_call<Args&&...>(CT_NonConst | CT_LValue);
                return {};
            }

            template <class... Args>
            char operator()(Args&&...) const& {
                set_call<Args&&...>(CT_Const | CT_LValue);
                return {};
            }

            // Don't allow the call operator to be invoked as an rvalue.
            template <class... Args>
            short operator()(Args&&...) && {
                set_call<Args&&...>(CT_NonConst | CT_RValue);
                return {};
            }

            template <class... Args>
            int operator()(Args&&...) const&& {
                set_call<Args&&...>(CT_Const | CT_RValue);
                return {};
            }

            template <class... Args>
            static void set_call(CallType type) {
                assert(last_call_type == CT_None);
                assert(last_call_args == nullptr);
                last_call_type = type;
                last_call_args = std::addressof(makeArgumentID<Args...>());
            }

            template <class... Args>
            static bool check_call(CallType type) {
                bool result = last_call_type == type && last_call_args && *last_call_args == makeArgumentID<Args...>();
                last_call_type = CT_None;
                last_call_args = nullptr;
                return result;
            }

            static CallType last_call_type;
            static const TypeID* last_call_args;
        };

        CallType ForwardingCallObject::last_call_type      = CT_None;
        const TypeID* ForwardingCallObject::last_call_args = nullptr;

        template <class R>
        void test_call_operator_forwarding() {
            using Fn = ForwardingCallObject;
            Fn obj{};
            const Fn& cobj = obj;
            { // test call operator forwarding - no variant
                std::visit<R>(obj);
                assert(Fn::check_call<>(CT_NonConst | CT_LValue));
                std::visit<R>(cobj);
                assert(Fn::check_call<>(CT_Const | CT_LValue));
                std::visit<R>(std::move(obj));
                assert(Fn::check_call<>(CT_NonConst | CT_RValue));
                std::visit<R>(std::move(cobj));
                assert(Fn::check_call<>(CT_Const | CT_RValue));
            }
            { // test call operator forwarding - single variant, single arg
                using V = std::variant<int>;
                V v(42);
                std::visit<R>(obj, v);
                assert(Fn::check_call<int&>(CT_NonConst | CT_LValue));
                std::visit<R>(cobj, v);
                assert(Fn::check_call<int&>(CT_Const | CT_LValue));
                std::visit<R>(std::move(obj), v);
                assert(Fn::check_call<int&>(CT_NonConst | CT_RValue));
                std::visit<R>(std::move(cobj), v);
                assert(Fn::check_call<int&>(CT_Const | CT_RValue));
            }
            { // test call operator forwarding - single variant, multi arg
                using V = std::variant<int, long, double>;
                V v(42l);
                std::visit<R>(obj, v);
                assert(Fn::check_call<long&>(CT_NonConst | CT_LValue));
                std::visit<R>(cobj, v);
                assert(Fn::check_call<long&>(CT_Const | CT_LValue));
                std::visit<R>(std::move(obj), v);
                assert(Fn::check_call<long&>(CT_NonConst | CT_RValue));
                std::visit<R>(std::move(cobj), v);
                assert(Fn::check_call<long&>(CT_Const | CT_RValue));
            }
            { // test call operator forwarding - multi variant, multi arg
                using V  = std::variant<int, long, double>;
                using V2 = std::variant<int*, std::string>;
                V v(42l);
                V2 v2("hello");
                std::visit<R>(obj, v, v2);
                assert((Fn::check_call<long&, std::string&>(CT_NonConst | CT_LValue)));
                std::visit<R>(cobj, v, v2);
                assert((Fn::check_call<long&, std::string&>(CT_Const | CT_LValue)));
                std::visit<R>(std::move(obj), v, v2);
                assert((Fn::check_call<long&, std::string&>(CT_NonConst | CT_RValue)));
                std::visit<R>(std::move(cobj), v, v2);
                assert((Fn::check_call<long&, std::string&>(CT_Const | CT_RValue)));
            }
        }

        template <class R>
        void test_argument_forwarding() {
            using Fn = ForwardingCallObject;
            Fn obj{};
            const auto Val = CT_LValue | CT_NonConst;
            { // single argument - value type
                using V = std::variant<int>;
                V v(42);
                const V& cv = v;
                std::visit<R>(obj, v);
                assert(Fn::check_call<int&>(Val));
                std::visit<R>(obj, cv);
                assert(Fn::check_call<const int&>(Val));
                std::visit<R>(obj, std::move(v));
                assert(Fn::check_call<int&&>(Val));
                std::visit<R>(obj, std::move(cv));
                assert(Fn::check_call<const int&&>(Val));
            }
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
            { // single argument - lvalue reference
                using V = std::variant<int&>;
                int x   = 42;
                V v(x);
                const V& cv = v;
                std::visit<R>(obj, v);
                assert(Fn::check_call<int&>(Val));
                std::visit<R>(obj, cv);
                assert(Fn::check_call<int&>(Val));
                std::visit<R>(obj, std::move(v));
                assert(Fn::check_call<int&>(Val));
                std::visit<R>(obj, std::move(cv));
                assert(Fn::check_call<int&>(Val));
            }
            { // single argument - rvalue reference
                using V = std::variant<int&&>;
                int x   = 42;
                V v(std::move(x));
                const V& cv = v;
                std::visit<R>(obj, v);
                assert(Fn::check_call<int&>(Val));
                std::visit<R>(obj, cv);
                assert(Fn::check_call<int&>(Val));
                std::visit<R>(obj, std::move(v));
                assert(Fn::check_call<int&&>(Val));
                std::visit<R>(obj, std::move(cv));
                assert(Fn::check_call<int&&>(Val));
            }
            { // multi argument - multi variant
                using S               = const std::string&;
                using V               = std::variant<int, S, long&&>;
                const std::string str = "hello";
                long l                = 43;
                V v1(42);
                const V& cv1 = v1;
                V v2(str);
                const V& cv2 = v2;
                V v3(std::move(l));
                const V& cv3 = v3;
                std::visit<R>(obj, v1, v2, v3);
                assert((Fn::check_call<int&, S, long&>(Val)));
                std::visit<R>(obj, cv1, cv2, std::move(v3));
                assert((Fn::check_call<const int&, S, long&&>(Val)));
            }
#endif
        }

        struct ReturnFirst {
            template <class F, class... Args>
            constexpr F operator()(F f, Args&&...) const {
                return f;
            }
        };

        struct ReturnArity {
            template <class... Args>
            constexpr int operator()(Args&&...) const {
                return sizeof...(Args);
            }
        };

        struct simple_base {
            int x;

            constexpr explicit simple_base(int i) noexcept : x{i} {}
        };

        template <int>
        struct simple_derived : simple_base {
            using simple_base::simple_base;
        };

        constexpr bool test_constexpr() {
            constexpr ReturnFirst obj{};
            constexpr ReturnArity aobj{};
            {
                using V = std::variant<int>;
                constexpr V v(42);
                STATIC_ASSERT(std::visit<int>(obj, v) == 42);
            }
            {
                using V = std::variant<short, long, char>;
                constexpr V v(42l);
                STATIC_ASSERT(std::visit<long>(obj, v) == 42);
            }
            {
                using V1 = std::variant<int>;
                using V2 = std::variant<int, char*, long long>;
                using V3 = std::variant<bool, int, int>;
                constexpr V1 v1;
                constexpr V2 v2(nullptr);
                constexpr V3 v3;
                STATIC_ASSERT(std::visit<double>(aobj, v1, v2, v3) == 3.0);
            }
            {
                using V1 = std::variant<int>;
                using V2 = std::variant<int, char*, long long>;
                using V3 = std::variant<void*, int, int>;
                constexpr V1 v1;
                constexpr V2 v2(nullptr);
                constexpr V3 v3;
                STATIC_ASSERT(std::visit<long long>(aobj, v1, v2, v3) == 3LL);
            }
            {
                using V = std::variant<simple_derived<0>, simple_derived<1>, simple_derived<2>>;
                V v{simple_derived<1>{42}};
                auto&& b = std::visit<simple_base&>(std::identity{}, v);
                ASSERT_SAME_TYPE(decltype(b), simple_base&);
                assert(b.x == 42);
                auto&& cb = std::visit<const simple_base&>(std::identity{}, std::as_const(v));
                ASSERT_SAME_TYPE(decltype(cb), const simple_base&);
                assert(cb.x == 42);
                auto&& rb = std::visit<simple_base&&>(std::identity{}, std::move(v));
                ASSERT_SAME_TYPE(decltype(rb), simple_base &&);
                assert(rb.x == 42);
                auto&& crb = std::visit<const simple_base&&>(std::identity{}, std::move(std::as_const(v)));
                ASSERT_SAME_TYPE(decltype(crb), const simple_base&&);
                assert(crb.x == 42);
            }
            return true;
        }

        void test_exceptions() {
#ifndef TEST_HAS_NO_EXCEPTIONS
            ReturnArity obj{};
            auto test = [&](auto&&... args) {
                try {
                    std::visit<void>(obj, args...);
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
            {
                using V  = std::variant<int, MakeEmptyT>;
                using V2 = std::variant<long, std::string, void*>;
                V v;
                makeEmpty(v);
                V2 v2("hello");
                assert(test(v, v2));
            }
            {
                using V  = std::variant<int, MakeEmptyT>;
                using V2 = std::variant<long, std::string, void*>;
                V v;
                makeEmpty(v);
                V2 v2("hello");
                assert(test(v2, v));
            }
            {
                using V  = std::variant<int, MakeEmptyT>;
                using V2 = std::variant<long, std::string, void*, MakeEmptyT>;
                V v;
                makeEmpty(v);
                V2 v2;
                makeEmpty(v2);
                assert(test(v, v2));
            }
#endif
        }

        // See LLVM-31916
        void test_caller_accepts_nonconst() {
            struct A {};
            struct Visitor {
                void operator()(A&) {}
            };
            std::variant<A> v;
            std::visit<void>(Visitor{}, v);
        }

        struct mobile_data {
            int x;

            /* implicit */ mobile_data(int i) : x{i} {}
            mobile_data(const mobile_data&) {
                abort();
            }
            mobile_data(mobile_data&&) {
                abort();
            }
            mobile_data& operator=(const mobile_data&) {
                abort();
                return *this;
            }
            mobile_data& operator=(mobile_data&&) {
                abort();
                return *this;
            }
        };

        struct immobile_data : mobile_data {
            using mobile_data::mobile_data;
            immobile_data(const immobile_data&)            = delete;
            immobile_data& operator=(const immobile_data&) = delete;
        };

        void test_perfect_return() {
            {
                // Verify that a return object need not be copied/moved
                using R = immobile_data;
                assert(std::visit<R>(std::identity{}, std::variant<int, short>{13}).x == 13);
                assert(std::visit<R>(std::identity{}, std::variant<int, short>{short{42}}).x == 42);

                // Verify that conversions to an object that can't be copied/moved are correctly handled
                struct convertible_to_immobile_one {
                    operator immobile_data() const {
                        return immobile_data{1729};
                    }
                };

                struct convertible_to_immobile_other {
                    operator immobile_data() const {
                        return immobile_data{1138};
                    }
                };

                using VarTestConv = std::variant<convertible_to_immobile_one, convertible_to_immobile_other>;
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-10112408
                assert(std::visit<R>(std::identity{}, VarTestConv{convertible_to_immobile_one{}}).x == 1729);
                assert(std::visit<R>(std::identity{}, VarTestConv{convertible_to_immobile_other{}}).x == 1138);
#endif // TRANSITION, DevCom-10112408
                auto immobile_converter = [](auto src) -> immobile_data { return src; };
                assert(std::visit<R>(immobile_converter, VarTestConv{convertible_to_immobile_one{}}).x == 1729);
                assert(std::visit<R>(immobile_converter, VarTestConv{convertible_to_immobile_other{}}).x == 1138);
            }
            {
                // Verify that a returned object is not copied/moved/modified
                using R = mobile_data;
                assert(std::visit<R>(std::identity{}, std::variant<int, short>{13}).x == 13);
                assert(std::visit<R>(std::identity{}, std::variant<int, short>{short{42}}).x == 42);

                // Verify that a returned reference is not copied/moved/modified
                auto visitor1 = [x = R{1729}](auto) mutable -> R& { return x; };
                assert(std::visit<R&>(visitor1, std::variant<int, short>{13}).x == 1729);
                auto visitor2 = [x = R{1138}](auto) mutable -> R&& { return std::move(x); };
                assert(std::visit<R&&>(visitor2, std::variant<int, short>{13}).x == 1138);
            }
        }

        void test_immobile_function() {
            {
                // Validate that visit need not copy or move the visitor
                using V = std::variant<int, double>;
                immobile_visitor<std::identity> visitor{};
                assert(std::visit<double>(visitor, V{42}) == 42.0);
                assert(std::visit<double>(std::as_const(visitor), V{3.14}) == 3.14);
                assert(std::visit<double>(std::move(visitor), V{1729}) == 1729.0);
                assert(std::visit<double>(std::move(std::as_const(visitor)), V{1.414}) == 1.414);
            }
            {
                // Validate that visit does not copy or move the visitor
                using V = std::variant<int, double>;
                mobile_visitor<std::identity> visitor{};
                assert(std::visit<double>(visitor, V{42}) == 42.0);
                assert(std::visit<double>(std::as_const(visitor), V{3.14}) == 3.14);
                assert(std::visit<double>(std::move(visitor), V{1729}) == 1729.0);
                assert(std::visit<double>(std::move(std::as_const(visitor)), V{1.414}) == 1.414);
            }
        }

        void run_test() {
            test_call_operator_forwarding<void>();
            test_call_operator_forwarding<const void>();
            test_call_operator_forwarding<long>();
            test_argument_forwarding<void>();
            test_argument_forwarding<const void>();
            test_argument_forwarding<long long>();
            test_constexpr();
            static_assert(test_constexpr());
            test_exceptions();
            test_caller_accepts_nonconst();
            test_perfect_return();
            test_immobile_function();
        }
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
        void run_test() {}
#endif // _HAS_CXX20
    } // namespace visit_R

    namespace visit_pointer_to_member {
        struct base {
            int x;

            int f() const {
                return x;
            }
        };
        struct derived : base {
            int y;
        };

        void run_test() {
            using V = std::variant<base, derived>;
            assert(std::visit(&base::x, V{base{13}}) == 13);
            assert(std::visit(&base::x, V{derived{{42}, 29}}) == 42);

            assert(std::visit(&base::f, V{base{13}}) == 13);
            assert(std::visit(&base::f, V{derived{{42}, 29}}) == 42);
        }
    } // namespace visit_pointer_to_member

    template <class, class = void>
    inline constexpr bool has_type = false;
    template <class T>
    inline constexpr bool has_type<T, std::void_t<typename T::type>> = true;

    // Verify that `_Meta_at_<_Meta_list<>, size_t(-1)>` has no member named `type`, and that instantiating it doesn't
    // consume the entire compiler heap.
    STATIC_ASSERT(!has_type<std::_Meta_at_<std::_Meta_list<>, static_cast<std::size_t>(-1)>>);

    namespace vso468746 {
        // Defend against regression of VSO-468746
        // "std::function's converting constructor/assignment should be unusable for performing copy/move assignments"

        void run_test() {
            struct S {
                std::variant<std::function<S()>> member;
            };

            static_assert(
                sizeof(std::variant<std::function<S()>>) == sizeof(std::function<S()>) + alignof(std::function<S()>));
            static_assert(sizeof(S::member) == sizeof(std::variant<std::function<S()>>));
            static_assert(sizeof(S) >= sizeof(S::member));
        }
    } // namespace vso468746

    namespace vso492097 {
        // Defend against regression of VSO-492097
        // The compiler was not correctly emitting constant data for variants constructed from the address of static
        // duration variables with a constexpr operator& due to mishandling initialization of nested classes with
        // anonymous union members.
        template <class T>
        struct wrap {
            T val_;
            constexpr T* operator&() {
                return &val_;
            }
        };

        void run_test() {
            static wrap<int> intVar       = {42};
            static wrap<double> doubleVar = {3.14};

            using V = std::variant<int*, double*>;
            static constexpr V v1(&intVar);
            static constexpr V v2(&doubleVar);

            STATIC_ASSERT(v1.index() == 0);
            assert(*std::get<0>(v1) == 42);
            STATIC_ASSERT(v2.index() == 1);
            assert(*std::get<1>(v2) == 3.14);
        }
    } // namespace vso492097

    namespace vso508126 {
        void run_test() {
            struct S {};
            static_assert(!std::is_copy_constructible_v<volatile S>);
            static_assert(!std::is_copy_constructible_v<std::variant<volatile S>>);
        }
    } // namespace vso508126

    namespace DevCom1031281 {
        // Compilers may warn when initializing a variant from a "weird" argument, e.g., std::variant<short>{some_int}
        // is potentially narrowing. Compilers should not, however, emit such diagnostics from the metaprogramming that
        // determines which alternative a variant initialization would activate. We don't want to emit warnings when
        // determining implicit conversion sequences early in overload resolution.

        void Overload(int) {}
        void Overload(std::variant<unsigned short>) {}

        void run_test() {
            Overload(42);
        }
    } // namespace DevCom1031281

    namespace gh2770 {
        // Previous metaprogramming to validate the type requirements for std::visit required typelists too long for
        // Clang.
        struct S {
            template <class T0, class T1, class T2, class T3, class T4>
            int operator()(T0, T1, T2, T3, T4) const {
                return 1729;
            }
        };

        void run_test() {
            using V = std::variant<char, int, long, long long>;
            assert(std::visit(S{}, V{'a'}, V{'b'}, V{10}, V{20L}, V{30LL}) == 1729);
#if _HAS_CXX20
            assert(std::visit<int>(S{}, V{'a'}, V{'b'}, V{10}, V{20L}, V{30LL}) == 1729);
#endif // _HAS_CXX20
        }
    } // namespace gh2770

    namespace assign_cv {
        template <class T>
        struct TypeIdentityImpl {
            using type = T;
        };
        template <class T>
        using TypeIdentity = typename TypeIdentityImpl<T>::type;

        struct CvAssignable {
            CvAssignable()                               = default;
            CvAssignable(const CvAssignable&)            = default;
            CvAssignable(CvAssignable&&)                 = default;
            CvAssignable& operator=(const CvAssignable&) = default;
            CvAssignable& operator=(CvAssignable&&)      = default;

            template <class T = CvAssignable>
            CvAssignable(const volatile TypeIdentity<T>&) noexcept {}
            template <class T = CvAssignable>
            CvAssignable(const volatile TypeIdentity<T>&&) noexcept {}

            template <class T = CvAssignable>
            constexpr CvAssignable& operator=(const volatile TypeIdentity<T>&) noexcept {
                return *this;
            }
            template <class T = CvAssignable>
            constexpr CvAssignable& operator=(const volatile TypeIdentity<T>&&) noexcept {
                return *this;
            }

            template <class T = CvAssignable>
            constexpr const volatile CvAssignable& operator=(const volatile TypeIdentity<T>&) const volatile noexcept {
                return *this;
            }
            template <class T = CvAssignable>
            constexpr const volatile CvAssignable& operator=(const volatile TypeIdentity<T>&&) const volatile noexcept {
                return *this;
            }
        };

        void run_test() {
            using std::swap;
            {
                std::variant<const int> oc{};
                oc.emplace<0>(0);
                STATIC_ASSERT(!std::is_copy_assignable_v<decltype(oc)>);
                STATIC_ASSERT(!std::is_move_assignable_v<decltype(oc)>);
                STATIC_ASSERT(!std::is_swappable_v<decltype(oc)>);

                std::variant<volatile int> ov{};
                std::variant<volatile int> ov2{};
                ov.emplace<0>(0);
                swap(ov, ov);
                ov = ov2;
                ov = std::move(ov2);

                std::variant<const volatile int> ocv{};
                ocv.emplace<0>(0);
                STATIC_ASSERT(!std::is_copy_assignable_v<decltype(ocv)>);
                STATIC_ASSERT(!std::is_move_assignable_v<decltype(ocv)>);
                STATIC_ASSERT(!std::is_swappable_v<decltype(ocv)>);
            }
            {
                std::variant<const CvAssignable> oc{};
                std::variant<const CvAssignable> oc2{};
                oc.emplace<0>(CvAssignable{});
                swap(oc, oc);
                oc = oc2;
                oc = std::move(oc2);

                std::variant<volatile CvAssignable> ov{};
                std::variant<volatile CvAssignable> ov2{};
                ov.emplace<0>(CvAssignable{});
                swap(ov, ov);
                ov = ov2;
                ov = std::move(ov2);

                std::variant<const volatile CvAssignable> ocv{};
                std::variant<const volatile CvAssignable> ocv2{};
                ocv.emplace<0>(CvAssignable{});
                swap(ocv, ocv);
                ocv = ocv2;
                ocv = std::move(ocv2);
            }
        }
    } // namespace assign_cv
} // namespace msvc

int main() {
    bad_variant_access::run_test();

    get_if::index::run_test();
    get_if::type::run_test();
    get::index::run_test();
    get::type::run_test();
    holds_alternative::run_test();

    enabled_hash::run_test();
    hash::run_test();

    helpers::variant_alternative::run_test();
    helpers::variant_size::run_test();

    monostate::properties::run_test();
    monostate::relops::run_test();

    relops::run_test();

    npos::run_test();

    assign::conv::run_test();
    assign::copy::run_test();
    assign::move::run_test();
    assign::T::run_test();

    ctor::conv::run_test();
    ctor::copy::run_test();
    ctor::default_::run_test();
    ctor::in_place_index_args::run_test();
    ctor::in_place_type_args::run_test();
    ctor::in_place_index_init_list_args::run_test();
    ctor::in_place_type_init_list_args::run_test();
    ctor::move::run_test();
    ctor::T::run_test();

    dtor::run_test();

    emplace::index::run_test();
    emplace::index_init_list_args::run_test();
    emplace::type_args::run_test();
    emplace::type_init_list_args::run_test();

    status::index::run_test();
    status::valueless_by_exception::run_test();

    member_swap::run_test();

    visit::robust_against_adl::run_test();
    visit::run_test();
    visit::return_type::run_test();

    msvc::big_variant::run_test();
    msvc::derived_variant::run_test();
    msvc::visit::run_test();
    msvc::visit_R::run_test();
    msvc::visit_pointer_to_member::run_test();

    msvc::vso468746::run_test();
    msvc::vso508126::run_test();
    msvc::vso492097::run_test();
    msvc::DevCom1031281::run_test();
    msvc::gh2770::run_test();
    msvc::assign_cv::run_test();
}
#else // ^^^ not x86 or not /analyze / x86 /analyze vvv
int main() {}
#endif // !defined(_PREFAST_) || !defined(_M_IX86)
