// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <execution>
#include <functional>
#include <memory>
#include <numeric>
#include <type_traits>
#include <vector>

struct bigint {
    int value;
    int more_value[10];

    bigint(int value_ = 0) : value(value_) {}

    operator int() const {
        return value;
    }
};

struct int128 {
    long long value;
    long long more_value;

    int128(int value_ = 0) : value(value_), more_value(0) {}

    operator int() const {
        return static_cast<int>(value);
    }
};

// Also test constraints and conditional existence of difference_type specified by
// P3323R1 "Forbid atomic<cv T>, Specify atomic_ref<cv T>".

template <class T>
void test_atomic_ref_constraints_single() { // COMPILE-ONLY
    using TD = std::remove_cv_t<T>;
    using AR = std::atomic_ref<T>;

    static_assert(std::is_same_v<typename AR::value_type, TD>);
    static_assert(requires(const AR& r, TD v, std::memory_order ord) {
#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-2574432
        r.operator TD();
#endif // ^^^ no workaround ^^^
        { r.load() } -> std::same_as<TD>;
        { r.load(ord) } -> std::same_as<TD>;
        { r.wait(v) } -> std::same_as<void>;
        { r.wait(v, ord) } -> std::same_as<void>;
    });
    {
        [[maybe_unused]] auto instantiator = [](const AR& r, TD v, std::memory_order ord) {
            (void) r.operator TD();
            (void) r.load();
            (void) r.load(ord);
            r.wait(v);
            r.wait(v, ord);
        };
    }

    if constexpr (!std::is_const_v<T>) {
        static_assert(requires(const AR& r, TD v, TD& vx, std::memory_order ord1, std::memory_order ord2) {
            { r.store(v) } -> std::same_as<void>;
            { r.store(v, ord1) } -> std::same_as<void>;
            { r = v } -> std::same_as<TD>;
            { r.exchange(v) } -> std::same_as<TD>;
            { r.exchange(v, ord1) } -> std::same_as<TD>;
            { r.compare_exchange_weak(vx, v) } -> std::same_as<bool>;
            { r.compare_exchange_weak(vx, v, ord1) } -> std::same_as<bool>;
            { r.compare_exchange_weak(vx, v, ord1, ord2) } -> std::same_as<bool>;
            { r.compare_exchange_strong(vx, v) } -> std::same_as<bool>;
            { r.compare_exchange_strong(vx, v, ord1) } -> std::same_as<bool>;
            { r.compare_exchange_strong(vx, v, ord1, ord2) } -> std::same_as<bool>;
            { r.notify_one() } -> std::same_as<void>;
            { r.notify_all() } -> std::same_as<void>;
        });

        [[maybe_unused]] auto instantiator = [](const AR& r, TD v, TD& vx, std::memory_order ord1,
                                                 std::memory_order ord2) {
            r.store(v);
            r.store(v, ord1);
            (void) (r = v);
            (void) r.exchange(v);
            (void) r.exchange(v, ord1);
            (void) r.compare_exchange_weak(vx, v);
            (void) r.compare_exchange_weak(vx, v, ord1);
            (void) r.compare_exchange_weak(vx, v, ord1, ord2);
            (void) r.compare_exchange_strong(vx, v);
            (void) r.compare_exchange_strong(vx, v, ord1);
            (void) r.compare_exchange_strong(vx, v, ord1, ord2);
            r.notify_one();
            r.notify_all();
        };
    } else {
        static_assert(!requires(const AR& r, TD v) { r.store(v); });
        static_assert(!requires(const AR& r, TD v, std::memory_order ord) { r.store(v, ord); });
        static_assert(!requires(const AR& r, TD v) { r = v; });
        static_assert(!requires(const AR& r, TD v) { r.exchange(v); });
        static_assert(!requires(const AR& r, TD v, std::memory_order ord) { r.exchange(v, ord); });
        static_assert(!requires(const AR& r, TD& v1, TD v2) { r.compare_exchange_weak(v1, v2); });
        static_assert(
            !requires(const AR& r, TD& v1, TD v2, std::memory_order ord) { r.compare_exchange_weak(v1, v2, ord); });
        static_assert(!requires(const AR& r, TD& v1, TD v2, std::memory_order ord1, std::memory_order ord2) {
            r.compare_exchange_weak(v1, v2, ord1, ord2);
        });
        static_assert(!requires(const AR& r, TD& v1, TD v2) { r.compare_exchange_strong(v1, v2); });
        static_assert(
            !requires(const AR& r, TD& v1, TD v2, std::memory_order ord) { r.compare_exchange_strong(v1, v2, ord); });
        static_assert(!requires(const AR& r, TD& v1, TD v2, std::memory_order ord1, std::memory_order ord2) {
            r.compare_exchange_strong(v1, v2, ord1, ord2);
        });
        static_assert(!requires(const AR& r) { r.notify_one(); });
        static_assert(!requires(const AR& r) { r.notify_all(); });
    }

    constexpr bool has_difference_type = (std::is_arithmetic_v<TD> && !std::is_same_v<TD, bool>)
                                      || (std::is_pointer_v<TD> && std::is_object_v<std::remove_pointer_t<TD>>);

    if constexpr (has_difference_type) {
        if constexpr (std::is_arithmetic_v<TD>) {
            static_assert(std::is_same_v<typename AR::difference_type, TD>);
        } else {
            static_assert(std::is_same_v<typename AR::difference_type, std::ptrdiff_t>);
        }
    } else {
        static_assert(!requires { typename AR::difference_type; });
    }

    if constexpr (has_difference_type && !std::is_const_v<T>) {
        static_assert(requires(const AR& r, AR::difference_type d, std::memory_order ord) {
            { r.fetch_add(d) } -> std::same_as<TD>;
            { r.fetch_add(d, ord) } -> std::same_as<TD>;
            { r.fetch_sub(d) } -> std::same_as<TD>;
            { r.fetch_sub(d, ord) } -> std::same_as<TD>;
            { r += d } -> std::same_as<TD>;
            { r -= d } -> std::same_as<TD>;
        });

        [[maybe_unused]] auto instantiator = [](const AR& r, AR::difference_type d, std::memory_order ord) {
            (void) r.fetch_add(d);
            (void) r.fetch_add(d, ord);
            (void) r.fetch_sub(d);
            (void) r.fetch_sub(d, ord);
            (void) (r += d);
            (void) (r -= d);
        };
    } else {
        static_assert(!requires(const AR& r, AR::difference_type d) { r.fetch_add(d); });
        static_assert(!requires(const AR& r, AR::difference_type d, std::memory_order ord) { r.fetch_add(d, ord); });
        static_assert(!requires(const AR& r, AR::difference_type d) { r.fetch_sub(d); });
        static_assert(!requires(const AR& r, AR::difference_type d, std::memory_order ord) { r.fetch_sub(d, ord); });
        static_assert(!requires(const AR& r, AR::difference_type d) { r += d; });
        static_assert(!requires(const AR& r, AR::difference_type d) { r -= d; });
    }

    if constexpr (has_difference_type && !std::is_floating_point_v<TD> && !std::is_const_v<T>) {
        static_assert(requires(const AR& r) {
            { ++r } -> std::same_as<TD>;
            { r++ } -> std::same_as<TD>;
            { --r } -> std::same_as<TD>;
            { r-- } -> std::same_as<TD>;
        });

        [[maybe_unused]] auto instantiator = [](const AR& r) {
            (void) ++r;
            (void) r++;
            (void) --r;
            (void) r--;
        };
    } else {
        static_assert(!requires(const AR& r) { ++r; });
        static_assert(!requires(const AR& r) { r++; });
        static_assert(!requires(const AR& r) { --r; });
        static_assert(!requires(const AR& r) { r--; });
    }

    if constexpr (std::is_integral_v<TD> && !std::is_same_v<TD, bool> && !std::is_const_v<T>) {
        static_assert(requires(const AR& r, TD v, std::memory_order ord) {
            { r.fetch_and(v) } -> std::same_as<TD>;
            { r.fetch_and(v, ord) } -> std::same_as<TD>;
            { r.fetch_or(v) } -> std::same_as<TD>;
            { r.fetch_or(v, ord) } -> std::same_as<TD>;
            { r.fetch_xor(v) } -> std::same_as<TD>;
            { r.fetch_xor(v, ord) } -> std::same_as<TD>;
            { r &= v } -> std::same_as<TD>;
            { r |= v } -> std::same_as<TD>;
            { r ^= v } -> std::same_as<TD>;
        });

        [[maybe_unused]] auto instantiator = [](const AR& r, TD v, std::memory_order ord) {
            (void) r.fetch_and(v);
            (void) r.fetch_and(v, ord);
            (void) r.fetch_or(v);
            (void) r.fetch_or(v, ord);
            (void) r.fetch_xor(v);
            (void) r.fetch_xor(v, ord);
            (void) (r &= v);
            (void) (r |= v);
            (void) (r ^= v);
        };
    } else {
        static_assert(!requires(const AR& r, TD v) { r.fetch_and(v); });
        static_assert(!requires(const AR& r, TD v, std::memory_order ord) { r.fetch_and(v, ord); });
        static_assert(!requires(const AR& r, TD v) { r.fetch_or(v); });
        static_assert(!requires(const AR& r, TD v, std::memory_order ord) { r.fetch_or(v, ord); });
        static_assert(!requires(const AR& r, TD v) { r.fetch_xor(v); });
        static_assert(!requires(const AR& r, TD v, std::memory_order ord) { r.fetch_xor(v, ord); });
        static_assert(!requires(const AR& r, TD v) { r &= v; });
        static_assert(!requires(const AR& r, TD v) { r |= v; });
        static_assert(!requires(const AR& r, TD v) { r ^= v; });
    }
}

