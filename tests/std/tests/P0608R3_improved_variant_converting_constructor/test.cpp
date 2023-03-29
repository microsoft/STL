// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Also tests for P1957R2: Converting from T* to bool should be considered narrowing

#include <bitset>
#include <cassert>
#include <functional>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

using namespace std;

struct double_double {
    double_double(double x) : x_(x) {}

    double x_;
};
struct convertible_bool {
    convertible_bool(bool x) : x_(x) {}

    operator bool() const noexcept {
        return x_;
    }

    bool x_;
};
struct default_struct {};

void assert_P0608R3() {
    // P0608R3 examples
    static_assert(is_constructible_v<variant<string, bool>, const char*>);
    static_assert(is_constructible_v<variant<string, bool>, string>);
    static_assert(is_constructible_v<variant<char, optional<char16_t>>, char16_t>);
    static_assert(is_constructible_v<variant<int, reference_wrapper<double>>, double&>);
    static_assert(is_constructible_v<variant<float, int>, char>);
    static_assert(is_constructible_v<variant<float, long>, int>);
    static_assert(is_constructible_v<variant<float, long long>, int>);
    static_assert(is_constructible_v<variant<float, long, double>, int>);
    static_assert(is_constructible_v<variant<float, vector<int>, long long>, int>);
    static_assert(is_constructible_v<variant<float, int, long long>, char>);
    static_assert(!is_constructible_v<variant<float>, int>);
    static_assert(!is_constructible_v<variant<float, vector<int>>, int>);
    static_assert(!is_constructible_v<variant<float, char>, int>);

    static_assert(is_assignable_v<variant<string, bool>, const char*>);
    static_assert(is_assignable_v<variant<string, bool>, string>);
    static_assert(is_assignable_v<variant<char, optional<char16_t>>, char16_t>);
    static_assert(is_assignable_v<variant<int, reference_wrapper<double>>, double&>);
    static_assert(is_assignable_v<variant<float, int>, char>);
    static_assert(is_assignable_v<variant<float, long>, int>);
    static_assert(is_assignable_v<variant<float, long long>, int>);
    static_assert(is_assignable_v<variant<float, long, double>, int>);
    static_assert(is_assignable_v<variant<float, vector<int>, long long>, int>);
    static_assert(is_assignable_v<variant<float, int, long long>, char>);
    static_assert(!is_assignable_v<variant<float>, int>);
    static_assert(!is_assignable_v<variant<float, vector<int>>, int>);
    static_assert(!is_assignable_v<variant<float, char>, int>);
}

void assert_P1957R2() {
    // P1957R2 examples
    static_assert(is_constructible_v<variant<bool, int>, bool>);
    static_assert(is_constructible_v<variant<bool, int>, bitset<4>::reference>);
    static_assert(is_constructible_v<variant<bool>, bitset<4>::reference>);

    static_assert(is_assignable_v<variant<bool, int>, bool>);
    static_assert(is_assignable_v<variant<bool, int>, bitset<4>::reference>);
    static_assert(is_assignable_v<variant<bool>, bitset<4>::reference>);
}

void assert_more_examples() {
    // More examples
    static_assert(is_constructible_v<variant<double_double>, double>);
    static_assert(is_constructible_v<variant<vector<vector<int>>, optional<int>, int>, int>);
    static_assert(is_constructible_v<variant<vector<vector<int>>, optional<int>>, int>);
    static_assert(is_constructible_v<variant<vector<int>, optional<int>, float>, int>);
    static_assert(is_constructible_v<variant<bool>, convertible_bool>);
    static_assert(is_constructible_v<variant<bool, int>, convertible_bool>);
    static_assert(is_constructible_v<variant<convertible_bool>, bool>);
    static_assert(is_constructible_v<variant<float, bool, convertible_bool>, convertible_bool>);
    static_assert(is_constructible_v<variant<float, bool, convertible_bool>, bool>);
    static_assert(is_constructible_v<variant<char, int>, bool>);
    static_assert(is_constructible_v<variant<double_double>, int>);
    static_assert(!is_constructible_v<variant<float>, unsigned int>);
    static_assert(!is_constructible_v<variant<char, default_struct>, int>);
    static_assert(!is_constructible_v<variant<float, long, long long>, int>);

    static_assert(is_assignable_v<variant<double_double>, double>);
    static_assert(is_assignable_v<variant<vector<vector<int>>, optional<int>, int>, int>);
    static_assert(is_assignable_v<variant<vector<vector<int>>, optional<int>>, int>);
    static_assert(is_assignable_v<variant<vector<int>, optional<int>, float>, int>);
    static_assert(is_assignable_v<variant<bool>, convertible_bool>);
    static_assert(is_assignable_v<variant<bool, int>, convertible_bool>);
    static_assert(is_assignable_v<variant<convertible_bool>, bool>);
    static_assert(is_assignable_v<variant<float, bool, convertible_bool>, convertible_bool>);
    static_assert(is_assignable_v<variant<float, bool, convertible_bool>, bool>);
    static_assert(is_assignable_v<variant<char, int>, bool>);
    static_assert(is_assignable_v<variant<double_double>, int>);
    static_assert(!is_assignable_v<variant<float>, unsigned int>);
    static_assert(!is_assignable_v<variant<char, default_struct>, int>);
    static_assert(!is_assignable_v<variant<float, long, long long>, int>);
}

