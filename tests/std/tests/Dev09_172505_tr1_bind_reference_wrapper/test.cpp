// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>

using namespace std;

struct Cat {
    int m_n;

    void square() {
        m_n *= m_n;
    }
};

int main() {
    Cat c = {4};

    bind(&Cat::square, ref(c))();

    assert(c.m_n == 16);
}
