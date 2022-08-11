// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <ranges>
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
            : first_{first}, last_{last} {}

        const int* begin() const; // not defined
        const int* end() const; // not defined

        const int* first_;
        const int* last_;
    };

    int some_ints[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    {
        std::same_as<common_constructible> auto c0 = ranges::to<common_constructible>(some_ints, secret_key);
        assert(c0.first_ == ranges::begin(some_ints));
        assert(c0.last_ == ranges::end(some_ints));
    }
    {
        std::same_as<common_constructible> auto c1 = some_ints | ranges::to<common_constructible>(secret_key);
        assert(c1.first_ == ranges::begin(some_ints));
        assert(c1.last_ == ranges::end(some_ints));
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

int main() {
    test_reservable();
    static_assert(test_reservable());

    test_common_constructible();
    static_assert(test_common_constructible());

    test_nested_range();
#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-1588614
    static_assert(test_nested_range());
#endif // defined(__clang__) || defined(__EDG__)
}
