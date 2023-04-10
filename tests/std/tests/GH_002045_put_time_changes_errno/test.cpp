// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cerrno>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace chrono;

tm tm_now() {
    const time_t t(system_clock::to_time_t(system_clock::now()));

    tm tm_val;

    const errno_t err = localtime_s(&tm_val, &t);
    assert(err == 0);

    return tm_val;
}

int main() {
    const tm tm_val = tm_now();

    {
        ostringstream os;

        assert(errno == 0);

        os << put_time(&tm_val, "%Y-%m-%d %H:%M:%S %Z");

        assert(os);
        assert(errno == 0);
    }
    {
        wostringstream os;

        assert(errno == 0);

        os << put_time(&tm_val, L"%Y-%m-%d %H:%M:%S %Z");

        assert(os);
        assert(errno == 0);
    }
}
