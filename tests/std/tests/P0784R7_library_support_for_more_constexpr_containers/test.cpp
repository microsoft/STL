// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <cstring>
#include <limits>
#include <memory>
#include <span>
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

static_assert(!can_construct_at<int, X>);
static_assert(!can_construct_at<X, int>);

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

static_assert(construct_at_noexcept<int, int>());
static_assert(construct_at_noexcept<const int, int>());
static_assert(construct_at_noexcept<volatile int, int>());
static_assert(construct_at_noexcept<const volatile int, int>());

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

static_assert(destroy_at_noexcept<int[42]>());
static_assert(destroy_at_noexcept<string[42]>());
static_assert(destroy_at_noexcept<const int[42]>());
static_assert(destroy_at_noexcept<const string[42]>());
static_assert(destroy_at_noexcept<volatile int[42]>());
static_assert(destroy_at_noexcept<volatile string[42]>());
static_assert(destroy_at_noexcept<const volatile int[42]>());
static_assert(destroy_at_noexcept<const volatile string[42]>());

struct throwing_dtor {
    ~throwing_dtor() noexcept(false) {}
};

static_assert(destroy_at_noexcept<throwing_dtor>());
static_assert(destroy_at_noexcept<throwing_dtor[42]>());

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

    ranges::destroy_at(reinterpret_cast<U(*)[N]>(ptr));
#endif // __cpp_lib_concepts
}

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

#ifdef __cpp_lib_concepts
        ranges::construct_at(&s.object, 1729);
        assert(s.object == 1729);
        ranges::destroy_at(&s.object);
#endif // __cpp_lib_concepts
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

#ifdef __cpp_lib_concepts
        ranges::construct_at(&s.object, 1729);
        assert(s.object.x == 1729);
        ranges::destroy_at(&s.object);
#endif // __cpp_lib_concepts
    }
}
static_assert((test_compiletime(), true));

template <class T>
struct A {
    T value;

    constexpr A() noexcept = default;
    constexpr ~A()         = default;
};

template <class T>
struct nontrivial_A {
    T value;

    constexpr nontrivial_A(T in = T{}) noexcept : value(in) {}
    constexpr ~nontrivial_A() {}
};

constexpr void test_compiletime_destroy_variants() {
    {
        allocator<A<int>> alloc{};
        A<int>* a = alloc.allocate(10);
        for (int i = 0; i < 10; ++i) {
            construct_at(a + i);
        }
        destroy(a, a + 10);
        alloc.deallocate(a, 10);
    }
    {
        allocator<nontrivial_A<int>> alloc{};
        nontrivial_A<int>* a = alloc.allocate(10);
        for (int i = 0; i < 10; ++i) {
            construct_at(a + i);
        }
        destroy(a, a + 10);
        alloc.deallocate(a, 10);
    }
#ifdef __cpp_lib_concepts
    {
        allocator<A<int>> alloc{};
        A<int>* a = alloc.allocate(10);
        for (int i = 0; i < 10; ++i) {
            ranges::construct_at(a + i);
        }
        ranges::destroy(a, a + 10);
        alloc.deallocate(a, 10);
    }
    {
        allocator<nontrivial_A<int>> alloc{};
        nontrivial_A<int>* a = alloc.allocate(10);
        for (int i = 0; i < 10; ++i) {
            ranges::construct_at(a + i);
        }
        ranges::destroy(a, a + 10);
        alloc.deallocate(a, 10);
    }
    {
        allocator<A<int>> alloc{};
        A<int>* a = alloc.allocate(10);
        for (int i = 0; i < 10; ++i) {
            ranges::construct_at(a + i);
        }
        span s{a, 10};
        ranges::destroy(s);
        alloc.deallocate(a, 10);
    }
    {
        allocator<nontrivial_A<int>> alloc{};
        nontrivial_A<int>* a = alloc.allocate(10);
        for (int i = 0; i < 10; ++i) {
            ranges::construct_at(a + i);
        }
        span s{a, 10};
        ranges::destroy(s);
        alloc.deallocate(a, 10);
    }
#endif // __cpp_lib_concepts
    {
        allocator<A<int>> alloc{};
        A<int>* a = alloc.allocate(10);
        for (int i = 0; i < 10; ++i) {
            construct_at(a + i);
        }
        destroy_n(a, 10);
        alloc.deallocate(a, 10);
    }
    {
        allocator<nontrivial_A<int>> alloc{};
        nontrivial_A<int>* a = alloc.allocate(10);
        for (int i = 0; i < 10; ++i) {
            construct_at(a + i);
        }
        destroy_n(a, 10);
        alloc.deallocate(a, 10);
    }
#ifdef __cpp_lib_concepts
    {
        allocator<A<int>> alloc{};
        A<int>* a = alloc.allocate(10);
        for (int i = 0; i < 10; ++i) {
            ranges::construct_at(a + i);
        }
        ranges::destroy_n(a, 10);
        alloc.deallocate(a, 10);
    }
    {
        allocator<nontrivial_A<int>> alloc{};
        nontrivial_A<int>* a = alloc.allocate(10);
        for (int i = 0; i < 10; ++i) {
            ranges::construct_at(a + i);
        }
        ranges::destroy_n(a, 10);
        alloc.deallocate(a, 10);
    }
#endif // __cpp_lib_concepts
}
static_assert((test_compiletime_destroy_variants(), true));

