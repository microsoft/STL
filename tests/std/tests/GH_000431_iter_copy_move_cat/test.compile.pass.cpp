// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <list>
#include <type_traits>
#include <vector>

#if _HAS_CXX17
#include <string_view>
#endif // _HAS_CXX17

#if _HAS_CXX20
#include <span>
#endif // _HAS_CXX20

#pragma warning(disable : 4984) // 'if constexpr' is a C++17 language extension
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++17-extensions" // constexpr if is a C++17 extension
#endif // __clang__

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <bool ExpectedConstructible, bool ExpectedAssignable, class IterCat>
void assert_iter_cat() {
    STATIC_ASSERT(IterCat::_Bitcopy_constructible == ExpectedConstructible);
    STATIC_ASSERT(IterCat::_Bitcopy_assignable == ExpectedAssignable);
}

template <bool ExpectedCopyConstructible, bool ExpectedMoveConstructible, bool ExpectedCopyAssignable,
    bool ExpectedMoveAssignable, class SourceIt, class DestIt>
void test_iter_cat() {
    assert_iter_cat<ExpectedCopyConstructible, ExpectedCopyAssignable, _Iter_copy_cat<SourceIt, DestIt>>();
    assert_iter_cat<ExpectedMoveConstructible, ExpectedMoveAssignable,
        _Iter_copy_cat<move_iterator<SourceIt>, DestIt>>();
    assert_iter_cat<ExpectedMoveConstructible, ExpectedMoveAssignable, _Iter_move_cat<SourceIt, DestIt>>();
    assert_iter_cat<ExpectedMoveConstructible, ExpectedMoveAssignable,
        _Iter_move_cat<move_iterator<SourceIt>, DestIt>>();
}

template <bool ExpectedCopyConstructible, bool ExpectedMoveConstructible, bool ExpectedCopyAssignable,
    bool ExpectedMoveAssignable, class SourceIt, class ConstSourceIt, class DestIt, class ConstDestIt>
void test_iter_cat_for_iterators() {
    test_iter_cat<ExpectedCopyConstructible, ExpectedMoveConstructible, ExpectedCopyAssignable, ExpectedMoveAssignable,
        SourceIt, DestIt>();
    test_iter_cat<ExpectedCopyConstructible, ExpectedCopyConstructible, ExpectedCopyAssignable, ExpectedCopyAssignable,
        ConstSourceIt, DestIt>();

    // Assigning to ConstDestIt doesn't make any sense but constructing is fine
    test_iter_cat<ExpectedCopyConstructible, ExpectedMoveConstructible, false, false, SourceIt, ConstDestIt>();
    test_iter_cat<ExpectedCopyConstructible, ExpectedCopyConstructible, false, false, ConstSourceIt, ConstDestIt>();
}

template <bool ExpectedCopyConstructible, bool ExpectedMoveConstructible, bool ExpectedCopyAssignable,
    bool ExpectedMoveAssignable, class Source, class Dest = Source>
void test_iter_cat_for_types() {
    test_iter_cat_for_iterators<ExpectedCopyConstructible, ExpectedMoveConstructible, ExpectedCopyAssignable,
        ExpectedMoveAssignable, Source*, const Source*, Dest*, const Dest*>();
}

template <bool Expected, class Source, class Dest>
void test_iter_cat_for_trivially_copyable_types() {
    test_iter_cat_for_types<Expected, Expected, Expected, Expected, Source, Dest>();
}

