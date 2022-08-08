/*  Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>.
SPDX-License-Identifier: CC0-1.0 */

#pragma once

#include <bit>
#include <stdint.h>

struct xoshiro256ss {
    xoshiro256ss() = delete;
    xoshiro256ss(uint64_t s0, uint64_t s1, uint64_t s2, uint64_t s3) : s0_(s0), s1_(s1), s2_(s2), s3_(s3) {}

    uint64_t next() {
        auto result = std::rotl(s1_ * 5, 7) * 9;

        const uint64_t t = s1_ << 17;

        s2_ ^= s0_;
        s3_ ^= s1_;
        s1_ ^= s2_;
        s0_ ^= s3_;

        s2_ ^= t;

        s3_ = std::rotl(s3_, 45);

        return result;
    }

private:
    uint64_t s0_;
    uint64_t s1_;
    uint64_t s2_;
    uint64_t s3_;
};
