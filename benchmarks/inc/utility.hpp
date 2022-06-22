#pragma once

#include <iterator>
#include <random>
#include <vector>

struct xoshiro256ss {
    xoshiro256ss() = delete;
    xoshiro256ss(uint64_t s0, uint64_t s1, uint64_t s2, uint64_t s3) : s0_(s0), s1_(s1), s2_(s2), s3_(s3) {}

    static uint64_t rotate_left(uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }

    uint64_t next() {
        auto result = rotate_left(s1_ * 5, 7) * 9;

        const uint64_t t = s1_ << 17;

        s2_ ^= s0_;
        s3_ ^= s1_;
        s1_ ^= s2_;
        s0_ ^= s3_;

        s2_ ^= t;

        s3_ = rotate_left(s3_, 45);

        return result;
    }

private:
    uint64_t s0_;
    uint64_t s1_;
    uint64_t s2_;
    uint64_t s3_;
};

template <class Contained>
std::vector<Contained> random_vector(size_t n) {
    std::random_device rd;
    xoshiro256ss prng{rd(), rd(), rd(), rd()};

    std::vector<Contained> res(n);
    std::generate(res.begin(), res.end(), [&prng]() { return static_cast<Contained>(prng.next()); });
    return res;
}
