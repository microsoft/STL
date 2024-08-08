// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#if !defined(__clang__) && !defined(__EDG__)
// Invalid annotation: 'NullTerminated' property may only be used on buffers whose elements are of integral or pointer
// type
#pragma warning(disable : 6510)
#endif // !defined(__clang__) && !defined(__EDG__)

#include <algorithm>
#include <array>
#include <cassert>
#include <cwchar>
#include <deque>
#include <forward_list>
#include <iosfwd>
#include <list>
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#if _HAS_CXX20
#include <ranges>
#endif // _HAS_CXX20

#if _HAS_CXX23
#include <any>
#include <expected>
#endif // _HAS_CXX23

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

#if __has_cpp_attribute(nodiscard) != 0
#define NODISCARD [[nodiscard]]
#else // ^^^ __has_cpp_attribute(nodiscard) != 0 / __has_cpp_attribute(nodiscard) == 0 vvv
#define NODISCARD
#endif // ^^^ __has_cpp_attribute(nodiscard) == 0 ^^^

using namespace std;

struct any_convertible_equal {
    any_convertible_equal() = default;

    template <class T, enable_if_t<!is_same_v<remove_cv_t<remove_reference_t<T>>, any_convertible_equal>, int> = 0>
    constexpr any_convertible_equal(T&&) noexcept {}

    // intentionally provide redundant overloads to test potential ambiguity
#if _HAS_CXX20
    NODISCARD friend bool operator==(const any_convertible_equal&, const any_convertible_equal&) = default;

    NODISCARD friend auto operator<=>(const any_convertible_equal&, const any_convertible_equal&) = default;
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    NODISCARD friend constexpr bool operator==(const any_convertible_equal&, const any_convertible_equal&) noexcept {
        return true;
    }

    NODISCARD friend constexpr bool operator!=(const any_convertible_equal&, const any_convertible_equal&) noexcept {
        return false;
    }

    NODISCARD friend constexpr bool operator<(const any_convertible_equal&, const any_convertible_equal&) noexcept {
        return false;
    }

    NODISCARD friend constexpr bool operator>(const any_convertible_equal&, const any_convertible_equal&) noexcept {
        return false;
    }

    NODISCARD friend constexpr bool operator<=(const any_convertible_equal&, const any_convertible_equal&) noexcept {
        return true;
    }

    NODISCARD friend constexpr bool operator>=(const any_convertible_equal&, const any_convertible_equal&) noexcept {
        return true;
    }
#endif // ^^^ !_HAS_CXX20 ^^^

    template <class T>
    NODISCARD friend constexpr bool operator==(const any_convertible_equal&, const T&) noexcept {
        return true;
    }

#if !_HAS_CXX20
    template <class T>
    NODISCARD friend constexpr bool operator==(const T&, const any_convertible_equal&) noexcept {
        return true;
    }

    template <class T>
    NODISCARD friend constexpr bool operator!=(const any_convertible_equal&, const T&) noexcept {
        return false;
    }
    template <class T>
    NODISCARD friend constexpr bool operator!=(const T&, const any_convertible_equal&) noexcept {
        return false;
    }
#endif

#if _HAS_CXX20
    template <class T>
    NODISCARD friend constexpr strong_ordering operator<=>(const any_convertible_equal&, const T&) noexcept {
        return strong_ordering::equal;
    }
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20
    template <class T>
    NODISCARD friend constexpr bool operator<(const any_convertible_equal&, const T&) noexcept {
        return false;
    }
    template <class T>
    NODISCARD friend constexpr bool operator<(const T&, const any_convertible_equal&) noexcept {
        return false;
    }

    template <class T>
    NODISCARD friend constexpr bool operator>(const any_convertible_equal&, const T&) noexcept {
        return false;
    }
    template <class T>
    NODISCARD friend constexpr bool operator>(const T&, const any_convertible_equal&) noexcept {
        return false;
    }

    template <class T>
    NODISCARD friend constexpr bool operator<=(const any_convertible_equal&, const T&) noexcept {
        return true;
    }
    template <class T>
    NODISCARD friend constexpr bool operator<=(const T&, const any_convertible_equal&) noexcept {
        return true;
    }

    template <class T>
    NODISCARD friend constexpr bool operator>=(const any_convertible_equal&, const T&) noexcept {
        return true;
    }
    template <class T>
    NODISCARD friend constexpr bool operator>=(const T&, const any_convertible_equal&) noexcept {
        return true;
    }
#endif // ^^^ _HAS_CXX20 ^^^

