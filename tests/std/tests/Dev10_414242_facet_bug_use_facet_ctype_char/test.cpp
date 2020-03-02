// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <locale>

using namespace std;

struct Cat {
    Cat() {
        m_p = &use_facet<ctype<char>>(locale());
    }

    const ctype<char>* m_p;
};

Cat g_cat;

int main() {
    // runtime tests are in constructors and destructors of variables with static storage duration
}
