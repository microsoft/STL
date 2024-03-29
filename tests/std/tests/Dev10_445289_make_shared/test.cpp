// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>
#include <type_traits>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

#pragma warning(disable : 28251) // Inconsistent annotation for 'new': this instance has no annotations.

int g_mallocs = 0;
int g_frees   = 0;

template <typename T>
class Mallocator {
public:
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    T* address(T& r) const {
        return &r;
    }

    const T* address(const T& s) const {
        return &s;
    }

    size_t max_size() const {
        return (static_cast<size_t>(0) - static_cast<size_t>(1)) / sizeof(T);
    }

    template <typename U>
    struct rebind {
        typedef Mallocator<U> other;
    };

    bool operator!=(const Mallocator& other) const {
        return !(*this == other);
    }

    void construct(T* const p, const T& t) const {
        void* const pv = static_cast<void*>(p);

        new (pv) T(t);
    }

    void destroy(T* const p) const {
        p->~T();
    }

    bool operator==(const Mallocator&) const {
        return true;
    }

    Mallocator() {}

    Mallocator(const Mallocator&) {}

    template <typename U>
    Mallocator(const Mallocator<U>&) {}

    ~Mallocator() {}

    T* allocate(const size_t n) const {
        if (n == 0) {
            return nullptr;
        }

        assert(n <= max_size());
        void* const pv = malloc(n * sizeof(T) + 256);
        if (!pv) {
            abort();
        }

        memset(pv, 0xCC, n * sizeof(T) + 256);
        ++g_mallocs;
        return static_cast<T*>(static_cast<void*>(static_cast<unsigned char*>(pv) + 128));
    }

    void deallocate(T* const p, size_t) const {
        unsigned char* const x = static_cast<unsigned char*>(static_cast<void*>(p)) - 128;
        const auto equal_to_cc = [](const unsigned char c) { return c == 0xCC; };
        assert(all_of(x, x + 128, equal_to_cc));
        const auto afterT = x + 128 + sizeof(T);
        assert(all_of(afterT, afterT + 128, equal_to_cc));
        ++g_frees;
        free(x);
    }

    template <typename U>
    T* allocate(const size_t n, const U*) const {
        return allocate(n);
    }

private:
    Mallocator& operator=(const Mallocator&);
};


int g_news    = 0;
int g_deletes = 0;

void* operator new(size_t size) {
    void* const p = ::operator new(size, nothrow);
    assert(p != nullptr);
    return p;
}

void* operator new(size_t size, const nothrow_t&) noexcept {
    ++g_news;

    return malloc(size == 0 ? 1 : size);
}

void operator delete(void* ptr) noexcept {
    ::operator delete(ptr, nothrow);
}

void operator delete(void* ptr, const nothrow_t&) noexcept {
    if (ptr) {
        ++g_deletes;

        free(ptr);
    }
}

void* operator new[](size_t size) {
    return ::operator new(size);
}

void* operator new[](size_t size, const nothrow_t&) noexcept {
    return ::operator new(size, nothrow);
}

void operator delete[](void* ptr) noexcept {
    ::operator delete(ptr);
}

void operator delete[](void* ptr, const nothrow_t&) noexcept {
    ::operator delete(ptr, nothrow);
}


int g_scorches = 0;
int g_burns    = 0;

void scorch(int* p) {
    if (p) {
        ++g_scorches;
        delete p;
    }
}

struct Burn {
    void operator()(int* p) const {
        if (p) {
            ++g_burns;
            delete p;
        }
    }
};

void test(int mallocs, int frees, int news, int deletes, int scorches, int burns) {
    assert(g_mallocs == mallocs);
    assert(g_frees == frees);
    assert(g_news == news);
    assert(g_deletes == deletes);
    assert(g_scorches == scorches);
    assert(g_burns == burns);
}

void reset() {
    g_mallocs  = 0;
    g_frees    = 0;
    g_news     = 0;
    g_deletes  = 0;
    g_scorches = 0;
    g_burns    = 0;
}

void test_and_reset(int mallocs, int frees, int news, int deletes, int scorches, int burns) {
    test(mallocs, frees, news, deletes, scorches, burns);
    reset();
}

#ifndef _M_CEE // TRANSITION, VSO-1659496
// GH-3100: "<memory> etc.: ADL should be avoided when calling _Construct_in_place and its friends"

template <class T>
struct adl_proof_allocator_provider {
    struct alloc;
};

// adl_proof_allocator<T> is equality-comparable even if T is ADL-incompatible.
template <class T>
using adl_proof_allocator = typename adl_proof_allocator_provider<T>::alloc;

template <class, class = void>
constexpr bool is_adl_proof_allocator = false;

template <class Alloc>
constexpr bool
    is_adl_proof_allocator<Alloc, void_t<typename adl_proof_allocator_provider<typename Alloc::value_type>::alloc>> =
        is_same_v<typename adl_proof_allocator_provider<typename Alloc::value_type>::alloc, Alloc>;

