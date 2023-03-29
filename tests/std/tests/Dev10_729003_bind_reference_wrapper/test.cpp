// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>
#include <string>

using namespace std;

struct A {
    string meow(int i) {
        return "A::meow(" + to_string(i + 9 * 9 * 9) + ")";
    }
};

int main() {
    A a;

    auto f = bind(&A::meow, ref(a), 1000);

    assert(f() == "A::meow(1729)");
}
