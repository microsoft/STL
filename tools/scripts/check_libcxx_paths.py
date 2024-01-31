# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# This script checks tests/libcxx/expected_results.txt for nonexistent test paths.

from pathlib import Path
import re
import sys

if __name__ == "__main__":
    if len(sys.argv) != 1:
        sys.exit(f"Usage: python {sys.argv[0]}")

    # Use the location of this script to find the base of the repo.
    absolute_repo_path = Path(sys.argv[0]).absolute().parents[2]

    # Tests can be mentioned multiple times for different configurations.
    # Build up a unique set before checking for existence.
    unique_tests = set()

    with open(absolute_repo_path / "tests/libcxx/expected_results.txt") as file:
        for line in map(lambda x: x.strip(), file):
            if line and not line.startswith("#"): # Ignore empty lines and comments.
                unique_tests.add(re.sub(r"(:\d+)? (FAIL|SKIPPED)$", "", line))

    # Build up a list of nonexistent tests so they can be printed in sorted order.
    nonexistent_tests = []

    for str in unique_tests:
        if not (absolute_repo_path / "llvm-project/libcxx/test" / str).is_file():
            nonexistent_tests.append(str)

    if nonexistent_tests:
        print(f"Failure, found {len(nonexistent_tests)} nonexistent test paths:")
        nonexistent_tests.sort()
        for str in nonexistent_tests:
            print(f"{str}")
        sys.exit(1)

    print("Success, all test paths exist.")
