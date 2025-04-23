// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <string>
#include <string_view>
#include <system_error>
using namespace std;
using namespace chrono;

// GH-5276 <system_error>: heap-use-after-free for _HAS_EXCEPTIONS=0
int main() {
    const string str{"abc"};
    const error_code ec{2, system_category()};
    {
        system_error syserr1{ec};
        assert(syserr1.what() == "system error"sv);
    }
    {
        system_error syserr2{ec, str};
        assert(syserr2.what() == "system error"sv);
    }
    {
        system_error syserr3{ec, "meow"};
        assert(syserr3.what() == "system error"sv);
    }
    {
        system_error syserr4{2, system_category()};
        assert(syserr4.what() == "system error"sv);
    }
    {
        system_error syserr5{2, system_category(), str};
        assert(syserr5.what() == "system error"sv);
    }
    {
        system_error syserr6{2, system_category(), "meow"};
        assert(syserr6.what() == "system error"sv);
    }

    {
        ambiguous_local_time alt{local_seconds{}, local_info{}};
        assert(alt.what() == "ambiguous local time"sv);
    }
    {
        nonexistent_local_time nlt{local_seconds{}, local_info{}};
        assert(nlt.what() == "nonexistent local time"sv);
    }
}
