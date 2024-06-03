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
//      ../../tools/scripts/transform_llvm.sh test/std/utilities/utility/utility.inplace test/std/utilities/optional
//    into a file.
// 3. Replicate the namespace structure from here into that file, use its content to replace everything between the
//    "LLVM SOURCES BEGIN"/"END" delimiters, and ensure that `main` properly calls each of the `run_test` functions.
// 4. Fix the specialization of std::hash by closing/reopening namespaces and qualifying types.
// 5. Restore the TRANSITION-commented workarounds.
// 6. Restore the _HAS_CXX20 and _HAS_CXX23 guards.
//
// Yes, this is an awkward hand process; notably the required headers can change without notice. We should investigate
// running the libc++ tests directly in all of our configurations so we needn't replicate this subset of files.

#define _LIBCXX_IN_DEVCRT
#include <msvc_stdlib_force_include.h> // Must precede any other libc++ headers
#include <stdlib.h>

// clang-format off
// LLVM SOURCES BEGIN
// -- BEGIN: test/std/utilities/utility/utility.inplace/inplace.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <utility>

// struct in_place_t {
//   explicit in_place_t() = default;
// };
// inline constexpr in_place_t in_place{};

// template <class T>
//   struct in_place_type_t {
//     explicit in_place_type_t() = default;
//   };
// template <class T>
//   inline constexpr in_place_type_t<T> in_place_type{};

// template <size_t I>
//   struct in_place_index_t {
//     explicit in_place_index_t() = default;
//   };
// template <size_t I>
//   inline constexpr in_place_index_t<I> in_place_index{};

#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>

namespace utility::in_place {
template <class Tp, class Up>
constexpr bool check_tag(Up) {
    return std::is_same<Tp, std::decay_t<Tp>>::value
        && std::is_same<Tp, Up>::value;
}

int run_test() {
    // test in_place_t
    {
        using T = std::in_place_t;
        static_assert(check_tag<T>(std::in_place));
    }
    // test in_place_type_t
    {
        using T1 = std::in_place_type_t<void>;
        using T2 = std::in_place_type_t<int>;
        using T3 = std::in_place_type_t<const int>;
        static_assert(!std::is_same<T1, T2>::value && !std::is_same<T1, T3>::value);
        static_assert(!std::is_same<T2, T3>::value);
        static_assert(check_tag<T1>(std::in_place_type<void>));
        static_assert(check_tag<T2>(std::in_place_type<int>));
        static_assert(check_tag<T3>(std::in_place_type<const int>));
    }
    // test in_place_index_t
    {
        using T1 = std::in_place_index_t<0>;
        using T2 = std::in_place_index_t<1>;
        using T3 = std::in_place_index_t<static_cast<std::size_t>(-1)>;
        static_assert(!std::is_same<T1, T2>::value && !std::is_same<T1, T3>::value);
        static_assert(!std::is_same<T2, T3>::value);
        static_assert(check_tag<T1>(std::in_place_index<0>));
        static_assert(check_tag<T2>(std::in_place_index<1>));
        static_assert(check_tag<T3>(std::in_place_index<static_cast<std::size_t>(-1)>));
    }

  return 0;
}
} // namespace utility::in_place
// -- END: test/std/utilities/utility/utility.inplace/inplace.pass.cpp

// -- BEGIN: test/std/utilities/optional/iterator_concept_conformance.compile.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// optional

#include <optional>

#include <iterator>

#if _HAS_CXX20
static_assert(!std::indirectly_readable<std::optional<int> >);
static_assert(!std::indirectly_writable<std::optional<int>, int>);
static_assert(!std::weakly_incrementable<std::optional<int> >);
static_assert(!std::indirectly_movable<std::optional<int>, std::optional<int>>);
static_assert(!std::indirectly_movable_storable<std::optional<int>, std::optional<int>>);
static_assert(!std::indirectly_copyable<std::optional<int>, std::optional<int>>);
static_assert(!std::indirectly_copyable_storable<std::optional<int>, std::optional<int>>);
#endif // ^^^ _HAS_CXX20 ^^^
// -- END: test/std/utilities/optional/iterator_concept_conformance.compile.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.bad_optional_access/default.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>

// class bad_optional_access is default constructible

#include <optional>
#include <type_traits>

#include "test_macros.h"

namespace bad_optional_access::default_ {
int run_test()
{
    using std::bad_optional_access;
    bad_optional_access ex;

  return 0;
}
} // namespace bad_optional_access::default_
// -- END: test/std/utilities/optional/optional.bad_optional_access/default.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.bad_optional_access/derive.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>

// class bad_optional_access : public exception

#include <optional>
#include <type_traits>

#include "test_macros.h"

namespace bad_optional_access::derive {
int run_test()
{
    using std::bad_optional_access;

    static_assert(std::is_base_of<std::exception, bad_optional_access>::value, "");
    static_assert(std::is_convertible<bad_optional_access*, std::exception*>::value, "");

  return 0;
}
} // namespace bad_optional_access::derive
// -- END: test/std/utilities/optional/optional.bad_optional_access/derive.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.comp_with_t/compare.three_way.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>

// [optional.comp.with.t], comparison with T

// template<class T, class U>
//     requires (!is-derived-from-optional<U>) && three_way_comparable_with<T, U>
//   constexpr compare_three_way_result_t<T, U>
//     operator<=>(const optional<T>&, const U&);

#include <cassert>
#include <compare>
#include <optional>

#include "test_comparisons.h"

namespace comp_with_t::three_way {
#if _HAS_CXX20
struct SomeInt {
  int value_;

  constexpr explicit SomeInt(int value = 0) : value_(value) {}

  auto operator<=>(const SomeInt&) const = default;
};

template <class T, class U>
concept HasSpaceship = requires(T t, U u) { t <=> u; };

// SFINAE tests.

static_assert(std::three_way_comparable_with<std::optional<int>, std::optional<int>>);
static_assert(HasSpaceship<std::optional<int>, std::optional<int>>);

static_assert(std::three_way_comparable_with<std::optional<SomeInt>, std::optional<SomeInt>>);
static_assert(HasSpaceship<std::optional<SomeInt>, std::optional<SomeInt>>);

static_assert(!HasSpaceship<std::optional<int>, std::optional<SomeInt>>);

// Runtime and static tests.

constexpr void test_custom_integral() {
  {
    SomeInt t{3};
    std::optional<SomeInt> op{3};
    assert((t <=> op) == std::strong_ordering::equal);
    assert(testOrder(t, op, std::strong_ordering::equal));
  }
  {
    SomeInt t{2};
    std::optional<SomeInt> op{3};
    assert((t <=> op) == std::strong_ordering::less);
    assert(testOrder(t, op, std::strong_ordering::less));
  }
  {
    SomeInt t{3};
    std::optional<SomeInt> op{2};
    assert((t <=> op) == std::strong_ordering::greater);
    assert(testOrder(t, op, std::strong_ordering::greater));
  }
}

constexpr void test_int() {
  {
    int t{3};
    std::optional<int> op{3};
    assert((t <=> op) == std::strong_ordering::equal);
    assert(testOrder(t, op, std::strong_ordering::equal));
  }
  {
    int t{2};
    std::optional<int> op{3};
    assert((t <=> op) == std::strong_ordering::less);
    assert(testOrder(t, op, std::strong_ordering::less));
  }
  {
    int t{3};
    std::optional<int> op{2};
    assert((t <=> op) == std::strong_ordering::greater);
    assert(testOrder(t, op, std::strong_ordering::greater));
  }
}

constexpr bool test() {
  test_custom_integral();
  test_int();

  return true;
}

int run_test() {
  assert(test());
  static_assert(test());
  return 0;
}
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
int run_test() {
    return 0;
}
#endif // ^^^ !_HAS_CXX20 ^^^
} // namespace comp_with_t::three_way
// -- END: test/std/utilities/optional/optional.comp_with_t/compare.three_way.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.comp_with_t/equal.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T, class U> constexpr bool operator==(const optional<T>& x, const U& v);
// template <class T, class U> constexpr bool operator==(const U& v, const optional<T>& x);

#include <optional>

#include "test_macros.h"

namespace comp_with_t::equal {
using std::optional;

struct X {
  int i_;

  constexpr X(int i) : i_(i) {}
};

constexpr bool operator==(const X& lhs, const X& rhs) {
  return lhs.i_ == rhs.i_;
}

int run_test() {
  {
    typedef X T;
    typedef optional<T> O;

    constexpr T val(2);
    constexpr O o1;      // disengaged
    constexpr O o2{1};   // engaged
    constexpr O o3{val}; // engaged

    static_assert(!(o1 == T(1)), "");
    static_assert((o2 == T(1)), "");
    static_assert(!(o3 == T(1)), "");
    static_assert((o3 == T(2)), "");
    static_assert((o3 == val), "");

    static_assert(!(T(1) == o1), "");
    static_assert((T(1) == o2), "");
    static_assert(!(T(1) == o3), "");
    static_assert((T(2) == o3), "");
    static_assert((val == o3), "");
  }
  {
    using O = optional<int>;
    constexpr O o1(42);
    static_assert(o1 == 42l, "");
    static_assert(!(101l == o1), "");
  }
  {
    using O = optional<const int>;
    constexpr O o1(42);
    static_assert(o1 == 42, "");
    static_assert(!(101 == o1), "");
  }

  return 0;
}
} // namespace comp_with_t::equal
// -- END: test/std/utilities/optional/optional.comp_with_t/equal.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.comp_with_t/greater.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T, class U> constexpr bool operator>(const optional<T>& x, const U& v);
// template <class T, class U> constexpr bool operator>(const U& v, const optional<T>& x);

#include <optional>

#include "test_macros.h"

namespace comp_with_t::greater_than {
using std::optional;

struct X {
  int i_;

  constexpr X(int i) : i_(i) {}
};

constexpr bool operator>(const X& lhs, const X& rhs) { return lhs.i_ > rhs.i_; }

int run_test() {
  {
    typedef X T;
    typedef optional<T> O;

    constexpr T val(2);
    constexpr O o1;      // disengaged
    constexpr O o2{1};   // engaged
    constexpr O o3{val}; // engaged

    static_assert(!(o1 > T(1)), "");
    static_assert(!(o2 > T(1)), ""); // equal
    static_assert((o3 > T(1)), "");
    static_assert(!(o2 > val), "");
    static_assert(!(o3 > val), ""); // equal
    static_assert(!(o3 > T(3)), "");

    static_assert((T(1) > o1), "");
    static_assert(!(T(1) > o2), ""); // equal
    static_assert(!(T(1) > o3), "");
    static_assert((val > o2), "");
    static_assert(!(val > o3), ""); // equal
    static_assert((T(3) > o3), "");
  }
  {
    using O = optional<int>;
    constexpr O o1(42);
    static_assert(o1 > 11l, "");
    static_assert(!(42l > o1), "");
  }
  {
    using O = optional<const int>;
    constexpr O o1(42);
    static_assert(o1 > 11, "");
    static_assert(!(42 > o1), "");
  }

  return 0;
}
} // namespace comp_with_t::greater_than
// -- END: test/std/utilities/optional/optional.comp_with_t/greater.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.comp_with_t/greater_equal.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T, class U> constexpr bool operator>=(const optional<T>& x, const U& v);
// template <class T, class U> constexpr bool operator>=(const U& v, const optional<T>& x);

#include <optional>

#include "test_macros.h"

namespace comp_with_t::greater_equal {
using std::optional;

struct X {
  int i_;

  constexpr X(int i) : i_(i) {}
};

constexpr bool operator>=(const X& lhs, const X& rhs) {
  return lhs.i_ >= rhs.i_;
}

int run_test() {
  {
    typedef X T;
    typedef optional<T> O;

    constexpr T val(2);
    constexpr O o1;      // disengaged
    constexpr O o2{1};   // engaged
    constexpr O o3{val}; // engaged

    static_assert(!(o1 >= T(1)), "");
    static_assert((o2 >= T(1)), ""); // equal
    static_assert((o3 >= T(1)), "");
    static_assert(!(o2 >= val), "");
    static_assert((o3 >= val), ""); // equal
    static_assert(!(o3 >= T(3)), "");

    static_assert((T(1) >= o1), "");
    static_assert((T(1) >= o2), ""); // equal
    static_assert(!(T(1) >= o3), "");
    static_assert((val >= o2), "");
    static_assert((val >= o3), ""); // equal
    static_assert((T(3) >= o3), "");
  }
  {
    using O = optional<int>;
    constexpr O o1(42);
    static_assert(o1 >= 42l, "");
    static_assert(!(11l >= o1), "");
  }
  {
    using O = optional<const int>;
    constexpr O o1(42);
    static_assert(o1 >= 42, "");
    static_assert(!(11 >= o1), "");
  }

  return 0;
}
} // namespace comp_with_t::greater_equal
// -- END: test/std/utilities/optional/optional.comp_with_t/greater_equal.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.comp_with_t/less_equal.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T, class U> constexpr bool operator<=(const optional<T>& x, const U& v);
// template <class T, class U> constexpr bool operator<=(const U& v, const optional<T>& x);

#include <optional>

#include "test_macros.h"

namespace comp_with_t::less_equal {
using std::optional;

struct X {
  int i_;

  constexpr X(int i) : i_(i) {}
};

constexpr bool operator<=(const X& lhs, const X& rhs) {
  return lhs.i_ <= rhs.i_;
}

int run_test() {
  {
    typedef X T;
    typedef optional<T> O;

    constexpr T val(2);
    constexpr O o1;      // disengaged
    constexpr O o2{1};   // engaged
    constexpr O o3{val}; // engaged

    static_assert((o1 <= T(1)), "");
    static_assert((o2 <= T(1)), ""); // equal
    static_assert(!(o3 <= T(1)), "");
    static_assert((o2 <= val), "");
    static_assert((o3 <= val), ""); // equal
    static_assert((o3 <= T(3)), "");

    static_assert(!(T(1) <= o1), "");
    static_assert((T(1) <= o2), ""); // equal
    static_assert((T(1) <= o3), "");
    static_assert(!(val <= o2), "");
    static_assert((val <= o3), ""); // equal
    static_assert(!(T(3) <= o3), "");
  }
  {
    using O = optional<int>;
    constexpr O o1(42);
    static_assert(o1 <= 42l, "");
    static_assert(!(101l <= o1), "");
  }
  {
    using O = optional<const int>;
    constexpr O o1(42);
    static_assert(o1 <= 42, "");
    static_assert(!(101 <= o1), "");
  }

  return 0;
}
} // namespace comp_with_t::less_equal
// -- END: test/std/utilities/optional/optional.comp_with_t/less_equal.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.comp_with_t/less_than.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T, class U> constexpr bool operator<(const optional<T>& x, const U& v);
// template <class T, class U> constexpr bool operator<(const U& v, const optional<T>& x);

#include <optional>

#include "test_macros.h"

namespace comp_with_t::less_than {
using std::optional;

struct X {
  int i_;

  constexpr X(int i) : i_(i) {}
};

constexpr bool operator<(const X& lhs, const X& rhs) { return lhs.i_ < rhs.i_; }

int run_test() {
  {
    typedef X T;
    typedef optional<T> O;

    constexpr T val(2);
    constexpr O o1;      // disengaged
    constexpr O o2{1};   // engaged
    constexpr O o3{val}; // engaged

    static_assert((o1 < T(1)), "");
    static_assert(!(o2 < T(1)), ""); // equal
    static_assert(!(o3 < T(1)), "");
    static_assert((o2 < val), "");
    static_assert(!(o3 < val), ""); // equal
    static_assert((o3 < T(3)), "");

    static_assert(!(T(1) < o1), "");
    static_assert(!(T(1) < o2), ""); // equal
    static_assert((T(1) < o3), "");
    static_assert(!(val < o2), "");
    static_assert(!(val < o3), ""); // equal
    static_assert(!(T(3) < o3), "");
  }
  {
    using O = optional<int>;
    constexpr O o1(42);
    static_assert(o1 < 101l, "");
    static_assert(!(42l < o1), "");
  }
  {
    using O = optional<const int>;
    constexpr O o1(42);
    static_assert(o1 < 101, "");
    static_assert(!(42 < o1), "");
  }

  return 0;
}
} // namespace comp_with_t::less_than
// -- END: test/std/utilities/optional/optional.comp_with_t/less_than.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.comp_with_t/not_equal.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T, class U> constexpr bool operator!=(const optional<T>& x, const U& v);
// template <class T, class U> constexpr bool operator!=(const U& v, const optional<T>& x);

#include <optional>

#include "test_macros.h"

namespace comp_with_t::not_equal {
using std::optional;

struct X {
  int i_;

  constexpr X(int i) : i_(i) {}
};

constexpr bool operator!=(const X& lhs, const X& rhs) {
  return lhs.i_ != rhs.i_;
}

int run_test() {
  {
    typedef X T;
    typedef optional<T> O;

    constexpr T val(2);
    constexpr O o1;      // disengaged
    constexpr O o2{1};   // engaged
    constexpr O o3{val}; // engaged

    static_assert((o1 != T(1)), "");
    static_assert(!(o2 != T(1)), "");
    static_assert((o3 != T(1)), "");
    static_assert(!(o3 != T(2)), "");
    static_assert(!(o3 != val), "");

    static_assert((T(1) != o1), "");
    static_assert(!(T(1) != o2), "");
    static_assert((T(1) != o3), "");
    static_assert(!(T(2) != o3), "");
    static_assert(!(val != o3), "");
  }
  {
    using O = optional<int>;
    constexpr O o1(42);
    static_assert(o1 != 101l, "");
    static_assert(!(42l != o1), "");
  }
  {
    using O = optional<const int>;
    constexpr O o1(42);
    static_assert(o1 != 101, "");
    static_assert(!(42 != o1), "");
  }

  return 0;
}
} // namespace comp_with_t::not_equal
// -- END: test/std/utilities/optional/optional.comp_with_t/not_equal.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.hash/enabled_hash.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>

// Test that <optional> provides all of the arithmetic, enum, and pointer
// hash specializations.

#include <functional>
#include <optional>

#include "poisoned_hash_helper.h"

#include "test_macros.h"

namespace hash::enabled_hash {
int run_test() {
  test_library_hash_specializations_available();
  {

  }

  return 0;
}
} // namespace hash::enabled_hash
// -- END: test/std/utilities/optional/optional.hash/enabled_hash.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.hash/hash.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T> struct hash<optional<T>>;

#include <optional>
#include <string>
#include <memory>
#include <cassert>

#include "poisoned_hash_helper.h"

#include "test_macros.h"

namespace hash {
struct A {};
struct B {};
} // namespace hash

namespace std {

template <>
struct hash<::hash::B> {
  std::size_t operator()(::hash::B const&) noexcept(false) { return 0; }
};

}

namespace hash {
int run_test()
{
    using std::optional;
    const std::size_t nullopt_hash =
        std::hash<optional<double>>{}(optional<double>{});


    {
        optional<B> opt;
        ASSERT_NOT_NOEXCEPT(std::hash<optional<B>>()(opt));
        ASSERT_NOT_NOEXCEPT(std::hash<optional<const B>>()(opt));
    }

    {
        typedef int T;
        optional<T> opt;
        assert(std::hash<optional<T>>{}(opt) == nullopt_hash);
        opt = 2;
        assert(std::hash<optional<T>>{}(opt) == std::hash<T>{}(*opt));
    }
    {
        typedef std::string T;
        optional<T> opt;
        assert(std::hash<optional<T>>{}(opt) == nullopt_hash);
        opt = std::string("123");
        assert(std::hash<optional<T>>{}(opt) == std::hash<T>{}(*opt));
    }
    {
        typedef std::unique_ptr<int> T;
        optional<T> opt;
        assert(std::hash<optional<T>>{}(opt) == nullopt_hash);
        opt = std::unique_ptr<int>(new int(3));
        assert(std::hash<optional<T>>{}(opt) == std::hash<T>{}(*opt));
    }
    {
#ifndef __EDG__ // TRANSITION, DevCom-10107834
      test_hash_enabled_for_type<std::optional<int> >();
      test_hash_enabled_for_type<std::optional<int*> >();
      test_hash_enabled_for_type<std::optional<const int> >();
      test_hash_enabled_for_type<std::optional<int* const> >();
#endif // ^^^ no workaround ^^^

      test_hash_disabled_for_type<std::optional<A>>();
      test_hash_disabled_for_type<std::optional<const A>>();

#ifndef __EDG__ // TRANSITION, DevCom-10107834
      test_hash_enabled_for_type<std::optional<B>>();
      test_hash_enabled_for_type<std::optional<const B>>();
#endif // ^^^ no workaround ^^^
    }

  return 0;
}
} // namespace hash
// -- END: test/std/utilities/optional/optional.hash/hash.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.monadic/and_then.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>

// template<class F> constexpr auto and_then(F&&) &;
// template<class F> constexpr auto and_then(F&&) &&;
// template<class F> constexpr auto and_then(F&&) const&;
// template<class F> constexpr auto and_then(F&&) const&&;

#include <cassert>
#include <optional>

#include "test_macros.h"

