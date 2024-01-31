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
    std::uniform_int_distribution<uint64_t> id64;
    xoshiro256ss prng{id64(rd), id64(rd), id64(rd), id64(rd)};

    std::vector<Contained> res(n);

#pragma warning(push)
#pragma warning(disable : 4244) // conversion from 'uint64_t' to 'Contained', possible loss of data
    std::generate(res.begin(), res.end(), [&prng] { return static_cast<Contained>(prng.next()); });
#pragma warning(pop)

    return res;
}
