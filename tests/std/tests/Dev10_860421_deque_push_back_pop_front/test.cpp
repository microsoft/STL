// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <deque>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

void test_push_back_pop_front() {
    deque<int> d;

    for (int n = 0; n < 1000; ++n) {
        d.push_back(n);

        if (d.size() < 26) {
            continue;
        }

        vector<deque<int>::iterator> v;

        for (deque<int>::iterator i = d.begin() + 1; i != d.end(); ++i) {
            v.push_back(i);
        }

        d.pop_front();

        for (deque<int>::size_type i = 0; i < d.size(); ++i) {
            assert(d[i] == *v[i]);
        }
    }
}

// Also test Dev10-391805 "STL: Prefast error in deque".

void test_Dev10_391805() {
    deque<int> d;

    d.push_back(10);
    d.push_back(20);
    d.push_front(30);
    d.push_front(40);

    assert(d.size() == 4 && d[0] == 40 && d[1] == 30 && d[2] == 10 && d[3] == 20);
}

// Also test GH-1023 "<deque>: std::deque::insert performance"
// - support for single-element insertion of non-swappable type, and
// - exception safety for single-element insertion.

struct ThrowingConstructionTag {
    explicit ThrowingConstructionTag() = default;
};

struct UniqueError {
    explicit UniqueError() = default;
};

class NonswappableMovable {
public:
    NonswappableMovable() = default;
    NonswappableMovable(NonswappableMovable&& other) noexcept : payload{exchange(other.payload, -1)} {}
    NonswappableMovable(const NonswappableMovable&) = default;

    explicit NonswappableMovable(int n) noexcept : payload{n} {}
    explicit NonswappableMovable(ThrowingConstructionTag) {
        throw UniqueError{};
    }

    NonswappableMovable& operator=(NonswappableMovable&& other) noexcept {
        payload = exchange(other.payload, -1);
        return *this;
    }
    NonswappableMovable& operator=(const NonswappableMovable&) = default;

#if _HAS_CXX20
    friend bool operator==(const NonswappableMovable&, const NonswappableMovable&) = default;
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    friend bool operator==(const NonswappableMovable& lhs, const NonswappableMovable& rhs) noexcept {
        return lhs.payload == rhs.payload;
    }

    friend bool operator!=(const NonswappableMovable& lhs, const NonswappableMovable& rhs) noexcept {
        return lhs.payload != rhs.payload;
    }
#endif // ^^^ !_HAS_CXX20 ^^^

    friend void swap(NonswappableMovable&, NonswappableMovable&) = delete;

private:
    int payload = -1;
};

#if _HAS_CXX17
static_assert(!is_swappable_v<NonswappableMovable>);
#endif // _HAS_CXX17

void test_exception_safety_for_nonswappable_movable() {
    using Diff = deque<NonswappableMovable>::difference_type;

    deque<NonswappableMovable> d;
    for (int i = 0; i < 10; ++i) {
        d.emplace_back(i);
    }

    {
        auto it = d.emplace(d.begin(), 33);
        assert(it == d.begin());
        assert(d.front() == NonswappableMovable{33});
    }
    {
        auto it = d.emplace(d.begin() + Diff{3}, 42);
        assert(it == d.begin() + Diff{3});
        assert(d[3] == NonswappableMovable{42});
    }
    {
        auto it = d.emplace(d.end() - Diff{3}, 1729);
        assert(it == d.end() - Diff{4});
        assert(d[d.size() - 4] == NonswappableMovable{1729});
    }
    {
        auto it = d.emplace(d.end(), 2023);
        assert(it == d.end() - Diff{1});
        assert(d.back() == NonswappableMovable{2023});
    }
    {
        static constexpr int correct[] = {33, 0, 1, 42, 2, 3, 4, 5, 6, 1729, 7, 8, 9, 2023};
        auto comp = [](const NonswappableMovable& lhs, const int rhs) { return lhs == NonswappableMovable{rhs}; };
        assert(equal(d.begin(), d.end(), begin(correct), end(correct), comp));
    }

    const auto d_orig = d;
    try {
        d.emplace_front(ThrowingConstructionTag{});
        assert(false);
    } catch (const UniqueError&) {
    }
    assert(d == d_orig);

    try {
        d.emplace_back(ThrowingConstructionTag{});
        assert(false);
    } catch (const UniqueError&) {
    }
    assert(d == d_orig);

    try {
        d.emplace(d.begin(), ThrowingConstructionTag{});
        assert(false);
    } catch (const UniqueError&) {
    }
    assert(d == d_orig);

    try {
        d.emplace(d.begin() + Diff{2}, ThrowingConstructionTag{});
        assert(false);
    } catch (const UniqueError&) {
    }
    assert(d == d_orig);

    try {
        d.emplace(d.end() - Diff{2}, ThrowingConstructionTag{});
        assert(false);
    } catch (const UniqueError&) {
    }
    assert(d == d_orig);

    try {
        d.emplace(d.end(), ThrowingConstructionTag{});
        assert(false);
    } catch (const UniqueError&) {
    }
    assert(d == d_orig);
}

