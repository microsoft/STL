// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>
#include <random>
#include <vector>

using namespace std;

vector<bool> vb_from_str(const char* str) {
    vector<bool> vb;

    for (; *str != '\0'; ++str) {
        assert(*str == '0' || *str == '1');

        vb.push_back(*str == '1');
    }

    return vb;
}

enum class Ordering : int { Less = -1, Equal = 0, Greater = 1 };

constexpr Ordering Lt = Ordering::Less;
constexpr Ordering Eq = Ordering::Equal;
constexpr Ordering Gt = Ordering::Greater;

void test_comparison(const char* const left_str, const char* const right_str, const Ordering order) {
    const auto left  = vb_from_str(left_str);
    const auto right = vb_from_str(right_str);

#ifdef __cpp_lib_concepts
    assert((left <=> right) == (static_cast<int>(order) <=> 0));
    assert((right <=> left) == (0 <=> static_cast<int>(order)));
#endif // __cpp_lib_concepts

    switch (order) {
    case Lt:
        assert(!(left == right));
        assert(!(right == left));
        assert(left < right);
        assert(!(right < left));
        break;
    case Eq:
        assert(left == right);
        assert(right == left);
        assert(!(left < right));
        assert(!(right < left));
        break;
    case Gt:
        assert(!(left == right));
        assert(!(right == left));
        assert(!(left < right));
        assert(right < left);
        break;
    default:
        assert(false);
        break;
    }
}

int main() {

    {
        mt19937 eng(1729);

        uniform_int_distribution<int> dist(0, 1);

        const size_t N = 137;

        vector<bool> x(N);
        vector<bool> y(N);

        for (size_t i = 0; i < N; ++i) {
            const bool b = dist(eng) != 0;

            x[i] = b;
            y[i] = b;
        }

        assert(x == y);

        y.push_back(0);

        assert(x != y);

        y.pop_back();

        assert(x == y);

        y.push_back(1);

        assert(x != y);

        y.pop_back();

        assert(x == y);

        x.back().flip();

        assert(x != y);

        y.back().flip();

        assert(x == y);
    }

    {
        // Also test DevDiv-850453 "<vector>: Missing emplace methods in std::vector<bool> container".

        vector<bool> v(47, allocator<bool>());

        v.emplace_back(make_shared<int>(123));
        v.emplace_back(shared_ptr<int>());

        v.emplace(v.cbegin(), make_shared<double>(3.14));
        v.emplace(v.cbegin(), make_unique<int>(456));
        v.emplace(v.cbegin(), shared_ptr<double>());
        v.emplace(v.cbegin(), unique_ptr<int>());
        v.emplace(v.cbegin(), unique_ptr<int[]>());


        vector<bool> correct;

        correct.insert(correct.cend(), 3, false);
        correct.insert(correct.cend(), 2, true);
        correct.insert(correct.cend(), 47, false);
        correct.insert(correct.cend(), 1, true);
        correct.insert(correct.cend(), 1, false);

        assert(v == correct);
    }

    // Also test GH-1046 optimizing vector<bool> spaceship and less-than comparisons.
    test_comparison("", "", Eq); // both empty
    test_comparison("", "00", Lt); // empty vs. partial word
    test_comparison("", "01", Lt);
    test_comparison("", "00000000000000000000000000000000", Lt); // empty vs. full word
    test_comparison("", "01000000000000000000000000000000", Lt);
    test_comparison("", "0000000000000000000000000000000000", Lt); // empty vs. full and partial words
    test_comparison("", "0100000000000000000000000000000001", Lt);

    test_comparison("010111010", "010111010", Eq);
    test_comparison("010111010", "011110010", Lt); // test that bits are compared in the correct direction

    // same test, after an initial matching word
    test_comparison("00001111000011110000111100001111010111010", "00001111000011110000111100001111010111010", Eq);
    test_comparison("00001111000011110000111100001111010111010", "00001111000011110000111100001111011110010", Lt);

    test_comparison("00001111", "00001111", Eq);
    test_comparison("00001111", "000011110", Lt); // matching prefixes, test size comparison
    test_comparison("00001111", "000011111", Lt);
    test_comparison("00001111", "00001111000000000000000000000000", Lt); // full word
    test_comparison("00001111", "00001111000000000000000000000001", Lt);
    test_comparison("00001111", "0000111100000000000000000000000000", Lt); // full and partial words
    test_comparison("00001111", "0000111100000000000000000000000001", Lt);

    test_comparison("10", "01111", Gt); // shorter but greater
    test_comparison("10", "01111111111111111111111111111111", Gt); // full word
    test_comparison("10", "0111111111111111111111111111111111", Gt); // full and partial words
}
