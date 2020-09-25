// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <memory>
#include <stddef.h>
#include <string.h>
#include <string>
#include <type_traits>
#include <utility>

#pragma warning(disable : 4582) // '%s': constructor is not implicitly called
#pragma warning(disable : 4583) // '%s': destructor is not implicitly called

using namespace std;

#ifdef __cpp_lib_concepts
template <class Ty, class... Types>
concept can_std_construct_at = requires(Ty* ptr, Types&&... args) {
    construct_at(ptr, forward<Types>(args)...);
};

template <class Ty, class... Types>
concept can_ranges_construct_at = requires(Ty* ptr, Types&&... args) {
    ranges::construct_at(ptr, forward<Types>(args)...);
};

template <class Ty>
concept can_ranges_destroy_at = requires(Ty* ptr) {
    ranges::destroy_at(ptr);
};

template <class Ty, class... Types>
inline constexpr bool can_construct_at = [] {
    constexpr bool result = can_std_construct_at<Ty, Types...>;
    static_assert(can_ranges_construct_at<Ty, Types...> == result);
    return result;
}();

template <class T, class... Args>
constexpr bool construct_at_noexcept() {
    constexpr bool result = noexcept(construct_at(declval<T*>(), declval<Args>()...));
    static_assert(noexcept(ranges::construct_at(declval<T*>(), declval<Args>()...)) == result);
    return result;
}

template <class T>
constexpr bool destroy_at_noexcept() {
    static_assert(noexcept(destroy_at(declval<T*>())));
    if constexpr (can_ranges_destroy_at<T>) {
        static_assert(noexcept(ranges::destroy_at(declval<T*>())));
    }
    return true;
}
#else // ^^^ Concepts and Ranges / No Concepts or Ranges vvv
template <class Void, class Ty, class... Types>
inline constexpr bool can_construct_at_impl = false;

template <class Ty, class... Types>
inline constexpr bool
    can_construct_at_impl<void_t<decltype(construct_at(declval<Ty*>(), declval<Types>()...))>, Ty, Types...> = true;

template <class Ty, class... Types>
inline constexpr bool can_construct_at = can_construct_at_impl<void, Ty, Types...>;

template <class T, class... Args>
constexpr bool construct_at_noexcept() {
    return noexcept(construct_at(declval<T*>(), declval<Args>()...));
}

template <class T>
constexpr bool destroy_at_noexcept() {
    return noexcept(destroy_at(declval<T*>()));
}
#endif // __cpp_lib_concepts

static_assert(can_construct_at<int>);
static_assert(can_construct_at<const int>);
static_assert(can_construct_at<volatile int>);
static_assert(can_construct_at<const volatile int>);
static_assert(can_construct_at<int, int>);
static_assert(can_construct_at<const int, int>);
static_assert(can_construct_at<volatile int, int>);
static_assert(can_construct_at<const volatile int, int>);
static_assert(can_construct_at<int, int&>);
static_assert(can_construct_at<const int, int&>);
static_assert(can_construct_at<volatile int, int&>);
static_assert(can_construct_at<const volatile int, int&>);

struct X {};

#ifndef __EDG__ // TRANSITION, VSO-1075296
static_assert(!can_construct_at<int, X>);
static_assert(!can_construct_at<X, int>);
#endif // __EDG__

// note that indestructible isn't constructible but is construct_at-ible:
struct indestructible {
    void destroy() {
        this->~indestructible();
    };

private:
    ~indestructible() = default;
};

static_assert(can_construct_at<indestructible>);
static_assert(can_construct_at<const indestructible>);
static_assert(can_construct_at<volatile indestructible>);
static_assert(can_construct_at<const volatile indestructible>);

static_assert(can_construct_at<X>);
static_assert(can_construct_at<X, X>);
static_assert(can_construct_at<X, const X>);
static_assert(can_construct_at<X, const X&>);
static_assert(can_construct_at<X, X&>);

static_assert(can_construct_at<string>);
static_assert(can_construct_at<string, size_t, char>);
static_assert(!can_construct_at<string, size_t, char, char>);
static_assert(!can_construct_at<string, X>);

// The following static_asserts test our strengthening of noexcept

#ifndef __EDG__ // TRANSITION, VSO-1075296
static_assert(construct_at_noexcept<int, int>());
static_assert(construct_at_noexcept<const int, int>());
static_assert(construct_at_noexcept<volatile int, int>());
static_assert(construct_at_noexcept<const volatile int, int>());
#endif // __EDG__

static_assert(!construct_at_noexcept<string, const char (&)[6]>());
static_assert(!construct_at_noexcept<const string, const char (&)[6]>());
static_assert(!construct_at_noexcept<volatile string, const char (&)[6]>());
static_assert(!construct_at_noexcept<const volatile string, const char (&)[6]>());

static_assert(destroy_at_noexcept<int>());
static_assert(destroy_at_noexcept<string>());
static_assert(destroy_at_noexcept<const int>());
static_assert(destroy_at_noexcept<const string>());
static_assert(destroy_at_noexcept<volatile int>());
static_assert(destroy_at_noexcept<volatile string>());
static_assert(destroy_at_noexcept<const volatile int>());
static_assert(destroy_at_noexcept<const volatile string>());

