// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>

using namespace std;

static_assert(is_same_v<decltype(dynamic_extent), const size_t>);
static_assert(dynamic_extent == static_cast<size_t>(-1));

static_assert(is_same_v<decltype(span<int>::extent), const size_t>);
static_assert(span<int>::extent == dynamic_extent);

static_assert(is_same_v<decltype(span<int, 3>::extent), const size_t>);
static_assert(span<int, 3>::extent == 3);

static_assert(is_same_v<span<int>::element_type, int>);
static_assert(is_same_v<span<int>::value_type, int>);
static_assert(is_same_v<span<int>::size_type, size_t>);
static_assert(is_same_v<span<int>::difference_type, ptrdiff_t>);
static_assert(is_same_v<span<int>::pointer, int*>);
static_assert(is_same_v<span<int>::const_pointer, const int*>);
static_assert(is_same_v<span<int>::reference, int&>);
static_assert(is_same_v<span<int>::const_reference, const int&>);

static_assert(is_same_v<span<int, 3>::element_type, int>);
static_assert(is_same_v<span<int, 3>::value_type, int>);
static_assert(is_same_v<span<int, 3>::size_type, size_t>);
static_assert(is_same_v<span<int, 3>::difference_type, ptrdiff_t>);
static_assert(is_same_v<span<int, 3>::pointer, int*>);
static_assert(is_same_v<span<int, 3>::const_pointer, const int*>);
static_assert(is_same_v<span<int, 3>::reference, int&>);
static_assert(is_same_v<span<int, 3>::const_reference, const int&>);

static_assert(is_same_v<span<const int>::element_type, const int>);
static_assert(is_same_v<span<const int>::value_type, int>);
static_assert(is_same_v<span<const int>::size_type, size_t>);
static_assert(is_same_v<span<const int>::difference_type, ptrdiff_t>);
static_assert(is_same_v<span<const int>::pointer, const int*>);
static_assert(is_same_v<span<const int>::const_pointer, const int*>);
static_assert(is_same_v<span<const int>::reference, const int&>);
static_assert(is_same_v<span<const int>::const_reference, const int&>);

static_assert(is_same_v<span<const int, 3>::element_type, const int>);
static_assert(is_same_v<span<const int, 3>::value_type, int>);
static_assert(is_same_v<span<const int, 3>::size_type, size_t>);
static_assert(is_same_v<span<const int, 3>::difference_type, ptrdiff_t>);
static_assert(is_same_v<span<const int, 3>::pointer, const int*>);
static_assert(is_same_v<span<const int, 3>::const_pointer, const int*>);
static_assert(is_same_v<span<const int, 3>::reference, const int&>);
static_assert(is_same_v<span<const int, 3>::const_reference, const int&>);

// The iterators are thoroughly tested by P0896R4_ranges_range_machinery
static_assert(is_same_v<iterator_traits<span<int>::iterator>::pointer, int*>);
static_assert(is_same_v<span<int>::reverse_iterator, reverse_iterator<span<int>::iterator>>);

static_assert(is_same_v<iterator_traits<span<int, 3>::iterator>::pointer, int*>);
static_assert(is_same_v<span<int, 3>::reverse_iterator, reverse_iterator<span<int, 3>::iterator>>);

static_assert(is_same_v<iterator_traits<span<const int>::iterator>::pointer, const int*>);
static_assert(is_same_v<span<const int>::reverse_iterator, reverse_iterator<span<const int>::iterator>>);

static_assert(is_same_v<iterator_traits<span<const int, 3>::iterator>::pointer, const int*>);
static_assert(is_same_v<span<const int, 3>::reverse_iterator, reverse_iterator<span<const int, 3>::iterator>>);

static_assert(ranges::enable_borrowed_range<span<int>>);
static_assert(ranges::enable_borrowed_range<span<int, 3>>);

// N4901 [span.overview]/3
static_assert(is_trivially_copyable_v<span<int>>);
static_assert(is_trivially_copyable_v<span<int, 3>>);
static_assert(is_trivially_copyable_v<span<const int>>);
static_assert(is_trivially_copyable_v<span<const int, 3>>);

// Our implementation provides additional guarantees beyond the Standard
// that span and its iterators are standard-layout, and that the iterators are trivially copyable.
static_assert(is_standard_layout_v<span<int>>);
static_assert(is_standard_layout_v<span<int>::iterator>);
static_assert(is_standard_layout_v<span<int, 3>>);
static_assert(is_standard_layout_v<span<int, 3>::iterator>);
static_assert(is_standard_layout_v<span<const int>>);
static_assert(is_standard_layout_v<span<const int>::iterator>);
static_assert(is_standard_layout_v<span<const int, 3>>);
static_assert(is_standard_layout_v<span<const int, 3>::iterator>);
static_assert(is_trivially_copyable_v<span<int>::iterator>);
static_assert(is_trivially_copyable_v<span<int, 3>::iterator>);
static_assert(is_trivially_copyable_v<span<const int>::iterator>);
static_assert(is_trivially_copyable_v<span<const int, 3>::iterator>);

// For performance, our implementation provides an additional guarantee beyond the Standard
// that fixed-size span has only the size of a pointer.
static_assert(sizeof(span<int, 4>) == sizeof(void*));
static_assert(sizeof(span<int>) == sizeof(void*) + sizeof(size_t));

struct Base {};
struct Derived : Base {};
static_assert(is_convertible_v<Derived*, Base*>);
static_assert(!is_convertible_v<Derived (*)[], Base (*)[]>);

struct NonRange {};

template <typename T, bool Borrowed = false>
struct BasicRange {
    T elements[3]{};

    constexpr size_t size() const {
        return 3;
    }

    constexpr T* data() {
        return elements;
    }

    constexpr const T* data() const {
        return elements;
    }

    constexpr T* begin() {
        return elements;
    }

    constexpr const T* begin() const {
        return elements;
    }

    constexpr T* end() {
        return elements + 3;
    }

    constexpr const T* end() const {
        return elements + 3;
    }
};

namespace std::ranges {
    template <typename T, bool Borrowed>
    constexpr bool enable_borrowed_range<BasicRange<T, Borrowed>> = Borrowed;
}

using ContiguousSizedRange = BasicRange<int>;

// Not truly a model of borrowed_range; this is a convenient fiction for testing purposes.
using BorrowedContiguousSizedRange = BasicRange<int, true>;

template <typename T, size_t Extent = dynamic_extent>
constexpr void FunctionTakingSpan(type_identity_t<span<T, Extent>>) {}

template <typename U, typename = void>
constexpr bool AsWritableBytesCompilesFor = false;

template <typename U>
constexpr bool AsWritableBytesCompilesFor<U, void_t<decltype(as_writable_bytes(declval<U>()))>> = true;

