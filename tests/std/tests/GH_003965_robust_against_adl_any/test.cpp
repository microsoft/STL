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
        foo p[20]{};
    };

    std::any a{foo{}}, b(bar{}), c{baz{}};
    a = a, b = b, c = c;
    a = std::move(a), b = std::move(b), c = std::move(c);
    (void) std::any_cast<foo>(a);
    (void) std::any_cast<bar>(b);
    (void) std::any_cast<baz>(c);

    return 0;
}
