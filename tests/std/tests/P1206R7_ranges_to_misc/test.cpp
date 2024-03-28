// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>
#include <optional>
#include <ranges>
#include <utility>
#include <vector>

namespace ranges = std::ranges;

struct secret_key_t {
    explicit secret_key_t() = default;
};
inline constexpr secret_key_t secret_key;

struct reservable {
    using value_type = int;

    std::size_t cap_      = 0;
    std::size_t reserved_ = 0;
    std::size_t size_     = 0;

    constexpr reservable(secret_key_t) {}

    constexpr std::size_t capacity() const {
        return cap_;
    }
    constexpr std::size_t max_size() const {
        return ~std::size_t{};
    }
    constexpr std::size_t size() const {
        return size_;
    }
    constexpr void reserve(const std::size_t n) {
        if (n > cap_) {
            assert(reserved_ == 0);
            reserved_ = n;
            cap_      = n;
        }
    }

    constexpr void push_back(int) {
        if (++size_ > cap_) {
            assert(false);
        }
    }

    int* begin(); // not defined
    int* end(); // not defined
};

constexpr bool test_reservable() {
    int some_ints[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    {
        std::same_as<reservable> auto r = some_ints | ranges::to<reservable>(secret_key);
        assert(r.size_ == ranges::size(some_ints));
        assert(r.cap_ == ranges::size(some_ints));
        assert(r.reserved_ == ranges::size(some_ints));
    }

    return true;
}

constexpr bool test_common_constructible() {
    struct common_constructible {
        using value_type = int;

        constexpr common_constructible(const int* const first, const int* const last, secret_key_t)
            : first_{first}, last_{last}, args_{3} {}

        constexpr common_constructible(const int* const first, const int* const last, secret_key_t, double)
            : first_{first}, last_{last}, args_{4} {}

        const int* begin() const; // not defined
        const int* end() const; // not defined

        const int* first_;
        const int* last_;
        int args_;
    };

    int some_ints[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    {
        std::same_as<common_constructible> auto c0 = ranges::to<common_constructible>(some_ints, secret_key);
        assert(c0.first_ == ranges::begin(some_ints));
        assert(c0.last_ == ranges::end(some_ints));
        assert(c0.args_ == 3);
    }
    {
        std::same_as<common_constructible> auto c1 = some_ints | ranges::to<common_constructible>(secret_key);
        assert(c1.first_ == ranges::begin(some_ints));
        assert(c1.last_ == ranges::end(some_ints));
        assert(c1.args_ == 3);
    }

    // Verify that more than one argument can be passed after the range:
    {
        std::same_as<common_constructible> auto c2 = ranges::to<common_constructible>(some_ints, secret_key, 3.14);
        assert(c2.first_ == ranges::begin(some_ints));
        assert(c2.last_ == ranges::end(some_ints));
        assert(c2.args_ == 4);
    }
    {
        std::same_as<common_constructible> auto c3 = some_ints | ranges::to<common_constructible>(secret_key, 3.14);
        assert(c3.first_ == ranges::begin(some_ints));
        assert(c3.last_ == ranges::end(some_ints));
        assert(c3.args_ == 4);
    }

    return true;
}

constexpr bool test_nested_range() {
    int some_int_ervals[3][2][2] = {{{0, 1}, {2, 3}}, {{4, 5}, {6, 7}}, {{8, 9}, {10, 11}}};

    using C = std::vector<std::vector<std::vector<int>>>;

    {
        std::same_as<C> auto c0 = ranges::to<C>(some_int_ervals);
        assert(c0.size() == 3);
        assert(c0[0].size() == 2);
        assert(c0[0][0].size() == 2);
        assert(c0[0][0][0] == 0);
        assert(c0[0][0][1] == 1);
        assert(c0[0][1][0] == 2);
        assert(c0[0][1][1] == 3);
        assert(c0[1][0][0] == 4);
        assert(c0[1][0][1] == 5);
        assert(c0[1][1][0] == 6);
        assert(c0[1][1][1] == 7);
        assert(c0[2][0][0] == 8);
        assert(c0[2][0][1] == 9);
        assert(c0[2][1][0] == 10);
        assert(c0[2][1][1] == 11);
    }
    {
        std::same_as<C> auto c0 = some_int_ervals | ranges::to<C>();
        assert(c0.size() == 3);
        assert(c0[0].size() == 2);
        assert(c0[0][0].size() == 2);
        assert(c0[0][0][0] == 0);
        assert(c0[0][0][1] == 1);
        assert(c0[0][1][0] == 2);
        assert(c0[0][1][1] == 3);
        assert(c0[1][0][0] == 4);
        assert(c0[1][0][1] == 5);
        assert(c0[1][1][0] == 6);
        assert(c0[1][1][1] == 7);
        assert(c0[2][0][0] == 8);
        assert(c0[2][0][1] == 9);
        assert(c0[2][1][0] == 10);
        assert(c0[2][1][1] == 11);
    }

    return true;
}

struct ContainerLike {
    template <std::input_iterator Iter>
    constexpr ContainerLike(Iter first, Iter last) : dist(static_cast<std::ptrdiff_t>(ranges::distance(first, last))) {}

    constexpr char* begin() {
        return nullptr;
    }
    constexpr char* end() {
        return nullptr;
    }

    std::ptrdiff_t dist;
};

constexpr bool test_lwg3733() {
    auto nul_termination = std::views::take_while([](char ch) { return ch != '\0'; });
    auto c               = nul_termination("1729") | std::views::common | ranges::to<ContainerLike>();
    assert(c.dist == 4);
    return true;
}

constexpr bool test_lwg3785() {
    std::vector<int> vec{42, 1729};

    auto expe1 = ranges::to<std::optional<std::vector<int>>>(vec);
    assert(expe1.has_value());
    assert(*expe1 == vec);

    auto expe2 = vec | ranges::to<std::optional<std::vector<int>>>();
    assert(expe2.has_value());
    assert(*expe2 == vec);

    auto expe3 = ranges::to<std::optional>(vec);
    assert(expe3.has_value());
    assert(*expe3 == vec);

    auto expe4 = vec | ranges::to<std::optional>();
    assert(expe4.has_value());
    assert(*expe4 == vec);

    return true;
}

enum class restriction_kind {
    emplace_back,
    push_back,
    emplace,
    insert,
};

template <restriction_kind K, class T, class A = std::allocator<T>>
class restricted_vector : private std::vector<T, A> {
private:
    using base_type = std::vector<T, A>;

public:
    using typename base_type::allocator_type;
    using typename base_type::const_iterator;
    using typename base_type::const_reverse_iterator;
    using typename base_type::difference_type;
    using typename base_type::iterator;
    using typename base_type::pointer;
    using typename base_type::reference;
    using typename base_type::reverse_iterator;
    using typename base_type::size_type;
    using typename base_type::value_type;

    restricted_vector() = default;
    constexpr explicit restricted_vector(const A& alloc) noexcept : base_type(alloc) {}
    constexpr explicit restricted_vector(const size_type n, const A& alloc = A()) : base_type(n, alloc) {}
    constexpr restricted_vector(const size_type n, const T& val, const A& alloc = A()) : base_type(n, val, alloc) {}
    constexpr restricted_vector(const std::initializer_list<T> il, const A& alloc = A()) : base_type(il, alloc) {}
    constexpr restricted_vector(const restricted_vector& other, const A& alloc) : base_type(other, alloc) {}
    constexpr restricted_vector(restricted_vector&& other, const A& alloc) noexcept(
        std::allocator_traits<A>::is_always_equal::value)
        : base_type(std::move(other), alloc) {}

    using base_type::begin;
    using base_type::cbegin;
    using base_type::cend;
    using base_type::crbegin;
    using base_type::crend;
    using base_type::end;
    using base_type::rbegin;
    using base_type::rend;

    using base_type::back;
    using base_type::front;
    using base_type::operator[];
    using base_type::at;
    using base_type::data;

    template <class... Args>
    constexpr T& emplace_back(Args&&... args)
        requires (K == restriction_kind::emplace_back)
    {
        return base_type::emplace_back(std::forward<Args>(args)...);
    }

    constexpr void push_back(const T& t)
        requires (K == restriction_kind::push_back)
    {
        base_type::emplace_back(t);
    }
    constexpr void push_back(T&& t)
        requires (K == restriction_kind::push_back)
    {
        base_type::emplace_back(std::move(t));
    }

    template <class... Args>
    constexpr iterator emplace(const const_iterator it, Args&&... args)
        requires (K == restriction_kind::emplace)
    {
        return base_type::emplace(it, std::forward<Args>(args)...);
    }

    constexpr iterator insert(const const_iterator it, const T& t)
        requires (K == restriction_kind::insert)
    {
        return base_type::emplace(it, t);
    }
    constexpr iterator insert(const const_iterator it, T&& t)
        requires (K == restriction_kind::insert)
    {
        return base_type::emplace(it, std::move(t));
    }
};

template <restriction_kind K>
constexpr void test_lwg4016_per_kind() {
    using V = restricted_vector<K, int>;
    {
        std::same_as<V> auto vec = std::views::iota(0, 42) | ranges::to<V>();
        assert(ranges::equal(vec, std::views::iota(0, 42)));
    }
    {
        std::same_as<V> auto vec = std::views::iota(0, 42) | ranges::to<V>(std::allocator<int>{});
        assert(ranges::equal(vec, std::views::iota(0, 42)));
    }
    {
        std::same_as<V> auto vec = std::views::empty<int> | ranges::to<V>(std::size_t{42});
        assert(ranges::equal(vec, std::views::repeat(0, 42)));
    }
    {
        std::same_as<V> auto vec = std::views::empty<int> | ranges::to<V>(std::size_t{42}, std::allocator<int>{});
        assert(ranges::equal(vec, std::views::repeat(0, 42)));
    }
    {
        std::same_as<V> auto vec = ranges::to<V>(std::views::iota(0, 42), std::initializer_list<int>{-3, -2, -1});
        assert(ranges::equal(vec, std::views::iota(-3, 42)));
    }
    {
        std::same_as<V> auto vec =
            ranges::to<V>(std::views::iota(0, 42), std::initializer_list<int>{-3, -2, -1}, std::allocator<int>{});
        assert(ranges::equal(vec, std::views::iota(-3, 42)));
    }
    {
        std::same_as<V> auto vec = ranges::to<V>(std::views::iota(0, 42), V{-3, -2, -1}, std::allocator<int>{});
        assert(ranges::equal(vec, std::views::iota(-3, 42)));
    }
    {
        V vec0{1, 2, 3};
        std::same_as<V> auto vec = ranges::to<V>(std::views::iota(4, 7), vec0, std::allocator<int>{});
        assert(ranges::equal(vec, std::views::iota(1, 7)));
    }
}

constexpr bool test_lwg4016() {
    test_lwg4016_per_kind<restriction_kind::emplace_back>();
    test_lwg4016_per_kind<restriction_kind::push_back>();
    test_lwg4016_per_kind<restriction_kind::emplace>();
    test_lwg4016_per_kind<restriction_kind::insert>();
    return true;
}

int main() {
    test_reservable();
    static_assert(test_reservable());

    test_common_constructible();
    static_assert(test_common_constructible());

    test_nested_range();
#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-1588614
    static_assert(test_nested_range());
#endif // ^^^ no workaround ^^^

    test_lwg3733();
    static_assert(test_lwg3733());

    test_lwg3785();
    static_assert(test_lwg3785());

    test_lwg4016();
    static_assert(test_lwg4016());
}
