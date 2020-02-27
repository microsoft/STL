// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <new>
#include <string.h>

using namespace std;

int main() {
    assert(strcmp(std::bad_alloc().what(), "bad allocation") == 0);
}
