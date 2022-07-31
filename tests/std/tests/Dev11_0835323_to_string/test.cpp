// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <string>

using namespace std;

template <typename F>
void assert_out_of_range(F f) noexcept {
    try {
        f();
        abort();
    } catch (const out_of_range&) {
        // Good.
    }
}

// DevDiv-96290 "[Product Issue] to_string doesn't work with the param LDBL_MAX"
// DevDiv-730419 "<string>: std::to_string documentation & code disparity"
// DevDiv-835323 "<string>: Bad to_string() result for infinity"

int main() {
    assert(to_string(numeric_limits<int>::min()) == "-2147483648");
    assert(to_string(-1729) == "-1729");
    assert(to_string(0) == "0");
    assert(to_string(1729) == "1729");
    assert(to_string(numeric_limits<int>::max()) == "2147483647");

    assert(to_string(numeric_limits<unsigned int>::min()) == "0");
    assert(to_string(1729U) == "1729");
    assert(to_string(3000000000U) == "3000000000");
    assert(to_string(numeric_limits<unsigned int>::max()) == "4294967295");

    assert(to_string(numeric_limits<long>::min()) == "-2147483648");
    assert(to_string(-12345L) == "-12345");
    assert(to_string(0L) == "0");
    assert(to_string(12345L) == "12345");
    assert(to_string(numeric_limits<long>::max()) == "2147483647");

    assert(to_string(numeric_limits<unsigned long>::min()) == "0");
    assert(to_string(1729UL) == "1729");
    assert(to_string(4123456789UL) == "4123456789");
    assert(to_string(numeric_limits<unsigned long>::max()) == "4294967295");

    assert(to_string(numeric_limits<long long>::min()) == "-9223372036854775808");
    assert(to_string(-999999999999999999LL) == "-999999999999999999");
    assert(to_string(-2147483648LL) == "-2147483648");
    assert(to_string(0LL) == "0");
    assert(to_string(1729LL) == "1729");
    assert(to_string(2147483648LL) == "2147483648");
    assert(to_string(numeric_limits<long long>::max()) == "9223372036854775807");

    assert(to_string(numeric_limits<unsigned long long>::min()) == "0");
    assert(to_string(0x0FFFFFFFFULL) == "4294967295");
    assert(to_string(0x100000000ULL) == "4294967296");
    assert(to_string(36028797018963968ULL) == "36028797018963968");
    assert(to_string(numeric_limits<unsigned long long>::max()) == "18446744073709551615");

    assert(to_string(numeric_limits<float>::min()) == "0.000000");
    assert(to_string(1729.75f) == "1729.750000");
    assert(to_string(numeric_limits<float>::max()) == "340282346638528859811704183484516925440.000000");
    assert(to_string(numeric_limits<float>::infinity()) == "inf");

    assert(to_string(numeric_limits<double>::min()) == "0.000000");
    assert(to_string(47.734375) == "47.734375");
    assert(
        to_string(numeric_limits<double>::max())
        == "17976931348623157081452742373170435679807056752584499659891747680315726078002853876058955863276687817154045"
           "89535143824642343213268894641827684675467035375169860499105765512820762454900903893289440758685084551339423"
           "04583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000");
    assert(to_string(numeric_limits<double>::infinity()) == "inf");

    assert(to_string(numeric_limits<long double>::min()) == "0.000000");
    assert(to_string(1983.54296875L) == "1983.542969");
    assert(
        to_string(numeric_limits<long double>::max())
        == "17976931348623157081452742373170435679807056752584499659891747680315726078002853876058955863276687817154045"
           "89535143824642343213268894641827684675467035375169860499105765512820762454900903893289440758685084551339423"
           "04583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000");
    assert(to_string(numeric_limits<long double>::infinity()) == "inf");


    assert(to_wstring(numeric_limits<int>::min()) == L"-2147483648");
    assert(to_wstring(-1729) == L"-1729");
    assert(to_wstring(0) == L"0");
    assert(to_wstring(1729) == L"1729");
    assert(to_wstring(numeric_limits<int>::max()) == L"2147483647");

    assert(to_wstring(numeric_limits<unsigned int>::min()) == L"0");
    assert(to_wstring(1729U) == L"1729");
    assert(to_wstring(3000000000U) == L"3000000000");
    assert(to_wstring(numeric_limits<unsigned int>::max()) == L"4294967295");

    assert(to_wstring(numeric_limits<long>::min()) == L"-2147483648");
    assert(to_wstring(-12345L) == L"-12345");
    assert(to_wstring(0L) == L"0");
    assert(to_wstring(12345L) == L"12345");
    assert(to_wstring(numeric_limits<long>::max()) == L"2147483647");

    assert(to_wstring(numeric_limits<unsigned long>::min()) == L"0");
    assert(to_wstring(1729UL) == L"1729");
    assert(to_wstring(4123456789UL) == L"4123456789");
    assert(to_wstring(numeric_limits<unsigned long>::max()) == L"4294967295");

    assert(to_wstring(numeric_limits<long long>::min()) == L"-9223372036854775808");
    assert(to_wstring(-999999999999999999LL) == L"-999999999999999999");
    assert(to_wstring(-2147483648LL) == L"-2147483648");
    assert(to_wstring(0LL) == L"0");
    assert(to_wstring(1729LL) == L"1729");
    assert(to_wstring(2147483648LL) == L"2147483648");
    assert(to_wstring(numeric_limits<long long>::max()) == L"9223372036854775807");

    assert(to_wstring(numeric_limits<unsigned long long>::min()) == L"0");
    assert(to_wstring(0x0FFFFFFFFULL) == L"4294967295");
    assert(to_wstring(0x100000000ULL) == L"4294967296");
    assert(to_wstring(36028797018963968ULL) == L"36028797018963968");
    assert(to_wstring(numeric_limits<unsigned long long>::max()) == L"18446744073709551615");

    assert(to_wstring(numeric_limits<float>::min()) == L"0.000000");
    assert(to_wstring(1729.75f) == L"1729.750000");
    assert(to_wstring(numeric_limits<float>::max()) == L"340282346638528859811704183484516925440.000000");
    assert(to_wstring(numeric_limits<float>::infinity()) == L"inf");

    assert(to_wstring(numeric_limits<double>::min()) == L"0.000000");
    assert(to_wstring(47.734375) == L"47.734375");
    assert(
        to_wstring(numeric_limits<double>::max())
        == L"1797693134862315708145274237317043567980705675258449965989174768031572607800285387605895586327668781715404"
           L"5895351438246423432132688946418276846754670353751698604991057655128207624549009038932894407586850845513394"
           L"2304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000");
    assert(to_wstring(numeric_limits<double>::infinity()) == L"inf");

    assert(to_wstring(numeric_limits<long double>::min()) == L"0.000000");
    assert(to_wstring(1983.54296875L) == L"1983.542969");
    assert(
        to_wstring(numeric_limits<long double>::max())
        == L"1797693134862315708145274237317043567980705675258449965989174768031572607800285387605895586327668781715404"
           L"5895351438246423432132688946418276846754670353751698604991057655128207624549009038932894407586850845513394"
           L"2304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000");
    assert(to_wstring(numeric_limits<long double>::infinity()) == L"inf");


    // Also test DevDiv-875295 "<string>: std::stof returns 1.#INF instead of throwing out_of_range [libcxx]".

    assert_out_of_range([] { (void) stof("1.2e60"); });

    {
        const float f = stof("3.14");
        assert(3.13f < f && f < 3.15f);
    }

    assert_out_of_range([] { (void) stof("-1.3e61"); });

    assert_out_of_range([] { (void) stof(L"1.4e62"); });

    {
        const float f = stof(L"17.29");
        assert(17.28f < f && f < 17.30f);
    }

    assert_out_of_range([] { (void) stof(L"-1.5e63"); });


    // Also test DevDiv-1113936 "std::stod incorrectly throws exception on some inputs, violating STL specification".

    for (const char* const p : {"inf", "Inf", "INF", "infinity", "Infinity", "INFINITY"}) {
        assert(isinf(stof(p)));
        assert(isinf(stod(p)));
        assert(isinf(stold(p)));
    }

    for (const char* const p : {"nan", "Nan", "NaN", "NAN"}) {
        assert(isnan(stof(p)));
        assert(isnan(stod(p)));
        assert(isnan(stold(p)));
    }

    for (const wchar_t* const p : {L"inf", L"Inf", L"INF", L"infinity", L"Infinity", L"INFINITY"}) {
        assert(isinf(stof(p)));
        assert(isinf(stod(p)));
        assert(isinf(stold(p)));
    }

    for (const wchar_t* const p : {L"nan", L"Nan", L"NaN", L"NAN"}) {
        assert(isnan(stof(p)));
        assert(isnan(stod(p)));
        assert(isnan(stold(p)));
    }
}
