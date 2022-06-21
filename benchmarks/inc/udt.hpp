// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

template <typename Contained>
struct aggregate {
    Contained c;

    friend bool operator==(const aggregate&, const aggregate&) = default;
};

template <typename Contained>
struct non_trivial {
    Contained c;
    non_trivial() : c() {}
    non_trivial(const Contained& src) : c(src) {}
    non_trivial(const non_trivial& other) : c(other.c) {}
    non_trivial& operator=(const non_trivial& other) {
        c = other.c;
        return *this;
    }
    ~non_trivial() {}

    friend bool operator==(const non_trivial&, const non_trivial&) = default;
};
