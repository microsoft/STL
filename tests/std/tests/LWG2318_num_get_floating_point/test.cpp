// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

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

using namespace std;

class my_facet : public num_get<char, const char*> {
public:
    explicit my_facet(size_t refs = 0) : num_get<char, const char*>(refs) {}
};

class mid_zero_numpunct : public numpunct<char> {
public:
    mid_zero_numpunct() : numpunct<char>() {}

protected:
    virtual string do_grouping() const {
        return "\1\0\2"s;
    }
};

class my_numpunct : public numpunct<char> {
public:
    my_numpunct() : numpunct<char>() {}

protected:
    virtual char_type do_decimal_point() const {
        return ';';
    }
    virtual char_type do_thousands_sep() const {
        return '_';
    }
    virtual string do_grouping() const {
        return string("\1\2\3");
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

    // Also test non-ending unlimited grouping
    instr.imbue(locale(locale(), new mid_zero_numpunct));
    {
        v                     = -1;
        const char sep_str[]  = "17,2,9.0";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == 1729.0);
    }
    {
        v                     = -1;
        const char sep_str[]  = "17,222,9.0";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == 172229.0);
    }
    {
        v                     = -1;
        const char sep_str[]  = "0x17,2,9.0";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == 0x1729.0p0);
    }
    {
        v                     = -1;
        const char sep_str[]  = "0x17,222,9.0";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == 0x172229.0p0);
    }
}

// Also test non-ending unlimited grouping for integers
template <class Integer>
void test_int_grouping() {
    const my_facet f(1);
    ios instr(nullptr);
    instr.imbue(locale(locale(), new mid_zero_numpunct));

    Integer v = 0;
    {
        v                     = static_cast<Integer>(-1);
        const char sep_str[]  = "17,2,9";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == static_cast<Integer>(1729));
    }
    {
        v                     = static_cast<Integer>(-1);
        const char sep_str[]  = "17,222,9";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == static_cast<Integer>(172229));
    }
    {
        v                     = static_cast<Integer>(-1);
        const char sep_str[]  = "0x17,2,9";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;

        instr.flags(std::ios_base::fmtflags{});
        const char* iter = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == static_cast<Integer>(0x1729));
    }
    {
        v                     = static_cast<Integer>(-1);
        const char sep_str[]  = "0x17,222,9";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;

        instr.flags(std::ios_base::fmtflags{});
        const char* iter = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == static_cast<Integer>(0x172229));
    }
}

int main() {
    test<float>();
    test<double>();
    test<long double>();

    test_int_grouping<unsigned int>();
    test_int_grouping<long>();
    test_int_grouping<unsigned long>();
    test_int_grouping<long long>();
    test_int_grouping<unsigned long long>();
}
