from pathlib import Path
from typing import List
import re


def parse_commented_file(filename: Path, comment_str: str = '#') -> List[str]:
    comment_regex = re.compile(comment_str + r".*", re.DOTALL)
    result = list()
    with Path(filename).open() as f:
        for line in f.readlines():
            line = comment_regex.sub("", line)
            if line is not None:
                result.append(line.strip())

    return result
