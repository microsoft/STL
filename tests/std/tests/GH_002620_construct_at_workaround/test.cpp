// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <memory>

using namespace std;

struct S {
    int v;
    explicit S(int v_) : v(v_) {}
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
        return S{42};
    }
};

int main() {
    U u;
    // GH-2620: <xutility>: SFINAE constraint on construct_at prevents emplacing immovable objects with copy elision
#ifndef __EDG__ // TRANSITION, DevCom-10000388
    construct_at(&u.s, copy_elider{});
#endif // TRANSITION, DevCom-10000388
}
