// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <assert.h>
#include <format>
#include <iterator>
#include <stddef.h>
#include <vector>

using namespace std;

// Beginning of generated data - DO NOT EDIT manually!

struct test_case_data {
    vector<char32_t> code_points;
    vector<size_t> breaks;
};
const test_case_data test_data[] = {{{U'\x20', U'\x20'}, {0, 1, 2}}, {{U'\x20', U'\x308', U'\x20'}, {0, 2, 3}},
    {{U'\x20', U'\xd'}, {0, 1, 2}}, {{U'\x20', U'\x308', U'\xd'}, {0, 2, 3}}, {{U'\x20', U'\xa'}, {0, 1, 2}},
    {{U'\x20', U'\x308', U'\xa'}, {0, 2, 3}}, {{U'\x20', U'\x1'}, {0, 1, 2}}, {{U'\x20', U'\x308', U'\x1'}, {0, 2, 3}},
    {{U'\x20', U'\x34f'}, {0, 2}}, {{U'\x20', U'\x308', U'\x34f'}, {0, 3}}, {{U'\x20', U'\x1f1e6'}, {0, 1, 2}},
    {{U'\x20', U'\x308', U'\x1f1e6'}, {0, 2, 3}}, {{U'\x20', U'\x600'}, {0, 1, 2}},
    {{U'\x20', U'\x308', U'\x600'}, {0, 2, 3}}, {{U'\x20', U'\x903'}, {0, 2}}, {{U'\x20', U'\x308', U'\x903'}, {0, 3}},
    {{U'\x20', U'\x1100'}, {0, 1, 2}}, {{U'\x20', U'\x308', U'\x1100'}, {0, 2, 3}}, {{U'\x20', U'\x1160'}, {0, 1, 2}},
    {{U'\x20', U'\x308', U'\x1160'}, {0, 2, 3}}, {{U'\x20', U'\x11a8'}, {0, 1, 2}},
    {{U'\x20', U'\x308', U'\x11a8'}, {0, 2, 3}}, {{U'\x20', U'\xac00'}, {0, 1, 2}},
    {{U'\x20', U'\x308', U'\xac00'}, {0, 2, 3}}, {{U'\x20', U'\xac01'}, {0, 1, 2}},
    {{U'\x20', U'\x308', U'\xac01'}, {0, 2, 3}}, {{U'\x20', U'\x231a'}, {0, 1, 2}},
    {{U'\x20', U'\x308', U'\x231a'}, {0, 2, 3}}, {{U'\x20', U'\x300'}, {0, 2}}, {{U'\x20', U'\x308', U'\x300'}, {0, 3}},
    {{U'\x20', U'\x200d'}, {0, 2}}, {{U'\x20', U'\x308', U'\x200d'}, {0, 3}}, {{U'\x20', U'\x378'}, {0, 1, 2}},
    {{U'\x20', U'\x308', U'\x378'}, {0, 2, 3}}, {{U'\xd', U'\x20'}, {0, 1, 2}},
    {{U'\xd', U'\x308', U'\x20'}, {0, 1, 2, 3}}, {{U'\xd', U'\xd'}, {0, 1, 2}},
    {{U'\xd', U'\x308', U'\xd'}, {0, 1, 2, 3}}, {{U'\xd', U'\xa'}, {0, 2}}, {{U'\xd', U'\x308', U'\xa'}, {0, 1, 2, 3}},
    {{U'\xd', U'\x1'}, {0, 1, 2}}, {{U'\xd', U'\x308', U'\x1'}, {0, 1, 2, 3}}, {{U'\xd', U'\x34f'}, {0, 1, 2}},
    {{U'\xd', U'\x308', U'\x34f'}, {0, 1, 3}}, {{U'\xd', U'\x1f1e6'}, {0, 1, 2}},
    {{U'\xd', U'\x308', U'\x1f1e6'}, {0, 1, 2, 3}}, {{U'\xd', U'\x600'}, {0, 1, 2}},
    {{U'\xd', U'\x308', U'\x600'}, {0, 1, 2, 3}}, {{U'\xd', U'\x903'}, {0, 1, 2}},
    {{U'\xd', U'\x308', U'\x903'}, {0, 1, 3}}, {{U'\xd', U'\x1100'}, {0, 1, 2}},
    {{U'\xd', U'\x308', U'\x1100'}, {0, 1, 2, 3}}, {{U'\xd', U'\x1160'}, {0, 1, 2}},
    {{U'\xd', U'\x308', U'\x1160'}, {0, 1, 2, 3}}, {{U'\xd', U'\x11a8'}, {0, 1, 2}},
    {{U'\xd', U'\x308', U'\x11a8'}, {0, 1, 2, 3}}, {{U'\xd', U'\xac00'}, {0, 1, 2}},
    {{U'\xd', U'\x308', U'\xac00'}, {0, 1, 2, 3}}, {{U'\xd', U'\xac01'}, {0, 1, 2}},
    {{U'\xd', U'\x308', U'\xac01'}, {0, 1, 2, 3}}, {{U'\xd', U'\x231a'}, {0, 1, 2}},
    {{U'\xd', U'\x308', U'\x231a'}, {0, 1, 2, 3}}, {{U'\xd', U'\x300'}, {0, 1, 2}},
    {{U'\xd', U'\x308', U'\x300'}, {0, 1, 3}}, {{U'\xd', U'\x200d'}, {0, 1, 2}},
    {{U'\xd', U'\x308', U'\x200d'}, {0, 1, 3}}, {{U'\xd', U'\x378'}, {0, 1, 2}},
    {{U'\xd', U'\x308', U'\x378'}, {0, 1, 2, 3}}, {{U'\xa', U'\x20'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\x20'}, {0, 1, 2, 3}}, {{U'\xa', U'\xd'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\xd'}, {0, 1, 2, 3}}, {{U'\xa', U'\xa'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\xa'}, {0, 1, 2, 3}}, {{U'\xa', U'\x1'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\x1'}, {0, 1, 2, 3}}, {{U'\xa', U'\x34f'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\x34f'}, {0, 1, 3}}, {{U'\xa', U'\x1f1e6'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\x1f1e6'}, {0, 1, 2, 3}}, {{U'\xa', U'\x600'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\x600'}, {0, 1, 2, 3}}, {{U'\xa', U'\x903'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\x903'}, {0, 1, 3}}, {{U'\xa', U'\x1100'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\x1100'}, {0, 1, 2, 3}}, {{U'\xa', U'\x1160'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\x1160'}, {0, 1, 2, 3}}, {{U'\xa', U'\x11a8'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\x11a8'}, {0, 1, 2, 3}}, {{U'\xa', U'\xac00'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\xac00'}, {0, 1, 2, 3}}, {{U'\xa', U'\xac01'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\xac01'}, {0, 1, 2, 3}}, {{U'\xa', U'\x231a'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\x231a'}, {0, 1, 2, 3}}, {{U'\xa', U'\x300'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\x300'}, {0, 1, 3}}, {{U'\xa', U'\x200d'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\x200d'}, {0, 1, 3}}, {{U'\xa', U'\x378'}, {0, 1, 2}},
    {{U'\xa', U'\x308', U'\x378'}, {0, 1, 2, 3}}, {{U'\x1', U'\x20'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\x20'}, {0, 1, 2, 3}}, {{U'\x1', U'\xd'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\xd'}, {0, 1, 2, 3}}, {{U'\x1', U'\xa'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\xa'}, {0, 1, 2, 3}}, {{U'\x1', U'\x1'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\x1'}, {0, 1, 2, 3}}, {{U'\x1', U'\x34f'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\x34f'}, {0, 1, 3}}, {{U'\x1', U'\x1f1e6'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\x1f1e6'}, {0, 1, 2, 3}}, {{U'\x1', U'\x600'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\x600'}, {0, 1, 2, 3}}, {{U'\x1', U'\x903'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\x903'}, {0, 1, 3}}, {{U'\x1', U'\x1100'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\x1100'}, {0, 1, 2, 3}}, {{U'\x1', U'\x1160'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\x1160'}, {0, 1, 2, 3}}, {{U'\x1', U'\x11a8'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\x11a8'}, {0, 1, 2, 3}}, {{U'\x1', U'\xac00'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\xac00'}, {0, 1, 2, 3}}, {{U'\x1', U'\xac01'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\xac01'}, {0, 1, 2, 3}}, {{U'\x1', U'\x231a'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\x231a'}, {0, 1, 2, 3}}, {{U'\x1', U'\x300'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\x300'}, {0, 1, 3}}, {{U'\x1', U'\x200d'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\x200d'}, {0, 1, 3}}, {{U'\x1', U'\x378'}, {0, 1, 2}},
    {{U'\x1', U'\x308', U'\x378'}, {0, 1, 2, 3}}, {{U'\x34f', U'\x20'}, {0, 1, 2}},
    {{U'\x34f', U'\x308', U'\x20'}, {0, 2, 3}}, {{U'\x34f', U'\xd'}, {0, 1, 2}},
    {{U'\x34f', U'\x308', U'\xd'}, {0, 2, 3}}, {{U'\x34f', U'\xa'}, {0, 1, 2}},
    {{U'\x34f', U'\x308', U'\xa'}, {0, 2, 3}}, {{U'\x34f', U'\x1'}, {0, 1, 2}},
    {{U'\x34f', U'\x308', U'\x1'}, {0, 2, 3}}, {{U'\x34f', U'\x34f'}, {0, 2}}, {{U'\x34f', U'\x308', U'\x34f'}, {0, 3}},
    {{U'\x34f', U'\x1f1e6'}, {0, 1, 2}}, {{U'\x34f', U'\x308', U'\x1f1e6'}, {0, 2, 3}},
    {{U'\x34f', U'\x600'}, {0, 1, 2}}, {{U'\x34f', U'\x308', U'\x600'}, {0, 2, 3}}, {{U'\x34f', U'\x903'}, {0, 2}},
    {{U'\x34f', U'\x308', U'\x903'}, {0, 3}}, {{U'\x34f', U'\x1100'}, {0, 1, 2}},
    {{U'\x34f', U'\x308', U'\x1100'}, {0, 2, 3}}, {{U'\x34f', U'\x1160'}, {0, 1, 2}},
    {{U'\x34f', U'\x308', U'\x1160'}, {0, 2, 3}}, {{U'\x34f', U'\x11a8'}, {0, 1, 2}},
    {{U'\x34f', U'\x308', U'\x11a8'}, {0, 2, 3}}, {{U'\x34f', U'\xac00'}, {0, 1, 2}},
    {{U'\x34f', U'\x308', U'\xac00'}, {0, 2, 3}}, {{U'\x34f', U'\xac01'}, {0, 1, 2}},
    {{U'\x34f', U'\x308', U'\xac01'}, {0, 2, 3}}, {{U'\x34f', U'\x231a'}, {0, 1, 2}},
    {{U'\x34f', U'\x308', U'\x231a'}, {0, 2, 3}}, {{U'\x34f', U'\x300'}, {0, 2}},
    {{U'\x34f', U'\x308', U'\x300'}, {0, 3}}, {{U'\x34f', U'\x200d'}, {0, 2}},
    {{U'\x34f', U'\x308', U'\x200d'}, {0, 3}}, {{U'\x34f', U'\x378'}, {0, 1, 2}},
    {{U'\x34f', U'\x308', U'\x378'}, {0, 2, 3}}, {{U'\x1f1e6', U'\x20'}, {0, 1, 2}},
    {{U'\x1f1e6', U'\x308', U'\x20'}, {0, 2, 3}}, {{U'\x1f1e6', U'\xd'}, {0, 1, 2}},
    {{U'\x1f1e6', U'\x308', U'\xd'}, {0, 2, 3}}, {{U'\x1f1e6', U'\xa'}, {0, 1, 2}},
    {{U'\x1f1e6', U'\x308', U'\xa'}, {0, 2, 3}}, {{U'\x1f1e6', U'\x1'}, {0, 1, 2}},
    {{U'\x1f1e6', U'\x308', U'\x1'}, {0, 2, 3}}, {{U'\x1f1e6', U'\x34f'}, {0, 2}},
    {{U'\x1f1e6', U'\x308', U'\x34f'}, {0, 3}}, {{U'\x1f1e6', U'\x1f1e6'}, {0, 2}},
    {{U'\x1f1e6', U'\x308', U'\x1f1e6'}, {0, 2, 3}}, {{U'\x1f1e6', U'\x600'}, {0, 1, 2}},
    {{U'\x1f1e6', U'\x308', U'\x600'}, {0, 2, 3}}, {{U'\x1f1e6', U'\x903'}, {0, 2}},
    {{U'\x1f1e6', U'\x308', U'\x903'}, {0, 3}}, {{U'\x1f1e6', U'\x1100'}, {0, 1, 2}},
    {{U'\x1f1e6', U'\x308', U'\x1100'}, {0, 2, 3}}, {{U'\x1f1e6', U'\x1160'}, {0, 1, 2}},
    {{U'\x1f1e6', U'\x308', U'\x1160'}, {0, 2, 3}}, {{U'\x1f1e6', U'\x11a8'}, {0, 1, 2}},
    {{U'\x1f1e6', U'\x308', U'\x11a8'}, {0, 2, 3}}, {{U'\x1f1e6', U'\xac00'}, {0, 1, 2}},
    {{U'\x1f1e6', U'\x308', U'\xac00'}, {0, 2, 3}}, {{U'\x1f1e6', U'\xac01'}, {0, 1, 2}},
    {{U'\x1f1e6', U'\x308', U'\xac01'}, {0, 2, 3}}, {{U'\x1f1e6', U'\x231a'}, {0, 1, 2}},
    {{U'\x1f1e6', U'\x308', U'\x231a'}, {0, 2, 3}}, {{U'\x1f1e6', U'\x300'}, {0, 2}},
    {{U'\x1f1e6', U'\x308', U'\x300'}, {0, 3}}, {{U'\x1f1e6', U'\x200d'}, {0, 2}},
    {{U'\x1f1e6', U'\x308', U'\x200d'}, {0, 3}}, {{U'\x1f1e6', U'\x378'}, {0, 1, 2}},
    {{U'\x1f1e6', U'\x308', U'\x378'}, {0, 2, 3}}, {{U'\x600', U'\x20'}, {0, 2}},
    {{U'\x600', U'\x308', U'\x20'}, {0, 2, 3}}, {{U'\x600', U'\xd'}, {0, 1, 2}},
    {{U'\x600', U'\x308', U'\xd'}, {0, 2, 3}}, {{U'\x600', U'\xa'}, {0, 1, 2}},
    {{U'\x600', U'\x308', U'\xa'}, {0, 2, 3}}, {{U'\x600', U'\x1'}, {0, 1, 2}},
    {{U'\x600', U'\x308', U'\x1'}, {0, 2, 3}}, {{U'\x600', U'\x34f'}, {0, 2}}, {{U'\x600', U'\x308', U'\x34f'}, {0, 3}},
    {{U'\x600', U'\x1f1e6'}, {0, 2}}, {{U'\x600', U'\x308', U'\x1f1e6'}, {0, 2, 3}}, {{U'\x600', U'\x600'}, {0, 2}},
    {{U'\x600', U'\x308', U'\x600'}, {0, 2, 3}}, {{U'\x600', U'\x903'}, {0, 2}},
    {{U'\x600', U'\x308', U'\x903'}, {0, 3}}, {{U'\x600', U'\x1100'}, {0, 2}},
    {{U'\x600', U'\x308', U'\x1100'}, {0, 2, 3}}, {{U'\x600', U'\x1160'}, {0, 2}},
    {{U'\x600', U'\x308', U'\x1160'}, {0, 2, 3}}, {{U'\x600', U'\x11a8'}, {0, 2}},
    {{U'\x600', U'\x308', U'\x11a8'}, {0, 2, 3}}, {{U'\x600', U'\xac00'}, {0, 2}},
    {{U'\x600', U'\x308', U'\xac00'}, {0, 2, 3}}, {{U'\x600', U'\xac01'}, {0, 2}},
    {{U'\x600', U'\x308', U'\xac01'}, {0, 2, 3}}, {{U'\x600', U'\x231a'}, {0, 2}},
    {{U'\x600', U'\x308', U'\x231a'}, {0, 2, 3}}, {{U'\x600', U'\x300'}, {0, 2}},
    {{U'\x600', U'\x308', U'\x300'}, {0, 3}}, {{U'\x600', U'\x200d'}, {0, 2}},
    {{U'\x600', U'\x308', U'\x200d'}, {0, 3}}, {{U'\x600', U'\x378'}, {0, 2}},
    {{U'\x600', U'\x308', U'\x378'}, {0, 2, 3}}, {{U'\x903', U'\x20'}, {0, 1, 2}},
    {{U'\x903', U'\x308', U'\x20'}, {0, 2, 3}}, {{U'\x903', U'\xd'}, {0, 1, 2}},
    {{U'\x903', U'\x308', U'\xd'}, {0, 2, 3}}, {{U'\x903', U'\xa'}, {0, 1, 2}},
    {{U'\x903', U'\x308', U'\xa'}, {0, 2, 3}}, {{U'\x903', U'\x1'}, {0, 1, 2}},
    {{U'\x903', U'\x308', U'\x1'}, {0, 2, 3}}, {{U'\x903', U'\x34f'}, {0, 2}}, {{U'\x903', U'\x308', U'\x34f'}, {0, 3}},
    {{U'\x903', U'\x1f1e6'}, {0, 1, 2}}, {{U'\x903', U'\x308', U'\x1f1e6'}, {0, 2, 3}},
    {{U'\x903', U'\x600'}, {0, 1, 2}}, {{U'\x903', U'\x308', U'\x600'}, {0, 2, 3}}, {{U'\x903', U'\x903'}, {0, 2}},
    {{U'\x903', U'\x308', U'\x903'}, {0, 3}}, {{U'\x903', U'\x1100'}, {0, 1, 2}},
    {{U'\x903', U'\x308', U'\x1100'}, {0, 2, 3}}, {{U'\x903', U'\x1160'}, {0, 1, 2}},
    {{U'\x903', U'\x308', U'\x1160'}, {0, 2, 3}}, {{U'\x903', U'\x11a8'}, {0, 1, 2}},
    {{U'\x903', U'\x308', U'\x11a8'}, {0, 2, 3}}, {{U'\x903', U'\xac00'}, {0, 1, 2}},
    {{U'\x903', U'\x308', U'\xac00'}, {0, 2, 3}}, {{U'\x903', U'\xac01'}, {0, 1, 2}},
    {{U'\x903', U'\x308', U'\xac01'}, {0, 2, 3}}, {{U'\x903', U'\x231a'}, {0, 1, 2}},
    {{U'\x903', U'\x308', U'\x231a'}, {0, 2, 3}}, {{U'\x903', U'\x300'}, {0, 2}},
    {{U'\x903', U'\x308', U'\x300'}, {0, 3}}, {{U'\x903', U'\x200d'}, {0, 2}},
    {{U'\x903', U'\x308', U'\x200d'}, {0, 3}}, {{U'\x903', U'\x378'}, {0, 1, 2}},
    {{U'\x903', U'\x308', U'\x378'}, {0, 2, 3}}, {{U'\x1100', U'\x20'}, {0, 1, 2}},
    {{U'\x1100', U'\x308', U'\x20'}, {0, 2, 3}}, {{U'\x1100', U'\xd'}, {0, 1, 2}},
    {{U'\x1100', U'\x308', U'\xd'}, {0, 2, 3}}, {{U'\x1100', U'\xa'}, {0, 1, 2}},
    {{U'\x1100', U'\x308', U'\xa'}, {0, 2, 3}}, {{U'\x1100', U'\x1'}, {0, 1, 2}},
    {{U'\x1100', U'\x308', U'\x1'}, {0, 2, 3}}, {{U'\x1100', U'\x34f'}, {0, 2}},
    {{U'\x1100', U'\x308', U'\x34f'}, {0, 3}}, {{U'\x1100', U'\x1f1e6'}, {0, 1, 2}},
    {{U'\x1100', U'\x308', U'\x1f1e6'}, {0, 2, 3}}, {{U'\x1100', U'\x600'}, {0, 1, 2}},
    {{U'\x1100', U'\x308', U'\x600'}, {0, 2, 3}}, {{U'\x1100', U'\x903'}, {0, 2}},
    {{U'\x1100', U'\x308', U'\x903'}, {0, 3}}, {{U'\x1100', U'\x1100'}, {0, 2}},
    {{U'\x1100', U'\x308', U'\x1100'}, {0, 2, 3}}, {{U'\x1100', U'\x1160'}, {0, 2}},
    {{U'\x1100', U'\x308', U'\x1160'}, {0, 2, 3}}, {{U'\x1100', U'\x11a8'}, {0, 1, 2}},
    {{U'\x1100', U'\x308', U'\x11a8'}, {0, 2, 3}}, {{U'\x1100', U'\xac00'}, {0, 2}},
    {{U'\x1100', U'\x308', U'\xac00'}, {0, 2, 3}}, {{U'\x1100', U'\xac01'}, {0, 2}},
    {{U'\x1100', U'\x308', U'\xac01'}, {0, 2, 3}}, {{U'\x1100', U'\x231a'}, {0, 1, 2}},
    {{U'\x1100', U'\x308', U'\x231a'}, {0, 2, 3}}, {{U'\x1100', U'\x300'}, {0, 2}},
    {{U'\x1100', U'\x308', U'\x300'}, {0, 3}}, {{U'\x1100', U'\x200d'}, {0, 2}},
    {{U'\x1100', U'\x308', U'\x200d'}, {0, 3}}, {{U'\x1100', U'\x378'}, {0, 1, 2}},
    {{U'\x1100', U'\x308', U'\x378'}, {0, 2, 3}}, {{U'\x1160', U'\x20'}, {0, 1, 2}},
    {{U'\x1160', U'\x308', U'\x20'}, {0, 2, 3}}, {{U'\x1160', U'\xd'}, {0, 1, 2}},
    {{U'\x1160', U'\x308', U'\xd'}, {0, 2, 3}}, {{U'\x1160', U'\xa'}, {0, 1, 2}},
    {{U'\x1160', U'\x308', U'\xa'}, {0, 2, 3}}, {{U'\x1160', U'\x1'}, {0, 1, 2}},
    {{U'\x1160', U'\x308', U'\x1'}, {0, 2, 3}}, {{U'\x1160', U'\x34f'}, {0, 2}},
    {{U'\x1160', U'\x308', U'\x34f'}, {0, 3}}, {{U'\x1160', U'\x1f1e6'}, {0, 1, 2}},
    {{U'\x1160', U'\x308', U'\x1f1e6'}, {0, 2, 3}}, {{U'\x1160', U'\x600'}, {0, 1, 2}},
    {{U'\x1160', U'\x308', U'\x600'}, {0, 2, 3}}, {{U'\x1160', U'\x903'}, {0, 2}},
    {{U'\x1160', U'\x308', U'\x903'}, {0, 3}}, {{U'\x1160', U'\x1100'}, {0, 1, 2}},
    {{U'\x1160', U'\x308', U'\x1100'}, {0, 2, 3}}, {{U'\x1160', U'\x1160'}, {0, 2}},
    {{U'\x1160', U'\x308', U'\x1160'}, {0, 2, 3}}, {{U'\x1160', U'\x11a8'}, {0, 2}},
    {{U'\x1160', U'\x308', U'\x11a8'}, {0, 2, 3}}, {{U'\x1160', U'\xac00'}, {0, 1, 2}},
    {{U'\x1160', U'\x308', U'\xac00'}, {0, 2, 3}}, {{U'\x1160', U'\xac01'}, {0, 1, 2}},
    {{U'\x1160', U'\x308', U'\xac01'}, {0, 2, 3}}, {{U'\x1160', U'\x231a'}, {0, 1, 2}},
    {{U'\x1160', U'\x308', U'\x231a'}, {0, 2, 3}}, {{U'\x1160', U'\x300'}, {0, 2}},
    {{U'\x1160', U'\x308', U'\x300'}, {0, 3}}, {{U'\x1160', U'\x200d'}, {0, 2}},
    {{U'\x1160', U'\x308', U'\x200d'}, {0, 3}}, {{U'\x1160', U'\x378'}, {0, 1, 2}},
    {{U'\x1160', U'\x308', U'\x378'}, {0, 2, 3}}, {{U'\x11a8', U'\x20'}, {0, 1, 2}},
    {{U'\x11a8', U'\x308', U'\x20'}, {0, 2, 3}}, {{U'\x11a8', U'\xd'}, {0, 1, 2}},
    {{U'\x11a8', U'\x308', U'\xd'}, {0, 2, 3}}, {{U'\x11a8', U'\xa'}, {0, 1, 2}},
    {{U'\x11a8', U'\x308', U'\xa'}, {0, 2, 3}}, {{U'\x11a8', U'\x1'}, {0, 1, 2}},
    {{U'\x11a8', U'\x308', U'\x1'}, {0, 2, 3}}, {{U'\x11a8', U'\x34f'}, {0, 2}},
    {{U'\x11a8', U'\x308', U'\x34f'}, {0, 3}}, {{U'\x11a8', U'\x1f1e6'}, {0, 1, 2}},
    {{U'\x11a8', U'\x308', U'\x1f1e6'}, {0, 2, 3}}, {{U'\x11a8', U'\x600'}, {0, 1, 2}},
    {{U'\x11a8', U'\x308', U'\x600'}, {0, 2, 3}}, {{U'\x11a8', U'\x903'}, {0, 2}},
    {{U'\x11a8', U'\x308', U'\x903'}, {0, 3}}, {{U'\x11a8', U'\x1100'}, {0, 1, 2}},
    {{U'\x11a8', U'\x308', U'\x1100'}, {0, 2, 3}}, {{U'\x11a8', U'\x1160'}, {0, 1, 2}},
    {{U'\x11a8', U'\x308', U'\x1160'}, {0, 2, 3}}, {{U'\x11a8', U'\x11a8'}, {0, 2}},
    {{U'\x11a8', U'\x308', U'\x11a8'}, {0, 2, 3}}, {{U'\x11a8', U'\xac00'}, {0, 1, 2}},
    {{U'\x11a8', U'\x308', U'\xac00'}, {0, 2, 3}}, {{U'\x11a8', U'\xac01'}, {0, 1, 2}},
    {{U'\x11a8', U'\x308', U'\xac01'}, {0, 2, 3}}, {{U'\x11a8', U'\x231a'}, {0, 1, 2}},
    {{U'\x11a8', U'\x308', U'\x231a'}, {0, 2, 3}}, {{U'\x11a8', U'\x300'}, {0, 2}},
    {{U'\x11a8', U'\x308', U'\x300'}, {0, 3}}, {{U'\x11a8', U'\x200d'}, {0, 2}},
    {{U'\x11a8', U'\x308', U'\x200d'}, {0, 3}}, {{U'\x11a8', U'\x378'}, {0, 1, 2}},
    {{U'\x11a8', U'\x308', U'\x378'}, {0, 2, 3}}, {{U'\xac00', U'\x20'}, {0, 1, 2}},
    {{U'\xac00', U'\x308', U'\x20'}, {0, 2, 3}}, {{U'\xac00', U'\xd'}, {0, 1, 2}},
    {{U'\xac00', U'\x308', U'\xd'}, {0, 2, 3}}, {{U'\xac00', U'\xa'}, {0, 1, 2}},
    {{U'\xac00', U'\x308', U'\xa'}, {0, 2, 3}}, {{U'\xac00', U'\x1'}, {0, 1, 2}},
    {{U'\xac00', U'\x308', U'\x1'}, {0, 2, 3}}, {{U'\xac00', U'\x34f'}, {0, 2}},
    {{U'\xac00', U'\x308', U'\x34f'}, {0, 3}}, {{U'\xac00', U'\x1f1e6'}, {0, 1, 2}},
    {{U'\xac00', U'\x308', U'\x1f1e6'}, {0, 2, 3}}, {{U'\xac00', U'\x600'}, {0, 1, 2}},
    {{U'\xac00', U'\x308', U'\x600'}, {0, 2, 3}}, {{U'\xac00', U'\x903'}, {0, 2}},
    {{U'\xac00', U'\x308', U'\x903'}, {0, 3}}, {{U'\xac00', U'\x1100'}, {0, 1, 2}},
    {{U'\xac00', U'\x308', U'\x1100'}, {0, 2, 3}}, {{U'\xac00', U'\x1160'}, {0, 2}},
    {{U'\xac00', U'\x308', U'\x1160'}, {0, 2, 3}}, {{U'\xac00', U'\x11a8'}, {0, 2}},
    {{U'\xac00', U'\x308', U'\x11a8'}, {0, 2, 3}}, {{U'\xac00', U'\xac00'}, {0, 1, 2}},
    {{U'\xac00', U'\x308', U'\xac00'}, {0, 2, 3}}, {{U'\xac00', U'\xac01'}, {0, 1, 2}},
    {{U'\xac00', U'\x308', U'\xac01'}, {0, 2, 3}}, {{U'\xac00', U'\x231a'}, {0, 1, 2}},
    {{U'\xac00', U'\x308', U'\x231a'}, {0, 2, 3}}, {{U'\xac00', U'\x300'}, {0, 2}},
    {{U'\xac00', U'\x308', U'\x300'}, {0, 3}}, {{U'\xac00', U'\x200d'}, {0, 2}},
    {{U'\xac00', U'\x308', U'\x200d'}, {0, 3}}, {{U'\xac00', U'\x378'}, {0, 1, 2}},
    {{U'\xac00', U'\x308', U'\x378'}, {0, 2, 3}}, {{U'\xac01', U'\x20'}, {0, 1, 2}},
    {{U'\xac01', U'\x308', U'\x20'}, {0, 2, 3}}, {{U'\xac01', U'\xd'}, {0, 1, 2}},
    {{U'\xac01', U'\x308', U'\xd'}, {0, 2, 3}}, {{U'\xac01', U'\xa'}, {0, 1, 2}},
    {{U'\xac01', U'\x308', U'\xa'}, {0, 2, 3}}, {{U'\xac01', U'\x1'}, {0, 1, 2}},
    {{U'\xac01', U'\x308', U'\x1'}, {0, 2, 3}}, {{U'\xac01', U'\x34f'}, {0, 2}},
    {{U'\xac01', U'\x308', U'\x34f'}, {0, 3}}, {{U'\xac01', U'\x1f1e6'}, {0, 1, 2}},
    {{U'\xac01', U'\x308', U'\x1f1e6'}, {0, 2, 3}}, {{U'\xac01', U'\x600'}, {0, 1, 2}},
    {{U'\xac01', U'\x308', U'\x600'}, {0, 2, 3}}, {{U'\xac01', U'\x903'}, {0, 2}},
    {{U'\xac01', U'\x308', U'\x903'}, {0, 3}}, {{U'\xac01', U'\x1100'}, {0, 1, 2}},
    {{U'\xac01', U'\x308', U'\x1100'}, {0, 2, 3}}, {{U'\xac01', U'\x1160'}, {0, 1, 2}},
    {{U'\xac01', U'\x308', U'\x1160'}, {0, 2, 3}}, {{U'\xac01', U'\x11a8'}, {0, 2}},
    {{U'\xac01', U'\x308', U'\x11a8'}, {0, 2, 3}}, {{U'\xac01', U'\xac00'}, {0, 1, 2}},
    {{U'\xac01', U'\x308', U'\xac00'}, {0, 2, 3}}, {{U'\xac01', U'\xac01'}, {0, 1, 2}},
    {{U'\xac01', U'\x308', U'\xac01'}, {0, 2, 3}}, {{U'\xac01', U'\x231a'}, {0, 1, 2}},
    {{U'\xac01', U'\x308', U'\x231a'}, {0, 2, 3}}, {{U'\xac01', U'\x300'}, {0, 2}},
    {{U'\xac01', U'\x308', U'\x300'}, {0, 3}}, {{U'\xac01', U'\x200d'}, {0, 2}},
    {{U'\xac01', U'\x308', U'\x200d'}, {0, 3}}, {{U'\xac01', U'\x378'}, {0, 1, 2}},
    {{U'\xac01', U'\x308', U'\x378'}, {0, 2, 3}}, {{U'\x231a', U'\x20'}, {0, 1, 2}},
    {{U'\x231a', U'\x308', U'\x20'}, {0, 2, 3}}, {{U'\x231a', U'\xd'}, {0, 1, 2}},
    {{U'\x231a', U'\x308', U'\xd'}, {0, 2, 3}}, {{U'\x231a', U'\xa'}, {0, 1, 2}},
    {{U'\x231a', U'\x308', U'\xa'}, {0, 2, 3}}, {{U'\x231a', U'\x1'}, {0, 1, 2}},
    {{U'\x231a', U'\x308', U'\x1'}, {0, 2, 3}}, {{U'\x231a', U'\x34f'}, {0, 2}},
    {{U'\x231a', U'\x308', U'\x34f'}, {0, 3}}, {{U'\x231a', U'\x1f1e6'}, {0, 1, 2}},
    {{U'\x231a', U'\x308', U'\x1f1e6'}, {0, 2, 3}}, {{U'\x231a', U'\x600'}, {0, 1, 2}},
    {{U'\x231a', U'\x308', U'\x600'}, {0, 2, 3}}, {{U'\x231a', U'\x903'}, {0, 2}},
    {{U'\x231a', U'\x308', U'\x903'}, {0, 3}}, {{U'\x231a', U'\x1100'}, {0, 1, 2}},
    {{U'\x231a', U'\x308', U'\x1100'}, {0, 2, 3}}, {{U'\x231a', U'\x1160'}, {0, 1, 2}},
    {{U'\x231a', U'\x308', U'\x1160'}, {0, 2, 3}}, {{U'\x231a', U'\x11a8'}, {0, 1, 2}},
    {{U'\x231a', U'\x308', U'\x11a8'}, {0, 2, 3}}, {{U'\x231a', U'\xac00'}, {0, 1, 2}},
    {{U'\x231a', U'\x308', U'\xac00'}, {0, 2, 3}}, {{U'\x231a', U'\xac01'}, {0, 1, 2}},
    {{U'\x231a', U'\x308', U'\xac01'}, {0, 2, 3}}, {{U'\x231a', U'\x231a'}, {0, 1, 2}},
    {{U'\x231a', U'\x308', U'\x231a'}, {0, 2, 3}}, {{U'\x231a', U'\x300'}, {0, 2}},
    {{U'\x231a', U'\x308', U'\x300'}, {0, 3}}, {{U'\x231a', U'\x200d'}, {0, 2}},
    {{U'\x231a', U'\x308', U'\x200d'}, {0, 3}}, {{U'\x231a', U'\x378'}, {0, 1, 2}},
    {{U'\x231a', U'\x308', U'\x378'}, {0, 2, 3}}, {{U'\x300', U'\x20'}, {0, 1, 2}},
    {{U'\x300', U'\x308', U'\x20'}, {0, 2, 3}}, {{U'\x300', U'\xd'}, {0, 1, 2}},
    {{U'\x300', U'\x308', U'\xd'}, {0, 2, 3}}, {{U'\x300', U'\xa'}, {0, 1, 2}},
    {{U'\x300', U'\x308', U'\xa'}, {0, 2, 3}}, {{U'\x300', U'\x1'}, {0, 1, 2}},
    {{U'\x300', U'\x308', U'\x1'}, {0, 2, 3}}, {{U'\x300', U'\x34f'}, {0, 2}}, {{U'\x300', U'\x308', U'\x34f'}, {0, 3}},
    {{U'\x300', U'\x1f1e6'}, {0, 1, 2}}, {{U'\x300', U'\x308', U'\x1f1e6'}, {0, 2, 3}},
    {{U'\x300', U'\x600'}, {0, 1, 2}}, {{U'\x300', U'\x308', U'\x600'}, {0, 2, 3}}, {{U'\x300', U'\x903'}, {0, 2}},
    {{U'\x300', U'\x308', U'\x903'}, {0, 3}}, {{U'\x300', U'\x1100'}, {0, 1, 2}},
    {{U'\x300', U'\x308', U'\x1100'}, {0, 2, 3}}, {{U'\x300', U'\x1160'}, {0, 1, 2}},
    {{U'\x300', U'\x308', U'\x1160'}, {0, 2, 3}}, {{U'\x300', U'\x11a8'}, {0, 1, 2}},
    {{U'\x300', U'\x308', U'\x11a8'}, {0, 2, 3}}, {{U'\x300', U'\xac00'}, {0, 1, 2}},
    {{U'\x300', U'\x308', U'\xac00'}, {0, 2, 3}}, {{U'\x300', U'\xac01'}, {0, 1, 2}},
    {{U'\x300', U'\x308', U'\xac01'}, {0, 2, 3}}, {{U'\x300', U'\x231a'}, {0, 1, 2}},
    {{U'\x300', U'\x308', U'\x231a'}, {0, 2, 3}}, {{U'\x300', U'\x300'}, {0, 2}},
    {{U'\x300', U'\x308', U'\x300'}, {0, 3}}, {{U'\x300', U'\x200d'}, {0, 2}},
    {{U'\x300', U'\x308', U'\x200d'}, {0, 3}}, {{U'\x300', U'\x378'}, {0, 1, 2}},
    {{U'\x300', U'\x308', U'\x378'}, {0, 2, 3}}, {{U'\x200d', U'\x20'}, {0, 1, 2}},
    {{U'\x200d', U'\x308', U'\x20'}, {0, 2, 3}}, {{U'\x200d', U'\xd'}, {0, 1, 2}},
    {{U'\x200d', U'\x308', U'\xd'}, {0, 2, 3}}, {{U'\x200d', U'\xa'}, {0, 1, 2}},
    {{U'\x200d', U'\x308', U'\xa'}, {0, 2, 3}}, {{U'\x200d', U'\x1'}, {0, 1, 2}},
    {{U'\x200d', U'\x308', U'\x1'}, {0, 2, 3}}, {{U'\x200d', U'\x34f'}, {0, 2}},
    {{U'\x200d', U'\x308', U'\x34f'}, {0, 3}}, {{U'\x200d', U'\x1f1e6'}, {0, 1, 2}},
    {{U'\x200d', U'\x308', U'\x1f1e6'}, {0, 2, 3}}, {{U'\x200d', U'\x600'}, {0, 1, 2}},
    {{U'\x200d', U'\x308', U'\x600'}, {0, 2, 3}}, {{U'\x200d', U'\x903'}, {0, 2}},
    {{U'\x200d', U'\x308', U'\x903'}, {0, 3}}, {{U'\x200d', U'\x1100'}, {0, 1, 2}},
    {{U'\x200d', U'\x308', U'\x1100'}, {0, 2, 3}}, {{U'\x200d', U'\x1160'}, {0, 1, 2}},
    {{U'\x200d', U'\x308', U'\x1160'}, {0, 2, 3}}, {{U'\x200d', U'\x11a8'}, {0, 1, 2}},
    {{U'\x200d', U'\x308', U'\x11a8'}, {0, 2, 3}}, {{U'\x200d', U'\xac00'}, {0, 1, 2}},
    {{U'\x200d', U'\x308', U'\xac00'}, {0, 2, 3}}, {{U'\x200d', U'\xac01'}, {0, 1, 2}},
    {{U'\x200d', U'\x308', U'\xac01'}, {0, 2, 3}}, {{U'\x200d', U'\x231a'}, {0, 1, 2}},
    {{U'\x200d', U'\x308', U'\x231a'}, {0, 2, 3}}, {{U'\x200d', U'\x300'}, {0, 2}},
    {{U'\x200d', U'\x308', U'\x300'}, {0, 3}}, {{U'\x200d', U'\x200d'}, {0, 2}},
    {{U'\x200d', U'\x308', U'\x200d'}, {0, 3}}, {{U'\x200d', U'\x378'}, {0, 1, 2}},
    {{U'\x200d', U'\x308', U'\x378'}, {0, 2, 3}}, {{U'\x378', U'\x20'}, {0, 1, 2}},
    {{U'\x378', U'\x308', U'\x20'}, {0, 2, 3}}, {{U'\x378', U'\xd'}, {0, 1, 2}},
    {{U'\x378', U'\x308', U'\xd'}, {0, 2, 3}}, {{U'\x378', U'\xa'}, {0, 1, 2}},
    {{U'\x378', U'\x308', U'\xa'}, {0, 2, 3}}, {{U'\x378', U'\x1'}, {0, 1, 2}},
    {{U'\x378', U'\x308', U'\x1'}, {0, 2, 3}}, {{U'\x378', U'\x34f'}, {0, 2}}, {{U'\x378', U'\x308', U'\x34f'}, {0, 3}},
    {{U'\x378', U'\x1f1e6'}, {0, 1, 2}}, {{U'\x378', U'\x308', U'\x1f1e6'}, {0, 2, 3}},
    {{U'\x378', U'\x600'}, {0, 1, 2}}, {{U'\x378', U'\x308', U'\x600'}, {0, 2, 3}}, {{U'\x378', U'\x903'}, {0, 2}},
    {{U'\x378', U'\x308', U'\x903'}, {0, 3}}, {{U'\x378', U'\x1100'}, {0, 1, 2}},
    {{U'\x378', U'\x308', U'\x1100'}, {0, 2, 3}}, {{U'\x378', U'\x1160'}, {0, 1, 2}},
    {{U'\x378', U'\x308', U'\x1160'}, {0, 2, 3}}, {{U'\x378', U'\x11a8'}, {0, 1, 2}},
    {{U'\x378', U'\x308', U'\x11a8'}, {0, 2, 3}}, {{U'\x378', U'\xac00'}, {0, 1, 2}},
    {{U'\x378', U'\x308', U'\xac00'}, {0, 2, 3}}, {{U'\x378', U'\xac01'}, {0, 1, 2}},
    {{U'\x378', U'\x308', U'\xac01'}, {0, 2, 3}}, {{U'\x378', U'\x231a'}, {0, 1, 2}},
    {{U'\x378', U'\x308', U'\x231a'}, {0, 2, 3}}, {{U'\x378', U'\x300'}, {0, 2}},
    {{U'\x378', U'\x308', U'\x300'}, {0, 3}}, {{U'\x378', U'\x200d'}, {0, 2}},
    {{U'\x378', U'\x308', U'\x200d'}, {0, 3}}, {{U'\x378', U'\x378'}, {0, 1, 2}},
    {{U'\x378', U'\x308', U'\x378'}, {0, 2, 3}}, {{U'\xd', U'\xa', U'\x61', U'\xa', U'\x308'}, {0, 2, 3, 4, 5}},
    {{U'\x61', U'\x308'}, {0, 2}}, {{U'\x20', U'\x200d', U'\x646'}, {0, 2, 3}},
    {{U'\x646', U'\x200d', U'\x20'}, {0, 2, 3}}, {{U'\x1100', U'\x1100'}, {0, 2}},
    {{U'\xac00', U'\x11a8', U'\x1100'}, {0, 2, 3}}, {{U'\xac01', U'\x11a8', U'\x1100'}, {0, 2, 3}},
    {{U'\x1f1e6', U'\x1f1e7', U'\x1f1e8', U'\x62'}, {0, 2, 3, 4}},
    {{U'\x61', U'\x1f1e6', U'\x1f1e7', U'\x1f1e8', U'\x62'}, {0, 1, 3, 4, 5}},
    {{U'\x61', U'\x1f1e6', U'\x1f1e7', U'\x200d', U'\x1f1e8', U'\x62'}, {0, 1, 4, 5, 6}},
    {{U'\x61', U'\x1f1e6', U'\x200d', U'\x1f1e7', U'\x1f1e8', U'\x62'}, {0, 1, 3, 5, 6}},
    {{U'\x61', U'\x1f1e6', U'\x1f1e7', U'\x1f1e8', U'\x1f1e9', U'\x62'}, {0, 1, 3, 5, 6}},
    {{U'\x61', U'\x200d'}, {0, 2}}, {{U'\x61', U'\x308', U'\x62'}, {0, 2, 3}},
    {{U'\x61', U'\x903', U'\x62'}, {0, 2, 3}}, {{U'\x61', U'\x600', U'\x62'}, {0, 1, 3}},
    {{U'\x1f476', U'\x1f3ff', U'\x1f476'}, {0, 2, 3}}, {{U'\x61', U'\x1f3ff', U'\x1f476'}, {0, 2, 3}},
    {{U'\x61', U'\x1f3ff', U'\x1f476', U'\x200d', U'\x1f6d1'}, {0, 2, 5}},
    {{U'\x1f476', U'\x1f3ff', U'\x308', U'\x200d', U'\x1f476', U'\x1f3ff'}, {0, 6}},
    {{U'\x1f6d1', U'\x200d', U'\x1f6d1'}, {0, 3}}, {{U'\x61', U'\x200d', U'\x1f6d1'}, {0, 2, 3}},
    {{U'\x2701', U'\x200d', U'\x2701'}, {0, 3}}, {{U'\x61', U'\x200d', U'\x2701'}, {0, 2, 3}}};


