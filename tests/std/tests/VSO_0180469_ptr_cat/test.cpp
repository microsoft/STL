// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_CXX20_IS_POD_DEPRECATION_WARNING

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <iterator>
#include <list>
#include <type_traits>
#include <utility>
#include <vector>

#ifdef __cpp_lib_concepts
#include <ranges>
#include <span>
#endif // __cpp_lib_concepts

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

int main() {} // COMPILE-ONLY

template <class Lhs, class Rhs>
void assert_same() {
    STATIC_ASSERT(is_same_v<Lhs, Rhs>);
}

template <int Expected, class Source, class Dest>
struct test_ptr_cat_helper {
    static constexpr bool CopyReallyTrivial     = _Ptr_copy_cat<Source, Dest>::_Really_trivial;
    static constexpr bool CopyTriviallyCopyable = _Ptr_copy_cat<Source, Dest>::_Trivially_copyable;
    STATIC_ASSERT(Expected == CopyReallyTrivial + CopyTriviallyCopyable);
    STATIC_ASSERT(!CopyReallyTrivial || CopyTriviallyCopyable);

    static constexpr bool MoveReallyTrivial     = _Ptr_move_cat<Source, Dest>::_Really_trivial;
    static constexpr bool MoveTriviallyCopyable = _Ptr_move_cat<Source, Dest>::_Trivially_copyable;
    STATIC_ASSERT(Expected == MoveReallyTrivial + MoveTriviallyCopyable);
    STATIC_ASSERT(!MoveReallyTrivial || MoveTriviallyCopyable);
};

template <int Expected, class Source, class Dest, int Volatile = Expected>
void test_ptr_cat() {
    (void) test_ptr_cat_helper<Expected, Source*, Dest*>{};
    // Also make sure that the source being const doesn't change the answer
    (void) test_ptr_cat_helper<Expected, const Source*, Dest*>{};
    // Also make sure wrapping the source in a move_iterator doesn't change the answer:
    (void) test_ptr_cat_helper<Expected, move_iterator<Source*>, Dest*>{};
    (void) test_ptr_cat_helper<Expected, move_iterator<const Source*>, Dest*>{};
    // const Dest doesn't make any sense so we always want that to blow up in the general implementation
    (void) test_ptr_cat_helper<0, Source*, const Dest*>{};
    (void) test_ptr_cat_helper<0, const Source*, const Dest*>{};
    (void) test_ptr_cat_helper<0, volatile Source*, const Dest*>{};
    (void) test_ptr_cat_helper<0, Source*, const volatile Dest*>{};
    (void) test_ptr_cat_helper<0, volatile Source*, const volatile Dest*>{};
    (void) test_ptr_cat_helper<0, const volatile Source*, const Dest*>{};
    (void) test_ptr_cat_helper<0, const Source*, const volatile Dest*>{};
    (void) test_ptr_cat_helper<0, const volatile Source*, const volatile Dest*>{};
    // volatile anywhere should go to the trivial implementation for builtin types, but
    // the general implementation for PODs, since the compiler-generated copy assign has signature:
    // Meow& operator=(const Meow&);
    // which hates volatile on both the source and the target
    (void) test_ptr_cat_helper<Volatile, volatile Source*, Dest*>{};
    (void) test_ptr_cat_helper<Volatile, Source*, volatile Dest*>{};
    (void) test_ptr_cat_helper<Volatile, volatile Source*, volatile Dest*>{};
    (void) test_ptr_cat_helper<Volatile, const volatile Source*, Dest*>{};
    (void) test_ptr_cat_helper<Volatile, const Source*, volatile Dest*>{};
    (void) test_ptr_cat_helper<Volatile, const volatile Source*, volatile Dest*>{};

    // Also make sure _Ptr_cat listens to the iterator type
    (void) test_ptr_cat_helper<0, typename list<Source>::iterator, typename list<Dest>::iterator>{};
    (void) test_ptr_cat_helper<0, typename list<Source>::const_iterator, typename list<Dest>::iterator>{};
}

struct pod_struct {
    int a;
};

STATIC_ASSERT(is_pod_v<pod_struct>);

struct trivially_copyable_struct {
    int a;
    trivially_copyable_struct() {
        // do some complex setup :)
    }

