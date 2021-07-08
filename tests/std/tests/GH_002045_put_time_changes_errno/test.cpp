// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <ctime>
#include <errno.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <time.h>

using namespace std;
using namespace chrono;

int main() {
    time_t t(system_clock::to_time_t(system_clock::now()));
    {
        ostringstream os;

        assert(errno == 0);

        std::tm tm;
        {
            const errno_t err = localtime_s(&tm, &t);
            assert(err == 0);
        }

        assert(errno == 0);

        os << put_time(&tm, "%Y-%m-%d %H:%M:%S %Z");

        assert(os);
        assert(errno == 0);
    }
    {
        wostringstream os;

        assert(errno == 0);

        std::tm tm;
        {
            const errno_t err = localtime_s(&tm, &t);
            assert(err == 0);
        }

        assert(errno == 0);

        os << put_time(&tm, L"%Y-%m-%d %H:%M:%S %Z");

        assert(os);
        assert(errno == 0);
    }
}
