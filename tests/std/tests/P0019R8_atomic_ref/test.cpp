// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <execution>
#include <functional>
#include <numeric>
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


// code reuse of ../P1135R6_atomic_flag_test/test.cpp

template <bool AddViaCas, typename ValueType>
void test_ops() {
#ifndef _M_CEE // TRANSITION, VSO-1659408
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
#endif // _M_CEE
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
}