template <class T>
void test_atomic_ref_constraints_cv() { // COMPILE-ONLY
    static_assert(!std::is_const_v<T> && !std::is_volatile_v<T>);
    test_atomic_ref_constraints_single<T>();
    test_atomic_ref_constraints_single<const T>();
    if constexpr (std::atomic_ref<T>::is_always_lock_free) {
        test_atomic_ref_constraints_single<volatile T>();
        test_atomic_ref_constraints_single<const volatile T>();
    }
}

void test_atomic_ref_constraints() { // COMPILE-ONLY
    test_atomic_ref_constraints_cv<signed char>();
    test_atomic_ref_constraints_cv<short>();
    test_atomic_ref_constraints_cv<int>();
    test_atomic_ref_constraints_cv<long>();
    test_atomic_ref_constraints_cv<long long>();
    test_atomic_ref_constraints_cv<unsigned char>();
    test_atomic_ref_constraints_cv<unsigned short>();
    test_atomic_ref_constraints_cv<unsigned int>();
    test_atomic_ref_constraints_cv<unsigned long>();
    test_atomic_ref_constraints_cv<unsigned long long>();
    test_atomic_ref_constraints_cv<bool>();
    test_atomic_ref_constraints_cv<char>();
    test_atomic_ref_constraints_cv<wchar_t>();
#ifdef __cpp_char8_t
    test_atomic_ref_constraints_cv<char8_t>();
#endif // defined(__cpp_char8_t)
    test_atomic_ref_constraints_cv<char16_t>();
    test_atomic_ref_constraints_cv<char32_t>();
    test_atomic_ref_constraints_cv<float>();
    test_atomic_ref_constraints_cv<double>();
    test_atomic_ref_constraints_cv<long double>();

    test_atomic_ref_constraints_cv<void*>();
    test_atomic_ref_constraints_cv<const void*>();
    test_atomic_ref_constraints_cv<volatile void*>();
    test_atomic_ref_constraints_cv<const volatile void*>();

    test_atomic_ref_constraints_cv<char*>();
    test_atomic_ref_constraints_cv<const char*>();
    test_atomic_ref_constraints_cv<volatile char*>();
    test_atomic_ref_constraints_cv<const volatile char*>();

    test_atomic_ref_constraints_cv<int*>();
    test_atomic_ref_constraints_cv<const int*>();
    test_atomic_ref_constraints_cv<volatile int*>();
    test_atomic_ref_constraints_cv<const volatile int*>();

    test_atomic_ref_constraints_cv<int128*>();
    test_atomic_ref_constraints_cv<const int128*>();
    test_atomic_ref_constraints_cv<volatile int128*>();
    test_atomic_ref_constraints_cv<const volatile int128*>();

    test_atomic_ref_constraints_cv<bigint*>();
    test_atomic_ref_constraints_cv<const bigint*>();
    test_atomic_ref_constraints_cv<volatile bigint*>();
    test_atomic_ref_constraints_cv<const volatile bigint*>();

    test_atomic_ref_constraints_cv<int (*)()>();
    test_atomic_ref_constraints_cv<bigint (*)(int128)>();

    test_atomic_ref_constraints_cv<std::nullptr_t>();

    test_atomic_ref_constraints_cv<bigint>();
    test_atomic_ref_constraints_cv<int128>();
}


