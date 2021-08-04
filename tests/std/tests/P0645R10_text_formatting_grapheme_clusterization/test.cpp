// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <assert.h>
#include <filesystem>
#include <format>
#include <string_view>
#include <vector>

using namespace std;


struct test_case_data {
    std::vector<char32_t> code_points;
    std::vector<size_t> breaks;
};
const test_case_data test_data[] = {{{U'\x20', U'\x20'}, {0ull, 1ull, 2ull}},
    {{U'\x20', U'\x308', U'\x20'}, {0ull, 2ull, 3ull}}, {{U'\x20', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\x20', U'\x308', U'\xd'}, {0ull, 2ull, 3ull}}, {{U'\x20', U'\xa'}, {0ull, 1ull, 2ull}},
    {{U'\x20', U'\x308', U'\xa'}, {0ull, 2ull, 3ull}}, {{U'\x20', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\x20', U'\x308', U'\x1'}, {0ull, 2ull, 3ull}}, {{U'\x20', U'\x34f'}, {0ull, 2ull}},
    {{U'\x20', U'\x308', U'\x34f'}, {0ull, 3ull}}, {{U'\x20', U'\x1f1e6'}, {0ull, 1ull, 2ull}},
    {{U'\x20', U'\x308', U'\x1f1e6'}, {0ull, 2ull, 3ull}}, {{U'\x20', U'\x600'}, {0ull, 1ull, 2ull}},
    {{U'\x20', U'\x308', U'\x600'}, {0ull, 2ull, 3ull}}, {{U'\x20', U'\x903'}, {0ull, 2ull}},
    {{U'\x20', U'\x308', U'\x903'}, {0ull, 3ull}}, {{U'\x20', U'\x1100'}, {0ull, 1ull, 2ull}},
    {{U'\x20', U'\x308', U'\x1100'}, {0ull, 2ull, 3ull}}, {{U'\x20', U'\x1160'}, {0ull, 1ull, 2ull}},
    {{U'\x20', U'\x308', U'\x1160'}, {0ull, 2ull, 3ull}}, {{U'\x20', U'\x11a8'}, {0ull, 1ull, 2ull}},
    {{U'\x20', U'\x308', U'\x11a8'}, {0ull, 2ull, 3ull}}, {{U'\x20', U'\xac00'}, {0ull, 1ull, 2ull}},
    {{U'\x20', U'\x308', U'\xac00'}, {0ull, 2ull, 3ull}}, {{U'\x20', U'\xac01'}, {0ull, 1ull, 2ull}},
    {{U'\x20', U'\x308', U'\xac01'}, {0ull, 2ull, 3ull}}, {{U'\x20', U'\x231a'}, {0ull, 1ull, 2ull}},
    {{U'\x20', U'\x308', U'\x231a'}, {0ull, 2ull, 3ull}}, {{U'\x20', U'\x300'}, {0ull, 2ull}},
    {{U'\x20', U'\x308', U'\x300'}, {0ull, 3ull}}, {{U'\x20', U'\x200d'}, {0ull, 2ull}},
    {{U'\x20', U'\x308', U'\x200d'}, {0ull, 3ull}}, {{U'\x20', U'\x378'}, {0ull, 1ull, 2ull}},
    {{U'\x20', U'\x308', U'\x378'}, {0ull, 2ull, 3ull}}, {{U'\xd', U'\x20'}, {0ull, 1ull, 2ull}},
    {{U'\xd', U'\x308', U'\x20'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xd', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\xd', U'\x308', U'\xd'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xd', U'\xa'}, {0ull, 2ull}},
    {{U'\xd', U'\x308', U'\xa'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xd', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\xd', U'\x308', U'\x1'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xd', U'\x34f'}, {0ull, 1ull, 2ull}},
    {{U'\xd', U'\x308', U'\x34f'}, {0ull, 1ull, 3ull}}, {{U'\xd', U'\x1f1e6'}, {0ull, 1ull, 2ull}},
    {{U'\xd', U'\x308', U'\x1f1e6'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xd', U'\x600'}, {0ull, 1ull, 2ull}},
    {{U'\xd', U'\x308', U'\x600'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xd', U'\x903'}, {0ull, 1ull, 2ull}},
    {{U'\xd', U'\x308', U'\x903'}, {0ull, 1ull, 3ull}}, {{U'\xd', U'\x1100'}, {0ull, 1ull, 2ull}},
    {{U'\xd', U'\x308', U'\x1100'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xd', U'\x1160'}, {0ull, 1ull, 2ull}},
    {{U'\xd', U'\x308', U'\x1160'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xd', U'\x11a8'}, {0ull, 1ull, 2ull}},
    {{U'\xd', U'\x308', U'\x11a8'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xd', U'\xac00'}, {0ull, 1ull, 2ull}},
    {{U'\xd', U'\x308', U'\xac00'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xd', U'\xac01'}, {0ull, 1ull, 2ull}},
    {{U'\xd', U'\x308', U'\xac01'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xd', U'\x231a'}, {0ull, 1ull, 2ull}},
    {{U'\xd', U'\x308', U'\x231a'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xd', U'\x300'}, {0ull, 1ull, 2ull}},
    {{U'\xd', U'\x308', U'\x300'}, {0ull, 1ull, 3ull}}, {{U'\xd', U'\x200d'}, {0ull, 1ull, 2ull}},
    {{U'\xd', U'\x308', U'\x200d'}, {0ull, 1ull, 3ull}}, {{U'\xd', U'\x378'}, {0ull, 1ull, 2ull}},
    {{U'\xd', U'\x308', U'\x378'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xa', U'\x20'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\x20'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xa', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\xd'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xa', U'\xa'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\xa'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xa', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\x1'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xa', U'\x34f'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\x34f'}, {0ull, 1ull, 3ull}}, {{U'\xa', U'\x1f1e6'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\x1f1e6'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xa', U'\x600'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\x600'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xa', U'\x903'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\x903'}, {0ull, 1ull, 3ull}}, {{U'\xa', U'\x1100'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\x1100'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xa', U'\x1160'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\x1160'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xa', U'\x11a8'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\x11a8'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xa', U'\xac00'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\xac00'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xa', U'\xac01'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\xac01'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xa', U'\x231a'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\x231a'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\xa', U'\x300'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\x300'}, {0ull, 1ull, 3ull}}, {{U'\xa', U'\x200d'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\x200d'}, {0ull, 1ull, 3ull}}, {{U'\xa', U'\x378'}, {0ull, 1ull, 2ull}},
    {{U'\xa', U'\x308', U'\x378'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\x1', U'\x20'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\x20'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\x1', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\xd'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\x1', U'\xa'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\xa'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\x1', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\x1'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\x1', U'\x34f'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\x34f'}, {0ull, 1ull, 3ull}}, {{U'\x1', U'\x1f1e6'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\x1f1e6'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\x1', U'\x600'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\x600'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\x1', U'\x903'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\x903'}, {0ull, 1ull, 3ull}}, {{U'\x1', U'\x1100'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\x1100'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\x1', U'\x1160'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\x1160'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\x1', U'\x11a8'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\x11a8'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\x1', U'\xac00'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\xac00'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\x1', U'\xac01'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\xac01'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\x1', U'\x231a'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\x231a'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\x1', U'\x300'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\x300'}, {0ull, 1ull, 3ull}}, {{U'\x1', U'\x200d'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\x200d'}, {0ull, 1ull, 3ull}}, {{U'\x1', U'\x378'}, {0ull, 1ull, 2ull}},
    {{U'\x1', U'\x308', U'\x378'}, {0ull, 1ull, 2ull, 3ull}}, {{U'\x34f', U'\x20'}, {0ull, 1ull, 2ull}},
    {{U'\x34f', U'\x308', U'\x20'}, {0ull, 2ull, 3ull}}, {{U'\x34f', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\x34f', U'\x308', U'\xd'}, {0ull, 2ull, 3ull}}, {{U'\x34f', U'\xa'}, {0ull, 1ull, 2ull}},
    {{U'\x34f', U'\x308', U'\xa'}, {0ull, 2ull, 3ull}}, {{U'\x34f', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\x34f', U'\x308', U'\x1'}, {0ull, 2ull, 3ull}}, {{U'\x34f', U'\x34f'}, {0ull, 2ull}},
    {{U'\x34f', U'\x308', U'\x34f'}, {0ull, 3ull}}, {{U'\x34f', U'\x1f1e6'}, {0ull, 1ull, 2ull}},
    {{U'\x34f', U'\x308', U'\x1f1e6'}, {0ull, 2ull, 3ull}}, {{U'\x34f', U'\x600'}, {0ull, 1ull, 2ull}},
    {{U'\x34f', U'\x308', U'\x600'}, {0ull, 2ull, 3ull}}, {{U'\x34f', U'\x903'}, {0ull, 2ull}},
    {{U'\x34f', U'\x308', U'\x903'}, {0ull, 3ull}}, {{U'\x34f', U'\x1100'}, {0ull, 1ull, 2ull}},
    {{U'\x34f', U'\x308', U'\x1100'}, {0ull, 2ull, 3ull}}, {{U'\x34f', U'\x1160'}, {0ull, 1ull, 2ull}},
    {{U'\x34f', U'\x308', U'\x1160'}, {0ull, 2ull, 3ull}}, {{U'\x34f', U'\x11a8'}, {0ull, 1ull, 2ull}},
    {{U'\x34f', U'\x308', U'\x11a8'}, {0ull, 2ull, 3ull}}, {{U'\x34f', U'\xac00'}, {0ull, 1ull, 2ull}},
    {{U'\x34f', U'\x308', U'\xac00'}, {0ull, 2ull, 3ull}}, {{U'\x34f', U'\xac01'}, {0ull, 1ull, 2ull}},
    {{U'\x34f', U'\x308', U'\xac01'}, {0ull, 2ull, 3ull}}, {{U'\x34f', U'\x231a'}, {0ull, 1ull, 2ull}},
    {{U'\x34f', U'\x308', U'\x231a'}, {0ull, 2ull, 3ull}}, {{U'\x34f', U'\x300'}, {0ull, 2ull}},
    {{U'\x34f', U'\x308', U'\x300'}, {0ull, 3ull}}, {{U'\x34f', U'\x200d'}, {0ull, 2ull}},
    {{U'\x34f', U'\x308', U'\x200d'}, {0ull, 3ull}}, {{U'\x34f', U'\x378'}, {0ull, 1ull, 2ull}},
    {{U'\x34f', U'\x308', U'\x378'}, {0ull, 2ull, 3ull}}, {{U'\x1f1e6', U'\x20'}, {0ull, 1ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\x20'}, {0ull, 2ull, 3ull}}, {{U'\x1f1e6', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\xd'}, {0ull, 2ull, 3ull}}, {{U'\x1f1e6', U'\xa'}, {0ull, 1ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\xa'}, {0ull, 2ull, 3ull}}, {{U'\x1f1e6', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\x1'}, {0ull, 2ull, 3ull}}, {{U'\x1f1e6', U'\x34f'}, {0ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\x34f'}, {0ull, 3ull}}, {{U'\x1f1e6', U'\x1f1e6'}, {0ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\x1f1e6'}, {0ull, 2ull, 3ull}}, {{U'\x1f1e6', U'\x600'}, {0ull, 1ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\x600'}, {0ull, 2ull, 3ull}}, {{U'\x1f1e6', U'\x903'}, {0ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\x903'}, {0ull, 3ull}}, {{U'\x1f1e6', U'\x1100'}, {0ull, 1ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\x1100'}, {0ull, 2ull, 3ull}}, {{U'\x1f1e6', U'\x1160'}, {0ull, 1ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\x1160'}, {0ull, 2ull, 3ull}}, {{U'\x1f1e6', U'\x11a8'}, {0ull, 1ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\x11a8'}, {0ull, 2ull, 3ull}}, {{U'\x1f1e6', U'\xac00'}, {0ull, 1ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\xac00'}, {0ull, 2ull, 3ull}}, {{U'\x1f1e6', U'\xac01'}, {0ull, 1ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\xac01'}, {0ull, 2ull, 3ull}}, {{U'\x1f1e6', U'\x231a'}, {0ull, 1ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\x231a'}, {0ull, 2ull, 3ull}}, {{U'\x1f1e6', U'\x300'}, {0ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\x300'}, {0ull, 3ull}}, {{U'\x1f1e6', U'\x200d'}, {0ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\x200d'}, {0ull, 3ull}}, {{U'\x1f1e6', U'\x378'}, {0ull, 1ull, 2ull}},
    {{U'\x1f1e6', U'\x308', U'\x378'}, {0ull, 2ull, 3ull}}, {{U'\x600', U'\x20'}, {0ull, 2ull}},
    {{U'\x600', U'\x308', U'\x20'}, {0ull, 2ull, 3ull}}, {{U'\x600', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\x600', U'\x308', U'\xd'}, {0ull, 2ull, 3ull}}, {{U'\x600', U'\xa'}, {0ull, 1ull, 2ull}},
    {{U'\x600', U'\x308', U'\xa'}, {0ull, 2ull, 3ull}}, {{U'\x600', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\x600', U'\x308', U'\x1'}, {0ull, 2ull, 3ull}}, {{U'\x600', U'\x34f'}, {0ull, 2ull}},
    {{U'\x600', U'\x308', U'\x34f'}, {0ull, 3ull}}, {{U'\x600', U'\x1f1e6'}, {0ull, 2ull}},
    {{U'\x600', U'\x308', U'\x1f1e6'}, {0ull, 2ull, 3ull}}, {{U'\x600', U'\x600'}, {0ull, 2ull}},
    {{U'\x600', U'\x308', U'\x600'}, {0ull, 2ull, 3ull}}, {{U'\x600', U'\x903'}, {0ull, 2ull}},
    {{U'\x600', U'\x308', U'\x903'}, {0ull, 3ull}}, {{U'\x600', U'\x1100'}, {0ull, 2ull}},
    {{U'\x600', U'\x308', U'\x1100'}, {0ull, 2ull, 3ull}}, {{U'\x600', U'\x1160'}, {0ull, 2ull}},
    {{U'\x600', U'\x308', U'\x1160'}, {0ull, 2ull, 3ull}}, {{U'\x600', U'\x11a8'}, {0ull, 2ull}},
    {{U'\x600', U'\x308', U'\x11a8'}, {0ull, 2ull, 3ull}}, {{U'\x600', U'\xac00'}, {0ull, 2ull}},
    {{U'\x600', U'\x308', U'\xac00'}, {0ull, 2ull, 3ull}}, {{U'\x600', U'\xac01'}, {0ull, 2ull}},
    {{U'\x600', U'\x308', U'\xac01'}, {0ull, 2ull, 3ull}}, {{U'\x600', U'\x231a'}, {0ull, 2ull}},
    {{U'\x600', U'\x308', U'\x231a'}, {0ull, 2ull, 3ull}}, {{U'\x600', U'\x300'}, {0ull, 2ull}},
    {{U'\x600', U'\x308', U'\x300'}, {0ull, 3ull}}, {{U'\x600', U'\x200d'}, {0ull, 2ull}},
    {{U'\x600', U'\x308', U'\x200d'}, {0ull, 3ull}}, {{U'\x600', U'\x378'}, {0ull, 2ull}},
    {{U'\x600', U'\x308', U'\x378'}, {0ull, 2ull, 3ull}}, {{U'\x903', U'\x20'}, {0ull, 1ull, 2ull}},
    {{U'\x903', U'\x308', U'\x20'}, {0ull, 2ull, 3ull}}, {{U'\x903', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\x903', U'\x308', U'\xd'}, {0ull, 2ull, 3ull}}, {{U'\x903', U'\xa'}, {0ull, 1ull, 2ull}},
    {{U'\x903', U'\x308', U'\xa'}, {0ull, 2ull, 3ull}}, {{U'\x903', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\x903', U'\x308', U'\x1'}, {0ull, 2ull, 3ull}}, {{U'\x903', U'\x34f'}, {0ull, 2ull}},
    {{U'\x903', U'\x308', U'\x34f'}, {0ull, 3ull}}, {{U'\x903', U'\x1f1e6'}, {0ull, 1ull, 2ull}},
    {{U'\x903', U'\x308', U'\x1f1e6'}, {0ull, 2ull, 3ull}}, {{U'\x903', U'\x600'}, {0ull, 1ull, 2ull}},
    {{U'\x903', U'\x308', U'\x600'}, {0ull, 2ull, 3ull}}, {{U'\x903', U'\x903'}, {0ull, 2ull}},
    {{U'\x903', U'\x308', U'\x903'}, {0ull, 3ull}}, {{U'\x903', U'\x1100'}, {0ull, 1ull, 2ull}},
    {{U'\x903', U'\x308', U'\x1100'}, {0ull, 2ull, 3ull}}, {{U'\x903', U'\x1160'}, {0ull, 1ull, 2ull}},
    {{U'\x903', U'\x308', U'\x1160'}, {0ull, 2ull, 3ull}}, {{U'\x903', U'\x11a8'}, {0ull, 1ull, 2ull}},
    {{U'\x903', U'\x308', U'\x11a8'}, {0ull, 2ull, 3ull}}, {{U'\x903', U'\xac00'}, {0ull, 1ull, 2ull}},
    {{U'\x903', U'\x308', U'\xac00'}, {0ull, 2ull, 3ull}}, {{U'\x903', U'\xac01'}, {0ull, 1ull, 2ull}},
    {{U'\x903', U'\x308', U'\xac01'}, {0ull, 2ull, 3ull}}, {{U'\x903', U'\x231a'}, {0ull, 1ull, 2ull}},
    {{U'\x903', U'\x308', U'\x231a'}, {0ull, 2ull, 3ull}}, {{U'\x903', U'\x300'}, {0ull, 2ull}},
    {{U'\x903', U'\x308', U'\x300'}, {0ull, 3ull}}, {{U'\x903', U'\x200d'}, {0ull, 2ull}},
    {{U'\x903', U'\x308', U'\x200d'}, {0ull, 3ull}}, {{U'\x903', U'\x378'}, {0ull, 1ull, 2ull}},
    {{U'\x903', U'\x308', U'\x378'}, {0ull, 2ull, 3ull}}, {{U'\x1100', U'\x20'}, {0ull, 1ull, 2ull}},
    {{U'\x1100', U'\x308', U'\x20'}, {0ull, 2ull, 3ull}}, {{U'\x1100', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\x1100', U'\x308', U'\xd'}, {0ull, 2ull, 3ull}}, {{U'\x1100', U'\xa'}, {0ull, 1ull, 2ull}},
    {{U'\x1100', U'\x308', U'\xa'}, {0ull, 2ull, 3ull}}, {{U'\x1100', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\x1100', U'\x308', U'\x1'}, {0ull, 2ull, 3ull}}, {{U'\x1100', U'\x34f'}, {0ull, 2ull}},
    {{U'\x1100', U'\x308', U'\x34f'}, {0ull, 3ull}}, {{U'\x1100', U'\x1f1e6'}, {0ull, 1ull, 2ull}},
    {{U'\x1100', U'\x308', U'\x1f1e6'}, {0ull, 2ull, 3ull}}, {{U'\x1100', U'\x600'}, {0ull, 1ull, 2ull}},
    {{U'\x1100', U'\x308', U'\x600'}, {0ull, 2ull, 3ull}}, {{U'\x1100', U'\x903'}, {0ull, 2ull}},
    {{U'\x1100', U'\x308', U'\x903'}, {0ull, 3ull}}, {{U'\x1100', U'\x1100'}, {0ull, 2ull}},
    {{U'\x1100', U'\x308', U'\x1100'}, {0ull, 2ull, 3ull}}, {{U'\x1100', U'\x1160'}, {0ull, 2ull}},
    {{U'\x1100', U'\x308', U'\x1160'}, {0ull, 2ull, 3ull}}, {{U'\x1100', U'\x11a8'}, {0ull, 1ull, 2ull}},
    {{U'\x1100', U'\x308', U'\x11a8'}, {0ull, 2ull, 3ull}}, {{U'\x1100', U'\xac00'}, {0ull, 2ull}},
    {{U'\x1100', U'\x308', U'\xac00'}, {0ull, 2ull, 3ull}}, {{U'\x1100', U'\xac01'}, {0ull, 2ull}},
    {{U'\x1100', U'\x308', U'\xac01'}, {0ull, 2ull, 3ull}}, {{U'\x1100', U'\x231a'}, {0ull, 1ull, 2ull}},
    {{U'\x1100', U'\x308', U'\x231a'}, {0ull, 2ull, 3ull}}, {{U'\x1100', U'\x300'}, {0ull, 2ull}},
    {{U'\x1100', U'\x308', U'\x300'}, {0ull, 3ull}}, {{U'\x1100', U'\x200d'}, {0ull, 2ull}},
    {{U'\x1100', U'\x308', U'\x200d'}, {0ull, 3ull}}, {{U'\x1100', U'\x378'}, {0ull, 1ull, 2ull}},
    {{U'\x1100', U'\x308', U'\x378'}, {0ull, 2ull, 3ull}}, {{U'\x1160', U'\x20'}, {0ull, 1ull, 2ull}},
    {{U'\x1160', U'\x308', U'\x20'}, {0ull, 2ull, 3ull}}, {{U'\x1160', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\x1160', U'\x308', U'\xd'}, {0ull, 2ull, 3ull}}, {{U'\x1160', U'\xa'}, {0ull, 1ull, 2ull}},
    {{U'\x1160', U'\x308', U'\xa'}, {0ull, 2ull, 3ull}}, {{U'\x1160', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\x1160', U'\x308', U'\x1'}, {0ull, 2ull, 3ull}}, {{U'\x1160', U'\x34f'}, {0ull, 2ull}},
    {{U'\x1160', U'\x308', U'\x34f'}, {0ull, 3ull}}, {{U'\x1160', U'\x1f1e6'}, {0ull, 1ull, 2ull}},
    {{U'\x1160', U'\x308', U'\x1f1e6'}, {0ull, 2ull, 3ull}}, {{U'\x1160', U'\x600'}, {0ull, 1ull, 2ull}},
    {{U'\x1160', U'\x308', U'\x600'}, {0ull, 2ull, 3ull}}, {{U'\x1160', U'\x903'}, {0ull, 2ull}},
    {{U'\x1160', U'\x308', U'\x903'}, {0ull, 3ull}}, {{U'\x1160', U'\x1100'}, {0ull, 1ull, 2ull}},
    {{U'\x1160', U'\x308', U'\x1100'}, {0ull, 2ull, 3ull}}, {{U'\x1160', U'\x1160'}, {0ull, 2ull}},
    {{U'\x1160', U'\x308', U'\x1160'}, {0ull, 2ull, 3ull}}, {{U'\x1160', U'\x11a8'}, {0ull, 2ull}},
    {{U'\x1160', U'\x308', U'\x11a8'}, {0ull, 2ull, 3ull}}, {{U'\x1160', U'\xac00'}, {0ull, 1ull, 2ull}},
    {{U'\x1160', U'\x308', U'\xac00'}, {0ull, 2ull, 3ull}}, {{U'\x1160', U'\xac01'}, {0ull, 1ull, 2ull}},
    {{U'\x1160', U'\x308', U'\xac01'}, {0ull, 2ull, 3ull}}, {{U'\x1160', U'\x231a'}, {0ull, 1ull, 2ull}},
    {{U'\x1160', U'\x308', U'\x231a'}, {0ull, 2ull, 3ull}}, {{U'\x1160', U'\x300'}, {0ull, 2ull}},
    {{U'\x1160', U'\x308', U'\x300'}, {0ull, 3ull}}, {{U'\x1160', U'\x200d'}, {0ull, 2ull}},
    {{U'\x1160', U'\x308', U'\x200d'}, {0ull, 3ull}}, {{U'\x1160', U'\x378'}, {0ull, 1ull, 2ull}},
    {{U'\x1160', U'\x308', U'\x378'}, {0ull, 2ull, 3ull}}, {{U'\x11a8', U'\x20'}, {0ull, 1ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\x20'}, {0ull, 2ull, 3ull}}, {{U'\x11a8', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\xd'}, {0ull, 2ull, 3ull}}, {{U'\x11a8', U'\xa'}, {0ull, 1ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\xa'}, {0ull, 2ull, 3ull}}, {{U'\x11a8', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\x1'}, {0ull, 2ull, 3ull}}, {{U'\x11a8', U'\x34f'}, {0ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\x34f'}, {0ull, 3ull}}, {{U'\x11a8', U'\x1f1e6'}, {0ull, 1ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\x1f1e6'}, {0ull, 2ull, 3ull}}, {{U'\x11a8', U'\x600'}, {0ull, 1ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\x600'}, {0ull, 2ull, 3ull}}, {{U'\x11a8', U'\x903'}, {0ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\x903'}, {0ull, 3ull}}, {{U'\x11a8', U'\x1100'}, {0ull, 1ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\x1100'}, {0ull, 2ull, 3ull}}, {{U'\x11a8', U'\x1160'}, {0ull, 1ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\x1160'}, {0ull, 2ull, 3ull}}, {{U'\x11a8', U'\x11a8'}, {0ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\x11a8'}, {0ull, 2ull, 3ull}}, {{U'\x11a8', U'\xac00'}, {0ull, 1ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\xac00'}, {0ull, 2ull, 3ull}}, {{U'\x11a8', U'\xac01'}, {0ull, 1ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\xac01'}, {0ull, 2ull, 3ull}}, {{U'\x11a8', U'\x231a'}, {0ull, 1ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\x231a'}, {0ull, 2ull, 3ull}}, {{U'\x11a8', U'\x300'}, {0ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\x300'}, {0ull, 3ull}}, {{U'\x11a8', U'\x200d'}, {0ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\x200d'}, {0ull, 3ull}}, {{U'\x11a8', U'\x378'}, {0ull, 1ull, 2ull}},
    {{U'\x11a8', U'\x308', U'\x378'}, {0ull, 2ull, 3ull}}, {{U'\xac00', U'\x20'}, {0ull, 1ull, 2ull}},
    {{U'\xac00', U'\x308', U'\x20'}, {0ull, 2ull, 3ull}}, {{U'\xac00', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\xac00', U'\x308', U'\xd'}, {0ull, 2ull, 3ull}}, {{U'\xac00', U'\xa'}, {0ull, 1ull, 2ull}},
    {{U'\xac00', U'\x308', U'\xa'}, {0ull, 2ull, 3ull}}, {{U'\xac00', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\xac00', U'\x308', U'\x1'}, {0ull, 2ull, 3ull}}, {{U'\xac00', U'\x34f'}, {0ull, 2ull}},
    {{U'\xac00', U'\x308', U'\x34f'}, {0ull, 3ull}}, {{U'\xac00', U'\x1f1e6'}, {0ull, 1ull, 2ull}},
    {{U'\xac00', U'\x308', U'\x1f1e6'}, {0ull, 2ull, 3ull}}, {{U'\xac00', U'\x600'}, {0ull, 1ull, 2ull}},
    {{U'\xac00', U'\x308', U'\x600'}, {0ull, 2ull, 3ull}}, {{U'\xac00', U'\x903'}, {0ull, 2ull}},
    {{U'\xac00', U'\x308', U'\x903'}, {0ull, 3ull}}, {{U'\xac00', U'\x1100'}, {0ull, 1ull, 2ull}},
    {{U'\xac00', U'\x308', U'\x1100'}, {0ull, 2ull, 3ull}}, {{U'\xac00', U'\x1160'}, {0ull, 2ull}},
    {{U'\xac00', U'\x308', U'\x1160'}, {0ull, 2ull, 3ull}}, {{U'\xac00', U'\x11a8'}, {0ull, 2ull}},
    {{U'\xac00', U'\x308', U'\x11a8'}, {0ull, 2ull, 3ull}}, {{U'\xac00', U'\xac00'}, {0ull, 1ull, 2ull}},
    {{U'\xac00', U'\x308', U'\xac00'}, {0ull, 2ull, 3ull}}, {{U'\xac00', U'\xac01'}, {0ull, 1ull, 2ull}},
    {{U'\xac00', U'\x308', U'\xac01'}, {0ull, 2ull, 3ull}}, {{U'\xac00', U'\x231a'}, {0ull, 1ull, 2ull}},
    {{U'\xac00', U'\x308', U'\x231a'}, {0ull, 2ull, 3ull}}, {{U'\xac00', U'\x300'}, {0ull, 2ull}},
    {{U'\xac00', U'\x308', U'\x300'}, {0ull, 3ull}}, {{U'\xac00', U'\x200d'}, {0ull, 2ull}},
    {{U'\xac00', U'\x308', U'\x200d'}, {0ull, 3ull}}, {{U'\xac00', U'\x378'}, {0ull, 1ull, 2ull}},
    {{U'\xac00', U'\x308', U'\x378'}, {0ull, 2ull, 3ull}}, {{U'\xac01', U'\x20'}, {0ull, 1ull, 2ull}},
    {{U'\xac01', U'\x308', U'\x20'}, {0ull, 2ull, 3ull}}, {{U'\xac01', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\xac01', U'\x308', U'\xd'}, {0ull, 2ull, 3ull}}, {{U'\xac01', U'\xa'}, {0ull, 1ull, 2ull}},
    {{U'\xac01', U'\x308', U'\xa'}, {0ull, 2ull, 3ull}}, {{U'\xac01', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\xac01', U'\x308', U'\x1'}, {0ull, 2ull, 3ull}}, {{U'\xac01', U'\x34f'}, {0ull, 2ull}},
    {{U'\xac01', U'\x308', U'\x34f'}, {0ull, 3ull}}, {{U'\xac01', U'\x1f1e6'}, {0ull, 1ull, 2ull}},
    {{U'\xac01', U'\x308', U'\x1f1e6'}, {0ull, 2ull, 3ull}}, {{U'\xac01', U'\x600'}, {0ull, 1ull, 2ull}},
    {{U'\xac01', U'\x308', U'\x600'}, {0ull, 2ull, 3ull}}, {{U'\xac01', U'\x903'}, {0ull, 2ull}},
    {{U'\xac01', U'\x308', U'\x903'}, {0ull, 3ull}}, {{U'\xac01', U'\x1100'}, {0ull, 1ull, 2ull}},
    {{U'\xac01', U'\x308', U'\x1100'}, {0ull, 2ull, 3ull}}, {{U'\xac01', U'\x1160'}, {0ull, 1ull, 2ull}},
    {{U'\xac01', U'\x308', U'\x1160'}, {0ull, 2ull, 3ull}}, {{U'\xac01', U'\x11a8'}, {0ull, 2ull}},
    {{U'\xac01', U'\x308', U'\x11a8'}, {0ull, 2ull, 3ull}}, {{U'\xac01', U'\xac00'}, {0ull, 1ull, 2ull}},
    {{U'\xac01', U'\x308', U'\xac00'}, {0ull, 2ull, 3ull}}, {{U'\xac01', U'\xac01'}, {0ull, 1ull, 2ull}},
    {{U'\xac01', U'\x308', U'\xac01'}, {0ull, 2ull, 3ull}}, {{U'\xac01', U'\x231a'}, {0ull, 1ull, 2ull}},
    {{U'\xac01', U'\x308', U'\x231a'}, {0ull, 2ull, 3ull}}, {{U'\xac01', U'\x300'}, {0ull, 2ull}},
    {{U'\xac01', U'\x308', U'\x300'}, {0ull, 3ull}}, {{U'\xac01', U'\x200d'}, {0ull, 2ull}},
    {{U'\xac01', U'\x308', U'\x200d'}, {0ull, 3ull}}, {{U'\xac01', U'\x378'}, {0ull, 1ull, 2ull}},
    {{U'\xac01', U'\x308', U'\x378'}, {0ull, 2ull, 3ull}}, {{U'\x231a', U'\x20'}, {0ull, 1ull, 2ull}},
    {{U'\x231a', U'\x308', U'\x20'}, {0ull, 2ull, 3ull}}, {{U'\x231a', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\x231a', U'\x308', U'\xd'}, {0ull, 2ull, 3ull}}, {{U'\x231a', U'\xa'}, {0ull, 1ull, 2ull}},
    {{U'\x231a', U'\x308', U'\xa'}, {0ull, 2ull, 3ull}}, {{U'\x231a', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\x231a', U'\x308', U'\x1'}, {0ull, 2ull, 3ull}}, {{U'\x231a', U'\x34f'}, {0ull, 2ull}},
    {{U'\x231a', U'\x308', U'\x34f'}, {0ull, 3ull}}, {{U'\x231a', U'\x1f1e6'}, {0ull, 1ull, 2ull}},
    {{U'\x231a', U'\x308', U'\x1f1e6'}, {0ull, 2ull, 3ull}}, {{U'\x231a', U'\x600'}, {0ull, 1ull, 2ull}},
    {{U'\x231a', U'\x308', U'\x600'}, {0ull, 2ull, 3ull}}, {{U'\x231a', U'\x903'}, {0ull, 2ull}},
    {{U'\x231a', U'\x308', U'\x903'}, {0ull, 3ull}}, {{U'\x231a', U'\x1100'}, {0ull, 1ull, 2ull}},
    {{U'\x231a', U'\x308', U'\x1100'}, {0ull, 2ull, 3ull}}, {{U'\x231a', U'\x1160'}, {0ull, 1ull, 2ull}},
    {{U'\x231a', U'\x308', U'\x1160'}, {0ull, 2ull, 3ull}}, {{U'\x231a', U'\x11a8'}, {0ull, 1ull, 2ull}},
    {{U'\x231a', U'\x308', U'\x11a8'}, {0ull, 2ull, 3ull}}, {{U'\x231a', U'\xac00'}, {0ull, 1ull, 2ull}},
    {{U'\x231a', U'\x308', U'\xac00'}, {0ull, 2ull, 3ull}}, {{U'\x231a', U'\xac01'}, {0ull, 1ull, 2ull}},
    {{U'\x231a', U'\x308', U'\xac01'}, {0ull, 2ull, 3ull}}, {{U'\x231a', U'\x231a'}, {0ull, 1ull, 2ull}},
    {{U'\x231a', U'\x308', U'\x231a'}, {0ull, 2ull, 3ull}}, {{U'\x231a', U'\x300'}, {0ull, 2ull}},
    {{U'\x231a', U'\x308', U'\x300'}, {0ull, 3ull}}, {{U'\x231a', U'\x200d'}, {0ull, 2ull}},
    {{U'\x231a', U'\x308', U'\x200d'}, {0ull, 3ull}}, {{U'\x231a', U'\x378'}, {0ull, 1ull, 2ull}},
    {{U'\x231a', U'\x308', U'\x378'}, {0ull, 2ull, 3ull}}, {{U'\x300', U'\x20'}, {0ull, 1ull, 2ull}},
    {{U'\x300', U'\x308', U'\x20'}, {0ull, 2ull, 3ull}}, {{U'\x300', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\x300', U'\x308', U'\xd'}, {0ull, 2ull, 3ull}}, {{U'\x300', U'\xa'}, {0ull, 1ull, 2ull}},
    {{U'\x300', U'\x308', U'\xa'}, {0ull, 2ull, 3ull}}, {{U'\x300', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\x300', U'\x308', U'\x1'}, {0ull, 2ull, 3ull}}, {{U'\x300', U'\x34f'}, {0ull, 2ull}},
    {{U'\x300', U'\x308', U'\x34f'}, {0ull, 3ull}}, {{U'\x300', U'\x1f1e6'}, {0ull, 1ull, 2ull}},
    {{U'\x300', U'\x308', U'\x1f1e6'}, {0ull, 2ull, 3ull}}, {{U'\x300', U'\x600'}, {0ull, 1ull, 2ull}},
    {{U'\x300', U'\x308', U'\x600'}, {0ull, 2ull, 3ull}}, {{U'\x300', U'\x903'}, {0ull, 2ull}},
    {{U'\x300', U'\x308', U'\x903'}, {0ull, 3ull}}, {{U'\x300', U'\x1100'}, {0ull, 1ull, 2ull}},
    {{U'\x300', U'\x308', U'\x1100'}, {0ull, 2ull, 3ull}}, {{U'\x300', U'\x1160'}, {0ull, 1ull, 2ull}},
    {{U'\x300', U'\x308', U'\x1160'}, {0ull, 2ull, 3ull}}, {{U'\x300', U'\x11a8'}, {0ull, 1ull, 2ull}},
    {{U'\x300', U'\x308', U'\x11a8'}, {0ull, 2ull, 3ull}}, {{U'\x300', U'\xac00'}, {0ull, 1ull, 2ull}},
    {{U'\x300', U'\x308', U'\xac00'}, {0ull, 2ull, 3ull}}, {{U'\x300', U'\xac01'}, {0ull, 1ull, 2ull}},
    {{U'\x300', U'\x308', U'\xac01'}, {0ull, 2ull, 3ull}}, {{U'\x300', U'\x231a'}, {0ull, 1ull, 2ull}},
    {{U'\x300', U'\x308', U'\x231a'}, {0ull, 2ull, 3ull}}, {{U'\x300', U'\x300'}, {0ull, 2ull}},
    {{U'\x300', U'\x308', U'\x300'}, {0ull, 3ull}}, {{U'\x300', U'\x200d'}, {0ull, 2ull}},
    {{U'\x300', U'\x308', U'\x200d'}, {0ull, 3ull}}, {{U'\x300', U'\x378'}, {0ull, 1ull, 2ull}},
    {{U'\x300', U'\x308', U'\x378'}, {0ull, 2ull, 3ull}}, {{U'\x200d', U'\x20'}, {0ull, 1ull, 2ull}},
    {{U'\x200d', U'\x308', U'\x20'}, {0ull, 2ull, 3ull}}, {{U'\x200d', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\x200d', U'\x308', U'\xd'}, {0ull, 2ull, 3ull}}, {{U'\x200d', U'\xa'}, {0ull, 1ull, 2ull}},
    {{U'\x200d', U'\x308', U'\xa'}, {0ull, 2ull, 3ull}}, {{U'\x200d', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\x200d', U'\x308', U'\x1'}, {0ull, 2ull, 3ull}}, {{U'\x200d', U'\x34f'}, {0ull, 2ull}},
    {{U'\x200d', U'\x308', U'\x34f'}, {0ull, 3ull}}, {{U'\x200d', U'\x1f1e6'}, {0ull, 1ull, 2ull}},
    {{U'\x200d', U'\x308', U'\x1f1e6'}, {0ull, 2ull, 3ull}}, {{U'\x200d', U'\x600'}, {0ull, 1ull, 2ull}},
    {{U'\x200d', U'\x308', U'\x600'}, {0ull, 2ull, 3ull}}, {{U'\x200d', U'\x903'}, {0ull, 2ull}},
    {{U'\x200d', U'\x308', U'\x903'}, {0ull, 3ull}}, {{U'\x200d', U'\x1100'}, {0ull, 1ull, 2ull}},
    {{U'\x200d', U'\x308', U'\x1100'}, {0ull, 2ull, 3ull}}, {{U'\x200d', U'\x1160'}, {0ull, 1ull, 2ull}},
    {{U'\x200d', U'\x308', U'\x1160'}, {0ull, 2ull, 3ull}}, {{U'\x200d', U'\x11a8'}, {0ull, 1ull, 2ull}},
    {{U'\x200d', U'\x308', U'\x11a8'}, {0ull, 2ull, 3ull}}, {{U'\x200d', U'\xac00'}, {0ull, 1ull, 2ull}},
    {{U'\x200d', U'\x308', U'\xac00'}, {0ull, 2ull, 3ull}}, {{U'\x200d', U'\xac01'}, {0ull, 1ull, 2ull}},
    {{U'\x200d', U'\x308', U'\xac01'}, {0ull, 2ull, 3ull}}, {{U'\x200d', U'\x231a'}, {0ull, 1ull, 2ull}},
    {{U'\x200d', U'\x308', U'\x231a'}, {0ull, 2ull, 3ull}}, {{U'\x200d', U'\x300'}, {0ull, 2ull}},
    {{U'\x200d', U'\x308', U'\x300'}, {0ull, 3ull}}, {{U'\x200d', U'\x200d'}, {0ull, 2ull}},
    {{U'\x200d', U'\x308', U'\x200d'}, {0ull, 3ull}}, {{U'\x200d', U'\x378'}, {0ull, 1ull, 2ull}},
    {{U'\x200d', U'\x308', U'\x378'}, {0ull, 2ull, 3ull}}, {{U'\x378', U'\x20'}, {0ull, 1ull, 2ull}},
    {{U'\x378', U'\x308', U'\x20'}, {0ull, 2ull, 3ull}}, {{U'\x378', U'\xd'}, {0ull, 1ull, 2ull}},
    {{U'\x378', U'\x308', U'\xd'}, {0ull, 2ull, 3ull}}, {{U'\x378', U'\xa'}, {0ull, 1ull, 2ull}},
    {{U'\x378', U'\x308', U'\xa'}, {0ull, 2ull, 3ull}}, {{U'\x378', U'\x1'}, {0ull, 1ull, 2ull}},
    {{U'\x378', U'\x308', U'\x1'}, {0ull, 2ull, 3ull}}, {{U'\x378', U'\x34f'}, {0ull, 2ull}},
    {{U'\x378', U'\x308', U'\x34f'}, {0ull, 3ull}}, {{U'\x378', U'\x1f1e6'}, {0ull, 1ull, 2ull}},
    {{U'\x378', U'\x308', U'\x1f1e6'}, {0ull, 2ull, 3ull}}, {{U'\x378', U'\x600'}, {0ull, 1ull, 2ull}},
    {{U'\x378', U'\x308', U'\x600'}, {0ull, 2ull, 3ull}}, {{U'\x378', U'\x903'}, {0ull, 2ull}},
    {{U'\x378', U'\x308', U'\x903'}, {0ull, 3ull}}, {{U'\x378', U'\x1100'}, {0ull, 1ull, 2ull}},
    {{U'\x378', U'\x308', U'\x1100'}, {0ull, 2ull, 3ull}}, {{U'\x378', U'\x1160'}, {0ull, 1ull, 2ull}},
    {{U'\x378', U'\x308', U'\x1160'}, {0ull, 2ull, 3ull}}, {{U'\x378', U'\x11a8'}, {0ull, 1ull, 2ull}},
    {{U'\x378', U'\x308', U'\x11a8'}, {0ull, 2ull, 3ull}}, {{U'\x378', U'\xac00'}, {0ull, 1ull, 2ull}},
    {{U'\x378', U'\x308', U'\xac00'}, {0ull, 2ull, 3ull}}, {{U'\x378', U'\xac01'}, {0ull, 1ull, 2ull}},
    {{U'\x378', U'\x308', U'\xac01'}, {0ull, 2ull, 3ull}}, {{U'\x378', U'\x231a'}, {0ull, 1ull, 2ull}},
    {{U'\x378', U'\x308', U'\x231a'}, {0ull, 2ull, 3ull}}, {{U'\x378', U'\x300'}, {0ull, 2ull}},
    {{U'\x378', U'\x308', U'\x300'}, {0ull, 3ull}}, {{U'\x378', U'\x200d'}, {0ull, 2ull}},
    {{U'\x378', U'\x308', U'\x200d'}, {0ull, 3ull}}, {{U'\x378', U'\x378'}, {0ull, 1ull, 2ull}},
    {{U'\x378', U'\x308', U'\x378'}, {0ull, 2ull, 3ull}},
    {{U'\xd', U'\xa', U'\x61', U'\xa', U'\x308'}, {0ull, 2ull, 3ull, 4ull, 5ull}}, {{U'\x61', U'\x308'}, {0ull, 2ull}},
    {{U'\x20', U'\x200d', U'\x646'}, {0ull, 2ull, 3ull}}, {{U'\x646', U'\x200d', U'\x20'}, {0ull, 2ull, 3ull}},
    {{U'\x1100', U'\x1100'}, {0ull, 2ull}}, {{U'\xac00', U'\x11a8', U'\x1100'}, {0ull, 2ull, 3ull}},
    {{U'\xac01', U'\x11a8', U'\x1100'}, {0ull, 2ull, 3ull}},
    {{U'\x1f1e6', U'\x1f1e7', U'\x1f1e8', U'\x62'}, {0ull, 2ull, 3ull, 4ull}},
    {{U'\x61', U'\x1f1e6', U'\x1f1e7', U'\x1f1e8', U'\x62'}, {0ull, 1ull, 3ull, 4ull, 5ull}},
    {{U'\x61', U'\x1f1e6', U'\x1f1e7', U'\x200d', U'\x1f1e8', U'\x62'}, {0ull, 1ull, 4ull, 5ull, 6ull}},
    {{U'\x61', U'\x1f1e6', U'\x200d', U'\x1f1e7', U'\x1f1e8', U'\x62'}, {0ull, 1ull, 3ull, 5ull, 6ull}},
    {{U'\x61', U'\x1f1e6', U'\x1f1e7', U'\x1f1e8', U'\x1f1e9', U'\x62'}, {0ull, 1ull, 3ull, 5ull, 6ull}},
    {{U'\x61', U'\x200d'}, {0ull, 2ull}}, {{U'\x61', U'\x308', U'\x62'}, {0ull, 2ull, 3ull}},
    {{U'\x61', U'\x903', U'\x62'}, {0ull, 2ull, 3ull}}, {{U'\x61', U'\x600', U'\x62'}, {0ull, 1ull, 3ull}},
    {{U'\x1f476', U'\x1f3ff', U'\x1f476'}, {0ull, 2ull, 3ull}}, {{U'\x61', U'\x1f3ff', U'\x1f476'}, {0ull, 2ull, 3ull}},
    {{U'\x61', U'\x1f3ff', U'\x1f476', U'\x200d', U'\x1f6d1'}, {0ull, 2ull, 5ull}},
    {{U'\x1f476', U'\x1f3ff', U'\x308', U'\x200d', U'\x1f476', U'\x1f3ff'}, {0ull, 6ull}},
    {{U'\x1f6d1', U'\x200d', U'\x1f6d1'}, {0ull, 3ull}}, {{U'\x61', U'\x200d', U'\x1f6d1'}, {0ull, 2ull, 3ull}},
    {{U'\x2701', U'\x200d', U'\x2701'}, {0ull, 3ull}}, {{U'\x61', U'\x200d', U'\x2701'}, {0ull, 2ull, 3ull}}};


bool run_unicode_test_data_utf32() {
    for (auto& test : test_data) {
        _Grapheme_break_property_iterator<char32_t> iter(
            test.code_points.data(), test.code_points.data() + test.code_points.size());
        int i = 0;
        while (iter != _Grapheme_break_property_end_iterator{}) {
            assert(i < test.breaks.size());
            assert(*iter == test.code_points[test.breaks[i]]);
            ++iter;
            ++i;
        }
    }
    return true;
}

constexpr bool test_unicode_properties() {
    assert(_Grapheme_Break_Property_Data._Get_property_for_codepoint(0xB)
           == _Grapheme_Break_Property_Values::_Control_value);
    assert(_Grapheme_Break_Property_Data._Get_property_for_codepoint(0xC)
           == _Grapheme_Break_Property_Values::_Control_value);
    assert(
        _Grapheme_Break_Property_Data._Get_property_for_codepoint(0xA) == _Grapheme_Break_Property_Values::_LF_value);
    assert(
        _Grapheme_Break_Property_Data._Get_property_for_codepoint(0xD) == _Grapheme_Break_Property_Values::_CR_value);

    assert(_Grapheme_Break_Property_Data._Get_property_for_codepoint(0x7F)
           == _Grapheme_Break_Property_Values::_Control_value);
    assert(_Grapheme_Break_Property_Data._Get_property_for_codepoint(0x80)
           == _Grapheme_Break_Property_Values::_Control_value);
    assert(_Grapheme_Break_Property_Data._Get_property_for_codepoint(0x9F)
           == _Grapheme_Break_Property_Values::_Control_value);
    assert(
        _Grapheme_Break_Property_Data._Get_property_for_codepoint(0x7E) == _Grapheme_Break_Property_Values::_No_value);
    assert(
        _Grapheme_Break_Property_Data._Get_property_for_codepoint(0xA0) == _Grapheme_Break_Property_Values::_No_value);


    // emoji-data
    assert(_Extended_Pictographic_Property_Data._Get_property_for_codepoint(0x2194)
           == _Extended_Pictographic_Property_Values::_Extended_Pictographic_value);

    // emoji_component, not extended_pictographic
    assert(_Extended_Pictographic_Property_Data._Get_property_for_codepoint(0x23)
           == _Extended_Pictographic_Property_Values::_No_value);
    return true;
}

template <typename CharT, size_t N_enc, size_t N_dec>
constexpr void test_utf_decode_helper(const CharT (&encoded)[N_enc], const char32_t (&decoded)[N_dec]) {
    static_assert(_Is_any_of_v<CharT, char, wchar_t>);
    assert(ranges::equal(_Unicode_codepoint_iterator(encoded, encoded + N_enc), _Unicode_codepoint_end_iterator{},
        decoded, decoded + N_dec));
}

constexpr bool test_utf8_decode() {
    test_utf_decode_helper<char>("\xC0\xAF\xE0\x80\xBF\xF0\x81\x82\x41",
        {0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x41, 0x0});
    test_utf_decode_helper<char>("\xED\xA0\x80\xED\xBF\xBF\xED\xAF\x41",
        {0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x41, 0x0});
    test_utf_decode_helper<char>("\xF4\x91\x92\x93\xFF\x41\x80\xBF\x42",
        {0xFFFd, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x41, 0xFFFD, 0xFFFD, 0x42, 0x0});
    test_utf_decode_helper<char>("\xE1\x80\xE2\xF0\x91\x92\xF1\xBF\x41", {0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x41, 0x0});
    return true;
}

constexpr bool test_utf16_decode() {
    test_utf_decode_helper<wchar_t>({0xD7FF, 0xD800}, {0xD7FF, 0xFFFD});
    test_utf_decode_helper<wchar_t>({0xD800, 0xD7FF}, {0xFFFD, 0xD7FF});
    test_utf_decode_helper<wchar_t>({0xD7FF, 0xDF02}, {0xD7FF, 0xFFFD});
    test_utf_decode_helper<wchar_t>({0xDF02, 0xD7FF}, {0xFFFD, 0xD7FF});

    return true;
}

int main() {
    _set_error_mode(_OUT_TO_MSGBOX);
    test_unicode_properties();
    static_assert(test_unicode_properties());

    test_utf8_decode();
    static_assert(test_utf8_decode());

    test_utf16_decode();
    static_assert(test_utf16_decode());

    static_assert(forward_iterator<_Unicode_codepoint_iterator<char>>);
    static_assert(sentinel_for<_Unicode_codepoint_end_iterator, _Unicode_codepoint_iterator<char>>);
    static_assert(forward_iterator<_Grapheme_break_property_iterator<char>>);
    return 0;
}
