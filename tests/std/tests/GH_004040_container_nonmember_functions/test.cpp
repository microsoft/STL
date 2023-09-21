// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <stdexcept>
#include <utility>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

#if _HAS_CXX20
#define CONSTEXPR20 constexpr
#define CONSTEXPR17 constexpr
#define NODISCARD20 [[nodiscard]]
#elif _HAS_CXX17 // ^^^ _HAS_CXX20 / !_HAS_CXX20 && _HAS_CXX17 vvv
#define CONSTEXPR20 inline
#define CONSTEXPR17 constexpr
#define NODISCARD20
#else // ^^^ !_HAS_CXX20 && _HAS_CXX17 / !_HAS_CXX17 vvv
#define CONSTEXPR20 inline
#define CONSTEXPR17 inline
#define NODISCARD20
#endif // ^^^ !_HAS_CXX17 ^^^

using namespace std;

struct Foo {
#if _HAS_CXX20
    friend constexpr auto operator<=>(Foo, Foo) = default;
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    friend constexpr bool operator==(Foo, Foo) noexcept {
        return true;
    }

    friend constexpr bool operator!=(Foo, Foo) noexcept {
        return false;
    }

    friend constexpr bool operator<(Foo, Foo) noexcept {
        return false;
    }

    friend constexpr bool operator>(Foo, Foo) noexcept {
        return false;
    }

    friend constexpr bool operator<=(Foo, Foo) noexcept {
        return true;
    }

    friend constexpr bool operator>=(Foo, Foo) noexcept {
        return true;
    }
#endif // ^^^ !_HAS_CXX20 ^^^
};

template <size_t N>
struct std::array<Foo, N> {
    using value_type             = Foo;
    using pointer                = Foo*;
    using const_pointer          = const Foo*;
    using reference              = Foo&;
    using const_reference        = const Foo&;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using iterator               = Foo*;
    using const_iterator         = const Foo*;
    using reverse_iterator       = std::reverse_iterator<Foo*>;
    using const_reverse_iterator = std::reverse_iterator<const Foo*>;

    CONSTEXPR20 void fill(const Foo&) {} // Foo is no-op assignable.
    CONSTEXPR20 void swap(array&) noexcept {} // Foo is no-op swappable.

    CONSTEXPR17 iterator begin() noexcept {
        return elems_;
    }
    CONSTEXPR17 const_iterator begin() const noexcept {
        return elems_;
    }
    CONSTEXPR17 iterator end() noexcept {
        return elems_ + N;
    }
    CONSTEXPR17 const_iterator end() const noexcept {
        return elems_ + N;
    }

    CONSTEXPR17 reverse_iterator rbegin() noexcept {
        return reverse_iterator{elems_ + N};
    }
    CONSTEXPR17 const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator{elems_ + N};
    }
    CONSTEXPR17 reverse_iterator rend() noexcept {
        return reverse_iterator{elems_};
    }
    CONSTEXPR17 const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator{elems_};
    }

    CONSTEXPR17 const_iterator cbegin() const noexcept {
        return elems_;
    }
    CONSTEXPR17 const_iterator cend() const noexcept {
        return elems_ + N;
    }
    CONSTEXPR17 const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator{elems_ + N};
    }
    CONSTEXPR17 const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator{elems_};
    }

    NODISCARD20 constexpr bool empty() const noexcept {
        return false;
    }
    constexpr size_type size() const noexcept {
        return N;
    }
    constexpr size_type max_size() const noexcept {
        return N;
    }

    CONSTEXPR17 reference operator[](size_type n) {
        return elems_[n];
    }
    constexpr const_reference operator[](size_type n) const {
        return elems_[n];
    }
    CONSTEXPR17 reference at(size_type n) {
        return n < N ? elems_[n] : throw out_of_range{"bad array<Foo> access"};
    }
    constexpr const_reference at(size_type n) const {
        return n < N ? elems_[n] : throw out_of_range{"bad array<Foo> access"};
    }
    CONSTEXPR17 reference front() {
        return elems_[0];
    }
    constexpr const_reference front() const {
        return elems_[0];
    }
    CONSTEXPR17 reference back() {
        return elems_[N - 1];
    }
    constexpr const_reference back() const {
        return elems_[N - 1];
    }

    CONSTEXPR17 pointer data() noexcept {
        return elems_;
    }
    CONSTEXPR17 const_pointer data() const noexcept {
        return elems_;
    }

    Foo elems_[N];
};

