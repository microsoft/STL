// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

#include <xoshiro.hpp>

template <class Contained>
std::vector<Contained> random_vector(size_t n) {
    std::random_device rd;
    std::uniform_int_distribution<std::uint64_t> id64;
    xoshiro256ss prng{id64(rd), id64(rd), id64(rd), id64(rd)};

    std::vector<Contained> res(n);

// Here, the type Contained can be char, int, aggregate<Data>, or non_trivial<Data> where Data is char or int.
// (aggregate<Data> and non_trivial<Data> are defined in udt.hpp.)
// static_cast<Contained> silences truncation warnings when Contained is directly char or int,
// but is insufficient for aggregate<Data> or non_trivial<Data>.
#pragma warning(push)
#pragma warning(disable : 4244) // warning C4244: conversion from 'uint64_t' to 'Data', possible loss of data
    std::generate(res.begin(), res.end(), [&prng] { return static_cast<Contained>(prng.next()); });
#pragma warning(pop)

    return res;
}
