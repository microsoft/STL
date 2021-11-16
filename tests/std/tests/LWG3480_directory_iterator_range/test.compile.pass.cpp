// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <filesystem>
#include <ranges>

using namespace std::filesystem;
using namespace std::ranges;

bool is_tiff(const directory_entry& entry) {
    return entry.is_regular_file() && entry.path().extension().native() == L"tif";
}

void test1(directory_iterator dir) {
    [[maybe_unused]] auto tif_files     = dir | views::filter(is_tiff);
    [[maybe_unused]] auto first_5_files = dir | views::take(5);
}

void test_recursive(recursive_directory_iterator dir) {
    [[maybe_unused]] auto tif_files     = dir | views::filter(is_tiff);
    [[maybe_unused]] auto first_5_files = dir | views::take(5);
}

int main() {} // COMPILE-ONLY