namespace monadic::and_then {
#if _HAS_CXX23
struct LVal {
  constexpr std::optional<int> operator()(int&) { return 1; }
  std::optional<int> operator()(const int&) = delete;
  std::optional<int> operator()(int&&) = delete;
  std::optional<int> operator()(const int&&) = delete;
};

struct CLVal {
  std::optional<int> operator()(int&) = delete;
  constexpr std::optional<int> operator()(const int&) { return 1; }
  std::optional<int> operator()(int&&) = delete;
  std::optional<int> operator()(const int&&) = delete;
};

struct RVal {
  std::optional<int> operator()(int&) = delete;
  std::optional<int> operator()(const int&) = delete;
  constexpr std::optional<int> operator()(int&&) { return 1; }
  std::optional<int> operator()(const int&&) = delete;
};

struct CRVal {
  std::optional<int> operator()(int&) = delete;
  std::optional<int> operator()(const int&) = delete;
  std::optional<int> operator()(int&&) = delete;
  constexpr std::optional<int> operator()(const int&&) { return 1; }
};

struct RefQual {
  constexpr std::optional<int> operator()(int) & { return 1; }
  std::optional<int> operator()(int) const& = delete;
  std::optional<int> operator()(int) && = delete;
  std::optional<int> operator()(int) const&& = delete;
};

struct CRefQual {
  std::optional<int> operator()(int) & = delete;
  constexpr std::optional<int> operator()(int) const& { return 1; }
  std::optional<int> operator()(int) && = delete;
  std::optional<int> operator()(int) const&& = delete;
};

struct RVRefQual {
  std::optional<int> operator()(int) & = delete;
  std::optional<int> operator()(int) const& = delete;
  constexpr std::optional<int> operator()(int) && { return 1; }
  std::optional<int> operator()(int) const&& = delete;
};

struct RVCRefQual {
  std::optional<int> operator()(int) & = delete;
  std::optional<int> operator()(int) const& = delete;
  std::optional<int> operator()(int) && = delete;
  constexpr std::optional<int> operator()(int) const&& { return 1; }
};

struct NOLVal {
  constexpr std::optional<int> operator()(int&) { return std::nullopt; }
  std::optional<int> operator()(const int&) = delete;
  std::optional<int> operator()(int&&) = delete;
  std::optional<int> operator()(const int&&) = delete;
};

struct NOCLVal {
  std::optional<int> operator()(int&) = delete;
  constexpr std::optional<int> operator()(const int&) { return std::nullopt; }
  std::optional<int> operator()(int&&) = delete;
  std::optional<int> operator()(const int&&) = delete;
};

struct NORVal {
  std::optional<int> operator()(int&) = delete;
  std::optional<int> operator()(const int&) = delete;
  constexpr std::optional<int> operator()(int&&) { return std::nullopt; }
  std::optional<int> operator()(const int&&) = delete;
};

struct NOCRVal {
  std::optional<int> operator()(int&) = delete;
  std::optional<int> operator()(const int&) = delete;
  std::optional<int> operator()(int&&) = delete;
  constexpr std::optional<int> operator()(const int&&) { return std::nullopt; }
};

struct NORefQual {
  constexpr std::optional<int> operator()(int) & { return std::nullopt; }
  std::optional<int> operator()(int) const& = delete;
  std::optional<int> operator()(int) && = delete;
  std::optional<int> operator()(int) const&& = delete;
};

struct NOCRefQual {
  std::optional<int> operator()(int) & = delete;
  constexpr std::optional<int> operator()(int) const& { return std::nullopt; }
  std::optional<int> operator()(int) && = delete;
  std::optional<int> operator()(int) const&& = delete;
};

struct NORVRefQual {
  std::optional<int> operator()(int) & = delete;
  std::optional<int> operator()(int) const& = delete;
  constexpr std::optional<int> operator()(int) && { return std::nullopt; }
  std::optional<int> operator()(int) const&& = delete;
};

struct NORVCRefQual {
  std::optional<int> operator()(int) & = delete;
  std::optional<int> operator()(int) const& = delete;
  std::optional<int> operator()(int) && = delete;
  constexpr std::optional<int> operator()(int) const&& { return std::nullopt; }
};

struct NoCopy {
  NoCopy() = default;
  NoCopy(const NoCopy&) { assert(false); }
  std::optional<int> operator()(const NoCopy&&) { return 1; }
};

struct NonConst {
  std::optional<int> non_const() { return 1; }
};

constexpr void test_val_types() {
  // Test & overload
  {
    // Without & qualifier on F's operator()
    {
      std::optional<int> i{0};
      assert(i.and_then(LVal{}) == 1);
      assert(i.and_then(NOLVal{}) == std::nullopt);
      ASSERT_SAME_TYPE(decltype(i.and_then(LVal{})), std::optional<int>);
    }

    //With & qualifier on F's operator()
    {
      std::optional<int> i{0};
      RefQual l{};
      assert(i.and_then(l) == 1);
      NORefQual nl{};
      assert(i.and_then(nl) == std::nullopt);
      ASSERT_SAME_TYPE(decltype(i.and_then(l)), std::optional<int>);
    }
  }

  // Test const& overload
  {
    // Without & qualifier on F's operator()
    {
      const std::optional<int> i{0};
      assert(i.and_then(CLVal{}) == 1);
      assert(i.and_then(NOCLVal{}) == std::nullopt);
      ASSERT_SAME_TYPE(decltype(i.and_then(CLVal{})), std::optional<int>);
    }

    //With & qualifier on F's operator()
    {
      const std::optional<int> i{0};
      const CRefQual l{};
      assert(i.and_then(l) == 1);
      const NOCRefQual nl{};
      assert(i.and_then(nl) == std::nullopt);
      ASSERT_SAME_TYPE(decltype(i.and_then(l)), std::optional<int>);
    }
  }

  // Test && overload
  {
    // Without & qualifier on F's operator()
    {
      std::optional<int> i{0};
      assert(std::move(i).and_then(RVal{}) == 1);
      assert(std::move(i).and_then(NORVal{}) == std::nullopt);
      ASSERT_SAME_TYPE(decltype(std::move(i).and_then(RVal{})), std::optional<int>);
    }

    //With & qualifier on F's operator()
    {
      std::optional<int> i{0};
      assert(i.and_then(RVRefQual{}) == 1);
      assert(i.and_then(NORVRefQual{}) == std::nullopt);
      ASSERT_SAME_TYPE(decltype(i.and_then(RVRefQual{})), std::optional<int>);
    }
  }

  // Test const&& overload
  {
    // Without & qualifier on F's operator()
    {
      const std::optional<int> i{0};
      assert(std::move(i).and_then(CRVal{}) == 1);
      assert(std::move(i).and_then(NOCRVal{}) == std::nullopt);
      ASSERT_SAME_TYPE(decltype(std::move(i).and_then(CRVal{})), std::optional<int>);
    }

    //With & qualifier on F's operator()
    {
      const std::optional<int> i{0};
      const RVCRefQual l{};
      assert(i.and_then(std::move(l)) == 1);
      const NORVCRefQual nl{};
      assert(i.and_then(std::move(nl)) == std::nullopt);
      ASSERT_SAME_TYPE(decltype(i.and_then(std::move(l))), std::optional<int>);
    }
  }
}

// check that the lambda body is not instantiated during overload resolution
constexpr void test_sfinae() {
  std::optional<NonConst> opt{};
  auto l = [](auto&& x) { return x.non_const(); };
  opt.and_then(l);
  std::move(opt).and_then(l);
}

constexpr bool test() {
  test_val_types();
  std::optional<int> opt{};
  const auto& copt = opt;

  const auto never_called = [](int) {
    assert(false);
    return std::optional<int>{};
  };

  opt.and_then(never_called);
  std::move(opt).and_then(never_called);
  copt.and_then(never_called);
  std::move(copt).and_then(never_called);

  std::optional<NoCopy> nc;
  const auto& cnc = nc;
  std::move(cnc).and_then(NoCopy{});
  std::move(nc).and_then(NoCopy{});

  return true;
}

int run_test() {
  test();
  static_assert(test());
  return 0;
}
#else // ^^^ _HAS_CXX23 / !_HAS_CXX23 vvv
int run_test() {
    return 0;
}
#endif // ^^^ !_HAS_CXX23 ^^^
} // namespace monadic::and_then
// -- END: test/std/utilities/optional/optional.monadic/and_then.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.monadic/or_else.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template<class F> constexpr optional or_else(F&&) &&;
// template<class F> constexpr optional or_else(F&&) const&;

#include "MoveOnly.h"

#include <cassert>
#include <optional>

namespace monadic::or_else {
#if _HAS_CXX23
struct NonMovable {
  NonMovable() = default;
  NonMovable(NonMovable&&) = delete;
};

template <class Opt, class F>
concept has_or_else = requires(Opt&& opt, F&& f) {
  {std::forward<Opt>(opt).or_else(std::forward<F>(f))};
};

template <class T>
std::optional<T> return_optional() {}

static_assert(has_or_else<std::optional<int>&, decltype(return_optional<int>)>);
static_assert(has_or_else<std::optional<int>&&, decltype(return_optional<int>)>);
static_assert(!has_or_else<std::optional<MoveOnly>&, decltype(return_optional<MoveOnly>)>);
static_assert(has_or_else<std::optional<MoveOnly>&&, decltype(return_optional<MoveOnly>)>);
static_assert(!has_or_else<std::optional<NonMovable>&, decltype(return_optional<NonMovable>)>);
static_assert(!has_or_else<std::optional<NonMovable>&&, decltype(return_optional<NonMovable>)>);

std::optional<int> take_int(int) { return 0; }
void take_int_return_void(int) {}

static_assert(!has_or_else<std::optional<int>, decltype(take_int)>);
static_assert(!has_or_else<std::optional<int>, decltype(take_int_return_void)>);
static_assert(!has_or_else<std::optional<int>, int>);

constexpr bool test() {
  {
    std::optional<int> opt;
    assert(opt.or_else([] { return std::optional<int>{0}; }) == 0);
    opt = 1;
    opt.or_else([] {
      assert(false);
      return std::optional<int>{};
    });
  }
  {
    std::optional<MoveOnly> opt;
    opt = std::move(opt).or_else([] { return std::optional<MoveOnly>{MoveOnly{}}; });
    std::move(opt).or_else([] {
      assert(false);
      return std::optional<MoveOnly>{};
    });
  }

  return true;
}

int run_test() {
  test();
  static_assert(test());
  return 0;
}
#else // ^^^ _HAS_CXX23 / !_HAS_CXX23 vvv
int run_test() {
    return 0;
}
#endif // ^^^ !_HAS_CXX23 ^^^
} // namespace monadic::or_else
// -- END: test/std/utilities/optional/optional.monadic/or_else.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.monadic/transform.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>

// template<class F> constexpr auto transform(F&&) &;
// template<class F> constexpr auto transform(F&&) &&;
// template<class F> constexpr auto transform(F&&) const&;
// template<class F> constexpr auto transform(F&&) const&&;

#include "test_macros.h"
#include <cassert>
#include <optional>
#include <type_traits>

namespace monadic::transform {
#if _HAS_CXX23
struct LVal {
  constexpr int operator()(int&) { return 1; }
  int operator()(const int&) = delete;
  int operator()(int&&) = delete;
  int operator()(const int&&) = delete;
};

struct CLVal {
  int operator()(int&) = delete;
  constexpr int operator()(const int&) { return 1; }
  int operator()(int&&) = delete;
  int operator()(const int&&) = delete;
};

struct RVal {
  int operator()(int&) = delete;
  int operator()(const int&) = delete;
  constexpr int operator()(int&&) { return 1; }
  int operator()(const int&&) = delete;
};

struct CRVal {
  int operator()(int&) = delete;
  int operator()(const int&) = delete;
  int operator()(int&&) = delete;
  constexpr int operator()(const int&&) { return 1; }
};

struct RefQual {
  constexpr int operator()(int) & { return 1; }
  int operator()(int) const& = delete;
  int operator()(int) && = delete;
  int operator()(int) const&& = delete;
};

struct CRefQual {
  int operator()(int) & = delete;
  constexpr int operator()(int) const& { return 1; }
  int operator()(int) && = delete;
  int operator()(int) const&& = delete;
};

struct RVRefQual {
  int operator()(int) & = delete;
  int operator()(int) const& = delete;
  constexpr int operator()(int) && { return 1; }
  int operator()(int) const&& = delete;
};

struct RVCRefQual {
  int operator()(int) & = delete;
  int operator()(int) const& = delete;
  int operator()(int) && = delete;
  constexpr int operator()(int) const&& { return 1; }
};

struct NoCopy {
  NoCopy() = default;
  NoCopy(const NoCopy&) { assert(false); }
  int operator()(const NoCopy&&) { return 1; }
};

struct NoMove {
  NoMove() = default;
  NoMove(NoMove&&) = delete;
  NoMove operator()(const NoCopy&&) { return NoMove{}; }
};

constexpr void test_val_types() {
  // Test & overload
  {
    // Without & qualifier on F's operator()
    {
      std::optional<int> i{0};
      assert(i.transform(LVal{}) == 1);
      ASSERT_SAME_TYPE(decltype(i.transform(LVal{})), std::optional<int>);
    }

    //With & qualifier on F's operator()
    {
      std::optional<int> i{0};
      RefQual l{};
      assert(i.transform(l) == 1);
      ASSERT_SAME_TYPE(decltype(i.transform(l)), std::optional<int>);
    }
  }

  // Test const& overload
  {
    // Without & qualifier on F's operator()
    {
      const std::optional<int> i{0};
      assert(i.transform(CLVal{}) == 1);
      ASSERT_SAME_TYPE(decltype(i.transform(CLVal{})), std::optional<int>);
    }

    //With & qualifier on F's operator()
    {
      const std::optional<int> i{0};
      const CRefQual l{};
      assert(i.transform(l) == 1);
      ASSERT_SAME_TYPE(decltype(i.transform(l)), std::optional<int>);
    }
  }

  // Test && overload
  {
    // Without & qualifier on F's operator()
    {
      std::optional<int> i{0};
      assert(std::move(i).transform(RVal{}) == 1);
      ASSERT_SAME_TYPE(decltype(std::move(i).transform(RVal{})), std::optional<int>);
    }

    //With & qualifier on F's operator()
    {
      std::optional<int> i{0};
      assert(i.transform(RVRefQual{}) == 1);
      ASSERT_SAME_TYPE(decltype(i.transform(RVRefQual{})), std::optional<int>);
    }
  }

  // Test const&& overload
  {
    // Without & qualifier on F's operator()
    {
      const std::optional<int> i{0};
      assert(std::move(i).transform(CRVal{}) == 1);
      ASSERT_SAME_TYPE(decltype(std::move(i).transform(CRVal{})), std::optional<int>);
    }

    //With & qualifier on F's operator()
    {
      const std::optional<int> i{0};
      const RVCRefQual l{};
      assert(i.transform(std::move(l)) == 1);
      ASSERT_SAME_TYPE(decltype(i.transform(std::move(l))), std::optional<int>);
    }
  }
}

struct NonConst {
  int non_const() { return 1; }
};

// check that the lambda body is not instantiated during overload resolution
constexpr void test_sfinae() {
  std::optional<NonConst> opt{};
  auto l = [](auto&& x) { return x.non_const(); };
  opt.transform(l);
  std::move(opt).transform(l);
}

constexpr bool test() {
  test_sfinae();
  test_val_types();
  std::optional<int> opt;
  const auto& copt = opt;

  const auto never_called = [](int) {
    assert(false);
    return 0;
  };

  opt.transform(never_called);
  std::move(opt).transform(never_called);
  copt.transform(never_called);
  std::move(copt).transform(never_called);

  std::optional<NoCopy> nc;
  const auto& cnc = nc;
  std::move(nc).transform(NoCopy{});
  std::move(cnc).transform(NoCopy{});

  std::move(nc).transform(NoMove{});
  std::move(cnc).transform(NoMove{});

  return true;
}

int run_test() {
  test();
  static_assert(test());
  return 0;
}
#else // ^^^ _HAS_CXX23 / !_HAS_CXX23 vvv
int run_test() {
    return 0;
}
#endif // ^^^ !_HAS_CXX23 ^^^
} // namespace monadic::transform
// -- END: test/std/utilities/optional/optional.monadic/transform.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.nullops/compare.three_way.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>

// [optional.nullops], comparison with nullopt

// template<class T>
//   constexpr strong_ordering operator<=>(const optional<T>&, nullopt_t) noexcept;

#include <cassert>
#include <compare>
#include <optional>

#include "test_comparisons.h"

namespace nullops::three_way {
#if _HAS_CXX20
constexpr bool test() {
  {
    std::optional<int> op;
    assert((std::nullopt <=> op) == std::strong_ordering::equal);
    assert(testOrder(std::nullopt, op, std::strong_ordering::equal));
    assert((op <=> std::nullopt) == std::strong_ordering::equal);
    assert(testOrder(op, std::nullopt, std::strong_ordering::equal));
  }
  {
    std::optional<int> op{1};
    assert((std::nullopt <=> op) == std::strong_ordering::less);
    assert(testOrder(std::nullopt, op, std::strong_ordering::less));
  }
  {
    std::optional<int> op{1};
    assert((op <=> std::nullopt) == std::strong_ordering::greater);
    assert(testOrder(op, std::nullopt, std::strong_ordering::greater));
  }

  return true;
}

int run_test() {
  assert(test());
  static_assert(test());
  return 0;
}
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
int run_test() {
    return 0;
}
#endif // ^^^ !_HAS_CXX20 ^^^
} // namespace nullops::three_way
// -- END: test/std/utilities/optional/optional.nullops/compare.three_way.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.nullops/equal.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T> constexpr bool operator==(const optional<T>& x, nullopt_t) noexcept;
// template <class T> constexpr bool operator==(nullopt_t, const optional<T>& x) noexcept;

#include <optional>

#include "test_macros.h"

namespace nullops::equal {
int run_test()
{
    using std::optional;
    using std::nullopt_t;
    using std::nullopt;

    {
    typedef int T;
    typedef optional<T> O;

    constexpr O o1;     // disengaged
    constexpr O o2{1};  // engaged

    static_assert (  (nullopt == o1), "" );
    static_assert ( !(nullopt == o2), "" );
    static_assert (  (o1 == nullopt), "" );
    static_assert ( !(o2 == nullopt), "" );

    static_assert (noexcept(nullopt == o1), "");
    static_assert (noexcept(o1 == nullopt), "");
    }

  return 0;
}
} // namespace nullops::equal
// -- END: test/std/utilities/optional/optional.nullops/equal.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.nullops/greater.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T> constexpr bool operator>(const optional<T>& x, nullopt_t) noexcept;
// template <class T> constexpr bool operator>(nullopt_t, const optional<T>& x) noexcept;

#include <optional>

#include "test_macros.h"

namespace nullops::greater_than {
int run_test()
{
    using std::optional;
    using std::nullopt_t;
    using std::nullopt;

    {
    typedef int T;
    typedef optional<T> O;

    constexpr O o1;     // disengaged
    constexpr O o2{1};  // engaged

    static_assert ( !(nullopt > o1), "" );
    static_assert ( !(nullopt > o2), "" );
    static_assert ( !(o1 > nullopt), "" );
    static_assert (  (o2 > nullopt), "" );

    static_assert (noexcept(nullopt > o1), "");
    static_assert (noexcept(o1 > nullopt), "");
    }

  return 0;
}
} // namespace nullops::greater_than
// -- END: test/std/utilities/optional/optional.nullops/greater.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.nullops/greater_equal.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T> constexpr bool operator>=(const optional<T>& x, nullopt_t) noexcept;
// template <class T> constexpr bool operator>=(nullopt_t, const optional<T>& x) noexcept;

#include <optional>

#include "test_macros.h"

namespace nullops::greater_equal {
int run_test()
{
    using std::optional;
    using std::nullopt_t;
    using std::nullopt;

    {
    typedef int T;
    typedef optional<T> O;

    constexpr O o1;     // disengaged
    constexpr O o2{1};  // engaged

    static_assert (  (nullopt >= o1), "" );
    static_assert ( !(nullopt >= o2), "" );
    static_assert (  (o1 >= nullopt), "" );
    static_assert (  (o2 >= nullopt), "" );

    static_assert (noexcept(nullopt >= o1), "");
    static_assert (noexcept(o1 >= nullopt), "");
    }

  return 0;
}
} // namespace nullops::greater_equal
// -- END: test/std/utilities/optional/optional.nullops/greater_equal.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.nullops/less_equal.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>

// template <class T> constexpr bool operator<=(const optional<T>& x, nullopt_t) noexcept;
// template <class T> constexpr bool operator<=(nullopt_t, const optional<T>& x) noexcept;

#include <optional>

#include "test_macros.h"

namespace nullops::less_equal {
int run_test()
{
    using std::optional;
    using std::nullopt_t;
    using std::nullopt;

    {
    typedef int T;
    typedef optional<T> O;

    constexpr O o1;     // disengaged
    constexpr O o2{1};  // engaged

    static_assert (  (nullopt <= o1), "" );
    static_assert (  (nullopt <= o2), "" );
    static_assert (  (o1 <= nullopt), "" );
    static_assert ( !(o2 <= nullopt), "" );

    static_assert (noexcept(nullopt <= o1), "");
    static_assert (noexcept(o1 <= nullopt), "");
    }

  return 0;
}
} // namespace nullops::less_equal
// -- END: test/std/utilities/optional/optional.nullops/less_equal.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.nullops/less_than.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T> constexpr bool operator<(const optional<T>& x, nullopt_t) noexcept;
// template <class T> constexpr bool operator<(nullopt_t, const optional<T>& x) noexcept;

#include <optional>

#include "test_macros.h"

namespace nullops::less_than {
int run_test()
{
    using std::optional;
    using std::nullopt_t;
    using std::nullopt;

    {
    typedef int T;
    typedef optional<T> O;

    constexpr O o1;     // disengaged
    constexpr O o2{1};  // engaged

    static_assert ( !(nullopt < o1), "" );
    static_assert (  (nullopt < o2), "" );
    static_assert ( !(o1 < nullopt), "" );
    static_assert ( !(o2 < nullopt), "" );

    static_assert (noexcept(nullopt < o1), "");
    static_assert (noexcept(o1 < nullopt), "");
    }

  return 0;
}
} // namespace nullops::less_than
// -- END: test/std/utilities/optional/optional.nullops/less_than.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.nullops/not_equal.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T> constexpr bool operator!=(const optional<T>& x, nullopt_t) noexcept;
// template <class T> constexpr bool operator!=(nullopt_t, const optional<T>& x) noexcept;

#include <optional>

#include "test_macros.h"

namespace nullops::not_equal {
int run_test()
{
    using std::optional;
    using std::nullopt_t;
    using std::nullopt;

    {
    typedef int T;
    typedef optional<T> O;

    constexpr O o1;     // disengaged
    constexpr O o2{1};  // engaged

    static_assert ( !(nullopt != o1), "" );
    static_assert (  (nullopt != o2), "" );
    static_assert ( !(o1 != nullopt), "" );
    static_assert (  (o2 != nullopt), "" );

    static_assert (noexcept(nullopt != o1), "");
    static_assert (noexcept(o1 != nullopt), "");
    }

  return 0;
}
} // namespace nullops::not_equal
// -- END: test/std/utilities/optional/optional.nullops/not_equal.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.nullopt/nullopt_t.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// struct nullopt_t{see below};
// inline constexpr nullopt_t nullopt(unspecified);

// [optional.nullopt]/2:
//   Type nullopt_t shall not have a default constructor or an initializer-list
//   constructor, and shall not be an aggregate.

#include <optional>
#include <type_traits>

#include "test_macros.h"

namespace nullopt {
using std::nullopt_t;
using std::nullopt;

constexpr bool test()
{
    nullopt_t foo{nullopt};
    (void)foo;
    return true;
}

int run_test()
{
    static_assert(std::is_empty_v<nullopt_t>);
    static_assert(!std::is_default_constructible_v<nullopt_t>);

    static_assert(std::is_same_v<const nullopt_t, decltype(nullopt)>);
    static_assert(test());

  return 0;
}
} // namespace nullopt
// -- END: test/std/utilities/optional/optional.nullopt/nullopt_t.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.assign/assign_value.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class U> optional<T>& operator=(U&& v);

#include <optional>
#include <type_traits>
#include <cassert>
#include <memory>

#include "test_macros.h"
#include "archetypes.h"

namespace assign::value {
using std::optional;

struct ThrowAssign {
  static int dtor_called;
  ThrowAssign() = default;
  ThrowAssign(int) { TEST_THROW(42); }
  ThrowAssign& operator=(int) {
      TEST_THROW(42);
  }
  ~ThrowAssign() { ++dtor_called; }
};
int ThrowAssign::dtor_called = 0;

template <class T, class Arg = T, bool Expect = true>
void assert_assignable() {
    static_assert(std::is_assignable<optional<T>&, Arg>::value == Expect, "");
    static_assert(!std::is_assignable<const optional<T>&, Arg>::value, "");
}

struct MismatchType {
  explicit MismatchType(int) {}
  explicit MismatchType(char*) {}
  explicit MismatchType(int*) = delete;
  MismatchType& operator=(int) { return *this; }
  MismatchType& operator=(int*) { return *this; }
  MismatchType& operator=(char*) = delete;
};

struct FromOptionalType {
  using Opt = std::optional<FromOptionalType>;
  FromOptionalType() = default;
  FromOptionalType(FromOptionalType const&) = delete;
  template <class Dummy = void>
  constexpr FromOptionalType(Opt&) { Dummy::BARK; }
  template <class Dummy = void>
  constexpr FromOptionalType& operator=(Opt&) { Dummy::BARK; return *this; }
};

void test_sfinae() {
    using I = TestTypes::TestType;
    using E = ExplicitTestTypes::TestType;
    assert_assignable<int>();
    assert_assignable<int, int&>();
    assert_assignable<int, int const&>();
    // Implicit test type
    assert_assignable<I, I const&>();
    assert_assignable<I, I&&>();
    assert_assignable<I, int>();
    assert_assignable<I, void*, false>();
    // Explicit test type
    assert_assignable<E, E const&>();
    assert_assignable<E, E &&>();
    assert_assignable<E, int>();
    assert_assignable<E, void*, false>();
    // Mismatch type
    assert_assignable<MismatchType, int>();
    assert_assignable<MismatchType, int*, false>();
    assert_assignable<MismatchType, char*, false>();
    // Type constructible from optional
    assert_assignable<FromOptionalType, std::optional<FromOptionalType>&, false>();
}

void test_with_test_type()
{
    using T = TestTypes::TestType;
    T::reset();
    { // to empty
        optional<T> opt;
        opt = 3;
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::value_constructed == 1);
        assert(T::assigned == 0);
        assert(T::destroyed == 0);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(3));
    }
    { // to existing
        optional<T> opt(42);
        T::reset_constructors();
        opt = 3;
        assert(T::alive == 1);
        assert(T::constructed == 0);
        assert(T::assigned == 1);
        assert(T::value_assigned == 1);
        assert(T::destroyed == 0);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(3));
    }
    { // test default argument
        optional<T> opt;
        T::reset_constructors();
        opt = {1, 2};
        assert(T::alive == 1);
        assert(T::constructed == 2);
        assert(T::value_constructed == 1);
        assert(T::move_constructed == 1);
        assert(T::assigned == 0);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(1, 2));
    }
    { // test default argument
        optional<T> opt(42);
        T::reset_constructors();
        opt = {1, 2};
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::value_constructed == 1);
        assert(T::assigned == 1);
        assert(T::move_assigned == 1);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(1, 2));
    }
    { // test default argument
        optional<T> opt;
        T::reset_constructors();
        opt = {1};
        assert(T::alive == 1);
        assert(T::constructed == 2);
        assert(T::value_constructed == 1);
        assert(T::move_constructed == 1);
        assert(T::assigned == 0);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(1));
    }
    { // test default argument
        optional<T> opt(42);
        T::reset_constructors();
        opt = {};
        assert(static_cast<bool>(opt) == false);
        assert(T::alive == 0);
        assert(T::constructed == 0);
        assert(T::assigned == 0);
        assert(T::destroyed == 1);
    }
}

