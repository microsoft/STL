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
    expected_results_txt = "tests/libcxx/expected_results.txt"

    # Tests can be mentioned multiple times for different configurations.
    # Build up a unique set before checking for existence.
    with open(absolute_repo_path / expected_results_txt) as file:
        stripped_lines = [line.strip() for line in file]

    filtered_lines = [s for s in stripped_lines if s and not s.startswith("#")] # Ignore empty lines and comments.
    config_rgx = re.compile(r"(:\d+)? (FAIL|SKIPPED)$")
    unique_tests = {config_rgx.sub("", line) for line in filtered_lines}

    # Build up a list of nonexistent tests so they can be printed in sorted order.
    absolute_libcxx_test = absolute_repo_path / "llvm-project/libcxx/test"
    nonexistent_tests = [str for str in unique_tests if not (absolute_libcxx_test / str).is_file()]

    if nonexistent_tests:
        print(f"Failure: {expected_results_txt} contains {len(nonexistent_tests)} nonexistent tests:", file=sys.stderr)
        for str in sorted(nonexistent_tests):
            print(f"{str}", file=sys.stderr)
        print(f"##vso[task.logissue type=error]{expected_results_txt} failed validation, see log.", file=sys.stderr)
        sys.exit(1)