template <bool Expected, class Int1, class Int2>
void test_iter_cat_for_integrals_helper() {
    enum Enum1 : Int1 {};
    enum Enum2 : Int2 {};
    enum class EnumClass1 : Int1 {};
    enum class EnumClass2 : Int2 {};

    test_iter_cat_for_trivially_copyable_types<Expected, Int1, Int2>();

    test_iter_cat_for_trivially_copyable_types<Expected, Enum1, Int2>();

    if constexpr (is_same_v<Int1, Int2>) {
        // Allow enum and enum class
        test_iter_cat_for_trivially_copyable_types<true, Enum1, Enum1>();
        test_iter_cat_for_trivially_copyable_types<true, EnumClass1, EnumClass1>();
    }

    // Don't allow conversion from int to enum (since that requires a cast):
    test_iter_cat_for_trivially_copyable_types<false, Int1, Enum2>();

    // Similarly, don't allow enum class types to be implicitly converted to/from integral types
    test_iter_cat_for_trivially_copyable_types<false, Int1, EnumClass2>();
    test_iter_cat_for_trivially_copyable_types<false, EnumClass1, Int2>();

    // Don't allow conversions between enums
    test_iter_cat_for_trivially_copyable_types<false, Enum1, Enum2>();
    test_iter_cat_for_trivially_copyable_types<false, EnumClass1, EnumClass2>();
    test_iter_cat_for_trivially_copyable_types<false, Enum1, EnumClass2>();
    test_iter_cat_for_trivially_copyable_types<false, EnumClass1, Enum2>();
}

template <bool Expected, class Int1, class Int2>
void test_iter_cat_for_integrals() {
    test_iter_cat_for_integrals_helper<Expected, Int1, Int2>();

    if constexpr (!is_same_v<Int1, bool>) {
        using UInt1 = make_unsigned_t<Int1>;

        // Signedness shouldn't change answer
        test_iter_cat_for_integrals_helper<Expected, UInt1, Int2>();
    }

    if constexpr (!is_same_v<Int2, bool>) {
        using UInt2 = make_unsigned_t<Int2>;

        // Signedness shouldn't change answer
        test_iter_cat_for_integrals_helper<Expected, Int1, UInt2>();
    }

    if constexpr (!is_same_v<Int1, bool> && !is_same_v<Int2, bool>) {
        using UInt1 = make_unsigned_t<Int1>;
        using UInt2 = make_unsigned_t<Int2>;

        // Signedness shouldn't change answer
        test_iter_cat_for_integrals_helper<Expected, UInt1, UInt2>();
    }
}

template <bool Expected, class Char1, class Char2>
void test_iter_cat_for_char_types() {
    using SInt1 = make_signed_t<Char1>;
    using UInt1 = make_unsigned_t<Char1>;
    using SInt2 = make_signed_t<Char2>;
    using UInt2 = make_unsigned_t<Char2>;

    test_iter_cat_for_integrals_helper<Expected, Char1, Char2>();
    test_iter_cat_for_integrals_helper<Expected, SInt1, Char2>();
    test_iter_cat_for_integrals_helper<Expected, UInt1, Char2>();
    test_iter_cat_for_integrals_helper<Expected, Char1, SInt2>();
    test_iter_cat_for_integrals_helper<Expected, Char1, UInt2>();
}

template <bool Expected, class Elem1, class Elem2>
void test_iter_cat_for_pointers() {
    test_iter_cat_for_trivially_copyable_types<Expected, Elem1*, Elem2*>();
    test_iter_cat_for_trivially_copyable_types<Expected, Elem1*, const Elem2*>();
    test_iter_cat_for_trivially_copyable_types<Expected, const Elem1*, const Elem2*>();

    test_iter_cat_for_trivially_copyable_types<false, const Elem1*, Elem2*>();
}

template <bool Expected, class Elem1, class Elem2>
void test_iter_cat_for_pointers_with_volatile() {
    test_iter_cat_for_pointers<Expected, Elem1, Elem2>();
    test_iter_cat_for_pointers<Expected, Elem1, volatile Elem2>();
    test_iter_cat_for_pointers<Expected, volatile Elem1, volatile Elem2>();

    test_iter_cat_for_pointers<false, volatile Elem1, Elem2>();
}