template <class T, class Value = int>
void test_with_type() {
    { // to empty
        optional<T> opt;
        opt = Value(3);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(3));
    }
    { // to existing
        optional<T> opt(Value(42));
        opt = Value(3);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(3));
    }
    { // test const
        optional<T> opt(Value(42));
        const T t(Value(3));
        opt = t;
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(3));
    }
    { // test default argument
        optional<T> opt;
        opt = {Value(1)};
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(1));
    }
    { // test default argument
        optional<T> opt(Value(42));
        opt = {};
        assert(static_cast<bool>(opt) == false);
    }
}

template <class T>
void test_with_type_multi() {
    test_with_type<T>();
    { // test default argument
        optional<T> opt;
        opt = {1, 2};
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(1, 2));
    }
    { // test default argument
        optional<T> opt(42);
        opt = {1, 2};
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(1, 2));
    }
}

void test_throws()
{
#ifndef TEST_HAS_NO_EXCEPTIONS
    using T = ThrowAssign;
    {
        optional<T> opt;
        try {
            opt = 42;
            assert(false);
        } catch (int) {}
        assert(static_cast<bool>(opt) == false);
    }
    assert(T::dtor_called == 0);
    {
        T::dtor_called = 0;
        optional<T> opt(std::in_place);
        try {
            opt = 42;
            assert(false);
        } catch (int) {}
        assert(static_cast<bool>(opt) == true);
        assert(T::dtor_called == 0);
    }
    assert(T::dtor_called == 1);
#endif
}

enum MyEnum { Zero, One, Two, Three, FortyTwo = 42 };

using Fn = void(*)();

// https://llvm.org/PR38638
template <class T>
constexpr T pr38638(T v)
{
  std::optional<T> o;
  o = v;
  return *o + 2;
}


int run_test()
{
    test_sfinae();
    // Test with instrumented type
    test_with_test_type();
    // Test with various scalar types
    test_with_type<int>();
    test_with_type<MyEnum, MyEnum>();
    test_with_type<int, MyEnum>();
    test_with_type<Fn, Fn>();
    // Test types with multi argument constructors
    test_with_type_multi<ConstexprTestTypes::TestType>();
    test_with_type_multi<TrivialTestTypes::TestType>();
    // Test move only types
    {
        optional<std::unique_ptr<int>> opt;
        opt = std::unique_ptr<int>(new int(3));
        assert(static_cast<bool>(opt) == true);
        assert(**opt == 3);
    }
    {
        optional<std::unique_ptr<int>> opt(std::unique_ptr<int>(new int(2)));
        opt = std::unique_ptr<int>(new int(3));
        assert(static_cast<bool>(opt) == true);
        assert(**opt == 3);
    }
    test_throws();

    static_assert(pr38638(3) == 5, "");

  return 0;
}
} // namespace assign::value
// -- END: test/std/utilities/optional/optional.object/optional.object.assign/assign_value.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.assign/const_optional_U.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// From LWG2451:
// template<class U>
//   optional<T>& operator=(const optional<U>& rhs);

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"

namespace assign::const_optional_U {
using std::optional;

struct X
{
    static bool throw_now;

    X() = default;
    X(int)
    {
        if (throw_now)
            TEST_THROW(6);
    }
};

bool X::throw_now = false;

struct Y1
{
    Y1() = default;
    Y1(const int&) {}
    Y1& operator=(const Y1&) = delete;
};

struct Y2
{
    Y2() = default;
    Y2(const int&) = delete;
    Y2& operator=(const int&) { return *this; }
};

template <class T>
struct AssignableFrom {
  static int type_constructed;
  static int type_assigned;
static int int_constructed;
  static int int_assigned;

  static void reset() {
      type_constructed = int_constructed = 0;
      type_assigned = int_assigned = 0;
  }

  AssignableFrom() = default;

  explicit AssignableFrom(T) { ++type_constructed; }
  AssignableFrom& operator=(T) { ++type_assigned; return *this; }

  AssignableFrom(int) { ++int_constructed; }
  AssignableFrom& operator=(int) { ++int_assigned; return *this; }
private:
  AssignableFrom(AssignableFrom const&) = delete;
  AssignableFrom& operator=(AssignableFrom const&) = delete;
};

template <class T> int AssignableFrom<T>::type_constructed = 0;
template <class T> int AssignableFrom<T>::type_assigned = 0;
template <class T> int AssignableFrom<T>::int_constructed = 0;
template <class T> int AssignableFrom<T>::int_assigned = 0;


void test_with_test_type() {
    using T = TestTypes::TestType;
    T::reset();
    { // non-empty to empty
        T::reset_constructors();
        optional<T> opt;
        const optional<int> other(42);
        opt = other;
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::value_constructed == 1);
        assert(T::assigned == 0);
        assert(T::destroyed == 0);
        assert(static_cast<bool>(other) == true);
        assert(*other == 42);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(42));
    }
    assert(T::alive == 0);
    { // non-empty to non-empty
        optional<T> opt(101);
        const optional<int> other(42);
        T::reset_constructors();
        opt = other;
        assert(T::alive == 1);
        assert(T::constructed == 0);
        assert(T::assigned == 1);
        assert(T::value_assigned == 1);
        assert(T::destroyed == 0);
        assert(static_cast<bool>(other) == true);
        assert(*other == 42);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(42));
    }
    assert(T::alive == 0);
    { // empty to non-empty
        optional<T> opt(101);
        const optional<int> other;
        T::reset_constructors();
        opt = other;
        assert(T::alive == 0);
        assert(T::constructed == 0);
        assert(T::assigned == 0);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(other) == false);
        assert(static_cast<bool>(opt) == false);
    }
    assert(T::alive == 0);
    { // empty to empty
        optional<T> opt;
        const optional<int> other;
        T::reset_constructors();
        opt = other;
        assert(T::alive == 0);
        assert(T::constructed == 0);
        assert(T::assigned == 0);
        assert(T::destroyed == 0);
        assert(static_cast<bool>(other) == false);
        assert(static_cast<bool>(opt) == false);
    }
    assert(T::alive == 0);
}

void test_ambiguous_assign() {
    using OptInt = std::optional<int>;
    {
        using T = AssignableFrom<OptInt const&>;
        const OptInt a(42);
        T::reset();
        {
            std::optional<T> t;
            t = a;
            assert(T::type_constructed == 1);
            assert(T::type_assigned == 0);
            assert(T::int_constructed == 0);
            assert(T::int_assigned == 0);
        }
        T::reset();
        {
            std::optional<T> t(42);
            t = a;
            assert(T::type_constructed == 0);
            assert(T::type_assigned == 1);
            assert(T::int_constructed == 1);
            assert(T::int_assigned == 0);
        }
        T::reset();
        {
            std::optional<T> t(42);
            t = std::move(a);
            assert(T::type_constructed == 0);
            assert(T::type_assigned == 1);
            assert(T::int_constructed == 1);
            assert(T::int_assigned == 0);
        }
    }
    {
        using T = AssignableFrom<OptInt&>;
        OptInt a(42);
        T::reset();
        {
            std::optional<T> t;
            t = a;
            assert(T::type_constructed == 1);
            assert(T::type_assigned == 0);
            assert(T::int_constructed == 0);
            assert(T::int_assigned == 0);
        }
        {
            using Opt = std::optional<T>;
            static_assert(!std::is_assignable_v<Opt&, OptInt const&>, "");
        }
    }
}


int run_test()
{
    test_with_test_type();
    test_ambiguous_assign();
    {
        optional<int> opt;
        constexpr optional<short> opt2;
        opt = opt2;
        static_assert(static_cast<bool>(opt2) == false, "");
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
    }
    {
        optional<int> opt;
        constexpr optional<short> opt2(short{2});
        opt = opt2;
        static_assert(static_cast<bool>(opt2) == true, "");
        static_assert(*opt2 == 2, "");
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
        assert(*opt == *opt2);
    }
    {
        optional<int> opt(3);
        constexpr optional<short> opt2;
        opt = opt2;
        static_assert(static_cast<bool>(opt2) == false, "");
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
    }
    {
        optional<int> opt(3);
        constexpr optional<short> opt2(short{2});
        opt = opt2;
        static_assert(static_cast<bool>(opt2) == true, "");
        static_assert(*opt2 == 2, "");
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
        assert(*opt == *opt2);
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        optional<X> opt;
        optional<int> opt2(42);
        assert(static_cast<bool>(opt2) == true);
        try
        {
            X::throw_now = true;
            opt = opt2;
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
            assert(static_cast<bool>(opt) == false);
        }
    }
#endif

  return 0;
}
} // namespace assign::const_optional_U
// -- END: test/std/utilities/optional/optional.object/optional.object.assign/const_optional_U.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.assign/copy.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// constexpr optional<T>& operator=(const optional<T>& rhs);

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"

namespace assign::copy {
using std::optional;

struct X
{
    static bool throw_now;

    X() = default;
    X(const X&)
    {
        if (throw_now)
            TEST_THROW(6);
    }
    X& operator=(X const&) = default;
};

bool X::throw_now = false;

template <class Tp>
constexpr bool assign_empty(optional<Tp>&& lhs) {
    const optional<Tp> rhs;
    lhs = rhs;
    return !lhs.has_value() && !rhs.has_value();
}

template <class Tp>
constexpr bool assign_value(optional<Tp>&& lhs) {
    const optional<Tp> rhs(101);
    lhs = rhs;
    return lhs.has_value() && rhs.has_value() && *lhs == *rhs;
}

int run_test()
{
    {
        using O = optional<int>;
        static_assert(assign_empty(O{42}));
        static_assert(assign_value(O{42}));
        assert(assign_empty(O{42}));
        assert(assign_value(O{42}));
    }
    {
        using O = optional<TrivialTestTypes::TestType>;
        static_assert(assign_empty(O{42}));
        static_assert(assign_value(O{42}));
        assert(assign_empty(O{42}));
        assert(assign_value(O{42}));
    }
    {
        using O = optional<TestTypes::TestType>;
        assert(assign_empty(O{42}));
        assert(assign_value(O{42}));
    }
    {
        using T = TestTypes::TestType;
        T::reset();
        optional<T> opt(3);
        const optional<T> opt2;
        assert(T::alive == 1);
        opt = opt2;
        assert(T::alive == 0);
        assert(!opt2.has_value());
        assert(!opt.has_value());
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        optional<X> opt;
        optional<X> opt2(X{});
        assert(static_cast<bool>(opt2) == true);
        try
        {
            X::throw_now = true;
            opt = opt2;
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
            assert(static_cast<bool>(opt) == false);
        }
    }
#endif

  return 0;
}
} // namespace assign::copy
// -- END: test/std/utilities/optional/optional.object/optional.object.assign/copy.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.assign/emplace.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class... Args> T& optional<T>::emplace(Args&&... args);

#include <optional>
#include <type_traits>
#include <cassert>
#include <memory>

#include "test_macros.h"
#include "archetypes.h"

namespace assign::emplace {
using std::optional;

class X
{
    int i_;
    int j_ = 0;
public:
    constexpr X() : i_(0) {}
    constexpr X(int i) : i_(i) {}
    constexpr X(int i, int j) : i_(i), j_(j) {}

    friend constexpr bool operator==(const X& x, const X& y)
        {return x.i_ == y.i_ && x.j_ == y.j_;}
};

class Y
{
public:
    static bool dtor_called;
    Y() = default;
    Y(int) { TEST_THROW(6);}
    Y(const Y&) = default;
    Y& operator=(const Y&) = default;
    ~Y() {dtor_called = true;}
};

bool Y::dtor_called = false;

template <class T>
constexpr bool test_one_arg() {
    using Opt = std::optional<T>;
    {
        Opt opt;
        auto & v = opt.emplace();
        static_assert( std::is_same_v<T&, decltype(v)>, "" );
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(0));
        assert(&v == &*opt);
    }
    {
        Opt opt;
        auto & v = opt.emplace(1);
        static_assert( std::is_same_v<T&, decltype(v)>, "" );
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(1));
        assert(&v == &*opt);
    }
    {
        Opt opt(2);
        auto & v = opt.emplace();
        static_assert( std::is_same_v<T&, decltype(v)>, "" );
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(0));
        assert(&v == &*opt);
    }
    {
        Opt opt(2);
        auto & v = opt.emplace(1);
        static_assert( std::is_same_v<T&, decltype(v)>, "" );
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(1));
        assert(&v == &*opt);
    }
    return true;
}


template <class T>
constexpr bool test_multi_arg()
{
    test_one_arg<T>();
    using Opt = std::optional<T>;
    {
        Opt opt;
        auto &v = opt.emplace(101, 41);
        static_assert( std::is_same_v<T&, decltype(v)>, "" );
        assert(static_cast<bool>(opt) == true);
        assert(   v == T(101, 41));
        assert(*opt == T(101, 41));
    }
    {
        Opt opt;
        auto &v = opt.emplace({1, 2, 3, 4});
        static_assert( std::is_same_v<T&, decltype(v)>, "" );
        assert(static_cast<bool>(opt) == true);
        assert(  v == T(4)); // T sets its value to the size of the init list
        assert(*opt == T(4));
    }
    {
        Opt opt;
        auto &v = opt.emplace({1, 2, 3, 4, 5}, 6);
        static_assert( std::is_same_v<T&, decltype(v)>, "" );
        assert(static_cast<bool>(opt) == true);
        assert(  v == T(5)); // T sets its value to the size of the init list
        assert(*opt == T(5)); // T sets its value to the size of the init list
    }
    return true;
}

template <class T>
void test_on_test_type() {

    T::reset();
    optional<T> opt;
    assert(T::alive == 0);
    {
        T::reset_constructors();
        auto &v = opt.emplace();
        static_assert( std::is_same_v<T&, decltype(v)>, "" );
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::default_constructed == 1);
        assert(T::destroyed == 0);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T());
        assert(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto &v = opt.emplace();
        static_assert( std::is_same_v<T&, decltype(v)>, "" );
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::default_constructed == 1);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T());
        assert(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto &v = opt.emplace(101);
        static_assert( std::is_same_v<T&, decltype(v)>, "" );
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::value_constructed == 1);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(101));
        assert(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto &v = opt.emplace(-10, 99);
        static_assert( std::is_same_v<T&, decltype(v)>, "" );
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::value_constructed == 1);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(-10, 99));
        assert(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto &v = opt.emplace(-10, 99);
        static_assert( std::is_same_v<T&, decltype(v)>, "" );
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::value_constructed == 1);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(-10, 99));
        assert(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto &v = opt.emplace({-10, 99, 42, 1});
        static_assert( std::is_same_v<T&, decltype(v)>, "" );
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::value_constructed == 1);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(4)); // size of the initializer list
        assert(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto &v = opt.emplace({-10, 99, 42, 1}, 42);
        static_assert( std::is_same_v<T&, decltype(v)>, "" );
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::value_constructed == 1);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(4)); // size of the initializer list
        assert(&v == &*opt);
    }
}

TEST_CONSTEXPR_CXX20 bool test_empty_emplace() {
  optional<const int> opt;
  auto& v = opt.emplace(42);
  static_assert(std::is_same_v<const int&, decltype(v)>, "");
  assert(*opt == 42);
  assert(v == 42);
  opt.emplace();
  assert(*opt == 0);
  return true;
}

int run_test()
{
    {
        test_on_test_type<TestTypes::TestType>();
        test_on_test_type<ExplicitTestTypes::TestType>();
    }
    {
        using T = int;
        test_one_arg<T>();
        test_one_arg<const T>();
#if TEST_STD_VER > 17
        static_assert(test_one_arg<T>());
        static_assert(test_one_arg<const T>());
#endif
    }
    {
        using T = ConstexprTestTypes::TestType;
        test_multi_arg<T>();
#if TEST_STD_VER > 17
        static_assert(test_multi_arg<T>());
#endif
    }
    {
        using T = ExplicitConstexprTestTypes::TestType;
        test_multi_arg<T>();
#if TEST_STD_VER > 17
        static_assert(test_multi_arg<T>());
#endif
    }
    {
        using T = TrivialTestTypes::TestType;
        test_multi_arg<T>();
#if TEST_STD_VER > 17
        static_assert(test_multi_arg<T>());
#endif
    }
    {
        using T = ExplicitTrivialTestTypes::TestType;
        test_multi_arg<T>();
#if TEST_STD_VER > 17
        static_assert(test_multi_arg<T>());
#endif
    }
    {
        test_empty_emplace();
#if TEST_STD_VER > 17
        static_assert(test_empty_emplace());
#endif
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    Y::dtor_called = false;
    {
        Y y;
        optional<Y> opt(y);
        try
        {
            assert(static_cast<bool>(opt) == true);
            assert(Y::dtor_called == false);
            auto &v = opt.emplace(1);
            static_assert( std::is_same_v<Y&, decltype(v)>, "" );
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
            assert(static_cast<bool>(opt) == false);
            assert(Y::dtor_called == true);
        }
    }
#endif

  return 0;
}
} // namespace assign::emplace
// -- END: test/std/utilities/optional/optional.object/optional.object.assign/emplace.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.assign/emplace_initializer_list.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class U, class... Args>
//   T& optional<T>::emplace(initializer_list<U> il, Args&&... args);

#include <optional>
#include <type_traits>
#include <cassert>
#include <vector>

#include "test_macros.h"

namespace assign::emplace_initializer_list {
using std::optional;

class X
{
    int i_;
    int j_ = 0;
    bool* dtor_called_;
public:
    constexpr X(bool& dtor_called) : i_(0), dtor_called_(&dtor_called) {}
    constexpr X(int i, bool& dtor_called) : i_(i), dtor_called_(&dtor_called) {}
    constexpr X(std::initializer_list<int> il, bool& dtor_called)
    : i_(il.begin()[0]), j_(il.begin()[1]), dtor_called_(&dtor_called) {}
    X(const X&) = default;
    X& operator=(const X&) = default;
    TEST_CONSTEXPR_CXX20 ~X() {*dtor_called_ = true;}

    friend constexpr bool operator==(const X& x, const X& y)
        {return x.i_ == y.i_ && x.j_ == y.j_;}
};

class Y
{
    int i_;
    int j_ = 0;
public:
    constexpr Y() : i_(0) {}
    constexpr Y(int i) : i_(i) {}
    constexpr Y(std::initializer_list<int> il) : i_(il.begin()[0]), j_(il.begin()[1]) {}

    friend constexpr bool operator==(const Y& x, const Y& y)
        {return x.i_ == y.i_ && x.j_ == y.j_;}
};

class Z
{
    int i_;
    int j_ = 0;
public:
    static bool dtor_called;
    Z() : i_(0) {}
    Z(int i) : i_(i) {}
    Z(std::initializer_list<int> il) : i_(il.begin()[0]), j_(il.begin()[1])
        { TEST_THROW(6);}
    Z(const Z&) = default;
    Z& operator=(const Z&) = default;
    ~Z() {dtor_called = true;}

    friend bool operator==(const Z& x, const Z& y)
        {return x.i_ == y.i_ && x.j_ == y.j_;}
};

bool Z::dtor_called = false;

TEST_CONSTEXPR_CXX20 bool check_X()
{
    bool dtor_called = false;
    X x(dtor_called);
    optional<X> opt(x);
    assert(dtor_called == false);
    auto &v = opt.emplace({1, 2}, dtor_called);
    static_assert( std::is_same_v<X&, decltype(v)>, "" );
    assert(dtor_called);
    assert(*opt == X({1, 2}, dtor_called));
    assert(&v == &*opt);
    return true;
}

TEST_CONSTEXPR_CXX20 bool check_Y()
{
    optional<Y> opt;
    auto &v = opt.emplace({1, 2});
    static_assert( std::is_same_v<Y&, decltype(v)>, "" );
    assert(static_cast<bool>(opt) == true);
    assert(*opt == Y({1, 2}));
    assert(&v == &*opt);
    return true;
}

int run_test()
{
    {
        check_X();
#if TEST_STD_VER > 17
        static_assert(check_X());
#endif
    }
    {
        optional<std::vector<int>> opt;
        auto &v = opt.emplace({1, 2, 3}, std::allocator<int>());
        static_assert( std::is_same_v<std::vector<int>&, decltype(v)>, "" );
        assert(static_cast<bool>(opt) == true);
        assert(*opt == std::vector<int>({1, 2, 3}));
        assert(&v == &*opt);
    }
    {
        check_Y();
#if TEST_STD_VER > 17
        static_assert(check_Y());
#endif
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        Z z;
        optional<Z> opt(z);
        try
        {
            assert(static_cast<bool>(opt) == true);
            assert(Z::dtor_called == false);
            auto &v = opt.emplace({1, 2});
            static_assert( std::is_same_v<Z&, decltype(v)>, "" );
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
            assert(static_cast<bool>(opt) == false);
            assert(Z::dtor_called == true);
        }
    }
#endif

  return 0;
}
} // namespace assign::emplace_initializer_list
// -- END: test/std/utilities/optional/optional.object/optional.object.assign/emplace_initializer_list.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.assign/move.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// constexpr optional<T>& operator=(optional<T>&& rhs)
//     noexcept(is_nothrow_move_assignable<T>::value &&
//              is_nothrow_move_constructible<T>::value);

#include <optional>
#include <cassert>
#include <type_traits>
#include <utility>

#include "test_macros.h"
#include "archetypes.h"

namespace assign::move {
using std::optional;

struct X
{
    static bool throw_now;
    static int alive;

    X() { ++alive; }
    X(X&&)
    {
        if (throw_now)
            TEST_THROW(6);
        ++alive;
    }

    X& operator=(X&&)
    {
        if (throw_now)
            TEST_THROW(42);
        return *this;
    }