    NODISCARD friend constexpr ptrdiff_t operator-(
        const any_convertible_equal&, const any_convertible_equal&) noexcept {
        return 0;
    }
    template <class T>
    NODISCARD friend constexpr ptrdiff_t operator-(const T&, const any_convertible_equal&) noexcept {
        return 0;
    }
    template <class T>
    NODISCARD friend constexpr ptrdiff_t operator-(const any_convertible_equal&, const T&) noexcept {
        return 0;
    }
};

template <>
struct std::char_traits<any_convertible_equal> {
    using char_type  = any_convertible_equal;
    using int_type   = bool;
    using pos_type   = streampos;
    using off_type   = streamoff;
    using state_type = mbstate_t;
#if _HAS_CXX20
    using comparison_category = strong_ordering;
#endif // _HAS_CXX20

    static constexpr char_type* copy(char_type* const first1, const char_type*, size_t) noexcept {
        return first1;
    }

    static constexpr char_type* move(char_type* const first1, const char_type*, const size_t) noexcept {
        return first1;
    }

    NODISCARD static constexpr int compare(const char_type*, const char_type*, size_t) noexcept {
        return 0;
    }

    NODISCARD static constexpr size_t length(const char_type*) noexcept {
        return 0;
    }

    NODISCARD static constexpr const char_type* find(const char_type* const first, size_t, const char_type&) noexcept {
        return first;
    }

    static constexpr char_type* assign(char_type* const first, size_t, char_type) noexcept {
        return first;
    }

    static constexpr void assign(char_type&, const char_type&) noexcept {}

    NODISCARD static constexpr bool eq(char_type, char_type) noexcept {
        return true;
    }

    NODISCARD static constexpr bool lt(char_type, char_type) noexcept {
        return false;
    }

    NODISCARD static constexpr char_type to_char_type(int_type) noexcept {
        return {};
    }

    NODISCARD static constexpr int_type to_int_type(char_type) noexcept {
        return true;
    }

    NODISCARD static constexpr bool eq_int_type(const int_type left, const int_type right) noexcept {
        return left == right;
    }

    NODISCARD static constexpr int_type not_eof(const int_type i) noexcept {
        return i;
    }

    NODISCARD static constexpr int_type eof() noexcept {
        return false;
    }
};

template <>
struct std::hash<any_convertible_equal> {
    NODISCARD size_t operator()(any_convertible_equal) const noexcept {
        return 0xDEADBEEFu;
    }
};

template <class ContType>
void assert_nonempty_container_iterator_comparison(ContType&& cont) {
    STATIC_ASSERT(is_same_v<decltype(cont.begin() == cont.end()), bool>);
    STATIC_ASSERT(is_same_v<decltype(cont.begin() != cont.end()), bool>);

    STATIC_ASSERT(is_same_v<decltype(cont.begin() == cont.cend()), bool>);
    STATIC_ASSERT(is_same_v<decltype(cont.begin() != cont.cend()), bool>);

    STATIC_ASSERT(is_same_v<decltype(cont.cbegin() == cont.end()), bool>);
    STATIC_ASSERT(is_same_v<decltype(cont.cbegin() != cont.end()), bool>);

    STATIC_ASSERT(is_same_v<decltype(cont.cbegin() == cont.cend()), bool>);
    STATIC_ASSERT(is_same_v<decltype(cont.cbegin() != cont.cend()), bool>);

#if _HAS_CXX20
    static_assert(ranges::forward_range<ContType>);
    static_assert(ranges::forward_range<const ContType>);
#endif // _HAS_CXX20

    assert(!(cont.begin() == cont.end()));
    assert(cont.begin() != cont.end());

    assert(!(cont.begin() == cont.cend()));
    assert(cont.begin() != cont.cend());

    assert(!(cont.cbegin() == cont.end()));
    assert(cont.cbegin() != cont.end());

    assert(!(cont.cbegin() == cont.cend()));
    assert(cont.cbegin() != cont.cend());

    auto always_true = [](auto&&) { return true; };
    assert(count_if(cont.begin(), cont.end(), always_true) != 0);
    assert(count_if(cont.cbegin(), cont.cend(), always_true) != 0);
#if _HAS_CXX20
    assert(ranges::count_if(cont, always_true) != 0);
    assert(ranges::count_if(as_const(cont), always_true) != 0);
#endif // _HAS_CXX20
}

