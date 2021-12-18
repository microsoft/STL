// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//
// A more or less identical version of this test is in the compiler optimizer team's test harness for arm64 coverage at:
// Regress\intrin\atomic.cpp
//

#include <assert.h>
#include <atomic>
#include <limits>
#include <new>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <type_traits>


using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

// N3797 29.6.5 [atomics.types.operations.req]/21:
// bool A::compare_exchange_weak(C & expected, C desired, memory_order order = memory_order_seq_cst) volatile noexcept;
// bool A::compare_exchange_weak(C & expected, C desired, memory_order order = memory_order_seq_cst) noexcept;
// bool A::compare_exchange_strong(C & expected, C desired, memory_order order = memory_order_seq_cst) volatile
// noexcept; bool A::compare_exchange_strong(C & expected, C desired, memory_order order = memory_order_seq_cst)
// noexcept; When only one memory_order argument is supplied, the value of success is order, and the value of failure is
// order except that a value of memory_order_acq_rel shall be replaced by the value memory_order_acquire and a value of
// memory_order_release shall be replaced by the value memory_order_relaxed.

template <typename T>
void test(T t) {
    atomic<T> a(t);

    a.compare_exchange_weak(t, t, memory_order_acq_rel);
    a.compare_exchange_weak(t, t, memory_order_release);
    a.compare_exchange_strong(t, t, memory_order_acq_rel);
    a.compare_exchange_strong(t, t, memory_order_release);

    volatile atomic<T> v(t);

    v.compare_exchange_weak(t, t, memory_order_acq_rel);
    v.compare_exchange_weak(t, t, memory_order_release);
    v.compare_exchange_strong(t, t, memory_order_acq_rel);
    v.compare_exchange_strong(t, t, memory_order_release);
}

struct X {
    int n;
};

struct Y {
    int n;

    Y() {}
};

// Also test:
// VSO-197633 atomic<T>::operator= should not return the old value
void test_operator_assign() {
    atomic<int> a(1);
    const int b(a = 2);
    assert(a.load() == 2 && b == 2);

    atomic<X> aX({1});
    const X bX(aX = {2});
    assert(aX.load().n == 2 && bX.n == 2);
}

// Also test:
// VSO-257598: atomic<T*> == nullptr is ambiguous
void test_nullptr_compares() {
    assert(atomic<int*>{} == nullptr);
    assert(nullptr == atomic<int*>{});
    assert(!(atomic<int*>{} != nullptr));
    assert(!(nullptr != atomic<int*>{}));

    assert(atomic<void*>{} == nullptr);
    assert(nullptr == atomic<void*>{});
    assert(!(atomic<void*>{} != nullptr));
    assert(!(nullptr != atomic<void*>{}));
}

// Also test:
// DevDiv-826403 "<atomic>: passing volatile atomic<T> to store won't compile  [libs-conformance]"
// DevDiv-829873 "<atomic>: Error when using atomic pointer to const"
// DevDiv-846428 "<atomic>: std::atomic<T>::store with volatile specifier does not work for non-integral type"
// DevDiv-879700 "<atomic>: atomic<T *> constructor missing cast?"
// DevDiv-1181758 "<atomic>: MSVC 2015 std::atomic is implemented using non-conforming C++"
// This is compile-only.

template <typename T>
struct is_object_pointer : false_type {};
template <typename T>
struct is_object_pointer<T*> : is_object<T>::type {};

template <typename Obj, typename Atom>
void helper4_integral(Obj& obj, Atom& atom, true_type) {
    obj = atom.fetch_and(42);
    obj = atom.fetch_and(42, memory_order_seq_cst);

    obj = atom.fetch_or(42);
    obj = atom.fetch_or(42, memory_order_seq_cst);

    obj = atom.fetch_xor(42);
    obj = atom.fetch_xor(42, memory_order_seq_cst);

    obj = (atom &= 42);
    obj = (atom |= 42);
    obj = (atom ^= 42);
}

template <typename Obj, typename Atom>
void helper4_integral(Obj&, Atom&, false_type) {}

