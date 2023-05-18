// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// derived from libc++'s test files:
// * std/localization/locale.categories/category.numeric/locale.num.get/facet.num.get.members/get_float.pass.cpp
// * std/localization/locale.categories/category.numeric/locale.num.get/facet.num.get.members/get_double.pass.cpp

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <ios>
#include <locale>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

#if _HAS_CXX17
#include <charconv>

#include "../P0067R5_charconv/test.hpp"
// ^^^ needs to be included first ^^^
#include "../P0067R5_charconv/double_from_chars_test_cases.hpp"
#include "../P0067R5_charconv/float_from_chars_test_cases.hpp"
#endif // _HAS_CXX17

using namespace std;

class my_facet : public num_get<char, const char*> {
public:
    explicit my_facet(size_t refs = 0) : num_get<char, const char*>(refs) {}
};

class my_numpunct : public numpunct<char> {
public:
    my_numpunct() : numpunct<char>() {}

protected:
    char_type do_decimal_point() const override {
        return ';';
    }
    char_type do_thousands_sep() const override {
        return '_';
    }
    string do_grouping() const override {
        return "\1\2\3"s;
    }
};

template <class Flt>
void test() {
    const my_facet f(1);
    ios instr(nullptr);
    Flt v = 0;
    {
        const char str[] = "123";
        assert((instr.flags() & instr.basefield) == instr.dec);
        assert(instr.getloc().name() == "C");
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(str, str + sizeof(str), instr, err, v);
        assert(iter == str + sizeof(str) - 1);
        assert(err == instr.goodbit);
        assert(v == 123);
    }
    {
        const char str[]      = "-123";
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(str, str + sizeof(str), instr, err, v);
        assert(iter == str + sizeof(str) - 1);
        assert(err == instr.goodbit);
        assert(v == -123);
    }
    {
        const char str[]      = "123.5";
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(str, str + sizeof(str), instr, err, v);
        assert(iter == str + sizeof(str) - 1);
        assert(err == instr.goodbit);
        assert(v == 123.5);
    }
    {
        const char str[] = "125e-1";
        hex(instr);
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(str, str + sizeof(str), instr, err, v);
        assert(iter == str + sizeof(str) - 1);
        assert(err == instr.goodbit);
        assert(v == 125e-1);
    }
    {
        const char str[] = "0x125p-1";
        hex(instr);
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(str, str + sizeof(str), instr, err, v);
        assert(iter == str + sizeof(str) - 1);
        assert(err == instr.goodbit);
        assert(v == static_cast<double>(0x125) * 0.5);
    }
    {
        v                     = -1;
        const char str[]      = "123_456_78_9;125";
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(str, str + sizeof(str), instr, err, v);
        assert(iter == str + 3);
        assert(err == instr.goodbit);
        assert(v == 123);
    }
    {
        v                     = -1;
        const char str[]      = "2-";
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(str, str + sizeof(str), instr, err, v);
        assert(iter == str + 1);
        assert(err == instr.goodbit);
        assert(v == 2);
    }
    {
        v                     = -1;
        const char* inf_str   = is_same_v<Flt, float> ? "3.40283e+39" : "1.7979e+309"; // unrepresentable
        const size_t len      = strlen(inf_str);
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(inf_str, inf_str + len + 1, instr, err, v);
        assert(iter == inf_str + len);
        assert(err == instr.failbit);
        assert(v == HUGE_VAL);
    }
    {
        v                     = -1;
        const char* inf_str   = is_same_v<Flt, float> ? "-3.40283e+39" : "-1.7979e+309"; // unrepresentable
        const size_t len      = strlen(inf_str);
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(inf_str, inf_str + len + 1, instr, err, v);
        assert(iter == inf_str + len);
        assert(err == instr.failbit);
        assert(v == -HUGE_VAL);
    }

    instr.imbue(locale(locale(), new my_numpunct));
    {
        v                     = -1;
        const char* sep_str   = is_same_v<Flt, float> ? "456_78_9;5" : "123_456_78_9;125";
        const size_t len      = strlen(sep_str);
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == (is_same_v<Flt, float> ? 456789.5 : 123456789.125));
    }
    {
        v                     = -1;
        const char str[]      = "1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_"
                                "1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_"
                                "1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_"
                                "1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_"
                                "1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_"
                                "1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_"
                                "1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_"
                                "1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_"
                                "1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_"
                                "1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_"
                                "1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_1_2_3_4_5_6_7_8_9_0_";
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(str, str + sizeof(str), instr, err, v);
        assert(iter == str + sizeof(str) - 1);
        assert(err == instr.failbit);
    }
    {
        v                     = -1;
        const char str[]      = "3;"
                                "14159265358979323846264338327950288419716939937510582097494459230781640628620899862803"
                                "482534211706798214808651e+10";
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(str, str + sizeof(str), instr, err, v);
        assert(iter == str + sizeof(str) - 1);
        assert(err == instr.goodbit);
        assert(abs(v - 3.14159265358979e+10) / 3.14159265358979e+10 < 1.e-8);
    }
    {
        v                = -1;
        const char str[] = "0x125p-1 ";
        istringstream str_instr(str);
        str_instr >> v;
        assert(v == 0x125p-1);
        assert(str_instr.good());
    }

    // "0\n" and its friends
    {
        v                = -1;
        const char str[] = "0 ";
        istringstream str_instr(str);
        str_instr >> v;
        assert(v == 0);
        assert(str_instr.good());
    }
    {
        v                = -1;
        const char str[] = "+0 ";
        istringstream str_instr(str);
        str_instr >> v;
        assert(v == 0);
        assert(str_instr.good());
    }
    {
        v                = -1;
        const char str[] = "-0 ";
        istringstream str_instr(str);
        str_instr >> v;
        assert(v == 0);
        assert(str_instr.good());
    }
    {
        v                = -1;
        const char str[] = "0\n";
        istringstream str_instr(str);
        str_instr >> v;
        assert(v == 0);
        assert(str_instr.good());
    }
    {
        v                = -1;
        const char str[] = "+0\n";
        istringstream str_instr(str);
        str_instr >> v;
        assert(v == 0);
        assert(str_instr.good());
    }
    {
        v                = -1;
        const char str[] = "-0\n";
        istringstream str_instr(str);
        str_instr >> v;
        assert(v == 0);
        assert(str_instr.good());
    }
}