// End of generated data - DO NOT EDIT manually!

bool run_unicode_test_data_utf32() {
    for (const auto& test_case : test_data) {
        _Grapheme_break_property_iterator<char32_t> iter(
            test_case.code_points.data(), test_case.code_points.data() + test_case.code_points.size());
        size_t i = 0;
        while (iter != _Grapheme_break_property_sentinel{}) {
            assert(i < test_case.breaks.size());
            assert(*iter == test_case.code_points[test_case.breaks[i]]);
            ++iter;
            ++i;
        }
    }
    return true;
}

constexpr bool test_unicode_properties() {
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0xB)
           == _Grapheme_Break_property_values::_Control_value);
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0xC)
           == _Grapheme_Break_property_values::_Control_value);
    assert(
        _Grapheme_Break_property_data._Get_property_for_codepoint(0xA) == _Grapheme_Break_property_values::_LF_value);
    assert(
        _Grapheme_Break_property_data._Get_property_for_codepoint(0xD) == _Grapheme_Break_property_values::_CR_value);

    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0x7F)
           == _Grapheme_Break_property_values::_Control_value);
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0x80)
           == _Grapheme_Break_property_values::_Control_value);
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0x9F)
           == _Grapheme_Break_property_values::_Control_value);
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0x300)
           == _Grapheme_Break_property_values::_Extend_value);
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0x08D4)
           == _Grapheme_Break_property_values::_Extend_value);
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0xE01EF)
           == _Grapheme_Break_property_values::_Extend_value);
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0x1F1E6)
           == _Grapheme_Break_property_values::_Regional_Indicator_value);
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0x1F1FF)
           == _Grapheme_Break_property_values::_Regional_Indicator_value);
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0x0903)
           == _Grapheme_Break_property_values::_SpacingMark_value);
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0x1934)
           == _Grapheme_Break_property_values::_SpacingMark_value);
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0x1D16D)
           == _Grapheme_Break_property_values::_SpacingMark_value);
    assert(
        _Grapheme_Break_property_data._Get_property_for_codepoint(0x1100) == _Grapheme_Break_property_values::_L_value);
    assert(
        _Grapheme_Break_property_data._Get_property_for_codepoint(0xA97C) == _Grapheme_Break_property_values::_L_value);
    assert(
        _Grapheme_Break_property_data._Get_property_for_codepoint(0x1160) == _Grapheme_Break_property_values::_V_value);
    assert(
        _Grapheme_Break_property_data._Get_property_for_codepoint(0xD7C6) == _Grapheme_Break_property_values::_V_value);
    assert(
        _Grapheme_Break_property_data._Get_property_for_codepoint(0x11A8) == _Grapheme_Break_property_values::_T_value);
    assert(
        _Grapheme_Break_property_data._Get_property_for_codepoint(0xD7FB) == _Grapheme_Break_property_values::_T_value);
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0xAC00)
           == _Grapheme_Break_property_values::_LV_value);
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0xD788)
           == _Grapheme_Break_property_values::_LV_value);
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0xAC01)
           == _Grapheme_Break_property_values::_LVT_value);
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0xD7A3)
           == _Grapheme_Break_property_values::_LVT_value);
    assert(_Grapheme_Break_property_data._Get_property_for_codepoint(0x200D)
           == _Grapheme_Break_property_values::_ZWJ_value);
    assert(
        _Grapheme_Break_property_data._Get_property_for_codepoint(0x7E) == _Grapheme_Break_property_values::_No_value);
    assert(
        _Grapheme_Break_property_data._Get_property_for_codepoint(0xA0) == _Grapheme_Break_property_values::_No_value);


    // emoji-data
    assert(_Extended_Pictographic_property_data._Get_property_for_codepoint(0x2194)
           == _Extended_Pictographic_property_values::_Extended_Pictographic_value);

    // emoji_component, not extended_pictographic
    assert(_Extended_Pictographic_property_data._Get_property_for_codepoint(0x23)
           == _Extended_Pictographic_property_values::_No_value);
    return true;
}

