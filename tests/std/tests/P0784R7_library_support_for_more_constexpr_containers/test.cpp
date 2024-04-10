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

template <class Ty, class... Types>
concept can_std_construct_at = requires(Ty* ptr, Types&&... args) { construct_at(ptr, forward<Types>(args)...); };

template <class Ty, class... Types>
concept can_ranges_construct_at =
    requires(Ty* ptr, Types&&... args) { ranges::construct_at(ptr, forward<Types>(args)...); };

template <class Ty>
concept can_ranges_destroy_at = requires(Ty* ptr) { ranges::destroy_at(ptr); };

template <class Ty, class... Types>
constexpr bool can_construct_at = [] {
    constexpr bool result = can_std_construct_at<Ty, Types...>;
    static_assert(can_ranges_construct_at<Ty, Types...> == result);
    return result;
}();

template <class T, class... Args>
constexpr bool construct_at_noexcept() {
    if constexpr (can_construct_at<T, Args...>) {
        constexpr bool result = noexcept(construct_at(declval<T*>(), declval<Args>()...));
        static_assert(noexcept(ranges::construct_at(declval<T*>(), declval<Args>()...)) == result);
        return result;
    } else {
        return false;
    }
}

template <class T>
constexpr bool destroy_at_noexcept() {
    static_assert(noexcept(destroy_at(declval<T*>())));
    if constexpr (can_ranges_destroy_at<T>) {
        static_assert(noexcept(ranges::destroy_at(declval<T*>())));
    }
    return true;
}

static_assert(can_construct_at<int>);
static_assert(can_construct_at<int, int>);
static_assert(can_construct_at<int, int&>);
// per LWG-3888
static_assert(!can_construct_at<const int>);
static_assert(!can_construct_at<const int, int>);
static_assert(!can_construct_at<const int, int&>);
static_assert(!can_construct_at<volatile int>);
static_assert(!can_construct_at<volatile int, int>);
static_assert(!can_construct_at<volatile int, int&>);
static_assert(!can_construct_at<const volatile int>);
static_assert(!can_construct_at<const volatile int, int>);
static_assert(!can_construct_at<const volatile int, int&>);

struct X {};

static_assert(!can_construct_at<int, X>);
static_assert(!can_construct_at<X, int>);

// note that indestructible isn't constructible but is construct_at-ible:
struct indestructible {
    void destroy() {
        this->~indestructible();
    }

private:
    ~indestructible() = default;
};

static_assert(can_construct_at<indestructible>);
// per LWG-3888
static_assert(!can_construct_at<const indestructible>);
static_assert(!can_construct_at<volatile indestructible>);
static_assert(!can_construct_at<const volatile indestructible>);

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
// per LWG-3888
static_assert(!construct_at_noexcept<const int, int>());
static_assert(!construct_at_noexcept<volatile int, int>());
static_assert(!construct_at_noexcept<const volatile int, int>());

static_assert(!construct_at_noexcept<string, const char (&)[6]>());
// per LWG-3888
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

static_assert(!can_ranges_destroy_at<throwing_dtor>);
static_assert(!can_ranges_destroy_at<throwing_dtor[42]>);

template <class Ty>
void test_runtime(const Ty& val) {
    alignas(Ty) unsigned char storage[sizeof(Ty)];
    const auto asPtrTy = reinterpret_cast<Ty*>(&storage);
    memset(storage, 42, sizeof(Ty));
    assert(asPtrTy == construct_at(asPtrTy, val));
    assert(*asPtrTy == val);
    destroy_at(asPtrTy);

    // test ranges:
    memset(storage, 42, sizeof(Ty));
    assert(asPtrTy == ranges::construct_at(asPtrTy, val));
    assert(*asPtrTy == val);
    ranges::destroy_at(asPtrTy);
}