// Also test GH-1582 <xlocnum>: "multiply by power of 10" logic is imprecise
template <class Flt>
void test_gh_1582() {
    constexpr size_t digit_counts[]{200, 400, 800, 1600, 3200, 6400, 12800};
    for (const size_t n : digit_counts) {
        {
            istringstream iss{"1" + string(n, '0') + "e-" + to_string(n)};
            Flt x{};
            assert(iss >> x);
            assert(x == 1.0);
        }
        {
            istringstream iss{"0." + string(n, '0') + "1e+" + to_string(n + 1)};
            Flt x{};
            assert(iss >> x);
            assert(x == 1.0);
        }
        {
            istringstream iss{"0." + string(n, '0') + "1e" + to_string(n + 1)};
            Flt x{};
            assert(iss >> x);
            assert(x == 1.0);
        }
        {
            istringstream iss{"2" + string(n, '0') + "e-" + to_string(n)};
            Flt x{};
            assert(iss >> x);
            assert(x == 2.0);
        }
        {
            istringstream iss{"0." + string(n, '0') + "2e+" + to_string(n + 1)};
            Flt x{};
            assert(iss >> x);
            assert(x == 2.0);
        }
        {
            istringstream iss{"0." + string(n, '0') + "2e" + to_string(n + 1)};
            Flt x{};
            assert(iss >> x);
            assert(x == 2.0);
        }
        {
            istringstream iss{"0x1" + string(n, '0') + "p-" + to_string(n * 4)};
            Flt x{};
            assert(iss >> x);
            assert(x == 0x1.0p0);
        }
        {
            istringstream iss{"0x0." + string(n, '0') + "1p+" + to_string(n * 4 + 4)};
            Flt x{};
            assert(iss >> x);
            assert(x == 0x1.0p0);
        }
        {
            istringstream iss{"0x0." + string(n, '0') + "1p" + to_string(n * 4 + 4)};
            Flt x{};
            assert(iss >> x);
            assert(x == 0x1.0p0);
        }
        {
            istringstream iss{"0xA" + string(n, '0') + "p-" + to_string(n * 4)};
            Flt x{};
            assert(iss >> x);
            assert(x == 0xA.0p0);
        }
        {
            istringstream iss{"0x0." + string(n, '0') + "Ap+" + to_string(n * 4 + 4)};
            Flt x{};
            assert(iss >> x);
            assert(x == 0xA.0p0);
        }
        {
            istringstream iss{"0x0." + string(n, '0') + "Ap" + to_string(n * 4 + 4)};
            Flt x{};
            assert(iss >> x);
            assert(x == 0xA.0p0);
        }
    }
}

