# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# The following code generates data for _Width_estimate_intervals_v2 in <format>.

import re
from enum import Enum
from typing import TextIO
from pathlib import Path


# Width estimation.
class UnicodeWidth(Enum):
    IS_1: int = 1
    IS_2: int = 2


class UnicodeWidthTable:
    # A valid Unicode code point won't exceed MAX_CODE_POINT.
    MAX_CODE_POINT: int = 0x10FFFF
    TABLE_SIZE: int = MAX_CODE_POINT + 1

    def __init__(self):
        self.table = [UnicodeWidth.IS_1] * (self.TABLE_SIZE)

    # "rng" denotes a right-closed range.
    def fill_range(self, rng: tuple, width: int):
        from_, to_ = rng
        assert from_ <= to_, "invalid range"
        assert to_ <= self.MAX_CODE_POINT, "invalid range"
        self.table[from_ : to_ + 1] = [width] * (to_ - from_ + 1)

    def width_estimate_intervals(self):
        """
        Divide [0..MAX_CODE_POINT] into ranges with different width estimations.
        Represent each range with their starting values.
        The starting value of the first range is always 0 and omitted.
        The width estimation should be 1 for the first range, then alternate between 2 and 1.
        """
        values = []
        assert self.table[0] == UnicodeWidth.IS_1
        for u in range(1, self.TABLE_SIZE):
            assert (
                self.table[u] == UnicodeWidth.IS_1
                or self.table[u] == UnicodeWidth.IS_2
            )
            if self.table[u] != self.table[u - 1]:
                values.append(u)

        return ", ".join([f"0x{u:X}u" for u in values])

    # Print all ranges (right-closed), where self's width is 1 and other's width is 2.
    def print_ranges_1_vs_2(self, other):
        def _1_vs_2(u: int):
            return (
                self.table[u] == UnicodeWidth.IS_1
                and other.table[u] == UnicodeWidth.IS_2
            )

        u = 0
        while u < self.TABLE_SIZE:
            if _1_vs_2(u):
                from_ = u
                to_ = from_
                while to_ + 1 < self.TABLE_SIZE and _1_vs_2(to_ + 1):
                    to_ += 1
                if from_ == to_:
                    print(f"U+{from_:X}")
                else:
                    print(f"U+{from_:X}..U+{to_:X}")
                u = to_
            u += 1


def get_table_cpp20() -> UnicodeWidthTable:
    std_wide_ranges_cpp20 = [
        (0x1100, 0x115F),
        (0x2329, 0x232A),
        (0x2E80, 0x303E),
        (0x3040, 0xA4CF),
        (0xAC00, 0xD7A3),
        (0xF900, 0xFAFF),
        (0xFE10, 0xFE19),
        (0xFE30, 0xFE6F),
        (0xFF00, 0xFF60),
        (0xFFE0, 0xFFE6),
        (0x1F300, 0x1F64F),
        (0x1F900, 0x1F9FF),
        (0x20000, 0x2FFFD),
        (0x30000, 0x3FFFD),
    ]

    table = UnicodeWidthTable()
    for rng in std_wide_ranges_cpp20:
        table.fill_range(rng, UnicodeWidth.IS_2)

    return table


def read_from(source: TextIO) -> UnicodeWidthTable:
    """
    Read data from "EastAsianWidth.txt".
    The latest version can be found at:
    https://www.unicode.org/Public/UCD/latest/ucd/EastAsianWidth.txt
    The current implementation works for:
    https://www.unicode.org/Public/15.1.0/ucd/EastAsianWidth.txt
    To make this function work, the file should not contain a BOM.
    """
    table = UnicodeWidthTable()

    # "The unassigned code points in the following blocks default to "W":"
    default_wide_ranges = [
        (0x4E00, 0x9FFF),
        (0x3400, 0x4DBF),
        (0xF900, 0xFAFF),
        (0x20000, 0x2FFFD),
        (0x30000, 0x3FFFD),
    ]
    for rng in default_wide_ranges:
        table.fill_range(rng, UnicodeWidth.IS_2)

    # Read explicitly assigned ranges.
    # The lines that are not empty or pure comment are uniformly of the format "HEX(..HEX)? ; (A|F|H|N|Na|W) #comment".
    LINE_REGEX = re.compile(r"([0-9A-Z]+)(\.\.[0-9A-Z]+)? *; *(A|F|H|N|Na|W) *#.*")

    def get_width(str: str):
        if str == "F" or str == "W":
            return UnicodeWidth.IS_2
        else:
            assert str == "A" or str == "H" or str == "N" or str == "Na"
            return UnicodeWidth.IS_1

    for line in source:
        line = line.strip()
        if line and not line.startswith("#"):
            match = LINE_REGEX.fullmatch(line)
            assert match, line # invalid line
            from_val = int(match.group(1), base=16)
            width = get_width(match.group(3))
            if match.group(2):
                # range (HEX..HEX)
                to_val = int(match.group(2)[2:], base=16)
                table.fill_range((from_val, to_val), width)
            else:
                # single character (HEX)
                table.table[from_val] = width

    return table


def get_table_cpp23(source: TextIO) -> UnicodeWidthTable:
    table = read_from(source)

    # Override with ranges specified by the C++ standard.
    std_wide_ranges_cpp23 = [
        (0x4DC0, 0x4DFF),
        (0x1F300, 0x1F5FF),
        (0x1F900, 0x1F9FF),
    ]

    for rng in std_wide_ranges_cpp23:
        table.fill_range(rng, UnicodeWidth.IS_2)

    return table


def main():
    print("Old table:")
    old_table = get_table_cpp20()
    print(old_table.width_estimate_intervals())

    path = Path(__file__).absolute().with_name("EastAsianWidth.txt")
    with open(path, mode="rt", encoding="utf-8") as source:
        filename = source.readline().replace("#", "//").rstrip()
        timestamp = source.readline().replace("#", "//").rstrip()
        new_table = get_table_cpp23(source)
    print("\n\nNew table:")
    print()
    print(filename)
    print(timestamp)
    print("inline constexpr char32_t _Width_estimate_intervals_v2[] = { //")
    print(new_table.width_estimate_intervals())
    print("};")

    print("\nWas 1, now 2:")
    old_table.print_ranges_1_vs_2(new_table)
    print("\nWas 2, now 1:")
    new_table.print_ranges_1_vs_2(old_table)


if __name__ == "__main__":
    main()