template <typename Obj, typename Atom>
void helper3_integral_object_pointer(Obj& obj, Atom& atom, true_type) {

    obj = ++atom;
    obj = --atom;
    obj = atom++;
    obj = atom--;
}

template <typename Obj, typename Atom>
void helper3_integral_object_pointer(Obj&, Atom&, false_type) {}

template <typename Obj, typename Atom>
void helper5_arithmetic_object_pointer(Obj& obj, Atom& atom, true_type) {
    obj = atom.fetch_add(42);
    obj = atom.fetch_add(42, memory_order_seq_cst);

    obj = atom.fetch_sub(42);
    obj = atom.fetch_sub(42, memory_order_seq_cst);

    obj = (atom += 42);
    obj = (atom -= 42);
}

template <typename Obj, typename Atom>
void helper5_arithmetic_object_pointer(Obj&, Atom&, false_type) {}

template <typename Obj, typename Atom>
void helper2() {
    Obj obj{};

    Atom atom;

    const Atom const_atom(obj);

    bool b = const_atom.is_lock_free();
    (void) b;

    atom.store(obj);
    atom.store(obj, memory_order_seq_cst);

    obj = const_atom.load();
    obj = const_atom.load(memory_order_seq_cst);

    obj = const_atom;

    obj = atom.exchange(obj);
    obj = atom.exchange(obj, memory_order_seq_cst);

    b = atom.compare_exchange_weak(obj, obj);
    b = atom.compare_exchange_weak(obj, obj, memory_order_seq_cst);
    b = atom.compare_exchange_weak(obj, obj, memory_order_seq_cst, memory_order_seq_cst);

    b = atom.compare_exchange_strong(obj, obj);
    b = atom.compare_exchange_strong(obj, obj, memory_order_seq_cst);
    b = atom.compare_exchange_strong(obj, obj, memory_order_seq_cst, memory_order_seq_cst);

    obj = (atom = obj);

    helper3_integral_object_pointer(obj, atom, disjunction<is_integral<Obj>, is_object_pointer<Obj>>());

    helper4_integral(obj, atom, is_integral<Obj>());

#if _HAS_CXX20
    helper5_arithmetic_object_pointer(obj, atom, disjunction<is_arithmetic<Obj>, is_object_pointer<Obj>>());
#else
    helper5_arithmetic_object_pointer(obj, atom, disjunction<is_integral<Obj>, is_object_pointer<Obj>>());
#endif // _HAS_CXX20
}

template <typename Obj>
void helper1() {
    helper2<Obj, atomic<Obj>>();

#if _HAS_CXX20
    if constexpr (atomic<Obj>::is_always_lock_free) {
#endif // _HAS_CXX20
        helper2<Obj, volatile atomic<Obj>>();
#if _HAS_CXX20
    }
#endif // _HAS_CXX20
}

template <int N>
struct Bytes {
    unsigned char bytes[N];
};

void test_types() {
    helper1<int8_t>();
    helper1<int16_t>();
    helper1<int32_t>();
    helper1<int64_t>();
    helper1<uint8_t>();
    helper1<uint16_t>();
    helper1<uint32_t>();
    helper1<uint64_t>();

    helper1<float>();
    helper1<double>();
    helper1<long double>();

    helper1<int*>();
    helper1<const int*>();
    helper1<volatile int*>();
    helper1<const volatile int*>();

    helper1<void*>();
    helper1<const void*>();
    helper1<volatile void*>();
    helper1<const volatile void*>();

    helper1<int (*)(int)>();

    helper1<Bytes<1>>();
    helper1<Bytes<2>>();
    helper1<Bytes<4>>();
    helper1<Bytes<8>>();
    helper1<Bytes<16>>();
    helper1<Bytes<17>>();
}


// Also test:
// VSO-152725 "<atomic>: Code generated by MSVC doesn't operate atomically
// on std::atomic<T> object when sizeof(T)==n && alignof(T)!=n, (n=2,4,8)"
// VSO-212461 "<atomic>: atomic<unsigned long long> isn't 8-aligned"

// These values aren't guaranteed by the Standard, but this needs to be true for our implementation.

