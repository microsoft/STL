// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _HAS_DEPRECATED_ADAPTOR_TYPEDEFS 1
#define _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING

#include <bitset>
#include <cassert>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <typeindex>
#include <utility>
#include <variant>
#include <vector>

#ifndef _M_CEE_PURE
#include <thread>
#endif // _M_CEE_PURE

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <typename T, typename = void>
struct hash_callable : false_type {};

template <typename T>
struct hash_callable<T, void_t<decltype(declval<hash<T>>()(declval<const T&>()))>> : true_type {};

template <typename T>
constexpr bool hash_disabled() {
    return !is_default_constructible_v<hash<T>> //
        && !is_copy_constructible_v<hash<T>> //
        && !is_move_constructible_v<hash<T>> //
        && !is_copy_assignable_v<hash<T>> //
        && !is_move_assignable_v<hash<T>> //
        && !hash_callable<T>::value;
}

template <typename T, bool NoExcept = true>
constexpr bool standard_hash_enabled() {
    // technically the standard doesn't require triviality here, but it's easy
    // for us to provide, so we should do that.
    return is_trivially_default_constructible_v<hash<T>> //
        && is_trivially_copy_constructible_v<hash<T>> //
        && is_trivially_move_constructible_v<hash<T>> //
        && is_trivially_copy_assignable_v<hash<T>> //
        && is_trivially_move_assignable_v<hash<T>> //
        && is_trivial_v<hash<T>> // as a consequence of the above
        && is_same_v<typename hash<T>::argument_type, T> //
        && is_same_v<typename hash<T>::result_type, size_t> //
        && (noexcept(hash<T>{}(declval<const T&>())) == NoExcept) //
        && hash_disabled<const T>() //
        && hash_disabled<volatile T>() //
        && hash_disabled<const volatile T>();
}

STATIC_ASSERT(standard_hash_enabled<bool>());
STATIC_ASSERT(standard_hash_enabled<char>());
STATIC_ASSERT(standard_hash_enabled<signed char>());
STATIC_ASSERT(standard_hash_enabled<unsigned char>());
STATIC_ASSERT(standard_hash_enabled<wchar_t>());
#ifdef __cpp_char8_t
STATIC_ASSERT(standard_hash_enabled<char8_t>());
#endif // __cpp_char8_t
STATIC_ASSERT(standard_hash_enabled<char16_t>());
STATIC_ASSERT(standard_hash_enabled<char32_t>());
STATIC_ASSERT(standard_hash_enabled<short>());
STATIC_ASSERT(standard_hash_enabled<unsigned short>());
STATIC_ASSERT(standard_hash_enabled<int>());
STATIC_ASSERT(standard_hash_enabled<unsigned int>());
STATIC_ASSERT(standard_hash_enabled<long>());
STATIC_ASSERT(standard_hash_enabled<unsigned long>());
STATIC_ASSERT(standard_hash_enabled<long long>());
STATIC_ASSERT(standard_hash_enabled<unsigned long long>());
STATIC_ASSERT(standard_hash_enabled<float>());
STATIC_ASSERT(standard_hash_enabled<double>());
STATIC_ASSERT(standard_hash_enabled<long double>());
STATIC_ASSERT(standard_hash_enabled<void*>());
STATIC_ASSERT(standard_hash_enabled<int*>());
STATIC_ASSERT(standard_hash_enabled<int (*)(int)>());
STATIC_ASSERT(standard_hash_enabled<nullptr_t>());

STATIC_ASSERT(standard_hash_enabled<bitset<32>>());
STATIC_ASSERT(standard_hash_enabled<error_code>());
STATIC_ASSERT(standard_hash_enabled<error_condition>());
STATIC_ASSERT(standard_hash_enabled<type_index>());
STATIC_ASSERT(standard_hash_enabled<vector<bool>>());

#ifndef _M_CEE_PURE
STATIC_ASSERT(standard_hash_enabled<thread::id>());
#endif // _M_CEE_PURE

struct NotHashable {};

struct Hashable {};

namespace std {

    template <>
    struct hash<Hashable> {
        hash() { // non-trivial
        }
        hash(const hash&) { // non-trivial
        }
        hash& operator=(const hash&) { // non-trivial
            return *this;
        }
        size_t operator()(Hashable) {
            return 5925961;
        }
    };

} // namespace std

STATIC_ASSERT(!hash_disabled<Hashable>());
STATIC_ASSERT(hash_disabled<NotHashable>());

enum NonScopedEnum : int { xNonScoped, yNonScoped };
enum class ScopedEnum : int { x, y };

STATIC_ASSERT(standard_hash_enabled<NonScopedEnum>());
STATIC_ASSERT(standard_hash_enabled<ScopedEnum>());

void test_enum_hash_invariants() {
    hash<int> hInt;
    hash<NonScopedEnum> hNonScoped;
    hash<ScopedEnum> hScoped;

    assert(hInt(0) == hNonScoped(xNonScoped));
    assert(hInt(0) == hScoped(ScopedEnum::x));
}

template <typename Float>
void test_float_hash_invariants() {
    // positive and negative zero have the same hash
    Float positiveZero = 0;
    Float negativeZero = -positiveZero;
    hash<Float> hf;
    assert(hf(positiveZero) == hf(negativeZero));
}