    trivially_copyable_struct(const trivially_copyable_struct&) = default;
    trivially_copyable_struct& operator=(const trivially_copyable_struct&) = default;
};

STATIC_ASSERT(is_trivially_copyable_v<trivially_copyable_struct>);
STATIC_ASSERT(!is_trivial_v<trivially_copyable_struct>);

struct custom_copy_struct {
    int a;
    custom_copy_struct() = default;
    custom_copy_struct(const custom_copy_struct& o) {
        a = o.a;
    }

    custom_copy_struct& operator=(const custom_copy_struct& o) {
        a = o.a;
        return *this;
    }
};

STATIC_ASSERT(!is_trivially_copyable_v<custom_copy_struct>);
STATIC_ASSERT(!is_trivial_v<custom_copy_struct>);

enum int_enum : int {};
enum short_enum : short {};
enum bool_enum : bool {};

enum class int_enum_class : int {};
enum class short_enum_class : short {};
enum class bool_enum_class : bool {};

struct base_class {};

bool operator<(const base_class&, const base_class&) {
    return false;
}

struct derived_class : base_class {};


void ptr_cat_test_cases() {
    // Identity cases:
    test_ptr_cat<2, int, int>();
    test_ptr_cat<2, bool, bool>();
    test_ptr_cat<2, pod_struct, pod_struct, 0>();
    test_ptr_cat<1, trivially_copyable_struct, trivially_copyable_struct, 0>();
    test_ptr_cat<0, custom_copy_struct, custom_copy_struct>();
    test_ptr_cat<2, int_enum, int_enum>();
    test_ptr_cat<2, short_enum, short_enum>();
    test_ptr_cat<2, bool_enum, bool_enum>();
    test_ptr_cat<2, int_enum_class, int_enum_class>();
    test_ptr_cat<2, short_enum_class, short_enum_class>();
    test_ptr_cat<2, bool_enum_class, bool_enum_class>();

    // Allow conversions between integers of the same size:
    static_assert(sizeof(int) == sizeof(long), "this test assumes that int and long are the same size");
    test_ptr_cat<2, int, long>();
    test_ptr_cat<2, long, int>();
    test_ptr_cat<2, unsigned int, long>();
    test_ptr_cat<2, long, unsigned int>();
    test_ptr_cat<2, int_enum, int>();
    test_ptr_cat<2, short_enum, short>();
    test_ptr_cat<2, bool_enum, bool>();
    test_ptr_cat<2, int, unsigned int>();
    test_ptr_cat<2, unsigned int, int>();
    test_ptr_cat<2, char, signed char>();
    test_ptr_cat<2, char, unsigned char>();
    test_ptr_cat<2, signed char, char>();
    test_ptr_cat<2, signed char, unsigned char>();
    test_ptr_cat<2, unsigned char, char>();
    test_ptr_cat<2, unsigned char, signed char>();
    // Don't allow conversion from int to enum (since that requires a cast):
    test_ptr_cat<0, int, int_enum>();
    test_ptr_cat<0, short, short_enum>();
    test_ptr_cat<0, bool, bool_enum>();

    // Don't allow such conversions if the sizes differ:
    test_ptr_cat<0, int, short>();
    test_ptr_cat<0, short_enum, int>();

    // Pointer cases:
    test_ptr_cat<2, int*, int*>();
    test_ptr_cat<2, pod_struct*, pod_struct*>();
    test_ptr_cat<2, trivially_copyable_struct*, trivially_copyable_struct*>();
    test_ptr_cat<2, custom_copy_struct*, custom_copy_struct*>();
    // ... adding const on the destination:
    test_ptr_cat<2, int*, const int*>();
    test_ptr_cat<2, pod_struct*, const pod_struct*>();
    test_ptr_cat<2, trivially_copyable_struct*, const trivially_copyable_struct*>();
    test_ptr_cat<2, custom_copy_struct*, const custom_copy_struct*>();
    // ... other cv changes:
    test_ptr_cat<2, int*, volatile int*>();
    test_ptr_cat<2, int*, const volatile int*>();
    test_ptr_cat<2, const int*, const int*>();
    test_ptr_cat<2, const int*, const volatile int*>();
    test_ptr_cat<2, volatile int*, volatile int*>();
    test_ptr_cat<2, volatile int*, const volatile int*>();
    test_ptr_cat<2, const volatile int*, const volatile int*>();

    // Pointers to derived are implicitly convertible to pointers to base, but there
    // may still be code required to change an offset, so we don't want to memmove them
    test_ptr_cat<0, derived_class, base_class>();
    test_ptr_cat<0, derived_class*, base_class*>();
    // Paranoia
    test_ptr_cat<0, base_class, derived_class>();
    test_ptr_cat<0, base_class*, derived_class*>();

    // Don't allow conversions between pointers to distinct types, even if
    // those types have the same representation. That is:
    //     unsigned int * a = meow;
    //     int * b = purr;
    //     a = b;
    // won't compile, so we don't want copy-like algorithms to compile
    // by reinterpret_cast-ing to char *:
    test_ptr_cat<0, int*, unsigned int*>();
    test_ptr_cat<0, int*, const unsigned int*>();

    // Similarly, don't allow enum class types to be implicitly converted to/from
    // integral types:
    test_ptr_cat<0, int_enum_class, int>();
    test_ptr_cat<0, int, int_enum_class>();
    test_ptr_cat<0, short_enum_class, short>();
    test_ptr_cat<0, bool_enum_class, bool>();
    test_ptr_cat<0, bool, bool_enum_class>();
    test_ptr_cat<0, bool_enum, bool_enum_class>();
    test_ptr_cat<0, bool_enum_class, bool_enum>();

    // bool always generates code when you inspect it, so special case it to the general thing:
    test_ptr_cat<0, bool, char>();
    test_ptr_cat<0, char, bool>();
    test_ptr_cat<0, bool_enum, char>();
}

