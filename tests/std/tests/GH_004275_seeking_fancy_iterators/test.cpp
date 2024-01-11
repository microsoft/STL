// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// some portions of this file are derived from libc++'s test files:
// * support/min_allocator.h

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

using namespace std;

template <class T, class = enable_if<false>>
class min_pointer;
template <class T, class ID>
class min_pointer<const T, ID>;
template <class ID>
class min_pointer<void, ID>;
template <class ID>
class min_pointer<const void, ID>;
template <class T>
class min_allocator;

template <class ID>
class min_pointer<const void, ID> {
    const void* ptr_;

public:
    min_pointer() = default;
    min_pointer(nullptr_t) noexcept : ptr_(nullptr) {}
    template <class T>
    min_pointer(min_pointer<T, ID> p) noexcept : ptr_(p.ptr_) {}

    explicit operator bool() const {
        return ptr_ != nullptr;
    }

    friend bool operator==(min_pointer x, min_pointer y) {
        return x.ptr_ == y.ptr_;
    }
    friend bool operator!=(min_pointer x, min_pointer y) {
        return !(x == y);
    }
    template <class U, class XID>
    friend class min_pointer;
};

template <class ID>
class min_pointer<void, ID> {
    void* ptr_;

public:
    min_pointer() = default;
    constexpr min_pointer(nullptr_t) noexcept : ptr_(nullptr) {}
    template <class T, class = enable_if_t<!is_const_v<T>>>
    constexpr min_pointer(min_pointer<T, ID> p) noexcept : ptr_(p.ptr_) {}

    constexpr explicit operator bool() const {
        return ptr_ != nullptr;
    }

    friend constexpr bool operator==(min_pointer x, min_pointer y) {
        return x.ptr_ == y.ptr_;
    }
    friend constexpr bool operator!=(min_pointer x, min_pointer y) {
        return !(x == y);
    }
    template <class U, class XID>
    friend class min_pointer;
};

template <class T, class ID>
class min_pointer {
    T* ptr_;

    constexpr explicit min_pointer(T* p) noexcept : ptr_(p) {}

public:
    min_pointer() = default;
    constexpr min_pointer(nullptr_t) noexcept : ptr_(nullptr) {}
    constexpr explicit min_pointer(min_pointer<void, ID> p) noexcept : ptr_(static_cast<T*>(p.ptr_)) {}

    constexpr explicit operator bool() const {
        return ptr_ != nullptr;
    }

    using difference_type   = ptrdiff_t;
    using reference         = T&;
    using pointer           = T*;
    using value_type        = T;
    using iterator_category = random_access_iterator_tag;

    constexpr reference operator*() const {
        if (ptr_ == nullptr) {
            abort();
        }
        return *ptr_;
    }
    constexpr pointer operator->() const {
        return ptr_;
    }

    constexpr min_pointer& operator++() {
        ++ptr_;
        return *this;
    }
    constexpr min_pointer operator++(int) {
        min_pointer tmp(*this);
        ++ptr_;
        return tmp;
    }

    constexpr min_pointer& operator--() {
        --ptr_;
        return *this;
    }
    constexpr min_pointer operator--(int) {
        min_pointer tmp(*this);
        --ptr_;
        return tmp;
    }

    constexpr min_pointer& operator+=(difference_type n) {
        ptr_ += n;
        return *this;
    }
    constexpr min_pointer& operator-=(difference_type n) {
        ptr_ -= n;
        return *this;
    }

    constexpr min_pointer operator+(difference_type n) const {
        min_pointer tmp(*this);
        tmp += n;
        return tmp;
    }

    friend constexpr min_pointer operator+(difference_type n, min_pointer x) {
        return x + n;
    }

    constexpr min_pointer operator-(difference_type n) const {
        min_pointer tmp(*this);
        tmp -= n;
        return tmp;
    }

    friend constexpr difference_type operator-(min_pointer x, min_pointer y) {
        return x.ptr_ - y.ptr_;
    }

    constexpr reference operator[](difference_type n) const {
        return ptr_[n];
    }

    friend constexpr bool operator<(min_pointer x, min_pointer y) {
        return x.ptr_ < y.ptr_;
    }
    friend constexpr bool operator>(min_pointer x, min_pointer y) {
        return y < x;
    }
    friend constexpr bool operator<=(min_pointer x, min_pointer y) {
        return !(y < x);
    }
    friend constexpr bool operator>=(min_pointer x, min_pointer y) {
        return !(x < y);
    }