#if _HAS_CXX20
static_assert(destroy_at_noexcept<int[42]>());
static_assert(destroy_at_noexcept<string[42]>());
static_assert(destroy_at_noexcept<const int[42]>());
static_assert(destroy_at_noexcept<const string[42]>());
static_assert(destroy_at_noexcept<volatile int[42]>());
static_assert(destroy_at_noexcept<volatile string[42]>());
static_assert(destroy_at_noexcept<const volatile int[42]>());
static_assert(destroy_at_noexcept<const volatile string[42]>());
#endif // _HAS_CXX20

struct throwing_dtor {
    ~throwing_dtor() noexcept(false) {}
};

static_assert(destroy_at_noexcept<throwing_dtor>());
#if _HAS_CXX20
static_assert(destroy_at_noexcept<throwing_dtor[42]>());
#endif // _HAS_CXX20

#ifdef __cpp_lib_concepts
static_assert(!can_ranges_destroy_at<throwing_dtor>);
static_assert(!can_ranges_destroy_at<throwing_dtor[42]>);
#endif // __cpp_lib_concepts

template <class Ty>
void test_runtime(const Ty& val) {
    alignas(Ty) unsigned char storage[sizeof(Ty)];
    const auto asPtrTy = reinterpret_cast<Ty*>(&storage);
    memset(storage, 42, sizeof(Ty));
    assert(asPtrTy == construct_at(asPtrTy, val));
    assert(*asPtrTy == val);
    destroy_at(asPtrTy);

#ifdef __cpp_lib_concepts
    // test ranges:
    memset(storage, 42, sizeof(Ty));
    assert(asPtrTy == ranges::construct_at(asPtrTy, val));
    assert(*asPtrTy == val);
    ranges::destroy_at(asPtrTy);
#endif // __cpp_lib_concepts

    // test voidify:
    const auto asCv = static_cast<const volatile Ty*>(asPtrTy);
    memset(storage, 42, sizeof(Ty));
    assert(asPtrTy == construct_at(asCv, val));
    assert(const_cast<const Ty&>(*asCv) == val);
    destroy_at(asCv);

#ifdef __cpp_lib_concepts
    memset(storage, 42, sizeof(Ty));
    assert(asPtrTy == ranges::construct_at(asCv, val));
    assert(const_cast<const Ty&>(*asCv) == val);
    ranges::destroy_at(asCv);
#endif // __cpp_lib_concepts
}

template <class T>
void test_array(const T& val) {
    constexpr int N = 42;
    (void) val;

#if _HAS_CXX20
    alignas(T) unsigned char storage[sizeof(T) * N];
    using U        = conditional_t<is_scalar_v<T>, const volatile T, T>;
    const auto ptr = reinterpret_cast<U*>(storage);

    for (auto i = 0; i < N; ++i) {
        construct_at(ptr + i, val);
    }

    destroy_at(reinterpret_cast<U(*)[N]>(ptr));

#ifdef __cpp_lib_concepts
    for (auto i = 0; i < N; ++i) {
        ranges::construct_at(ptr + i, val);
    }

#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-1049320
    ranges::destroy_at(reinterpret_cast<U(*)[N]>(ptr));
#else // ^^^ no workaround / workaround vvv
    ranges::destroy_at(reinterpret_cast<T(*)[N]>(const_cast<T*>(ptr)));
#endif // TRANSITION, VSO-1049320
#endif // __cpp_lib_concepts
#endif // _HAS_CXX20
}

#if _HAS_CXX20 && defined(__cpp_constexpr_dynamic_alloc)
template <class T>
struct storage_for {
    union {
        T object;
    };

    constexpr storage_for() noexcept {}
    constexpr ~storage_for() {}
};

constexpr void test_compiletime() {
    {
        storage_for<int> s;
        construct_at(&s.object, 42);
        assert(s.object == 42);
        destroy_at(&s.object);

        ranges::construct_at(&s.object, 1729);
        assert(s.object == 1729);
        ranges::destroy_at(&s.object);
    }

    struct nontrivial {
        constexpr nontrivial(int i = 42) noexcept : x{i} {}
        constexpr ~nontrivial() {}

        int x = 42;
    };

    {
        storage_for<nontrivial> s;
        construct_at(&s.object, 42);
        assert(s.object.x == 42);
        destroy_at(&s.object);

        ranges::construct_at(&s.object, 1729);
        assert(s.object.x == 1729);
        ranges::destroy_at(&s.object);
    }
}
static_assert((test_compiletime(), true));
#endif // _HAS_CXX20 && defined(__cpp_constexpr_dynamic_alloc)

int main() {
    test_runtime(1234);
    test_runtime(string("hello world"));
    test_runtime(string("hello to some really long world that certainly doesn't fit in SSO"));

    {
        alignas(indestructible) unsigned char storage[sizeof(indestructible)];
        const auto ptr = reinterpret_cast<indestructible*>(storage);
        construct_at(ptr);
        ptr->destroy();

#ifdef __cpp_lib_concepts
        ranges::construct_at(ptr);
        ptr->destroy();
#endif // __cpp_lib_concepts
    }

    test_array(1234);
    test_array(string("hello world"));
    test_array(string("hello to some really long world that certainly doesn't fit in SSO"));
}