template <bool Expected, class SourceContainer, class DestContainer>
void test_iter_cat_for_containers() {
    using SourceIt      = typename SourceContainer::iterator;
    using ConstSourceIt = typename SourceContainer::const_iterator;
    using DestIt        = typename DestContainer::iterator;
    using ConstDestIt   = typename DestContainer::const_iterator;
    test_iter_cat_for_iterators<Expected, Expected, Expected, Expected, SourceIt, ConstSourceIt, DestIt, ConstDestIt>();
}

struct TrivialStruct {
    int i;
};
STATIC_ASSERT(is_trivial_v<TrivialStruct>);

struct TriviallyCopyableStruct {
    int i;
    TriviallyCopyableStruct();
};
STATIC_ASSERT(is_trivially_copyable_v<TriviallyCopyableStruct>);
STATIC_ASSERT(!is_trivial_v<TriviallyCopyableStruct>);

struct TriviallyMovableStruct {
    int i;
    TriviallyMovableStruct();
    TriviallyMovableStruct(const TriviallyMovableStruct&)            = delete;
    TriviallyMovableStruct(TriviallyMovableStruct&&)                 = default;
    TriviallyMovableStruct& operator=(const TriviallyMovableStruct&) = delete;
    TriviallyMovableStruct& operator=(TriviallyMovableStruct&&)      = default;
};
STATIC_ASSERT(is_trivially_copyable_v<TriviallyMovableStruct>);
STATIC_ASSERT(!is_trivial_v<TriviallyMovableStruct>);

struct TriviallyConstructibleStruct {
    int i;
    TriviallyConstructibleStruct();
    TriviallyConstructibleStruct(const TriviallyConstructibleStruct&) = default;
    TriviallyConstructibleStruct(TriviallyConstructibleStruct&&)      = default;

    TriviallyConstructibleStruct& operator=(const TriviallyConstructibleStruct&);
    TriviallyConstructibleStruct& operator=(TriviallyConstructibleStruct&&);
};
STATIC_ASSERT(is_trivially_copy_constructible_v<TriviallyConstructibleStruct>);
STATIC_ASSERT(is_trivially_move_constructible_v<TriviallyConstructibleStruct>);
STATIC_ASSERT(!is_trivially_copy_assignable_v<TriviallyConstructibleStruct>);
STATIC_ASSERT(!is_trivially_move_assignable_v<TriviallyConstructibleStruct>);
STATIC_ASSERT(!is_trivially_copyable_v<TriviallyConstructibleStruct>);
STATIC_ASSERT(!is_trivial_v<TriviallyConstructibleStruct>);

struct TriviallyAssignableStruct {
    int i;
    TriviallyAssignableStruct();
    TriviallyAssignableStruct(const TriviallyAssignableStruct&);
    TriviallyAssignableStruct(TriviallyAssignableStruct&&);

    TriviallyAssignableStruct& operator=(const TriviallyAssignableStruct&) = default;
    TriviallyAssignableStruct& operator=(TriviallyAssignableStruct&&)      = default;
};
STATIC_ASSERT(!is_trivially_copy_constructible_v<TriviallyAssignableStruct>);
STATIC_ASSERT(!is_trivially_move_constructible_v<TriviallyAssignableStruct>);
STATIC_ASSERT(is_trivially_copy_assignable_v<TriviallyAssignableStruct>);
STATIC_ASSERT(is_trivially_move_assignable_v<TriviallyAssignableStruct>);
STATIC_ASSERT(!is_trivially_copyable_v<TriviallyAssignableStruct>);
STATIC_ASSERT(!is_trivial_v<TriviallyAssignableStruct>);

struct TriviallyCopyConstructibleStruct {
    int i;
    TriviallyCopyConstructibleStruct();
    TriviallyCopyConstructibleStruct(const TriviallyCopyConstructibleStruct&) = default;
    TriviallyCopyConstructibleStruct(TriviallyCopyConstructibleStruct&&);