template <class T, bool Construct = false, bool Destroy = false>
struct Alloc {
    using value_type = T;
    using size_type  = size_t;

    template <class U>
    struct rebind {
        using other = Alloc<U, Construct, Destroy>;
    };

    constexpr Alloc(int id_) noexcept : id(id_) {}

    template <class U>
    constexpr Alloc(const Alloc<U, Construct, Destroy>& al) noexcept : id(al.id) {}

    constexpr value_type* allocate(size_t n) {
        assert(n == 10);
        return allocator<T>{}.allocate(n);
    }

    constexpr void deallocate(value_type* ptr, size_t n) {
        assert(n == 10);
        allocator<T>{}.deallocate(ptr, n);
    }

    constexpr void construct(value_type* ptr, value_type n) requires Construct {
        construct_at(ptr, n);
    }

    constexpr void destroy(value_type* ptr) requires Destroy {
        destroy_at(ptr);
    }

    constexpr Alloc select_on_container_copy_construction() const noexcept {
        return Alloc{id + 1};
    }

    constexpr size_type max_size() const noexcept {
        return numeric_limits<size_type>::max() / sizeof(value_type);
    }

    template <class U>
    constexpr bool operator==(const Alloc<U, Construct, Destroy>&) const noexcept {
        return true;
    }

    int id;
};

constexpr void test_compiletime_allocator_traits() {
    {
        storage_for<A<int>> a;
        Alloc<A<int>> alloc{10};
        assert(alloc.id == 10);

        auto result = allocator_traits<Alloc<A<int>>>::allocate(alloc, 10);
        assert(result != nullptr);
        allocator_traits<Alloc<A<int>>>::deallocate(alloc, result, 10);

        allocator_traits<Alloc<A<int>>>::construct(alloc, &a.object);
        assert(a.object.value == 0);
        allocator_traits<Alloc<A<int>>>::destroy(alloc, &a.object);

        assert(allocator_traits<Alloc<A<int>>>::select_on_container_copy_construction(alloc).id == 11);

        assert(allocator_traits<Alloc<A<int>>>::max_size(alloc)
               == numeric_limits<Alloc<A<int>>::size_type>::max() / sizeof(Alloc<A<int>>::value_type));
    }
    {
        storage_for<nontrivial_A<int>> a;
        Alloc<nontrivial_A<int>> alloc{10};
        assert(alloc.id == 10);

        auto result = allocator_traits<Alloc<nontrivial_A<int>>>::allocate(alloc, 10);
        assert(result != nullptr);
        allocator_traits<Alloc<nontrivial_A<int>>>::deallocate(alloc, result, 10);

        allocator_traits<Alloc<nontrivial_A<int>>>::construct(alloc, &a.object, 10);
        assert(a.object.value == 10);
        allocator_traits<Alloc<nontrivial_A<int>>>::destroy(alloc, &a.object);

        assert(allocator_traits<Alloc<nontrivial_A<int>>>::select_on_container_copy_construction(alloc).id == 11);

        assert(allocator_traits<Alloc<nontrivial_A<int>>>::max_size(alloc)
               == numeric_limits<Alloc<nontrivial_A<int>>::size_type>::max()
                      / sizeof(Alloc<nontrivial_A<int>>::value_type));
    }
    {
        storage_for<nontrivial_A<int>> a;
        Alloc<nontrivial_A<int>, true> alloc{10};

        allocator_traits<Alloc<nontrivial_A<int>, true>>::construct(alloc, &a.object, 10);
        assert(a.object.value == 10);
        allocator_traits<Alloc<nontrivial_A<int>, true>>::destroy(alloc, &a.object);
    }
    {
        storage_for<nontrivial_A<int>> a;
        Alloc<nontrivial_A<int>, false, true> alloc{10};

        allocator_traits<Alloc<nontrivial_A<int>, false, true>>::construct(alloc, &a.object, 10);
        assert(a.object.value == 10);
        allocator_traits<Alloc<nontrivial_A<int>, false, true>>::destroy(alloc, &a.object);
    }
    {
        storage_for<nontrivial_A<int>> a;
        Alloc<nontrivial_A<int>, true, true> alloc{10};

        allocator_traits<Alloc<nontrivial_A<int>, true, true>>::construct(alloc, &a.object, 10);
        assert(a.object.value == 10);
        allocator_traits<Alloc<nontrivial_A<int>, true, true>>::destroy(alloc, &a.object);
    }
}
static_assert((test_compiletime_allocator_traits(), true));

constexpr void test_compiletime_allocator() {
    {
        auto result = allocator<A<int>>{}.allocate(10);
        allocator<A<int>>{}.deallocate(result, 10);
    }
    {
        auto result = allocator<nontrivial_A<int>>{}.allocate(10);
        allocator<nontrivial_A<int>>{}.deallocate(result, 10);
    }
}
static_assert((test_compiletime_allocator(), true));

constexpr void test_compiletime_operators() {
    {
        allocator<int> allocatorA{};
        allocator<float> allocatorB{};
        constexpr auto allocatorC = allocatorA;

        static_assert(allocatorA == allocatorB);
        static_assert(!(allocatorA != allocatorB));
        static_assert(allocatorA == allocatorC);
    }
}
static_assert((test_compiletime_operators(), true));

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
