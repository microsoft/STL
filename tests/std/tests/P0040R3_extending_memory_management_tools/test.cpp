// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING

#include <algorithm>
#include <cassert>
#include <iterator>
#include <memory>
#include <new>
#include <type_traits>

using namespace std;

constexpr int exampleCount        = 20;
constexpr char fillChar           = 0x7d;
constexpr char nonTrivialInitChar = 10;

struct Trivial {
    char c;
};
struct NonTrivial {
    char c = nonTrivialInitChar;
};

const auto booleanTestsTrue     = [](const auto& x) { return static_cast<bool>(x); };
const auto isFillChar           = [](char c) { return c == fillChar; };
const auto isZeroChar           = [](char c) { return c == 0; };
const auto isNonTrivialInitChar = [](char c) { return c == nonTrivialInitChar; };

template <typename Target>
struct uninitialized_fixture {
    static_assert(sizeof(Target) == 1);
    static_assert(alignof(Target) == 1);
    static_assert(is_trivially_destructible_v<Target>);

    char storage[exampleCount];

    uninitialized_fixture() {
        fill(std::begin(storage), std::end(storage), fillChar);
    }

    Target* begin() {
        return &reinterpret_cast<Target&>(storage[0]);
    }

    Target* end() {
        return begin() + size(storage);
    }

    template <typename Fn>
    void assert_all_of(Fn fn) {
        assert(all_of(std::begin(storage), std::end(storage), fn));
    }
};

template <typename T, size_t Count>
struct uninitialized_storage {
    alignas(T) char storage[sizeof(T) * Count];

    uninitialized_storage() {
        fill(std::begin(storage), std::end(storage), fillChar);
    }

    T* begin() {
        return &reinterpret_cast<T&>(storage);
    }

    T* end() {
        return begin() + Count;
    }
};

void test_uninitialized_move() {
    unique_ptr<int> ptrs[exampleCount];
    for (auto& ptr : ptrs) {
        ptr = make_unique<int>(52);
    }

    assert(all_of(begin(ptrs), end(ptrs), booleanTestsTrue));
    uninitialized_storage<unique_ptr<int>, exampleCount> storage;
    assert(
        uninitialized_move(begin(ptrs), end(ptrs), stdext::make_checked_array_iterator(storage.begin(), exampleCount))
            .base()
        == storage.end());

    assert(none_of(begin(ptrs), end(ptrs), booleanTestsTrue));
    assert(all_of(storage.begin(), storage.end(), booleanTestsTrue));
    destroy(storage.begin(), storage.end());
}

void test_uninitialized_move_n() {
    unique_ptr<int> ptrs[exampleCount];
    for (auto& ptr : ptrs) {
        ptr = make_unique<int>(52);
    }

    assert(all_of(begin(ptrs), end(ptrs), booleanTestsTrue));

    uninitialized_storage<unique_ptr<int>, exampleCount> storage;
    const auto result = uninitialized_move_n(
        begin(ptrs), exampleCount, stdext::make_checked_array_iterator(storage.begin(), exampleCount));

    assert(result.first == end(ptrs));
    assert(result.second.base() == storage.end());
    assert(none_of(begin(ptrs), end(ptrs), booleanTestsTrue));
    assert(all_of(storage.begin(), storage.end(), booleanTestsTrue));
    destroy(storage.begin(), storage.end());
}

void test_uninitialized_value_construct() {
    {
        uninitialized_fixture<Trivial> storage;
        uninitialized_value_construct(storage.begin(), storage.end());
        storage.assert_all_of(isZeroChar);
    }

    {
        uninitialized_fixture<NonTrivial> storage;
        uninitialized_value_construct(storage.begin(), storage.end());
        storage.assert_all_of(isNonTrivialInitChar);
    }
}

void test_uninitialized_value_construct_n() {
    {
        uninitialized_fixture<Trivial> storage;
        assert(uninitialized_value_construct_n(storage.begin(), exampleCount) == storage.end());
        storage.assert_all_of(isZeroChar);
    }

    {
        uninitialized_fixture<NonTrivial> storage;
        assert(uninitialized_value_construct_n(storage.begin(), exampleCount) == storage.end());
        storage.assert_all_of(isNonTrivialInitChar);
    }
}

void test_uninitialized_default_construct() {
    {
        uninitialized_fixture<Trivial> storage;
        uninitialized_default_construct(storage.begin(), storage.end());
        storage.assert_all_of(isFillChar); // that is, nothing happened
    }

    {
        uninitialized_fixture<NonTrivial> storage;
        uninitialized_default_construct(storage.begin(), storage.end());
        storage.assert_all_of(isNonTrivialInitChar);
    }
}

void test_uninitialized_default_construct_n() {
    {
        uninitialized_fixture<Trivial> storage;
        assert(uninitialized_default_construct_n(storage.begin(), exampleCount) == storage.end());
        storage.assert_all_of(isFillChar); // that is, nothing happened
    }

    {
        uninitialized_fixture<NonTrivial> storage;
        assert(uninitialized_default_construct_n(storage.begin(), exampleCount) == storage.end());
        storage.assert_all_of(isNonTrivialInitChar);
    }
}