    ~X() { assert(alive > 0); --alive; }
};

struct Y {};

bool X::throw_now = false;
int X::alive = 0;


template <class Tp>
constexpr bool assign_empty(optional<Tp>&& lhs) {
    optional<Tp> rhs;
    lhs = std::move(rhs);
    return !lhs.has_value() && !rhs.has_value();
}

template <class Tp>
constexpr bool assign_value(optional<Tp>&& lhs) {
    optional<Tp> rhs(101);
    lhs = std::move(rhs);
    return lhs.has_value() && rhs.has_value() && *lhs == Tp{101};
}

int run_test()
{
    {
        static_assert(std::is_nothrow_move_assignable<optional<int>>::value, "");
        optional<int> opt;
        constexpr optional<int> opt2;
        opt = std::move(opt2);
        static_assert(static_cast<bool>(opt2) == false, "");
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
    }
    {
        optional<int> opt;
        constexpr optional<int> opt2(2);
        opt = std::move(opt2);
        static_assert(static_cast<bool>(opt2) == true, "");
        static_assert(*opt2 == 2, "");
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
        assert(*opt == *opt2);
    }
    {
        optional<int> opt(3);
        constexpr optional<int> opt2;
        opt = std::move(opt2);
        static_assert(static_cast<bool>(opt2) == false, "");
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
    }
    {
        using T = TestTypes::TestType;
        T::reset();
        optional<T> opt(3);
        optional<T> opt2;
        assert(T::alive == 1);
        opt = std::move(opt2);
        assert(T::alive == 0);
        assert(static_cast<bool>(opt2) == false);
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
    }
    {
        optional<int> opt(3);
        constexpr optional<int> opt2(2);
        opt = std::move(opt2);
        static_assert(static_cast<bool>(opt2) == true, "");
        static_assert(*opt2 == 2, "");
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
        assert(*opt == *opt2);
    }
    {
        using O = optional<int>;
        static_assert(assign_empty(O{42}));
        static_assert(assign_value(O{42}));
        assert(assign_empty(O{42}));
        assert(assign_value(O{42}));
    }
    {
        using O = optional<TrivialTestTypes::TestType>;
        static_assert(assign_empty(O{42}));
        static_assert(assign_value(O{42}));
        assert(assign_empty(O{42}));
        assert(assign_value(O{42}));
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        static_assert(!std::is_nothrow_move_assignable<optional<X>>::value, "");
        X::alive = 0;
        X::throw_now = false;
        optional<X> opt;
        optional<X> opt2(X{});
        assert(X::alive == 1);
        assert(static_cast<bool>(opt2) == true);
        try
        {
            X::throw_now = true;
            opt = std::move(opt2);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
            assert(static_cast<bool>(opt) == false);
        }
        assert(X::alive == 1);
    }
    assert(X::alive == 0);
    {
        static_assert(!std::is_nothrow_move_assignable<optional<X>>::value, "");
        X::throw_now = false;
        optional<X> opt(X{});
        optional<X> opt2(X{});
        assert(X::alive == 2);
        assert(static_cast<bool>(opt2) == true);
        try
        {
            X::throw_now = true;
            opt = std::move(opt2);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 42);
            assert(static_cast<bool>(opt) == true);
        }
        assert(X::alive == 2);
    }
    assert(X::alive == 0);
#endif // TEST_HAS_NO_EXCEPTIONS
    {
        static_assert(std::is_nothrow_move_assignable<optional<Y>>::value, "");
    }
    {
        struct ThrowsMove {
            ThrowsMove() noexcept {}
            ThrowsMove(ThrowsMove const&) noexcept {}
            ThrowsMove(ThrowsMove &&) noexcept(false) {}
            ThrowsMove& operator=(ThrowsMove const&) noexcept { return *this; }
            ThrowsMove& operator=(ThrowsMove &&) noexcept { return *this; }
        };
        static_assert(!std::is_nothrow_move_assignable<optional<ThrowsMove>>::value, "");
        struct ThrowsMoveAssign {
            ThrowsMoveAssign() noexcept {}
            ThrowsMoveAssign(ThrowsMoveAssign const&) noexcept {}
            ThrowsMoveAssign(ThrowsMoveAssign &&) noexcept {}
            ThrowsMoveAssign& operator=(ThrowsMoveAssign const&) noexcept { return *this; }
            ThrowsMoveAssign& operator=(ThrowsMoveAssign &&) noexcept(false) { return *this; }
        };
        static_assert(!std::is_nothrow_move_assignable<optional<ThrowsMoveAssign>>::value, "");
        struct NoThrowMove {
            NoThrowMove() noexcept(false) {}
            NoThrowMove(NoThrowMove const&) noexcept(false) {}
            NoThrowMove(NoThrowMove &&) noexcept {}
            NoThrowMove& operator=(NoThrowMove const&) noexcept { return *this; }
            NoThrowMove& operator=(NoThrowMove&&) noexcept { return *this; }
        };
        static_assert(std::is_nothrow_move_assignable<optional<NoThrowMove>>::value, "");
    }
    return 0;
}
} // namespace assign::move
// -- END: test/std/utilities/optional/optional.object/optional.object.assign/move.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.assign/nullopt_t.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// optional<T>& operator=(nullopt_t) noexcept;

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"

namespace assign::nullopt {
using std::optional;
using std::nullopt_t;
using std::nullopt;

TEST_CONSTEXPR_CXX20 bool test()
{
    enum class State { inactive, constructed, destroyed };
    State state = State::inactive;

    struct StateTracker {
      TEST_CONSTEXPR_CXX20 StateTracker(State& s)
      : state_(&s)
      {
        s = State::constructed;
      }
      TEST_CONSTEXPR_CXX20 ~StateTracker() { *state_ = State::destroyed; }

      State* state_;
    };
    {
        optional<int> opt;
        static_assert(noexcept(opt = nullopt) == true, "");
        opt = nullopt;
        assert(static_cast<bool>(opt) == false);
    }
    {
        optional<int> opt(3);
        opt = nullopt;
        assert(static_cast<bool>(opt) == false);
    }
    {
        optional<StateTracker> opt;
        opt = nullopt;
        assert(state == State::inactive);
        assert(static_cast<bool>(opt) == false);
    }
    {
        optional<StateTracker> opt(state);
        assert(state == State::constructed);
        opt = nullopt;
        assert(state == State::destroyed);
        assert(static_cast<bool>(opt) == false);
    }
    return true;
}


int run_test()
{
#if TEST_STD_VER > 17
    static_assert(test());
#endif
    test();
    using TT = TestTypes::TestType;
    TT::reset();
    {
        optional<TT> opt;
        static_assert(noexcept(opt = nullopt) == true, "");
        assert(TT::destroyed == 0);
        opt = nullopt;
        assert(TT::constructed == 0);
        assert(TT::alive == 0);
        assert(TT::destroyed == 0);
        assert(static_cast<bool>(opt) == false);
    }
    assert(TT::alive == 0);
    assert(TT::destroyed == 0);
    TT::reset();
    {
        optional<TT> opt(42);
        assert(TT::destroyed == 0);
        TT::reset_constructors();
        opt = nullopt;
        assert(TT::constructed == 0);
        assert(TT::alive == 0);
        assert(TT::destroyed == 1);
        assert(static_cast<bool>(opt) == false);
    }
    assert(TT::alive == 0);
    assert(TT::destroyed == 1);
    TT::reset();

  return 0;
}
} // namespace assign::nullopt
// -- END: test/std/utilities/optional/optional.object/optional.object.assign/nullopt_t.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.assign/optional_U.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// From LWG2451:
// template <class U>
// optional<T>& operator=(optional<U>&& rhs);

#include <optional>

#include <array>
#include <cassert>
#include <memory>
#include <type_traits>

#include "test_macros.h"
#include "archetypes.h"

namespace assign::optional_U {
using std::optional;

struct X
{
    static bool throw_now;

    X() = default;
    X(int &&)
    {
        if (throw_now)
            TEST_THROW(6);
    }
};

bool X::throw_now = false;

struct Y1
{
    Y1() = default;
    Y1(const int&) {}
    Y1& operator=(const Y1&) = delete;
};

struct Y2
{
    Y2() = default;
    Y2(const int&) = delete;
    Y2& operator=(const int&) { return *this; }
};

struct B { virtual ~B() = default; };
class D : public B {};


template <class T>
struct AssignableFrom {
  static int type_constructed;
  static int type_assigned;
static int int_constructed;
  static int int_assigned;

  static void reset() {
      type_constructed = int_constructed = 0;
      type_assigned = int_assigned = 0;
  }

  AssignableFrom() = default;

  explicit AssignableFrom(T) { ++type_constructed; }
  AssignableFrom& operator=(T) { ++type_assigned; return *this; }

  AssignableFrom(int) { ++int_constructed; }
  AssignableFrom& operator=(int) { ++int_assigned; return *this; }
private:
  AssignableFrom(AssignableFrom const&) = delete;
  AssignableFrom& operator=(AssignableFrom const&) = delete;
};

template <class T> int AssignableFrom<T>::type_constructed = 0;
template <class T> int AssignableFrom<T>::type_assigned = 0;
template <class T> int AssignableFrom<T>::int_constructed = 0;
template <class T> int AssignableFrom<T>::int_assigned = 0;

void test_with_test_type() {
    using T = TestTypes::TestType;
    T::reset();
    { // non-empty to empty
        T::reset_constructors();
        optional<T> opt;
        optional<int> other(42);
        opt = std::move(other);
        assert(T::alive == 1);
        assert(T::constructed == 1);
        assert(T::value_constructed == 1);
        assert(T::assigned == 0);
        assert(T::destroyed == 0);
        assert(static_cast<bool>(other) == true);
        assert(*other == 42);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(42));
    }
    assert(T::alive == 0);
    { // non-empty to non-empty
        optional<T> opt(101);
        optional<int> other(42);
        T::reset_constructors();
        opt = std::move(other);
        assert(T::alive == 1);
        assert(T::constructed == 0);
        assert(T::assigned == 1);
        assert(T::value_assigned == 1);
        assert(T::destroyed == 0);
        assert(static_cast<bool>(other) == true);
        assert(*other == 42);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == T(42));
    }
    assert(T::alive == 0);
    { // empty to non-empty
        optional<T> opt(101);
        optional<int> other;
        T::reset_constructors();
        opt = std::move(other);
        assert(T::alive == 0);
        assert(T::constructed == 0);
        assert(T::assigned == 0);
        assert(T::destroyed == 1);
        assert(static_cast<bool>(other) == false);
        assert(static_cast<bool>(opt) == false);
    }
    assert(T::alive == 0);
    { // empty to empty
        optional<T> opt;
        optional<int> other;
        T::reset_constructors();
        opt = std::move(other);
        assert(T::alive == 0);
        assert(T::constructed == 0);
        assert(T::assigned == 0);
        assert(T::destroyed == 0);
        assert(static_cast<bool>(other) == false);
        assert(static_cast<bool>(opt) == false);
    }
    assert(T::alive == 0);
}


void test_ambiguous_assign() {
    using OptInt = std::optional<int>;
    {
        using T = AssignableFrom<OptInt&&>;
        T::reset();
        {
            OptInt a(42);
            std::optional<T> t;
            t = std::move(a);
            assert(T::type_constructed == 1);
            assert(T::type_assigned == 0);
            assert(T::int_constructed == 0);
            assert(T::int_assigned == 0);
        }
        {
            using Opt = std::optional<T>;
            static_assert(!std::is_assignable<Opt&, const OptInt&&>::value, "");
            static_assert(!std::is_assignable<Opt&, const OptInt&>::value, "");
            static_assert(!std::is_assignable<Opt&, OptInt&>::value, "");
        }
    }
    {
        using T = AssignableFrom<OptInt const&&>;
        T::reset();
        {
            const OptInt a(42);
            std::optional<T> t;
            t = std::move(a);
            assert(T::type_constructed == 1);
            assert(T::type_assigned == 0);
            assert(T::int_constructed == 0);
            assert(T::int_assigned == 0);
        }
        T::reset();
        {
            OptInt a(42);
            std::optional<T> t;
            t = std::move(a);
            assert(T::type_constructed == 1);
            assert(T::type_assigned == 0);
            assert(T::int_constructed == 0);
            assert(T::int_assigned == 0);
        }
        {
            using Opt = std::optional<T>;
            static_assert(std::is_assignable<Opt&, OptInt&&>::value, "");
            static_assert(!std::is_assignable<Opt&, const OptInt&>::value, "");
            static_assert(!std::is_assignable<Opt&, OptInt&>::value, "");
        }
    }
}


TEST_CONSTEXPR_CXX20 bool test()
{
    {
        optional<int> opt;
        optional<short> opt2;
        opt = std::move(opt2);
        assert(static_cast<bool>(opt2) == false);
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
    }
    {
        optional<int> opt;
        optional<short> opt2(short{2});
        opt = std::move(opt2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
        assert(*opt == *opt2);
    }
    {
        optional<int> opt(3);
        optional<short> opt2;
        opt = std::move(opt2);
        assert(static_cast<bool>(opt2) == false);
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
    }
    {
        optional<int> opt(3);
        optional<short> opt2(short{2});
        opt = std::move(opt2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
        assert(*opt == *opt2);
    }

    enum class state_t { inactive, constructed, copy_assigned, move_assigned };
    class StateTracker {
    public:
      constexpr StateTracker(state_t& s)
      : state_(&s)
      {
        *state_ = state_t::constructed;
      }

      StateTracker(StateTracker&&) = default;
      StateTracker(StateTracker const&) = default;

      constexpr StateTracker& operator=(StateTracker&& other) noexcept
      {
        *state_ = state_t::inactive;
        state_ = other.state_;
        *state_ = state_t::move_assigned;
        other.state_ = nullptr;
        return *this;
      }

      constexpr StateTracker& operator=(StateTracker const& other) noexcept
      {
        *state_ = state_t::inactive;
        state_ = other.state_;
        *state_ = state_t::copy_assigned;
        return *this;
      }
    private:
      state_t* state_;
    };
    {
      auto state = std::array{state_t::inactive, state_t::inactive};
      auto opt1 = std::optional<StateTracker>(state[0]);
      assert(state[0] == state_t::constructed);

      auto opt2 = std::optional<StateTracker>(state[1]);
      assert(state[1] == state_t::constructed);

      opt1 = std::move(opt2);
      assert(state[0] == state_t::inactive);
      assert(state[1] == state_t::move_assigned);
    }
    {
      auto state = std::array{state_t::inactive, state_t::inactive};
      auto opt1 = std::optional<StateTracker>(state[0]);
      assert(state[0] == state_t::constructed);

      auto opt2 = std::optional<StateTracker>(state[1]);
      assert(state[1] == state_t::constructed);

      opt1 = opt2;
      assert(state[0] == state_t::inactive);
      assert(state[1] == state_t::copy_assigned);
    }

    return true;
}


int run_test()
{
#if TEST_STD_VER > 17
    static_assert(test());
#endif
    test_with_test_type();
    test_ambiguous_assign();
    test();
    {
        optional<std::unique_ptr<B>> opt;
        optional<std::unique_ptr<D>> other(new D());
        opt = std::move(other);
        assert(static_cast<bool>(opt) == true);
        assert(static_cast<bool>(other) == true);
        assert(opt->get() != nullptr);
        assert(other->get() == nullptr);
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        optional<X> opt;
        optional<int> opt2(42);
        assert(static_cast<bool>(opt2) == true);
        try
        {
            X::throw_now = true;
            opt = std::move(opt2);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
            assert(static_cast<bool>(opt) == false);
        }
    }
#endif

  return 0;
}
} // namespace assign::optional_U
// -- END: test/std/utilities/optional/optional.object/optional.object.assign/optional_U.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.ctor/const_optional_U.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class U>
//   optional(const optional<U>& rhs);

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace ctor::const_optional_U {
using std::optional;

template <class T, class U>
TEST_CONSTEXPR_CXX20 void
test(const optional<U>& rhs, bool is_going_to_throw = false)
{
    bool rhs_engaged = static_cast<bool>(rhs);
#ifndef TEST_HAS_NO_EXCEPTIONS
    try
    {
        optional<T> lhs = rhs;
        assert(is_going_to_throw == false);
        assert(static_cast<bool>(lhs) == rhs_engaged);
        if (rhs_engaged)
            assert(*lhs == *rhs);
    }
    catch (int i)
    {
        assert(i == 6);
    }
#else
    if (is_going_to_throw) return;
    optional<T> lhs = rhs;
    assert(static_cast<bool>(lhs) == rhs_engaged);
    if (rhs_engaged)
        assert(*lhs == *rhs);
#endif
}

class X
{
    int i_;
public:
    constexpr X(int i) : i_(i) {}
    constexpr X(const X& x) : i_(x.i_) {}
    TEST_CONSTEXPR_CXX20 ~X() {i_ = 0;}
    friend constexpr bool operator==(const X& x, const X& y) {return x.i_ == y.i_;}
};

class Y
{
    int i_;
public:
    constexpr Y(int i) : i_(i) {}

    friend constexpr bool operator==(const Y& x, const Y& y) {return x.i_ == y.i_;}
};

int count = 0;

class Z
{
    int i_;
public:
    Z(int i) : i_(i) {TEST_THROW(6);}

    friend bool operator==(const Z& x, const Z& y) {return x.i_ == y.i_;}
};

template<class T, class U>
constexpr bool test_all()
{
  {
    optional<U> rhs;
    test<T>(rhs);
  }
  {
    optional<U> rhs(U{3});
    test<T>(rhs);
  }
  return true;
}

int run_test()
{
    test_all<int, short>();
    test_all<X, int>();
    test_all<Y, int>();
#if TEST_STD_VER > 17
    static_assert(test_all<int, short>());
    static_assert(test_all<X, int>());
    static_assert(test_all<Y, int>());
#endif
    {
        typedef Z T;
        typedef int U;
        optional<U> rhs;
        test<T>(rhs);
    }
    {
        typedef Z T;
        typedef int U;
        optional<U> rhs(U{3});
        test<T>(rhs, true);
    }

    static_assert(!(std::is_constructible<optional<X>, const optional<Y>&>::value), "");

  return 0;
}
} // namespace ctor::const_optional_U
// -- END: test/std/utilities/optional/optional.object/optional.object.ctor/const_optional_U.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.ctor/const_T.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//


// <optional>

// constexpr optional(const T& v);

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"

namespace ctor::const_T {
using std::optional;

int run_test()
{
    {
        typedef int T;
        constexpr T t(5);
        constexpr optional<T> opt(t);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == 5, "");

        struct test_constexpr_ctor
            : public optional<T>
        {
            constexpr test_constexpr_ctor(const T&) {}
        };

    }
    {
        typedef double T;
        constexpr T t(3);
        constexpr optional<T> opt(t);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == 3, "");

        struct test_constexpr_ctor
            : public optional<T>
        {
            constexpr test_constexpr_ctor(const T&) {}
        };

    }
    {
        const int x = 42;
        optional<const int> o(x);
        assert(*o == x);
    }
    {
        typedef TestTypes::TestType T;
        T::reset();
        const T t(3);
        optional<T> opt = t;
        assert(T::alive == 2);
        assert(T::copy_constructed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(opt.value().value == 3);
    }
    {
        typedef ExplicitTestTypes::TestType T;
        static_assert(!std::is_convertible<T const&, optional<T>>::value, "");
        T::reset();
        const T t(3);
        optional<T> opt(t);
        assert(T::alive == 2);
        assert(T::copy_constructed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(opt.value().value == 3);
    }
    {
        typedef ConstexprTestTypes::TestType T;
        constexpr T t(3);
        constexpr optional<T> opt = {t};
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(opt.value().value == 3, "");

        struct test_constexpr_ctor
            : public optional<T>
        {
            constexpr test_constexpr_ctor(const T&) {}
        };
    }
    {
        typedef ExplicitConstexprTestTypes::TestType T;
        static_assert(!std::is_convertible<const T&, optional<T>>::value, "");
        constexpr T t(3);
        constexpr optional<T> opt(t);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(opt.value().value == 3, "");

        struct test_constexpr_ctor
            : public optional<T>
        {
            constexpr test_constexpr_ctor(const T&) {}
        };

    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        struct Z {
            Z(int) {}
            Z(const Z&) {throw 6;}
        };
        typedef Z T;
        try
        {
            const T t(3);
            optional<T> opt(t);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
        }
    }
#endif

  return 0;
}
} // namespace ctor::const_T
// -- END: test/std/utilities/optional/optional.object/optional.object.ctor/const_T.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.ctor/copy.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// constexpr optional(const optional<T>& rhs);

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"

namespace ctor::copy {
using std::optional;

template <class T, class ...InitArgs>
void test(InitArgs&&... args)
{
    const optional<T> rhs(std::forward<InitArgs>(args)...);
    bool rhs_engaged = static_cast<bool>(rhs);
    optional<T> lhs = rhs;
    assert(static_cast<bool>(lhs) == rhs_engaged);
    if (rhs_engaged)
        assert(*lhs == *rhs);
}

template <class T, class ...InitArgs>
constexpr bool constexpr_test(InitArgs&&... args)
{
    static_assert( std::is_trivially_copy_constructible_v<T>, ""); // requirement
    const optional<T> rhs(std::forward<InitArgs>(args)...);
    optional<T> lhs = rhs;
    return (lhs.has_value() == rhs.has_value()) &&
           (lhs.has_value() ? *lhs == *rhs : true);
}

void test_throwing_ctor() {
#ifndef TEST_HAS_NO_EXCEPTIONS
    struct Z {
        Z() : count(0) {}
        Z(Z const& o) : count(o.count + 1)
        { if (count == 2) throw 6; }
        int count;
    };
    const Z z;
    const optional<Z> rhs(z);
    try
    {
        optional<Z> lhs(rhs);
        assert(false);
    }
    catch (int i)
    {
        assert(i == 6);
    }
#endif
}

template <class T, class ...InitArgs>
void test_ref(InitArgs&&... args)
{
    const optional<T> rhs(std::forward<InitArgs>(args)...);
    bool rhs_engaged = static_cast<bool>(rhs);
    optional<T> lhs = rhs;
    assert(static_cast<bool>(lhs) == rhs_engaged);
    if (rhs_engaged)
        assert(&(*lhs) == &(*rhs));
}


void test_reference_extension()
{
#if defined(_LIBCPP_VERSION) && 0 // TODO these extensions are currently disabled.
    using T = TestTypes::TestType;
    T::reset();
    {
        T t;
        T::reset_constructors();
        test_ref<T&>();
        test_ref<T&>(t);
        assert(T::alive == 1);
        assert(T::constructed == 0);
        assert(T::assigned == 0);
        assert(T::destroyed == 0);
    }
    assert(T::destroyed == 1);
    assert(T::alive == 0);
    {
        T t;
        const T& ct = t;
        T::reset_constructors();
        test_ref<T const&>();
        test_ref<T const&>(t);
        test_ref<T const&>(ct);
        assert(T::alive == 1);
        assert(T::constructed == 0);
        assert(T::assigned == 0);
        assert(T::destroyed == 0);
    }
    assert(T::alive == 0);
    assert(T::destroyed == 1);
    {
        static_assert(!std::is_copy_constructible<std::optional<T&&>>::value, "");
        static_assert(!std::is_copy_constructible<std::optional<T const&&>>::value, "");
    }
#endif
}

int run_test()
{
    test<int>();
    test<int>(3);
    static_assert(constexpr_test<int>(), "" );
    static_assert(constexpr_test<int>(3), "" );

    {
        const optional<const int> o(42);
        optional<const int> o2(o);
        assert(*o2 == 42);
    }
    {
        using T = TestTypes::TestType;
        T::reset();
        const optional<T> rhs;
        assert(T::alive == 0);
        const optional<T> lhs(rhs);
        assert(lhs.has_value() == false);
        assert(T::alive == 0);
    }
    TestTypes::TestType::reset();
    {
        using T = TestTypes::TestType;
        T::reset();
        const optional<T> rhs(42);
        assert(T::alive == 1);
        assert(T::value_constructed == 1);
        assert(T::copy_constructed == 0);
        const optional<T> lhs(rhs);
        assert(lhs.has_value());
        assert(T::copy_constructed == 1);
        assert(T::alive == 2);
    }
    TestTypes::TestType::reset();
    {
        using namespace ConstexprTestTypes;
        test<TestType>();
        test<TestType>(42);
    }
    {
        using namespace TrivialTestTypes;
        test<TestType>();
        test<TestType>(42);
    }
    {
        test_throwing_ctor();
    }
    {
        test_reference_extension();
    }
    {
        constexpr std::optional<int> o1{4};
        constexpr std::optional<int> o2 = o1;
        static_assert( *o2 == 4, "" );
    }

    // LWG3836 https://wg21.link/LWG3836
    // std::optional<bool> conversion constructor optional(const optional<U>&)
    // should take precedence over optional(U&&) with operator bool
    {
        std::optional<bool> o1(false);
        std::optional<bool> o2(o1);
        assert(!o2.value());
    }

  return 0;
}
} // namespace ctor::copy
// -- END: test/std/utilities/optional/optional.object/optional.object.ctor/copy.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.ctor/deduct.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <optional>


// template<class T>
//   optional(T) -> optional<T>;

#include <optional>
#include <cassert>

#include "test_macros.h"

namespace ctor::deduct {
struct A {};

int run_test()
{
//  Test the explicit deduction guides
    {
//  optional(T)
    std::optional opt(5);
    ASSERT_SAME_TYPE(decltype(opt), std::optional<int>);
    assert(static_cast<bool>(opt));
    assert(*opt == 5);
    }

    {
//  optional(T)
    std::optional opt(A{});
    ASSERT_SAME_TYPE(decltype(opt), std::optional<A>);
    assert(static_cast<bool>(opt));
    }

    {
//  optional(const T&);
    const int& source = 5;
    std::optional opt(source);
    ASSERT_SAME_TYPE(decltype(opt), std::optional<int>);
    assert(static_cast<bool>(opt));
    assert(*opt == 5);
    }

    {
//  optional(T*);
    const int* source = nullptr;
    std::optional opt(source);
    ASSERT_SAME_TYPE(decltype(opt), std::optional<const int*>);
    assert(static_cast<bool>(opt));
    assert(*opt == nullptr);
    }

    {
//  optional(T[]);
    int source[] = {1, 2, 3};
    std::optional opt(source);
    ASSERT_SAME_TYPE(decltype(opt), std::optional<int*>);
    assert(static_cast<bool>(opt));
    assert((*opt)[0] == 1);
    }

//  Test the implicit deduction guides
    {
//  optional(optional);
    std::optional<char> source('A');
    std::optional opt(source);
    ASSERT_SAME_TYPE(decltype(opt), std::optional<char>);
    assert(static_cast<bool>(opt) == static_cast<bool>(source));
    assert(*opt == *source);
    }

  return 0;
}
} // namespace ctor::deduct
// -- END: test/std/utilities/optional/optional.object/optional.object.ctor/deduct.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.ctor/default.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// constexpr optional() noexcept;

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"

namespace ctor::default_ {
using std::optional;

template <class Opt>
void
test_constexpr()
{
    static_assert(std::is_nothrow_default_constructible<Opt>::value, "");
    static_assert(std::is_trivially_destructible<Opt>::value, "");
    static_assert(std::is_trivially_destructible<typename Opt::value_type>::value, "");

    constexpr Opt opt;
    static_assert(static_cast<bool>(opt) == false, "");

    struct test_constexpr_ctor
        : public Opt
    {
        constexpr test_constexpr_ctor() {}
    };
}

template <class Opt>
void
test()
{
    static_assert(std::is_nothrow_default_constructible<Opt>::value, "");
    static_assert(!std::is_trivially_destructible<Opt>::value, "");
    static_assert(!std::is_trivially_destructible<typename Opt::value_type>::value, "");
    {
        Opt opt;
        assert(static_cast<bool>(opt) == false);
    }
    {
        const Opt opt;
        assert(static_cast<bool>(opt) == false);
    }

    struct test_constexpr_ctor
        : public Opt
    {
        constexpr test_constexpr_ctor() {}
    };
}

int run_test()
{
    test_constexpr<optional<int>>();
    test_constexpr<optional<int*>>();
    test_constexpr<optional<ImplicitTypes::NoCtors>>();
    test_constexpr<optional<NonTrivialTypes::NoCtors>>();
    test_constexpr<optional<NonConstexprTypes::NoCtors>>();
    test<optional<NonLiteralTypes::NoCtors>>();
    // EXTENSIONS
#if defined(_LIBCPP_VERSION) && 0 // TODO these extensions are currently disabled.
    test_constexpr<optional<int&>>();
    test_constexpr<optional<const int&>>();
    test_constexpr<optional<int&>>();
    test_constexpr<optional<NonLiteralTypes::NoCtors&>>();
    test_constexpr<optional<NonLiteralTypes::NoCtors&&>>();
#endif

  return 0;
}
} // namespace ctor::default_
// -- END: test/std/utilities/optional/optional.object/optional.object.ctor/default.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.ctor/empty_in_place_t_does_not_clobber.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// constexpr optional(in_place_t);

// Test that the SFINAE "is_constructible<value_type>" isn't evaluated by the
// in_place_t constructor with no arguments when the Clang is trying to check
// copy constructor.

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"

namespace ctor::clobber {
using std::optional;

struct Wrapped {
  struct Inner {
    bool Dummy = true;
  };
  std::optional<Inner> inner;
};

int run_test() {
  static_assert(std::is_default_constructible<Wrapped::Inner>::value, "");
  Wrapped w;
  w.inner.emplace();
  assert(w.inner.has_value());

  return 0;
}
} // namespace ctor::clobber
// -- END: test/std/utilities/optional/optional.object/optional.object.ctor/empty_in_place_t_does_not_clobber.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.ctor/explicit_const_optional_U.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class U>
//   explicit optional(const optional<U>& rhs);

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace ctor::explicit_const_optional_U {
using std::optional;

template <class T, class U>
TEST_CONSTEXPR_CXX20 void
test(const optional<U>& rhs, bool is_going_to_throw = false)
{
    static_assert(!(std::is_convertible<const optional<U>&, optional<T>>::value), "");
    bool rhs_engaged = static_cast<bool>(rhs);
#ifndef TEST_HAS_NO_EXCEPTIONS
    try
    {
        optional<T> lhs(rhs);
        assert(is_going_to_throw == false);
        assert(static_cast<bool>(lhs) == rhs_engaged);
        if (rhs_engaged)
            assert(*lhs == T(*rhs));
    }
    catch (int i)
    {
        assert(i == 6);
    }
#else
    if (is_going_to_throw) return;
    optional<T> lhs(rhs);
    assert(static_cast<bool>(lhs) == rhs_engaged);
    if (rhs_engaged)
        assert(*lhs == T(*rhs));
#endif
}

class X
{
    int i_;
public:
    constexpr explicit X(int i) : i_(i) {}
    constexpr X(const X& x) : i_(x.i_) {}
    TEST_CONSTEXPR_CXX20 ~X() {i_ = 0;}
    friend constexpr bool operator==(const X& x, const X& y) {return x.i_ == y.i_;}
};

class Y
{
    int i_;
public:
    constexpr explicit Y(int i) : i_(i) {}

    friend constexpr bool operator==(const Y& x, const Y& y) {return x.i_ == y.i_;}
};

int count = 0;

class Z
{
    int i_;
public:
    explicit Z(int i) : i_(i) {TEST_THROW(6);}

    friend bool operator==(const Z& x, const Z& y) {return x.i_ == y.i_;}
};

template<class T, class U>
constexpr bool test_all()
{
  {
    optional<U> rhs;
    test<T>(rhs);
  }
  {
    optional<U> rhs(3);
    test<T>(rhs);
  }
  return true;
}


int run_test()
{
    test_all<X, int>();
    test_all<Y, int>();
#if TEST_STD_VER > 17
    static_assert(test_all<X, int>());
    static_assert(test_all<Y, int>());
#endif
    {
        typedef Z T;
        typedef int U;
        optional<U> rhs;
        test<T>(rhs);
    }
    {
        typedef Z T;
        typedef int U;
        optional<U> rhs(3);
        test<T>(rhs, true);
    }

  return 0;
}
} // namespace ctor::explicit_const_optional_U
// -- END: test/std/utilities/optional/optional.object/optional.object.ctor/explicit_const_optional_U.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.ctor/explicit_optional_U.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class U>
//   explicit optional(optional<U>&& rhs);

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace ctor::explicit_optional_U {
using std::optional;

template <class T, class U>
TEST_CONSTEXPR_CXX20 void test(optional<U>&& rhs, bool is_going_to_throw = false)
{
    static_assert(!(std::is_convertible<optional<U>&&, optional<T>>::value), "");
    bool rhs_engaged = static_cast<bool>(rhs);
#ifndef TEST_HAS_NO_EXCEPTIONS
    try
    {
        optional<T> lhs(std::move(rhs));
        assert(is_going_to_throw == false);
        assert(static_cast<bool>(lhs) == rhs_engaged);
    }
    catch (int i)
    {
        assert(i == 6);
    }
#else
    if (is_going_to_throw) return;
    optional<T> lhs(std::move(rhs));
    assert(static_cast<bool>(lhs) == rhs_engaged);
#endif
}

class X
{
    int i_;
public:
    constexpr explicit X(int i) : i_(i) {}
    constexpr X(X&& x) : i_(x.i_) { x.i_ = 0; }
    TEST_CONSTEXPR_CXX20 ~X() {i_ = 0;}
    friend constexpr bool operator==(const X& x, const X& y) {return x.i_ == y.i_;}
};

int count = 0;

class Z
{
public:
    explicit Z(int) { TEST_THROW(6); }
};

TEST_CONSTEXPR_CXX20 bool test()
{
    {
        optional<int> rhs;
        test<X>(std::move(rhs));
    }
    {
        optional<int> rhs(3);
        test<X>(std::move(rhs));
    }

    return true;
}

int run_test()
{
#if TEST_STD_VER > 17
    static_assert(test());
#endif
    test();
    {
        optional<int> rhs;
        test<Z>(std::move(rhs));
    }
    {
        optional<int> rhs(3);
        test<Z>(std::move(rhs), true);
    }

  return 0;
}
} // namespace ctor::explicit_optional_U
// -- END: test/std/utilities/optional/optional.object/optional.object.ctor/explicit_optional_U.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.ctor/initializer_list.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class U, class... Args>
//     constexpr
//     explicit optional(in_place_t, initializer_list<U> il, Args&&... args);

#include <optional>
#include <type_traits>
#include <memory>
#include <vector>
#include <cassert>

#include "test_macros.h"

namespace ctor::initializer_list {
using std::optional;
using std::in_place_t;
using std::in_place;

class X
{
    int i_;
    int j_ = 0;
public:
    X() : i_(0) {}
    X(int i) : i_(i) {}
    X(int i, int j) : i_(i), j_(j) {}

    ~X() {}

    friend bool operator==(const X& x, const X& y)
        {return x.i_ == y.i_ && x.j_ == y.j_;}
};

class Y
{
    int i_;
    int j_ = 0;
public:
    constexpr Y() : i_(0) {}
    constexpr Y(int i) : i_(i) {}
    constexpr Y(std::initializer_list<int> il) : i_(il.begin()[0]), j_(il.begin()[1]) {}

    friend constexpr bool operator==(const Y& x, const Y& y)
        {return x.i_ == y.i_ && x.j_ == y.j_;}
};

class Z
{
    int i_;
    int j_ = 0;
public:
    Z() : i_(0) {}
    Z(int i) : i_(i) {}
    Z(std::initializer_list<int> il) : i_(il.begin()[0]), j_(il.begin()[1])
        {TEST_THROW(6);}

    friend bool operator==(const Z& x, const Z& y)
        {return x.i_ == y.i_ && x.j_ == y.j_;}
};

int run_test()
{
    {
        static_assert(!std::is_constructible<X, std::initializer_list<int>&>::value, "");
        static_assert(!std::is_constructible<optional<X>, std::initializer_list<int>&>::value, "");
    }
    {
        optional<std::vector<int>> opt(in_place, {3, 1});
        assert(static_cast<bool>(opt) == true);
        assert((*opt == std::vector<int>{3, 1}));
        assert(opt->size() == 2);
    }
    {
        optional<std::vector<int>> opt(in_place, {3, 1}, std::allocator<int>());
        assert(static_cast<bool>(opt) == true);
        assert((*opt == std::vector<int>{3, 1}));
        assert(opt->size() == 2);
    }
    {
        static_assert(std::is_constructible<optional<Y>, std::initializer_list<int>&>::value, "");
        constexpr optional<Y> opt(in_place, {3, 1});
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == Y{3, 1}, "");

        struct test_constexpr_ctor
            : public optional<Y>
        {
            constexpr test_constexpr_ctor(in_place_t, std::initializer_list<int> i)
                : optional<Y>(in_place, i) {}
        };

    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        static_assert(std::is_constructible<optional<Z>, std::initializer_list<int>&>::value, "");
        try
        {
            optional<Z> opt(in_place, {3, 1});
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
        }
    }
#endif

  return 0;
}
} // namespace ctor::initializer_list
// -- END: test/std/utilities/optional/optional.object/optional.object.ctor/initializer_list.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.ctor/in_place_t.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//


