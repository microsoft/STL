// xcharconv_ryu_tables.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception


// Copyright 2018 Ulf Adams
// Copyright (c) Microsoft Corporation. All rights reserved.

// Boost Software License - Version 1.0 - August 17th, 2003

// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:

// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.


#pragma once
#ifndef _XCHARCONV_RYU_TABLES_H
#define _XCHARCONV_RYU_TABLES_H
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#include <cstdint>

#if !_HAS_CXX17
#error The contents of <charconv> are only available with C++17. (Also, you should not include this internal header.)
#endif // !_HAS_CXX17

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN

// https://github.com/ulfjack/ryu
// See xcharconv_ryu.h for the exact commit.
// (Keep the cgmanifest.json commitHash in sync.)

// clang-format off

// vvvvvvvvvv DERIVED FROM digit_table.h vvvvvvvvvv

// A table of all two-digit numbers. This is used to speed up decimal digit
// generation by copying pairs of digits into the final output.
template <class _CharT> inline constexpr _CharT __DIGIT_TABLE[] = {_CharT{}};

template <> inline constexpr char __DIGIT_TABLE<char>[200] = {
  '0','0','0','1','0','2','0','3','0','4','0','5','0','6','0','7','0','8','0','9',
  '1','0','1','1','1','2','1','3','1','4','1','5','1','6','1','7','1','8','1','9',
  '2','0','2','1','2','2','2','3','2','4','2','5','2','6','2','7','2','8','2','9',
  '3','0','3','1','3','2','3','3','3','4','3','5','3','6','3','7','3','8','3','9',
  '4','0','4','1','4','2','4','3','4','4','4','5','4','6','4','7','4','8','4','9',
  '5','0','5','1','5','2','5','3','5','4','5','5','5','6','5','7','5','8','5','9',
  '6','0','6','1','6','2','6','3','6','4','6','5','6','6','6','7','6','8','6','9',
  '7','0','7','1','7','2','7','3','7','4','7','5','7','6','7','7','7','8','7','9',
  '8','0','8','1','8','2','8','3','8','4','8','5','8','6','8','7','8','8','8','9',
  '9','0','9','1','9','2','9','3','9','4','9','5','9','6','9','7','9','8','9','9'
};

template <> inline constexpr wchar_t __DIGIT_TABLE<wchar_t>[200] = {
  L'0',L'0',L'0',L'1',L'0',L'2',L'0',L'3',L'0',L'4',L'0',L'5',L'0',L'6',L'0',L'7',L'0',L'8',L'0',L'9',
  L'1',L'0',L'1',L'1',L'1',L'2',L'1',L'3',L'1',L'4',L'1',L'5',L'1',L'6',L'1',L'7',L'1',L'8',L'1',L'9',
  L'2',L'0',L'2',L'1',L'2',L'2',L'2',L'3',L'2',L'4',L'2',L'5',L'2',L'6',L'2',L'7',L'2',L'8',L'2',L'9',
  L'3',L'0',L'3',L'1',L'3',L'2',L'3',L'3',L'3',L'4',L'3',L'5',L'3',L'6',L'3',L'7',L'3',L'8',L'3',L'9',
  L'4',L'0',L'4',L'1',L'4',L'2',L'4',L'3',L'4',L'4',L'4',L'5',L'4',L'6',L'4',L'7',L'4',L'8',L'4',L'9',
  L'5',L'0',L'5',L'1',L'5',L'2',L'5',L'3',L'5',L'4',L'5',L'5',L'5',L'6',L'5',L'7',L'5',L'8',L'5',L'9',
  L'6',L'0',L'6',L'1',L'6',L'2',L'6',L'3',L'6',L'4',L'6',L'5',L'6',L'6',L'6',L'7',L'6',L'8',L'6',L'9',
  L'7',L'0',L'7',L'1',L'7',L'2',L'7',L'3',L'7',L'4',L'7',L'5',L'7',L'6',L'7',L'7',L'7',L'8',L'7',L'9',
  L'8',L'0',L'8',L'1',L'8',L'2',L'8',L'3',L'8',L'4',L'8',L'5',L'8',L'6',L'8',L'7',L'8',L'8',L'8',L'9',
  L'9',L'0',L'9',L'1',L'9',L'2',L'9',L'3',L'9',L'4',L'9',L'5',L'9',L'6',L'9',L'7',L'9',L'8',L'9',L'9'
};

// ^^^^^^^^^^ DERIVED FROM digit_table.h ^^^^^^^^^^

// vvvvvvvvvv DERIVED FROM d2s_full_table.h vvvvvvvvvv

// These tables are generated by PrintDoubleLookupTable.
extern const uint64_t __DOUBLE_POW5_INV_SPLIT[292][2];

extern const uint64_t __DOUBLE_POW5_SPLIT[326][2];

// ^^^^^^^^^^ DERIVED FROM d2s_full_table.h ^^^^^^^^^^

// vvvvvvvvvv DERIVED FROM d2fixed_full_table.h vvvvvvvvvv

inline constexpr int __TABLE_SIZE = 64;

inline constexpr uint16_t __POW10_OFFSET[__TABLE_SIZE] = {
  0, 2, 5, 8, 12, 16, 21, 26, 32, 39,
  46, 54, 62, 71, 80, 90, 100, 111, 122, 134,
  146, 159, 173, 187, 202, 217, 233, 249, 266, 283,
  301, 319, 338, 357, 377, 397, 418, 440, 462, 485,
  508, 532, 556, 581, 606, 632, 658, 685, 712, 740,
  769, 798, 828, 858, 889, 920, 952, 984, 1017, 1050,
  1084, 1118, 1153, 1188
};

extern const uint64_t __POW10_SPLIT[1224][3];

inline constexpr int __TABLE_SIZE_2 = 69;
inline constexpr int __ADDITIONAL_BITS_2 = 120;

inline constexpr uint16_t __POW10_OFFSET_2[__TABLE_SIZE_2] = {
     0,    2,    6,   12,   20,   29,   40,   52,   66,   80,
    95,  112,  130,  150,  170,  192,  215,  240,  265,  292,
   320,  350,  381,  413,  446,  480,  516,  552,  590,  629,
   670,  712,  755,  799,  845,  892,  940,  989, 1040, 1092,
  1145, 1199, 1254, 1311, 1369, 1428, 1488, 1550, 1613, 1678,
  1743, 1810, 1878, 1947, 2017, 2088, 2161, 2235, 2311, 2387,
  2465, 2544, 2625, 2706, 2789, 2873, 2959, 3046, 3133
};

inline constexpr uint8_t __MIN_BLOCK_2[__TABLE_SIZE_2] = {
     0,    0,    0,    0,    0,    0,    1,    1,    2,    3,
     3,    4,    4,    5,    5,    6,    6,    7,    7,    8,
     8,    9,    9,   10,   11,   11,   12,   12,   13,   13,
    14,   14,   15,   15,   16,   16,   17,   17,   18,   19,
    19,   20,   20,   21,   21,   22,   22,   23,   23,   24,
    24,   25,   26,   26,   27,   27,   28,   28,   29,   29,
    30,   30,   31,   31,   32,   32,   33,   34,    0
};

extern const uint64_t __POW10_SPLIT_2[3133][3];

// ^^^^^^^^^^ DERIVED FROM d2fixed_full_table.h ^^^^^^^^^^

// clang-format on

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XCHARCONV_RYU_TABLES_H
