// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <string>
#include <utility>

#include <constexpr_char_traits.hpp>

using namespace std;

using tstring = basic_string<char, constexpr_char_traits>; // disable memcpy optimizations in basic_string

void test_case_move_from_small() {
    tstring str("short");
    tstring moved_to(move(str));
    assert(str[0] == '\0'); // not technically standards-required, but required for
                            // reasonable behavior on our implementation (repeated below)
    assert(moved_to[0] == 's');
}

void test_case_move_from_large() {
    tstring str("long string long enough to disable the small string optimization");
    // Index:    0    5   ^
    tstring moved_to(move(str));
    assert(str[0] == '\0');
    assert(moved_to[9] == 'n');
}

void test_case_move_assign_from_small_to_small() {
    tstring str("short");
    tstring moved_to("old");
    moved_to = move(str);
    assert(str[0] == '\0');
    assert(moved_to[1] == 'h');
}

void test_case_move_assign_from_small_to_large() {
    tstring str("short");
    tstring moved_to("the quick brown fox jumps over the lazy dog");
    moved_to = move(str);
    assert(str[0] == '\0');
    assert(moved_to[1] == 'h');
}

void test_case_move_assign_from_large_to_small() {
    tstring str("steal the pointer to my guts!");
    tstring moved_to("short");
    moved_to = move(str);
    assert(str[0] == '\0');
    assert(moved_to[10] == 'p');
}

void test_case_move_assign_from_large_to_large() {
    tstring str("Sphinx of black quartz, judge my vow.");
    tstring moved_to("The five boxing wizards jump quickly.");
    moved_to = move(str);
    assert(str[0] == '\0');
    assert(moved_to[4] == 'n');
}

void test_case_swap_small_small() {
    tstring smallA("cute");
    tstring smallB("kittens");
    swap(smallA, smallB);
    assert(smallB[0] == 'c');
    assert(smallA[0] == 'k');
}

void test_case_swap_small_large() {
    tstring smallA("cute");
    tstring largeB("kittens that are going to be put into a tiny cute container");
    swap(smallA, largeB);
    assert(largeB[3] == 'e');
    assert(smallA[3] == 't');
}

void test_case_swap_large_small() {
    tstring largeA("cute containers are often large enough for many");
    tstring smallB("kittens");
    swap(largeA, smallB);
    assert(smallB[3] == 'e');
    assert(largeA[3] == 't');
}

void test_case_swap_large_large() {
    tstring largeA("cute fluffy kittens are often even cuter in a large group");
    tstring largeB("hungry EVIL zombies need to be kept away from the cute kittens");
    swap(largeA, largeB);
    assert(largeB[5] == 'f');
    assert(largeA[5] == 'y');
}

int main() {
    test_case_move_from_small();
    test_case_move_from_large();
    test_case_move_assign_from_small_to_small();
    test_case_move_assign_from_small_to_large();
    test_case_move_assign_from_large_to_small();
    test_case_move_assign_from_large_to_large();
    test_case_swap_small_small();
    test_case_swap_small_large();
    test_case_swap_large_small();
    test_case_swap_large_large();
}
