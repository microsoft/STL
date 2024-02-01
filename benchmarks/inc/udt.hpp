// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

template <typename Data>
struct aggregate {
    Data c;

    friend bool operator==(const aggregate&, const aggregate&) = default;
};

template <typename Data>
struct non_trivial {
    Data c;
    non_trivial() : c() {}
    non_trivial(const Data& src) : c(src) {}
    non_trivial(const non_trivial& other) : c(other.c) {}
    non_trivial& operator=(const non_trivial& other) {
        c = other.c;
        return *this;
    }
    ~non_trivial() {}

    friend bool operator==(const non_trivial&, const non_trivial&) = default;
};
