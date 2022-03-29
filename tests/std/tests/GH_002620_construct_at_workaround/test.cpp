// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <memory>

using namespace std;

struct S {
    int v;
    S(int v) : v(v) {}
    S(const S&) = delete;
};

union U {
    char c;
    S s;
    U() : c{} {}
    ~U() noexcept {}
};

struct copy_elider {
    operator S() const {
        return S(42);
    }
};

int main() {
    U u;
    // GH-2620: <xutility>: SFINAE constraint on construct_at prevents emplacing immovable objects with copy elision
#ifndef __EDG__ // TRANSITION, DevCom-XXXX
    construct_at(&u.s, copy_elider{});
#endif // TRANSITION, DevCom-XXXX
}