    TriviallyCopyConstructibleStruct& operator=(const TriviallyCopyConstructibleStruct&);
    TriviallyCopyConstructibleStruct& operator=(TriviallyCopyConstructibleStruct&&);
};
STATIC_ASSERT(is_trivially_copy_constructible_v<TriviallyCopyConstructibleStruct>);
STATIC_ASSERT(!is_trivially_move_constructible_v<TriviallyCopyConstructibleStruct>);
STATIC_ASSERT(!is_trivially_copy_assignable_v<TriviallyCopyConstructibleStruct>);
STATIC_ASSERT(!is_trivially_move_assignable_v<TriviallyCopyConstructibleStruct>);
STATIC_ASSERT(!is_trivially_copyable_v<TriviallyCopyConstructibleStruct>);
STATIC_ASSERT(!is_trivial_v<TriviallyCopyConstructibleStruct>);

struct TriviallyCopyAssignableStruct {
    int i;
    TriviallyCopyAssignableStruct();
    TriviallyCopyAssignableStruct(const TriviallyCopyAssignableStruct&);
    TriviallyCopyAssignableStruct(TriviallyCopyAssignableStruct&&);

    TriviallyCopyAssignableStruct& operator=(const TriviallyCopyAssignableStruct&) = default;

    TriviallyCopyAssignableStruct& operator=(TriviallyCopyAssignableStruct&&);
};
STATIC_ASSERT(!is_trivially_copy_constructible_v<TriviallyCopyAssignableStruct>);
STATIC_ASSERT(!is_trivially_move_constructible_v<TriviallyCopyAssignableStruct>);
STATIC_ASSERT(is_trivially_copy_assignable_v<TriviallyCopyAssignableStruct>);
STATIC_ASSERT(!is_trivially_move_assignable_v<TriviallyCopyAssignableStruct>);
STATIC_ASSERT(!is_trivially_copyable_v<TriviallyCopyAssignableStruct>);
STATIC_ASSERT(!is_trivial_v<TriviallyCopyAssignableStruct>);

struct TriviallyMoveConstructibleStruct {
    int i;
    TriviallyMoveConstructibleStruct();
    TriviallyMoveConstructibleStruct(const TriviallyMoveConstructibleStruct&);
    TriviallyMoveConstructibleStruct(TriviallyMoveConstructibleStruct&&) = default;

    TriviallyMoveConstructibleStruct& operator=(const TriviallyMoveConstructibleStruct&);
    TriviallyMoveConstructibleStruct& operator=(TriviallyMoveConstructibleStruct&&);
};
STATIC_ASSERT(!is_trivially_copy_constructible_v<TriviallyMoveConstructibleStruct>);
STATIC_ASSERT(is_trivially_move_constructible_v<TriviallyMoveConstructibleStruct>);
STATIC_ASSERT(!is_trivially_copy_assignable_v<TriviallyMoveConstructibleStruct>);
STATIC_ASSERT(!is_trivially_move_assignable_v<TriviallyMoveConstructibleStruct>);
STATIC_ASSERT(!is_trivially_copyable_v<TriviallyMoveConstructibleStruct>);
STATIC_ASSERT(!is_trivial_v<TriviallyMoveConstructibleStruct>);

struct TriviallyMoveAssignableStruct {
    int i;
    TriviallyMoveAssignableStruct();
    TriviallyMoveAssignableStruct(const TriviallyMoveAssignableStruct&);
    TriviallyMoveAssignableStruct(TriviallyMoveAssignableStruct&&);

