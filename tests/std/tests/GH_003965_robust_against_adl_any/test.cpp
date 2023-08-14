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
    std::any *ap = &a, *bp = &b, *cp = &c;
    a = *ap, b = *bp, c = *cp;
    a = std::move(*ap), b = std::move(*bp), c = std::move(*cp);
    (void) std::any_cast<foo>(a);
    (void) std::any_cast<bar>(b);
    (void) std::any_cast<baz>(c);

    return 0;
}