STATIC_ASSERT(sizeof(atomic<Bytes<1>>) == 1);
STATIC_ASSERT(sizeof(atomic<Bytes<2>>) == 2);
STATIC_ASSERT(sizeof(atomic<Bytes<4>>) == 4);
STATIC_ASSERT(sizeof(atomic<Bytes<8>>) == 8);

STATIC_ASSERT(alignof(atomic<Bytes<1>>) == 1);
STATIC_ASSERT(alignof(atomic<Bytes<2>>) == 2);
STATIC_ASSERT(alignof(atomic<Bytes<4>>) == 4);
STATIC_ASSERT(alignof(atomic<Bytes<8>>) == 8);

STATIC_ASSERT(sizeof(atomic<int8_t>) == 1);
STATIC_ASSERT(sizeof(atomic<int16_t>) == 2);
STATIC_ASSERT(sizeof(atomic<int32_t>) == 4);
STATIC_ASSERT(sizeof(atomic<int64_t>) == 8);

STATIC_ASSERT(alignof(atomic<int8_t>) == 1);
STATIC_ASSERT(alignof(atomic<int16_t>) == 2);
STATIC_ASSERT(alignof(atomic<int32_t>) == 4);
STATIC_ASSERT(alignof(atomic<int64_t>) == 8);

void test_alignment() {
    atomic<Bytes<2>> ab2{};
    atomic<Bytes<4>> ab4{};
    atomic<Bytes<8>> ab8{};

    assert(reinterpret_cast<uintptr_t>(&ab2) % 2 == 0);
    assert(reinterpret_cast<uintptr_t>(&ab4) % 4 == 0);
    assert(reinterpret_cast<uintptr_t>(&ab8) % 8 == 0);

    atomic<int16_t> ai16{};
    atomic<int32_t> ai32{};
    atomic<int64_t> ai64{};

    assert(reinterpret_cast<uintptr_t>(&ai16) % 2 == 0);
    assert(reinterpret_cast<uintptr_t>(&ai32) % 4 == 0);
    assert(reinterpret_cast<uintptr_t>(&ai64) % 8 == 0);
}

STATIC_ASSERT(noexcept(atomic<X>()));
STATIC_ASSERT(!noexcept(atomic<Y>()));
STATIC_ASSERT(noexcept(atomic<int>()));
STATIC_ASSERT(noexcept(atomic<void*>()));
STATIC_ASSERT(noexcept(atomic<Bytes<1>>()));
STATIC_ASSERT(noexcept(atomic<Bytes<2>>()));
STATIC_ASSERT(noexcept(atomic<Bytes<4>>()));
STATIC_ASSERT(noexcept(atomic<Bytes<8>>()));
STATIC_ASSERT(noexcept(atomic<Bytes<17>>()));

// verify that atomic default constructors are constexpr
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-const-variable"
#endif // __clang__

constexpr atomic<int> i_constexpr;
constexpr atomic<float> f_constexpr;
constexpr atomic<void*> ptr_constexpr;
constexpr atomic<X> x_constexpr;
constexpr atomic<Bytes<1>> b1_constexpr;
constexpr atomic<Bytes<2>> b2_constexpr;
constexpr atomic<Bytes<4>> b4_constexpr;
constexpr atomic<Bytes<8>> b8_constexpr;
constexpr atomic<Bytes<17>> b17_constexpr;

constexpr atomic<int> i_constexpr_v(42);
constexpr atomic<float> f_constexpr_v(0.2f);
constexpr atomic<void*> ptr_constexpr_v(nullptr);
constexpr atomic<X> x_constexpr_v(X{42});
constexpr atomic<Bytes<1>> b1_constexpr_v(Bytes<1>{});
constexpr atomic<Bytes<2>> b2_constexpr_v(Bytes<2>{});
constexpr atomic<Bytes<4>> b4_constexpr_v(Bytes<4>{});
constexpr atomic<Bytes<8>> b8_constexpr_v(Bytes<8>{});
constexpr atomic<Bytes<17>> b17_constexpr_v(Bytes<17>{});

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

