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

def parseBreakTestLine(input: TextIO) -> Optional[BreakTestItem]:
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
template <typename T>
struct test_case_data {{
    vector<T> code_points;
    vector<size_t> breaks;
}};

template <typename T>
const test_case_data<T> test_data[{0}];

template <>
const test_case_data<char32_t> test_data<char32_t>[{0}] = {{
    {1}
}};

template <>
const test_case_data<char> test_data<char>[{0}] = {{
    {2}
}};
"""

cpp_test_data_line_template = "{{ {{{}}}, {{{}}} }}"

def lineToCppDataLineUtf32(line: BreakTestItem) -> str:
    return cpp_test_data_line_template.format(','.join(
        [f"U'\\x{x:x}'" for x in line.code_points]), ','.join(
        [str(x) for x in line.breaks]))

def lineToCppDataLineUtf8(line: BreakTestItem) -> str:
    utf8_rep = str(array('L', line.code_points),
                   encoding='utf-32').encode('utf-8')
    return cpp_test_data_line_template.format(','.join(
        [f"'\\x{x:x}'" for x in utf8_rep]
    ), ','.join([str(x) for x in line.breaks]))


"""
Generate test data from "GraphemeBreakText.txt"
This file can be downloaded from: https://www.unicode.org/Public/14.0.0/ucd/auxiliary/GraphemeBreakTest.txt
This script looks for GraphemeBreakTest.txt in same directory as this script
"""
def generate_all() -> str:
    test_data_path = Path(__file__)
    test_data_path = test_data_path.absolute()
    test_data_path = test_data_path.with_name("GraphemeBreakTest.txt")
    lines = list()
    with open(test_data_path, mode='rt', encoding='utf-8') as file:
        while line := parseBreakTestLine(file):
            if len(line.code_points) > 0:
                lines.append(line)
    return cpp_template.format(len(lines), ','.join(map(lineToCppDataLineUtf32, lines)),
        ','.join(map(lineToCppDataLineUtf8, lines)))

if __name__ == "__main__":
    print(generate_all())
