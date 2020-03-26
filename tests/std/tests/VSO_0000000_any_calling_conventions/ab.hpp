// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#include <any>

std::any __cdecl f(std::any);

struct small_but_nontrivial {
    static int constructions;
    static int defaults;
    static int copies;
    static int moves;
    static int destructions;

    small_but_nontrivial() {
        ++constructions;
        ++defaults;
    }
    small_but_nontrivial(small_but_nontrivial&&) {
        ++constructions;
        ++moves;
    }
    small_but_nontrivial(const small_but_nontrivial&) {
        ++constructions;
        ++copies;
    }
    ~small_but_nontrivial() {
        ++destructions;
    }
};
