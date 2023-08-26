# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# The following code generates data for _Width_estimate_intervals_v2 in <format>.

import re
from dataclasses import dataclass
from typing import TextIO


@dataclass
class range_u:
    from_: int
    to_: int = None

    def __post_init__(self):
        if self.to_ is None:
            self.to_ = self.from_


class width_u:
    is_1: bool = False
    is_2: bool = True


class table_u:
    # A valid Unicode code point won't exceed max_u.
    max_u: int = 0x10FFFF

    def __init__(self):
        self.table = [width_u.is_1] * (self.max_u + 1)

    def fill_range(self, rng: range_u, width: bool):
        from_, to_ = rng.from_, rng.to_
        assert from_ <= to_, "impl assertion failed"
        assert to_ <= self.max_u, "impl assertion failed"
        for u in range(from_, to_ + 1):
            self.table[u] = width

    def print_intervals(self):
        # Print table for _Width_estimate_intervals_v2.
        c = 0
        last = self.table[0]
        for u in range(self.max_u + 1):
            if self.table[u] != last:
                print(f"0x{u:X}u, ", end="")
                if c == 11:
                    c = 0
                    print()
                else:
                    c += 1
            last = self.table[u]

        print()

    def print_clusters_1_vs_2(self, other):
        cluster_table = [False] * (self.max_u + 1)
        for u in range(self.max_u + 1):
            if self.table[u] == width_u.is_1 and other.table[u] == width_u.is_2:
                cluster_table[u] = True

        u = 0
        while u < self.max_u + 1:
            if cluster_table[u]:
                from_ = u
                to_ = from_
                while to_ + 1 <= self.max_u and cluster_table[to_ + 1]:
                    to_ += 1
                if from_ == to_:
                    print(f"U+{from_:X}")
                else:
                    print(f"U+{from_:X}..U+{to_:X}")
                u = to_
            u += 1


def get_table_cpp20() -> table_u:
    std_wide_ranges_cpp20 = [
        range_u(0x1100, 0x115F),
        range_u(0x2329, 0x232A),
        range_u(0x2E80, 0x303E),
        range_u(0x3040, 0xA4CF),
        range_u(0xAC00, 0xD7A3),
        range_u(0xF900, 0xFAFF),
        range_u(0xFE10, 0xFE19),
        range_u(0xFE30, 0xFE6F),
        range_u(0xFF00, 0xFF60),
        range_u(0xFFE0, 0xFFE6),
        range_u(0x1F300, 0x1F64F),
        range_u(0x1F900, 0x1F9FF),
        range_u(0x20000, 0x2FFFD),
        range_u(0x30000, 0x3FFFD),
    ]

    table = table_u()
    for rng in std_wide_ranges_cpp20:
        table.fill_range(rng, width_u.is_2)

    return table


# Read data from "EastAsianWidth.txt".
# The latest version can be found at:
# https://www.unicode.org/Public/UCD/latest/ucd/EastAsianWidth.txt
# The current implementation works for:
# https://www.unicode.org/Public/15.0.0/ucd/EastAsianWidth.txt
# To make this function work, the file should not contain a BOM.
def read_from(source: TextIO) -> table_u:
    table = table_u()

    # "The unassigned code points in the following blocks default to "W":"
    default_wide_ranges = [
        range_u(0x4E00, 0x9FFF),
        range_u(0x3400, 0x4DBF),
        range_u(0xF900, 0xFAFF),
        range_u(0x20000, 0x2FFFD),
        range_u(0x30000, 0x3FFFD),
    ]
    for rng in default_wide_ranges:
        table.fill_range(rng, width_u.is_2)

    # Read explicitly assigned ranges.
    # The lines that are not empty or pure comment are uniformly of the format "HEX(..HEX)?;(A|F|H|N|Na|W) #comment".
    def get_width(str: str):
        if str == "F" or str == "W":
            return width_u.is_2
        else:
            assert str == "A" or str == "H" or str == "N" or str == "Na"
            return width_u.is_1

    if not source:
        raise ValueError("invalid path")
    reg = re.compile(r"([0-9A-Z]+)(\.\.[0-9A-Z]+)?;(A|F|H|N|Na|W) *#.*")
    for line in source:
        line = line.strip()
        if line and not line.startswith("#"):
            match = reg.fullmatch(line)
            assert match, "invalid line"
            from_val = int(match.group(1), base=16)
            width = get_width(match.group(3))
            if match.group(2):
                # range (HEX..HEX)
                to_val = int(match.group(2)[2:], base=16)
                table.fill_range(range_u(from_val, to_val), width)
            else:
                # single character (HEX)
                table.fill_range(range_u(from_val), width)

    return table


def get_table_cpp23(source: TextIO) -> table_u:
    table = read_from(source)

    # Override with ranges specified by the C++ standard.
    std_wide_ranges_cpp23 = [
        range_u(0x4DC0, 0x4DFF),
        range_u(0x1F300, 0x1F5FF),
        range_u(0x1F900, 0x1F9FF),
    ]

    for rng in std_wide_ranges_cpp23:
        table.fill_range(rng, width_u.is_2)

    return table


def main():
    print("Old table:")
    old_table = get_table_cpp20()
    old_table.print_intervals()

    print("\nNew table:\nInput path for EastAsianWidth.txt: ", end="")
    path = input()
    with open(path) as source:
        new_table = get_table_cpp23(source)
    new_table.print_intervals()

    print("\nWas 1, now 2:")
    old_table.print_clusters_1_vs_2(new_table)
    print("\nWas 2, now 1:")
    new_table.print_clusters_1_vs_2(old_table)


if __name__ == "__main__":
    main()