// Also test trivial destructors
STATIC_ASSERT(is_trivially_destructible_v<atomic<Bytes<1>>>); // these struct cases (and bool)
STATIC_ASSERT(is_trivially_destructible_v<atomic<Bytes<2>>>); // aren't technically required to work
STATIC_ASSERT(is_trivially_destructible_v<atomic<Bytes<3>>>); // in the spec, but we think they should
STATIC_ASSERT(is_trivially_destructible_v<atomic<Bytes<4>>>); // on any reasonable implementation
STATIC_ASSERT(is_trivially_destructible_v<atomic<Bytes<5>>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<Bytes<6>>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<Bytes<7>>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<Bytes<8>>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<Bytes<9>>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<bool>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<char>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<signed char>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<unsigned char>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<wchar_t>>);
#ifdef __cpp_char8_t
STATIC_ASSERT(is_trivially_destructible_v<atomic<char8_t>>);
#endif // __cpp_char8_t
STATIC_ASSERT(is_trivially_destructible_v<atomic<char16_t>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<char32_t>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<short>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<unsigned short>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<int>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<unsigned int>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<float>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<long>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<unsigned long>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<long long>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<unsigned long long>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<double>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<long double>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<Bytes<1>*>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<void*>>);
STATIC_ASSERT(is_trivially_destructible_v<atomic<int (*)(int)>>);

STATIC_ASSERT(is_trivially_destructible_v<atomic_flag>);

// Also test standard layout for lock free atomics
STATIC_ASSERT(is_standard_layout_v<atomic<Bytes<1>>>); // the struct cases (but not bool) aren't required
STATIC_ASSERT(is_standard_layout_v<atomic<Bytes<2>>>);
STATIC_ASSERT(!is_standard_layout_v<atomic<Bytes<3>>>); // TRANSITION, ABI, should already be fixed in vNext
STATIC_ASSERT(is_standard_layout_v<atomic<Bytes<4>>>);
STATIC_ASSERT(!is_standard_layout_v<atomic<Bytes<5>>>);
STATIC_ASSERT(!is_standard_layout_v<atomic<Bytes<6>>>);
STATIC_ASSERT(!is_standard_layout_v<atomic<Bytes<7>>>);
STATIC_ASSERT(is_standard_layout_v<atomic<Bytes<8>>>);
STATIC_ASSERT(!is_standard_layout_v<atomic<Bytes<9>>>);
STATIC_ASSERT(is_standard_layout_v<atomic<bool>>);
STATIC_ASSERT(is_standard_layout_v<atomic<char>>);
STATIC_ASSERT(is_standard_layout_v<atomic<signed char>>);
STATIC_ASSERT(is_standard_layout_v<atomic<unsigned char>>);
STATIC_ASSERT(is_standard_layout_v<atomic<wchar_t>>);
#ifdef __cpp_char8_t
STATIC_ASSERT(is_standard_layout_v<atomic<char8_t>>);
#endif // __cpp_char8_t
STATIC_ASSERT(is_standard_layout_v<atomic<char16_t>>);
STATIC_ASSERT(is_standard_layout_v<atomic<char32_t>>);
STATIC_ASSERT(is_standard_layout_v<atomic<short>>);
STATIC_ASSERT(is_standard_layout_v<atomic<unsigned short>>);
STATIC_ASSERT(is_standard_layout_v<atomic<int>>);
STATIC_ASSERT(is_standard_layout_v<atomic<unsigned int>>);
STATIC_ASSERT(is_standard_layout_v<atomic<float>>);
STATIC_ASSERT(is_standard_layout_v<atomic<long>>);
STATIC_ASSERT(is_standard_layout_v<atomic<unsigned long>>);
STATIC_ASSERT(is_standard_layout_v<atomic<long long>>);
STATIC_ASSERT(is_standard_layout_v<atomic<unsigned long long>>);
STATIC_ASSERT(is_standard_layout_v<atomic<double>>);
STATIC_ASSERT(is_standard_layout_v<atomic<long double>>);
STATIC_ASSERT(is_standard_layout_v<atomic<Bytes<1>*>>);
STATIC_ASSERT(is_standard_layout_v<atomic<void*>>);
STATIC_ASSERT(is_standard_layout_v<atomic<int (*)(int)>>);

STATIC_ASSERT(is_standard_layout_v<atomic_flag>);