int g_alive = 0;
struct alive_counter {
    alive_counter() {
        ++g_alive;
    }
    alive_counter(const alive_counter&)            = delete;
    alive_counter& operator=(const alive_counter&) = delete;
    ~alive_counter() {
        --g_alive;
    }
};

void test_destroy_at() {
    uninitialized_storage<alive_counter, 1> storage;
    ::new (storage.begin()) alive_counter;
    assert(g_alive == 1);
    destroy_at(storage.begin());
    assert(g_alive == 0);
}

void test_destroy() {
    uninitialized_storage<alive_counter, exampleCount> storage;
    uninitialized_value_construct(storage.begin(), storage.end());
    assert(g_alive == exampleCount);
    destroy(storage.begin(), storage.end());
    assert(g_alive == 0);
}

void test_destroy_n() {
    uninitialized_storage<alive_counter, exampleCount> storage;
    uninitialized_value_construct(storage.begin(), storage.end());
    assert(g_alive == exampleCount);
    assert(destroy_n(storage.begin(), exampleCount) == storage.end());
    assert(g_alive == 0);
}

struct copy_elision_dest;

class pinned {
public:
    explicit pinned(int n) : n_{n} {}

    pinned(const pinned&)            = delete;
    pinned& operator=(const pinned&) = delete;

private:
    friend copy_elision_dest;

    int n_;
};

class pinned_ioterator {
private:
    struct arrow_proxy {
        pinned val_;

        pinned* operator->() {
            return &val_;
        }
    };

public:
    using iterator_category = input_iterator_tag;
    using difference_type   = int;
    using value_type        = pinned;
    using pointer           = arrow_proxy;
    using reference         = pinned;

    explicit pinned_ioterator(int n) : n_{n} {}

    pinned operator*() const {
        return pinned{n_};
    }
    pinned_ioterator& operator++() {
        ++n_;
        return *this;
    }
    pinned_ioterator operator++(int) {
        auto old = *this;
        ++*this;
        return old;
    }

    arrow_proxy operator->() const {
        return arrow_proxy{pinned{n_}};
    }

    friend bool operator==(pinned_ioterator i, pinned_ioterator j) {
        return i.n_ == j.n_;
    }
#if !_HAS_CXX20
    friend bool operator!=(pinned_ioterator i, pinned_ioterator j) {
        return !(i == j);
    }
#endif // !_HAS_CXX20

private:
    int n_;
};

struct copy_elision_dest {
    explicit copy_elision_dest(pinned x) : n_{x.n_} {}

    int n_;
};

// std::uninitialized_copy/_n are required to perform guaranteed copy elision since C++17.
void test_guaranteed_copy_elision_uninitialized_copy() {
    constexpr int len = 42;

    uninitialized_storage<copy_elision_dest, len> us;
    uninitialized_copy(pinned_ioterator{0}, pinned_ioterator{len}, us.begin());
    for (int i = 0; i != len; ++i) {
        assert(us.begin()[i].n_ == i);
    }
    destroy(us.begin(), us.end());
}

void test_guaranteed_copy_elision_uninitialized_copy_n() {
    constexpr int len = 42;

    uninitialized_storage<copy_elision_dest, len> us;
    uninitialized_copy_n(pinned_ioterator{0}, len, us.begin());
    for (int i = 0; i != len; ++i) {
        assert(us.begin()[i].n_ == i);
    }
    destroy(us.begin(), us.end());
}

// Also test LWG-3918 "std::uninitialized_move/_n and guaranteed copy elision".
void test_guaranteed_copy_elision_uninitialized_move() {
    constexpr int len = 42;

    uninitialized_storage<copy_elision_dest, len> us;
    uninitialized_move(pinned_ioterator{0}, pinned_ioterator{len}, us.begin());
    for (int i = 0; i != len; ++i) {
        assert(us.begin()[i].n_ == i);
    }
    destroy(us.begin(), us.end());
}

void test_guaranteed_copy_elision_uninitialized_move_n() {
    constexpr int len = 42;

    uninitialized_storage<copy_elision_dest, len> us;
    uninitialized_move_n(pinned_ioterator{0}, len, us.begin());
    for (int i = 0; i != len; ++i) {
        assert(us.begin()[i].n_ == i);
    }
    destroy(us.begin(), us.end());
}

int main() {
    test_uninitialized_move();
    test_uninitialized_move_n();
    test_uninitialized_value_construct();
    test_uninitialized_value_construct_n();
    test_uninitialized_default_construct();
    test_uninitialized_default_construct_n();
    test_destroy_at();
    test_destroy();
    test_destroy_n();

    test_guaranteed_copy_elision_uninitialized_copy();
    test_guaranteed_copy_elision_uninitialized_copy_n();
    test_guaranteed_copy_elision_uninitialized_move();
    test_guaranteed_copy_elision_uninitialized_move_n();
}
