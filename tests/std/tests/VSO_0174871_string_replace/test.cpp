// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>

using namespace std;

const string g_input("0123456789");

void test_replace(
    const size_t removedOffset, const size_t removedSize, const string& newContent, const char* const expected) {
    string target(g_input);
    target.replace(removedOffset, removedSize, newContent);
    if (target != expected) {
        printf(R"(test_replace(%zu, %zu, "%s", "%s"):         bad answer: "%s")"
               "\n",
            removedOffset, removedSize, newContent.c_str(), expected, target.c_str());
        abort();
    }

    string cTarget(g_input);
    cTarget.replace(removedOffset, removedSize, newContent.c_str());
    if (cTarget != expected) {
        printf(R"(test_replace(%zu, %zu, "%s", "%s"): bad c_str() answer: "%s")"
               "\n",
            removedOffset, removedSize, newContent.c_str(), expected, cTarget.c_str());
        abort();
    }
}

void test_self_replace(const size_t removedOffset, const size_t removedSize, const size_t addedOffset,
    const size_t addedSize, const char* const expected) {
    string target(g_input);
    target.replace(removedOffset, removedSize, target, addedOffset, addedSize);
    if (target != expected) {
        printf(R"(test_self_replace(%zu, %zu, %zu, %zu, "%s"):         bad answer: "%s")"
               "\n",
            removedOffset, removedSize, addedOffset, addedSize, expected, target.c_str());
        abort();
    }

    string cTarget(g_input);
    cTarget.replace(removedOffset, removedSize, cTarget.c_str() + addedOffset, addedSize);
    if (cTarget != expected) {
        printf(R"(test_self_replace(%zu, %zu, %zu, %zu, "%s"): bad c_str() answer: "%s")"
               "\n",
            removedOffset, removedSize, addedOffset, addedSize, expected, cTarget.c_str());
        abort();
    }
}

template <typename StringT>
void test_index_boundary_cases_impl(StringT& exampleString) {
    // operator[] allows accessing the terminating null character
    // (see N4567 21.4.5 [string.access]/2)
    // so IDL checks must not blow up accessing it:
    assert(exampleString[exampleString.size()] == '\0');

    try {
        assert(exampleString.at(exampleString.size() - 1) == 'e');
    } catch (const out_of_range&) {
        puts("Accessing string::at(string::size() - 1) should not throw out_of_range");
        abort();
    }

    try {
        (void) exampleString.at(exampleString.size());
        puts("Accessing string::at(string::size()) should throw out_of_range");
        abort();
    } catch (const out_of_range&) {
        // purposely do nothing on out_of_range
    }
}

void test_index_boundary_cases() {
    // boundary cases for at() and operator[]
    string exampleString("example");
    test_index_boundary_cases_impl(exampleString);
    test_index_boundary_cases_impl(static_cast<const string&>(exampleString));
}

void test_shrink_to_fit() {
    const char shortStr[] = "hello";
    string example{shortStr};
    example.shrink_to_fit();
    assert(example == shortStr);
    example.resize(1789);
    assert(strcmp(shortStr, example.c_str()) == 0);
    example.resize(sizeof(shortStr) - 1);
    example.shrink_to_fit(); // reenters small mode
    assert(example == shortStr);

    const char longerStr[] = "the quick brown fox jumps over the lazy dog";
    example.resize(32768);
    example = longerStr;
    assert(example == longerStr);
    example.shrink_to_fit(); // reallocates smaller buffer
    assert(example == longerStr);
}

int main() {
    // Plain replacements with shrinking / same size / growing
    test_replace(3, 3, "ab", "012ab6789");
    test_replace(3, 2, "ab", "012ab56789");
    test_replace(3, 1, "ab", "012ab456789");
    // Empty string removed
    test_replace(3, 0, "ab", "012ab3456789");
    // Empty string inserted
    test_replace(3, 3, "", "0126789");
    // Complete no-op
    test_replace(3, 0, "", "0123456789");
    // Boundary edge cases
    test_replace(0, 0, "a", "a0123456789");
    test_replace(0, 1, "a", "a123456789");
    test_replace(9, 1, "a", "012345678a");
    test_replace(10, 0, "a", "0123456789a");

    // Self replacement cases are intended to align with the special cases in
    // basic_string::replace(size_type _Off, size_type _N0,
    //                       const _Myt& _Right, size_type _Roff, size_type _Count = npos)
    test_self_replace(3, 1, 4, 1, "0124456789");
    test_self_replace(3, 2, 4, 2, "0124556789"); // repeated
    test_self_replace(3, 2, 2, 2, "0122356789"); // repeated
    test_self_replace(3, 3, 3, 3, "0123456789"); // repeated
    test_self_replace(0, 0, 0, 0, "0123456789"); // repeated
    test_self_replace(0, 10, 0, 10, "0123456789"); // repeated
    test_self_replace(10, 0, 10, 0, "0123456789"); // repeated
    // "no overlap" case omitted
    test_self_replace(3, 2, 5, 1, "012556789");
    test_self_replace(3, 1, 1, 2, "01212456789");
    test_self_replace(3, 1, 8, 2, "01289456789");
    test_self_replace(3, 2, 3, 3, "01234556789");
    test_self_replace(3, 2, 4, 3, "01245656789"); // repeated

    // And one more case when shrinking where the substring is in the removed part
    test_self_replace(3, 5, 4, 2, "0124589");

    test_index_boundary_cases();
    test_shrink_to_fit();
}
