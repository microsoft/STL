// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iterator>
#include <memory>
#include <random>
#include <type_traits>

using namespace std;

size_t counter = 0;

template <class T, class Diff = ptrdiff_t>
class counting_ptr {
private:
    T* p_;

    explicit counting_ptr(T* raw_ptr) noexcept : p_(raw_ptr) {
        ++counter;
    }

    template <class U>
    friend struct ptr_counting_allocator;

    template <class U>
    friend struct inconsistent_difference_allocator;

public:
#if _HAS_CXX20
    using iterator_concept = contiguous_iterator_tag;
#endif // _HAS_CXX20
    using iterator_category = random_access_iterator_tag;
    using value_type        = T;
    using difference_type   = Diff;
    using pointer           = T*;
    using reference         = add_lvalue_reference_t<T>;

    counting_ptr(nullptr_t) noexcept : counting_ptr{static_cast<T*>(nullptr)} {}

    counting_ptr() noexcept : counting_ptr{nullptr} {}

    counting_ptr(const counting_ptr& other) noexcept : counting_ptr{other.p_} {}

    counting_ptr& operator=(const counting_ptr&) = default;

    counting_ptr& operator=(nullptr_t) noexcept {
        p_ = nullptr;
        return *this;
    }

    ~counting_ptr() {
        assert(counter != 0);
        --counter;
    }

    explicit operator bool() const noexcept {
        return p_ != nullptr;
    }

    static counting_ptr pointer_to(T& obj) noexcept {
        return counting_ptr{addressof(obj)};
    }

    T& operator*() const noexcept {
        return *p_;
    }

    T* operator->() const noexcept {
        return p_;
    }

    counting_ptr& operator++() noexcept {
        ++p_;
        return *this;
    }

    counting_ptr operator++(int) noexcept {
        auto tmp = *this;
        ++p_;
        return tmp;
    }

    counting_ptr& operator--() noexcept {
        --p_;
        return *this;
    }

    counting_ptr operator--(int) noexcept {
        auto tmp = *this;
        --p_;
        return tmp;
    }

    template <class I = Diff, enable_if_t<is_same_v<I, Diff>, int> = 0>
    T& operator[](I n) const noexcept {
        return p_[n];
    }

    template <class I = Diff, enable_if_t<is_same_v<I, Diff>, int> = 0>
    counting_ptr& operator+=(I n) noexcept {
        p_ += n;
        return *this;
    }

    template <class I = Diff, enable_if_t<is_same_v<I, Diff>, int> = 0>
    counting_ptr& operator-=(I n) noexcept {
        p_ -= n;
        return *this;
    }

    template <class I = Diff, enable_if_t<is_same_v<I, Diff>, int> = 0>
    friend counting_ptr operator+(const counting_ptr& p, I n) noexcept {
        auto tmp = p;
        tmp += n;
        return tmp;
    }

    template <class I = Diff, enable_if_t<is_same_v<I, Diff>, int> = 0>
    friend counting_ptr operator+(I n, const counting_ptr& p) noexcept {
        auto tmp = p;
        tmp += n;
        return tmp;
    }

    template <class I = Diff, enable_if_t<is_same_v<I, Diff>, int> = 0>
    friend counting_ptr operator-(const counting_ptr& p, I n) noexcept {
        auto tmp = p;
        tmp -= n;
        return tmp;
    }

    friend Diff operator-(const counting_ptr& lhs, const counting_ptr& rhs) noexcept {
        return static_cast<Diff>(lhs.p_ - rhs.p_);
    }

    friend bool operator==(const counting_ptr& p, nullptr_t) noexcept {
        return p.p_ == nullptr;
    }

#if _HAS_CXX20
    friend bool operator==(const counting_ptr& lhs, const counting_ptr& rhs) = default;

    friend auto operator<=>(const counting_ptr& lhs, const counting_ptr& rhs) = default;
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    friend bool operator==(nullptr_t, const counting_ptr& p) noexcept {
        return p.p_ == nullptr;
    }

    friend bool operator!=(const counting_ptr& p, nullptr_t) noexcept {
        return !(p == nullptr);
    }

    friend bool operator!=(nullptr_t, const counting_ptr& p) noexcept {
        return !(p == nullptr);
    }

    friend bool operator==(const counting_ptr& lhs, const counting_ptr& rhs) noexcept {
        return lhs.p_ == rhs.p_;
    }

    friend bool operator!=(const counting_ptr& lhs, const counting_ptr& rhs) noexcept {
        return !(lhs == rhs);
    }

    friend bool operator<(const counting_ptr& lhs, const counting_ptr& rhs) noexcept {
        return lhs.p_ < rhs.p_;
    }

    friend bool operator>(const counting_ptr& lhs, const counting_ptr& rhs) noexcept {
        return rhs < lhs;
    }

    friend bool operator<=(const counting_ptr& lhs, const counting_ptr& rhs) noexcept {
        return !(rhs < lhs);
    }

