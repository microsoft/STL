// xcharconv_tables.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XCHARCONV_TABLES_H
#define _XCHARCONV_TABLES_H
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#if !_HAS_CXX17
#error The contents of <charconv> are only available with C++17. (Also, you should not include this internal header.)
#endif // !_HAS_CXX17

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

#include <cstdint>

// For general precision, we can use lookup tables to avoid performing trial formatting.

// For a simple example, imagine counting the number of digits D in an integer, and needing to know
// whether D is less than 3, equal to 3/4/5/6, or greater than 6. We could use a lookup table:
// D | Largest integer with D digits
// 2 |      99
// 3 |     999
// 4 |   9'999
// 5 |  99'999
// 6 | 999'999
// 7 | table end
// Looking up an integer in this table with lower_bound() will work:
// * Too-small integers, like 7, 70, and 99, will cause lower_bound() to return the D == 2 row. (If all we care
//   about is whether D is less than 3, then it's okay to smash the D == 1 and D == 2 cases together.)
// * Integers in [100, 999] will cause lower_bound() to return the D == 3 row, and so forth.
// * Too-large integers, like 1'000'000 and above, will cause lower_bound() to return the end of the table. If we
//   compute D from that index, this will be considered D == 7, which will activate any "greater than 6" logic.

// Floating-point is slightly more complicated.

// The ordinary lookup tables are for X within [-5, 38] for float, and [-5, 308] for double.
// (-5 absorbs too-negative exponents, outside the P > X >= -4 criterion. 38 and 308 are the maximum exponents.)
// Due to the P > X condition, we can use a subset of the table for X within [-5, P - 1], suitably clamped.

// When P is small, rounding can affect X. For example:
// For P == 1, the largest double with X == 0 is: 9.4999999999999982236431605997495353221893310546875
// For P == 2, the largest double with X == 0 is: 9.949999999999999289457264239899814128875732421875
// For P == 3, the largest double with X == 0 is: 9.9949999999999992184029906638897955417633056640625

// Exponent adjustment is a concern for P within [1, 7] for float, and [1, 15] for double (determined via
// brute force). While larger values of P still perform rounding, they can't trigger exponent adjustment.
// This is because only values with repeated '9' digits can undergo exponent adjustment during rounding,
// and floating-point granularity limits the number of consecutive '9' digits that can appear.

// So, we need special lookup tables for small values of P.
// These tables have varying lengths due to the P > X >= -4 criterion. For example:
// For P == 1, need table entries for X: -5, -4, -3, -2, -1, 0
// For P == 2, need table entries for X: -5, -4, -3, -2, -1, 0, 1
// For P == 3, need table entries for X: -5, -4, -3, -2, -1, 0, 1, 2
// For P == 4, need table entries for X: -5, -4, -3, -2, -1, 0, 1, 2, 3

// We can concatenate these tables for compact storage, using triangular numbers to access them.
// The table for P begins at index (P - 1) * (P + 10) / 2 with length P + 5.

// For both the ordinary and special lookup tables, after an index I is returned by lower_bound(), X is I - 5.

// We need to special-case the floating-point value 0.0, which is considered to have X == 0.
// Otherwise, the lookup tables would consider it to have a highly negative X.

// Finally, because we're working with positive floating-point values,
// representation comparisons behave identically to floating-point comparisons.

// The following code generated the lookup tables for the scientific exponent X. Don't remove this code.
#if 0
// cl /EHsc /nologo /W4 /MT /O2 /std:c++17 generate_tables.cpp && generate_tables

#include <algorithm>
#include <assert.h>
#include <charconv>
#include <cmath>
#include <limits>
#include <map>
#include <stdint.h>
#include <stdio.h>
#include <system_error>
#include <type_traits>
#include <vector>
using namespace std;

template <typename UInt, typename Pred>
[[nodiscard]] UInt uint_partition_point(UInt first, const UInt last, Pred pred) {
    // Find the beginning of the false partition in [first, last).
    // [first, last) is partitioned when all of the true values occur before all of the false values.

    static_assert(is_unsigned_v<UInt>);
    assert(first <= last);

    for (UInt n = last - first; n > 0;) {
        const UInt n2  = n / 2;
        const UInt mid = first + n2;

        if (pred(mid)) {
            first = mid + 1;
            n     = n - n2 - 1;
        } else {
            n = n2;
        }
    }

    return first;
}

template <typename Floating>
[[nodiscard]] int scientific_exponent_X(const int P, const Floating flt) {
    char buf[400]; // more than enough

    // C11 7.21.6.1 "The fprintf function"/8 performs trial formatting with scientific precision P - 1.
    const auto to_result = to_chars(buf, end(buf), flt, chars_format::scientific, P - 1);
    assert(to_result.ec == errc{});

    const char* exp_ptr = find(buf, to_result.ptr, 'e');
    assert(exp_ptr != to_result.ptr);

    ++exp_ptr; // advance past 'e'

    if (*exp_ptr == '+') {
        ++exp_ptr; // advance past '+' which from_chars() won't parse
    }

    int X;
    const auto from_result = from_chars(exp_ptr, to_result.ptr, X);
    assert(from_result.ec == errc{});
    return X;
}