void test_pointer_hash_invariants() {
    hash<void*> hVoid;
    hash<nullptr_t> hNullptr;
    assert(hVoid(nullptr) == hNullptr(nullptr));
}

void test_unique_ptr_hash_invariants() {
    STATIC_ASSERT(standard_hash_enabled<unique_ptr<int>>());
    const auto x = make_unique<int>(70);
    assert(hash<unique_ptr<int>>{}(x) == hash<int*>{}(x.get()));
}

void test_shared_ptr_hash_invariants() {
    STATIC_ASSERT(standard_hash_enabled<shared_ptr<int>>());
    const auto x = make_shared<int>(70);
    assert(hash<shared_ptr<int>>{}(x) == hash<int*>{}(x.get()));
}

template <class CharT>
void test_string_T_hash_invariants(const CharT* const exampleStr) {
    STATIC_ASSERT(standard_hash_enabled<basic_string<CharT>>());
#if _HAS_CXX17
    static_assert(standard_hash_enabled<basic_string_view<CharT>>());

    hash<basic_string<CharT>> hString;
    hash<basic_string_view<CharT>> hStringView;
    assert(hString(exampleStr) == hStringView(exampleStr));
#else // ^^^ _HAS_CXX17 / !_HAS_CXX17 vvv
    (void) exampleStr;
#endif // _HAS_CXX17
}

void test_string_hash_invariants() {
    test_string_T_hash_invariants("example");
    test_string_T_hash_invariants(L"example");
    test_string_T_hash_invariants(u"example");
    test_string_T_hash_invariants(U"example");
}

void test_optional_hash_invariants() {
#if _HAS_CXX17
    static_assert(hash_disabled<optional<NotHashable>>());
    static_assert(hash_disabled<optional<const NotHashable>>());
    static_assert(standard_hash_enabled<optional<Hashable>, false>());
    static_assert(standard_hash_enabled<optional<const Hashable>, false>());
    static_assert(hash_disabled<optional<volatile Hashable>>());
    static_assert(hash_disabled<optional<const volatile Hashable>>());
    static_assert(standard_hash_enabled<optional<int>>()); // Tests for strengthened noexcept
    static_assert(standard_hash_enabled<optional<const int>>()); // Tests for strengthened noexcept

    hash<optional<Hashable>> hOptHashable;
    hash<optional<int>> hOptInt;
    hash<optional<const int>> hOptConstInt;

    optional<int> disengagedInt;
    optional<const int> disengagedConstInt;
    optional<int> engagedInt(42);
    optional<const int> engagedConstInt(42);

    // all disengaged optionals have the same unspecified value hash
    // (may technically be an implementation assumption)
    assert(hOptHashable(optional<Hashable>{}) == hOptInt(disengagedInt));
    assert(hOptHashable(optional<Hashable>{}) == hOptInt(disengagedConstInt));

    // all engaged optionals forward to hash<remove_const_t<T>>
    assert(hOptHashable(optional<Hashable>(Hashable{})) == 5925961);
    assert(hOptInt(engagedInt) == hOptConstInt(engagedConstInt));
#endif // _HAS_CXX17
}

void test_variant_hash_invariants() {
#if _HAS_CXX17
    static_assert(standard_hash_enabled<variant<int, double>>()); // Tests for strengthened noexcept
    static_assert(hash_disabled<variant<NotHashable, NotHashable>>());
    static_assert(hash_disabled<variant<NotHashable, Hashable>>());
    static_assert(hash_disabled<variant<Hashable, NotHashable>>());
    static_assert(standard_hash_enabled<variant<Hashable, Hashable>, false>());
    static_assert(standard_hash_enabled<variant<const Hashable, Hashable>, false>());
    static_assert(hash_disabled<variant<volatile Hashable, Hashable>>());
    static_assert(hash_disabled<variant<const volatile Hashable, Hashable>>());
    static_assert(standard_hash_enabled<monostate>());

    {
        variant<int, const int, double, const double> vInt(in_place_index<0>, 1234);
        variant<int, const int, double, const double> vConstInt(in_place_index<1>, 1234);
        variant<int, const int, double, const double> vDouble(in_place_index<2>, 42.0);
        variant<int, const int, double, const double> vConstDouble(in_place_index<3>, 42.0);

        hash<variant<int, const int, double, const double>> hV;
        assert(hV(vInt) == hV(vConstInt));
        assert(hV(vDouble) == hV(vConstDouble));
    }

    {
        variant<int, Hashable> vInt(in_place_index<0>, 1234);
        variant<int, Hashable> vHashable(in_place_index<1>);
        hash<variant<int, Hashable>> hV;
        assert(hV(vInt) == hash<int>{}(1234));
        assert(hV(vHashable) == 5925961);
    }
#endif // _HAS_CXX17
}

int main() {
    test_enum_hash_invariants();

    test_float_hash_invariants<float>();
    test_float_hash_invariants<double>();
    test_float_hash_invariants<long double>();

    test_pointer_hash_invariants();
    test_unique_ptr_hash_invariants();
    test_shared_ptr_hash_invariants();

    test_string_hash_invariants();
    test_optional_hash_invariants();
    test_variant_hash_invariants();
}