    TriviallyMoveAssignableStruct& operator=(const TriviallyMoveAssignableStruct&);
    TriviallyMoveAssignableStruct& operator=(TriviallyMoveAssignableStruct&&) = default;
};
STATIC_ASSERT(!is_trivially_copy_constructible_v<TriviallyMoveAssignableStruct>);
STATIC_ASSERT(!is_trivially_move_constructible_v<TriviallyMoveAssignableStruct>);
STATIC_ASSERT(!is_trivially_copy_assignable_v<TriviallyMoveAssignableStruct>);
STATIC_ASSERT(is_trivially_move_assignable_v<TriviallyMoveAssignableStruct>);
STATIC_ASSERT(!is_trivially_copyable_v<TriviallyMoveAssignableStruct>);
STATIC_ASSERT(!is_trivial_v<TriviallyMoveAssignableStruct>);

struct EmptyBase {};

struct EmptyDerived : EmptyBase {};

struct EmptyPrivatelyDerived : private EmptyBase {};

struct StatefulBase {
    int i;
};

struct StatefulDerived : StatefulBase, EmptyBase {};

struct StatefulPrivatelyDerived : private StatefulBase, private EmptyBase {};

struct StatefulDerived2 : EmptyBase, StatefulBase {};

struct StatefulPrivatelyDerived2 : private EmptyBase, private StatefulBase {};

#ifdef __cpp_lib_is_pointer_interconvertible
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<EmptyBase, EmptyDerived>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<EmptyBase, EmptyPrivatelyDerived>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<StatefulBase, StatefulDerived>);
STATIC_ASSERT(!is_pointer_interconvertible_base_of_v<EmptyBase, StatefulDerived>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<StatefulBase, StatefulPrivatelyDerived>);
STATIC_ASSERT(!is_pointer_interconvertible_base_of_v<EmptyBase, StatefulPrivatelyDerived>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<StatefulBase, StatefulDerived2>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<EmptyBase, StatefulDerived2>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<StatefulBase, StatefulPrivatelyDerived2>);
STATIC_ASSERT(is_pointer_interconvertible_base_of_v<EmptyBase, StatefulPrivatelyDerived2>);
#endif // __cpp_lib_is_pointer_interconvertible