// <optional>

// template <class... Args>
//   constexpr explicit optional(in_place_t, Args&&... args);

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace ctor::in_place {
using std::optional;
using std::in_place_t;
using std::in_place;

class X
{
    int i_;
    int j_ = 0;
public:
    X() : i_(0) {}
    X(int i) : i_(i) {}
    X(int i, int j) : i_(i), j_(j) {}

    ~X() {}

    friend bool operator==(const X& x, const X& y)
        {return x.i_ == y.i_ && x.j_ == y.j_;}
};

class Y
{
    int i_;
    int j_ = 0;
public:
    constexpr Y() : i_(0) {}
    constexpr Y(int i) : i_(i) {}
    constexpr Y(int i, int j) : i_(i), j_(j) {}

    friend constexpr bool operator==(const Y& x, const Y& y)
        {return x.i_ == y.i_ && x.j_ == y.j_;}
};

class Z
{
public:
    Z(int) {TEST_THROW(6);}
};


int run_test()
{
    {
        constexpr optional<int> opt(in_place, 5);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == 5, "");

        struct test_constexpr_ctor
            : public optional<int>
        {
            constexpr test_constexpr_ctor(in_place_t, int i)
                : optional<int>(in_place, i) {}
        };

    }
    {
        optional<const int> opt(in_place, 5);
        assert(*opt == 5);
    }
    {
        const optional<X> opt(in_place);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == X());
    }
    {
        const optional<X> opt(in_place, 5);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == X(5));
    }
    {
        const optional<X> opt(in_place, 5, 4);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == X(5, 4));
    }
    {
        constexpr optional<Y> opt(in_place);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == Y(), "");

        struct test_constexpr_ctor
            : public optional<Y>
        {
            constexpr test_constexpr_ctor(in_place_t)
                : optional<Y>(in_place) {}
        };

    }
    {
        constexpr optional<Y> opt(in_place, 5);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == Y(5), "");

        struct test_constexpr_ctor
            : public optional<Y>
        {
            constexpr test_constexpr_ctor(in_place_t, int i)
                : optional<Y>(in_place, i) {}
        };

    }
    {
        constexpr optional<Y> opt(in_place, 5, 4);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == Y(5, 4), "");

        struct test_constexpr_ctor
            : public optional<Y>
        {
            constexpr test_constexpr_ctor(in_place_t, int i, int j)
                : optional<Y>(in_place, i, j) {}
        };

    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        try
        {
            const optional<Z> opt(in_place, 1);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
        }
    }
#endif

  return 0;
}
} // namespace ctor::in_place
// -- END: test/std/utilities/optional/optional.object/optional.object.ctor/in_place_t.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.ctor/move.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>

// constexpr optional(optional<T>&& rhs);

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"

namespace ctor::move {
using std::optional;

template <class T, class ...InitArgs>
void test(InitArgs&&... args)
{
    const optional<T> orig(std::forward<InitArgs>(args)...);
    optional<T> rhs(orig);
    bool rhs_engaged = static_cast<bool>(rhs);
    optional<T> lhs = std::move(rhs);
    assert(static_cast<bool>(lhs) == rhs_engaged);
    if (rhs_engaged)
        assert(*lhs == *orig);
}

template <class T, class ...InitArgs>
constexpr bool constexpr_test(InitArgs&&... args)
{
    static_assert( std::is_trivially_copy_constructible_v<T>, ""); // requirement
    const optional<T> orig(std::forward<InitArgs>(args)...);
    optional<T> rhs(orig);
    optional<T> lhs = std::move(rhs);
    return (lhs.has_value() == orig.has_value()) &&
           (lhs.has_value() ? *lhs == *orig : true);
}

void test_throwing_ctor() {
#ifndef TEST_HAS_NO_EXCEPTIONS
    struct Z {
        Z() : count(0) {}
        Z(Z&& o) : count(o.count + 1)
        { if (count == 2) throw 6; }
        int count;
    };
    Z z;
    optional<Z> rhs(std::move(z));
    try
    {
        optional<Z> lhs(std::move(rhs));
        assert(false);
    }
    catch (int i)
    {
        assert(i == 6);
    }
#endif
}


template <class T, class ...InitArgs>
void test_ref(InitArgs&&... args)
{
    optional<T> rhs(std::forward<InitArgs>(args)...);
    bool rhs_engaged = static_cast<bool>(rhs);
    optional<T> lhs = std::move(rhs);
    assert(static_cast<bool>(lhs) == rhs_engaged);
    if (rhs_engaged)
        assert(&(*lhs) == &(*rhs));
}

void test_reference_extension()
{
#if defined(_LIBCPP_VERSION) && 0 // TODO these extensions are currently disabled.
    using T = TestTypes::TestType;
    T::reset();
    {
        T t;
        T::reset_constructors();
        test_ref<T&>();
        test_ref<T&>(t);
        assert(T::alive == 1);
        assert(T::constructed == 0);
        assert(T::assigned == 0);
        assert(T::destroyed == 0);
    }
    assert(T::destroyed == 1);
    assert(T::alive == 0);
    {
        T t;
        const T& ct = t;
        T::reset_constructors();
        test_ref<T const&>();
        test_ref<T const&>(t);
        test_ref<T const&>(ct);
        assert(T::alive == 1);
        assert(T::constructed == 0);
        assert(T::assigned == 0);
        assert(T::destroyed == 0);
    }
    assert(T::alive == 0);
    assert(T::destroyed == 1);
    {
        T t;
        T::reset_constructors();
        test_ref<T&&>();
        test_ref<T&&>(std::move(t));
        assert(T::alive == 1);
        assert(T::constructed == 0);
        assert(T::assigned == 0);
        assert(T::destroyed == 0);
    }
    assert(T::alive == 0);
    assert(T::destroyed == 1);
    {
        T t;
        const T& ct = t;
        T::reset_constructors();
        test_ref<T const&&>();
        test_ref<T const&&>(std::move(t));
        test_ref<T const&&>(std::move(ct));
        assert(T::alive == 1);
        assert(T::constructed == 0);
        assert(T::assigned == 0);
        assert(T::destroyed == 0);
    }
    assert(T::alive == 0);
    assert(T::destroyed == 1);
    {
        static_assert(!std::is_copy_constructible<std::optional<T&&>>::value, "");
        static_assert(!std::is_copy_constructible<std::optional<T const&&>>::value, "");
    }
#endif
}


int run_test()
{
    test<int>();
    test<int>(3);
    static_assert(constexpr_test<int>(), "" );
    static_assert(constexpr_test<int>(3), "" );

    {
        optional<const int> o(42);
        optional<const int> o2(std::move(o));
        assert(*o2 == 42);
    }
    {
        using T = TestTypes::TestType;
        T::reset();
        optional<T> rhs;
        assert(T::alive == 0);
        const optional<T> lhs(std::move(rhs));
        assert(lhs.has_value() == false);
        assert(rhs.has_value() == false);
        assert(T::alive == 0);
    }
    TestTypes::TestType::reset();
    {
        using T = TestTypes::TestType;
        T::reset();
        optional<T> rhs(42);
        assert(T::alive == 1);
        assert(T::value_constructed == 1);
        assert(T::move_constructed == 0);
        const optional<T> lhs(std::move(rhs));
        assert(lhs.has_value());
        assert(rhs.has_value());
        assert(lhs.value().value == 42);
        assert(rhs.value().value == -1);
        assert(T::move_constructed == 1);
        assert(T::alive == 2);
    }
    TestTypes::TestType::reset();
    {
        using namespace ConstexprTestTypes;
        test<TestType>();
        test<TestType>(42);
    }
    {
        using namespace TrivialTestTypes;
        test<TestType>();
        test<TestType>(42);
    }
    {
        test_throwing_ctor();
    }
    {
        struct ThrowsMove {
          ThrowsMove() noexcept(false) {}
          ThrowsMove(ThrowsMove const&) noexcept(false) {}
          ThrowsMove(ThrowsMove &&) noexcept(false) {}
        };
        static_assert(!std::is_nothrow_move_constructible<optional<ThrowsMove>>::value, "");
        struct NoThrowMove {
          NoThrowMove() noexcept(false) {}
          NoThrowMove(NoThrowMove const&) noexcept(false) {}
          NoThrowMove(NoThrowMove &&) noexcept(true) {}
        };
        static_assert(std::is_nothrow_move_constructible<optional<NoThrowMove>>::value, "");
    }
    {
        test_reference_extension();
    }
    {
    constexpr std::optional<int> o1{4};
    constexpr std::optional<int> o2 = std::move(o1);
    static_assert( *o2 == 4, "" );
    }

  return 0;
}
} // namespace ctor::move
// -- END: test/std/utilities/optional/optional.object/optional.object.ctor/move.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.ctor/nullopt_t.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// constexpr optional(nullopt_t) noexcept;

#include <optional>
#include <type_traits>
#include <cassert>

#include "archetypes.h"

#include "test_macros.h"

namespace ctor::nullopt {
using std::optional;
using std::nullopt_t;
using std::nullopt;

template <class Opt>
void
test_constexpr()
{
    static_assert(std::is_nothrow_constructible<Opt, nullopt_t&>::value, "");
    static_assert(std::is_trivially_destructible<Opt>::value, "");
    static_assert(std::is_trivially_destructible<typename Opt::value_type>::value, "");

    constexpr Opt opt(nullopt);
    static_assert(static_cast<bool>(opt) == false, "");

    struct test_constexpr_ctor
        : public Opt
    {
        constexpr test_constexpr_ctor() {}
    };
}

template <class Opt>
void
test()
{
    static_assert(std::is_nothrow_constructible<Opt, nullopt_t&>::value, "");
    static_assert(!std::is_trivially_destructible<Opt>::value, "");
    static_assert(!std::is_trivially_destructible<typename Opt::value_type>::value, "");
    {
    Opt opt(nullopt);
    assert(static_cast<bool>(opt) == false);
    }
    {
    const Opt opt(nullopt);
    assert(static_cast<bool>(opt) == false);
    }
    struct test_constexpr_ctor
        : public Opt
    {
        constexpr test_constexpr_ctor() {}
    };
}

int run_test()
{
    test_constexpr<optional<int>>();
    test_constexpr<optional<int*>>();
    test_constexpr<optional<ImplicitTypes::NoCtors>>();
    test_constexpr<optional<NonTrivialTypes::NoCtors>>();
    test_constexpr<optional<NonConstexprTypes::NoCtors>>();
    test<optional<NonLiteralTypes::NoCtors>>();

  return 0;
}
} // namespace ctor::nullopt
// -- END: test/std/utilities/optional/optional.object/optional.object.ctor/nullopt_t.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.ctor/optional_U.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class U>
//   optional(optional<U>&& rhs);

#include <cassert>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

#include "test_macros.h"

namespace ctor::optional_U {
using std::optional;

template <class T, class U>
TEST_CONSTEXPR_CXX20 void
test(optional<U>&& rhs, bool is_going_to_throw = false)
{
    bool rhs_engaged = static_cast<bool>(rhs);
#ifndef TEST_HAS_NO_EXCEPTIONS
    try
    {
        optional<T> lhs = std::move(rhs);
        assert(is_going_to_throw == false);
        assert(static_cast<bool>(lhs) == rhs_engaged);
    }
    catch (int i)
    {
        assert(i == 6);
    }
#else
    if (is_going_to_throw) return;
    optional<T> lhs = std::move(rhs);
    assert(static_cast<bool>(lhs) == rhs_engaged);
#endif
}

class X
{
    int i_;
public:
    TEST_CONSTEXPR_CXX20 X(int i) : i_(i) {}
    TEST_CONSTEXPR_CXX20 X(X&& x) : i_(std::exchange(x.i_, 0)) {}
    TEST_CONSTEXPR_CXX20 ~X() {i_ = 0;}
    friend constexpr bool operator==(const X& x, const X& y) {return x.i_ == y.i_;}
};

struct Z
{
    Z(int) { TEST_THROW(6); }
};

template<class T, class U>
TEST_CONSTEXPR_CXX20 bool test_all()
{
    {
        optional<T> rhs;
        test<U>(std::move(rhs));
    }
    {
        optional<T> rhs(short{3});
        test<U>(std::move(rhs));
    }
    return true;
}

int run_test()
{
    test_all<short, int>();
    test_all<int, X>();
#if TEST_STD_VER > 17
    static_assert(test_all<short, int>());
    static_assert(test_all<int, X>());
#endif
    {
        optional<int> rhs;
        test<Z>(std::move(rhs));
    }
    {
        optional<int> rhs(3);
        test<Z>(std::move(rhs), true);
    }

    static_assert(!(std::is_constructible<optional<X>, optional<Z>>::value), "");

  return 0;
}
} // namespace ctor::optional_U
// -- END: test/std/utilities/optional/optional.object/optional.object.ctor/optional_U.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.ctor/rvalue_T.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//