// code reuse of ../P1135R6_atomic_flag_test/test.cpp

template <bool AddViaCas, typename ValueType>
void test_ops() {
    constexpr std::size_t unique      = 80; // small to avoid overflow even for char
    constexpr std::size_t repetitions = 8000;
    constexpr std::size_t total       = unique * repetitions;
    constexpr std::size_t range       = 10;

    struct alignas(std::atomic_ref<ValueType>::required_alignment) Padded {
        ValueType vals[unique] = {};
    };
    Padded padded;

    auto& vals = padded.vals;

    std::vector<std::atomic_ref<ValueType>> refs;
    refs.reserve(total);
    for (std::size_t i = 0; i != repetitions; ++i) {
        for (auto& val : vals) {
            refs.emplace_back(val);
        }
    }

    using std::execution::par;

    auto load  = [](const std::atomic_ref<ValueType>& ref) { return static_cast<int>(ref.load()); };
    auto xchg0 = [](std::atomic_ref<ValueType>& ref) { return static_cast<int>(ref.exchange(0)); };

    int (*inc)(std::atomic_ref<ValueType>& ref);
    if constexpr (AddViaCas) {
        inc = [](std::atomic_ref<ValueType>& ref) {
            for (;;) {
                ValueType e = ref.load();
                ValueType d = static_cast<ValueType>(static_cast<int>(e) + 1);
                if (ref.compare_exchange_weak(e, d)) {
                    return static_cast<int>(e);
                }
            }
        };
    } else {
        inc = [](std::atomic_ref<ValueType>& ref) { return static_cast<int>(ref.fetch_add(1)); };
    }

    assert(std::transform_reduce(par, refs.begin(), refs.end(), 0, std::plus{}, load) == 0);
    assert(std::transform_reduce(par, refs.begin(), refs.begin() + range, 0, std::plus{}, inc) == 0);
    assert(std::transform_reduce(par, refs.begin(), refs.end(), 0, std::plus{}, load) == range * repetitions);
    assert(std::transform_reduce(par, refs.begin(), refs.begin() + range, 0, std::plus{}, inc) == range);
    assert(std::transform_reduce(par, refs.begin(), refs.end(), 0, std::plus{}, load) == range * repetitions * 2);
    assert(std::transform_reduce(par, refs.begin(), refs.end(), 0, std::plus{}, xchg0) == range * 2);
    assert(std::transform_reduce(par, refs.begin(), refs.end(), 0, std::plus{}, load) == 0);
}

