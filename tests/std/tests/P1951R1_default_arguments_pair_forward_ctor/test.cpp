// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <memory>
#include <utility>

using namespace std;

int main() {
    pair<int, unique_ptr<int>> p{42, {}};
}