// <optional>

// constexpr optional(T&& v);

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"


namespace ctor::rvalue_T {
using std::optional;


class Z
{
public:
    Z(int) {}
    Z(Z&&) {TEST_THROW(6);}
};


int run_test()
{
    {
        typedef int T;
        constexpr optional<T> opt(T(5));
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == 5, "");

        struct test_constexpr_ctor
            : public optional<T>
        {
            constexpr test_constexpr_ctor(T&&) {}
        };
    }
    {
        typedef double T;
        constexpr optional<T> opt(T(3));
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == 3, "");

        struct test_constexpr_ctor
            : public optional<T>
        {
            constexpr test_constexpr_ctor(T&&) {}
        };
    }
    {
        const int x = 42;
        optional<const int> o(std::move(x));
        assert(*o == 42);
    }
    {
        typedef TestTypes::TestType T;
        T::reset();
        optional<T> opt = T{3};
        assert(T::alive == 1);
        assert(T::move_constructed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(opt.value().value == 3);
    }
    {
        typedef ExplicitTestTypes::TestType T;
        static_assert(!std::is_convertible<T&&, optional<T>>::value, "");
        T::reset();
        optional<T> opt(T{3});
        assert(T::alive == 1);
        assert(T::move_constructed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(opt.value().value == 3);
    }
    {
        typedef TestTypes::TestType T;
        T::reset();
        optional<T> opt = {3};
        assert(T::alive == 1);
        assert(T::value_constructed == 1);
        assert(T::copy_constructed == 0);
        assert(T::move_constructed == 0);
        assert(static_cast<bool>(opt) == true);
        assert(opt.value().value == 3);
    }
    {
        typedef ConstexprTestTypes::TestType T;
        constexpr optional<T> opt = {T(3)};
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(opt.value().value == 3, "");

        struct test_constexpr_ctor
            : public optional<T>
        {
            constexpr test_constexpr_ctor(const T&) {}
        };
    }
    {
        typedef ConstexprTestTypes::TestType T;
        constexpr optional<T> opt = {3};
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(opt.value().value == 3, "");

        struct test_constexpr_ctor
            : public optional<T>
        {
            constexpr test_constexpr_ctor(const T&) {}
        };
    }
    {
        typedef ExplicitConstexprTestTypes::TestType T;
        static_assert(!std::is_convertible<T&&, optional<T>>::value, "");
        constexpr optional<T> opt(T{3});
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(opt.value().value == 3, "");

        struct test_constexpr_ctor
            : public optional<T>
        {
            constexpr test_constexpr_ctor(T&&) {}
        };

    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        try
        {
            Z z(3);
            optional<Z> opt(std::move(z));
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
        }
    }
#endif

  return 0;
}
} // namespace ctor::rvalue_T
// -- END: test/std/utilities/optional/optional.object/optional.object.ctor/rvalue_T.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.ctor/U.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//


// <optional>

// template <class U>
//   constexpr EXPLICIT optional(U&& u);

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"
#include "test_convertible.h"


namespace ctor::U {
using std::optional;

struct ImplicitThrow
{
    constexpr ImplicitThrow(int x) { if (x != -1) TEST_THROW(6);}
};

struct ExplicitThrow
{
    constexpr explicit ExplicitThrow(int x) { if (x != -1) TEST_THROW(6);}
};

struct ImplicitAny {
  template <class U>
  constexpr ImplicitAny(U&&) {}
};


template <class To, class From>
constexpr bool implicit_conversion(optional<To>&& opt, const From& v)
{
    using O = optional<To>;
    static_assert(test_convertible<O, From>(), "");
    static_assert(!test_convertible<O, void*>(), "");
    static_assert(!test_convertible<O, From, int>(), "");
    return opt && *opt == static_cast<To>(v);
}

template <class To, class Input, class Expect>
constexpr bool explicit_conversion(Input&& in, const Expect& v)
{
    using O = optional<To>;
    static_assert(std::is_constructible<O, Input>::value, "");
    static_assert(!std::is_convertible<Input, O>::value, "");
    static_assert(!std::is_constructible<O, void*>::value, "");
    static_assert(!std::is_constructible<O, Input, int>::value, "");
    optional<To> opt(std::forward<Input>(in));
    return opt && *opt == static_cast<To>(v);
}

void test_implicit()
{
    {
        static_assert(implicit_conversion<long long>(42, 42), "");
    }
    {
        static_assert(implicit_conversion<long double>(3.14, 3.14), "");
    }
    {
        int x = 42;
        optional<void* const> o(&x);
        assert(*o == &x);
    }
    {
        using T = TrivialTestTypes::TestType;
        static_assert(implicit_conversion<T>(42, 42), "");
    }
    {
        using T = TestTypes::TestType;
        assert(implicit_conversion<T>(3, T(3)));
    }
  {
    using O = optional<ImplicitAny>;
    static_assert(!test_convertible<O, std::in_place_t>(), "");
    static_assert(!test_convertible<O, std::in_place_t&>(), "");
    static_assert(!test_convertible<O, const std::in_place_t&>(), "");
    static_assert(!test_convertible<O, std::in_place_t&&>(), "");
    static_assert(!test_convertible<O, const std::in_place_t&&>(), "");

  }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        try {
            using T = ImplicitThrow;
            optional<T> t = 42;
            assert(false);
            ((void)t);
        } catch (int) {
        }
    }
#endif
}

void test_explicit() {
    {
        using T = ExplicitTrivialTestTypes::TestType;
        static_assert(explicit_conversion<T>(42, 42), "");
    }
    {
        using T = ExplicitConstexprTestTypes::TestType;
        static_assert(explicit_conversion<T>(42, 42), "");
        static_assert(!std::is_convertible<int, T>::value, "");
    }
    {
        using T = ExplicitTestTypes::TestType;
        T::reset();
        {
            assert(explicit_conversion<T>(42, 42));
            assert(T::alive == 0);
        }
        T::reset();
        {
            optional<T> t(42);
            assert(T::alive == 1);
            assert(T::value_constructed == 1);
            assert(T::move_constructed == 0);
            assert(T::copy_constructed == 0);
            assert(t.value().value == 42);
        }
        assert(T::alive == 0);
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        try {
            using T = ExplicitThrow;
            optional<T> t(42);
            assert(false);
        } catch (int) {
        }
    }
#endif
}

int run_test() {
    test_implicit();
    test_explicit();

  return 0;
}
} // namespace ctor::U
// -- END: test/std/utilities/optional/optional.object/optional.object.ctor/U.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.dtor/dtor.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// ~optional();

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace dtor {
using std::optional;

struct PODType {
  int value;
  int value2;
};

class X
{
public:
    static bool dtor_called;
    X() = default;
    X(const X&) = default;
    X& operator=(const X&) = default;
    ~X() {dtor_called = true;}
};

bool X::dtor_called = false;

int run_test()
{
    {
        typedef int T;
        static_assert(std::is_trivially_destructible<T>::value, "");
        static_assert(std::is_trivially_destructible<optional<T>>::value, "");
    }
    {
        typedef double T;
        static_assert(std::is_trivially_destructible<T>::value, "");
        static_assert(std::is_trivially_destructible<optional<T>>::value, "");
    }
    {
        typedef PODType T;
        static_assert(std::is_trivially_destructible<T>::value, "");
        static_assert(std::is_trivially_destructible<optional<T>>::value, "");
    }
    {
        typedef X T;
        static_assert(!std::is_trivially_destructible<T>::value, "");
        static_assert(!std::is_trivially_destructible<optional<T>>::value, "");
        {
            X x;
            optional<X> opt{x};
            assert(X::dtor_called == false);
        }
        assert(X::dtor_called == true);
    }

  return 0;
}
} // namespace dtor
// -- END: test/std/utilities/optional/optional.object/optional.object.dtor/dtor.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.mod/reset.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>

// void reset() noexcept;

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace modifiers::reset {
using std::optional;

struct X
{
    static bool dtor_called;
    X() = default;
    X(const X&) = default;
    X& operator=(const X&) = default;
    ~X() {dtor_called = true;}
};

bool X::dtor_called = false;

TEST_CONSTEXPR_CXX20 bool check_reset() {
  {
    optional<int> opt;
    static_assert(noexcept(opt.reset()) == true, "");
    opt.reset();
    assert(static_cast<bool>(opt) == false);
  }
  {
    optional<int> opt(3);
    opt.reset();
    assert(static_cast<bool>(opt) == false);
  }
  return true;
}

int run_test()
{
    check_reset();
#if TEST_STD_VER >= 20
    static_assert(check_reset());
#endif
    {
        optional<X> opt;
        static_assert(noexcept(opt.reset()) == true, "");
        assert(X::dtor_called == false);
        opt.reset();
        assert(X::dtor_called == false);
        assert(static_cast<bool>(opt) == false);
    }
    {
        optional<X> opt(X{});
        X::dtor_called = false;
        opt.reset();
        assert(X::dtor_called == true);
        assert(static_cast<bool>(opt) == false);
        X::dtor_called = false;
    }

  return 0;
}
} // namespace modifiers::reset
// -- END: test/std/utilities/optional/optional.object/optional.object.mod/reset.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.observe/bool.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// constexpr explicit optional<T>::operator bool() const noexcept;

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace observe::op_bool {
int run_test()
{
    using std::optional;
    {
        const optional<int> opt; ((void)opt);
        ASSERT_NOEXCEPT(bool(opt));
        static_assert(!std::is_convertible<optional<int>, bool>::value, "");
    }
    {
        constexpr optional<int> opt;
        static_assert(!opt, "");
    }
    {
        constexpr optional<int> opt(0);
        static_assert(opt, "");
    }

  return 0;
}
} // namespace observe::op_bool
// -- END: test/std/utilities/optional/optional.object/optional.object.observe/bool.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.observe/dereference.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// constexpr T& optional<T>::operator*() &;

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace observe::deref {
using std::optional;

struct X
{
    constexpr int test() const& {return 3;}
    int test() & {return 4;}
    constexpr int test() const&& {return 5;}
    int test() && {return 6;}
};

struct Y
{
    constexpr int test() {return 7;}
};

constexpr int
test()
{
    optional<Y> opt{Y{}};
    return (*opt).test();
}

int run_test()
{
    {
        optional<X> opt; ((void)opt);
        ASSERT_SAME_TYPE(decltype(*opt), X&);
        LIBCPP_STATIC_ASSERT(noexcept(*opt));
        // ASSERT_NOT_NOEXCEPT(*opt);
        // TODO: This assertion fails with GCC because it can see that
        // (A) operator*() is constexpr, and
        // (B) there is no path through the function that throws.
        // It's arguable if this is the correct behavior for the noexcept
        // operator.
        // Regardless this function should still be noexcept(false) because
        // it has a narrow contract.
    }
    {
        optional<X> opt(X{});
        assert((*opt).test() == 4);
    }
    static_assert(test() == 7, "");

    return 0;
}
} // namespace observe::deref
// -- END: test/std/utilities/optional/optional.object/optional.object.observe/dereference.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.observe/dereference_const.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// constexpr const T& optional<T>::operator*() const &;

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace observe::deref_const {
using std::optional;

struct X
{
    constexpr int test() const& {return 3;}
    int test() & {return 4;}
    constexpr int test() const&& {return 5;}
    int test() && {return 6;}
};

struct Y
{
    int test() const {return 2;}
};

int run_test()
{
    {
        const optional<X> opt; ((void)opt);
        ASSERT_SAME_TYPE(decltype(*opt), X const&);
        LIBCPP_STATIC_ASSERT(noexcept(*opt));
        // ASSERT_NOT_NOEXCEPT(*opt);
        // TODO: This assertion fails with GCC because it can see that
        // (A) operator*() is constexpr, and
        // (B) there is no path through the function that throws.
        // It's arguable if this is the correct behavior for the noexcept
        // operator.
        // Regardless this function should still be noexcept(false) because
        // it has a narrow contract.
    }
    {
        constexpr optional<X> opt(X{});
        static_assert((*opt).test() == 3, "");
    }
    {
        constexpr optional<Y> opt(Y{});
        assert((*opt).test() == 2);
    }

    return 0;
}
} // namespace observe::deref_const
// -- END: test/std/utilities/optional/optional.object/optional.object.observe/dereference_const.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.observe/dereference_const_rvalue.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// constexpr T&& optional<T>::operator*() const &&;

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace observe::deref_const_rvalue {
using std::optional;

struct X
{
    constexpr int test() const& {return 3;}
    int test() & {return 4;}
    constexpr int test() const&& {return 5;}
    int test() && {return 6;}
};

struct Y
{
    int test() const && {return 2;}
};

int run_test()
{
    {
        const optional<X> opt; ((void)opt);
        ASSERT_SAME_TYPE(decltype(*std::move(opt)), X const &&);
        LIBCPP_STATIC_ASSERT(noexcept(*opt));
        // ASSERT_NOT_NOEXCEPT(*std::move(opt));
        // TODO: This assertion fails with GCC because it can see that
        // (A) operator*() is constexpr, and
        // (B) there is no path through the function that throws.
        // It's arguable if this is the correct behavior for the noexcept
        // operator.
        // Regardless this function should still be noexcept(false) because
        // it has a narrow contract.
    }
    {
        constexpr optional<X> opt(X{});
        static_assert((*std::move(opt)).test() == 5, "");
    }
    {
        constexpr optional<Y> opt(Y{});
        assert((*std::move(opt)).test() == 2);
    }

    return 0;
}
} // namespace observe::deref_const_rvalue
// -- END: test/std/utilities/optional/optional.object/optional.object.observe/dereference_const_rvalue.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.observe/dereference_rvalue.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// constexpr T&& optional<T>::operator*() &&;

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace observe::deref_rvalue {
using std::optional;

struct X
{
    constexpr int test() const& {return 3;}
    int test() & {return 4;}
    constexpr int test() const&& {return 5;}
    int test() && {return 6;}
};

struct Y
{
    constexpr int test() && {return 7;}
};

constexpr int
test()
{
    optional<Y> opt{Y{}};
    return (*std::move(opt)).test();
}

int run_test()
{
    {
        optional<X> opt; ((void)opt);
        ASSERT_SAME_TYPE(decltype(*std::move(opt)), X&&);
        LIBCPP_STATIC_ASSERT(noexcept(*opt));
        // ASSERT_NOT_NOEXCEPT(*std::move(opt));
        // TODO: This assertion fails with GCC because it can see that
        // (A) operator*() is constexpr, and
        // (B) there is no path through the function that throws.
        // It's arguable if this is the correct behavior for the noexcept
        // operator.
        // Regardless this function should still be noexcept(false) because
        // it has a narrow contract.
    }
    {
        optional<X> opt(X{});
        assert((*std::move(opt)).test() == 6);
    }
    static_assert(test() == 7, "");

    return 0;
}
} // namespace observe::deref_rvalue
// -- END: test/std/utilities/optional/optional.object/optional.object.observe/dereference_rvalue.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.observe/has_value.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// constexpr bool optional<T>::has_value() const noexcept;

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace observe::has_value {
int run_test()
{
    using std::optional;
    {
        const optional<int> opt; ((void)opt);
        ASSERT_NOEXCEPT(opt.has_value());
        ASSERT_SAME_TYPE(decltype(opt.has_value()), bool);
    }
    {
        constexpr optional<int> opt;
        static_assert(!opt.has_value(), "");
    }
    {
        constexpr optional<int> opt(0);
        static_assert(opt.has_value(), "");
    }

  return 0;
}
} // namespace observe::has_value
// -- END: test/std/utilities/optional/optional.object/optional.object.observe/has_value.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.observe/op_arrow.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// constexpr T* optional<T>::operator->();

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace observe::op_arrow {
using std::optional;

struct X
{
    int test() noexcept {return 3;}
};

struct Y
{
    constexpr int test() {return 3;}
};

constexpr int
test()
{
    optional<Y> opt{Y{}};
    return opt->test();
}

int run_test()
{
    {
        std::optional<X> opt; ((void)opt);
        ASSERT_SAME_TYPE(decltype(opt.operator->()), X*);
        // ASSERT_NOT_NOEXCEPT(opt.operator->());
        // TODO: This assertion fails with GCC because it can see that
        // (A) operator->() is constexpr, and
        // (B) there is no path through the function that throws.
        // It's arguable if this is the correct behavior for the noexcept
        // operator.
        // Regardless this function should still be noexcept(false) because
        // it has a narrow contract.
    }
    {
        optional<X> opt(X{});
        assert(opt->test() == 3);
    }
    {
        static_assert(test() == 3, "");
    }

    return 0;
}
} // namespace observe::op_arrow
// -- END: test/std/utilities/optional/optional.object/optional.object.observe/op_arrow.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.observe/op_arrow_const.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// constexpr const T* optional<T>::operator->() const;

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace observe::op_arrow_const {
using std::optional;

struct X
{
    constexpr int test() const {return 3;}
};

struct Y
{
    int test() const noexcept {return 2;}
};

struct Z
{
    const Z* operator&() const;
    constexpr int test() const {return 1;}
};

int run_test()
{
    {
        const std::optional<X> opt; ((void)opt);
        ASSERT_SAME_TYPE(decltype(opt.operator->()), X const*);
        // ASSERT_NOT_NOEXCEPT(opt.operator->());
        // TODO: This assertion fails with GCC because it can see that
        // (A) operator->() is constexpr, and
        // (B) there is no path through the function that throws.
        // It's arguable if this is the correct behavior for the noexcept
        // operator.
        // Regardless this function should still be noexcept(false) because
        // it has a narrow contract.
    }
    {
        constexpr optional<X> opt(X{});
        static_assert(opt->test() == 3, "");
    }
    {
        constexpr optional<Y> opt(Y{});
        assert(opt->test() == 2);
    }
    {
        constexpr optional<Z> opt(Z{});
        static_assert(opt->test() == 1, "");
    }

    return 0;
}
} // namespace observe::op_arrow_const
// -- END: test/std/utilities/optional/optional.object/optional.object.observe/op_arrow_const.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.observe/value.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>

// constexpr T& optional<T>::value() &;

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace observe::value {
using std::optional;
using std::bad_optional_access;

struct X
{
    X() = default;
    X(const X&) = delete;
    constexpr int test() const & {return 3;}
    int test() & {return 4;}
    constexpr int test() const && {return 5;}
    int test() && {return 6;}
};

struct Y
{
    constexpr int test() & {return 7;}
};

constexpr int
test()
{
    optional<Y> opt{Y{}};
    return opt.value().test();
}


int run_test()
{
    {
        optional<X> opt; ((void)opt);
        ASSERT_NOT_NOEXCEPT(opt.value());
        ASSERT_SAME_TYPE(decltype(opt.value()), X&);
    }
    {
        optional<X> opt;
        opt.emplace();
        assert(opt.value().test() == 4);
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        optional<X> opt;
        try
        {
            (void)opt.value();
            assert(false);
        }
        catch (const bad_optional_access&)
        {
        }
    }
#endif
    static_assert(test() == 7, "");

  return 0;
}
} // namespace observe::value
// -- END: test/std/utilities/optional/optional.object/optional.object.observe/value.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.observe/value_const.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>

// constexpr const T& optional<T>::value() const &;

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace observe::value_const {
using std::optional;
using std::in_place_t;
using std::in_place;
using std::bad_optional_access;

struct X
{
    X() = default;
    X(const X&) = delete;
    constexpr int test() const & {return 3;}
    int test() & {return 4;}
    constexpr int test() const && {return 5;}
    int test() && {return 6;}
};

int run_test()
{
    {
        const optional<X> opt; ((void)opt);
        ASSERT_NOT_NOEXCEPT(opt.value());
        ASSERT_SAME_TYPE(decltype(opt.value()), X const&);
    }
    {
        constexpr optional<X> opt(in_place);
        static_assert(opt.value().test() == 3, "");
    }
    {
        const optional<X> opt(in_place);
        assert(opt.value().test() == 3);
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        const optional<X> opt;
        try
        {
            (void)opt.value();
            assert(false);
        }
        catch (const bad_optional_access&)
        {
        }
    }
#endif

  return 0;
}
} // namespace observe::value_const
// -- END: test/std/utilities/optional/optional.object/optional.object.observe/value_const.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.observe/value_const_rvalue.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>

// constexpr const T& optional<T>::value() const &&;

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace observe::value_const_rvalue {
using std::optional;
using std::in_place_t;
using std::in_place;
using std::bad_optional_access;

struct X
{
    X() = default;
    X(const X&) = delete;
    constexpr int test() const & {return 3;}
    int test() & {return 4;}
    constexpr int test() const && {return 5;}
    int test() && {return 6;}
};

int run_test()
{
    {
        const optional<X> opt; ((void)opt);
        ASSERT_NOT_NOEXCEPT(std::move(opt).value());
        ASSERT_SAME_TYPE(decltype(std::move(opt).value()), X const&&);
    }
    {
        constexpr optional<X> opt(in_place);
        static_assert(std::move(opt).value().test() == 5, "");
    }
    {
        const optional<X> opt(in_place);
        assert(std::move(opt).value().test() == 5);
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        const optional<X> opt;
        try
        {
            (void)std::move(opt).value();
            assert(false);
        }
        catch (const bad_optional_access&)
        {
        }
    }
#endif

  return 0;
}
} // namespace observe::value_const_rvalue
// -- END: test/std/utilities/optional/optional.object/optional.object.observe/value_const_rvalue.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.observe/value_or.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class U> constexpr T optional<T>::value_or(U&& v) &&;

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace observe::value_or {
using std::optional;
using std::in_place_t;
using std::in_place;

struct Y
{
    int i_;

    constexpr Y(int i) : i_(i) {}
};

struct X
{
    int i_;

    constexpr X(int i) : i_(i) {}
    constexpr X(X&& x) : i_(x.i_) {x.i_ = 0;}
    constexpr X(const Y& y) : i_(y.i_) {}
    constexpr X(Y&& y) : i_(y.i_+1) {}
    friend constexpr bool operator==(const X& x, const X& y)
        {return x.i_ == y.i_;}
};

constexpr int test()
{
    {
        optional<X> opt(in_place, 2);
        Y y(3);
        assert(std::move(opt).value_or(y) == 2);
        assert(*opt == 0);
    }
    {
        optional<X> opt(in_place, 2);
        assert(std::move(opt).value_or(Y(3)) == 2);
        assert(*opt == 0);
    }
    {
        optional<X> opt;
        Y y(3);
        assert(std::move(opt).value_or(y) == 3);
        assert(!opt);
    }
    {
        optional<X> opt;
        assert(std::move(opt).value_or(Y(3)) == 4);
        assert(!opt);
    }
    return 0;
}

int run_test()
{
    static_assert(test() == 0);

  return 0;
}
} // namespace observe::value_or
// -- END: test/std/utilities/optional/optional.object/optional.object.observe/value_or.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.observe/value_or_const.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class U> constexpr T optional<T>::value_or(U&& v) const&;

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace observe::value_or_const {
using std::optional;

struct Y
{
    int i_;

    constexpr Y(int i) : i_(i) {}
};

struct X
{
    int i_;

    constexpr X(int i) : i_(i) {}
    constexpr X(const Y& y) : i_(y.i_) {}
    constexpr X(Y&& y) : i_(y.i_+1) {}
    friend constexpr bool operator==(const X& x, const X& y)
        {return x.i_ == y.i_;}
};

int run_test()
{
    {
        constexpr optional<X> opt(2);
        constexpr Y y(3);
        static_assert(opt.value_or(y) == 2, "");
    }
    {
        constexpr optional<X> opt(2);
        static_assert(opt.value_or(Y(3)) == 2, "");
    }
    {
        constexpr optional<X> opt;
        constexpr Y y(3);
        static_assert(opt.value_or(y) == 3, "");
    }
    {
        constexpr optional<X> opt;
        static_assert(opt.value_or(Y(3)) == 4, "");
    }
    {
        const optional<X> opt(2);
        const Y y(3);
        assert(opt.value_or(y) == 2);
    }
    {
        const optional<X> opt(2);
        assert(opt.value_or(Y(3)) == 2);
    }
    {
        const optional<X> opt;
        const Y y(3);
        assert(opt.value_or(y) == 3);
    }
    {
        const optional<X> opt;
        assert(opt.value_or(Y(3)) == 4);
    }

  return 0;
}
} // namespace observe::value_or_const
// -- END: test/std/utilities/optional/optional.object/optional.object.observe/value_or_const.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.observe/value_rvalue.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// constexpr T& optional<T>::value() &&;

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace observe::value_rvalue {
using std::optional;
using std::bad_optional_access;

struct X
{
    X() = default;
    X(const X&) = delete;
    constexpr int test() const & {return 3;}
    int test() & {return 4;}
    constexpr int test() const && {return 5;}
    int test() && {return 6;}
};

struct Y
{
    constexpr int test() && {return 7;}
};

constexpr int
test()
{
    optional<Y> opt{Y{}};
    return std::move(opt).value().test();
}

int run_test()
{
    {
        optional<X> opt; ((void)opt);
        ASSERT_NOT_NOEXCEPT(std::move(opt).value());
        ASSERT_SAME_TYPE(decltype(std::move(opt).value()), X&&);
    }
    {
        optional<X> opt;
        opt.emplace();
        assert(std::move(opt).value().test() == 6);
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        optional<X> opt;
        try
        {
            (void)std::move(opt).value();
            assert(false);
        }
        catch (const bad_optional_access&)
        {
        }
    }
#endif
    static_assert(test() == 7, "");

  return 0;
}
} // namespace observe::value_rvalue
// -- END: test/std/utilities/optional/optional.object/optional.object.observe/value_rvalue.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/optional.object.swap/swap.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// void swap(optional&)
//     noexcept(is_nothrow_move_constructible<T>::value &&
//              is_nothrow_swappable<T>::value)

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"