template <class Integer>
void test_int_ops() {
    Integer v = 0x40;

    std::atomic vx(v);
    std::atomic vy(v);
    const std::atomic_ref rx(v);
    const std::atomic_ref ry(v);

    assert(vx.fetch_add(0x10) == 0x40);
    assert(rx.fetch_add(0x10) == 0x40);

    assert(vx.load() == 0x50);
    assert(vy.load() == 0x40);
    assert(rx.load() == 0x50);
    assert(ry.load() == 0x50);

    assert(vx.fetch_sub(0x8) == 0x50);
    assert(rx.fetch_sub(0x8) == 0x50);

    assert(vx.load() == 0x48);
    assert(vy.load() == 0x40);
    assert(rx.load() == 0x48);
    assert(ry.load() == 0x48);

    assert(vx.fetch_or(0xF) == 0x48);
    assert(rx.fetch_or(0xF) == 0x48);

    assert(vx.load() == 0x4F);
    assert(vy.load() == 0x40);
    assert(rx.load() == 0x4F);
    assert(ry.load() == 0x4F);

    assert(vx.fetch_and(0x3C) == 0x4F);
    assert(rx.fetch_and(0x3C) == 0x4F);

    assert(vx.load() == 0xC);
    assert(vy.load() == 0x40);
    assert(rx.load() == 0xC);
    assert(ry.load() == 0xC);

    assert(vx.fetch_xor(0x3F) == 0xC);
    assert(rx.fetch_xor(0x3F) == 0xC);

    assert(vx.load() == 0x33);
    assert(vy.load() == 0x40);
    assert(rx.load() == 0x33);
    assert(ry.load() == 0x33);

    assert(vx-- == 0x33);
    assert(rx-- == 0x33);

    assert(vx.load() == 0x32);
    assert(vy.load() == 0x40);
    assert(rx.load() == 0x32);
    assert(ry.load() == 0x32);

    assert(--vx == 0x31);
    assert(--rx == 0x31);

    assert(vx.load() == 0x31);
    assert(vy.load() == 0x40);
    assert(rx.load() == 0x31);
    assert(ry.load() == 0x31);

    assert(vx++ == 0x31);
    assert(rx++ == 0x31);

    assert(vx.load() == 0x32);
    assert(vy.load() == 0x40);
    assert(rx.load() == 0x32);
    assert(ry.load() == 0x32);

    assert(++vx == 0x33);
    assert(++rx == 0x33);

    assert(vx.load() == 0x33);
    assert(vy.load() == 0x40);
    assert(rx.load() == 0x33);
    assert(ry.load() == 0x33);
}


