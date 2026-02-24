// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// icx-cl /EHsc /W4 /MT /O2 /std:c++latest /fp:precise /Qoption,cpp,--extended_float_types

#include <cstddef>
#include <cstdio>
#include <print>
#include <queue>
#include <string_view>
#include <utility>
#include <vector>
using namespace std;

// get rid of the squiggles
#ifdef __INTELLISENSE__
using __float128 = long double;

inline constexpr __float128 operator"" q(long double value) noexcept {
    return value;
}
#endif // defined(__INTELLISENSE__)

extern "C" [[nodiscard]] __float128 __fabsq(__float128 x) noexcept;
extern "C" [[nodiscard]] __float128 __fminq(__float128 x, __float128 y) noexcept;
extern "C" [[nodiscard]] __float128 __nearbyintq(__float128 x) noexcept;
extern "C" [[nodiscard]] __float128 __expq(__float128 x) noexcept;
extern "C" [[nodiscard]] __float128 __scalbnq(__float128 x, int y) noexcept;
extern "C" [[nodiscard]] __float128 __logq(__float128 x) noexcept;
extern "C" [[nodiscard]] __float128 __sqrtq(__float128 x) noexcept;
extern "C" [[nodiscard]] __float128 __cbrtq(__float128 x) noexcept;
extern "C" [[nodiscard]] __float128 __erfq(__float128 x) noexcept;

constexpr size_t layer_division   = 256;
constexpr __float128 regular_area = 1.0q / layer_division;

struct ziggurat_layer {
    __float128 x_inner{};
    __float128 x_outer{};
    __float128 y_min{};
    __float128 y_max{};
};

struct alias_table_entry {
    __float128 probability{};
    size_t index{};
    size_t alias_index = index;
};

template <class FloatType>
struct modified_ziggurat_traits;

template <>
struct modified_ziggurat_traits<double> {
    static constexpr string_view type_name   = "double"sv;
    static constexpr string_view type_suffix = ""sv;
    static constexpr int bits                = 64;
};

template <>
struct modified_ziggurat_traits<float> {
    static constexpr string_view type_name   = "float"sv;
    static constexpr string_view type_suffix = "f"sv;
    static constexpr int bits                = 32;
};