template <>
struct std::array<Foo, 0> {
    using value_type             = Foo;
    using pointer                = Foo*;
    using const_pointer          = const Foo*;
    using reference              = Foo&;
    using const_reference        = const Foo&;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using iterator               = Foo*;
    using const_iterator         = const Foo*;
    using reverse_iterator       = std::reverse_iterator<Foo*>;
    using const_reverse_iterator = std::reverse_iterator<const Foo*>;

    CONSTEXPR20 void fill(const Foo&) {} // Foo is no-op assignable.
    CONSTEXPR20 void swap(array&) noexcept {} // Foo is no-op swappable.

    CONSTEXPR17 iterator begin() noexcept {
        return nullptr;
    }
    CONSTEXPR17 const_iterator begin() const noexcept {
        return nullptr;
    }
    CONSTEXPR17 iterator end() noexcept {
        return nullptr;
    }
    CONSTEXPR17 const_iterator end() const noexcept {
        return nullptr;
    }

    CONSTEXPR17 reverse_iterator rbegin() noexcept {
        return reverse_iterator{end()};
    }
    CONSTEXPR17 const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator{end()};
    }
    CONSTEXPR17 reverse_iterator rend() noexcept {
        return reverse_iterator{begin()};
    }
    CONSTEXPR17 const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator{begin()};
    }

    CONSTEXPR17 const_iterator cbegin() const noexcept {
        return nullptr;
    }
    CONSTEXPR17 const_iterator cend() const noexcept {
        return nullptr;
    }
    CONSTEXPR17 const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator{end()};
    }
    CONSTEXPR17 const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator{begin()};
    }

    NODISCARD20 constexpr bool empty() const noexcept {
        return true;
    }
    constexpr size_type size() const noexcept {
        return 0;
    }
    constexpr size_type max_size() const noexcept {
        return 0;
    }

    // Perhaps these functions of array<T, 0> shouldn't be constexpr as an invocation is always throwing or UB.
    reference operator[](size_type) {
        abort(); // UB
    }
    const_reference operator[](size_type) const {
        abort(); // UB
    }
    reference at(size_type) {
        throw out_of_range{"bad array<Foo, 0> access"};
    }
    const_reference at(size_type) const {
        throw out_of_range{"bad array<Foo, 0> access"};
    }
    reference front() {
        abort(); // UB
    }
    const_reference front() const {
        abort(); // UB
    }
    reference back() {
        abort(); // UB
    }
    const_reference back() const {
        abort(); // UB
    }

    CONSTEXPR17 pointer data() noexcept {
        return nullptr;
    }
    CONSTEXPR17 const_pointer data() const noexcept {
        return nullptr;
    }

    unsigned char dummy_[1];
};

constexpr bool test_array_get() {
    array<Foo, 1> a{};
    const auto& cref = a;

    STATIC_ASSERT(is_same_v<decltype(get<0>(a)), Foo&>);
    STATIC_ASSERT(is_same_v<decltype(get<0>(cref)), const Foo&>);
    STATIC_ASSERT(is_same_v<decltype(get<0>(move(a))), Foo&&>);
    STATIC_ASSERT(is_same_v<decltype(get<0>(move(cref))), const Foo&&>);

    assert(get<0>(a) == Foo{});
    assert(get<0>(cref) == Foo{});
    assert(get<0>(move(a)) == Foo{});
    assert(get<0>(move(cref)) == Foo{});

    return true;
}

CONSTEXPR20 bool test_array_comparison() {
    using A0 = array<Foo, 0>;
    assert(A0{} == A0{});
    assert(!(A0{} != A0{}));
    assert(!(A0{} < A0{}));
    assert(!(A0{} > A0{}));
    assert(A0{} <= A0{});
    assert(A0{} >= A0{});
#if _HAS_CXX20
    assert(A0{} <=> A0{} == strong_ordering::equal);
#endif // _HAS_CXX20

    using A1 = array<Foo, 1>;
    assert(A1{} == A1{});
    assert(!(A1{} != A1{}));
    assert(!(A1{} < A1{}));
    assert(!(A1{} > A1{}));
    assert(A1{} <= A1{});
    assert(A1{} >= A1{});
#if _HAS_CXX20
    assert(A1{} <=> A1{} == strong_ordering::equal);
#endif // _HAS_CXX20

    return true;
}

int main() {
    test_array_get();
    STATIC_ASSERT(test_array_get());
    test_array_comparison();
#if _HAS_CXX20
    static_assert(test_array_comparison());
#endif // _HAS_CXX20
}
