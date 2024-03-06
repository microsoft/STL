# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from urllib.request import urlretrieve


Unicode_data_files = {
    "DerivedCoreProperties.txt": "https://www.unicode.org/Public/UCD/latest/ucd/DerivedCoreProperties.txt",
    "DerivedGeneralCategory.txt": "https://www.unicode.org/Public/UCD/latest/ucd/extracted/DerivedGeneralCategory.txt",
    "EastAsianWidth.txt": "https://www.unicode.org/Public/UCD/latest/ucd/EastAsianWidth.txt",
    "GraphemeBreakProperty.txt": "https://www.unicode.org/Public/UCD/latest/ucd/auxiliary/GraphemeBreakProperty.txt",
    "GraphemeBreakTest.txt": "https://www.unicode.org/Public/UCD/latest/ucd/auxiliary/GraphemeBreakTest.txt",
    "emoji-data.txt": "https://www.unicode.org/Public/UCD/latest/ucd/emoji/emoji-data.txt",
}

def download_unicode_data_files():
    for filename, url in Unicode_data_files.items():
        print(f"downloading {filename} from {url}")
        urlretrieve(url, filename)


if __name__ == "__main__":
    download_unicode_data_files()
