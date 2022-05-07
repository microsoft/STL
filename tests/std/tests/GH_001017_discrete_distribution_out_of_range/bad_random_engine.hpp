// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <type_traits>

namespace detail {
    struct bad_rng_pattern_sentinel {};

    template <typename UInt, int Width = std::numeric_limits<UInt>::digits>
    class bad_rng_pattern_generator { // generates bit patterns for bad_random_engine
    public:
        using difference_type   = std::ptrdiff_t;
        using value_type        = UInt;
        using pointer           = const UInt*;
        using reference         = const UInt&;
        using iterator_category = std::input_iterator_tag;
        using my_iter           = bad_rng_pattern_generator;
        using my_sentinel       = bad_rng_pattern_sentinel;

        static constexpr value_type top_bit    = value_type{1} << (Width - 1);
        static constexpr value_type lower_bits = top_bit - 1;
        static constexpr value_type mask_bits  = top_bit | lower_bits;
        static constexpr int final_bit_count   = (Width - 1) / 2 + 1;

        constexpr reference operator*() const noexcept { // gets the current pattern
            return current_value_;
        }

        constexpr pointer operator->() const noexcept {
            return &current_value_;
        }

        constexpr my_iter& operator++() noexcept { // generates the next pattern
            current_value_ = (current_value_ & lower_bits) << 1 | (current_value_ & top_bit) >> (Width - 1);

            if (current_shift_ < Width - 1 && current_bit_count_ != 0 && current_bit_count_ != Width) {
                ++current_shift_;
                return *this;
            }

            current_shift_ = 0;

            if (current_bit_count_ < final_bit_count) { // n 1's -> n 0's
                current_bit_count_ = Width - current_bit_count_;
                current_value_ ^= mask_bits;
            } else if (current_bit_count_ > final_bit_count) { // n 0's -> (n+1) 1's
                current_bit_count_ = Width - current_bit_count_ + 1;
                current_value_     = (current_value_ ^ mask_bits) << 1 | value_type{1};
            } else { // all bit patterns have been generated, back to all 0's
                current_bit_count_ = 0;
                current_value_     = value_type{0};
            }

            return *this;
        }

        constexpr my_iter operator++(int) noexcept {
            const my_iter old = *this;
            ++*this;
            return old;
        }

        friend constexpr bool operator==(const my_iter& a, const my_iter& b) noexcept {
            return *a == *b;
        }

        friend constexpr bool operator!=(const my_iter& a, const my_iter& b) noexcept {
            return !(a == b);
        }

        friend constexpr bool operator==(const my_iter& iter, my_sentinel) noexcept {
            return *iter == 0;
        }

        friend constexpr bool operator!=(const my_iter& iter, const my_sentinel sentinel) noexcept {
            return !(iter == sentinel);
        }

        friend constexpr bool operator==(const my_sentinel sentinel, const my_iter& iter) noexcept {
            return iter == sentinel;
        }

        friend constexpr bool operator!=(const my_sentinel sentinel, const my_iter& iter) noexcept {
            return !(iter == sentinel);
        }

    private:
        value_type current_value_ = 0;
        int current_bit_count_    = 0;
        int current_shift_        = 0;
    };
} // namespace detail

template <typename UInt, int Width = std::numeric_limits<UInt>::digits, int Dimension = 1>
class bad_random_engine {
    // Generates bit patterns with at most two transitions between 0's and 1's.
    // (e.g. 00000000, 11111111, 00001111, 11110000, 00011000, 11100111)
    // When its output is grouped into subsequences of length Dimension, it cycles through all possible subsequences
    // containing only such bit patterns. Bit patterns with few 1's or few 0's are generated first, starting from all
    // 0's and all 1's.

    static_assert(std::is_integral_v<UInt>, "bad_random_engine: UInt should be unsigned integral type");
    static_assert(std::is_unsigned_v<UInt>, "bad_random_engine: UInt should be unsigned integral type");
    static_assert(Width > 0, "bad_random_engine: invalid value for Width");
    static_assert(Width <= std::numeric_limits<UInt>::digits, "bad_random_engine: invalid value for Width");
    static_assert(Dimension > 0, "bad_random_engine: invalid value for Dimension");

public:
    using result_type = UInt;

    static constexpr result_type(min)() noexcept {
        return result_type{0};
    }
    static constexpr result_type(max)() noexcept {
        return generator::mask_bits;
    }

    constexpr result_type operator()() noexcept {
        const result_type result = *generators_[current_dimension_];

        if (current_dimension_ < Dimension - 1) {
            ++current_dimension_;
        } else {
            current_dimension_ = 0;

            if (!generate_next()) {
                has_cycled_through_ = true;
            }
        }

        return result;
    }

    constexpr bool has_cycled_through() const noexcept { // have we finished a full cycle?
        return has_cycled_through_;
    }

private:
    using generator = detail::bad_rng_pattern_generator<UInt, Width>;
    using sentinel  = detail::bad_rng_pattern_sentinel;

    constexpr bool generate_next() noexcept { // generates the next subsequence, returns false if back to all 0's
        if (limit_value_ != sentinel{}) {
            for (int i = 0; i < limit_dimension_; ++i) {
                if (generators_[i] != limit_value_) {
                    ++generators_[i];
                    return true;
                } else {
                    generators_[i] = generator{};
                }
            }

            for (int i = limit_dimension_ + 1; i < Dimension; ++i) {
                ++generators_[i];
                if (generators_[i] != limit_value_) {
                    return true;
                } else {
                    generators_[i] = generator{};
                }
            }

            _Analysis_assume_(limit_dimension_ < Dimension);
            generators_[limit_dimension_] = generator{};

            if (limit_dimension_ < Dimension - 1) {
                ++limit_dimension_;
                generators_[limit_dimension_] = limit_value_;
                return true;
            }
        }

        limit_dimension_ = 0;
        generators_[0]   = ++limit_value_;
        return limit_value_ != sentinel{};
    }

    generator generators_[Dimension] = {};
    generator limit_value_{};
    int limit_dimension_     = 0;
    int current_dimension_   = 0;
    bool has_cycled_through_ = false;
};

// the cycle length of bad_random_generator is 32 546 312
using bad_random_generator = bad_random_engine<std::uint64_t, 64, 2>;
