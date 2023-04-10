// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <numeric>
#include <string>

using namespace std;

string get_alphabet_repeats(const size_t targetLength) {
    char alphabet[26];
    constexpr auto alphaSize = size(alphabet);
    iota(begin(alphabet), end(alphabet), 'a');
    size_t repeats = (targetLength + alphaSize - 1) / alphaSize;
    string expected;
    expected.reserve(repeats * alphaSize);
    for (size_t i = 0; i < repeats; ++i) {
        expected.append(alphabet, alphaSize);
    }

    expected.resize(targetLength);
    return expected;
}

void run_test(const string& expected, const ios::openmode mode) {
    const auto expectedSize = static_cast<ptrdiff_t>(expected.size());
    {
        ofstream file("testing.txt", ios::trunc | mode);
        file.write(expected.data(), expectedSize);
        assert(file.good());
    }

    {
        const auto readAmount = expectedSize + 100;
        string result(static_cast<size_t>(readAmount), '\xFF');
        ifstream file("testing.txt", mode);
        file.read(&result[0], readAmount);
        assert(file.eof());
        assert(!file.bad());
        assert(file.gcount() == expectedSize);
        const auto divider = result.begin() + expectedSize;
        assert(equal(expected.begin(), expected.end(), result.begin(), divider));
        assert(all_of(divider, result.end(), [](char ch) { return ch == '\xFF'; }));
    }

    remove("testing.txt");
}

int main() {
    string testCase = get_alphabet_repeats(8192);
    run_test(testCase, ios::openmode{});
    run_test(testCase, ios::binary);
    testCase.append(8100, '\n');
    run_test(testCase, ios::openmode{});
    run_test(testCase, ios::binary);
    testCase.assign(8100, '\n');
    run_test(testCase, ios::openmode{});
    run_test(testCase, ios::binary);
}
