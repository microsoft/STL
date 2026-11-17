# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from collections.abc import Callable
from dataclasses import dataclass, field
from heapq import heapify, heapify_max, heappop, heappop_max, heappush, heappush_max

from mpmath import mp, mpf

mp.prec = 113


@dataclass
class ziggurat_layer:
    x_inner: mpf
    x_outer: mpf
    y_min: mpf
    y_max: mpf


@dataclass(order=True)
class alias_table_entry:
    probability: mpf = mpf(0)
    index: int = field(default=0, compare=False)
    alias_index: int = field(init=False, compare=False)

    def __post_init__(self) -> None:
        self.alias_index = self.index


@dataclass(frozen=True)
class modified_ziggurat_traits:
    type_name: str
    type_suffix: str
    rand_bits: int
    layer_bits: int


def format_float(value: mpf, traits: modified_ziggurat_traits) -> str:
    if traits.type_name == "float":
        value = mpf(value, prec=24)
        for precision in range(1, 10):
            result = format(float(value), f".{precision}g")
            if mpf(result, prec=24) == value:
                break
    else:
        result = repr(float(value)).removesuffix(".0")

    significand, separator, exponent = result.partition("e")
    if "." not in significand:
        significand += "."
    return significand + separator + exponent + traits.type_suffix