namespace member_swap {
using std::optional;

class X
{
    int i_;
public:
    static unsigned dtor_called;
    X(int i) : i_(i) {}
    X(X&& x) = default;
    X& operator=(X&&) = default;
    ~X() {++dtor_called;}

    friend bool operator==(const X& x, const X& y) {return x.i_ == y.i_;}
};

unsigned X::dtor_called = 0;

class Y
{
    int i_;
public:
    static unsigned dtor_called;
    Y(int i) : i_(i) {}
    Y(Y&&) = default;
    ~Y() {++dtor_called;}

    friend constexpr bool operator==(const Y& x, const Y& y) {return x.i_ == y.i_;}
    friend void swap(Y& x, Y& y) {std::swap(x.i_, y.i_);}
};

unsigned Y::dtor_called = 0;

class Z
{
    int i_;
public:
    Z(int i) : i_(i) {}
    Z(Z&&) {TEST_THROW(7);}

    friend constexpr bool operator==(const Z& x, const Z& y) {return x.i_ == y.i_;}
    friend void swap(Z&, Z&) {TEST_THROW(6);}
};

class W
{
    int i_;
public:
    constexpr W(int i) : i_(i) {}

    friend constexpr bool operator==(const W& x, const W& y) {return x.i_ == y.i_;}
    friend TEST_CONSTEXPR_CXX20 void swap(W& x, W& y) noexcept {std::swap(x.i_, y.i_);}
};

template<class T>
TEST_CONSTEXPR_CXX20 bool check_swap()
{
    {
        optional<T> opt1;
        optional<T> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == true);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        optional<T> opt1(1);
        optional<T> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == true);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        optional<T> opt1;
        optional<T> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        optional<T> opt1(1);
        optional<T> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    return true;
}

int run_test()
{
    check_swap<int>();
    check_swap<W>();
#if TEST_STD_VER > 17
    static_assert(check_swap<int>());
    static_assert(check_swap<W>());
#endif
    {
        optional<X> opt1;
        optional<X> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        assert(X::dtor_called == 0);
    }
    {
        optional<X> opt1(1);
        optional<X> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        X::dtor_called = 0;
        opt1.swap(opt2);
        assert(X::dtor_called == 1);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        optional<X> opt1;
        optional<X> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        X::dtor_called = 0;
        opt1.swap(opt2);
        assert(X::dtor_called == 1);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        optional<X> opt1(1);
        optional<X> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        X::dtor_called = 0;
        opt1.swap(opt2);
        assert(X::dtor_called == 1);  // from inside std::swap
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        optional<Y> opt1;
        optional<Y> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        assert(Y::dtor_called == 0);
    }
    {
        optional<Y> opt1(1);
        optional<Y> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        Y::dtor_called = 0;
        opt1.swap(opt2);
        assert(Y::dtor_called == 1);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        optional<Y> opt1;
        optional<Y> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        Y::dtor_called = 0;
        opt1.swap(opt2);
        assert(Y::dtor_called == 1);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        optional<Y> opt1(1);
        optional<Y> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        Y::dtor_called = 0;
        opt1.swap(opt2);
        assert(Y::dtor_called == 0);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        optional<Z> opt1;
        optional<Z> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        optional<Z> opt1;
        opt1.emplace(1);
        optional<Z> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        try
        {
            opt1.swap(opt2);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 7);
        }
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        optional<Z> opt1;
        optional<Z> opt2;
        opt2.emplace(2);
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        try
        {
            opt1.swap(opt2);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 7);
        }
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
    }
    {
        optional<Z> opt1;
        opt1.emplace(1);
        optional<Z> opt2;
        opt2.emplace(2);
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        try
        {
            opt1.swap(opt2);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
        }
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
    }
#endif

  return 0;
}
} // namespace member_swap
// -- END: test/std/utilities/optional/optional.object/optional.object.swap/swap.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/special_members.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>

// Make sure we properly generate special member functions for optional<T>
// based on the properties of T itself.

#include <optional>
#include <type_traits>

#include "archetypes.h"

#include "test_macros.h"


namespace special_members {
template <class T>
struct SpecialMemberTest {
    using O = std::optional<T>;

    static_assert(std::is_default_constructible_v<O>,
        "optional is always default constructible.");

    static_assert(std::is_copy_constructible_v<O> == std::is_copy_constructible_v<T>,
        "optional<T> is copy constructible if and only if T is copy constructible.");

    static_assert(std::is_move_constructible_v<O> ==
        (std::is_copy_constructible_v<T> || std::is_move_constructible_v<T>),
        "optional<T> is move constructible if and only if T is copy or move constructible.");

    static_assert(std::is_copy_assignable_v<O> ==
        (std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>),
        "optional<T> is copy assignable if and only if T is both copy "
        "constructible and copy assignable.");

    static_assert(std::is_move_assignable_v<O> ==
        ((std::is_move_constructible_v<T> && std::is_move_assignable_v<T>) ||
         (std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>)),
        "optional<T> is move assignable if and only if T is both move constructible and "
        "move assignable, or both copy constructible and copy assignable.");
};

template <class ...Args> static void sink(Args&&...) {}

template <class ...TestTypes>
struct DoTestsMetafunction {
    DoTestsMetafunction() { sink(SpecialMemberTest<TestTypes>{}...); }
};

int run_test() {
    sink(
        ImplicitTypes::ApplyTypes<DoTestsMetafunction>{},
        ExplicitTypes::ApplyTypes<DoTestsMetafunction>{},
        NonLiteralTypes::ApplyTypes<DoTestsMetafunction>{},
        NonTrivialTypes::ApplyTypes<DoTestsMetafunction>{}
    );
    return 0;
}
} // namespace special_members
// -- END: test/std/utilities/optional/optional.object/special_members.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/triviality.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>

// The following special member functions should propagate the triviality of
// the element held in the optional (see P0602R4):
//
// constexpr optional(const optional& rhs);
// constexpr optional(optional&& rhs) noexcept(see below);
// constexpr optional<T>& operator=(const optional& rhs);
// constexpr optional<T>& operator=(optional&& rhs) noexcept(see below);


#include <optional>
#include <type_traits>

#include "archetypes.h"

#include "test_macros.h"


namespace triviality {
constexpr bool implies(bool p, bool q) {
    return !p || q;
}

template <class T>
struct SpecialMemberTest {
    using O = std::optional<T>;

    static_assert(implies(std::is_trivially_copy_constructible_v<T>,
                          std::is_trivially_copy_constructible_v<O>),
        "optional<T> is trivially copy constructible if T is trivially copy constructible.");

    static_assert(implies(std::is_trivially_move_constructible_v<T>,
                          std::is_trivially_move_constructible_v<O>),
        "optional<T> is trivially move constructible if T is trivially move constructible");

    static_assert(implies(std::is_trivially_copy_constructible_v<T> &&
                          std::is_trivially_copy_assignable_v<T> &&
                          std::is_trivially_destructible_v<T>,

                          std::is_trivially_copy_assignable_v<O>),
        "optional<T> is trivially copy assignable if T is "
        "trivially copy constructible, "
        "trivially copy assignable, and "
        "trivially destructible");

    static_assert(implies(std::is_trivially_move_constructible_v<T> &&
                          std::is_trivially_move_assignable_v<T> &&
                          std::is_trivially_destructible_v<T>,

                          std::is_trivially_move_assignable_v<O>),
        "optional<T> is trivially move assignable if T is "
        "trivially move constructible, "
        "trivially move assignable, and"
        "trivially destructible.");
};

template <class ...Args> static void sink(Args&&...) {}

template <class ...TestTypes>
struct DoTestsMetafunction {
    DoTestsMetafunction() { sink(SpecialMemberTest<TestTypes>{}...); }
};

struct TrivialMoveNonTrivialCopy {
    TrivialMoveNonTrivialCopy() = default;
    TrivialMoveNonTrivialCopy(const TrivialMoveNonTrivialCopy&) {}
    TrivialMoveNonTrivialCopy(TrivialMoveNonTrivialCopy&&) = default;
    TrivialMoveNonTrivialCopy& operator=(const TrivialMoveNonTrivialCopy&) { return *this; }
    TrivialMoveNonTrivialCopy& operator=(TrivialMoveNonTrivialCopy&&) = default;
};

struct TrivialCopyNonTrivialMove {
    TrivialCopyNonTrivialMove() = default;
    TrivialCopyNonTrivialMove(const TrivialCopyNonTrivialMove&) = default;
    TrivialCopyNonTrivialMove(TrivialCopyNonTrivialMove&&) {}
    TrivialCopyNonTrivialMove& operator=(const TrivialCopyNonTrivialMove&) = default;
    TrivialCopyNonTrivialMove& operator=(TrivialCopyNonTrivialMove&&) { return *this; }
};

int run_test() {
    sink(
        ImplicitTypes::ApplyTypes<DoTestsMetafunction>{},
        ExplicitTypes::ApplyTypes<DoTestsMetafunction>{},
        NonLiteralTypes::ApplyTypes<DoTestsMetafunction>{},
        NonTrivialTypes::ApplyTypes<DoTestsMetafunction>{},
        DoTestsMetafunction<TrivialMoveNonTrivialCopy, TrivialCopyNonTrivialMove>{}
    );
    return 0;
}
} // namespace triviality
// -- END: test/std/utilities/optional/optional.object/triviality.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.object/types.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T>
// class optional
// {
// public:
//     typedef T value_type;
//     ...

#include <optional>
#include <type_traits>

#include "test_macros.h"

namespace types {
using std::optional;

template <class Opt, class T>
void
test()
{
    static_assert(std::is_same<typename Opt::value_type, T>::value, "");
}

int run_test()
{
    test<optional<int>, int>();
    test<optional<const int>, const int>();
    test<optional<double>, double>();
    test<optional<const double>, const double>();

  return 0;
}
} // namespace types
// -- END: test/std/utilities/optional/optional.object/types.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.relops/compare.three_way.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>

// [optional.relops], relational operators

// template<class T, three_way_comparable_with<T> U>
//   constexpr compare_three_way_result_t<T, U>
//     operator<=>(const optional<T>&, const optional<U>&);

#include <cassert>
#include <compare>
#include <optional>

#include "test_comparisons.h"

namespace relops::three_way {
#if _HAS_CXX20
constexpr bool test() {
  {
    std::optional<int> op1;
    std::optional<int> op2;

    assert((op1 <=> op2) == std::strong_ordering::equal);
    assert(testOrder(op1, op2, std::strong_ordering::equal));
  }
  {
    std::optional<int> op1{3};
    std::optional<int> op2{3};
    assert((op1 <=> op1) == std::strong_ordering::equal);
    assert(testOrder(op1, op1, std::strong_ordering::equal));
    assert((op1 <=> op2) == std::strong_ordering::equal);
    assert(testOrder(op1, op2, std::strong_ordering::equal));
    assert((op2 <=> op1) == std::strong_ordering::equal);
    assert(testOrder(op2, op1, std::strong_ordering::equal));
  }
  {
    std::optional<int> op;
    std::optional<int> op1{2};
    std::optional<int> op2{3};
    assert((op <=> op2) == std::strong_ordering::less);
    assert(testOrder(op, op2, std::strong_ordering::less));
    assert((op1 <=> op2) == std::strong_ordering::less);
    assert(testOrder(op1, op2, std::strong_ordering::less));
  }
  {
    std::optional<int> op;
    std::optional<int> op1{3};
    std::optional<int> op2{2};
    assert((op1 <=> op) == std::strong_ordering::greater);
    assert(testOrder(op1, op, std::strong_ordering::greater));
    assert((op1 <=> op2) == std::strong_ordering::greater);
    assert(testOrder(op1, op2, std::strong_ordering::greater));
  }

  return true;
}

int run_test() {
  assert(test());
  static_assert(test());
  return 0;
}
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
int run_test() {
    return 0;
}
#endif // ^^^ !_HAS_CXX20 ^^^
} // namespace relops::three_way
// -- END: test/std/utilities/optional/optional.relops/compare.three_way.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.relops/equal.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T, class U> constexpr bool operator==(const optional<T>& x, const optional<U>& y);

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace relops::equal {
using std::optional;

struct X {
  int i_;

  constexpr X(int i) : i_(i) {}
};

constexpr bool operator==(const X& lhs, const X& rhs) {
  return lhs.i_ == rhs.i_;
}

int run_test() {
  {
    typedef X T;
    typedef optional<T> O;

    constexpr O o1;    // disengaged
    constexpr O o2;    // disengaged
    constexpr O o3{1}; // engaged
    constexpr O o4{2}; // engaged
    constexpr O o5{1}; // engaged

    static_assert(o1 == o1, "");
    static_assert(o1 == o2, "");
    static_assert(!(o1 == o3), "");
    static_assert(!(o1 == o4), "");
    static_assert(!(o1 == o5), "");

    static_assert(o2 == o1, "");
    static_assert(o2 == o2, "");
    static_assert(!(o2 == o3), "");
    static_assert(!(o2 == o4), "");
    static_assert(!(o2 == o5), "");

    static_assert(!(o3 == o1), "");
    static_assert(!(o3 == o2), "");
    static_assert(o3 == o3, "");
    static_assert(!(o3 == o4), "");
    static_assert(o3 == o5, "");

    static_assert(!(o4 == o1), "");
    static_assert(!(o4 == o2), "");
    static_assert(!(o4 == o3), "");
    static_assert(o4 == o4, "");
    static_assert(!(o4 == o5), "");

    static_assert(!(o5 == o1), "");
    static_assert(!(o5 == o2), "");
    static_assert(o5 == o3, "");
    static_assert(!(o5 == o4), "");
    static_assert(o5 == o5, "");
  }
  {
    using O1 = optional<int>;
    using O2 = optional<long>;
    constexpr O1 o1(42);
    static_assert(o1 == O2(42), "");
    static_assert(!(O2(101) == o1), "");
  }
  {
    using O1 = optional<int>;
    using O2 = optional<const int>;
    constexpr O1 o1(42);
    static_assert(o1 == O2(42), "");
    static_assert(!(O2(101) == o1), "");
  }

  return 0;
}
} // namespace relops::equal
// -- END: test/std/utilities/optional/optional.relops/equal.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.relops/greater_equal.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T, class U> constexpr bool operator>= (const optional<T>& x, const optional<U>& y);

#include <optional>

#include "test_macros.h"

namespace relops::greater_equal {
using std::optional;

struct X {
  int i_;

  constexpr X(int i) : i_(i) {}
};

constexpr bool operator>=(const X& lhs, const X& rhs) {
  return lhs.i_ >= rhs.i_;
}

int run_test() {
  {
    typedef optional<X> O;

    constexpr O o1;    // disengaged
    constexpr O o2;    // disengaged
    constexpr O o3{1}; // engaged
    constexpr O o4{2}; // engaged
    constexpr O o5{1}; // engaged

    static_assert((o1 >= o1), "");
    static_assert((o1 >= o2), "");
    static_assert(!(o1 >= o3), "");
    static_assert(!(o1 >= o4), "");
    static_assert(!(o1 >= o5), "");

    static_assert((o2 >= o1), "");
    static_assert((o2 >= o2), "");
    static_assert(!(o2 >= o3), "");
    static_assert(!(o2 >= o4), "");
    static_assert(!(o2 >= o5), "");

    static_assert((o3 >= o1), "");
    static_assert((o3 >= o2), "");
    static_assert((o3 >= o3), "");
    static_assert(!(o3 >= o4), "");
    static_assert((o3 >= o5), "");

    static_assert((o4 >= o1), "");
    static_assert((o4 >= o2), "");
    static_assert((o4 >= o3), "");
    static_assert((o4 >= o4), "");
    static_assert((o4 >= o5), "");

    static_assert((o5 >= o1), "");
    static_assert((o5 >= o2), "");
    static_assert((o5 >= o3), "");
    static_assert(!(o5 >= o4), "");
    static_assert((o5 >= o5), "");
  }
  {
    using O1 = optional<int>;
    using O2 = optional<long>;
    constexpr O1 o1(42);
    static_assert(o1 >= O2(42), "");
    static_assert(!(O2(11) >= o1), "");
  }
  {
    using O1 = optional<int>;
    using O2 = optional<const int>;
    constexpr O1 o1(42);
    static_assert(o1 >= O2(42), "");
    static_assert(!(O2(1) >= o1), "");
  }

  return 0;
}
} // namespace relops::greater_equal
// -- END: test/std/utilities/optional/optional.relops/greater_equal.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.relops/greater_than.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T, class U> constexpr bool operator> (const optional<T>& x, const optional<U>& y);

#include <optional>

#include "test_macros.h"

namespace relops::greater_than {
using std::optional;

struct X {
  int i_;

  constexpr X(int i) : i_(i) {}
};

constexpr bool operator>(const X& lhs, const X& rhs) { return lhs.i_ > rhs.i_; }

int run_test() {
  {
    typedef optional<X> O;

    constexpr O o1;    // disengaged
    constexpr O o2;    // disengaged
    constexpr O o3{1}; // engaged
    constexpr O o4{2}; // engaged
    constexpr O o5{1}; // engaged

    static_assert(!(o1 > o1), "");
    static_assert(!(o1 > o2), "");
    static_assert(!(o1 > o3), "");
    static_assert(!(o1 > o4), "");
    static_assert(!(o1 > o5), "");

    static_assert(!(o2 > o1), "");
    static_assert(!(o2 > o2), "");
    static_assert(!(o2 > o3), "");
    static_assert(!(o2 > o4), "");
    static_assert(!(o2 > o5), "");

    static_assert((o3 > o1), "");
    static_assert((o3 > o2), "");
    static_assert(!(o3 > o3), "");
    static_assert(!(o3 > o4), "");
    static_assert(!(o3 > o5), "");

    static_assert((o4 > o1), "");
    static_assert((o4 > o2), "");
    static_assert((o4 > o3), "");
    static_assert(!(o4 > o4), "");
    static_assert((o4 > o5), "");

    static_assert((o5 > o1), "");
    static_assert((o5 > o2), "");
    static_assert(!(o5 > o3), "");
    static_assert(!(o5 > o4), "");
    static_assert(!(o5 > o5), "");
  }
  {
    using O1 = optional<int>;
    using O2 = optional<long>;
    constexpr O1 o1(42);
    static_assert(o1 > O2(1), "");
    static_assert(!(O2(42) > o1), "");
  }
  {
    using O1 = optional<int>;
    using O2 = optional<const int>;
    constexpr O1 o1(42);
    static_assert(o1 > O2(1), "");
    static_assert(!(O2(42) > o1), "");
  }

  return 0;
}
} // namespace relops::greater_than
// -- END: test/std/utilities/optional/optional.relops/greater_than.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.relops/less_equal.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T, class U> constexpr bool operator<= (const optional<T>& x, const optional<U>& y);

#include <optional>

#include "test_macros.h"

namespace relops::less_equal {
using std::optional;

struct X {
  int i_;

  constexpr X(int i) : i_(i) {}
};

constexpr bool operator<=(const X& lhs, const X& rhs) {
  return lhs.i_ <= rhs.i_;
}

int run_test() {
  {
    typedef optional<X> O;

    constexpr O o1;    // disengaged
    constexpr O o2;    // disengaged
    constexpr O o3{1}; // engaged
    constexpr O o4{2}; // engaged
    constexpr O o5{1}; // engaged

    static_assert((o1 <= o1), "");
    static_assert((o1 <= o2), "");
    static_assert((o1 <= o3), "");
    static_assert((o1 <= o4), "");
    static_assert((o1 <= o5), "");

    static_assert((o2 <= o1), "");
    static_assert((o2 <= o2), "");
    static_assert((o2 <= o3), "");
    static_assert((o2 <= o4), "");
    static_assert((o2 <= o5), "");

    static_assert(!(o3 <= o1), "");
    static_assert(!(o3 <= o2), "");
    static_assert((o3 <= o3), "");
    static_assert((o3 <= o4), "");
    static_assert((o3 <= o5), "");

    static_assert(!(o4 <= o1), "");
    static_assert(!(o4 <= o2), "");
    static_assert(!(o4 <= o3), "");
    static_assert((o4 <= o4), "");
    static_assert(!(o4 <= o5), "");

    static_assert(!(o5 <= o1), "");
    static_assert(!(o5 <= o2), "");
    static_assert((o5 <= o3), "");
    static_assert((o5 <= o4), "");
    static_assert((o5 <= o5), "");
  }
  {
    using O1 = optional<int>;
    using O2 = optional<long>;
    constexpr O1 o1(42);
    static_assert(o1 <= O2(42), "");
    static_assert(!(O2(101) <= o1), "");
  }
  {
    using O1 = optional<int>;
    using O2 = optional<const int>;
    constexpr O1 o1(42);
    static_assert(o1 <= O2(42), "");
    static_assert(!(O2(101) <= o1), "");
  }

  return 0;
}
} // namespace relops::less_equal
// -- END: test/std/utilities/optional/optional.relops/less_equal.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.relops/less_than.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T, class U> constexpr bool operator< (const optional<T>& x, const optional<U>& y);

#include <optional>

#include "test_macros.h"

namespace relops::less_than {
using std::optional;

struct X {
  int i_;

