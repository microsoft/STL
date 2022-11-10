# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# This script prints the output of test failures that were recorded by `stl-lit.py -o TEST_LOG_FILENAME`
# or `stl-lit.py --xunit-xml-output=TEST_RESULTS.xml`.

import json
import os
import sys
import xml.dom.minidom


if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit(f"Usage: python {sys.argv[0]} [TEST_LOG_FILENAME|TEST_RESULTS.xml]")
    filename = sys.argv[1]
    extension = os.path.splitext(filename)[1]
    with open(filename) as file:
        if extension.casefold() == ".xml".casefold():
            test_xml = xml.dom.minidom.parse(file)
            for testcase_elem in test_xml.getElementsByTagName("testcase"):
                for failure_elem in testcase_elem.getElementsByTagName("failure"):
                    print(f"name: {testcase_elem.getAttribute('classname')}")
                    print(f"output: {failure_elem.firstChild.data}")
                    print("==================================================")
        else:
            test_log = json.load(file)
            for result in test_log["tests"]:
                if not result["code"] in ["PASS", "UNSUPPORTED", "XFAIL"]:
                    print(f"code: {result['code']}")
                    # Ignore result["elapsed"].
                    print(f"name: {result['name']}")
                    # The JSON contains embedded CRLFs (which aren't affected by opening the file in text mode).
                    # If we don't replace these CRLFs with LFs here, this script will appear to be okay in the console,
                    # but redirecting it to a file will result in ugly double newlines.
                    print("output: {}".format(result["output"].replace("\r\n", "\n")))
                    print("==================================================")