template <class T>
void test_array(const T& val) {
    constexpr int N = 42;
    (void) val;

    alignas(T) unsigned char storage[sizeof(T) * N];
    const auto ptr = reinterpret_cast<T*>(storage);

    for (auto i = 0; i < N; ++i) {
        construct_at(ptr + i, val);
    }

    destroy_at(reinterpret_cast<T(*)[N]>(ptr));

    for (auto i = 0; i < N; ++i) {
        ranges::construct_at(ptr + i, val);
    }

    ranges::destroy_at(reinterpret_cast<T(*)[N]>(ptr));
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

template <class T>
struct A {
    T value;

    constexpr A() noexcept                    = default;
    constexpr A(const A&) noexcept            = default;
    constexpr A& operator=(const A&) noexcept = default;
    constexpr ~A()                            = default;
};

template <class T>
struct nontrivial_A {
    T value;

    constexpr nontrivial_A(T in = T{}) noexcept : value(in) {}
    constexpr nontrivial_A(const nontrivial_A&) noexcept            = default;
    constexpr nontrivial_A& operator=(const nontrivial_A&) noexcept = default;
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

    constexpr void construct(value_type* ptr, value_type n)
        requires Construct
    {
        construct_at(ptr, n);
    }

    constexpr void destroy(value_type* ptr)
        requires Destroy
    {
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

// Also test LWG-3888 Most ranges uninitialized memory algorithms are underconstrained
template <class Rng>
concept CanUninitializedDefaultConstruct = requires(Rng& r) { ranges::uninitialized_default_construct(r); };

template <class It>
concept CanUninitializedDefaultConstructN =
    requires(It&& i) { ranges::uninitialized_default_construct_n(forward<It>(i), iter_difference_t<It>{}); };

template <class Rng>
concept CanUninitializedValueConstruct = requires(Rng& r) { ranges::uninitialized_value_construct(r); };

template <class It>
concept CanUninitializedValueConstructN =
    requires(It&& i) { ranges::uninitialized_value_construct_n(forward<It>(i), iter_difference_t<It>{}); };

template <class Rng, class T>
concept CanUninitializedFill = requires(Rng& r, const T& t) { ranges::uninitialized_fill(r, t); };

template <class It, class T>
concept CanUninitializedFillN =
    requires(It&& i, const T& t) { ranges::uninitialized_fill_n(forward<It>(i), iter_difference_t<It>{}, t); };

template <class InRng, class OutRng>
concept CanUninitializedCopy = requires(InRng& ri, OutRng& ro) { ranges::uninitialized_copy(ri, ro); };

template <class InIt, class OutIt, class S>
concept CanUninitializedCopyN = requires(InIt&& ii, OutIt&& io, S&& s) {
    ranges::uninitialized_copy_n(forward<InIt>(ii), iter_difference_t<InIt>{}, forward<OutIt>(io), forward<S>(s));
};

template <class InRng, class OutRng>
concept CanUninitializedMove = requires(InRng& ri, OutRng& ro) { ranges::uninitialized_move(ri, ro); };

template <class InIt, class OutIt, class S>
concept CanUninitializedMoveN = requires(InIt&& ii, OutIt&& io, S&& s) {
    ranges::uninitialized_move_n(forward<InIt>(ii), iter_difference_t<InIt>{}, forward<OutIt>(io), forward<S>(s));
};

template <class Rng>
concept CanDestroy = requires(Rng&& r) { ranges::destroy(forward<Rng>(r)); };

template <class It>
concept CanDestroyN = requires(It&& i) { ranges::destroy_n(forward<It>(i), iter_difference_t<It>{}); };

static_assert(CanUninitializedDefaultConstruct<char[42]>);
static_assert(!CanUninitializedDefaultConstruct<const char[42]>);
static_assert(!CanUninitializedDefaultConstruct<volatile char[42]>);
static_assert(!CanUninitializedDefaultConstruct<const volatile char[42]>);

static_assert(CanUninitializedDefaultConstructN<char*>);
static_assert(!CanUninitializedDefaultConstructN<const char*>);
static_assert(!CanUninitializedDefaultConstructN<volatile char*>);
static_assert(!CanUninitializedDefaultConstructN<const volatile char*>);

static_assert(CanUninitializedValueConstruct<char[42]>);
static_assert(!CanUninitializedValueConstruct<const char[42]>);
static_assert(!CanUninitializedValueConstruct<volatile char[42]>);
static_assert(!CanUninitializedValueConstruct<const volatile char[42]>);

static_assert(CanUninitializedValueConstructN<char*>);
static_assert(!CanUninitializedValueConstructN<const char*>);
static_assert(!CanUninitializedValueConstructN<volatile char*>);
static_assert(!CanUninitializedValueConstructN<const volatile char*>);

static_assert(CanUninitializedFill<char[42], int>);
static_assert(!CanUninitializedFill<const char[42], int>);
static_assert(!CanUninitializedFill<volatile char[42], int>);
static_assert(!CanUninitializedFill<const volatile char[42], int>);

static_assert(CanUninitializedFillN<char*, int>);
static_assert(!CanUninitializedFillN<const char*, int>);
static_assert(!CanUninitializedFillN<volatile char*, int>);
static_assert(!CanUninitializedFillN<const volatile char*, int>);

static_assert(CanUninitializedCopy<const int[42], char[42]>);
static_assert(!CanUninitializedCopy<const int[42], const char[42]>);
static_assert(!CanUninitializedCopy<const int[42], volatile char[42]>);
static_assert(!CanUninitializedCopy<const int[42], const volatile char[42]>);

static_assert(CanUninitializedCopyN<const int*, char*, const char*>);
static_assert(!CanUninitializedCopyN<const int*, const char*, const char*>);
static_assert(!CanUninitializedCopyN<const int*, volatile char*, const char*>);
static_assert(!CanUninitializedCopyN<const int*, const volatile char*, const char*>);

static_assert(CanUninitializedMove<const int[42], char[42]>);
static_assert(!CanUninitializedMove<const int[42], const char[42]>);
static_assert(!CanUninitializedMove<const int[42], volatile char[42]>);
static_assert(!CanUninitializedMove<const int[42], const volatile char[42]>);

static_assert(CanUninitializedMoveN<const int*, char*, const char*>);
static_assert(!CanUninitializedMoveN<const int*, const char*, const char*>);
static_assert(!CanUninitializedMoveN<const int*, volatile char*, const char*>);
static_assert(!CanUninitializedMoveN<const int*, const volatile char*, const char*>);

static_assert(CanDestroy<char[42]>);
static_assert(!CanDestroy<const char[42]>);
static_assert(!CanDestroy<volatile char[42]>);
static_assert(!CanDestroy<const volatile char[42]>);

static_assert(CanDestroyN<char*>);
static_assert(!CanDestroyN<const char*>);
static_assert(!CanDestroyN<volatile char*>);
static_assert(!CanDestroyN<const volatile char*>);

int main() {
    test_runtime(1234);
    test_runtime(string("hello world"));
    test_runtime(string("hello to some really long world that certainly doesn't fit in SSO"));

    {
        alignas(indestructible) unsigned char storage[sizeof(indestructible)];
        const auto ptr = reinterpret_cast<indestructible*>(storage);
        construct_at(ptr);
        ptr->destroy();

        ranges::construct_at(ptr);
        ptr->destroy();
    }

    test_array(1234);
    test_array(string("hello world"));
    test_array(string("hello to some really long world that certainly doesn't fit in SSO"));
}