template <class Float>
void test_float_ops() {
    Float v = 0x40;

    std::atomic vx(v);
    std::atomic vy(v);
    const std::atomic_ref rx(v);
    const std::atomic_ref ry(v);

    assert(vx.fetch_add(0x10) == 0x40);
    assert(rx.fetch_add(0x10) == 0x40);

    assert(vx.load() == 0x50);
    assert(vy.load() == 0x40);
    assert(rx.load() == 0x50);
    assert(ry.load() == 0x50);

    assert(vx.fetch_sub(0x8) == 0x50);
    assert(rx.fetch_sub(0x8) == 0x50);

    assert(vx.load() == 0x48);
    assert(vy.load() == 0x40);
    assert(rx.load() == 0x48);
    assert(ry.load() == 0x48);

    vx.store(0x10);
    rx.store(0x10);

    assert(vx.load() == 0x10);
    assert(vy.load() == 0x40);
    assert(rx.load() == 0x10);
    assert(ry.load() == 0x10);
}

template <class Ptr>
void test_ptr_ops() {
    std::remove_pointer_t<Ptr> a[0x100];
    Ptr v = a;

    std::atomic vx(v);
    std::atomic vy(v);
    const std::atomic_ref rx(v);
    const std::atomic_ref ry(v);

    assert(vx.fetch_add(0x10) == a);
    assert(rx.fetch_add(0x10) == a);

    assert(vx.load() == a + 0x10);
    assert(vy.load() == a);
    assert(rx.load() == a + 0x10);
    assert(ry.load() == a + 0x10);

    assert(vx.fetch_sub(0x8) == a + 0x10);
    assert(rx.fetch_sub(0x8) == a + 0x10);

    assert(vx.load() == a + 0x8);
    assert(vy.load() == a);
    assert(rx.load() == a + 0x8);
    assert(ry.load() == a + 0x8);

    vx.store(a + 0x10);
    rx.store(a + 0x10);

    assert(vx.load() == a + 0x10);
    assert(vy.load() == a);
    assert(rx.load() == a + 0x10);
    assert(ry.load() == a + 0x10);

    assert(vx-- == a + 0x10);
    assert(rx-- == a + 0x10);

    assert(vx.load() == a + 0xF);
    assert(vy.load() == a);
    assert(rx.load() == a + 0xF);
    assert(ry.load() == a + 0xF);

    assert(--vx == a + 0xE);
    assert(--rx == a + 0xE);

    assert(vx.load() == a + 0xE);
    assert(vy.load() == a);
    assert(rx.load() == a + 0xE);
    assert(ry.load() == a + 0xE);

    assert(vx++ == a + 0xE);
    assert(rx++ == a + 0xE);

    assert(vx.load() == a + 0xF);
    assert(vy.load() == a);
    assert(rx.load() == a + 0xF);
    assert(ry.load() == a + 0xF);

    assert(++vx == a + 0x10);
    assert(++rx == a + 0x10);

    assert(vx.load() == a + 0x10);
    assert(vy.load() == a);
    assert(rx.load() == a + 0x10);
    assert(ry.load() == a + 0x10);
}

