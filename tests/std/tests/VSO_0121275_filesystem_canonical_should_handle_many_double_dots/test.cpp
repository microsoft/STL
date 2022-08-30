// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cwctype>
#include <experimental/filesystem>
#include <string>
#include <system_error>

#include <Windows.h>

using namespace std;
namespace fs = std::experimental::filesystem;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

bool path_names_equal(const wstring& lhs, const wstring& rhs) {
    // This isn't strictly the same as NTFS's rules; but getting NTFS rules would
    // require <windows.h>, and it's "close enough" as far as these tests care
    return equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
        [](wchar_t lhs, wchar_t rhs) { return towupper(lhs) == towupper(rhs); });
}

bool path_names_equal(const fs::path& lhs, const fs::path& rhs) {
    return path_names_equal(lhs.native(), rhs.native());
}

void assert_canonical_result(const fs::path& input, const fs::path& expected, const fs::path& actual, error_code ec) {
    if (ec) {
        printf("Test failed because canonicalizing \"%ls\" failed with \"%s\".\n", input.c_str(), ec.message().c_str());
        abort();
    }

    if (!path_names_equal(expected, actual)) {
        printf("Test failed because the canonical path of \"%ls\" should be \"%ls\" but was \"%ls\".\n", input.c_str(),
            expected.c_str(), actual.c_str());
        abort();
    }
}

void assert_canonical_path_too_long(const fs::path& input, const char* const testCase) {
    error_code ec;
    fs::path actual = fs::canonical(input, ec);
    if (actual != fs::path()) {
        printf("Test failed because long \"%s\" path canonical() didn't return an empty path.\n", testCase);
        abort();
    }

    if (ec != make_error_condition(errc::filename_too_long)) {
        printf("Test failed because long \"%s\" path canonical() returned \"%s\" instead of errc::filename_too_long.\n",
            testCase, ec.message().c_str());
        abort();
    }
}

void exec_test_case(const fs::path& expectedAnswer, const fs::path& input) {
    error_code ec;
    fs::path actualAnswer = fs::canonical(input, ec);
    assert_canonical_result(input, expectedAnswer, actualAnswer, ec);
}

void exec_test_case_string(const fs::path& expectedAnswer, const fs::path& input) {
    // This tests calling _Ugly stuff because we want to test the string based
    // fallback which is used if GetFinalPathNameByHandleW cannot be called.
    // Note that GetFinalPathNameByHandleW isn't being called at all until
    // VSO-158882 gets fixed in Dev15 (or later).
    fs::path canonicalPath;
    fs::path absPath = fs::absolute(input);
    _Canonicalize_string_only(canonicalPath, absPath);
    assert_canonical_result(input, expectedAnswer, canonicalPath, error_code());
}

void exec_test_input_path_too_long_behavior() {
    error_code ec;
    wstring input(LR"(C:\Path\That\Is\Too\Long\)");
    // 259 characters is the maximum length supported
    input.append(259 - input.size(), L'c');
    (void) fs::canonical(input, ec);
    if (ec == make_error_condition(errc::filename_too_long)) {
        puts("Test failed because canonicalizing max length string returned too long.");
        abort();
    }

    // 260+ characters should fail, because MAX_PATH includes the terminating
    // null character
    input.push_back(L'c');
    assert_canonical_path_too_long(input, "input 260");
    input.push_back(L'c');
    assert_canonical_path_too_long(input, "input 261");
}

STATIC_ASSERT(260 == MAX_PATH);

void exec_test_output_path_too_long_behavior() {
    // Make a long (> MAX_PATH) file, make a short (< MAX_PATH) symlink to it,
    // and verify that canonical() fails with filename_too_long
    wstring longName(LR"(\\?\)");
    longName.append(fs::current_path().native());
    longName.append(LR"(\path_longer_than_MAX_PATH)");
    while (longName.size() < MAX_PATH) {
        longName.append(L"_component");
    }

    longName.append(L".txt");

    HANDLE hTouched = ::CreateFileW(longName.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE, nullptr);

    if (hTouched == INVALID_HANDLE_VALUE) {
        const auto lastError = static_cast<unsigned int>(::GetLastError());
        printf("Test failed because long target file could not be created. (Error 0x%08X)\n", lastError);
        abort();
    }

    const wchar_t* const hardLinkFileName = L"Link_to_long_path.txt";
    if (::CreateSymbolicLinkW(hardLinkFileName, longName.c_str(), 0) == 0) {
        const auto lastError = static_cast<unsigned int>(::GetLastError());
        printf("Test failed because symlink to long name could not be created. (Error 0x%08X)\n", lastError);
        ::CloseHandle(hTouched);
        abort();
    }

    assert_canonical_path_too_long(hardLinkFileName, "output");
    assert(::DeleteFileW(hardLinkFileName) != 0);
    ::CloseHandle(hTouched);
}

struct test_case_example {
    fs::path expected;
    fs::path actual;
};

int main() {
    const fs::path cp = fs::current_path();

    const fs::path meow = cp / "..//////////" / cp.filename();

    const test_case_example examples[] = {
        {cp, "."},
        {cp, meow},
        {cp.parent_path(), "sub/../sub/sub2/../../.."},
        {cp.parent_path(), ".."},
        {cp, "sub/sub2/sub3/././././../sub4/./../../.."},
    };

    for (const auto& example : examples) {
        exec_test_case(example.expected, example.actual);
    }

    for (const auto& example : examples) {
        exec_test_case_string(example.expected, example.actual);
    }

    // These examples can't be done with the "real" canonical because we don't
    // want tests to fail when the network is down.
    const test_case_example string_only_examples[] = {{R"(\\network\)", R"(\\network\path\..\.\sub\..\)"},
        {R"(\\network\)", R"(\\network\path\..\.\sub\..\..\..\..\..)"},
        {R"(\\network\sub)", R"(\\network\path\..\.\sub\.\)"}};

    for (const auto& example : string_only_examples) {
        exec_test_case_string(example.expected, example.actual);
    }

    exec_test_input_path_too_long_behavior();
}
