// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>

using namespace std;

int f() {
    return 456;
}

int g() {
    return 789;
}

int main() {
    {
        int a = 100;
        int b = 200;

        int* const pa = &a;
        int* const pb = &b;

        reference_wrapper<int* const> ra(pa);
        reference_wrapper<int* const> rb(pb);

        assert(a == 100 && b == 200);

        *ra += 1;

        assert(a == 101 && b == 200);

        ra = rb;

        *ra += 2;

        assert(a == 101 && b == 202);
    }

    {
        int (*const pf)() = f;
        int (*const pg)() = g;

        reference_wrapper<int (*const)()> rf(pf);
        reference_wrapper<int (*const)()> rg(pg);

        assert(rf() == 456);

        rf = rg;

        assert(rf() == 789);
    }
}
