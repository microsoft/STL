# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from pathlib import PurePosixPath
import sys
from urllib.error import HTTPError
from urllib.request import urlretrieve


def get_base_url():
    if len(sys.argv) != 2:
        sys.exit(f"Usage: python {sys.argv[0]} [latest|<VERSION LIKE 15.0.0>]")

    version = sys.argv[1]

    if version == "latest":
        return "https://unicode.org/Public/UCD/latest/"

    return f"https://unicode.org/Public/{version}/"


Unicode_data_files = [
    "ucd/DerivedCoreProperties.txt",
    "ucd/extracted/DerivedGeneralCategory.txt",
    "ucd/EastAsianWidth.txt",
    "ucd/auxiliary/GraphemeBreakProperty.txt",
    "ucd/auxiliary/GraphemeBreakTest.txt",
    "ucd/emoji/emoji-data.txt",
]


def download_unicode_data_files():
    base_url = get_base_url()
    print(f"   Base URL: {base_url}")

    for data_file in Unicode_data_files:
        url = base_url + data_file
        filename = PurePosixPath(data_file).name
        print(f"Downloading: {url}")
        try:
            urlretrieve(url, filename)
        except HTTPError as http_error:
            sys.exit(f"{http_error}")


if __name__ == "__main__":
    download_unicode_data_files()