def generate_tables(
    name: str,
    is_signed: bool,
    pdf: Callable[[mpf], mpf],
    inverse_pdf: Callable[[mpf], mpf],
    cdf: Callable[[mpf], mpf],
    pdf_derivative: Callable[[mpf], mpf],
    pdf_2nd_derivative: Callable[[mpf], mpf],
    height_scale: mpf = mpf(1),
    FloatType: str = "double",
) -> None:
    traits = {
        "double": modified_ziggurat_traits("double", "", 64, 8),
        "float": modified_ziggurat_traits("float", "f", 32, 7),  # 32 - 1 (sign) - 24 (magnitude)
    }[FloatType]
    layer_division = 1 << traits.layer_bits
    regular_area = mpf(1) / layer_division

    # the bottommost layer is excluded
    layers: list[ziggurat_layer] = []

    x_inner = mpf(0)
    y_max = pdf(x_inner)

    # minimize the total height of the top two layers
    if (y1 := pdf_derivative(mpf(0))) < 0:
        x_outer = mp.sqrt(regular_area / -y1)
    elif (y2 := pdf_2nd_derivative(mpf(0))) < 0:
        x_outer = mp.cbrt(regular_area / -y2)
    else:
        x_outer = mpf(1)

    while True:
        y1 = pdf_derivative(x_outer) + regular_area / (x_outer * x_outer)
        y2 = pdf_2nd_derivative(x_outer) - 2 * regular_area / (x_outer * x_outer * x_outer)
        dx = y1 / y2
        x_outer -= dx
        if not (abs(dx) > abs(x_outer) * mpf("0x1p-80")):
            break

    y_min = pdf(x_outer)

    # make sure that we have at most `layer_division` irregular regions
    # the bottommost layer has 2 irregular regions, other layers have 1 each
    while len(layers) + 2 < layer_division:
        layers.append(ziggurat_layer(x_inner=x_inner, x_outer=x_outer, y_min=y_min, y_max=y_max))

        x_inner = x_outer
        y_max = y_min
        y_min = y_max - regular_area / x_inner
        if y_min < 0:
            break

        x_outer = inverse_pdf(y_min)

    regular_layer_num = len(layers) - 1

    irregular_areas = [
        cdf(layer.x_outer) - cdf(layer.x_inner) - (layer.x_outer - layer.x_inner) * layer.y_min for layer in layers
    ]

    # the bottommost layer
    irregular_areas.append(1 - cdf(layers[-1].x_outer))
    irregular_areas.append(layers[-1].x_outer * layers[-1].y_min)

    # alias table for irregular region selection
    alias_table = [alias_table_entry() for _ in range(layer_division)]

    average_area = (layer_division - regular_layer_num) * regular_area / layer_division
    overs: list[alias_table_entry] = []
    unders: list[alias_table_entry] = []

    for i, area in enumerate(irregular_areas):
        probability = area / average_area
        entry = alias_table_entry(probability=probability, index=i)
        if probability > 1:
            overs.append(entry)
        elif probability < 1:
            unders.append(entry)
        else:
            alias_table[i] = entry

    for i in range(len(irregular_areas), layer_division):
        unders.append(alias_table_entry(probability=mpf(0), index=i))

    heapify(overs)
    heapify_max(unders)

    while overs and unders:
        over = heappop(overs)
        under = heappop_max(unders)
        alias_table[under.index].probability = under.probability
        alias_table[under.index].index = under.index
        alias_table[under.index].alias_index = over.index

        remaining = alias_table_entry(probability=over.probability - 1 + under.probability, index=over.index)
        if remaining.probability > 1:
            heappush(overs, remaining)
        elif remaining.probability < 1:
            heappush_max(unders, remaining)
        else:
            alias_table[remaining.index] = remaining

    while overs:
        entry = heappop(overs)
        alias_table[entry.index] = entry

    while unders:
        entry = heappop_max(unders)
        alias_table[entry.index] = entry

    # generate tables
    width_scale = mp.ldexp(2 if is_signed else 1, -traits.rand_bits)
    probability_scale = mp.ldexp(1, traits.rand_bits - traits.layer_bits)
    max_probability = 1 - 1 / probability_scale

    print()
    print("template <>")
    print(f"struct {name}<{traits.type_name}> {{")
    print(
        f"    static constexpr _Modified_ziggurat_tables<{traits.type_name}, uint{traits.rand_bits}_t, "
        f"{str(is_signed).lower()}, {traits.layer_bits}, {len(layers)}> _Value{{"
    )

    # _Ty _Layer_widths[_Layer_num + 1];
    widths = [layers[0].x_inner, *(layer.x_outer for layer in layers)]
    print(f"        {{{", ".join(format_float(x * width_scale, traits) for x in widths)}}},")

    # _Ty _Layer_heights[_Layer_num + 1];
    heights = [layers[0].y_max, *(layer.y_min for layer in layers)]
    print(f"        {{{", ".join(format_float(y * height_scale, traits) for y in heights)}}},")

    # _Uty _Alias_table[1 << _Lw];
    table_values = (
        (int(mp.nint(min(entry.probability, max_probability) * probability_scale)) << traits.layer_bits)
        | entry.alias_index
        for entry in alias_table
    )
    print(f"        {{{", ".join(f"{table_value:#x}u" for table_value in table_values)}}},")

    print("    };")
    print("};")


def sqrt_of_pi_over_2() -> mpf:
    return mpf("0x1.40d931ff627059657ca41fae722dp+0")


def inv_sqrt2() -> mpf:
    return mpf("0x1.6a09e667f3bcc908b2fb1366ea95p-1")


def sqrt_of_2_over_pi() -> mpf:
    return mpf("0x1.9884533d436508d0fcb3c500bab9p-1")


def half_normal_pdf(x: mpf) -> mpf:
    return mp.exp(x * x * -0.5) * sqrt_of_2_over_pi()


def half_normal_inverse_pdf(y: mpf) -> mpf:
    return mp.sqrt(-2 * mp.log(y * sqrt_of_pi_over_2()))


def half_normal_cdf(x: mpf) -> mpf:
    return mp.erf(x * inv_sqrt2())


def half_normal_pdf_derivative(x: mpf) -> mpf:
    return mp.exp(x * x * -0.5) * x * -sqrt_of_2_over_pi()


def half_normal_pdf_2nd_derivative(x: mpf) -> mpf:
    return mp.exp(x * x * -0.5) * (x + 1) * (x - 1) * sqrt_of_2_over_pi()


def generate_header() -> None:
    print("""// __msvc_random_ziggurat_tables.hpp internal header

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

template <class _Ty, class _Uty, bool _Signed, int _Lw, int _Lx>
struct _Modified_ziggurat_tables {
    static_assert(_Lw >= 2, "invalid table size");
    static_assert(_Lw < 31, "invalid table size");
    static_assert(_Lx >= 2, "invalid table size");
    static_assert(_Lx <= (1 << _Lw) - 2, "invalid table size");

    using _Uint_type = _Uty;
    using _Xtype     = conditional_t<_Signed, make_signed_t<_Uty>, _Uty>;

    static constexpr int _Layer_bits  = _Lw;
    static constexpr int _Layer_num   = _Lx;
    static constexpr _Ty _Width_scale = (_Signed ? _Ty{1} : _Ty{2}) * _Ty{static_cast<_Uty>(-1) / 2u + 1u};

    _Ty _Layer_widths[_Lx + 1];
    _Ty _Layer_heights[_Lx + 1];
    _Uty _Alias_table[1 << _Lw]; // lower _Layer_bits bits are indices, upper bits are probabilities
};""")


def generate_normal_distribution() -> None:
    print()
    print("template <class _Ty>")
    print("struct _Normal_distribution_tables;")

    generate_tables(
        "_Normal_distribution_tables", True, half_normal_pdf, half_normal_inverse_pdf,
        half_normal_cdf, half_normal_pdf_derivative, half_normal_pdf_2nd_derivative,
        sqrt_of_pi_over_2(), FloatType="double"
    )

    generate_tables(
        "_Normal_distribution_tables", True, half_normal_pdf, half_normal_inverse_pdf,
        half_normal_cdf, half_normal_pdf_derivative, half_normal_pdf_2nd_derivative,
        sqrt_of_pi_over_2(), FloatType="float"
    )


def generate_footer() -> None:
    print("""
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // __MSVC_RANDOM_ZIGGURAT_TABLES_HPP""")


def main() -> None:
    generate_header()
    generate_normal_distribution()
    generate_footer()


if __name__ == "__main__":
    main()