template <class T>
struct adl_proof_allocator_provider<T>::alloc {
    using value_type = T;

    template <class U>
    struct rebind {
        using other = typename adl_proof_allocator_provider<U>::alloc;
    };

    alloc() = default;
    template <class OtherAlloc, enable_if_t<is_adl_proof_allocator<OtherAlloc>, int> = 0>
    constexpr alloc(const OtherAlloc&) noexcept {}

    T* allocate(const size_t n) {
        return allocator<T>{}.allocate(n);
    }

#if _HAS_CXX23
    allocation_result<T*> allocate_at_least(const size_t n) {
        return allocator<T>{}.allocate_at_least(n);
    }
#endif // _HAS_CXX23

    void deallocate(T* const p, const size_t n) {
        return allocator<T>{}.deallocate(p, n);
    }

    template <class OtherAlloc, enable_if_t<is_adl_proof_allocator<OtherAlloc>, int> = 0>
    friend constexpr bool operator==(const alloc&, const OtherAlloc&) noexcept {
        return true;
    }
#if !_HAS_CXX20
    template <class OtherAlloc, enable_if_t<is_adl_proof_allocator<OtherAlloc>, int> = 0>
    friend constexpr bool operator!=(const alloc&, const OtherAlloc&) noexcept {
        return false;
    }
#endif // !_HAS_CXX20
};

template <class T>
struct holder {
    T t;
};

struct incomplete;

template <class T>
struct tagged_nontrivial {
    tagged_nontrivial() noexcept {}
    tagged_nontrivial(const tagged_nontrivial&) {}
    tagged_nontrivial(tagged_nontrivial&&) noexcept {}
    tagged_nontrivial& operator=(const tagged_nontrivial&) {
        return *this;
    }
    tagged_nontrivial& operator=(tagged_nontrivial&&) noexcept {
        return *this;
    }
    ~tagged_nontrivial() noexcept {}
};

template <class T>
void test_adl_proof_shared_ptr_creation_one() { // COMPILE-ONLY
    // Function calls are intentionally qualified.
    (void) shared_ptr<T>{};
    (void) shared_ptr<T>{new T};
    (void) shared_ptr<T>{new T, default_delete<T>{}};
    (void) shared_ptr<T>{new T, default_delete<T>{}, adl_proof_allocator<T>{}};
    (void) shared_ptr<T>{std::make_unique<T>()};

    (void) std::make_shared<T>();
    (void) std::make_shared<T>(T{});
    (void) std::allocate_shared<T>(adl_proof_allocator<T>{});
    (void) std::allocate_shared<T>(adl_proof_allocator<T>{}, T{});

#if _HAS_CXX20
    T src[42]{};
    T src2[6][6]{};

    (void) std::make_shared<T[]>(42);
    (void) std::make_shared<T[]>(42, T{});
    (void) std::make_shared<T[][42]>(42);
    (void) std::make_shared<T[][42]>(42, src);
    (void) std::make_shared<T[42][42]>();
    (void) std::make_shared<T[42][42]>(src);

    (void) std::make_shared<T[][6][6]>(6, src2);
    (void) std::make_shared<T[6][6][6]>(src2);

    (void) std::make_shared_for_overwrite<T>();
    (void) std::make_shared_for_overwrite<T[]>(42);
    (void) std::make_shared_for_overwrite<T[42]>();
    (void) std::make_shared_for_overwrite<T[][42]>(42);
    (void) std::make_shared_for_overwrite<T[42][42]>();

    (void) std::allocate_shared<T[]>(adl_proof_allocator<T>{}, 42);
    (void) std::allocate_shared<T[]>(adl_proof_allocator<T>{}, 42, T{});
    (void) std::allocate_shared<T[][42]>(adl_proof_allocator<T>{}, 42);
    (void) std::allocate_shared<T[][42]>(adl_proof_allocator<T>{}, 42, src);
    (void) std::allocate_shared<T[42]>(adl_proof_allocator<T>{});
    (void) std::allocate_shared<T[42]>(adl_proof_allocator<T>{}, T{});
    (void) std::allocate_shared<T[42][42]>(adl_proof_allocator<T>{});
    (void) std::allocate_shared<T[42][42]>(adl_proof_allocator<T>{}, src);

    (void) std::allocate_shared<T[][6][6]>(adl_proof_allocator<T>{}, 6, src2);
    (void) std::allocate_shared<T[6][6][6]>(adl_proof_allocator<T>{}, src2);

    (void) std::allocate_shared_for_overwrite<T>(adl_proof_allocator<T>{});
    (void) std::allocate_shared_for_overwrite<T[]>(adl_proof_allocator<T>{}, 42);
    (void) std::allocate_shared_for_overwrite<T[42]>(adl_proof_allocator<T>{});
    (void) std::allocate_shared_for_overwrite<T[][42]>(adl_proof_allocator<T>{}, 42);
    (void) std::allocate_shared_for_overwrite<T[42][42]>(adl_proof_allocator<T>{});
#endif // _HAS_CXX20
}

