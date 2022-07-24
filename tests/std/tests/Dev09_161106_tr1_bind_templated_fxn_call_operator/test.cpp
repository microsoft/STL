// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>

using namespace std;
using namespace std::placeholders;

struct Div {
    template <typename T, typename U>
    T operator()(const T& left, const U& right) const {
        return left / right;
    }
};

int main() {
    const Div div{};

    const int a = 16;
    const int b = 3;
    const int i = bind(div, a, _1)(b);

    assert(i == 5);
}