template <bool Expected, class Elem1, class Elem2, class Pr>
void test_case_Equal_memcmp_is_safe_comparator() {
    // Default case
    STATIC_ASSERT(_Equal_memcmp_is_safe<Elem1*, Elem2*, Pr> == Expected);
    // Adding const should not change the answer
    STATIC_ASSERT(_Equal_memcmp_is_safe<const Elem1*, Elem2*, Pr> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<Elem1*, const Elem2*, Pr> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const Elem1*, const Elem2*, Pr> == Expected);
    // Top level const should not change the answer
    STATIC_ASSERT(_Equal_memcmp_is_safe<Elem1* const, Elem2*, Pr> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<Elem1*, Elem2* const, Pr> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<Elem1* const, Elem2* const, Pr> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<Elem1* const, const Elem2*, Pr> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<Elem1*, const Elem2* const, Pr> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<Elem1* const, const Elem2* const, Pr> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const Elem1* const, Elem2*, Pr> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const Elem1*, Elem2* const, Pr> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const Elem1* const, Elem2* const, Pr> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const Elem1* const, const Elem2*, Pr> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const Elem1*, const Elem2* const, Pr> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const Elem1* const, const Elem2* const, Pr> == Expected);
    // Adding volatile anywhere should explode
    STATIC_ASSERT(_Equal_memcmp_is_safe<volatile Elem1*, Elem2*, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<Elem1*, volatile Elem2*, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<volatile Elem1*, volatile Elem2*, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const volatile Elem1*, Elem2*, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const Elem1*, volatile Elem2*, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const volatile Elem1*, volatile Elem2*, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<volatile Elem1*, const Elem2*, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<Elem1*, const volatile Elem2*, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<volatile Elem1*, const volatile Elem2*, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const volatile Elem1*, const Elem2*, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const Elem1*, const volatile Elem2*, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<const volatile Elem1*, const volatile Elem2*, Pr> == false);

#ifdef __cpp_lib_concepts
    // contiguous iterators should not change the answer
    if constexpr (!is_same_v<Elem1, bool> && !is_same_v<Elem2, bool>) { // vector<bool>::iterator is not contiguous
        STATIC_ASSERT(
            _Equal_memcmp_is_safe<typename vector<Elem1>::iterator, typename vector<Elem2>::iterator, Pr> == Expected);
        STATIC_ASSERT(_Equal_memcmp_is_safe<typename vector<Elem1>::const_iterator,
                          typename vector<Elem2>::const_iterator, Pr> == Expected);
    }
    STATIC_ASSERT(
        _Equal_memcmp_is_safe<typename array<Elem1, 1>::iterator, typename array<Elem2, 1>::iterator, Pr> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<typename array<Elem1, 1>::const_iterator,
                      typename array<Elem2, 1>::const_iterator, Pr> == Expected);
    // Mixing contiguous iterators should not change the answer
    if constexpr (!is_same_v<Elem1, bool> && !is_same_v<Elem2, bool>) {
        STATIC_ASSERT(_Equal_memcmp_is_safe<typename vector<Elem1>::iterator, typename vector<Elem2>::const_iterator,
                          Pr> == Expected);
    }

    if constexpr (!is_same_v<Elem2, bool>) {
        STATIC_ASSERT(_Equal_memcmp_is_safe<typename array<Elem1, 1>::const_iterator,
                          typename vector<Elem2>::const_iterator, Pr> == Expected);
    }

    if constexpr (!is_same_v<Elem1, bool>) {
        STATIC_ASSERT(_Equal_memcmp_is_safe<typename vector<Elem1>::iterator, typename array<Elem2, 1>::iterator,
                          Pr> == Expected);
        STATIC_ASSERT(_Equal_memcmp_is_safe<typename vector<Elem1>::iterator, typename array<Elem2, 1>::const_iterator,
                          Pr> == Expected);
    }
    // span iterators are contiguous
    STATIC_ASSERT(
        _Equal_memcmp_is_safe<typename span<Elem1>::iterator, typename span<Elem2>::iterator, Pr> == Expected);
    STATIC_ASSERT(
        _Equal_memcmp_is_safe<typename span<const Elem1>::iterator, typename span<Elem2>::iterator, Pr> == Expected);
    STATIC_ASSERT(
        _Equal_memcmp_is_safe<typename span<Elem1>::iterator, typename span<const Elem2>::iterator, Pr> == Expected);
    STATIC_ASSERT(_Equal_memcmp_is_safe<typename span<const Elem1>::iterator, typename span<const Elem2>::iterator,
                      Pr> == Expected);
    // contiguous iterators to volatile should explode
    STATIC_ASSERT(
        _Equal_memcmp_is_safe<typename span<volatile Elem1>::iterator, typename span<Elem2>::iterator, Pr> == false);
    STATIC_ASSERT(
        _Equal_memcmp_is_safe<typename span<Elem1>::iterator, typename span<volatile Elem2>::iterator, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<typename span<volatile Elem1>::iterator,
                      typename span<volatile Elem2>::iterator, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<typename span<const volatile Elem1>::iterator, typename span<Elem2>::iterator,
                      Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<typename span<const Elem1>::iterator, typename span<volatile Elem2>::iterator,
                      Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<typename span<const volatile Elem1>::iterator,
                      typename span<volatile Elem2>::iterator, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<typename span<volatile Elem1>::iterator, typename span<const Elem2>::iterator,
                      Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<typename span<Elem1>::iterator, typename span<const volatile Elem2>::iterator,
                      Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<typename span<volatile Elem1>::iterator,
                      typename span<const volatile Elem2>::iterator, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<typename span<const volatile Elem1>::iterator,
                      typename span<const Elem2>::iterator, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<typename span<const Elem1>::iterator,
                      typename span<const volatile Elem2>::iterator, Pr> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<typename span<const volatile Elem1>::iterator,
                      typename span<const volatile Elem2>::iterator, Pr> == false);
#endif // __cpp_lib_concepts

    // Non-contiguous iterators should explode
    STATIC_ASSERT(_Equal_memcmp_is_safe<typename list<Elem1>::iterator, typename list<Elem2>::iterator, Pr> == false);
}

