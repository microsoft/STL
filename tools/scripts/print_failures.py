# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# This script prints the output of test failures that were recorded by `stl-lit.py -o TEST_LOG_FILENAME`.

import json
import sys

if len(sys.argv) != 2:
    sys.exit(f"Usage: python {sys.argv[0]} TEST_LOG_FILENAME")

with open(sys.argv[1]) as file:
    test_log = json.load(file)
    for result in test_log["tests"]:
        if not result["code"] in ["PASS", "UNSUPPORTED", "XFAIL"]:
            print("code: {}".format(result["code"]))
            # Ignore result["elapsed"].
            print("name: {}".format(result["name"]))
            # The JSON contains embedded CRLFs (which aren't affected by opening the file in text mode).
            # If we don't replace these CRLFs with LFs here, this script will appear to be okay in the console,
            # but redirecting it to a file will result in ugly double newlines.
            print("output: {}".format(result["output"].replace("\r\n", "\n")))
            print("==================================================")
