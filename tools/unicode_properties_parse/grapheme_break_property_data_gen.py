from pathlib import Path
from enum import Enum
from dataclasses import dataclass
from typing import Optional, TextIO


@dataclass
class PropertyRange:
    lower: int
    upper: int
    prop: str


def parsePropertyLine(input: TextIO) -> Optional[PropertyRange]:
    result = PropertyRange()
    while True:
        c = input.read(1)


if __name__ == "__main__":
    gbp_data_path = Path(__file__).absolute(
    ).with_name("GraphemeBreakProperty.txt")
    emoji_data_path = Path(__file__).absolute().with_name("emoji-data.txt")
