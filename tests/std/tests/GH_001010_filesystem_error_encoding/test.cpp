// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <exception>
#include <filesystem>
#include <string_view>
#include <system_error>

using namespace std;
namespace fs = std::filesystem;

void test_filesystem_error_with_bad_codepage_characters() {
    fs::path problem_path{L"problematic\xD83Dtestpath"}; // path containing lone high surrogate

    // Make sure the path is not encodable in our current locale, otherwise the test tests nothing
    bool exception_caught = false;
    try {
        (void) problem_path.string();
    } catch (const exception&) {
        exception_caught = true;
    }
    assert(exception_caught);

    // filesystem_error should handle the non-encodable character gracefully when building its message
    fs::filesystem_error err{"testexception", problem_path, error_code{}};
    assert(string_view{err.what()}.find("problematic") != string_view::npos);
    assert(string_view{err.what()}.find("testpath") != string_view::npos);
}

int main() {
    test_filesystem_error_with_bad_codepage_characters();
}