void iter_cat_test_cases() {

    // Test compatible integral types
    test_iter_cat_for_integrals<true, bool, bool>();
    test_iter_cat_for_integrals<true, signed char, signed char>();
    test_iter_cat_for_integrals<true, short, short>();
    test_iter_cat_for_integrals<true, int, int>();
    test_iter_cat_for_integrals<true, long, long>();
    test_iter_cat_for_integrals<true, long long, long long>();

    test_iter_cat_for_integrals<sizeof(int) == sizeof(long), int, long>();
    test_iter_cat_for_integrals<sizeof(int) == sizeof(long), long, int>();

    test_iter_cat_for_char_types<true, char, char>();
    test_iter_cat_for_char_types<true, wchar_t, wchar_t>();
#ifdef __cpp_lib_char8_t
    test_iter_cat_for_char_types<true, char8_t, char8_t>();
#endif // __cpp_lib_char8_t
    test_iter_cat_for_char_types<true, char16_t, char16_t>();
    test_iter_cat_for_char_types<true, char32_t, char32_t>();

    // Test bool conversions
    test_iter_cat_for_integrals<true, bool, char>();
    test_iter_cat_for_integrals<false, char, bool>();
    test_iter_cat_for_integrals<false, bool, int>();
    test_iter_cat_for_integrals<false, int, bool>();

    // Don't allow conversions if sizes differ
    test_iter_cat_for_integrals<false, short, int>();
    test_iter_cat_for_integrals<false, int, short>();
    test_iter_cat_for_integrals<false, long, long long>();
    test_iter_cat_for_integrals<false, long long, long>();

#ifdef __cpp_lib_byte
    // Test std::byte
    test_iter_cat_for_trivially_copyable_types<true, byte, byte>();
#endif // __cpp_lib_byte

    // Test floating point
    test_iter_cat_for_trivially_copyable_types<true, float, float>();
    test_iter_cat_for_trivially_copyable_types<true, double, double>();
    test_iter_cat_for_trivially_copyable_types<false, float, double>();
    test_iter_cat_for_trivially_copyable_types<false, double, float>();

    test_iter_cat_for_trivially_copyable_types<sizeof(double) == sizeof(long double), double, long double>();
    test_iter_cat_for_trivially_copyable_types<sizeof(double) == sizeof(long double), long double, double>();

    test_iter_cat_for_trivially_copyable_types<false, int, float>();
    test_iter_cat_for_trivially_copyable_types<false, float, int>();
    test_iter_cat_for_trivially_copyable_types<false, long long, double>();
    test_iter_cat_for_trivially_copyable_types<false, double, long long>();
    test_iter_cat_for_trivially_copyable_types<false, long long, long double>();
    test_iter_cat_for_trivially_copyable_types<false, long double, long long>();

    // Test pointers
    test_iter_cat_for_pointers_with_volatile<true, int, int>();
    test_iter_cat_for_pointers<false, int, long>();
    test_iter_cat_for_pointers<false, unsigned int, int>();

    // Test void conversions
    test_iter_cat_for_pointers_with_volatile<true, void, void>();
    test_iter_cat_for_pointers_with_volatile<true, int, void>();
    test_iter_cat_for_pointers<false, void, int>();

    // Test pointers to derived classes
    constexpr bool enable_derived_to_base =
#ifdef __cpp_lib_is_pointer_interconvertible
        true
#else // ^^^ __cpp_lib_is_pointer_interconvertible ^^^ / vvv !__cpp_lib_is_pointer_interconvertible vvv
        false
#endif // ^^^ !__cpp_lib_is_pointer_interconvertible ^^^
        ;

    test_iter_cat_for_pointers<true, EmptyBase, EmptyBase>();
    test_iter_cat_for_pointers<true, EmptyDerived, EmptyDerived>();
    test_iter_cat_for_pointers<enable_derived_to_base, EmptyDerived, EmptyBase>();
    test_iter_cat_for_pointers<false, EmptyBase, EmptyDerived>();
    test_iter_cat_for_pointers<enable_derived_to_base, StatefulDerived, StatefulBase>();
    test_iter_cat_for_pointers<false, StatefulDerived, EmptyBase>();
    test_iter_cat_for_pointers<enable_derived_to_base, StatefulDerived2, StatefulBase>();
    test_iter_cat_for_pointers<enable_derived_to_base, StatefulDerived2, EmptyBase>();

    // Don't allow privately derived
    test_iter_cat_for_pointers<false, EmptyPrivatelyDerived, EmptyBase>();
    test_iter_cat_for_pointers<false, StatefulPrivatelyDerived, StatefulBase>();
    test_iter_cat_for_pointers<false, StatefulPrivatelyDerived2, StatefulBase>();
    test_iter_cat_for_pointers<false, StatefulPrivatelyDerived2, EmptyBase>();

    // Test function pointers
    test_iter_cat_for_trivially_copyable_types<true, void (*)(int), void (*)(int)>();
    test_iter_cat_for_trivially_copyable_types<false, void (*)(int), void (*)()>();

    // Converting from function pointers to void pointers is a non-standard extension
    test_iter_cat_for_trivially_copyable_types<is_convertible_v<void (*)(int), void*>, void (*)(int), void*>();
    test_iter_cat_for_trivially_copyable_types<false, void*, void (*)(int)>();

    // Test member object pointers
    test_iter_cat_for_trivially_copyable_types<true, int EmptyBase::*, int EmptyBase::*>();
    test_iter_cat_for_trivially_copyable_types<true, int EmptyDerived::*, int EmptyDerived::*>();
    test_iter_cat_for_trivially_copyable_types<false, int EmptyBase::*, int EmptyDerived::*>();
    test_iter_cat_for_trivially_copyable_types<false, int EmptyDerived::*, int EmptyBase::*>();

    // Test member function pointers
    test_iter_cat_for_trivially_copyable_types<true, int (EmptyBase::*)(), int (EmptyBase::*)()>();
    test_iter_cat_for_trivially_copyable_types<true, int (EmptyDerived::*)(), int (EmptyDerived::*)()>();
    test_iter_cat_for_trivially_copyable_types<false, int (EmptyBase::*)(), int (EmptyDerived::*)()>();
    test_iter_cat_for_trivially_copyable_types<false, int (EmptyDerived::*)(), int (EmptyBase::*)()>();

#if _HAS_CXX17
    test_iter_cat_for_trivially_copyable_types<true, string_view, string_view>();
    test_iter_cat_for_trivially_copyable_types<false, string_view, wstring_view>();
#endif // _HAS_CXX17

#if _HAS_CXX20
    test_iter_cat_for_trivially_copyable_types<true, span<int>, span<int>>();
    test_iter_cat_for_trivially_copyable_types<true, span<int, 7>, span<int, 7>>();
    test_iter_cat_for_trivially_copyable_types<false, span<int>, span<int, 7>>();
    test_iter_cat_for_trivially_copyable_types<false, span<int, 7>, span<int>>();
    test_iter_cat_for_trivially_copyable_types<false, span<int>, span<long>>();
    test_iter_cat_for_trivially_copyable_types<false, span<int, 7>, span<long, 7>>();
#endif // _HAS_CXX20

    test_iter_cat_for_types<true, true, true, true, TrivialStruct>();
    test_iter_cat_for_types<true, true, true, true, TriviallyCopyableStruct>();
    test_iter_cat_for_types<false, true, false, true, TriviallyMovableStruct>();
    test_iter_cat_for_types<true, true, false, false, TriviallyConstructibleStruct>();
    test_iter_cat_for_types<false, false, true, true, TriviallyAssignableStruct>();
    test_iter_cat_for_types<true, false, false, false, TriviallyCopyConstructibleStruct>();
    test_iter_cat_for_types<false, false, true, false, TriviallyCopyAssignableStruct>();
    test_iter_cat_for_types<false, true, false, false, TriviallyMoveConstructibleStruct>();
    test_iter_cat_for_types<false, false, false, true, TriviallyMoveAssignableStruct>();

    // Test different containers
#ifdef __cpp_lib_concepts
    test_iter_cat_for_containers<true, vector<int>, vector<int>>();
    test_iter_cat_for_containers<true, array<int, 8>, array<int, 8>>();
    test_iter_cat_for_containers<true, vector<int>, array<int, 8>>();
    test_iter_cat_for_containers<true, array<int, 8>, vector<int>>();
#endif // __cpp_lib_concepts

    test_iter_cat_for_containers<false, list<int>, list<int>>();
    test_iter_cat_for_containers<false, vector<int>, list<int>>();
    test_iter_cat_for_containers<false, list<int>, vector<int>>();

    // Test double move_iterator
    test_iter_cat<true, true, true, true, move_iterator<int*>, int*>();

#ifdef __cpp_lib_concepts
    // Test counted_iterator
    test_iter_cat<true, true, true, true, counted_iterator<int*>, int*>();
    test_iter_cat<true, true, true, true, int*, counted_iterator<int*>>();
    test_iter_cat<true, true, true, true, counted_iterator<int*>, counted_iterator<int*>>();
#endif // __cpp_lib_concepts

    // No volatile
    test_iter_cat_for_trivially_copyable_types<false, volatile int, int>();
    test_iter_cat_for_trivially_copyable_types<false, int, volatile int>();
    test_iter_cat_for_trivially_copyable_types<false, volatile int, volatile int>();

    test_iter_cat_for_containers<false, array<volatile int, 8>, array<int, 8>>();
    test_iter_cat_for_containers<false, array<int, 8>, array<volatile int, 8>>();
    test_iter_cat_for_containers<false, array<volatile int, 8>, array<volatile int, 8>>();
}

int main() {} // COMPILE-ONLY