  constexpr X(int i) : i_(i) {}
};

constexpr bool operator<(const X& lhs, const X& rhs) { return lhs.i_ < rhs.i_; }

int run_test() {
  {
    typedef optional<X> O;

    constexpr O o1;    // disengaged
    constexpr O o2;    // disengaged
    constexpr O o3{1}; // engaged
    constexpr O o4{2}; // engaged
    constexpr O o5{1}; // engaged

    static_assert(!(o1 < o1), "");
    static_assert(!(o1 < o2), "");
    static_assert((o1 < o3), "");
    static_assert((o1 < o4), "");
    static_assert((o1 < o5), "");

    static_assert(!(o2 < o1), "");
    static_assert(!(o2 < o2), "");
    static_assert((o2 < o3), "");
    static_assert((o2 < o4), "");
    static_assert((o2 < o5), "");

    static_assert(!(o3 < o1), "");
    static_assert(!(o3 < o2), "");
    static_assert(!(o3 < o3), "");
    static_assert((o3 < o4), "");
    static_assert(!(o3 < o5), "");

    static_assert(!(o4 < o1), "");
    static_assert(!(o4 < o2), "");
    static_assert(!(o4 < o3), "");
    static_assert(!(o4 < o4), "");
    static_assert(!(o4 < o5), "");

    static_assert(!(o5 < o1), "");
    static_assert(!(o5 < o2), "");
    static_assert(!(o5 < o3), "");
    static_assert((o5 < o4), "");
    static_assert(!(o5 < o5), "");
  }
  {
    using O1 = optional<int>;
    using O2 = optional<long>;
    constexpr O1 o1(42);
    static_assert(o1 < O2(101), "");
    static_assert(!(O2(101) < o1), "");
  }
  {
    using O1 = optional<int>;
    using O2 = optional<const int>;
    constexpr O1 o1(42);
    static_assert(o1 < O2(101), "");
    static_assert(!(O2(101) < o1), "");
  }

  return 0;
}
} // namespace relops::less_than
// -- END: test/std/utilities/optional/optional.relops/less_than.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.relops/not_equal.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T, class U> constexpr bool operator!=(const optional<T>& x, const optional<U>& y);

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"

namespace relops::not_equal {
using std::optional;

struct X {
  int i_;

  constexpr X(int i) : i_(i) {}
};

constexpr bool operator!=(const X& lhs, const X& rhs) {
  return lhs.i_ != rhs.i_;
}

int run_test() {
  {
    typedef X T;
    typedef optional<T> O;

    constexpr O o1;    // disengaged
    constexpr O o2;    // disengaged
    constexpr O o3{1}; // engaged
    constexpr O o4{2}; // engaged
    constexpr O o5{1}; // engaged

    static_assert(!(o1 != o1), "");
    static_assert(!(o1 != o2), "");
    static_assert((o1 != o3), "");
    static_assert((o1 != o4), "");
    static_assert((o1 != o5), "");

    static_assert(!(o2 != o1), "");
    static_assert(!(o2 != o2), "");
    static_assert((o2 != o3), "");
    static_assert((o2 != o4), "");
    static_assert((o2 != o5), "");

    static_assert((o3 != o1), "");
    static_assert((o3 != o2), "");
    static_assert(!(o3 != o3), "");
    static_assert((o3 != o4), "");
    static_assert(!(o3 != o5), "");

    static_assert((o4 != o1), "");
    static_assert((o4 != o2), "");
    static_assert((o4 != o3), "");
    static_assert(!(o4 != o4), "");
    static_assert((o4 != o5), "");

    static_assert((o5 != o1), "");
    static_assert((o5 != o2), "");
    static_assert(!(o5 != o3), "");
    static_assert((o5 != o4), "");
    static_assert(!(o5 != o5), "");
  }
  {
    using O1 = optional<int>;
    using O2 = optional<long>;
    constexpr O1 o1(42);
    static_assert(o1 != O2(101), "");
    static_assert(!(O2(42) != o1), "");
  }
  {
    using O1 = optional<int>;
    using O2 = optional<const int>;
    constexpr O1 o1(42);
    static_assert(o1 != O2(101), "");
    static_assert(!(O2(42) != o1), "");
  }

  return 0;
}
} // namespace relops::not_equal
// -- END: test/std/utilities/optional/optional.relops/not_equal.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.specalg/make_optional.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//



// <optional>
//
// template <class T>
//   constexpr optional<decay_t<T>> make_optional(T&& v);

#include <optional>
#include <string>
#include <memory>
#include <cassert>

#include "test_macros.h"

namespace nonmembers::make_optional {
int run_test()
{
    {
        int arr[10];
        auto opt = std::make_optional(arr);
        ASSERT_SAME_TYPE(decltype(opt), std::optional<int*>);
        assert(*opt == arr);
    }
    {
        constexpr auto opt = std::make_optional(2);
        ASSERT_SAME_TYPE(decltype(opt), const std::optional<int>);
        static_assert(opt.value() == 2);
    }
    {
        auto opt = std::make_optional(2);
        ASSERT_SAME_TYPE(decltype(opt), std::optional<int>);
        assert(*opt == 2);
    }
    {
        const std::string s = "123";
        auto opt = std::make_optional(s);
        ASSERT_SAME_TYPE(decltype(opt), std::optional<std::string>);
        assert(*opt == "123");
    }
    {
        std::unique_ptr<int> s = std::make_unique<int>(3);
        auto opt = std::make_optional(std::move(s));
        ASSERT_SAME_TYPE(decltype(opt), std::optional<std::unique_ptr<int>>);
        assert(**opt == 3);
        assert(s == nullptr);
    }

  return 0;
}
} // namespace nonmembers::make_optional
// -- END: test/std/utilities/optional/optional.specalg/make_optional.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.specalg/make_optional_explicit.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T, class... Args>
//   constexpr optional<T> make_optional(Args&&... args);

#include <optional>
#include <string>
#include <memory>
#include <cassert>

#include "test_macros.h"

namespace nonmembers::make_optional_explicit {
int run_test()
{
    {
        constexpr auto opt = std::make_optional<int>('a');
        static_assert(*opt == int('a'));
    }
    {
        std::string s = "123";
        auto opt = std::make_optional<std::string>(s);
        assert(*opt == "123");
    }
    {
        std::unique_ptr<int> s = std::make_unique<int>(3);
        auto opt = std::make_optional<std::unique_ptr<int>>(std::move(s));
        assert(**opt == 3);
        assert(s == nullptr);
    }
    {
        auto opt = std::make_optional<std::string>(4u, 'X');
        assert(*opt == "XXXX");
    }

  return 0;
}
} // namespace nonmembers::make_optional_explicit
// -- END: test/std/utilities/optional/optional.specalg/make_optional_explicit.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.specalg/make_optional_explicit_initializer_list.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T, class U, class... Args>
//   constexpr optional<T> make_optional(initializer_list<U> il, Args&&... args);

#include <cassert>
#include <memory>
#include <optional>
#include <string>

#include "test_macros.h"

namespace nonmembers::make_optional_explicit_init_list {
struct TestT {
  int x;
  int size;
  int *ptr;
  constexpr TestT(std::initializer_list<int> il)
    : x(*il.begin()), size(static_cast<int>(il.size())), ptr(nullptr) {}
  constexpr TestT(std::initializer_list<int> il, int *p)
    : x(*il.begin()), size(static_cast<int>(il.size())), ptr(p) {}
};

constexpr bool test()
{
  {
    auto opt = std::make_optional<TestT>({42, 2, 3});
    ASSERT_SAME_TYPE(decltype(opt), std::optional<TestT>);
    assert(opt->x == 42);
    assert(opt->size == 3);
    assert(opt->ptr == nullptr);
  }
  {
    int i = 42;
    auto opt = std::make_optional<TestT>({42, 2, 3}, &i);
    ASSERT_SAME_TYPE(decltype(opt), std::optional<TestT>);
    assert(opt->x == 42);
    assert(opt->size == 3);
    assert(opt->ptr == &i);
  }
  return true;
}

int run_test()
{
  test();
  static_assert(test());
  {
    auto opt = std::make_optional<std::string>({'1', '2', '3'});
    assert(*opt == "123");
  }
  {
    auto opt = std::make_optional<std::string>({'a', 'b', 'c'}, std::allocator<char>{});
    assert(*opt == "abc");
  }
  return 0;
}
} // namespace nonmembers::make_optional_explicit_init_list
// -- END: test/std/utilities/optional/optional.specalg/make_optional_explicit_initializer_list.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.specalg/swap.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// template <class T> void swap(optional<T>& x, optional<T>& y)
//     noexcept(noexcept(x.swap(y)));

#include <optional>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"

namespace nonmembers::swap_ {
using std::optional;

class X
{
    int i_;
public:
    static unsigned dtor_called;
    X(int i) : i_(i) {}
    X(X&& x) = default;
    X& operator=(X&&) = default;
    ~X() {++dtor_called;}

    friend bool operator==(const X& x, const X& y) {return x.i_ == y.i_;}
};

unsigned X::dtor_called = 0;

class Y
{
    int i_;
public:
    static unsigned dtor_called;
    Y(int i) : i_(i) {}
    Y(Y&&) = default;
    ~Y() {++dtor_called;}

    friend constexpr bool operator==(const Y& x, const Y& y) {return x.i_ == y.i_;}
    friend void swap(Y& x, Y& y) {std::swap(x.i_, y.i_);}
};

unsigned Y::dtor_called = 0;

class Z
{
    int i_;
public:
    Z(int i) : i_(i) {}
    Z(Z&&) { TEST_THROW(7);}

    friend constexpr bool operator==(const Z& x, const Z& y) {return x.i_ == y.i_;}
    friend void swap(Z&, Z&) { TEST_THROW(6);}
};


struct NonSwappable {
    NonSwappable(NonSwappable const&) = delete;
};
void swap(NonSwappable&, NonSwappable&) = delete;

void test_swap_sfinae() {
    using std::optional;
    {
        using T = TestTypes::TestType;
        static_assert(std::is_swappable_v<optional<T>>, "");
    }
    {
        using T = TestTypes::MoveOnly;
        static_assert(std::is_swappable_v<optional<T>>, "");
    }
    {
        using T = TestTypes::Copyable;
        static_assert(std::is_swappable_v<optional<T>>, "");
    }
    {
        using T = TestTypes::NoCtors;
        static_assert(!std::is_swappable_v<optional<T>>, "");
    }
    {
        using T = NonSwappable;
        static_assert(!std::is_swappable_v<optional<T>>, "");
    }
    {
        // Even though CopyOnly has deleted move operations, those operations
        // cause optional<CopyOnly> to have implicitly deleted move operations
        // that decay into copies.
        using T = TestTypes::CopyOnly;
        using Opt = optional<T>;
        T::reset();
        Opt L(101), R(42);
        T::reset_constructors();
        std::swap(L, R);
        assert(L->value == 42);
        assert(R->value == 101);
        assert(T::copy_constructed == 1);
        assert(T::constructed == T::copy_constructed);
        assert(T::assigned == 2);
        assert(T::assigned == T::copy_assigned);
    }
}

int run_test()
{
    test_swap_sfinae();
    {
        optional<int> opt1;
        optional<int> opt2;
        static_assert(noexcept(swap(opt1, opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        swap(opt1, opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        optional<int> opt1(1);
        optional<int> opt2;
        static_assert(noexcept(swap(opt1, opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        swap(opt1, opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        optional<int> opt1;
        optional<int> opt2(2);
        static_assert(noexcept(swap(opt1, opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        swap(opt1, opt2);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        optional<int> opt1(1);
        optional<int> opt2(2);
        static_assert(noexcept(swap(opt1, opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        swap(opt1, opt2);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        optional<X> opt1;
        optional<X> opt2;
        static_assert(noexcept(swap(opt1, opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        swap(opt1, opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        assert(X::dtor_called == 0);
    }
    {
        optional<X> opt1(1);
        optional<X> opt2;
        static_assert(noexcept(swap(opt1, opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        X::dtor_called = 0;
        swap(opt1, opt2);
        assert(X::dtor_called == 1);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        optional<X> opt1;
        optional<X> opt2(2);
        static_assert(noexcept(swap(opt1, opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        X::dtor_called = 0;
        swap(opt1, opt2);
        assert(X::dtor_called == 1);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        optional<X> opt1(1);
        optional<X> opt2(2);
        static_assert(noexcept(swap(opt1, opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        X::dtor_called = 0;
        swap(opt1, opt2);
        assert(X::dtor_called == 1);  // from inside std::swap
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        optional<Y> opt1;
        optional<Y> opt2;
        static_assert(noexcept(swap(opt1, opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        swap(opt1, opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        assert(Y::dtor_called == 0);
    }
    {
        optional<Y> opt1(1);
        optional<Y> opt2;
        static_assert(noexcept(swap(opt1, opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        Y::dtor_called = 0;
        swap(opt1, opt2);
        assert(Y::dtor_called == 1);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        optional<Y> opt1;
        optional<Y> opt2(2);
        static_assert(noexcept(swap(opt1, opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        Y::dtor_called = 0;
        swap(opt1, opt2);
        assert(Y::dtor_called == 1);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        optional<Y> opt1(1);
        optional<Y> opt2(2);
        static_assert(noexcept(swap(opt1, opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        Y::dtor_called = 0;
        swap(opt1, opt2);
        assert(Y::dtor_called == 0);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        optional<Z> opt1;
        optional<Z> opt2;
        static_assert(noexcept(swap(opt1, opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        swap(opt1, opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        optional<Z> opt1;
        opt1.emplace(1);
        optional<Z> opt2;
        static_assert(noexcept(swap(opt1, opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        try
        {
            swap(opt1, opt2);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 7);
        }
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        optional<Z> opt1;
        optional<Z> opt2;
        opt2.emplace(2);
        static_assert(noexcept(swap(opt1, opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        try
        {
            swap(opt1, opt2);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 7);
        }
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
    }
    {
        optional<Z> opt1;
        opt1.emplace(1);
        optional<Z> opt2;
        opt2.emplace(2);
        static_assert(noexcept(swap(opt1, opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        try
        {
            swap(opt1, opt2);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
        }
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
    }
#endif // TEST_HAS_NO_EXCEPTIONS

  return 0;
}
} // namespace nonmembers::swap_
// -- END: test/std/utilities/optional/optional.specalg/swap.pass.cpp

// -- BEGIN: test/std/utilities/optional/optional.syn/optional_includes_initializer_list.pass.cpp
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// <optional>

// #include <initializer_list>

#include <optional>

#include "test_macros.h"

namespace optional_includes_initializer_list {
int run_test()
{
    using std::optional;

    std::initializer_list<int> list;
    (void)list;

  return 0;
}
} // namespace optional_includes_initializer_list
// -- END: test/std/utilities/optional/optional.syn/optional_includes_initializer_list.pass.cpp
// LLVM SOURCES END
// clang-format on

#include <cassert>
#include <optional>
#include <type_traits>
#include <utility>


namespace msvc {
    namespace size {
        template <int>
        struct empty {};

        struct not_empty {
            int i;
        };

        struct __declspec(empty_bases) many_bases : empty<0>, empty<1>, empty<2>, empty<3> {};

        template <class T>
        constexpr bool check_size = sizeof(std::optional<T>) == sizeof(T) + alignof(T);

        static_assert(check_size<bool>);
        static_assert(check_size<char>);
        static_assert(check_size<unsigned char>);
        static_assert(check_size<int>);
        static_assert(check_size<unsigned int>);
        static_assert(check_size<long>);
        static_assert(check_size<long long>);
        static_assert(check_size<float>);
        static_assert(check_size<double>);
        static_assert(check_size<void*>);
        static_assert(check_size<empty<0>>);
        static_assert(check_size<not_empty>);
        static_assert(check_size<many_bases>);
    } // namespace size

    namespace lwg2842 {
        struct ConvertibleFromInPlace {
            ConvertibleFromInPlace(const std::in_place_t&) {}
        };

        using std::in_place_t;
        using std::is_constructible_v;
        using O = std::optional<ConvertibleFromInPlace>;
        // ConvertibleFromInPlace is not default constructible, so these constructions do not match
        // optional(in_place_t, args...). They will match the converting constructor template if it does not properly
        // reject arguments that decay to std::in_place_t as required by LWG-2842.
        static_assert(!is_constructible_v<O, in_place_t>);
        static_assert(!is_constructible_v<O, in_place_t&>);
        static_assert(!is_constructible_v<O, const in_place_t>);
        static_assert(!is_constructible_v<O, const in_place_t&>);
    } // namespace lwg2842

    namespace lwg3836 {
        static_assert(std::is_convertible_v<std::optional<int>, std::optional<bool>>);
        static_assert(std::is_convertible_v<const std::optional<int>&, std::optional<bool>>);

#if _HAS_CXX20
#define CONSTEXPR20 constexpr
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
#define CONSTEXPR20 inline
#endif // ^^^ !_HAS_CXX20 ^^^
        CONSTEXPR20 bool run_test() {
            std::optional<int> oi  = 0;
            std::optional<bool> ob = oi;
            assert(!ob.value());
            assert(!std::optional<bool>{std::optional<int>{0}}.value());

            return true;
        }
#undef CONSTEXPR20

#if _HAS_CXX20
        static_assert(run_test());
#endif // _HAS_CXX20
    } // namespace lwg3836

    namespace vso406124 {
        // Defend against regression of VSO-406124
        void run_test() {
            (void) std::optional<int>(0).value();
        }
    } // namespace vso406124

    namespace vso458030 {
        // Defend against regression of VSO-458030
        struct Meow {
            std::optional<std::pair<int, int>> DoStuff();

            void Weird(std::optional<std::pair<int, int>>& val) {
                [&] { val = DoStuff(); }();
            }
        };
    } // namespace vso458030

    namespace vso508126 {
        // Defend against regression of VSO-508126
        void run_test() {
            struct S {};
            static_assert(!std::is_copy_constructible_v<volatile S>);
            static_assert(!std::is_copy_constructible_v<std::optional<volatile S>>);
        }
    } // namespace vso508126

    namespace vso614907 {
        // Defend against regression of VSO-614907
        struct nontrivial_copy {
            nontrivial_copy() = default;
            nontrivial_copy(const nontrivial_copy&) {}
            nontrivial_copy(nontrivial_copy&&)                 = default;
            nontrivial_copy& operator=(const nontrivial_copy&) = default;
            nontrivial_copy& operator=(nontrivial_copy&&)      = default;
        };

        const std::optional<nontrivial_copy> s;

        struct Test {
            std::optional<nontrivial_copy> s_{s};
        };

        void run_test() {
            Test t;
            (void) t;
        }
    } // namespace vso614907

    namespace gh2458 {
        // GH-2458: optional<const meow> o = std::move(lvalue_optional_const_meow);
        // was moving from the contained const object
        enum class action { none, copy, move };
        action last = action::none;

        struct Copyable {
            Copyable() = default;
            Copyable(const Copyable&) {
                last = action::copy;
            }
            Copyable(Copyable&&) {
                last = action::move;
            }
        };

        struct ConstMovable {
            ConstMovable() = default;
            ConstMovable(const ConstMovable&) {
                last = action::copy;
            }
            ConstMovable(const ConstMovable&&) {
                last = action::move;
            }
        };

        template <class T, action Result>
        void testMove() {
            std::optional<T> orig{std::in_place};
            std::optional<T> moved = std::move(orig);
            assert(last == Result);
        }

        void run_test() {
            testMove<Copyable, action::move>();
            testMove<const Copyable, action::copy>();
            testMove<ConstMovable, action::move>();
            testMove<const ConstMovable, action::move>();
        }
    } // namespace gh2458

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
                std::optional<const int> oc{};
                oc.emplace(0);
                static_assert(!std::is_copy_assignable_v<decltype(oc)>);
                static_assert(!std::is_move_assignable_v<decltype(oc)>);
                static_assert(!std::is_swappable_v<decltype(oc)>);

                std::optional<volatile int> ov{};
                std::optional<volatile int> ov2{};
                ov.emplace(0);
                swap(ov, ov);
                ov = ov2;
                ov = std::move(ov2);

                std::optional<const volatile int> ocv{};
                ocv.emplace(0);
                static_assert(!std::is_copy_assignable_v<decltype(ocv)>);
                static_assert(!std::is_move_assignable_v<decltype(ocv)>);
                static_assert(!std::is_swappable_v<decltype(ocv)>);
            }
            {
                std::optional<const CvAssignable> oc{};
                std::optional<const CvAssignable> oc2{};
                oc.emplace(CvAssignable{});
                swap(oc, oc);
                oc = oc2;
                oc = std::move(oc2);

                std::optional<volatile CvAssignable> ov{};
                std::optional<volatile CvAssignable> ov2{};
                ov.emplace(CvAssignable{});
                swap(ov, ov);
                ov = ov2;
                ov = std::move(ov2);

                std::optional<const volatile CvAssignable> ocv{};
                std::optional<const volatile CvAssignable> ocv2{};
                ocv.emplace(CvAssignable{});
                swap(ocv, ocv);
                ocv = ocv2;
                ocv = std::move(ocv2);
            }
        }
    } // namespace assign_cv
} // namespace msvc

int main() {
    utility::in_place::run_test();

    bad_optional_access::default_::run_test();
    bad_optional_access::derive::run_test();

    comp_with_t::three_way::run_test();
    comp_with_t::equal::run_test();
    comp_with_t::greater_than::run_test();
    comp_with_t::greater_equal::run_test();
    comp_with_t::less_equal::run_test();
    comp_with_t::less_than::run_test();
    comp_with_t::not_equal::run_test();

    hash::enabled_hash::run_test();
    hash::run_test();

    monadic::and_then::run_test();
    monadic::or_else::run_test();
    monadic::transform::run_test();

    nullops::three_way::run_test();
    nullops::equal::run_test();
    nullops::greater_than::run_test();
    nullops::greater_equal::run_test();
    nullops::less_equal::run_test();
    nullops::less_than::run_test();
    nullops::not_equal::run_test();

    nullopt::run_test();

    assign::value::run_test();
    assign::const_optional_U::run_test();
    assign::copy::run_test();
    assign::emplace::run_test();
    assign::emplace_initializer_list::run_test();
    assign::move::run_test();
    assign::nullopt::run_test();
    assign::optional_U::run_test();

    ctor::const_optional_U::run_test();
    ctor::const_T::run_test();
    ctor::copy::run_test();
    ctor::deduct::run_test();
    ctor::default_::run_test();
    ctor::clobber::run_test();
    ctor::explicit_const_optional_U::run_test();
    ctor::explicit_optional_U::run_test();
    ctor::initializer_list::run_test();
    ctor::in_place::run_test();
    ctor::move::run_test();
    ctor::nullopt::run_test();
    ctor::optional_U::run_test();
    ctor::rvalue_T::run_test();
    ctor::U::run_test();

    dtor::run_test();

    modifiers::reset::run_test();

    observe::op_bool::run_test();
    observe::deref::run_test();
    observe::deref_const::run_test();
    observe::deref_const_rvalue::run_test();
    observe::deref_rvalue::run_test();
    observe::has_value::run_test();
    observe::op_arrow::run_test();
    observe::op_arrow_const::run_test();
    observe::value::run_test();
    observe::value_const::run_test();
    observe::value_const_rvalue::run_test();
    observe::value_or::run_test();
    observe::value_or_const::run_test();
    observe::value_rvalue::run_test();

    member_swap::run_test();

    special_members::run_test();

    triviality::run_test();

    types::run_test();

    relops::three_way::run_test();
    relops::equal::run_test();
    relops::greater_equal::run_test();
    relops::greater_than::run_test();
    relops::less_equal::run_test();
    relops::less_than::run_test();
    relops::not_equal::run_test();

    nonmembers::make_optional::run_test();
    nonmembers::make_optional_explicit::run_test();
    nonmembers::make_optional_explicit_init_list::run_test();
    nonmembers::swap_::run_test();

    optional_includes_initializer_list::run_test();

    msvc::lwg3836::run_test();

    msvc::vso406124::run_test();
    msvc::vso508126::run_test();
    msvc::vso614907::run_test();

    msvc::gh2458::run_test();

    msvc::assign_cv::run_test();
}