    friend bool operator>=(const counting_ptr& lhs, const counting_ptr& rhs) noexcept {
        return !(lhs < rhs);
    }
#endif // !_HAS_CXX20
};

template <class T>
struct ptr_counting_allocator {
    using value_type      = T;
    using pointer         = counting_ptr<T>;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;

    ptr_counting_allocator() = default;

    template <class U>
    constexpr ptr_counting_allocator(ptr_counting_allocator<U>) noexcept {}

    pointer allocate(size_type n) {
        return pointer{allocator<T>{}.allocate(n)};
    }

    void deallocate(pointer p, size_type n) {
        allocator<T>{}.deallocate(p.operator->(), n);
    }

    template <class U>
    friend constexpr bool operator==(ptr_counting_allocator, ptr_counting_allocator<U>) noexcept {
        return true;
    }
#if !_HAS_CXX20
    template <class U>
    friend constexpr bool operator!=(ptr_counting_allocator, ptr_counting_allocator<U>) noexcept {
        return false;
    }
#endif // !_HAS_CXX20
};

// GH-2769 <deque>: For allocators where allocator_traits<T>::pointer is an object, destructors aren't always called
void test_gh_2769() {
    {
        deque<int, ptr_counting_allocator<int>> dq{3, 1, 4, 1, 5, 9};
        dq.insert(dq.end(), {2, 6, 5, 3, 5, 8});
    }
    assert(counter == 0);

    {
        deque<int, ptr_counting_allocator<int>> dq(979, 323);
        dq.insert(dq.begin(), 84, 62);
        dq.erase(dq.begin() + 64, dq.begin() + 338);
    }
    assert(counter == 0);
}

size_t count_limit = 0;

struct live_counter {
    live_counter() {
        add();
    }
    live_counter(const live_counter&) {
        add();
    }
    live_counter& operator=(const live_counter&) = default;
    ~live_counter() {
        remove();
    }

    static void add() {
        assert(counter <= count_limit);
        if (counter == count_limit) {
            throw 42;
        }
        ++counter;
    }
    static void remove() {
        assert(counter <= count_limit);
        assert(counter > 0);
        --counter;
    }
};

void test_gh_3717() {
    // Also test GH-3717: deque(count) would leak elements if a constructor threw during resize(count)

    // make sure the counter/limit machinery works properly
    assert(counter == 0);
    count_limit = 8;
    {
        deque<live_counter> d{8};
        assert(counter == 8);
    }
    assert(counter == 0);

    // verify that deque(n) cleans up live objects on construction failure
    try {
        deque<live_counter>{16};
        assert(false);
    } catch (const int i) {
        assert(i == 42);
        assert(counter == 0);
    }
}

template <class T>
struct inconsistent_difference_allocator {
    using value_type      = T;
    using size_type       = size_t;
    using difference_type = conditional_t<is_arithmetic_v<T>, int64_t, int32_t>;
    using pointer         = counting_ptr<T, difference_type>;

    inconsistent_difference_allocator() = default;

    template <class U>
    constexpr inconsistent_difference_allocator(inconsistent_difference_allocator<U>) noexcept {}

    pointer allocate(size_type n) {
        return pointer{allocator<T>{}.allocate(n)};
    }

    void deallocate(pointer p, size_type n) {
        allocator<T>{}.deallocate(p.operator->(), n);
    }

    template <class U>
    friend constexpr bool operator==(inconsistent_difference_allocator, inconsistent_difference_allocator<U>) noexcept {
        return true;
    }
#if !_HAS_CXX20
    template <class U>
    friend constexpr bool operator!=(inconsistent_difference_allocator, inconsistent_difference_allocator<U>) noexcept {
        return false;
    }
#endif // !_HAS_CXX20
};

void test_inconsistent_difference_types() {
    {
        deque<int, inconsistent_difference_allocator<int>> dq{3, 1, 4, 1, 5, 9};
        dq.insert(dq.end(), {2, 6, 5, 3, 5, 8});
    }
    assert(counter == 0);

    {
        deque<int, inconsistent_difference_allocator<int>> dq(979, 323);
        dq.insert(dq.begin(), 84, 62);
        dq.erase(dq.begin() + 64, dq.begin() + 338);
    }
    assert(counter == 0);
}

// Also test GH-4954: Endless loop in deque::shrink_to_fit()
void test_gh_4954() {
    deque<int> qu;
    mt19937_64 mteng;

    for (int i = 0; i < 256; ++i) {
        const auto push_count = static_cast<size_t>((mteng() & 32767U) + 1);
        for (size_t j = 0; j < push_count; ++j) {
            qu.push_back(0);
        }

        auto pop_count = static_cast<size_t>((mteng() & 32767U) + 1);
        if (i % 100 == 0 || pop_count > qu.size()) {
            pop_count = qu.size();
        }
        for (size_t j = 0; j < pop_count; ++j) {
            qu.pop_front();
        }
        qu.shrink_to_fit();
    }
}

int main() {
    test_gh_2769();
    test_gh_3717();
    test_gh_4954();
    test_inconsistent_difference_types();
}
