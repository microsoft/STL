// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <chrono>
#include <cstdio>
#include <string>
#include <system_error>
using namespace std;
using namespace chrono;

int main() {
    string str{"abc"};
    error_code ec{2, system_category()};
    system_error syserr{ec};

    ambiguous_local_time alt{local_seconds{}, local_info{}};

    nonexistent_local_time nlt{local_seconds{}, local_info{}};

    printf("%s\n%s\n%s\n", syserr.what(), alt.what(), nlt.what());
}