// GH-1497 <atomic>: atomic_ref<const T> fails to compile
void test_gh_1497() {
    {
        static constexpr int ci{1729}; // static storage duration, so this is stored in read-only memory
        const std::atomic_ref atom{ci};
        assert(atom.load() == 1729);
    }

    {
        int i{11};
        const std::atomic_ref<int> atom_modify{i};
        const std::atomic_ref<const int> atom_observe{i};
        assert(atom_modify.load() == 11);
        assert(atom_observe.load() == 11);
        atom_modify.store(22);
        assert(atom_modify.load() == 22);
        assert(atom_observe.load() == 22);
    }
}

#ifndef _M_CEE // TRANSITION, VSO-1659496
// GH-140: STL: We should _STD qualify _Ugly function calls to avoid ADL
template <class T>
struct holder {
    T t;
};

struct incomplete;

template <class T, class Tag>
struct tagged_trivial {
    T t;
};

template <class T>
void test_incomplete_associated_class() { // COMPILE-ONLY
    T o{};
    std::atomic_ref<T> a{o};

    a = o;

    (void) a.is_lock_free();
    (void) a.load();
    (void) a.load(std::memory_order_relaxed);
    a.store(T{});
    a.store(T{}, std::memory_order_relaxed);
    (void) a.exchange(T{});
    (void) a.exchange(T{}, std::memory_order_relaxed);
    (void) a.compare_exchange_weak(o, T{});
    (void) a.compare_exchange_weak(o, T{}, std::memory_order_relaxed);
    (void) a.compare_exchange_weak(o, T{}, std::memory_order_relaxed, std::memory_order_relaxed);
    (void) a.compare_exchange_strong(o, T{});
    (void) a.compare_exchange_strong(o, T{}, std::memory_order_relaxed);
    (void) a.compare_exchange_strong(o, T{}, std::memory_order_relaxed, std::memory_order_relaxed);
    a.wait(T{});
    a.wait(T{}, std::memory_order_relaxed);
    a.notify_one();
    a.notify_all();

    if constexpr (std::is_pointer_v<T>) {
        std::remove_pointer_t<T> pointee{};
        a = std::addressof(pointee);

        (void) a.operator+=(0); // a += 0 triggers ADL
        (void) a.operator-=(0); // a -= 0 triggers ADL
        (void) a.operator++(); // ++a triggers ADL
        (void) a.operator--(); // --a triggers ADL
        (void) a.operator++(0); // a++ triggers ADL
        (void) a.operator--(0); // a-- triggers ADL
        (void) a.fetch_add(0);
        (void) a.fetch_add(0, std::memory_order_relaxed);
        (void) a.fetch_sub(0);
        (void) a.fetch_sub(0, std::memory_order_relaxed);
    }
}

