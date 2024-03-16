# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from pathlib import Path
from dataclasses import dataclass, field
from typing import Optional, TextIO
from array import array


@dataclass
class BreakTestItem:
    breaks: list[int] = field(default_factory=list)
    code_points: list[int] = field(default_factory=list)


class CommentLine:
    pass


class EOF:
    pass


def parse_break_test_line(input: TextIO) -> Optional[BreakTestItem]:
    result = BreakTestItem()
    while True:
        c = input.read(1)
        if c == '\N{DIVISION SIGN}':
            result.breaks.append(len(result.code_points))
            assert input.read(1).isspace()
            continue
        if c == '\N{MULTIPLICATION SIGN}':
            assert input.read(1).isspace()
            continue
        if c.isalnum():
            while next := input.read(1):
                if next.isalnum():
                    c += next
                else:
                    assert next.isspace()
                    break
            result.code_points.append(int(c, base=16))
            continue
        if c == '#':
            input.readline()
            return result
        if c == '\n':
            return result
        if c == '':
            return None
        assert False


cpp_template = """
{0}
{1}
template <typename T>
struct test_case_data {{
    vector<T> code_points;
    vector<size_t> breaks;
}};

template <typename T>
const test_case_data<T> test_data[{2}];

template <>
const test_case_data<char32_t> test_data<char32_t>[{2}] = {{
    {3}
}};

template <>
const test_case_data<char> test_data<char>[{2}] = {{
    {4}
}};
"""

cpp_test_data_line_template = "{{ {{{}}}, {{{}}} }}"

def line_to_cpp_data_line_utf32(line: BreakTestItem) -> str:
    return cpp_test_data_line_template.format(','.join(
        [f"U'\\x{x:x}'" for x in line.code_points]), ','.join(
        [str(x) for x in line.breaks]))


def line_to_cpp_data_line_utf8(line: BreakTestItem) -> str:
    utf8_rep = str(array('L', line.code_points),
                   encoding='utf-32').encode('utf-8')
    return cpp_test_data_line_template.format(','.join(
        [f"'\\x{x:x}'" for x in utf8_rep]
    ), ','.join([str(x) for x in line.breaks]))


"""
Generate test data from "GraphemeBreakTest.txt"
This file can be downloaded from: https://www.unicode.org/Public/UCD/latest/ucd/auxiliary/GraphemeBreakTest.txt
This script looks for GraphemeBreakTest.txt in same directory as this script
"""
def generate_all() -> str:
    test_data_path = Path(__file__).absolute().with_name("GraphemeBreakTest.txt")
    filename = ""
    timestamp = ""
    lines = list()
    with open(test_data_path, mode='rt', encoding='utf-8') as file:
        filename = file.readline().replace("#", "//").rstrip()
        timestamp = file.readline().replace("#", "//").rstrip()
        while line := parse_break_test_line(file):
            if len(line.code_points) > 0:
                lines.append(line)
    return cpp_template.format(filename, timestamp, len(lines), ','.join(map(line_to_cpp_data_line_utf32, lines)),
        ','.join(map(line_to_cpp_data_line_utf8, lines)))


if __name__ == "__main__":
    print(generate_all())
