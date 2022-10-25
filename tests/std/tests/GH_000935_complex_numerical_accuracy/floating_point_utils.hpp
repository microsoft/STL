// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <cassert>
#include <cfenv>
#include <cfloat>
#include <cmath>
#include <type_traits>
#include <xutility>

namespace fputil {
    template <typename T>
    using float_bits_t = typename _STD _Floating_type_traits<T>::_Uint_type;

    template <typename T>
    _INLINE_VAR constexpr float_bits_t<T> sign_mask_v = _STD _Floating_type_traits<T>::_Shifted_sign_mask;

    template <typename T>
    _INLINE_VAR constexpr float_bits_t<T> magnitude_mask_v = ~sign_mask_v<T>;

    template <typename T>
    _INLINE_VAR constexpr float_bits_t<T> exponent_mask_v = _STD _Floating_type_traits<T>::_Shifted_exponent_mask;

    template <typename T>
    _INLINE_VAR constexpr float_bits_t<T> significand_mask_v = magnitude_mask_v<T> & ~exponent_mask_v<T>;

    template <typename T>
    _INLINE_VAR constexpr float_bits_t<T> norm_min_bits_v = significand_mask_v<T> + 1U;

    template <typename T>
    _INLINE_VAR constexpr float_bits_t<T> norm_max_bits_v = exponent_mask_v<T> - 1U;

    template <typename T>
    _INLINE_VAR constexpr float_bits_t<T> infinity_bits_v = exponent_mask_v<T>;

    // not affected by abrupt underflow
    template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    constexpr bool iszero(const T& x) {
        return _STD _Float_abs_bits(x) == 0;
    }

    // not affected by /fp:fast
    template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    constexpr bool signbit(const T& x) {
        const auto bits = std::_Bit_cast<float_bits_t<T>>(x);
        return (bits & sign_mask_v<T>) != 0;
    }

    enum class rounding_mode {
        to_nearest_ties_even = FE_TONEAREST,
        toward_zero          = FE_TOWARDZERO,
        toward_positive      = FE_UPWARD,
        toward_negative      = FE_DOWNWARD,
    };

    bool is_directed_rounding_mode(const rounding_mode mode) {
        switch (mode) {
        case rounding_mode::to_nearest_ties_even:
            return false;

        case rounding_mode::toward_zero:
        case rounding_mode::toward_positive:
        case rounding_mode::toward_negative:
            return true;

        default:
            assert(false);
            return false;
        }
    }

#if TEST_FP_ROUNDING

#ifdef __clang__
// TRANSITION, should be #pragma STDC FENV_ACCESS ON
#else // ^^^ clang / MSVC vvv
// TRANSITION, VSO-923474 -- should be #pragma STDC FENV_ACCESS ON
#pragma fenv_access(on)
#endif // ^^^ MSVC ^^^

    constexpr rounding_mode all_rounding_modes[] = {
        rounding_mode::to_nearest_ties_even,
        rounding_mode::toward_zero,
        rounding_mode::toward_positive,
        rounding_mode::toward_negative,
    };

    class rounding_guard {
    public:
        explicit rounding_guard(const rounding_mode mode) : old_mode{static_cast<rounding_mode>(std::fegetround())} {
            const int result = std::fesetround(static_cast<int>(mode));
            assert(result == 0);
        }

        ~rounding_guard() {
            const int result = std::fesetround(static_cast<int>(old_mode));
            assert(result == 0);
        }

        rounding_guard(const rounding_guard&)            = delete;
        rounding_guard& operator=(const rounding_guard&) = delete;

    private:
        rounding_mode old_mode;
    };

#else // ^^^ alternative rounding modes / default rounding mode only vvv

    constexpr rounding_mode all_rounding_modes[] = {rounding_mode::to_nearest_ties_even};

    class rounding_guard {
    public:
        explicit rounding_guard(const rounding_mode mode) {
            static_cast<void>(mode);
        }

        ~rounding_guard() = default;

        rounding_guard(const rounding_guard&)            = delete;
        rounding_guard& operator=(const rounding_guard&) = delete;
    };

#endif // ^^^ default rounding mode only ^^^

    // compares whether two floating point values are equal
    // all NaNs are equal, +0.0 and -0.0 are not equal
    template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    bool precise_equal(const T& actual, const T& expected) {
        if (_STD _Is_nan(actual) || _STD _Is_nan(expected)) {
            return _STD _Is_nan(actual) == _STD _Is_nan(expected);
        } else {
            return actual == expected && fputil::signbit(actual) == fputil::signbit(expected);
        }
    }

