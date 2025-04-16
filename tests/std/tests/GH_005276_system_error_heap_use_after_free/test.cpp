// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <string>
#include <string_view>
#include <system_error>
using namespace std;
using namespace chrono;

int main() {
    string str{"abc"};
    error_code ec{2, system_category()};
    system_error syserr{ec};
    assert(syserr.what() == "system error"sv);

    ambiguous_local_time alt{local_seconds{}, local_info{}};
    assert(alt.what() == "ambiguous local time"sv);

    nonexistent_local_time nlt{local_seconds{}, local_info{}};
    assert(nlt.what() == "nonexistent local time"sv);
}