template <class FloatType = double>
void generate_tables(string_view name, bool is_signed, auto&& pdf, auto&& inverse_pdf, auto&& cdf,
    auto&& pdf_derivative, auto&& pdf_2nd_derivative, __float128 height_scale = 1.0q) {
    // the bottommost layer is excluded
    vector<ziggurat_layer> layers;

    __float128 x_inner = 0.0q;
    __float128 y_max   = pdf(x_inner);

    // minimize the total height of the top two layers
    __float128 x_outer = [&] {
        if (const __float128 y1 = pdf_derivative(0.0q); y1 < 0.0q) {
            return __sqrtq(regular_area / -y1);
        }

        if (const __float128 y2 = pdf_2nd_derivative(0.0q); y2 < 0.0q) {
            return __cbrtq(regular_area / -y2);
        }

        return 1.0q;
    }();

    for (;;) {
        const __float128 y1 = pdf_derivative(x_outer) + regular_area / (x_outer * x_outer);
        const __float128 y2 = pdf_2nd_derivative(x_outer) - 2.0q * regular_area / (x_outer * x_outer * x_outer);
        const __float128 dx = y1 / y2;
        x_outer -= dx;
        if (!(__fabsq(dx) > __fabsq(x_outer) * 0x1p-80q)) {
            break;
        }
    }

    __float128 y_min = pdf(x_outer);

    // make sure that we have at most `layer_division` irregular regions
    // the bottommost layer has 2 irregular regions, other layers have 1 each
    while (layers.size() + 2 < layer_division) {
        layers.push_back({.x_inner = x_inner, .x_outer = x_outer, .y_min = y_min, .y_max = y_max});

        x_inner = x_outer;
        y_max   = y_min;
        y_min   = y_max - regular_area / x_inner;
        if (y_min < 0.0q) {
            break;
        }

        x_outer = inverse_pdf(y_min);
    }

    const size_t regular_layer_num = layers.size() - 1;

    vector<__float128> irregular_areas;

    for (const ziggurat_layer& layer : layers) {
        irregular_areas.push_back(
            cdf(layer.x_outer) - cdf(layer.x_inner) - (layer.x_outer - layer.x_inner) * layer.y_min);
    }

    // the bottommost layer
    irregular_areas.push_back(1.0q - cdf(layers.back().x_outer));
    irregular_areas.push_back(layers.back().x_outer * layers.back().y_min);

    // alias table for irregular region selection
    vector<alias_table_entry> alias_table(layer_division);

    const __float128 average_area = (layer_division - regular_layer_num) * regular_area / layer_division;
    vector<alias_table_entry> temp_overs;
    vector<alias_table_entry> temp_unders;

    for (size_t i = 0; i != irregular_areas.size(); ++i) {
        const __float128 probability = irregular_areas[i] / average_area;
        const alias_table_entry entry{.probability = probability, .index = i};
        if (probability > 1.0) {
            temp_overs.push_back(entry);
        } else if (probability < 1.0) {
            temp_unders.push_back(entry);
        } else {
            alias_table[i] = entry;
        }
    }

    for (size_t i = irregular_areas.size(); i != layer_division; ++i) {
        temp_unders.push_back({.probability = 0.0q, .index = i});
    }

    priority_queue overs(
        [](const alias_table_entry& lhs, const alias_table_entry& rhs) { return lhs.probability > rhs.probability; },
        std::move(temp_overs));
    priority_queue unders(
        [](const alias_table_entry& lhs, const alias_table_entry& rhs) { return lhs.probability < rhs.probability; },
        std::move(temp_unders));

    while (!overs.empty() && !unders.empty()) {
        const alias_table_entry& over        = overs.top();
        const alias_table_entry& under       = unders.top();
        alias_table[under.index].probability = under.probability;
        alias_table[under.index].index       = under.index;
        alias_table[under.index].alias_index = over.index;

        const alias_table_entry remaining{
            .probability = over.probability - 1.0q + under.probability, .index = over.index};
        overs.pop();
        unders.pop();
        if (remaining.probability > 1.0) {
            overs.push(remaining);
        } else if (remaining.probability < 1.0) {
            unders.push(remaining);
        } else {
            alias_table[remaining.index] = remaining;
        }
    }

    while (!overs.empty()) {
        alias_table[overs.top().index] = overs.top();
        overs.pop();
    }

    while (!unders.empty()) {
        alias_table[unders.top().index] = unders.top();
        unders.pop();
    }

    // generate tables
    using traits                       = modified_ziggurat_traits<FloatType>;
    const __float128 width_scale       = __scalbnq(is_signed ? 2.0q : 1.0q, -traits::bits);
    const __float128 probability_scale = __scalbnq(1.0q, traits::bits);
    const __float128 max_probability   = 1.0q - __scalbnq(1.0q, -traits::bits);

    println();
    println("template <>");
    println("_INLINE_VAR constexpr _Modified_ziggurat_tables<{0}, uint{1}_t, {2}, {3}> {4}<{0}>{{", traits::type_name,
        traits::bits, is_signed, layers.size(), name);

    // _Ty _Layer_widths[_Layer_num + 1];
    print("    {{{:#}{}", static_cast<FloatType>(layers[0].x_inner * width_scale), traits::type_suffix);
    for (const ziggurat_layer& layer : layers) {
        print(", {:#}{}", static_cast<FloatType>(layer.x_outer * width_scale), traits::type_suffix);
    }

    println("}},");

    // _Ty _Layer_heights[_Layer_num + 1];
    print("    {{{:#}{}", static_cast<FloatType>(layers[0].y_max * height_scale), traits::type_suffix);
    for (const ziggurat_layer& layer : layers) {
        print(", {:#}{}", static_cast<FloatType>(layer.y_min * height_scale), traits::type_suffix);
    }

    println("}},");

    // _Uty _Alias_probabilities[256];
    for (bool first = true; const alias_table_entry& entry : alias_table) {
        if (first) {
            print("    {{{}u", static_cast<unsigned long long>(
                                   __nearbyintq(__fminq(entry.probability, max_probability) * probability_scale)));
            first = false;
        } else {
            print(", {}u", static_cast<unsigned long long>(
                               __nearbyintq(__fminq(entry.probability, max_probability) * probability_scale)));
        }
    }

    println("}},");

    // uint8_t _Alias_indices[256];
    for (bool first = true; const alias_table_entry& entry : alias_table) {
        if (first) {
            print("    {{{}", entry.alias_index);
            first = false;
        } else {
            print(", {}", entry.alias_index);
        }
    }

    println("}},");

    println("}};");
}