    namespace detail {
        // 0x80...00 = zero, 0x80...01 = numeric_limits<T>::denorm_min(), 0x7f...ff = -numeric_limits<T>::denorm_min()
        template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
        float_bits_t<T> offset_representation(const T& x) {
            const float_bits_t<T> abs_bits = _STD _Float_abs_bits(x);
            return fputil::signbit(x) ? sign_mask_v<T> - abs_bits : sign_mask_v<T> + abs_bits;
        }

        template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
        float_bits_t<T> is_offset_value_subnormal_or_zero(const float_bits_t<T> offset_value) {
            constexpr float_bits_t<T> positive_norm_min_offset = sign_mask_v<T> + norm_min_bits_v<T>;
            constexpr float_bits_t<T> negative_norm_min_offset = sign_mask_v<T> - norm_min_bits_v<T>;

            return negative_norm_min_offset < offset_value && offset_value < positive_norm_min_offset;
        }

        // number of ulps above zero, if we count [0, numeric_limits<T>::min()) as 1 ulp
        template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
        double abrupt_underflow_ulp(const float_bits_t<T> offset_value) {
            using bits_type = float_bits_t<T>;

            constexpr bits_type offset_positive_norm_min = sign_mask_v<T> + norm_min_bits_v<T>;
            constexpr bits_type offset_negative_norm_min = sign_mask_v<T> - norm_min_bits_v<T>;

            if (offset_value >= offset_positive_norm_min) {
                return 1.0 + (offset_value - offset_positive_norm_min);
            } else if (offset_value <= offset_negative_norm_min) {
                return -1.0 - (offset_negative_norm_min - offset_value);
            } else if (offset_value >= sign_mask_v<T>) {
                return static_cast<double>(offset_value - sign_mask_v<T>) / norm_min_bits_v<T>;
            } else {
                return -static_cast<double>(sign_mask_v<T> - offset_value) / norm_min_bits_v<T>;
            }
        }

        template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
        bool is_within_ulp_tolerance(const T& actual, const T& expected, const int ulp_tolerance) {
            if (_STD _Is_nan(actual) || _STD _Is_nan(expected)) {
                return _STD _Is_nan(actual) == _STD _Is_nan(expected);
            }

            if (_STD _Is_inf(expected)) {
                return actual == expected;
            }

            if (fputil::signbit(actual) != fputil::signbit(expected)) {
                return false;
            }

            using bits_type = float_bits_t<T>;

            // compute ulp difference
            const bits_type actual_offset   = detail::offset_representation(actual);
            const bits_type expected_offset = detail::offset_representation(expected);
            const bits_type ulp_diff =
                actual_offset < expected_offset ? expected_offset - actual_offset : actual_offset - expected_offset;

            if (ulp_diff <= static_cast<unsigned int>(ulp_tolerance) && ulp_tolerance >= 0) {
                return true;
            }

#if WITH_FP_ABRUPT_UNDERFLOW
            // handle abrupt underflow
            if (detail::is_offset_value_subnormal_or_zero<T>(expected_offset)
                || detail::is_offset_value_subnormal_or_zero<T>(actual_offset)) {
                const double adjusted_actual_ulp   = detail::abrupt_underflow_ulp<T>(actual_offset);
                const double adjusted_expected_ulp = detail::abrupt_underflow_ulp<T>(expected_offset);
                const double adjusted_ulp_diff     = std::abs(adjusted_actual_ulp - adjusted_expected_ulp);

                if (adjusted_ulp_diff <= ulp_tolerance) {
                    return true;
                }
            }
#endif // WITH_FP_ABRUPT_UNDERFLOW

            return false;
        }

        template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
        bool is_within_absolute_tolerance(const T& actual, const T& expected, const double absolute_tolerance) {
            return _STD _Is_finite(actual) && _STD _Is_finite(expected)
                && std::abs(actual - expected) <= absolute_tolerance;
        }
    } // namespace detail

    // returns whether floating point result is nearly equal to the expected value
    template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    bool near_equal(
        const T& actual, const T& expected, const int ulp_tolerance = 1, const double absolute_tolerance = 0) {
        if (precise_equal(actual, expected)) {
            return true;
        }

        if (ulp_tolerance > 0 && detail::is_within_ulp_tolerance(actual, expected, ulp_tolerance)) {
            return true;
        }

        if (absolute_tolerance > 0 && detail::is_within_absolute_tolerance(actual, expected, absolute_tolerance)) {
            return true;
        }

        return false;
    }
} // namespace fputil