void test_variant_constructor_P0608R3() {
    // P0608R3 runtime checks
    variant<string, bool> a = "abc"; // string
    assert(a.index() == 0);
    assert(get<0>(a) == "abc");

    variant<char, optional<char16_t>> b = u'\u2043'; // optional<char16_t>
    assert(b.index() == 1);
    assert(get<optional<char16_t>>(b) == u'\u2043');

    double c_data                             = 3.14;
    variant<int, reference_wrapper<double>> c = c_data; // reference_wrapper<double>
    assert(c.index() == 1);
    assert(get<1>(c) == c_data);

    variant<float, int> d;
    assert(d.index() == 0);
    d = 0; // int
    assert(d.index() == 1);

    variant<float, long> e;
    assert(e.index() == 0);
    e = 0; // long
    assert(e.index() == 1);

    variant<float, int> f = 'a'; // int
    assert(f.index() == 1);
    assert(get<int>(f) == 97);

    variant<float, long> g = 0; // long
    assert(g.index() == 1);

    variant<float, long, double> h = 0; // long
    assert(h.index() == 1);

    variant<float, vector<int>, long long> i = 0; // long long
    assert(i.index() == 2);

    variant<float, int, long long> j = 'a'; // int
    assert(j.index() == 1);
    assert(get<int>(j) == 97);
}

void test_variant_constructor_P1957R2() {
    bitset<4> a_bitset("0101");
    bool a_data          = a_bitset[2];
    variant<bool, int> a = a_data; // bool
    assert(a.index() == 0);
    assert(get<0>(a));

    bitset<4> b_bitset("0101");
    variant<bool, int> b  = b_bitset[2]; // bool
    variant<bool, int> b2 = b_bitset[1]; // bool
    assert(b.index() == 0);
    assert(get<0>(b));
    assert(b2.index() == 0);
    assert(!get<0>(b2));
}

void test_variant_constructor_more_examples() {
    variant<char, int, float, bool, vector<bool>> a = true; // bool
    assert(a.index() == 3);

    variant<bool, int> b = convertible_bool{true}; // bool
    assert(b.index() == 0);
    assert(get<0>(b));

    variant<char, int, bool> c = false; // bool
    assert(c.index() == 2);

    variant<float, bool, convertible_bool> d = convertible_bool{true}; // convertible_bool
    assert(d.index() == 2);

    variant<float, bool, convertible_bool> e = bool{}; // bool
    assert(e.index() == 1);
    assert(!get<1>(e));

    variant<float, bool> f = convertible_bool{false}; // bool
    assert(f.index() == 1);
    assert(!get<1>(f));

    variant<bool, int> g = true_type{}; // bool
    assert(g.index() == 0);
    assert(get<0>(g));
}

void test_assignment_operator() {
    variant<string, bool, int> a; // string
    assert(a.index() == 0);
    assert(get<string>(a) == "");
    a = 3; // int
    assert(a.index() == 2);
    assert(get<int>(a) == 3);
    a = true; // bool
    assert(a.index() == 1);
    assert(get<bool>(a) == true);

    bool b_data                         = true;
    variant<bool, int, double_double> b = b_data; // bool
    assert(b.index() == 0);
    assert(get<0>(b) == b_data);
    b = 12; // int
    assert(b.index() == 1);
    assert(get<1>(b) == 12);
    b = 12.5; // double_double
    assert(b.index() == 2);
    assert(get<2>(b).x_ == 12.5);

    variant<void*, bool, double_double> c;
    assert(c.index() == 0);
    c = false; // bool
    assert(c.index() == 1);
    assert(get<1>(c) == false);
    c = 5.12; // double_double
    assert(c.index() == 2);
    assert(get<2>(c).x_ == 5.12);
    double_double c_data{1.2};
    c = static_cast<void*>(&c_data); // void*
    assert(c.index() == 0);
    assert(static_cast<double_double*>(get<0>(c))->x_ == 1.2);
}

int main() {
    assert_P0608R3();
    assert_P1957R2();
    assert_more_examples();
    test_variant_constructor_P0608R3();
    test_variant_constructor_P1957R2();
    test_variant_constructor_more_examples();
    test_assignment_operator();
}
