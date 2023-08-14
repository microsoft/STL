// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <any>
#include <utility>

struct incomplete;

template <class T>
struct wrapper {
    T t;
};

struct nontrivial {
    nontrivial() {}
    ~nontrivial() {}
};

int main() {
    using foo = wrapper<incomplete>*;
    struct bar {
        foo p{};
        nontrivial q{};
    };
    struct baz {
        foo p[10]{};
    };

    std::any a{foo{}}, b(bar{}), c{baz{}};
    a = a, b = b, c = c;
    a = std::move(a), b = std::move(b), c = std::move(c);
    auto a_ = std::any_cast<foo>(a);
    auto b_ = std::any_cast<bar>(b);
    auto c_ = std::any_cast<baz>(c);

    return 0;
}