constexpr bool test() {
    {
        static_assert(is_nothrow_default_constructible_v<span<int>>);
        static_assert(is_nothrow_default_constructible_v<span<int, 0>>);
        static_assert(!is_default_constructible_v<span<int, 3>>);

        span<int> sp_dyn;
        assert(sp_dyn.data() == nullptr);
        assert(sp_dyn.size() == 0);
        assert(sp_dyn.empty());

        span<int, 0> sp_zero;
        assert(sp_zero.data() == nullptr);
        assert(sp_zero.size() == 0);
        assert(sp_zero.empty());

        FunctionTakingSpan<int>({});
        FunctionTakingSpan<int, 0>({});
    }

    int arr[3]{10, 20, 30};
    array<int, 3> stl{{100, 200, 300}};
    array<int*, 3> stl_nullptr{{nullptr, nullptr, nullptr}};

    {
        static_assert(is_nothrow_constructible_v<span<int>, int*, size_t>); // strengthened
        static_assert(is_nothrow_constructible_v<span<int>, int*, uint16_t>); // strengthened
        static_assert(!is_constructible_v<span<int>, const int*, size_t>);
        static_assert(!is_constructible_v<span<int>, double*, size_t>);
        static_assert(!is_constructible_v<span<int>, size_t, size_t>);

        static_assert(is_nothrow_constructible_v<span<int, 3>, int*, size_t>); // strengthened
        static_assert(is_nothrow_constructible_v<span<int, 3>, int*, uint16_t>); // strengthened
        static_assert(!is_constructible_v<span<int, 3>, const int*, size_t>);
        static_assert(!is_constructible_v<span<int, 3>, double*, size_t>);
        static_assert(!is_constructible_v<span<int, 3>, size_t, size_t>);

        static_assert(is_nothrow_constructible_v<span<const int>, int*, size_t>); // strengthened
        static_assert(is_nothrow_constructible_v<span<const int>, const int*, size_t>); // strengthened

        span<int> sp_dyn_a(arr, 3);
        span<int> sp_dyn_b(begin(arr), 3);
        assert(sp_dyn_a.data() == begin(arr));
        assert(sp_dyn_b.data() == begin(arr));
        assert(sp_dyn_a.size() == 3);
        assert(sp_dyn_b.size() == 3);

        span<int, 3> sp_three_a(arr, 3);
        span<int, 3> sp_three_b(begin(arr), 3);
        assert(sp_three_a.data() == begin(arr));
        assert(sp_three_b.data() == begin(arr));
        assert(sp_three_a.size() == 3);
        assert(sp_three_b.size() == 3);

        span<const int> sp_const_a(arr, 3);
        span<const int> sp_const_b(begin(arr), 3);
        assert(sp_const_a.data() == begin(arr));
        assert(sp_const_b.data() == begin(arr));
        assert(sp_const_a.size() == 3);
        assert(sp_const_b.size() == 3);

        span<const int> sp_const_c(as_const(arr), 3);
        span<const int> sp_const_d(cbegin(arr), 3);
        assert(sp_const_c.data() == begin(arr));
        assert(sp_const_d.data() == begin(arr));
        assert(sp_const_c.size() == 3);
        assert(sp_const_d.size() == 3);

        FunctionTakingSpan<int>({arr, 3});
        FunctionTakingSpan<int>({begin(arr), 3});
        FunctionTakingSpan<const int>({arr, 3});
        FunctionTakingSpan<const int>({begin(arr), 3});
        FunctionTakingSpan<const int>({as_const(arr), 3});
        FunctionTakingSpan<const int>({cbegin(arr), 3});

        static_assert(is_same_v<decltype(span{arr, 3}), span<int>>);
        static_assert(is_same_v<decltype(span{begin(arr), 3}), span<int>>);

        static_assert(is_same_v<decltype(span{as_const(arr), 3}), span<const int>>);
        static_assert(is_same_v<decltype(span{cbegin(arr), 3}), span<const int>>);

        static_assert(is_nothrow_constructible_v<span<int>, array<int, 3>::iterator, size_t>); // strengthened
        static_assert(!is_constructible_v<span<int>, array<int, 3>::const_iterator, size_t>);
        static_assert(!is_constructible_v<span<int>, array<double, 3>::iterator, size_t>);

        static_assert(is_nothrow_constructible_v<span<int, 3>, array<int, 3>::iterator, size_t>); // strengthened
        static_assert(!is_constructible_v<span<int, 3>, array<int, 3>::const_iterator, size_t>);
        static_assert(!is_constructible_v<span<int, 3>, array<double, 3>::iterator, size_t>);

        static_assert(is_nothrow_constructible_v<span<const int>, array<int, 3>::iterator, size_t>); // strengthened
        static_assert(
            is_nothrow_constructible_v<span<const int>, array<int, 3>::const_iterator, size_t>); // strengthened

        static_assert(is_nothrow_constructible_v<span<Base>, Base*, size_t>); // strengthened
        static_assert(is_nothrow_constructible_v<span<Base, 3>, Base*, size_t>); // strengthened
        static_assert(!is_constructible_v<span<Base>, Derived*, size_t>);
        static_assert(!is_constructible_v<span<Base, 3>, Derived*, size_t>);

        static_assert(is_nothrow_constructible_v<span<Base>, array<Base, 3>::iterator, size_t>); // strengthened
        static_assert(is_nothrow_constructible_v<span<Base, 3>, array<Base, 3>::iterator, size_t>); // strengthened
        static_assert(!is_constructible_v<span<Base>, array<Derived, 3>::iterator, size_t>);
        static_assert(!is_constructible_v<span<Base, 3>, array<Derived, 3>::iterator, size_t>);

        span<int> sp_dyn_x(stl.begin(), 3);
        assert(sp_dyn_x.data() == stl.data());
        assert(sp_dyn_x.size() == 3);

        span<int, 3> sp_three_x(stl.begin(), 3);
        assert(sp_three_x.data() == stl.data());
        assert(sp_three_x.size() == 3);

        span<const int> sp_const_x(stl.begin(), 3);
        assert(sp_const_x.data() == stl.data());
        assert(sp_const_x.size() == 3);

        span<const int> sp_const_y(stl.cbegin(), 3);
        assert(sp_const_y.data() == stl.data());
        assert(sp_const_y.size() == 3);

        FunctionTakingSpan<int>({stl.begin(), 3});
        FunctionTakingSpan<const int>({stl.begin(), 3});
        FunctionTakingSpan<const int>({stl.cbegin(), 3});

        static_assert(is_same_v<decltype(span{stl.begin(), 3}), span<int>>);
        static_assert(is_same_v<decltype(span{stl.cbegin(), 3}), span<const int>>);
    }

    {
        static_assert(is_nothrow_constructible_v<span<int>, int*, int*>); // strengthened
        static_assert(!is_constructible_v<span<int>, int*, double*>);
        static_assert(!is_constructible_v<span<int>, const int*, int*>);
        static_assert(!is_constructible_v<span<int>, const int*, const int*>);
        static_assert(!is_constructible_v<span<int>, const int*, double*>);
        static_assert(!is_constructible_v<span<int>, double*, int*>);
        static_assert(!is_constructible_v<span<int>, double*, const int*>);
        static_assert(!is_constructible_v<span<int>, double*, double*>);
        static_assert(!is_constructible_v<span<int>, size_t, int*>);

        static_assert(is_nothrow_constructible_v<span<int, 3>, int*, int*>); // strengthened
        static_assert(!is_constructible_v<span<int, 3>, int*, double*>);
        static_assert(!is_constructible_v<span<int, 3>, const int*, int*>);
        static_assert(!is_constructible_v<span<int, 3>, const int*, const int*>);
        static_assert(!is_constructible_v<span<int, 3>, const int*, double*>);
        static_assert(!is_constructible_v<span<int, 3>, double*, int*>);
        static_assert(!is_constructible_v<span<int, 3>, double*, const int*>);
        static_assert(!is_constructible_v<span<int, 3>, double*, double*>);
        static_assert(!is_constructible_v<span<int, 3>, size_t, int*>);

        static_assert(is_nothrow_constructible_v<span<const int>, int*, int*>); // strengthened
        static_assert(is_nothrow_constructible_v<span<const int>, int*, const int*>); // strengthened
        static_assert(is_nothrow_constructible_v<span<const int>, const int*, int*>); // strengthened
        static_assert(is_nothrow_constructible_v<span<const int>, const int*, const int*>); // strengthened

        span<int> sp_dyn_a(begin(arr), end(arr));
        assert(sp_dyn_a.data() == begin(arr));
        assert(sp_dyn_a.size() == 3);

        span<int, 3> sp_three_a(begin(arr), end(arr));
        assert(sp_three_a.data() == begin(arr));
        assert(sp_three_a.size() == 3);

        span<const int> sp_const_a(begin(arr), end(arr));
        span<const int> sp_const_b(begin(arr), cend(arr));
        span<const int> sp_const_c(cbegin(arr), end(arr));
        span<const int> sp_const_d(cbegin(arr), cend(arr));
        assert(sp_const_a.data() == begin(arr));
        assert(sp_const_b.data() == begin(arr));
        assert(sp_const_c.data() == begin(arr));
        assert(sp_const_d.data() == begin(arr));
        assert(sp_const_a.size() == 3);
        assert(sp_const_b.size() == 3);
        assert(sp_const_c.size() == 3);
        assert(sp_const_d.size() == 3);

        FunctionTakingSpan<int>({begin(arr), end(arr)});
        FunctionTakingSpan<const int>({begin(arr), end(arr)});
        FunctionTakingSpan<const int>({begin(arr), cend(arr)});
        FunctionTakingSpan<const int>({cbegin(arr), end(arr)});
        FunctionTakingSpan<const int>({cbegin(arr), cend(arr)});

        static_assert(is_same_v<decltype(span{begin(arr), end(arr)}), span<int>>);
        static_assert(is_same_v<decltype(span{cbegin(arr), cend(arr)}), span<const int>>);

        static_assert(is_nothrow_constructible_v<span<int>, int*, const int*>); // strengthened

        static_assert(is_nothrow_constructible_v<span<int, 3>, int*, const int*>); // strengthened

        // The types int* and unreachable_sentinel_t are a contiguous_iterator and sentinel
        // that model sentinel_for but not sized_sentinel_for.
        static_assert(!is_constructible_v<span<int>, int*, unreachable_sentinel_t>);
        static_assert(!is_constructible_v<span<int, 3>, int*, unreachable_sentinel_t>);

        span<int> sp_dyn_b(begin(arr), cend(arr));
        assert(sp_dyn_b.data() == begin(arr));
        assert(sp_dyn_b.size() == 3);

        span<int, 3> sp_three_b(begin(arr), cend(arr));
        assert(sp_three_b.data() == begin(arr));
        assert(sp_three_b.size() == 3);

        FunctionTakingSpan<int>({begin(arr), cend(arr)});

        static_assert(is_same_v<decltype(span{begin(arr), cend(arr)}), span<int>>);
        static_assert(is_same_v<decltype(span{cbegin(arr), end(arr)}), span<const int>>);

        using I = array<int, 3>::iterator;
        using C = array<int, 3>::const_iterator;
        using Z = array<double, 3>::iterator;

        static_assert(is_nothrow_constructible_v<span<int>, I, I>); // strengthened
        static_assert(is_nothrow_constructible_v<span<int>, I, C>); // strengthened
        static_assert(!is_constructible_v<span<int>, I, Z>);
        static_assert(!is_constructible_v<span<int>, C, I>);
        static_assert(!is_constructible_v<span<int>, C, C>);
        static_assert(!is_constructible_v<span<int>, C, Z>);
        static_assert(!is_constructible_v<span<int>, Z, I>);
        static_assert(!is_constructible_v<span<int>, Z, C>);
        static_assert(!is_constructible_v<span<int>, Z, Z>);

        static_assert(is_nothrow_constructible_v<span<int, 3>, I, I>); // strengthened
        static_assert(is_nothrow_constructible_v<span<int, 3>, I, C>); // strengthened
        static_assert(!is_constructible_v<span<int, 3>, I, Z>);
        static_assert(!is_constructible_v<span<int, 3>, C, I>);
        static_assert(!is_constructible_v<span<int, 3>, C, C>);
        static_assert(!is_constructible_v<span<int, 3>, C, Z>);
        static_assert(!is_constructible_v<span<int, 3>, Z, I>);
        static_assert(!is_constructible_v<span<int, 3>, Z, C>);
        static_assert(!is_constructible_v<span<int, 3>, Z, Z>);

        static_assert(is_nothrow_constructible_v<span<const int>, I, I>); // strengthened
        static_assert(is_nothrow_constructible_v<span<const int>, I, C>); // strengthened
        static_assert(is_nothrow_constructible_v<span<const int>, C, I>); // strengthened
        static_assert(is_nothrow_constructible_v<span<const int>, C, C>); // strengthened

        static_assert(!is_constructible_v<span<int>, I, int*>);
        static_assert(!is_constructible_v<span<int>, int*, I>);

        static_assert(is_nothrow_constructible_v<span<Base>, Base*, Base*>); // strengthened
        static_assert(is_nothrow_constructible_v<span<Base, 3>, Base*, Base*>); // strengthened
        static_assert(!is_constructible_v<span<Base>, Derived*, Derived*>);
        static_assert(!is_constructible_v<span<Base, 3>, Derived*, Derived*>);

        using B = array<Base, 3>::iterator;
        using D = array<Derived, 3>::iterator;
        static_assert(is_nothrow_constructible_v<span<Base>, B, B>); // strengthened
        static_assert(is_nothrow_constructible_v<span<Base, 3>, B, B>); // strengthened
        static_assert(!is_constructible_v<span<Base>, D, D>);
        static_assert(!is_constructible_v<span<Base, 3>, D, D>);

        span<int> sp_dyn_w(stl.begin(), stl.end());
        span<int> sp_dyn_x(stl.begin(), stl.cend());
        assert(sp_dyn_w.data() == stl.data());
        assert(sp_dyn_x.data() == stl.data());
        assert(sp_dyn_w.size() == 3);
        assert(sp_dyn_x.size() == 3);

        span<int, 3> sp_three_w(stl.begin(), stl.end());
        span<int, 3> sp_three_x(stl.begin(), stl.cend());
        assert(sp_three_w.data() == stl.data());
        assert(sp_three_x.data() == stl.data());
        assert(sp_three_w.size() == 3);
        assert(sp_three_x.size() == 3);

        span<const int> sp_const_w(stl.begin(), stl.end());
        span<const int> sp_const_x(stl.begin(), stl.cend());
        span<const int> sp_const_y(stl.cbegin(), stl.end());
        span<const int> sp_const_z(stl.cbegin(), stl.cend());
        assert(sp_const_w.data() == stl.data());
        assert(sp_const_x.data() == stl.data());
        assert(sp_const_y.data() == stl.data());
        assert(sp_const_z.data() == stl.data());
        assert(sp_const_w.size() == 3);
        assert(sp_const_x.size() == 3);
        assert(sp_const_y.size() == 3);
        assert(sp_const_z.size() == 3);

        FunctionTakingSpan<int>({stl.begin(), stl.end()});
        FunctionTakingSpan<int>({stl.begin(), stl.cend()});
        FunctionTakingSpan<const int>({stl.begin(), stl.end()});
        FunctionTakingSpan<const int>({stl.begin(), stl.cend()});
        FunctionTakingSpan<const int>({stl.cbegin(), stl.end()});
        FunctionTakingSpan<const int>({stl.cbegin(), stl.cend()});

        static_assert(is_same_v<decltype(span{stl.begin(), stl.end()}), span<int>>);
        static_assert(is_same_v<decltype(span{stl.begin(), stl.cend()}), span<int>>);
        static_assert(is_same_v<decltype(span{stl.cbegin(), stl.end()}), span<const int>>);
        static_assert(is_same_v<decltype(span{stl.cbegin(), stl.cend()}), span<const int>>);
    }

    {
        static_assert(is_nothrow_constructible_v<span<int>, int (&)[3]>);
        static_assert(!is_constructible_v<span<int>, const int (&)[3]>);
        static_assert(!is_constructible_v<span<int>, double (&)[3]>);
        static_assert(is_nothrow_constructible_v<span<int>, array<int, 3>&>);
        static_assert(!is_constructible_v<span<int>, const array<int, 3>&>);
        static_assert(!is_constructible_v<span<int>, array<double, 3>&>);

        static_assert(is_nothrow_constructible_v<span<int, 3>, int (&)[3]>);
        static_assert(!is_constructible_v<span<int, 3>, const int (&)[3]>);
        static_assert(!is_constructible_v<span<int, 3>, double (&)[3]>);
        static_assert(is_nothrow_constructible_v<span<int, 3>, array<int, 3>&>);
        static_assert(!is_constructible_v<span<int, 3>, const array<int, 3>&>);
        static_assert(!is_constructible_v<span<int, 3>, array<double, 3>&>);
        static_assert(!is_constructible_v<span<int, 3>, int (&)[500]>);
        static_assert(!is_constructible_v<span<int, 3>, array<int, 500>&>);

        static_assert(is_nothrow_constructible_v<span<const int>, int (&)[3]>);
        static_assert(is_nothrow_constructible_v<span<const int>, const int (&)[3]>);
        static_assert(!is_constructible_v<span<const int>, double (&)[3]>);
        static_assert(is_nothrow_constructible_v<span<const int>, array<int, 3>&>);
        static_assert(is_nothrow_constructible_v<span<const int>, const array<int, 3>&>);
        static_assert(!is_constructible_v<span<const int>, array<double, 3>&>);

        static_assert(is_nothrow_convertible_v<int (&)[3], span<int>>);
        static_assert(is_nothrow_convertible_v<array<int, 3>&, span<int>>);
        static_assert(is_nothrow_convertible_v<int (&)[3], span<int, 3>>);
        static_assert(is_nothrow_convertible_v<array<int, 3>&, span<int, 3>>);
        static_assert(is_nothrow_convertible_v<int (&)[3], span<const int>>);
        static_assert(is_nothrow_convertible_v<const int (&)[3], span<const int>>);
        static_assert(is_nothrow_convertible_v<array<int, 3>&, span<const int>>);
        static_assert(is_nothrow_convertible_v<const array<int, 3>&, span<const int>>);

        static_assert(is_nothrow_constructible_v<span<Base>, Base(&)[3]>);
        static_assert(is_nothrow_constructible_v<span<Base>, array<Base, 3>&>);
        static_assert(is_nothrow_constructible_v<span<Base, 3>, Base(&)[3]>);
        static_assert(is_nothrow_constructible_v<span<Base, 3>, array<Base, 3>&>);
        static_assert(is_nothrow_constructible_v<span<const Base>, Base(&)[3]>);
        static_assert(is_nothrow_constructible_v<span<const Base>, const Base(&)[3]>);
        static_assert(is_nothrow_constructible_v<span<const Base>, array<Base, 3>&>);
        static_assert(is_nothrow_constructible_v<span<const Base>, const array<Base, 3>&>);
        static_assert(!is_constructible_v<span<Base>, Derived(&)[3]>);
        static_assert(!is_constructible_v<span<Base>, array<Derived, 3>&>);
        static_assert(!is_constructible_v<span<Base, 3>, Derived(&)[3]>);
        static_assert(!is_constructible_v<span<Base, 3>, array<Derived, 3>&>);
        static_assert(!is_constructible_v<span<const Base>, Derived(&)[3]>);
        static_assert(!is_constructible_v<span<const Base>, const Derived(&)[3]>);
        static_assert(!is_constructible_v<span<const Base>, array<Derived, 3>&>);
        static_assert(!is_constructible_v<span<const Base>, const array<Derived, 3>&>);

        span<int> sp_dyn_a(arr);
        span<int> sp_dyn_b(stl);
        assert(sp_dyn_a.data() == begin(arr));
        assert(sp_dyn_b.data() == stl.data());
        assert(sp_dyn_a.size() == 3);
        assert(sp_dyn_b.size() == 3);

        span<int, 3> sp_three_a(arr);
        span<int, 3> sp_three_b(stl);
        assert(sp_three_a.data() == begin(arr));
        assert(sp_three_b.data() == stl.data());
        assert(sp_three_a.size() == 3);
        assert(sp_three_b.size() == 3);

        span<const int> sp_const_w(arr);
        span<const int> sp_const_x(as_const(arr));
        span<const int> sp_const_y(stl);
        span<const int> sp_const_z(as_const(stl));
        span<const int* const> sp_const_nullptr_1{stl_nullptr};
        span<const int* const> sp_const_nullptr_2{as_const(stl_nullptr)};
        assert(sp_const_w.data() == begin(arr));
        assert(sp_const_x.data() == begin(arr));
        assert(sp_const_y.data() == stl.data());
        assert(sp_const_z.data() == stl.data());
        assert(sp_const_nullptr_1.data() == stl_nullptr.data());
        assert(sp_const_nullptr_2.data() == stl_nullptr.data());
        assert(sp_const_w.size() == 3);
        assert(sp_const_x.size() == 3);
        assert(sp_const_y.size() == 3);
        assert(sp_const_z.size() == 3);
        assert(sp_const_nullptr_1.size() == 3);
        assert(sp_const_nullptr_2.size() == 3);

        FunctionTakingSpan<int>(arr);
        FunctionTakingSpan<int>(stl);
        FunctionTakingSpan<int*>(stl_nullptr);
        FunctionTakingSpan<int, 3>(arr);
        FunctionTakingSpan<int, 3>(stl);
        FunctionTakingSpan<int*, 3>(stl_nullptr);
        FunctionTakingSpan<const int>(arr);
        FunctionTakingSpan<const int>(as_const(arr));
        FunctionTakingSpan<const int>(stl);
        FunctionTakingSpan<const int>(as_const(stl));
        FunctionTakingSpan<const int* const>(stl_nullptr);
        FunctionTakingSpan<const int* const>(as_const(stl_nullptr));

        static_assert(is_same_v<decltype(span{arr}), span<int, 3>>);
        static_assert(is_same_v<decltype(span{as_const(arr)}), span<const int, 3>>);
        static_assert(is_same_v<decltype(span{stl}), span<int, 3>>);
        static_assert(is_same_v<decltype(span{as_const(stl)}), span<const int, 3>>);
        static_assert(is_same_v<decltype(span{stl_nullptr}), span<int*, 3>>);
        static_assert(is_same_v<decltype(span{as_const(stl_nullptr)}), span<int* const, 3>>);
    }

    {
        static_assert(is_constructible_v<span<int>, ContiguousSizedRange&>);
        static_assert(is_constructible_v<span<int, 3>, ContiguousSizedRange&>);

        static_assert(!is_constructible_v<span<int>, NonRange&>);
        static_assert(!is_constructible_v<span<int, 3>, NonRange&>);

        static_assert(!is_constructible_v<span<int>, const ContiguousSizedRange&>);
        static_assert(!is_constructible_v<span<int, 3>, const ContiguousSizedRange&>);
        static_assert(is_constructible_v<span<const int>, ContiguousSizedRange&>);
        static_assert(is_constructible_v<span<const int, 3>, ContiguousSizedRange&>);
        static_assert(is_constructible_v<span<const int>, const ContiguousSizedRange&>);
        static_assert(is_constructible_v<span<const int, 3>, const ContiguousSizedRange&>);
        static_assert(!is_constructible_v<span<double>, ContiguousSizedRange&>);
        static_assert(!is_constructible_v<span<double, 3>, ContiguousSizedRange&>);

        static_assert(is_convertible_v<ContiguousSizedRange&, span<int>>);
        static_assert(!is_convertible_v<ContiguousSizedRange&, span<int, 3>>);
        static_assert(is_convertible_v<ContiguousSizedRange&, span<const int>>);
        static_assert(!is_convertible_v<ContiguousSizedRange&, span<const int, 3>>);
        static_assert(is_convertible_v<const ContiguousSizedRange&, span<const int>>);
        static_assert(!is_convertible_v<const ContiguousSizedRange&, span<const int, 3>>);

        static_assert(is_constructible_v<span<Base>, BasicRange<Base>&>);
        static_assert(is_constructible_v<span<Base, 3>, BasicRange<Base>&>);
        static_assert(!is_constructible_v<span<Base>, BasicRange<Derived>&>);
        static_assert(!is_constructible_v<span<Base, 3>, BasicRange<Derived>&>);

        ContiguousSizedRange user_range;

        span<int> sp_1(user_range);
        span<const int> sp_2(user_range);
        span<const int> sp_3(as_const(user_range));

        span<int, 3> sp_4(user_range);
        span<const int, 3> sp_5(user_range);
        span<const int, 3> sp_6(as_const(user_range));

        assert(sp_1.data() == user_range.data());
        assert(sp_2.data() == user_range.data());
        assert(sp_3.data() == user_range.data());
        assert(sp_4.data() == user_range.data());
        assert(sp_5.data() == user_range.data());
        assert(sp_6.data() == user_range.data());

        assert(sp_1.size() == 3);
        assert(sp_2.size() == 3);
        assert(sp_3.size() == 3);
        assert(sp_4.size() == 3);
        assert(sp_5.size() == 3);
        assert(sp_6.size() == 3);

        FunctionTakingSpan<int>(user_range);
        FunctionTakingSpan<const int>(user_range);
        FunctionTakingSpan<const int>(as_const(user_range));

        static_assert(!is_constructible_v<span<int>, ContiguousSizedRange>);
        static_assert(!is_constructible_v<span<int, 3>, ContiguousSizedRange>);
        static_assert(is_constructible_v<span<int>, BorrowedContiguousSizedRange>);
        static_assert(is_constructible_v<span<int, 3>, BorrowedContiguousSizedRange>);
        static_assert(is_constructible_v<span<const int>, ContiguousSizedRange>);
        static_assert(is_constructible_v<span<const int, 3>, ContiguousSizedRange>);

        static_assert(is_convertible_v<BorrowedContiguousSizedRange, span<int>>);
        static_assert(!is_convertible_v<BorrowedContiguousSizedRange, span<int, 3>>);
        static_assert(is_convertible_v<ContiguousSizedRange, span<const int>>);
        static_assert(!is_convertible_v<ContiguousSizedRange, span<const int, 3>>);

        BorrowedContiguousSizedRange borrowed_user_range;

        span<int> sp_7(move(borrowed_user_range));
        span<int, 3> sp_8(move(borrowed_user_range));
        span<const int> sp_9(move(user_range));
        span<const int, 3> sp_10(move(user_range));

        assert(sp_7.data() == borrowed_user_range.data());
        assert(sp_8.data() == borrowed_user_range.data());
        assert(sp_9.data() == user_range.data());
        assert(sp_10.data() == user_range.data());

        assert(sp_7.size() == 3);
        assert(sp_8.size() == 3);
        assert(sp_9.size() == 3);
        assert(sp_10.size() == 3);

        FunctionTakingSpan<int>(move(borrowed_user_range));
        FunctionTakingSpan<const int>(move(user_range));

        static_assert(is_same_v<decltype(span{user_range}), span<int>>);
        static_assert(is_same_v<decltype(span{as_const(user_range)}), span<const int>>);
    }

    {
        static_assert(is_nothrow_copy_constructible_v<span<int>>);
        static_assert(is_nothrow_copy_constructible_v<span<int, 3>>);
        static_assert(is_nothrow_copy_constructible_v<span<const int>>);
        static_assert(is_nothrow_copy_constructible_v<span<const int, 3>>);

        static_assert(is_nothrow_constructible_v<span<int>, const span<int>&>);
        static_assert(is_nothrow_constructible_v<span<int>, const span<int, 3>&>);
        static_assert(is_nothrow_constructible_v<span<int>, const span<int, 500>&>);
        static_assert(!is_constructible_v<span<int>, const span<const int>&>);
        static_assert(!is_constructible_v<span<int>, const span<const int, 3>&>);
        static_assert(!is_constructible_v<span<int>, const span<const int, 500>&>);
        static_assert(!is_constructible_v<span<int>, const span<double, 3>&>);

        static_assert(is_constructible_v<span<int, 3>, const span<int>&>);
        static_assert(is_nothrow_constructible_v<span<int, 3>, const span<int, 3>&>);
        static_assert(!is_constructible_v<span<int, 3>, const span<int, 500>&>);
        static_assert(!is_constructible_v<span<int, 3>, const span<const int>&>);
        static_assert(!is_constructible_v<span<int, 3>, const span<const int, 3>&>);
        static_assert(!is_constructible_v<span<int, 3>, const span<const int, 500>&>);
        static_assert(!is_constructible_v<span<int, 3>, const span<double, 3>&>);

        static_assert(is_nothrow_constructible_v<span<const int>, const span<int>&>);
        static_assert(is_nothrow_constructible_v<span<const int>, const span<int, 3>&>);
        static_assert(is_nothrow_constructible_v<span<const int>, const span<int, 500>&>);
        static_assert(is_nothrow_constructible_v<span<const int>, const span<const int>&>);
        static_assert(is_nothrow_constructible_v<span<const int>, const span<const int, 3>&>);
        static_assert(is_nothrow_constructible_v<span<const int>, const span<const int, 500>&>);
        static_assert(!is_constructible_v<span<const int>, const span<double, 3>&>);

        static_assert(is_constructible_v<span<const int, 3>, const span<int>&>);
        static_assert(is_nothrow_constructible_v<span<const int, 3>, const span<int, 3>&>);
        static_assert(!is_constructible_v<span<const int, 3>, const span<int, 500>&>);
        static_assert(is_constructible_v<span<const int, 3>, const span<const int>&>);
        static_assert(is_nothrow_constructible_v<span<const int, 3>, const span<const int, 3>&>);
        static_assert(!is_constructible_v<span<const int, 3>, const span<const int, 500>&>);
        static_assert(!is_constructible_v<span<const int, 3>, const span<double, 3>&>);

        static_assert(is_nothrow_convertible_v<const span<int>&, span<int>>);
        static_assert(is_nothrow_convertible_v<const span<int, 3>&, span<int>>);
        static_assert(is_nothrow_convertible_v<const span<int, 500>&, span<int>>);
        static_assert(is_nothrow_convertible_v<const span<int, 3>&, span<int, 3>>);
        static_assert(is_nothrow_convertible_v<const span<int>&, span<const int>>);
        static_assert(is_nothrow_convertible_v<const span<int, 3>&, span<const int>>);
        static_assert(is_nothrow_convertible_v<const span<int, 500>&, span<const int>>);
        static_assert(is_nothrow_convertible_v<const span<const int>&, span<const int>>);
        static_assert(is_nothrow_convertible_v<const span<const int, 3>&, span<const int>>);
        static_assert(is_nothrow_convertible_v<const span<const int, 500>&, span<const int>>);
        static_assert(is_nothrow_convertible_v<const span<int, 3>&, span<const int, 3>>);
        static_assert(is_nothrow_convertible_v<const span<const int, 3>&, span<const int, 3>>);

        static_assert(is_nothrow_constructible_v<span<Base>, const span<Base>&>);
        static_assert(is_nothrow_constructible_v<span<Base>, const span<Base, 3>&>);
        static_assert(is_nothrow_constructible_v<span<Base, 3>, const span<Base, 3>&>);
        static_assert(is_nothrow_constructible_v<span<const Base>, const span<Base>&>);
        static_assert(is_nothrow_constructible_v<span<const Base>, const span<Base, 3>&>);
        static_assert(is_nothrow_constructible_v<span<const Base>, const span<const Base>&>);
        static_assert(is_nothrow_constructible_v<span<const Base>, const span<const Base, 3>&>);
        static_assert(is_nothrow_constructible_v<span<const Base, 3>, const span<Base, 3>&>);
        static_assert(is_nothrow_constructible_v<span<const Base, 3>, const span<const Base, 3>&>);
        static_assert(!is_constructible_v<span<Base>, const span<Derived>&>);
        static_assert(!is_constructible_v<span<Base>, const span<Derived, 3>&>);
        static_assert(!is_constructible_v<span<Base, 3>, const span<Derived, 3>&>);
        static_assert(!is_constructible_v<span<const Base>, const span<Derived>&>);
        static_assert(!is_constructible_v<span<const Base>, const span<Derived, 3>&>);
        static_assert(!is_constructible_v<span<const Base>, const span<const Derived>&>);
        static_assert(!is_constructible_v<span<const Base>, const span<const Derived, 3>&>);
        static_assert(!is_constructible_v<span<const Base, 3>, const span<Derived, 3>&>);
        static_assert(!is_constructible_v<span<const Base, 3>, const span<const Derived, 3>&>);

        const span<int> orig_dyn(arr);
        const span<int, 3> orig_three(arr);
        const span<const int> orig_const_dyn(arr);
        const span<const int, 3> orig_const_three(arr);

        span<int> sp_a(orig_dyn);
        span<int> sp_b(orig_three);

        span<int, 3> sp_c(orig_three);

        span<const int> sp_d(orig_dyn);
        span<const int> sp_e(orig_three);
        span<const int> sp_f(orig_const_dyn);
        span<const int> sp_g(orig_const_three);

        span<const int, 3> sp_h(orig_three);
        span<const int, 3> sp_i(orig_const_three);

        assert(sp_a.data() == begin(arr));
        assert(sp_b.data() == begin(arr));
        assert(sp_c.data() == begin(arr));
        assert(sp_d.data() == begin(arr));
        assert(sp_e.data() == begin(arr));
        assert(sp_f.data() == begin(arr));
        assert(sp_g.data() == begin(arr));
        assert(sp_h.data() == begin(arr));
        assert(sp_i.data() == begin(arr));
        assert(sp_a.size() == 3);
        assert(sp_b.size() == 3);
        assert(sp_c.size() == 3);
        assert(sp_d.size() == 3);
        assert(sp_e.size() == 3);
        assert(sp_f.size() == 3);
        assert(sp_g.size() == 3);
        assert(sp_h.size() == 3);
        assert(sp_i.size() == 3);

        FunctionTakingSpan<int>(orig_dyn);
        FunctionTakingSpan<int>(orig_three);
        FunctionTakingSpan<int, 3>(orig_three);
        FunctionTakingSpan<const int>(orig_dyn);
        FunctionTakingSpan<const int>(orig_three);
        FunctionTakingSpan<const int>(orig_const_dyn);
        FunctionTakingSpan<const int>(orig_const_three);
        FunctionTakingSpan<const int, 3>(orig_three);
        FunctionTakingSpan<const int, 3>(orig_const_three);

        static_assert(is_same_v<decltype(span{orig_dyn}), span<int>>);
        static_assert(is_same_v<decltype(span{orig_three}), span<int, 3>>);
        static_assert(is_same_v<decltype(span{orig_const_dyn}), span<const int>>);
        static_assert(is_same_v<decltype(span{orig_const_three}), span<const int, 3>>);
    }

    {
        static_assert(is_nothrow_copy_assignable_v<span<int>>);
        static_assert(is_nothrow_copy_assignable_v<span<int, 3>>);
        static_assert(is_nothrow_copy_assignable_v<span<const int>>);
        static_assert(is_nothrow_copy_assignable_v<span<const int, 3>>);

        span<int> sp_dyn(arr);
        span<int, 3> sp_three(arr);
        span<const int> sp_const_dyn(arr);
        span<const int, 3> sp_const_three(arr);

        assert(sp_dyn.data() == begin(arr));
        assert(sp_three.data() == begin(arr));
        assert(sp_const_dyn.data() == begin(arr));
        assert(sp_const_three.data() == begin(arr));
        assert(sp_dyn.size() == 3);
        assert(sp_three.size() == 3);
        assert(sp_const_dyn.size() == 3);
        assert(sp_const_three.size() == 3);

        int other[4]{12, 34, 56, 78};

        sp_dyn         = span<int>{other};
        sp_three       = span<int, 3>{stl};
        sp_const_dyn   = span<const int>{other};
        sp_const_three = span<const int, 3>{stl};

        assert(sp_dyn.data() == begin(other));
        assert(sp_three.data() == stl.data());
        assert(sp_const_dyn.data() == begin(other));
        assert(sp_const_three.data() == stl.data());
        assert(sp_dyn.size() == 4);
        assert(sp_three.size() == 3);
        assert(sp_const_dyn.size() == 4);
        assert(sp_const_three.size() == 3);
    }

    {
        static_assert(is_nothrow_destructible_v<span<int>>);
        static_assert(is_nothrow_destructible_v<span<int, 3>>);
        static_assert(is_nothrow_destructible_v<span<const int>>);

        span<int>::iterator it_dyn{};

        {
            span<int> sp_dyn(arr);
            it_dyn = sp_dyn.begin();
        }

        assert(*it_dyn == arr[0]);
        assert(it_dyn[2] == arr[2]);

        span<int, 3>::iterator it_three{};

        {
            span<int, 3> sp_three(stl);
            it_three = sp_three.begin();
        }

        assert(*it_three == stl[0]);
        assert(it_three[2] == stl[2]);
    }

    {
        int sequence[9]{10, 20, 30, 40, 50, 60, 70, 80, 90};

        const span<int> sp_dyn(sequence);
        const span<int, 9> sp_nine(sequence);

        auto first_3 = sp_dyn.first<3>();
        auto first_4 = sp_nine.first<4>();
        auto first_5 = sp_dyn.first(5);
        auto first_6 = sp_nine.first(6);
        static_assert(noexcept(sp_dyn.first<3>())); // strengthened
        static_assert(noexcept(sp_nine.first<4>())); // strengthened
        static_assert(noexcept(sp_dyn.first(5))); // strengthened
        static_assert(noexcept(sp_nine.first(6))); // strengthened
        static_assert(is_same_v<decltype(first_3), span<int, 3>>);
        static_assert(is_same_v<decltype(first_4), span<int, 4>>);
        static_assert(is_same_v<decltype(first_5), span<int>>);
        static_assert(is_same_v<decltype(first_6), span<int>>);
        assert(first_3.data() == begin(sequence));
        assert(first_4.data() == begin(sequence));
        assert(first_5.data() == begin(sequence));
        assert(first_6.data() == begin(sequence));
        assert(first_3.size() == 3);
        assert(first_4.size() == 4);
        assert(first_5.size() == 5);
        assert(first_6.size() == 6);

        auto last_3 = sp_dyn.last<3>();
        auto last_4 = sp_nine.last<4>();
        auto last_5 = sp_dyn.last(5);
        auto last_6 = sp_nine.last(6);
        static_assert(noexcept(sp_dyn.last<3>())); // strengthened
        static_assert(noexcept(sp_nine.last<4>())); // strengthened
        static_assert(noexcept(sp_dyn.last(5))); // strengthened
        static_assert(noexcept(sp_nine.last(6))); // strengthened
        static_assert(is_same_v<decltype(last_3), span<int, 3>>);
        static_assert(is_same_v<decltype(last_4), span<int, 4>>);
        static_assert(is_same_v<decltype(last_5), span<int>>);
        static_assert(is_same_v<decltype(last_6), span<int>>);
        assert(last_3.data() == begin(sequence) + 6);
        assert(last_4.data() == begin(sequence) + 5);
        assert(last_5.data() == begin(sequence) + 4);
        assert(last_6.data() == begin(sequence) + 3);
        assert(last_3.size() == 3);
        assert(last_4.size() == 4);
        assert(last_5.size() == 5);
        assert(last_6.size() == 6);

        auto offset_3 = sp_dyn.subspan<3>();
        auto offset_4 = sp_nine.subspan<4>();
        auto offset_5 = sp_dyn.subspan(5);
        auto offset_6 = sp_nine.subspan(6);
        static_assert(noexcept(sp_dyn.subspan<3>())); // strengthened
        static_assert(noexcept(sp_nine.subspan<4>())); // strengthened
        static_assert(noexcept(sp_dyn.subspan(5))); // strengthened
        static_assert(noexcept(sp_nine.subspan(6))); // strengthened
        static_assert(is_same_v<decltype(offset_3), span<int>>);
        static_assert(is_same_v<decltype(offset_4), span<int, 5>>);
        static_assert(is_same_v<decltype(offset_5), span<int>>);
        static_assert(is_same_v<decltype(offset_6), span<int>>);
        assert(offset_3.data() == begin(sequence) + 3);
        assert(offset_4.data() == begin(sequence) + 4);
        assert(offset_5.data() == begin(sequence) + 5);
        assert(offset_6.data() == begin(sequence) + 6);
        assert(offset_3.size() == 6);
        assert(offset_4.size() == 5);
        assert(offset_5.size() == 4);
        assert(offset_6.size() == 3);

        auto subspan_3 = sp_dyn.subspan<3, 2>();
        auto subspan_4 = sp_nine.subspan<4, 2>();
        auto subspan_5 = sp_dyn.subspan(5, 2);
        auto subspan_6 = sp_nine.subspan(6, 2);
        static_assert(noexcept(sp_dyn.subspan<3, 2>())); // strengthened
        static_assert(noexcept(sp_nine.subspan<4, 2>())); // strengthened
        static_assert(noexcept(sp_dyn.subspan(5, 2))); // strengthened
        static_assert(noexcept(sp_nine.subspan(6, 2))); // strengthened
        static_assert(is_same_v<decltype(subspan_3), span<int, 2>>);
        static_assert(is_same_v<decltype(subspan_4), span<int, 2>>);
        static_assert(is_same_v<decltype(subspan_5), span<int>>);
        static_assert(is_same_v<decltype(subspan_6), span<int>>);
        assert(subspan_3.data() == begin(sequence) + 3);
        assert(subspan_4.data() == begin(sequence) + 4);
        assert(subspan_5.data() == begin(sequence) + 5);
        assert(subspan_6.data() == begin(sequence) + 6);
        assert(subspan_3.size() == 2);
        assert(subspan_4.size() == 2);
        assert(subspan_5.size() == 2);
        assert(subspan_6.size() == 2);

        static_assert(noexcept(sp_dyn.size()));
        static_assert(noexcept(sp_dyn.size_bytes()));
        static_assert(noexcept(sp_dyn.empty()));
        static_assert(noexcept(sp_dyn[0])); // strengthened
        static_assert(noexcept(sp_dyn.front())); // strengthened
        static_assert(noexcept(sp_dyn.back())); // strengthened
        static_assert(noexcept(sp_dyn.data()));
        static_assert(noexcept(sp_dyn.begin()));
        static_assert(noexcept(sp_dyn.end()));
        static_assert(noexcept(sp_dyn.rbegin()));
        static_assert(noexcept(sp_dyn.rend()));

        static_assert(noexcept(sp_nine.size()));
        static_assert(noexcept(sp_nine.size_bytes()));
        static_assert(noexcept(sp_nine.empty()));
        static_assert(noexcept(sp_nine[0])); // strengthened
        static_assert(noexcept(sp_nine.front())); // strengthened
        static_assert(noexcept(sp_nine.back())); // strengthened
        static_assert(noexcept(sp_nine.data()));
        static_assert(noexcept(sp_nine.begin()));
        static_assert(noexcept(sp_nine.end()));
        static_assert(noexcept(sp_nine.rbegin()));
        static_assert(noexcept(sp_nine.rend()));

        assert(sp_dyn.size() == 9);
        assert(sp_nine.size() == 9);

        assert(sp_dyn.size_bytes() == 9 * sizeof(int));
        assert(sp_nine.size_bytes() == 9 * sizeof(int));

        assert(!sp_dyn.empty());
        assert(!sp_nine.empty());

        assert(sp_dyn[0] == 10);
        assert(sp_nine[0] == 10);
        assert(sp_dyn[8] == 90);
        assert(sp_nine[8] == 90);

        assert(sp_dyn.front() == 10);
        assert(sp_nine.front() == 10);

        assert(sp_dyn.back() == 90);
        assert(sp_nine.back() == 90);

        assert(&sp_dyn.front() == begin(sequence));
        assert(&sp_nine.front() == begin(sequence));
        assert(&sp_dyn[4] == begin(sequence) + 4);
        assert(&sp_nine[4] == begin(sequence) + 4);
        assert(&sp_dyn.back() == begin(sequence) + 8);
        assert(&sp_nine.back() == begin(sequence) + 8);

        assert(sp_dyn.data() == begin(sequence));
        assert(sp_nine.data() == begin(sequence));

        assert(*sp_dyn.begin() == 10);
        assert(*sp_nine.begin() == 10);

        assert(sp_dyn.end()[-2] == 80);
        assert(sp_nine.end()[-2] == 80);

        assert(*sp_dyn.rbegin() == 90);
        assert(*sp_nine.rbegin() == 90);

        assert(sp_dyn.rend()[-2] == 20);
        assert(sp_nine.rend()[-2] == 20);

        static_assert(is_same_v<decltype(sp_dyn.begin()), span<int>::iterator>);
        static_assert(is_same_v<decltype(sp_nine.begin()), span<int, 9>::iterator>);
        static_assert(is_same_v<decltype(sp_dyn.end()), span<int>::iterator>);
        static_assert(is_same_v<decltype(sp_nine.end()), span<int, 9>::iterator>);
        static_assert(is_same_v<decltype(sp_dyn.rbegin()), span<int>::reverse_iterator>);
        static_assert(is_same_v<decltype(sp_nine.rbegin()), span<int, 9>::reverse_iterator>);
        static_assert(is_same_v<decltype(sp_dyn.rend()), span<int>::reverse_iterator>);
        static_assert(is_same_v<decltype(sp_nine.rend()), span<int, 9>::reverse_iterator>);
    }

    return true;
}

