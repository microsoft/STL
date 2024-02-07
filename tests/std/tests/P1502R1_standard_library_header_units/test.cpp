// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// N4868 [headers]/4:
// "The headers listed in Table 21 [...] are collectively known as the importable C++ library headers.
// [Note 1: Importable C++ library headers can be imported as module units (10.3). - end note]"
// [tab:headers.cpp]: "Table 21: C++ library headers"

// This EXCLUDES the <cmeow> headers in:
// [tab:headers.cpp.c]: "Table 22: C++ headers for C library facilities"

import <algorithm>;
import <any>;
import <array>;
import <atomic>;
import <barrier>;
import <bit>;
import <bitset>;
import <charconv>;
import <chrono>;
import <codecvt>;
import <compare>;
import <complex>;
import <concepts>;
import <condition_variable>;
import <coroutine>;
import <deque>;
import <exception>;
import <execution>;
#if TEST_STANDARD >= 23
import <expected>;
#endif // TEST_STANDARD >= 23
import <filesystem>;
import <format>;
import <forward_list>;
import <fstream>;
import <functional>;
import <future>;
#if TEST_STANDARD >= 23
import <generator>;
#endif // TEST_STANDARD >= 23
import <initializer_list>;
import <iomanip>;
import <ios>;
import <iosfwd>;
import <iostream>;
import <istream>;
import <iterator>;
import <latch>;
import <limits>;
import <list>;
import <locale>;
import <map>;
#if TEST_STANDARD >= 23
import <mdspan>;
#endif // TEST_STANDARD >= 23
import <memory>;
import <memory_resource>;
import <mutex>;
import <new>;
import <numbers>;
import <numeric>;
import <optional>;
import <ostream>;
#if TEST_STANDARD >= 23
import <print>;
#endif // TEST_STANDARD >= 23
import <queue>;
import <random>;
import <ranges>;
import <ratio>;
import <regex>;
import <scoped_allocator>;
import <semaphore>;
import <set>;
import <shared_mutex>;
import <source_location>;
import <span>;
#if TEST_STANDARD >= 23
import <spanstream>;
#endif // TEST_STANDARD >= 23
import <sstream>;
import <stack>;
#if TEST_STANDARD >= 23
import <stacktrace>;
#endif // TEST_STANDARD >= 23
import <stdexcept>;
#if TEST_STANDARD >= 23
import <stdfloat>;
#endif // TEST_STANDARD >= 23
import <stop_token>;
import <streambuf>;
import <string>;
import <string_view>;
import <strstream>;
import <syncstream>;
import <system_error>;
import <thread>;
import <tuple>;
import <type_traits>;
import <typeindex>;
import <typeinfo>;
import <unordered_map>;
import <unordered_set>;
import <utility>;
import <valarray>;
import <variant>;
import <vector>;
import <version>;

#include <assert.h> // intentionally not <cassert>
#include <stdio.h> // intentionally not <cstdio>

#include <force_include.hpp>

// SPECIAL: This is order-dependent and must be included after all other Standard imports/includes.
#include <test_header_units_and_modules.hpp>

int main() {
    all_cpp_header_tests();
}