template <bool Expected, class Elem1, class Elem2>
void test_case_Equal_memcmp_is_safe() {
    test_case_Equal_memcmp_is_safe_comparator<Expected && is_same_v<Elem1, Elem2>, Elem1, Elem2, equal_to<Elem1>>();
    test_case_Equal_memcmp_is_safe_comparator<Expected, Elem1, Elem2, equal_to<>>();
#ifdef __cpp_lib_concepts
    test_case_Equal_memcmp_is_safe_comparator<Expected, Elem1, Elem2, ranges::equal_to>();
#endif // __cpp_lib_concepts

    // equal_to< some other T > should explode
    STATIC_ASSERT(_Equal_memcmp_is_safe<Elem1*, Elem2*, equal_to<list<int>>> == false);
    // Non equal_to comparison functions should explode
    auto lambda = [](Elem1*, Elem2*) { return false; };
    STATIC_ASSERT(_Equal_memcmp_is_safe<Elem1*, Elem2*, decltype(lambda)> == false);
    // equal_to<T> should not explode
    STATIC_ASSERT(_Equal_memcmp_is_safe<Elem1*, Elem2*, equal_to<Elem1>> == (Expected && is_same_v<Elem1, Elem2>) );
    // But again, not volatile
    STATIC_ASSERT(_Equal_memcmp_is_safe<Elem1*, Elem2*, equal_to<volatile Elem1>> == false);
    STATIC_ASSERT(_Equal_memcmp_is_safe<Elem1*, Elem2*, equal_to<const volatile Elem1>> == false);
}

