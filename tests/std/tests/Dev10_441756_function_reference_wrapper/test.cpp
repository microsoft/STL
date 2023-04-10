// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>
#include <random>

using namespace std;

int main() {
    mt19937 prng;

    function<mt19937::result_type()> f;

    f = ref(prng);

    const mt19937::result_type x = prng();
    const mt19937::result_type y = f();

    assert(x != y);
}