void test_non_constexpr() {
    int sequence[9]{10, 20, 30, 40, 50, 60, 70, 80, 90};

    constexpr size_t SizeBytes = sizeof(sequence);

    const span<int> sp_dyn(sequence);
    const span<int, 9> sp_nine(sequence);
    const span<const int> sp_const_dyn(sequence);
    const span<const int, 9> sp_const_nine(sequence);

    static_assert(noexcept(as_bytes(sp_dyn)));
    static_assert(noexcept(as_bytes(sp_nine)));
    static_assert(noexcept(as_bytes(sp_const_dyn)));
    static_assert(noexcept(as_bytes(sp_const_nine)));
    static_assert(noexcept(as_writable_bytes(sp_dyn)));
    static_assert(noexcept(as_writable_bytes(sp_nine)));

    static_assert(AsWritableBytesCompilesFor<span<int>>);
    static_assert(AsWritableBytesCompilesFor<span<int, 9>>);
    static_assert(!AsWritableBytesCompilesFor<span<const int>>);
    static_assert(!AsWritableBytesCompilesFor<span<const int, 9>>);

    auto sp_1 = as_bytes(sp_dyn);
    auto sp_2 = as_bytes(sp_nine);
    auto sp_3 = as_bytes(sp_const_dyn);
    auto sp_4 = as_bytes(sp_const_nine);
    auto sp_5 = as_writable_bytes(sp_dyn);
    auto sp_6 = as_writable_bytes(sp_nine);

    static_assert(is_same_v<decltype(sp_1), span<const byte>>);
    static_assert(is_same_v<decltype(sp_2), span<const byte, SizeBytes>>);
    static_assert(is_same_v<decltype(sp_3), span<const byte>>);
    static_assert(is_same_v<decltype(sp_4), span<const byte, SizeBytes>>);
    static_assert(is_same_v<decltype(sp_5), span<byte>>);
    static_assert(is_same_v<decltype(sp_6), span<byte, SizeBytes>>);

    assert(sp_1.data() == reinterpret_cast<const byte*>(begin(sequence)));
    assert(sp_2.data() == reinterpret_cast<const byte*>(begin(sequence)));
    assert(sp_3.data() == reinterpret_cast<const byte*>(begin(sequence)));
    assert(sp_4.data() == reinterpret_cast<const byte*>(begin(sequence)));
    assert(sp_5.data() == reinterpret_cast<byte*>(begin(sequence)));
    assert(sp_6.data() == reinterpret_cast<byte*>(begin(sequence)));

    assert(sp_1.size() == SizeBytes);
    assert(sp_2.size() == SizeBytes);
    assert(sp_3.size() == SizeBytes);
    assert(sp_4.size() == SizeBytes);
    assert(sp_5.size() == SizeBytes);
    assert(sp_6.size() == SizeBytes);
}

#if !_HAS_CXX23 // TRANSITION, ABI, const_iterator<_Span_iterator<holder<incomplete>*>> is ill-formed due to ADL in
                // constraints checking
#ifndef _M_CEE // TRANSITION, VSO-1659496
// GH-1596: "<algorithm>: unqualified calls to _Adl_verify_range incorrectly cause instantiation"
template <class T>
struct holder {
    T t;
};

struct incomplete;

void test_adl_proof_span_constructors() { // COMPILE-ONLY
    using validator = holder<incomplete>*;
    validator varr[1]{};

    [[maybe_unused]] span<validator> s1{varr, varr + 1};
    [[maybe_unused]] span<validator> s2{varr, 1};
}
#endif // ^^^ no workaround ^^^
#endif // !_HAS_CXX23

int main() {
    static_assert(test());
    assert(test());

    test_non_constexpr();
}