class ThrowingMovable {
public:
    ThrowingMovable() = default;
    ThrowingMovable(ThrowingMovable&&) {
        throw UniqueError{};
    }
    ThrowingMovable(const ThrowingMovable&) = default;

    explicit ThrowingMovable(int n) noexcept : payload{n} {}

    ThrowingMovable& operator=(ThrowingMovable&&) {
        throw UniqueError{};
    }
    ThrowingMovable& operator=(const ThrowingMovable&) = default;

#if _HAS_CXX20
    friend bool operator==(const ThrowingMovable&, const ThrowingMovable&) = default;
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    friend bool operator==(const ThrowingMovable& lhs, const ThrowingMovable& rhs) noexcept {
        return lhs.payload == rhs.payload;
    }

    friend bool operator!=(const ThrowingMovable& lhs, const ThrowingMovable& rhs) noexcept {
        return lhs.payload != rhs.payload;
    }
#endif // ^^^ !_HAS_CXX20 ^^^

private:
    int payload = -1;
};

void test_exception_safety_for_throwing_movable() {
    using Diff = deque<ThrowingMovable>::difference_type;

    deque<ThrowingMovable> d;
    for (int i = 0; i < 10; ++i) {
        d.emplace_back(i);
    }

    const auto d_orig = d;
    try {
        d.emplace_front(ThrowingMovable{});
        assert(false);
    } catch (const UniqueError&) {
    }
    assert(d == d_orig);

    try {
        d.emplace_back(ThrowingMovable{});
        assert(false);
    } catch (const UniqueError&) {
    }
    assert(d == d_orig);

    try {
        d.emplace(d.begin(), ThrowingMovable{});
        assert(false);
    } catch (const UniqueError&) {
    }
    assert(d == d_orig);

    try {
        d.emplace(d.begin() + Diff{2}, ThrowingMovable{});
        assert(false);
    } catch (const UniqueError&) {
    }
    assert(d == d_orig);

    try {
        d.emplace(d.end() - Diff{2}, ThrowingMovable{});
        assert(false);
    } catch (const UniqueError&) {
    }
    assert(d == d_orig);

    try {
        d.emplace(d.end(), ThrowingMovable{});
        assert(false);
    } catch (const UniqueError&) {
    }
    assert(d == d_orig);
}

// Also test GH-4072: <deque>: shrink_to_fit() should follow the Standard
void test_gh_4072() {
    {
        constexpr int removed_count = 768;

        deque<ThrowingMovable> d;
        for (int i = 0; i < 1729; ++i) {
            d.emplace_back(i);
        }

        for (int i = 0; i < removed_count; ++i) {
            d.pop_front();
            d.pop_back();
        }

        deque<ThrowingMovable> d2;
        for (int i = removed_count; i < 1729 - removed_count; ++i) {
            d2.emplace_back(i);
        }

        d.shrink_to_fit(); // ensures that no constructor or assignment operator of the element type is called
        assert(d == d2);
    }

    // ensure that the circular buffer is correctly handled
    {
        deque<ThrowingMovable> deq(128);
        deq.pop_back();
        deq.emplace_front(0);
        deq.shrink_to_fit();
    }
    {
        deque<ThrowingMovable> deq(128);
        for (int i = 0; i < 120; ++i) {
            deq.pop_back();
        }
        for (int i = 0; i < 5; ++i) {
            deq.emplace_front(0);
        }
        deq.shrink_to_fit();
    }
}

int main() {
    test_push_back_pop_front();

    test_Dev10_391805();

    test_exception_safety_for_nonswappable_movable();
    test_exception_safety_for_throwing_movable();

    test_gh_4072();
}
