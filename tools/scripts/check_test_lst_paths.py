# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# This script checks tests/std/test.lst (and tr1) for duplicate, missing, nonexistent, and unsorted test paths.

from pathlib import Path
import sys

def is_nonempty_dir(p: Path) -> bool:
    return p.is_dir() and any(p.iterdir())

if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit(f"Usage: python {sys.argv[0]} tests/std-OR-tr1")

    absolute_base = Path(sys.argv[1]).absolute()
    absolute_tests = absolute_base / "tests"
    absolute_test_lst = absolute_base / "test.lst"
    relative_test_lst = absolute_test_lst.relative_to(absolute_test_lst.parents[2])

    with open(absolute_test_lst) as file:
        stripped_lines = [line.strip() for line in file]

    filtered_lines = [s for s in stripped_lines if s and not s.startswith("#")] # Ignore empty lines and comments.

    unique_tests = set()
    duplicate_tests = set()
    for line in filtered_lines:
        if line in unique_tests:
            duplicate_tests.add(line) # Repeatedly duplicated tests will be reported only once.
        else:
            unique_tests.add(line)

    existing_tests = {fR"tests\{p.name}" for p in absolute_tests.iterdir() if is_nonempty_dir(p)}

    commented_tests = { # These tests are commented out. Avoid reporting them as missing.
        R"tests\GH_000639_nvcc_include_all",
        R"tests\GH_002094_cpp_core_guidelines",
    }

    missing_tests = existing_tests - commented_tests - unique_tests

    nonexistent_tests = [str for str in unique_tests if not is_nonempty_dir(absolute_base / str)]

    failed = False

    if duplicate_tests:
        failed = True
        print(f"Failure: {relative_test_lst} contains {len(duplicate_tests)} duplicate tests:", file=sys.stderr)
        for str in sorted(duplicate_tests):
            print(f"{str}", file=sys.stderr)

    if missing_tests:
        failed = True
        print(f"Failure: {relative_test_lst} is missing {len(missing_tests)} tests:", file=sys.stderr)
        for str in sorted(missing_tests):
            print(f"{str}", file=sys.stderr)

    if nonexistent_tests:
        failed = True
        print(f"Failure: {relative_test_lst} contains {len(nonexistent_tests)} nonexistent tests:", file=sys.stderr)
        for str in sorted(nonexistent_tests):
            print(f"{str}", file=sys.stderr)

    for i in range(len(filtered_lines) - 1): # If filtered_lines is empty, range(-1) is empty.
        if filtered_lines[i] > filtered_lines[i + 1]: # Use greater-than to avoid reporting adjacent duplicates.
            failed = True
            print(f"Failure: {relative_test_lst} isn't sorted, starting with:", file=sys.stderr)
            print(f"{filtered_lines[i + 1]}", file=sys.stderr)
            break # Report only the first unsorted line.

    if failed:
        print(f"##vso[task.logissue type=error]{relative_test_lst} failed validation, see log.", file=sys.stderr)
        sys.exit(1)
