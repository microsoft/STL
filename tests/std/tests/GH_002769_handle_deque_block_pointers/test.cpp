// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <deque>
#include <iterator>
#include <memory>
#include <type_traits>

using namespace std;

size_t fancy_counter = 0;

template <class T>
class counting_ptr {
private:
    T* p_;

    explicit counting_ptr(T* raw_ptr) noexcept : p_(raw_ptr) {
        ++fancy_counter;
    }

    template <class U>
    friend struct ptr_counting_allocator;

public:
#ifdef __cpp_lib_concepts
    using iterator_concept = contiguous_iterator_tag;
#endif // __cpp_lib_concepts
    using iterator_category = random_access_iterator_tag;
    using value_type        = T;
    using difference_type   = ptrdiff_t;
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
        assert(fancy_counter != 0);
        --fancy_counter;
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

    template <class I = ptrdiff_t, enable_if_t<is_integral_v<I>, int> = 0>
    T& operator[](I n) const noexcept {
        return p_[n];
    }

    template <class I = ptrdiff_t, enable_if_t<is_integral_v<I>, int> = 0>
    counting_ptr& operator+=(I n) noexcept {
        p_ += n;
        return *this;
    }

    template <class I = ptrdiff_t, enable_if_t<is_integral_v<I>, int> = 0>
    counting_ptr& operator-=(I n) noexcept {
        p_ -= n;
        return *this;
    }

    template <class I = ptrdiff_t, enable_if_t<is_integral_v<I>, int> = 0>
    friend counting_ptr operator+(const counting_ptr& p, I n) noexcept {
        auto tmp = p;
        tmp += n;
        return tmp;
    }

    template <class I = ptrdiff_t, enable_if_t<is_integral_v<I>, int> = 0>
    friend counting_ptr operator+(I n, const counting_ptr& p) noexcept {
        auto tmp = p;
        tmp += n;
        return tmp;
    }

    template <class I = ptrdiff_t, enable_if_t<is_integral_v<I>, int> = 0>
    friend counting_ptr operator-(const counting_ptr& p, I n) noexcept {
        auto tmp = p;
        tmp -= n;
        return tmp;
    }

    friend ptrdiff_t operator-(const counting_ptr& lhs, const counting_ptr& rhs) noexcept {
        return lhs.p_ - rhs.p_;
    }

    friend bool operator==(const counting_ptr& p, nullptr_t) noexcept {
        return p.p_ == nullptr;
    }

#if _HAS_CXX20
    friend bool operator==(const counting_ptr& lhs, const counting_ptr& rhs) = default;

    friend auto operator<=>(const counting_ptr& lhs, const counting_ptr& rhs) = default;
#else // ^^^ _HAS_CXX20 ^^^ / vvv !_HAS_CXX20 vvv
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

int main() {
    {
        deque<int, ptr_counting_allocator<int>> dq{3, 1, 4, 1, 5, 9};
        dq.insert(dq.end(), {2, 6, 5, 3, 5, 8});
    }
    assert(fancy_counter == 0);

    {
        deque<int, ptr_counting_allocator<int>> dq(979, 323);
        dq.insert(dq.begin(), 84, 62);
        dq.erase(dq.begin() + 64, dq.begin() + 338);
    }
    assert(fancy_counter == 0);
}
