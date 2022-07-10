// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <string>
#include <tuple>

using namespace std;

int main() {
    tuple<const char*, int> t = make_tuple("abc", 10);

    const string s("abc");
    const int i = 10;

    assert(get<0>(t) == s && get<1>(t) == i);
}
