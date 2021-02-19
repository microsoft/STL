// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Covers:
// * spaceship for containers

#include <array>
#include <cassert>
#include <chrono>
#include <compare>
#include <concepts>
#include <deque>
#include <filesystem>
#include <forward_list>
#include <functional>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <queue>
#include <ranges>
#include <ratio>
#include <regex>
#include <set>
#include <stack>
#include <string>
#include <system_error>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

template <class T>
using SpaceshipType = decltype(std::declval<T>() <=> std::declval<T>());

template <class T, class U>
concept HasSpaceshipWith = requires {
    std::declval<T>() <=> std::declval<U>();
};

using PartiallyOrdered = double;

struct WeaklyOrdered {
    [[nodiscard]] constexpr bool operator==(const WeaklyOrdered&) const {
        return true;
    }

    [[nodiscard]] constexpr std::weak_ordering operator<=>(const WeaklyOrdered&) const {
        return std::weak_ordering::equivalent;
    }
};

using StronglyOrdered = int;

// Activates synth-three-way in N4861 16.4.2.1 [expos.only.func]/2.
struct SynthOrdered {
    int val;

    constexpr SynthOrdered(const int x) : val{x} {}

    [[nodiscard]] constexpr bool operator==(const SynthOrdered& other) const {
        return val == other.val;
    }

    [[nodiscard]] constexpr bool operator<(const SynthOrdered& other) const {
        return val < other.val;
    }
};

struct OrderedChar {
    OrderedChar() = default;
    OrderedChar(const char other) : c(other) {}

    OrderedChar& operator=(const char& other) {
        c = other;
        return *this;
    }

    auto operator<=>(const OrderedChar&) const = default;

    operator char() const {
        return c;
    }

    char c;
};

struct WeaklyOrderedChar : OrderedChar {};
struct WeaklyOrderedByOmissionChar : OrderedChar {};
struct PartiallyOrderedChar : OrderedChar {};

namespace std {
    template <>
    struct char_traits<OrderedChar> : char_traits<char> {
        using char_type = OrderedChar;

        static int compare(const char_type* first1, const char_type* first2, size_t count) {
            for (; 0 < count; --count, ++first1, ++first2) {
                if (*first1 != *first2) {
                    return *first1 < *first2 ? -1 : +1;
                }
            }

            return 0;
        }

        static bool eq(const char_type l, const char_type r) {
            return l.c == r.c;
        }
    };

    template <>
    struct char_traits<WeaklyOrderedChar> : char_traits<OrderedChar> {
        using char_type           = WeaklyOrderedChar;
        using comparison_category = weak_ordering;
    };

    template <>
    struct char_traits<WeaklyOrderedByOmissionChar> : char_traits<OrderedChar> {
        using char_type = WeaklyOrderedByOmissionChar;

    private:
        using comparison_category = strong_ordering;
    };

    template <>
    struct char_traits<PartiallyOrderedChar> : char_traits<OrderedChar> {
        using char_type           = PartiallyOrderedChar;
        using comparison_category = partial_ordering;
    };
} // namespace std

struct dummy_diagnostic : std::error_category {
    const char* name() const noexcept override {
        return "dummy";
    }
    std::string message(int) const override {
        return "";
    }
};

template <class ReturnType, class SmallType, class EqualType, class LargeType>
void spaceship_test(const SmallType& smaller, const EqualType& smaller_equal, const LargeType& larger) {
    assert(smaller == smaller_equal);
    assert(smaller_equal == smaller);
    assert(smaller != larger);
    assert(larger != smaller);
    assert(smaller < larger);
    assert(!(larger < smaller));
    assert(larger > smaller);
    assert(!(smaller > larger));
    assert(smaller <= larger);
    assert(!(larger <= smaller));
    assert(larger >= smaller);
    assert(!(smaller >= larger));
    assert((smaller <=> larger) < 0);
    assert((larger <=> smaller) > 0);
    assert((smaller <=> smaller_equal) == 0);

    static_assert(std::is_same_v<decltype(smaller <=> larger), ReturnType>);
}

