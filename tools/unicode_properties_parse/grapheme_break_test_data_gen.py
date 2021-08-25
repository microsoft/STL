# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from pathlib import Path
from dataclasses import dataclass, field
from typing import Optional, TextIO, Union
from enum import Enum


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
struct test_case_data {{
    vector<char32_t> code_points;
    vector<size_t> breaks;
}};
const test_case_data test_data[] = {{
    {}
}};
"""

cpp_test_data_line_template = "{{ {{{}}}, {{{}}} }}"


def lineToCppDataLine(line: BreakTestItem) -> str:
    return cpp_test_data_line_template.format(','.join(
        ["U'\\x" + format(x, 'x') + "'" for x in line.code_points]), ','.join(
        [str(x) for x in line.breaks]))


if __name__ == "__main__":
    test_data_path = Path(__file__)
    test_data_path = test_data_path.absolute()
    test_data_path = test_data_path.with_name("GraphemeBreakTest.txt")
    file = open(test_data_path, mode='rt', encoding='utf-8')
    lines = list()
    while line := parseBreakTestLine(file):
        if len(line.code_points) > 0:
            lines.append(line)
    print(cpp_template.format(','.join(map(lineToCppDataLine, lines))))
