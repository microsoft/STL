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

    damaged_lines = set()
    unknown_results = set()
    unique_tests = set()

    syntax_rgx = re.compile(r"(?P<test>[^: ]+)(?P<config>:\d+)? (?P<result>\w+)")
    for line in filtered_lines:
        m = syntax_rgx.fullmatch(line)
        if m is None:
            damaged_lines.add(line)
        else:
            result = m.group("result")
            if result == "FAIL" or result == "SKIPPED":
                unique_tests.add(m.group("test"))
            else:
                unknown_results.add(line)

    # Build up a list of nonexistent tests so they can be printed in sorted order.
    absolute_libcxx_test = absolute_repo_path / "llvm-project/libcxx/test"
    nonexistent_tests = [s for s in unique_tests if not (absolute_libcxx_test / s).is_file()]

    failed = False

    if damaged_lines:
        failed = True
        print(f"Failure: {expected_results_txt} contains {len(damaged_lines)} damaged lines:", file=sys.stderr)
        print(f"(Double-check their syntax. Did you forget the result code at the end?)", file=sys.stderr)
        for s in sorted(damaged_lines):
            print(f"{s}", file=sys.stderr)

    if unknown_results:
        failed = True
        print(f"Failure: {expected_results_txt} contains {len(unknown_results)} unknown results:", file=sys.stderr)
        print(f"(Only FAIL and SKIPPED are known.)", file=sys.stderr)
        for s in sorted(unknown_results):
            print(f"{s}", file=sys.stderr)

    if nonexistent_tests:
        failed = True
        print(f"Failure: {expected_results_txt} contains {len(nonexistent_tests)} nonexistent tests:", file=sys.stderr)
        for s in sorted(nonexistent_tests):
            print(f"{s}", file=sys.stderr)

    if failed:
        print(f"##vso[task.logissue type=error]{expected_results_txt} failed validation, see log.", file=sys.stderr)
        sys.exit(1)