template <class T>
inline constexpr bool is_pair = false;
template <class A, class B>
inline constexpr bool is_pair<std::pair<A, B>> = true; // TRANSITION, std::pair spaceship not yet implemented

template <class Container>
void ordered_containers_test(const Container& smaller, const Container& smaller_equal, const Container& larger) {
    using Elem = typename Container::value_type;
    if constexpr (is_pair<Elem> // TRANSITION, std::pair spaceship not yet implemented
                  || std::is_same_v<Elem, SynthOrdered>) {
        spaceship_test<std::weak_ordering>(smaller, smaller_equal, larger);
    } else {
        spaceship_test<std::strong_ordering>(smaller, smaller_equal, larger);
    }
}

template <class Container>
void unordered_containers_test(
    const Container& something, const Container& something_equal, const Container& different) {
    assert(something == something_equal);
    assert(something != different);
}

template <class ErrorType>
void diagnostics_test() {
    dummy_diagnostic c_mem[2];
    {
        ErrorType e_smaller(0, c_mem[0]);
        ErrorType e_equal(0, c_mem[0]);
        ErrorType e_larger(1, c_mem[1]);

        spaceship_test<std::strong_ordering>(e_smaller, e_equal, e_larger);
    }
    {
        ErrorType e_smaller(0, c_mem[0]);
        ErrorType e_larger(0, c_mem[1]);

        assert(e_smaller < e_larger);
        assert(!(e_larger < e_smaller));
        assert((e_smaller <=> e_larger) < 0);
        assert((e_larger <=> e_smaller) > 0);
    }
    {
        ErrorType e_smaller(0, c_mem[0]);
        ErrorType e_larger(1, c_mem[0]);

        assert(e_smaller < e_larger);
        assert(!(e_larger < e_smaller));
        assert((e_smaller <=> e_larger) < 0);
        assert((e_larger <=> e_smaller) > 0);
    }
}

void ordering_test_cases() {
    { // chrono::duration
        using std::chrono::hours;
        using std::chrono::minutes;
        using std::chrono::seconds;

        spaceship_test<std::strong_ordering>(seconds{1}, seconds{1}, seconds{2});
        spaceship_test<std::strong_ordering>(seconds{3600}, hours{1}, minutes{61});

        using double_seconds     = std::chrono::duration<double>;
        using float_milliseconds = std::chrono::duration<float, std::milli>;
        using ntsc_fields        = std::chrono::duration<long long, std::ratio<1001, 60000>>;

        spaceship_test<std::partial_ordering>(double_seconds{1}, float_milliseconds{1000}, ntsc_fields{60});

        constexpr double_seconds nan_s{std::numeric_limits<double>::quiet_NaN()};
#ifdef __clang__ // TRANSITION, DevCom-445462
        static_assert(nan_s <=> nan_s == std::partial_ordering::unordered);
#endif // defined(__clang__)
        assert(nan_s <=> nan_s == std::partial_ordering::unordered);
    }
    { // chrono::time_point
        using std::chrono::milliseconds;
        using double_seconds = std::chrono::duration<double>;
        using sys_tp         = std::chrono::system_clock::time_point;
        using sys_ms         = std::chrono::time_point<std::chrono::system_clock, milliseconds>;
        using sys_double_s   = std::chrono::time_point<std::chrono::system_clock, double_seconds>;

        spaceship_test<std::strong_ordering>(sys_tp{}, sys_ms{}, sys_ms{milliseconds{1}});
        spaceship_test<std::partial_ordering>(sys_tp{}, sys_double_s{}, sys_double_s{double_seconds{1}});

        constexpr sys_double_s nan_tp{double_seconds{std::numeric_limits<double>::quiet_NaN()}};
#ifdef __clang__ // TRANSITION, DevCom-445462
        static_assert(nan_tp <=> nan_tp == std::partial_ordering::unordered);
#endif // defined(__clang__)
        assert(nan_tp <=> nan_tp == std::partial_ordering::unordered);

        using steady_tp = std::chrono::steady_clock::time_point;
        static_assert(!HasSpaceshipWith<sys_tp, steady_tp>);
    }
}

int main() {
    ordering_test_cases();
}
