// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <random>
#include <type_traits>
#include <vector>

template <class Contained, template <class> class Alloc = std::allocator>
std::vector<Contained, Alloc<Contained>> random_vector(size_t n) {
    std::vector<Contained, Alloc<Contained>> res(n);

    if constexpr (std::is_same_v<Contained, bool>) {
        std::mt19937 gen;
        std::bernoulli_distribution dist{0.5};
        std::generate(res.begin(), res.end(), [&] { return dist(gen); });
    } else {
        std::mt19937_64 prng;
// Here, the type Contained can be char, int, aggregate<Data>, or non_trivial<Data> where Data is char or int.
// (aggregate<Data> and non_trivial<Data> are defined in udt.hpp.)
// static_cast<Contained> silences truncation warnings when Contained is directly char or int,
// but is insufficient for aggregate<Data> or non_trivial<Data>.
#pragma warning(push)
#pragma warning(disable : 4244) // warning C4244: conversion from 'uint64_t' to 'Data', possible loss of data
        std::generate(res.begin(), res.end(), [&prng] { return static_cast<Contained>(prng()); });
#pragma warning(pop)
    }

    return res;
}