template <typename UInt>
void print_table(const vector<UInt>& v, const char* const name) {
    constexpr const char* UIntName = is_same_v<UInt, uint32_t> ? "uint32_t" : "uint64_t";

    printf("static const %s %s[%zu] = {\n", UIntName, name, v.size());

    for (const auto& val : v) {
        if constexpr (is_same_v<UInt, uint32_t>) {
            printf("0x%08Xu,\n", val);
        } else {
            printf("0x%016llXu,\n", val);
        }
    }

    printf("};\n");
}

enum class Mode { Tables, Tests };

template <typename Floating>
void generate_tables(const Mode mode) {
    using Limits = numeric_limits<Floating>;
    using UInt   = conditional_t<is_same_v<Floating, float>, uint32_t, uint64_t>;

    map<int, map<int, UInt>> P_X_LargestValWithX;

    constexpr int MaxP = Limits::max_exponent10 + 1; // MaxP performs no rounding during trial formatting

    for (int P = 1; P <= MaxP; ++P) {
        for (int X = -5; X < P; ++X) {
            constexpr Floating first = static_cast<Floating>(9e-5); // well below 9.5e-5, otherwise arbitrary
            constexpr Floating last  = Limits::infinity(); // one bit above Limits::max()

            const UInt val_beyond_X = uint_partition_point(reinterpret_cast<const UInt&>(first),
                reinterpret_cast<const UInt&>(last),
                [P, X](const UInt u) { return scientific_exponent_X(P, reinterpret_cast<const Floating&>(u)) <= X; });

            P_X_LargestValWithX[P][X] = val_beyond_X - 1;
        }
    }

    constexpr const char* FloatingName = is_same_v<Floating, float> ? "float" : "double";

    constexpr int MaxSpecialP = is_same_v<Floating, float> ? 7 : 15; // MaxSpecialP is affected by exponent adjustment

    if (mode == Mode::Tables) {
        printf("template <>\n");
        printf("struct _General_precision_tables_2<%s> {\n", FloatingName);

        printf("static constexpr int _Max_special_P = %d;\n", MaxSpecialP);

        vector<UInt> special;

        for (int P = 1; P <= MaxSpecialP; ++P) {
            for (int X = -5; X < P; ++X) {
                const UInt val = P_X_LargestValWithX[P][X];
                special.push_back(val);
            }
        }

        print_table(special, "_Special_X_table");

        for (int P = MaxSpecialP + 1; P < MaxP; ++P) {
            for (int X = -5; X < P; ++X) {
                const UInt val = P_X_LargestValWithX[P][X];
                assert(val == P_X_LargestValWithX[MaxP][X]);
            }
        }

        printf("static constexpr int _Max_P = %d;\n", MaxP);

        vector<UInt> ordinary;

        for (int X = -5; X < MaxP; ++X) {
            const UInt val = P_X_LargestValWithX[MaxP][X];
            ordinary.push_back(val);
        }

        print_table(ordinary, "_Ordinary_X_table");

        printf("};\n");
    } else {
        printf("==========\n");
        printf("Test cases for %s:\n", FloatingName);

        constexpr int Hexits         = is_same_v<Floating, float> ? 6 : 13;
        constexpr const char* Suffix = is_same_v<Floating, float> ? "f" : "";

        for (int P = 1; P <= MaxP; ++P) {
            for (int X = -5; X < P; ++X) {
                if (P <= MaxSpecialP || P == 25 || P == MaxP || X == P - 1) {
                    const UInt val1   = P_X_LargestValWithX[P][X];
                    const Floating f1 = reinterpret_cast<const Floating&>(val1);
                    const UInt val2   = val1 + 1;
                    const Floating f2 = reinterpret_cast<const Floating&>(val2);

                    printf("{%.*a%s, chars_format::general, %d, \"%.*g\"},\n", Hexits, f1, Suffix, P, P, f1);
                    if (isfinite(f2)) {
                        printf("{%.*a%s, chars_format::general, %d, \"%.*g\"},\n", Hexits, f2, Suffix, P, P, f2);
                    }
                }
            }
        }
    }
}

int main() {
    printf("template <class _Floating>\n");
    printf("struct _General_precision_tables_2;\n");
    generate_tables<float>(Mode::Tables);
    generate_tables<double>(Mode::Tables);
    generate_tables<float>(Mode::Tests);
    generate_tables<double>(Mode::Tests);
}
#endif // 0

_STD_BEGIN

template <class _Floating>
struct _General_precision_tables_2;

template <>
struct _General_precision_tables_2<float> {
    static constexpr int _Max_special_P = 7;

    static const uint32_t _Special_X_table[63];

    static constexpr int _Max_P = 39;

    static const uint32_t _Ordinary_X_table[44];
};

template <>
struct _General_precision_tables_2<double> {
    static constexpr int _Max_special_P = 15;

    static const uint64_t _Special_X_table[195];

    static constexpr int _Max_P = 309;

    static const uint64_t _Ordinary_X_table[314];
};

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XCHARCONV_TABLES_H