// Also test GH-3375 <xlocnum>: Incorrect rounding when parsing long hexadecimal floating point numbers just above
// midpoints
template <class Flt>
void test_gh_3375() {
    // Ensure long hexadecimal FP representations just above midpoints are correctly parsed.
    if (is_same_v<Flt, float>) {
        {
            istringstream stream("0x1.000001" + string(800, '0') + "1p+0");
            Flt x = 0.0;

            stream >> x;
            assert(static_cast<bool>(stream));
            assert((ostringstream{} << hexfloat << x).str() == "0x1.0000020000000p+0");
        }
        {
            istringstream stream("-0x1.000001" + string(800, '0') + "1p+0");
            Flt x = 0.0;

            stream >> x;
            assert(static_cast<bool>(stream));
            assert((ostringstream{} << hexfloat << x).str() == "-0x1.0000020000000p+0");
        }
    } else {
        {
            istringstream stream("0x1.00000000000008" + string(800, '0') + "1p+0");
            Flt x = 0.0;

            stream >> x;
            assert(static_cast<bool>(stream));
            assert((ostringstream{} << hexfloat << x).str() == "0x1.0000000000001p+0");
        }
        {
            istringstream stream("-0x1.00000000000008" + string(800, '0') + "1p+0");
            Flt x = 0.0;

            stream >> x;
            assert(static_cast<bool>(stream));
            assert((ostringstream{} << hexfloat << x).str() == "-0x1.0000000000001p+0");
        }
    }
}

// Also test GH-3376 <xlocnum>: Incorrect result when parsing 9.999999...
template <class Flt>
void test_gh_3376() {

    // Ensure that "0.0999....999" is still correctly parsed.
    {
        istringstream stream("0.09" + string(800, '9'));
        Flt x = 0.0;

        stream >> x;
        assert(static_cast<bool>(stream));
        assert((ostringstream{} << x).str() == "0.1");
    }
    {
        istringstream stream("-0.09" + string(800, '9'));
        Flt x = 0.0;

        stream >> x;
        assert(static_cast<bool>(stream));
        assert((ostringstream{} << x).str() == "-0.1");
    }

    // Ensure that "8.999....999" is still correctly parsed.
    {
        istringstream stream("8." + string(800, '9'));
        Flt x = 0.0;

        stream >> x;
        assert(static_cast<bool>(stream));
        assert((ostringstream{} << x).str() == "9");
    }
    {
        istringstream stream("-8." + string(800, '9'));
        Flt x = 0.0;

        stream >> x;
        assert(static_cast<bool>(stream));
        assert((ostringstream{} << x).str() == "-9");
    }

    // Ensure that "0.999...999" and its friends are correctly parsed.
    {
        istringstream stream("0." + string(800, '9'));
        Flt x = 0.0;

        stream >> x;
        assert(static_cast<bool>(stream));
        assert((ostringstream{} << x).str() == "1");
    }
    {
        istringstream stream("-0." + string(800, '9'));
        Flt x = 0.0;

        stream >> x;
        assert(static_cast<bool>(stream));
        assert((ostringstream{} << x).str() == "-1");
    }
    {
        istringstream stream("9." + string(800, '9'));
        Flt x = 0.0;

        stream >> x;
        assert(static_cast<bool>(stream));
        assert((ostringstream{} << x).str() == "10");
    }
    {
        istringstream stream("-9." + string(800, '9'));
        Flt x = 0.0;

        stream >> x;
        assert(static_cast<bool>(stream));
        assert((ostringstream{} << x).str() == "-10");
    }

    // Ensure that huge "999...999.999...999" representations are correctly parsed.
    {
        istringstream stream(string(38, '9') + "." + string(800, '9'));
        Flt x = 0.0;

        stream >> x;
        assert(static_cast<bool>(stream));
        assert((ostringstream{} << x).str() == "1e+38");
    }
    {
        istringstream stream("-" + string(38, '9') + "." + string(800, '9'));
        Flt x = 0.0;

        stream >> x;
        assert(static_cast<bool>(stream));
        assert((ostringstream{} << x).str() == "-1e+38");
    }
    {
        istringstream stream(string(308, '9') + "." + string(800, '9'));
        Flt x = 0.0;

        stream >> x;
        if (is_same_v<Flt, float>) {
            assert(!stream);
            assert(x == HUGE_VALF);
        } else {
            assert(static_cast<bool>(stream));
            assert((ostringstream{} << x).str() == "1e+308");
        }
    }
    {
        istringstream stream("-" + string(308, '9') + "." + string(800, '9'));
        Flt x = 0.0;

        stream >> x;
        if (is_same_v<Flt, float>) {
            assert(!stream);
            assert(x == -HUGE_VALF);
        } else {
            assert(static_cast<bool>(stream));
            assert((ostringstream{} << x).str() == "-1e+308");
        }
    }
}

