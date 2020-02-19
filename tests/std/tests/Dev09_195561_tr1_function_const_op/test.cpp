// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <functional>

struct Cat {
    void operator()() {}
};

int main() {
    std::function<void()> f = Cat();
}
