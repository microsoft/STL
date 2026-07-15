// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <algorithm>
#include <filesystem>
#include <iterator>
#include <random>
#include <string>

template <class T>
std::string get_test_directory_subname(const T& testName) {
    using namespace std;
    random_device rd;
    uniform_int_distribution<> dist(0, 15);
    string subName(testName);
    subName.push_back('_');
    generate_n(back_inserter(subName), 32, [&] { return "0123456789ABCDEF"[dist(rd)]; });
    return subName;
}

#if _HAS_CXX17
#include <cassert>
#include <iostream>
#include <string_view>
#include <system_error>

inline std::filesystem::path get_test_directory(std::string_view testName) {
    return std::filesystem::temp_directory_path() / get_test_directory_subname(testName);
}

struct [[nodiscard]] test_temp_directory {
    const std::filesystem::path directoryPath;

    explicit test_temp_directory(const std::string_view testName) : directoryPath(get_test_directory(testName)) {
        std::error_code ec;
        std::filesystem::remove_all(directoryPath, ec);
        if (ec) {
            std::wcerr << L"Warning, couldn't clean up " << directoryPath << L" before test." << std::endl;
        } else {
            std::filesystem::create_directories(directoryPath, ec);
            assert(!ec);
        }
    }

    test_temp_directory(const test_temp_directory&)            = delete;
    test_temp_directory& operator=(const test_temp_directory&) = delete;

    ~test_temp_directory() noexcept {
        std::error_code ec;
        std::filesystem::remove_all(directoryPath, ec);
        if (ec) {
            std::wcerr << L"Warning, couldn't clean up " << directoryPath << L" after test." << std::endl;
        }
    }
};
#endif // _HAS_CXX17