// Also test GH-3378: <xlocnum>: Incorrect rounding when parsing long floating point numbers just below midpoints
template <class Flt>
void test_gh_3378() {
    {
        // just below 2^-1022 + 2^-1074 + 2^-1075
        istringstream stream(
            "2."
            "2250738585072021241887014792022203290724052827943903781430313383743510731924419468675440643256388185138218"
            "8218502438069999947733013005649884107791928741341929297200970481951993067993290969042784064731682041565926"
            "7286329336304746701233168529834221527445172608358596545663192828352447877877998943107797838336991592885945"
            "5521371418112845825114558431922307989750439508685941245723089173894616936837232119137365897797772328669884"
            "0356390251044443035457396733706583981055420456693824658413747607155981176573877626747665912387199931904006"
            "3173347090030127901881752034471902500280612777779167983910905785840064647159438105114891542827750411746821"
            "9413395246668250343130618158782937900420539237507208336669324158000275839111885418864151316847843631308023"
            "75962957739830017089843749e-308");

        ostringstream os;
        os.precision(17);

        Flt x = 0.0;
        stream >> x;
        assert(static_cast<bool>(stream));
        assert((move(os) << x).str() == "2.2250738585072019e-308");
    }
    {
        // negative case
        istringstream stream(
            "-2."
            "2250738585072021241887014792022203290724052827943903781430313383743510731924419468675440643256388185138218"
            "8218502438069999947733013005649884107791928741341929297200970481951993067993290969042784064731682041565926"
            "7286329336304746701233168529834221527445172608358596545663192828352447877877998943107797838336991592885945"
            "5521371418112845825114558431922307989750439508685941245723089173894616936837232119137365897797772328669884"
            "0356390251044443035457396733706583981055420456693824658413747607155981176573877626747665912387199931904006"
            "3173347090030127901881752034471902500280612777779167983910905785840064647159438105114891542827750411746821"
            "9413395246668250343130618158782937900420539237507208336669324158000275839111885418864151316847843631308023"
            "75962957739830017089843749e-308");

        ostringstream os;
        os.precision(17);

        Flt x = 0.0;
        stream >> x;
        assert(static_cast<bool>(stream));
        assert((move(os) << x).str() == "-2.2250738585072019e-308");
    }
}

#if _HAS_CXX17
void test_float_from_char_cases() {
    for (const auto& test_case : float_from_chars_test_cases) {
        auto repstr = test_case.fmt == chars_format::hex ? "0x" + string(test_case.input) : string(test_case.input);
        istringstream is(repstr);
        float x = 0.0f;

        const bool expected_no_err = test_case.correct_ec == errc{};
        const bool tested_no_err   = static_cast<bool>(is >> x);
        assert(expected_no_err == tested_no_err);
        // TRANSITION, DevCom-10293606
        if (x != test_case.correct_value) {
            char* endptr = nullptr;
            assert(x == strtof(repstr.c_str(), &endptr));
        }
    }
}

template <class Flt>
void test_double_from_char_cases() {
    for (const auto& test_case : double_from_chars_test_cases) {
        auto repstr = test_case.fmt == chars_format::hex ? "0x" + string(test_case.input) : string(test_case.input);
        istringstream is(repstr);
        Flt x = 0.0;

        const bool expected_no_err = test_case.correct_ec == errc{};
        const bool tested_no_err   = static_cast<bool>(is >> x);
        assert(expected_no_err == tested_no_err);
        // TRANSITION, DevCom-10293606
        if (x != test_case.correct_value) {
            char* endptr = nullptr;
            assert(x == strtod(repstr.c_str(), &endptr));
        }
    }
}
#endif // _HAS_CXX17

int main() {
    test<float>();
    test<double>();
    test<long double>();

    test_gh_1582<float>();
    test_gh_1582<double>();
    test_gh_1582<long double>();

    test_gh_3375<float>();
    test_gh_3375<double>();
    test_gh_3375<long double>();

    test_gh_3376<float>();
    test_gh_3376<double>();
    test_gh_3376<long double>();

    test_gh_3378<double>();
    test_gh_3378<long double>();

#if _HAS_CXX17
    test_float_from_char_cases();
    test_double_from_char_cases<double>();
    test_double_from_char_cases<long double>();
#endif // _HAS_CXX17
}