template <typename CharT, size_t N_enc, size_t N_dec>
constexpr void test_utf_decode_helper(const CharT (&encoded)[N_enc], const char32_t (&decoded)[N_dec]) {
    static_assert(_Is_any_of_v<CharT, char, wchar_t>);
    assert(ranges::equal(_Unicode_codepoint_iterator(encoded, encoded + N_enc), _Unicode_codepoint_sentinel{}, decoded,
        decoded + N_dec));
}

constexpr bool test_utf8_decode() {
    test_utf_decode_helper("\xC0\xAF\xE0\x80\xBF\xF0\x81\x82\x41",
        {0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x41, 0x0});
    test_utf_decode_helper("\xED\xA0\x80\xED\xBF\xBF\xED\xAF\x41",
        {0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x41, 0x0});
    test_utf_decode_helper("\xF4\x91\x92\x93\xFF\x41\x80\xBF\x42",
        {0xFFFd, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x41, 0xFFFD, 0xFFFD, 0x42, 0x0});
    test_utf_decode_helper("\xE1\x80\xE2\xF0\x91\x92\xF1\xBF\x41", {0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x41, 0x0});
    test_utf_decode_helper("\xCE\xA9", {0x03A9, 0x0});
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
    run_unicode_test_data_utf32();

    test_unicode_properties();
    static_assert(test_unicode_properties());

    test_utf8_decode();
    static_assert(test_utf8_decode());

    test_utf16_decode();
    static_assert(test_utf16_decode());

    static_assert(forward_iterator<_Unicode_codepoint_iterator<char>>);
    static_assert(sentinel_for<_Unicode_codepoint_sentinel, _Unicode_codepoint_iterator<char>>);
    static_assert(forward_iterator<_Grapheme_break_property_iterator<char>>);
    return 0;
}
