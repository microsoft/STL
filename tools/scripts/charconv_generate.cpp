// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// The following code is the generator for the big powers of ten table found in _Multiply_by_power_of_ten().
// This code is provided for future use if the table needs to be amended. Do not remove this code.

#include <algorithm>
#include <charconv>
#include <cstdint>
#include <cstdio>
#include <tuple>
#include <vector>

using namespace std;

int main() {
    vector<uint32_t> elements;
    vector<tuple<uint32_t, uint32_t, uint32_t>> indices;

    for (uint32_t power = 10; power != 390; power += 10) {
        _Big_integer_flt big = _Make_big_integer_flt_one();

        for (uint32_t i = 0; i != power; ++i) {
            (void) _Multiply(big, 10); // assumes no overflow
        }

        const uint32_t* const first = big._Mydata;
        const uint32_t* const last  = first + big._Myused;
        const uint32_t* const mid   = find_if(first, last, [](const uint32_t elem) { return elem != 0; });

        indices.emplace_back(static_cast<uint32_t>(elements.size()), static_cast<uint32_t>(mid - first),
            static_cast<uint32_t>(last - mid));

        elements.insert(elements.end(), mid, last);
    }

    printf("static constexpr uint32_t _Large_power_data[] =\n{");
    for (uint32_t i = 0; i != elements.size(); ++i) {
        printf("%s0x%08x, ", i % 8 == 0 ? "\n\t" : "", elements[i]);
    }
    printf("\n};\n");

    printf("static constexpr _Unpack_index _Large_power_indices[] =\n{");
    for (uint32_t i = 0; i != indices.size(); ++i) {
        printf(
            "%s{ %u, %u, %u }, ", i % 6 == 0 ? "\n\t" : "", get<0>(indices[i]), get<1>(indices[i]), get<2>(indices[i]));
    }
    printf("\n};\n");
}