void equal_safe_test_cases() {
    // memcmp is safe for integral types
    test_case_Equal_memcmp_is_safe<true, bool, bool>();
    test_case_Equal_memcmp_is_safe<true, char, char>();
    test_case_Equal_memcmp_is_safe<true, signed char, signed char>();
    test_case_Equal_memcmp_is_safe<true, unsigned char, unsigned char>();
    test_case_Equal_memcmp_is_safe<true, short, short>();
    test_case_Equal_memcmp_is_safe<true, unsigned short, unsigned short>();
    test_case_Equal_memcmp_is_safe<true, wchar_t, wchar_t>();
    test_case_Equal_memcmp_is_safe<true, char16_t, char16_t>();
    test_case_Equal_memcmp_is_safe<true, char32_t, char32_t>();
    test_case_Equal_memcmp_is_safe<true, int, int>();
    test_case_Equal_memcmp_is_safe<true, unsigned int, unsigned int>();
    test_case_Equal_memcmp_is_safe<true, long, long>();
    test_case_Equal_memcmp_is_safe<true, unsigned long, unsigned long>();
    test_case_Equal_memcmp_is_safe<true, long long, long long>();
    test_case_Equal_memcmp_is_safe<true, unsigned long long, unsigned long long>();
    // unless their sizes differ
    test_case_Equal_memcmp_is_safe<false, unsigned short, unsigned long long>();
    test_case_Equal_memcmp_is_safe<false, unsigned long long, unsigned short>();
    // signedness must be the same if usual arithmetic conversions are not bits-preserving
    test_case_Equal_memcmp_is_safe<is_signed_v<char>, char, signed char>();
    test_case_Equal_memcmp_is_safe<is_signed_v<char>, signed char, char>();
    test_case_Equal_memcmp_is_safe<is_unsigned_v<char>, char, unsigned char>();
    test_case_Equal_memcmp_is_safe<is_unsigned_v<char>, unsigned char, char>();
    test_case_Equal_memcmp_is_safe<false, signed char, unsigned char>();
    test_case_Equal_memcmp_is_safe<false, unsigned char, signed char>();
    test_case_Equal_memcmp_is_safe<false, short, unsigned short>();
    test_case_Equal_memcmp_is_safe<false, unsigned short, short>();
    // but if UACs don't change bits the signedness can differ
    test_case_Equal_memcmp_is_safe<true, int, unsigned int>();
    test_case_Equal_memcmp_is_safe<true, unsigned int, int>();
    test_case_Equal_memcmp_is_safe<true, long, unsigned long>();
    test_case_Equal_memcmp_is_safe<true, unsigned long, long>();
    test_case_Equal_memcmp_is_safe<sizeof(int) == sizeof(long), int, long>();
    test_case_Equal_memcmp_is_safe<sizeof(int) == sizeof(long), long, int>();
    test_case_Equal_memcmp_is_safe<sizeof(int) == sizeof(long), unsigned int, long>();
    test_case_Equal_memcmp_is_safe<sizeof(int) == sizeof(long), unsigned long, int>();
    test_case_Equal_memcmp_is_safe<sizeof(int) == sizeof(long), int, unsigned long>();
    test_case_Equal_memcmp_is_safe<sizeof(int) == sizeof(long), long, unsigned int>();
    test_case_Equal_memcmp_is_safe<sizeof(int) == sizeof(long), unsigned int, unsigned long>();
    test_case_Equal_memcmp_is_safe<sizeof(int) == sizeof(long), unsigned long, unsigned int>();
    test_case_Equal_memcmp_is_safe<true, long long, unsigned long long>();
    test_case_Equal_memcmp_is_safe<true, unsigned long long, long long>();
    // memcmp is safe between bool and other integral types with the same size because we don't care about
    // representations other than 0 and 1
    test_case_Equal_memcmp_is_safe<true, bool, char>();
    test_case_Equal_memcmp_is_safe<true, char, bool>();
    // No enums
    test_case_Equal_memcmp_is_safe<false, bool_enum, bool>();
    test_case_Equal_memcmp_is_safe<false, bool, bool_enum>();
    test_case_Equal_memcmp_is_safe<false, short_enum, short>();
    test_case_Equal_memcmp_is_safe<false, short, short_enum>();
    test_case_Equal_memcmp_is_safe<false, int_enum, int>();
    test_case_Equal_memcmp_is_safe<false, int, int_enum>();
    // No user-defined types
    test_case_Equal_memcmp_is_safe<false, base_class, base_class>();

#ifdef __cpp_lib_byte
    // memcmp is safe for std::byte, but it can't be compared to integral types
    test_case_Equal_memcmp_is_safe<true, byte, byte>();
    test_case_Equal_memcmp_is_safe<false, byte, char>();
    test_case_Equal_memcmp_is_safe<false, char, byte>();
#endif // __cpp_lib_byte

    // Pointers to cv T are OK (they *point to* volatile stuff, they aren't volatile themselves)
    typedef void (*funcptr_t)(int);
    test_case_Equal_memcmp_is_safe<true, void*, void*>();
    test_case_Equal_memcmp_is_safe<true, void*, const void*>();
    test_case_Equal_memcmp_is_safe<true, void*, volatile void*>();
    test_case_Equal_memcmp_is_safe<true, void*, const volatile void*>();
    test_case_Equal_memcmp_is_safe<true, const void*, void*>();
    test_case_Equal_memcmp_is_safe<true, volatile void*, void*>();
    test_case_Equal_memcmp_is_safe<true, const volatile void*, void*>();
    test_case_Equal_memcmp_is_safe<true, base_class*, base_class*>();
    test_case_Equal_memcmp_is_safe<true, base_class*, const base_class*>();
    test_case_Equal_memcmp_is_safe<true, base_class*, volatile base_class*>();
    test_case_Equal_memcmp_is_safe<true, base_class*, const volatile base_class*>();
    test_case_Equal_memcmp_is_safe<true, funcptr_t, funcptr_t>();

    // Pointers to not-the-same-type need to go to the general algorithm
    test_case_Equal_memcmp_is_safe<false, base_class*, derived_class*>();

    // Technically pointers to cv-void and any other object pointer should be OK, but the
    // metaprogramming shouldn't attempt to handle that case because detecting function pointer
    // types is not worth it
    test_case_Equal_memcmp_is_safe<false, void*, int*>();
    test_case_Equal_memcmp_is_safe<false, void*, funcptr_t>();
}