__float128 half_normal_pdf(__float128 x) {
    return __expq(x * x * -0.5q) * 0x1.9884533d436508d0fcb3c500bab9p-1q;
}

__float128 half_normal_inverse_pdf(__float128 y) {
    return __sqrtq(-2.0q * __logq(y * 0x1.40d931ff627059657ca41fae722dp+0q));
}

__float128 half_normal_cdf(__float128 x) {
    return __erfq(x * 0x1.6a09e667f3bcda1ec56c7db8f04cp-1q);
}

__float128 half_normal_pdf_derivative(__float128 x) {
    return __expq(x * x * -0.5q) * x * -0x1.9884533d436508d0fcb3c500bab9p-1q;
}

__float128 half_normal_pdf_2nd_derivative(__float128 x) {
    return __expq(x * x * -0.5q) * (x + 1.0q) * (x - 1.0q) * 0x1.9884533d436508d0fcb3c500bab9p-1q;
}

void generate_header() {
    puts(R"(// __msvc_random_ziggurat_tables.hpp internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef __MSVC_RANDOM_ZIGGURAT_TABLES_HPP
#define __MSVC_RANDOM_ZIGGURAT_TABLES_HPP
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#include <cstdint>
#include <type_traits>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN

template <class _Ty, class _Uty, bool _Signed, int _Lx>
struct _Modified_ziggurat_tables {
    static_assert(_Lx >= 2, "invalid table size");
    static_assert(_Lx <= 254, "invalid table size");

    using _Uint_type = _Uty;
    using _Xtype     = conditional_t<_Signed, make_signed_t<_Uty>, _Uty>;

    static constexpr int _Layer_num   = _Lx;
    static constexpr _Ty _Width_scale = (_Signed ? _Ty{1} : _Ty{2}) * _Ty{static_cast<_Uty>(-1) / 2u + 1u};

    _Ty _Layer_widths[_Lx + 1];
    _Ty _Layer_heights[_Lx + 1];
    _Uty _Alias_probabilities[256];
    uint8_t _Alias_indices[256];
};)");
}

void generate_normal_distribution() {
    println();
    println("template <class _Ty>");
    println("_INLINE_VAR constexpr _Modified_ziggurat_tables<_Ty, unsigned int, true, 2> _Normal_distribution_tables;");

    generate_tables<double>("_Normal_distribution_tables"sv, true, half_normal_pdf, half_normal_inverse_pdf,
        half_normal_cdf, half_normal_pdf_derivative, half_normal_pdf_2nd_derivative,
        0x1.40d931ff627059657ca41fae722dp+0q);

    generate_tables<float>("_Normal_distribution_tables"sv, true, half_normal_pdf, half_normal_inverse_pdf,
        half_normal_cdf, half_normal_pdf_derivative, half_normal_pdf_2nd_derivative,
        0x1.40d931ff627059657ca41fae722dp+0q);
}

void generate_footer() {
    puts(R"(
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // __MSVC_RANDOM_ZIGGURAT_TABLES_HPP)");
}

int main() {
    generate_header();
    generate_normal_distribution();
    generate_footer();
    return 0;
}
