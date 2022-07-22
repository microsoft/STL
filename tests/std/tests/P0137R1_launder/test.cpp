// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <new>

using namespace std;

struct X {
    const int n;
};

union U {
    X x;
    float f;
};

void test_intro_object() {
    // N4727 6.6.2 [intro.object]/2
    U u = {{1}};
    assert(u.x.n == 1);
    u.f = 5.f;
    assert(u.f == 5.f);
    X* p = new (&u.x) X{2};
    assert(p->n == 2);
    assert(*launder(&u.x.n) == 2);
}

void test_ptr_launder() {
    // N4727 21.6.4 [ptr.launder]/5
    X* p        = new X{3};
    const int a = p->n;
    new (p) X{5};
    const int c = launder(p)->n;
    delete p;
    assert(a == 3);
    assert(c == 5);
}

int main() {
    test_intro_object();
    test_ptr_launder();
}