template <class Expected, class Ptr1, class Ptr2, class Pr>
void test_case_Lex_compare_optimize_helper() {
    assert_same<_Lex_compare_optimize<Expected>,
        decltype(_Lex_compare_memcmp_classify(declval<Ptr1>(), declval<Ptr2>(), declval<Pr>()))>();
}

template <class Expected, class Elem1, class Elem2, class Pr>
void test_case_Lex_compare_optimize() {
    // same cv-qualifiers song and dance as test_case_Equal_memcmp_is_safe
    test_case_Lex_compare_optimize_helper<Expected, Elem1*, Elem2*, Pr>();
    test_case_Lex_compare_optimize_helper<Expected, const Elem1*, Elem2*, Pr>();
    test_case_Lex_compare_optimize_helper<Expected, Elem1*, const Elem2*, Pr>();
    test_case_Lex_compare_optimize_helper<Expected, const Elem1*, const Elem2*, Pr>();
    test_case_Lex_compare_optimize_helper<void, volatile Elem1*, Elem2*, Pr>();
    test_case_Lex_compare_optimize_helper<void, Elem1*, volatile Elem2*, Pr>();
    test_case_Lex_compare_optimize_helper<void, volatile Elem1*, volatile Elem2*, Pr>();
    test_case_Lex_compare_optimize_helper<void, const volatile Elem1*, Elem2*, Pr>();
    test_case_Lex_compare_optimize_helper<void, const Elem1*, volatile Elem2*, Pr>();
    test_case_Lex_compare_optimize_helper<void, const volatile Elem1*, volatile Elem2*, Pr>();
    test_case_Lex_compare_optimize_helper<void, volatile Elem1*, const Elem2*, Pr>();
    test_case_Lex_compare_optimize_helper<void, Elem1*, const volatile Elem2*, Pr>();
    test_case_Lex_compare_optimize_helper<void, volatile Elem1*, const volatile Elem2*, Pr>();
    test_case_Lex_compare_optimize_helper<void, const volatile Elem1*, const Elem2*, Pr>();
    test_case_Lex_compare_optimize_helper<void, const Elem1*, const volatile Elem2*, Pr>();
    test_case_Lex_compare_optimize_helper<void, const volatile Elem1*, const volatile Elem2*, Pr>();

    // non-pointer iterators should get the general implementation
    test_case_Lex_compare_optimize_helper<void, typename list<Elem1>::iterator, typename list<Elem2>::iterator, Pr>();
}

