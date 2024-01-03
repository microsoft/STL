// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <stdexcept>
#include <type_traits>
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

struct Meow {
#if _HAS_CXX20
    friend constexpr auto operator<=>(Meow, Meow) = default;
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    friend constexpr bool operator==(Meow, Meow) noexcept {
        return true;
    }

    friend constexpr bool operator!=(Meow, Meow) noexcept {
        return false;
    }

    friend constexpr bool operator<(Meow, Meow) noexcept {
        return false;
    }

    friend constexpr bool operator>(Meow, Meow) noexcept {
        return false;
    }

    friend constexpr bool operator<=(Meow, Meow) noexcept {
        return true;
    }

    friend constexpr bool operator>=(Meow, Meow) noexcept {
        return true;
    }
#endif // ^^^ !_HAS_CXX20 ^^^
};

template <size_t N>
struct std::array<Meow, N> {
    using value_type             = Meow;
    using pointer                = Meow*;
    using const_pointer          = const Meow*;
    using reference              = Meow&;
    using const_reference        = const Meow&;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using iterator               = Meow*;
    using const_iterator         = const Meow*;
    using reverse_iterator       = std::reverse_iterator<Meow*>;
    using const_reverse_iterator = std::reverse_iterator<const Meow*>;

    CONSTEXPR20 void fill(const Meow&) {} // Meow is no-op assignable.
    CONSTEXPR20 void swap(array&) noexcept {} // Meow is no-op swappable.

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
        return n < N ? elems_[n] : throw out_of_range{"bad array<Meow> access"};
    }
    constexpr const_reference at(size_type n) const {
        return n < N ? elems_[n] : throw out_of_range{"bad array<Meow> access"};
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

    Meow elems_[N];
};

template <>
struct std::array<Meow, 0> {
    using value_type             = Meow;
    using pointer                = Meow*;
    using const_pointer          = const Meow*;
    using reference              = Meow&;
    using const_reference        = const Meow&;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using iterator               = Meow*;
    using const_iterator         = const Meow*;
    using reverse_iterator       = std::reverse_iterator<Meow*>;
    using const_reverse_iterator = std::reverse_iterator<const Meow*>;

    CONSTEXPR20 void fill(const Meow&) {} // Meow is no-op assignable.
    CONSTEXPR20 void swap(array&) noexcept {} // Meow is no-op swappable.

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
        throw out_of_range{"bad array<Meow, 0> access"};
    }
    const_reference at(size_type) const {
        throw out_of_range{"bad array<Meow, 0> access"};
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
    array<Meow, 1> a{};
    const auto& c = a;

    STATIC_ASSERT(is_same_v<decltype(get<0>(a)), Meow&>);
    STATIC_ASSERT(is_same_v<decltype(get<0>(c)), const Meow&>);
    STATIC_ASSERT(is_same_v<decltype(get<0>(move(a))), Meow&&>);
    STATIC_ASSERT(is_same_v<decltype(get<0>(move(c))), const Meow&&>);

    assert(get<0>(a) == Meow{});
    assert(get<0>(c) == Meow{});
    assert(get<0>(move(a)) == Meow{});
    assert(get<0>(move(c)) == Meow{});

    return true;
}

CONSTEXPR20 bool test_array_comparison() {
    using A0 = array<Meow, 0>;
    assert(A0{} == A0{});
    assert(!(A0{} != A0{}));
    assert(!(A0{} < A0{}));
    assert(!(A0{} > A0{}));
    assert(A0{} <= A0{});
    assert(A0{} >= A0{});
#if _HAS_CXX20
    assert(A0{} <=> A0{} == strong_ordering::equal);
#endif // _HAS_CXX20

    using A1 = array<Meow, 1>;
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