    static constexpr min_pointer pointer_to(T& t) {
        return min_pointer(addressof(t));
    }

    friend constexpr bool operator==(min_pointer x, min_pointer y) {
        return x.ptr_ == y.ptr_;
    }
    friend constexpr bool operator!=(min_pointer x, min_pointer y) {
        return !(x == y);
    }
    template <class U, class XID>
    friend class min_pointer;
    template <class U>
    friend class min_allocator;
};

template <class T, class ID>
class min_pointer<const T, ID> {
    const T* ptr_;

    constexpr explicit min_pointer(const T* p) : ptr_(p) {}

public:
    min_pointer() = default;
    constexpr min_pointer(nullptr_t) : ptr_(nullptr) {}
    constexpr min_pointer(min_pointer<T, ID> p) : ptr_(p.ptr_) {}
    constexpr explicit min_pointer(min_pointer<const void, ID> p) : ptr_(static_cast<const T*>(p.ptr_)) {}

    constexpr explicit operator bool() const {
        return ptr_ != nullptr;
    }

    using difference_type   = ptrdiff_t;
    using reference         = const T&;
    using pointer           = const T*;
    using value_type        = T;
    using iterator_category = random_access_iterator_tag;

    constexpr reference operator*() const {
        if (ptr_ == nullptr) {
            abort();
        }
        return *ptr_;
    }
    constexpr pointer operator->() const {
        return ptr_;
    }

    constexpr min_pointer& operator++() {
        ++ptr_;
        return *this;
    }
    constexpr min_pointer operator++(int) {
        min_pointer tmp(*this);
        ++ptr_;
        return tmp;
    }

    constexpr min_pointer& operator--() {
        --ptr_;
        return *this;
    }
    constexpr min_pointer operator--(int) {
        min_pointer tmp(*this);
        --ptr_;
        return tmp;
    }

    constexpr min_pointer& operator+=(difference_type n) {
        ptr_ += n;
        return *this;
    }
    constexpr min_pointer& operator-=(difference_type n) {
        ptr_ -= n;
        return *this;
    }

    constexpr min_pointer operator+(difference_type n) const {
        min_pointer tmp(*this);
        tmp += n;
        return tmp;
    }

    friend constexpr min_pointer operator+(difference_type n, min_pointer x) {
        return x + n;
    }

    constexpr min_pointer operator-(difference_type n) const {
        min_pointer tmp(*this);
        tmp -= n;
        return tmp;
    }

    friend constexpr difference_type operator-(min_pointer x, min_pointer y) {
        return x.ptr_ - y.ptr_;
    }

    constexpr reference operator[](difference_type n) const {
        return ptr_[n];
    }

    friend constexpr bool operator<(min_pointer x, min_pointer y) {
        return x.ptr_ < y.ptr_;
    }
    friend constexpr bool operator>(min_pointer x, min_pointer y) {
        return y < x;
    }
    friend constexpr bool operator<=(min_pointer x, min_pointer y) {
        return !(y < x);
    }
    friend constexpr bool operator>=(min_pointer x, min_pointer y) {
        return !(x < y);
    }

    static constexpr min_pointer pointer_to(const T& t) {
        return min_pointer(addressof(t));
    }

    friend constexpr bool operator==(min_pointer x, min_pointer y) {
        return x.ptr_ == y.ptr_;
    }
    friend constexpr bool operator!=(min_pointer x, min_pointer y) {
        return x.ptr_ != y.ptr_;
    }
    friend constexpr bool operator==(min_pointer x, nullptr_t) {
        return x.ptr_ == nullptr;
    }
    friend constexpr bool operator!=(min_pointer x, nullptr_t) {
        return x.ptr_ != nullptr;
    }
    friend constexpr bool operator==(nullptr_t, min_pointer x) {
        return x.ptr_ == nullptr;
    }
    friend constexpr bool operator!=(nullptr_t, min_pointer x) {
        return x.ptr_ != nullptr;
    }
    template <class U, class XID>
    friend class min_pointer;
};

#if _HAS_CXX20
#define CONSTEXPR20 constexpr
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
#define CONSTEXPR20 inline
#endif // ^^^ !_HAS_CXX20 ^^^

template <class T>
class min_allocator {
public:
    using value_type = T;
    using pointer    = min_pointer<T>;

    min_allocator() = default;
    template <class U>
    constexpr min_allocator(min_allocator<U>) noexcept {}

