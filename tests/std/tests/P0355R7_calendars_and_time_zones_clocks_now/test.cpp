// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
using namespace std::chrono;

#if _HAS_CXX17
#include <filesystem>
using namespace std::filesystem;
#endif

#if _HAS_CXX20
#include <Windows.h>

[[nodiscard]] sys_time<milliseconds> get_system_time() noexcept {
    SYSTEMTIME st;
    GetSystemTime(&st);
    return sys_days{year{st.wYear} / month{st.wMonth} / day{st.wDay}} + hours{st.wHour} + minutes{st.wMinute}
         + seconds{st.wSecond} + milliseconds{st.wMilliseconds};
}

template <typename Clock>
void test_clock_now() {
    // Typical system clock resolution: (1/64) s = 15.625 ms
    constexpr auto tolerance = 16ms;

    const auto before = get_system_time();
    const auto now    = time_point_cast<milliseconds>(clock_cast<system_clock>(Clock::now()));
    const auto after  = get_system_time();

    assert(before - tolerance <= now && now <= after + tolerance);
}
#endif // _HAS_CXX20

int main() {
#if _HAS_CXX20
    test_clock_now<system_clock>();
    test_clock_now<utc_clock>();
    test_clock_now<tai_clock>();
    test_clock_now<gps_clock>();
    test_clock_now<file_clock>();
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    (void) system_clock::now();
#if _HAS_CXX17
    (void) file_time_type::clock::now();
#endif // _HAS_CXX17
#endif // ^^^ !_HAS_CXX20 ^^^
    return 0;
}