template <class ContType>
void assert_nonempty_reversible_container_iterator_comparison(ContType&& cont) {
    assert_nonempty_container_iterator_comparison(cont);

#if _HAS_CXX20
    static_assert(ranges::bidirectional_range<ContType>);
    static_assert(ranges::bidirectional_range<const ContType>);
#endif // _HAS_CXX20
}

template <class ContType>
void assert_nonempty_random_access_container_iterator_operations(ContType&& cont) {
    assert_nonempty_reversible_container_iterator_comparison(cont);

    STATIC_ASSERT(is_same_v<decltype(cont.begin() < cont.end()), bool>);
    STATIC_ASSERT(is_same_v<decltype(cont.begin() > cont.end()), bool>);
    STATIC_ASSERT(is_same_v<decltype(cont.begin() <= cont.end()), bool>);
    STATIC_ASSERT(is_same_v<decltype(cont.begin() >= cont.end()), bool>);

    STATIC_ASSERT(is_same_v<decltype(cont.begin() < cont.cend()), bool>);
    STATIC_ASSERT(is_same_v<decltype(cont.begin() > cont.cend()), bool>);
    STATIC_ASSERT(is_same_v<decltype(cont.begin() <= cont.cend()), bool>);
    STATIC_ASSERT(is_same_v<decltype(cont.begin() >= cont.cend()), bool>);

    STATIC_ASSERT(is_same_v<decltype(cont.cbegin() < cont.end()), bool>);
    STATIC_ASSERT(is_same_v<decltype(cont.cbegin() > cont.end()), bool>);
    STATIC_ASSERT(is_same_v<decltype(cont.cbegin() <= cont.end()), bool>);
    STATIC_ASSERT(is_same_v<decltype(cont.cbegin() >= cont.end()), bool>);

    STATIC_ASSERT(is_same_v<decltype(cont.cbegin() < cont.cend()), bool>);
    STATIC_ASSERT(is_same_v<decltype(cont.cbegin() > cont.cend()), bool>);
    STATIC_ASSERT(is_same_v<decltype(cont.cbegin() <= cont.cend()), bool>);
    STATIC_ASSERT(is_same_v<decltype(cont.cbegin() >= cont.cend()), bool>);

    assert(cont.begin() < cont.end());
    assert(!(cont.begin() > cont.end()));
    assert(cont.begin() <= cont.end());
    assert(!(cont.begin() >= cont.end()));

    assert(cont.begin() < cont.cend());
    assert(!(cont.begin() > cont.cend()));
    assert(cont.begin() <= cont.cend());
    assert(!(cont.begin() >= cont.cend()));

    assert(cont.cbegin() < cont.end());
    assert(!(cont.cbegin() > cont.end()));
    assert(cont.cbegin() <= cont.end());
    assert(!(cont.cbegin() >= cont.end()));

    assert(cont.cbegin() < cont.cend());
    assert(!(cont.cbegin() > cont.cend()));
    assert(cont.cbegin() <= cont.cend());
    assert(!(cont.cbegin() >= cont.cend()));

    using diff_t  = typename iterator_traits<decltype(cont.begin())>::difference_type;
    using cdiff_t = typename iterator_traits<decltype(cont.cbegin())>::difference_type;
    STATIC_ASSERT(is_same_v<diff_t, cdiff_t>);

    STATIC_ASSERT(is_same_v<decltype(cont.begin() - cont.end()), diff_t>);
    STATIC_ASSERT(is_same_v<decltype(cont.end() - cont.begin()), diff_t>);

    STATIC_ASSERT(is_same_v<decltype(cont.cbegin() - cont.end()), diff_t>);
    STATIC_ASSERT(is_same_v<decltype(cont.cend() - cont.begin()), diff_t>);

    STATIC_ASSERT(is_same_v<decltype(cont.begin() - cont.cend()), diff_t>);
    STATIC_ASSERT(is_same_v<decltype(cont.end() - cont.cbegin()), diff_t>);

    STATIC_ASSERT(is_same_v<decltype(cont.cbegin() - cont.cend()), diff_t>);
    STATIC_ASSERT(is_same_v<decltype(cont.cend() - cont.cbegin()), diff_t>);

    assert(cont.begin() - cont.end() < 0);
    assert(cont.end() - cont.begin() > 0);

    assert(cont.cbegin() - cont.end() < 0);
    assert(cont.cend() - cont.begin() > 0);

    assert(cont.cbegin() - cont.cend() < 0);
    assert(cont.cend() - cont.cbegin() > 0);

#if _HAS_CXX20
    static_assert(is_same_v<decltype(cont.begin() <=> cont.end()), strong_ordering>);
    static_assert(is_same_v<decltype(cont.begin() <=> cont.cend()), strong_ordering>);
    static_assert(is_same_v<decltype(cont.cbegin() <=> cont.end()), strong_ordering>);
    static_assert(is_same_v<decltype(cont.cbegin() <=> cont.cend()), strong_ordering>);

    assert(cont.begin() <=> cont.end() == strong_ordering::less);
    assert(cont.begin() <=> cont.cend() == strong_ordering::less);
    assert(cont.cbegin() <=> cont.end() == strong_ordering::less);
    assert(cont.cbegin() <=> cont.cend() == strong_ordering::less);

    static_assert(ranges::random_access_range<ContType>);
    static_assert(ranges::random_access_range<const ContType>);
#endif // _HAS_CXX20
}

