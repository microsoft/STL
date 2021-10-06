// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <sstream>

using namespace std;

std::string getExpected(int* ptr) {
    ostringstream out;
    out << ptr;
    return out.str();
}

std::string getActual(volatile int* ptr) {
    ostringstream out;
    out << ptr;
    return out.str();
}

void test(size_t value) {
    int* p0          = reinterpret_cast<int*>(value);
    volatile int* p1 = reinterpret_cast<volatile int*>(p0);

    const string expected = getExpected(p0);
    const string actual   = getActual(p1);

    assert(expected == actual);
}

int main() {
    test(0xdeadbeef);
}