    CONSTEXPR20 pointer allocate(ptrdiff_t n) {
        return pointer(allocator<T>().allocate(n));
    }

    CONSTEXPR20 void deallocate(pointer p, ptrdiff_t n) {
        allocator<T>().deallocate(p.ptr_, n);
    }

    friend constexpr bool operator==(min_allocator, min_allocator) {
        return true;
    }
    friend constexpr bool operator!=(min_allocator x, min_allocator y) {
        return !(x == y);
    }
};

CONSTEXPR20 bool test_seeking_vector_iterators() {
    using VIt = vector<int, min_allocator<int>>::iterator;
    assert(find(VIt{}, VIt{}, 0) == VIt{});

    using VCIt = vector<int, min_allocator<int>>::const_iterator;
    assert(find(VCIt{}, VCIt{}, 0) == VCIt{});

    return true;
}

CONSTEXPR20 bool test_seeking_string_iterators() {
    using SIt = basic_string<char, char_traits<char>, min_allocator<char>>::iterator;
    assert(find(SIt{}, SIt{}, '\0') == SIt{});

    using SCIt = basic_string<char, char_traits<char>, min_allocator<char>>::const_iterator;
    assert(find(SCIt{}, SCIt{}, '\0') == SCIt{});

    return true;
}

CONSTEXPR20 bool test_vector_iterators_ordering() {
    using VIt  = vector<int, min_allocator<int>>::iterator;
    using VCIt = vector<int, min_allocator<int>>::const_iterator;

    assert(!(VIt{} < VIt{}));
    assert(!(VIt{} > VIt{}));
    assert(VIt{} <= VIt{});
    assert(VIt{} >= VIt{});

    assert(!(VCIt{} < VCIt{}));
    assert(!(VCIt{} > VCIt{}));
    assert(VCIt{} <= VCIt{});
    assert(VCIt{} >= VCIt{});

    assert(!(VIt{} < VCIt{}));
    assert(!(VIt{} > VCIt{}));
    assert(VIt{} <= VCIt{});
    assert(VIt{} >= VCIt{});

    assert(!(VCIt{} < VIt{}));
    assert(!(VCIt{} > VIt{}));
    assert(VCIt{} <= VIt{});
    assert(VCIt{} >= VIt{});

#if _HAS_CXX20
    assert(VIt{} <=> VIt{} == strong_ordering::equal);
    assert(VCIt{} <=> VCIt{} == strong_ordering::equal);
    assert(VIt{} <=> VCIt{} == strong_ordering::equal);
    assert(VCIt{} <=> VIt{} == strong_ordering::equal);
#endif // _HAS_CXX20

    return true;
}

CONSTEXPR20 bool test_string_iterators_ordering() {
    using SIt  = basic_string<char, char_traits<char>, min_allocator<char>>::iterator;
    using SCIt = basic_string<char, char_traits<char>, min_allocator<char>>::const_iterator;

    assert(!(SIt{} < SIt{}));
    assert(!(SIt{} > SIt{}));
    assert(SIt{} <= SIt{});
    assert(SIt{} >= SIt{});

    assert(!(SCIt{} < SCIt{}));
    assert(!(SCIt{} > SCIt{}));
    assert(SCIt{} <= SCIt{});
    assert(SCIt{} >= SCIt{});

    assert(!(SIt{} < SCIt{}));
    assert(!(SIt{} > SCIt{}));
    assert(SIt{} <= SCIt{});
    assert(SIt{} >= SCIt{});

    assert(!(SCIt{} < SIt{}));
    assert(!(SCIt{} > SIt{}));
    assert(SCIt{} <= SIt{});
    assert(SCIt{} >= SIt{});

#if _HAS_CXX20
    assert(SIt{} <=> SIt{} == strong_ordering::equal);
    assert(SCIt{} <=> SCIt{} == strong_ordering::equal);
    assert(SIt{} <=> SCIt{} == strong_ordering::equal);
    assert(SCIt{} <=> SIt{} == strong_ordering::equal);
#endif // _HAS_CXX20

    return true;
}

#if _HAS_CXX20
static_assert(test_seeking_vector_iterators());
static_assert(test_seeking_string_iterators());
static_assert(test_vector_iterators_ordering());
static_assert(test_string_iterators_ordering());
#endif // _HAS_CXX20

int main() {
    test_seeking_vector_iterators();
    test_seeking_string_iterators();
    test_vector_iterators_ordering();
    test_string_iterators_ordering();
}