template <class ContType>
void assert_nonempty_contiguous_container_iterator_operations(ContType&& cont) {
    assert_nonempty_random_access_container_iterator_operations(cont);

#if _HAS_CXX20
    static_assert(ranges::contiguous_range<ContType>);
    static_assert(ranges::contiguous_range<const ContType>);
#endif // _HAS_CXX20
}

template <class ContType>
void sort_nonempty_random_access_container(ContType&& cont) {
    sort(cont.begin(), cont.end());
#if _HAS_CXX20
    ranges::sort(cont);
#endif // _HAS_CXX20
}

int main() {
    assert_nonempty_contiguous_container_iterator_operations(
        basic_string<any_convertible_equal>(1, any_convertible_equal{}));
    assert_nonempty_contiguous_container_iterator_operations(array<any_convertible_equal, 1>{});
    assert_nonempty_random_access_container_iterator_operations(deque<any_convertible_equal>(1));
    assert_nonempty_container_iterator_comparison(forward_list<any_convertible_equal>(1));
    assert_nonempty_reversible_container_iterator_comparison(list<any_convertible_equal>(1));
    assert_nonempty_contiguous_container_iterator_operations(vector<any_convertible_equal>(1));

    sort_nonempty_random_access_container(basic_string<any_convertible_equal>(1, any_convertible_equal{}));
    sort_nonempty_random_access_container(array<any_convertible_equal, 1>{});
    sort_nonempty_random_access_container(deque<any_convertible_equal>(1));
    sort_nonempty_random_access_container(vector<any_convertible_equal>(1));

    assert_nonempty_reversible_container_iterator_comparison(
        map<int, any_convertible_equal>{{0, any_convertible_equal{}}});
    assert_nonempty_reversible_container_iterator_comparison(
        multimap<int, any_convertible_equal>{{0, any_convertible_equal{}}});
    assert_nonempty_reversible_container_iterator_comparison(set<any_convertible_equal>{any_convertible_equal{}});
    assert_nonempty_reversible_container_iterator_comparison(multiset<any_convertible_equal>{any_convertible_equal{}});

    assert_nonempty_container_iterator_comparison(
        unordered_map<int, any_convertible_equal>{{0, any_convertible_equal{}}});
    assert_nonempty_container_iterator_comparison(
        unordered_multimap<int, any_convertible_equal>{{0, any_convertible_equal{}}});
    assert_nonempty_container_iterator_comparison(unordered_set<any_convertible_equal>{any_convertible_equal{}});
    assert_nonempty_container_iterator_comparison(unordered_multiset<any_convertible_equal>{any_convertible_equal{}});

#if _HAS_CXX23
    assert_nonempty_contiguous_container_iterator_operations(array<expected<any, char>, 1>{});
    assert_nonempty_random_access_container_iterator_operations(deque<expected<any, char>>(1));
    assert_nonempty_container_iterator_comparison(forward_list<expected<any, char>>(1));
    assert_nonempty_reversible_container_iterator_comparison(list<expected<any, char>>(1));
    assert_nonempty_contiguous_container_iterator_operations(vector<expected<any, char>>(1));

    assert_nonempty_reversible_container_iterator_comparison(map<int, expected<any, char>>{{0, expected<any, char>{}}});
    assert_nonempty_reversible_container_iterator_comparison(
        multimap<int, expected<any, char>>{{0, expected<any, char>{}}});

    assert_nonempty_container_iterator_comparison(unordered_map<int, expected<any, char>>{{0, expected<any, char>{}}});
    assert_nonempty_container_iterator_comparison(
        unordered_multimap<int, expected<any, char>>{{0, expected<any, char>{}}});
#endif // _HAS_CXX23
}