void test_adl_proof_shared_ptr_creation() { // COMPILE-ONLY
    using validator             = holder<incomplete>*;
    using validating_nontrivial = tagged_nontrivial<holder<incomplete>>;

    test_adl_proof_shared_ptr_creation_one<validator>();
    test_adl_proof_shared_ptr_creation_one<validating_nontrivial>();
}
#endif // ^^^ no workaround ^^^


int main() {
    reset();

    shared_ptr<int> p(new int(100));
    test_and_reset(0, 0, 2, 0, 0, 0);

    shared_ptr<int> pd1(new int(201), scorch);
    test_and_reset(0, 0, 2, 0, 0, 0);

    shared_ptr<int> pd2(new int(202), Burn());
    test_and_reset(0, 0, 2, 0, 0, 0);

    shared_ptr<int> pda1(new int(301), scorch, Mallocator<int>());
    test_and_reset(1, 0, 1, 0, 0, 0);

    shared_ptr<int> pda2(new int(302), Burn(), Mallocator<int>());
    test_and_reset(1, 0, 1, 0, 0, 0);

    shared_ptr<int> mp = make_shared<int>(401);
    test_and_reset(0, 0, 1, 0, 0, 0);

    shared_ptr<int> mpa = allocate_shared<int>(Mallocator<int>(), 402);
    test_and_reset(1, 0, 0, 0, 0, 0);

    assert(*p == 100);
    assert(*pd1 == 201);
    assert(*pd2 == 202);
    assert(*pda1 == 301);
    assert(*pda2 == 302);
    assert(*mp == 401);
    assert(*mpa == 402);


    weak_ptr<int> wp(p);
    test_and_reset(0, 0, 0, 0, 0, 0);

    weak_ptr<int> wpd1(pd1);
    test_and_reset(0, 0, 0, 0, 0, 0);

    weak_ptr<int> wpd2(pd2);
    test_and_reset(0, 0, 0, 0, 0, 0);

    weak_ptr<int> wpda1(pda1);
    test_and_reset(0, 0, 0, 0, 0, 0);

    weak_ptr<int> wpda2(pda2);
    test_and_reset(0, 0, 0, 0, 0, 0);

    weak_ptr<int> wmp(mp);
    test_and_reset(0, 0, 0, 0, 0, 0);

    weak_ptr<int> wmpa(mpa);
    test_and_reset(0, 0, 0, 0, 0, 0);

    assert(*wp.lock() == 100);
    assert(*wpd1.lock() == 201);
    assert(*wpd2.lock() == 202);
    assert(*wpda1.lock() == 301);
    assert(*wpda2.lock() == 302);
    assert(*wmp.lock() == 401);
    assert(*wmpa.lock() == 402);


    p.reset();
    test_and_reset(0, 0, 0, 1, 0, 0);

    pd1.reset();
    test_and_reset(0, 0, 0, 1, 1, 0);

    pd2.reset();
    test_and_reset(0, 0, 0, 1, 0, 1);

    pda1.reset();
    test_and_reset(0, 0, 0, 1, 1, 0);

    pda2.reset();
    test_and_reset(0, 0, 0, 1, 0, 1);

    mp.reset();
    test_and_reset(0, 0, 0, 0, 0, 0);

    mpa.reset();
    test_and_reset(0, 0, 0, 0, 0, 0);

    assert(!p);
    assert(!pd1);
    assert(!pd2);
    assert(!pda1);
    assert(!pda2);
    assert(!mp);
    assert(!mpa);

    assert(wp.expired());
    assert(wpd1.expired());
    assert(wpd2.expired());
    assert(wpda1.expired());
    assert(wpda2.expired());
    assert(wmp.expired());
    assert(wmpa.expired());


    wp.reset();
    test_and_reset(0, 0, 0, 1, 0, 0);

    wpd1.reset();
    test_and_reset(0, 0, 0, 1, 0, 0);

    wpd2.reset();
    test_and_reset(0, 0, 0, 1, 0, 0);

    wpda1.reset();
    test_and_reset(0, 1, 0, 0, 0, 0);

    wpda2.reset();
    test_and_reset(0, 1, 0, 0, 0, 0);

    wmp.reset();
    test_and_reset(0, 0, 0, 1, 0, 0);

    wmpa.reset();
    test_and_reset(0, 1, 0, 0, 0, 0);

    assert(wp.expired());
    assert(wpd1.expired());
    assert(wpd2.expired());
    assert(wpda1.expired());
    assert(wpda2.expired());
    assert(wmp.expired());
    assert(wmpa.expired());
}
