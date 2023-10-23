# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# This script prints the number of failing/skipped libcxx tests, ignoring configurations (plain/ASAN/Clang).

from enum import auto, Enum
import re
import sys

class CommentSeen(Enum):
    NEVER = auto()
    CURRENTLY = auto()
    PREVIOUSLY = auto()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit(f"Usage: python {sys.argv[0]} STL_REPO/tests/libcxx/expected_results.txt")

    missing_stl_features = CommentSeen.NEVER
    test_paths = set()

    with open(sys.argv[1]) as file:
        for line in map(lambda x: x.strip(), file):
            if line == "# *** MISSING STL FEATURES ***":
                if missing_stl_features == CommentSeen.NEVER:
                    missing_stl_features = CommentSeen.CURRENTLY
                else:
                    sys.exit("ERROR: Saw the MISSING STL FEATURES comment multiple times.")
            elif line.startswith("# *** "):
                if missing_stl_features == CommentSeen.CURRENTLY:
                    missing_stl_features = CommentSeen.PREVIOUSLY
            elif line.startswith("std/"):
                if missing_stl_features != CommentSeen.CURRENTLY:
                    test_paths.add(re.sub(r"(:\d+)? (FAIL|SKIPPED)$", "", line))

    if missing_stl_features == CommentSeen.NEVER:
        sys.exit("ERROR: Never saw the MISSING STL FEATURES comment.")

    print(f"libcxx: {len(test_paths)}")
