// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <any>
#include <cassert>
#include <utility>

#include "ab.hpp"

int small_but_nontrivial::constructions = 0;
int small_but_nontrivial::defaults      = 0;
int small_but_nontrivial::copies        = 0;
int small_but_nontrivial::moves         = 0;
int small_but_nontrivial::destructions  = 0;

int __cdecl main() {
    {
        // Pass a std::any across TUs with different default calling conventions,
        // copy / move / destroy it. Get back a std::any from the other TU,
        // copy / move / destroy it as well.
        std::any a = f(small_but_nontrivial{});
        std::any b = a;
        { std::any c = std::move(a); }
    }

    assert(small_but_nontrivial::constructions == small_but_nontrivial::destructions);
    assert(small_but_nontrivial::constructions
           == small_but_nontrivial::defaults + small_but_nontrivial::copies + small_but_nontrivial::moves);
}
