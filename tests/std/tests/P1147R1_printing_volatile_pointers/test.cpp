// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <sstream>
#include <string>

using namespace std;

template <typename T>
string getTextValue(T* ptr) {
    ostringstream out;
    out << ptr;
    return out.str();
}

void test(int value) {
    int* p0          = &value;
    volatile int* p1 = p0;

    const string expected = getTextValue(p0);
    const string actual   = getTextValue(p1);

    assert(expected == actual);
}

int main() {
    test(42);
}
