from io import StringIO
from pathlib import Path
from enum import Enum
from dataclasses import dataclass, field
from typing import Optional, TextIO
import re


@dataclass
class PropertyRange:
    lower: int = -1
    upper: int = -1
    prop: str = None


@dataclass
class PropertyTable:
    lower_bounds: list[int] = field(default_factory=list)
    props_and_range: list[int] = field(default_factory=list)


LINE_REGEX = re.compile(
    r"^(?P<lower>[0-9A-F]{4,5})(?:\.\.(?P<upper>[0-9A-F]{4,5}))?\s*;\s*(?P<prop>\w+)")


def parsePropertyLine(inputLine: str) -> Optional[PropertyRange]:
    result = PropertyRange()
    if m := LINE_REGEX.match(inputLine):
        lower_str, upper_str, result.prop = m.group("lower", "upper", "prop")
        result.lower = int(lower_str, base=16)
        result.upper = result.lower
        if upper_str is not None:
            result.upper = int(upper_str, base=16)
        return result

    else:
        return None


PROP_VALUE_ENUMERATOR_TEMPLATE = "_{}_value"
PROP_VALUE_ENUM_TEMPLATE = """
enum class _{prop_name}_Property_Values : uint8_t {{
    {enumerators},
    _No_value = 255
}};
"""

DATA_ARRAY_TEMPLATE = """
static constexpr _Unicode_Property_Data<_{prop_name}_Property_Values, {size}> _{prop_name}_Property_Data{{
    {{{lower_bounds}}},
    {{{props_and_size}}}
}};
"""


def property_ranges_to_table(ranges: list[PropertyRange], props: list[str]) -> PropertyTable:
    result = PropertyTable()
    for range in sorted(ranges, key=lambda x: x.lower):
        result.lower_bounds.append(range.lower)
        size = (range.upper - range.lower) + 1
        assert size <= 0x0FFF
        prop_idx = props.index(range.prop)
        result.props_and_range.append(size | (prop_idx << 12))
    return result


def generate_cpp_data(prop_name: str, ranges: list[PropertyRange]) -> str:
    result = StringIO()
    prop_values = sorted(set(x.prop for x in ranges))
    table = property_ranges_to_table(ranges, prop_values)
    enumerator_values = [PROP_VALUE_ENUMERATOR_TEMPLATE.format(
        x) for x in prop_values]
    result.write(PROP_VALUE_ENUM_TEMPLATE.format(
        prop_name=prop_name, enumerators=",".join(enumerator_values)))
    result.write(DATA_ARRAY_TEMPLATE.format(prop_name=prop_name, size=len(table.lower_bounds),
                 lower_bounds=",".join(["0x" + format(x, 'x')
                                       for x in table.lower_bounds]),
                 props_and_size=",".join(["0x" + format(x, 'x') for x in table.props_and_range])))
    return result.getvalue()


if __name__ == "__main__":
    gbp_data_path = Path(__file__).absolute(
    ).with_name("GraphemeBreakProperty.txt")
    emoji_data_path = Path(__file__).absolute().with_name("emoji-data.txt")
    gbp_ranges = list()
    emoji_ranges = list()
    with gbp_data_path.open(encoding='utf-8') as f:
        gbp_ranges = [x for line in f if (x := parsePropertyLine(line))]
    with emoji_data_path.open(encoding='utf-8') as f:
        emoji_ranges = [x for line in f if (x := parsePropertyLine(line))]
    print(generate_cpp_data("Grapheme_Break", gbp_ranges))
    print(generate_cpp_data("Extended_Pictographic", [
          x for x in emoji_ranges if x.prop == "Extended_Pictographic"]))
