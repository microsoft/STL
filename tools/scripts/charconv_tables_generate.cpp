// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// The following code generated the lookup tables for the
// scientific exponent X. Don't remove this code.
// cl /EHsc /nologo /W4 /MT /O2 /std:c++17 charconv_tables_generate.cpp && charconv_tables_generate

#include <algorithm>
#include <cassert>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <map>
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
