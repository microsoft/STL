// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstring>
#include <new>

using namespace std;

int main() {
    assert(strcmp(std::bad_alloc().what(), "bad allocation") == 0);
}