void test_incomplete_associated_class_all() { // COMPILE-ONLY
    test_incomplete_associated_class<tagged_trivial<uint8_t, holder<incomplete>>>();
    test_incomplete_associated_class<tagged_trivial<uint16_t, holder<incomplete>>>();
    test_incomplete_associated_class<tagged_trivial<uint32_t, holder<incomplete>>>();
    test_incomplete_associated_class<tagged_trivial<uint64_t, holder<incomplete>>>();
    test_incomplete_associated_class<tagged_trivial<uint64_t[2], holder<incomplete>>>();

    test_incomplete_associated_class<tagged_trivial<uint8_t[3], holder<incomplete>>>();
    test_incomplete_associated_class<tagged_trivial<uint16_t[3], holder<incomplete>>>();
    test_incomplete_associated_class<tagged_trivial<uint32_t[3], holder<incomplete>>>();
    test_incomplete_associated_class<tagged_trivial<uint64_t[3], holder<incomplete>>>();

    test_incomplete_associated_class<tagged_trivial<uint8_t, holder<incomplete>>*>();
    test_incomplete_associated_class<tagged_trivial<uint16_t, holder<incomplete>>*>();
    test_incomplete_associated_class<tagged_trivial<uint32_t, holder<incomplete>>*>();
    test_incomplete_associated_class<tagged_trivial<uint64_t, holder<incomplete>>*>();
    test_incomplete_associated_class<tagged_trivial<uint64_t[2], holder<incomplete>>*>();

    test_incomplete_associated_class<tagged_trivial<uint8_t[3], holder<incomplete>>*>();
    test_incomplete_associated_class<tagged_trivial<uint16_t[3], holder<incomplete>>*>();
    test_incomplete_associated_class<tagged_trivial<uint32_t[3], holder<incomplete>>*>();
    test_incomplete_associated_class<tagged_trivial<uint64_t[3], holder<incomplete>>*>();
}
#endif // ^^^ no workaround ^^^

// GH-4472 "<atomic>: With _STD_ATOMIC_ALWAYS_USE_CMPXCHG16B defined to 1,
// atomic_ref<16 bytes> does not report is_lock_free and is_always_lock_free correctly"
void test_gh_4472() {
    struct two_pointers_t {
        void* left;
        void* right;
    };

    alignas(std::atomic_ref<two_pointers_t>::required_alignment) two_pointers_t two_pointers;

    static_assert(std::atomic_ref<two_pointers_t>::required_alignment == sizeof(two_pointers_t));

    static_assert(std::atomic_ref<two_pointers_t>::is_always_lock_free);

    std::atomic_ref<two_pointers_t> ar{two_pointers};
    assert(ar.is_lock_free());
}

// GH-4728 "<atomic>: On x64, atomic_ref::is_lock_free() incorrectly returns true when it shouldn't"
void test_gh_4728() {
    struct Large {
        char str[100]{};
    };

    alignas(std::atomic_ref<Large>::required_alignment) Large lg{};

    static_assert(std::atomic_ref<Large>::required_alignment == alignof(Large));

    static_assert(!std::atomic_ref<Large>::is_always_lock_free);

    std::atomic_ref<Large> ar{lg};
    assert(!ar.is_lock_free());
}

int main() {
    test_ops<false, char>();
    test_ops<false, signed char>();
    test_ops<false, unsigned char>();
    test_ops<false, short>();
    test_ops<false, unsigned short>();
    test_ops<false, int>();
    test_ops<false, unsigned int>();
    test_ops<false, long>();
    test_ops<false, unsigned long>();
    test_ops<false, long long>();
    test_ops<false, unsigned long long>();
    test_ops<false, float>();
    test_ops<false, double>();
    test_ops<false, long double>();

    test_ops<true, char>();
    test_ops<true, signed char>();
    test_ops<true, unsigned char>();
    test_ops<true, short>();
    test_ops<true, unsigned short>();
    test_ops<true, int>();
    test_ops<true, unsigned int>();
    test_ops<true, long>();
    test_ops<true, unsigned long>();
    test_ops<true, long long>();
    test_ops<true, unsigned long long>();
    test_ops<true, float>();
    test_ops<true, double>();
    test_ops<true, long double>();
    test_ops<true, bigint>();
    test_ops<true, int128>();

    test_int_ops<signed char>();
    test_int_ops<unsigned char>();
    test_int_ops<short>();
    test_int_ops<unsigned short>();
    test_int_ops<int>();
    test_int_ops<unsigned int>();
    test_int_ops<long>();
    test_int_ops<unsigned long>();
    test_int_ops<long long>();
    test_int_ops<unsigned long long>();

    test_float_ops<float>();
    test_float_ops<double>();
    test_float_ops<long double>();

    test_ptr_ops<char*>();
    test_ptr_ops<long*>();

    test_gh_1497();
    test_gh_4472();
    test_gh_4728();
}
