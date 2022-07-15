// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <class T, class E, size_t N>
constexpr void assert_equal(T&& actual, const array<E, N>& expected) {
    STATIC_ASSERT(is_same_v<T, array<E, N>>);
    assert(actual == expected);
}

struct A {
    [[nodiscard]] friend constexpr bool operator==(const A& a, const A& b) {
        return a.an_int == b.an_int && a.a_double == b.a_double;
    }
    int an_int;
    double a_double;
};

constexpr bool assert_constexpr() {
    int a[3]             = {};
    const int const_a[3] = {};
    assert_equal(to_array("meow"), array<char, 5>{'m', 'e', 'o', 'w', '\0'});
    assert_equal(to_array({1, 2}), array<int, 2>{1, 2});

    assert_equal(to_array<long>({1, 2}), array<long, 2>{1, 2});
    assert_equal(to_array(a), array<int, 3>{0, 0, 0});
    assert_equal(to_array(const_a), array<int, 3>{0, 0, 0});
    assert_equal(to_array(move(a)), array<int, 3>{0, 0, 0});
    assert_equal(to_array(move(const_a)), array<int, 3>{0, 0, 0});
    assert_equal(to_array<A>({{3, 0.1}}), array<A, 1>{{{3, 0.1}}});
    return true;
}

void assert_not_constexpr() {
    unique_ptr<int> c_array_of_unique_ptrs[3] = {make_unique<int>(1), make_unique<int>(2), make_unique<int>(3)};
    int* c_array_of_int_ptrs[3]               = {};
    transform(begin(c_array_of_unique_ptrs), end(c_array_of_unique_ptrs), begin(c_array_of_int_ptrs),
        [](const auto& elm) { return elm.get(); });
    array<unique_ptr<int>, 3> array_of_unique_ptrs = to_array(move(c_array_of_unique_ptrs));
    assert(equal(begin(array_of_unique_ptrs), end(array_of_unique_ptrs), begin(c_array_of_int_ptrs),
        end(c_array_of_int_ptrs), [](const unique_ptr<int>& a, const int* b) { return a.get() == b; }));
    assert_equal(to_array({"cats"s, "go"s, "meow"s}), array<string, 3>{"cats", "go", "meow"});
}

int main() {
    assert(assert_constexpr());
    static_assert(assert_constexpr());
    assert_not_constexpr();
}
