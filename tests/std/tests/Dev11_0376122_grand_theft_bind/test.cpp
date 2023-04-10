// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// N3485 20.8.9.1.3 [func.bind.bind]/10:
// The values of the bound arguments v1, v2, ..., vN and their corresponding types
// V1, V2, ..., VN depend on the types TiD derived from the call to bind and the
// cv-qualifiers cv of the call wrapper g as follows:
// [...]
// - otherwise, the value is tid and its type Vi is TiD cv &.

#include <cassert>
#include <functional>
#include <memory>

using namespace std;

void test1(const shared_ptr<int>& sp) {
    assert(sp && *sp == 1729);
}

void test2(shared_ptr<int> sp) {
    assert(sp && *sp == 19937);
}

int main() {
    auto sp1 = make_shared<int>(1729);
    test1(sp1);
    test1(sp1);
    auto b1 = bind(&test1, sp1);
    sp1.reset();
    b1();
    b1();

    auto sp2 = make_shared<int>(19937);
    test2(sp2);
    test2(sp2);
    auto b2 = bind(&test2, sp2);
    sp2.reset();
    b2();
    b2();
}