// Also test P0152R1 atomic::is_always_lock_free
#if _HAS_CXX17
STATIC_ASSERT(atomic<Bytes<1>>::is_always_lock_free);
STATIC_ASSERT(atomic<Bytes<2>>::is_always_lock_free);
STATIC_ASSERT(!atomic<Bytes<3>>::is_always_lock_free);
STATIC_ASSERT(atomic<Bytes<4>>::is_always_lock_free);
STATIC_ASSERT(!atomic<Bytes<5>>::is_always_lock_free);
STATIC_ASSERT(!atomic<Bytes<6>>::is_always_lock_free);
STATIC_ASSERT(!atomic<Bytes<7>>::is_always_lock_free);
STATIC_ASSERT(atomic<Bytes<8>>::is_always_lock_free);
STATIC_ASSERT(!atomic<Bytes<9>>::is_always_lock_free);
STATIC_ASSERT(atomic<bool>::is_always_lock_free);
STATIC_ASSERT(atomic<char>::is_always_lock_free);
STATIC_ASSERT(atomic<signed char>::is_always_lock_free);
STATIC_ASSERT(atomic<unsigned char>::is_always_lock_free);
STATIC_ASSERT(atomic<wchar_t>::is_always_lock_free);
#ifdef __cpp_char8_t
STATIC_ASSERT(atomic<char8_t>::is_always_lock_free);
#endif // __cpp_char8_t
STATIC_ASSERT(atomic<char16_t>::is_always_lock_free);
STATIC_ASSERT(atomic<char32_t>::is_always_lock_free);
STATIC_ASSERT(atomic<short>::is_always_lock_free);
STATIC_ASSERT(atomic<unsigned short>::is_always_lock_free);
STATIC_ASSERT(atomic<int>::is_always_lock_free);
STATIC_ASSERT(atomic<unsigned int>::is_always_lock_free);
STATIC_ASSERT(atomic<float>::is_always_lock_free);
STATIC_ASSERT(atomic<long>::is_always_lock_free);
STATIC_ASSERT(atomic<unsigned long>::is_always_lock_free);
STATIC_ASSERT(atomic<long long>::is_always_lock_free);
STATIC_ASSERT(atomic<unsigned long long>::is_always_lock_free);
STATIC_ASSERT(atomic<double>::is_always_lock_free);
STATIC_ASSERT(atomic<long double>::is_always_lock_free);
STATIC_ASSERT(atomic<Bytes<1>*>::is_always_lock_free);
STATIC_ASSERT(atomic<void*>::is_always_lock_free);
STATIC_ASSERT(atomic<int (*)(int)>::is_always_lock_free);
#endif // _HAS_CXX17


// Also test P0418R2 atomic compare_exchange memory_order Requirements
void test_compare_exchange_relaxed_memory_orders() {
    // assert that none of the following go to terminate
    Bytes<1> b1{};
    atomic<Bytes<1>>{}.compare_exchange_strong(b1, b1, memory_order_relaxed, memory_order_acquire);
    Bytes<2> b2{};
    atomic<Bytes<2>>{}.compare_exchange_strong(b2, b2, memory_order_relaxed, memory_order_acquire);
    Bytes<4> b4{};
    atomic<Bytes<4>>{}.compare_exchange_strong(b4, b4, memory_order_relaxed, memory_order_acquire);
    Bytes<8> b8{};
    atomic<Bytes<8>>{}.compare_exchange_strong(b8, b8, memory_order_relaxed, memory_order_acquire);
}

// Also test VSO-453574 "<atomic> std::atomic load does not work with non-default constructible types"
struct non_default_ctor_able {
    non_default_ctor_able() = delete;
    explicit non_default_ctor_able(int) {}
    non_default_ctor_able(const non_default_ctor_able&) = default;
    non_default_ctor_able& operator=(const non_default_ctor_able&) = default;
    ~non_default_ctor_able()                                       = default;
};

void test_atomic_load_non_default_constructible() {
    atomic<non_default_ctor_able> atm{non_default_ctor_able{123}};
    (void) atm.load();
    volatile atomic<non_default_ctor_able> atmV{non_default_ctor_able{123}};
    (void) atmV.load();
}

