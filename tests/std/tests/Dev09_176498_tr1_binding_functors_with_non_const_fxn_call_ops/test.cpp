// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>

using namespace std;

struct functor {
    typedef int result_type;

    int operator()(int x) { // NON-const
        return x * 2;
    }
};

struct nullary {
    typedef int result_type;

    int operator()() { // NON-const
        return 1729;
    }
};

int main() {
    functor f; // NON-const

    int r1 = bind(f, 123)();

    assert(r1 == 246);

    nullary g; // NON-const

    int r2 = bind(g)();

    assert(r2 == 1729);
}
