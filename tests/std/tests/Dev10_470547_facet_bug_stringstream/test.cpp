// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <sstream>

using namespace std;

struct Cat {
    ~Cat() {
        stringstream s1;
    }
};

Cat g_cat;

int main() {
    stringstream s2;
}