void test_p0883_atomic_value_init() {
    {
        alignas(atomic<int>) unsigned char buf[sizeof(atomic<int>)];
        buf[0]   = 42;
        buf[1]   = 43;
        auto ptr = new (&buf[0]) atomic<int>;
        assert(ptr->load() == 0);
    }

    {
        alignas(atomic<void*>) unsigned char buf[sizeof(atomic<void*>)];
        buf[0]   = 42;
        buf[1]   = 43;
        auto ptr = new (&buf[0]) atomic<void*>;
        assert(ptr->load() == nullptr);
    }

    {
        alignas(atomic<X>) unsigned char buf[sizeof(atomic<X>)];
        buf[0]   = 42;
        buf[1]   = 43;
        auto ptr = new (&buf[0]) atomic<X>;
        assert(ptr->load().n == 0);
    }
}

void test_layout_paranoia() {
    // Verify that we haven't broken ABI
    // NB: This is in NO WAY testing standard behavior.
    static_assert(sizeof(atomic_flag) == sizeof(long), "ABI BREAK");

    atomic<Bytes<3>> a{{1, 2, 3}};
    static_assert(alignof(atomic<Bytes<3>>) == alignof(long), "ABI BREAK");
    static_assert(sizeof(atomic<Bytes<3>>) == 2 * sizeof(long), "ABI BREAK");
    assert(reinterpret_cast<long&>(a) == 0); // first the spinlock
    const auto b = reinterpret_cast<const char*>(&a) + sizeof(long);
    assert(b[0] == 1); // then the data
    assert(b[1] == 2);
    assert(b[2] == 3);
}

void assert_bitwise_identical(const char* const msg, const double lhs, const double rhs) {
    if (memcmp(&lhs, &rhs, sizeof(lhs)) != 0) {
        printf("counterexample found in %s: %a and %a\n", msg, lhs, rhs);
        abort();
    }
}

void test_double_identical_results() {
#if _HAS_CXX20
    static constexpr double cases[][2] = {
        {0.0, 0.0},
        {numeric_limits<double>::infinity(), 0.0},
        {0.0, numeric_limits<double>::infinity()},
        {numeric_limits<double>::quiet_NaN(), 1.0},
        {1.0, numeric_limits<double>::quiet_NaN()},
        {-numeric_limits<double>::infinity(), 0.0},
        {0.0, -numeric_limits<double>::infinity()},
        {-numeric_limits<double>::quiet_NaN(), 1.0},
        {1.0, -numeric_limits<double>::quiet_NaN()},
        {numeric_limits<double>::lowest(), 0.0},
        {numeric_limits<double>::min(), 0.0},
        {numeric_limits<double>::min(), 1.0},
        {1.0, numeric_limits<double>::min()},
        {numeric_limits<double>::denorm_min(), 0.0},
        {numeric_limits<double>::denorm_min(), 1.0},
        {1.0, numeric_limits<double>::denorm_min()},
        {numeric_limits<double>::epsilon(), 1.0},
    };

    for (const auto& [x, y] : cases) {
        atomic add{x};
        assert_bitwise_identical("add before", add.fetch_add(y), x);
        assert_bitwise_identical("add after", add.load(), x + y);

        atomic sub{x};
        assert_bitwise_identical("sub before", sub.fetch_sub(y), x);
        assert_bitwise_identical("sub after", sub.load(), x - y);

        atomic addOp{x};
        auto addOpControl = x;
        assert_bitwise_identical("opAdd", addOp += y, addOpControl += y);

        atomic subOp{x};
        auto subOpControl = x;
        assert_bitwise_identical("opSub", subOp -= y, subOpControl -= y);
    }
#endif // _HAS_CXX20
}

int main() {
    X x = {1729};
    test(x);

    int i = 1729;
    test(i);

    int* p = nullptr;
    test(p);

    test_operator_assign();

    test_alignment();

    test_nullptr_compares();

    test_compare_exchange_relaxed_memory_orders();

    test_atomic_load_non_default_constructible();

    test_p0883_atomic_value_init();

    test_layout_paranoia();

    test_double_identical_results();
}