template <template <class> class Pr>
void test_case_Lex_compare_optimize_pr() {
    typedef Pr<void> transparent_fn;
    typedef conditional_t<is_signed_v<char>, void, Pr<int>> char_opt_fn;

    test_case_Lex_compare_optimize<char_opt_fn, char, char, transparent_fn>();
    test_case_Lex_compare_optimize<char_opt_fn, unsigned char, char, transparent_fn>();
    test_case_Lex_compare_optimize<char_opt_fn, char, unsigned char, transparent_fn>();
    test_case_Lex_compare_optimize<Pr<int>, unsigned char, unsigned char, transparent_fn>();
    test_case_Lex_compare_optimize<void, signed char, signed char, transparent_fn>();
    test_case_Lex_compare_optimize<void, base_class, base_class, transparent_fn>();

    test_case_Lex_compare_optimize<Pr<int>, char, char, Pr<unsigned char>>();
    test_case_Lex_compare_optimize<Pr<int>, unsigned char, char, Pr<unsigned char>>();
    test_case_Lex_compare_optimize<Pr<int>, char, unsigned char, Pr<unsigned char>>();
    test_case_Lex_compare_optimize<Pr<int>, unsigned char, unsigned char, Pr<unsigned char>>();
    test_case_Lex_compare_optimize<Pr<int>, signed char, signed char, Pr<unsigned char>>();
    test_case_Lex_compare_optimize<void, base_class, base_class, Pr<unsigned char>>();

    test_case_Lex_compare_optimize<void, char, char, Pr<signed char>>();
    test_case_Lex_compare_optimize<void, unsigned char, char, Pr<signed char>>();
    test_case_Lex_compare_optimize<void, char, unsigned char, Pr<signed char>>();
    test_case_Lex_compare_optimize<void, unsigned char, unsigned char, Pr<signed char>>();
    test_case_Lex_compare_optimize<void, signed char, signed char, Pr<signed char>>();
    test_case_Lex_compare_optimize<void, base_class, base_class, Pr<signed char>>();

    test_case_Lex_compare_optimize<char_opt_fn, char, char, Pr<char>>();
    test_case_Lex_compare_optimize<char_opt_fn, unsigned char, char, Pr<char>>();
    test_case_Lex_compare_optimize<char_opt_fn, char, unsigned char, Pr<char>>();
    test_case_Lex_compare_optimize<char_opt_fn, unsigned char, unsigned char, Pr<char>>();
    test_case_Lex_compare_optimize<char_opt_fn, signed char, signed char, Pr<char>>();
    test_case_Lex_compare_optimize<void, base_class, base_class, Pr<char>>();
}

void test_Lex_compare_optimize() {
    test_case_Lex_compare_optimize_pr<less>();
    test_case_Lex_compare_optimize_pr<greater>();
}
