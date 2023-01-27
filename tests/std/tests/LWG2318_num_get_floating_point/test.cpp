// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <cstddef>
#include <ios>
#include <locale>
#include <sstream>
#include <string>

using namespace std;

class my_facet : public num_get<char, const char*> {
public:
    explicit my_facet(size_t refs = 0) : num_get<char, const char*>(refs) {}
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
        // See PR11871
        v                     = -1;
        const char str[]      = "2-";
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(str, str + sizeof(str), instr, err, v);
        assert(iter == str + 1);
        assert(err == instr.goodbit);
        assert(v == 2);
    }
    if constexpr (is_same_v<Flt, float>) {
        {
            v                     = -1;
            const char str[]      = "3.40283e+39"; // unrepresentable
            ios_base::iostate err = instr.goodbit;
            const char* iter      = f.get(str, str + sizeof(str), instr, err, v);
            assert(iter == str + sizeof(str) - 1);
            assert(err == instr.failbit);
            assert(v == HUGE_VALF);
        }
        {
            v                     = -1;
            const char str[]      = "-3.40283e+38"; // unrepresentable
            ios_base::iostate err = instr.goodbit;
            const char* iter      = f.get(str, str + sizeof(str), instr, err, v);
            assert(iter == str + sizeof(str) - 1);
            assert(err == instr.failbit);
            assert(v == -HUGE_VALF);
        }
    } else {
        {
            v                     = -1;
            const char str[]      = "1.79779e+309"; // unrepresentable
            ios_base::iostate err = instr.goodbit;
            const char* iter      = f.get(str, str + sizeof(str), instr, err, v);
            assert(iter == str + sizeof(str) - 1);
            assert(err == instr.failbit);
            assert(v == HUGE_VAL);
        }
        {
            v                     = -1;
            const char str[]      = "-1.79779e+308"; // unrepresentable
            ios_base::iostate err = instr.goodbit;
            const char* iter      = f.get(str, str + sizeof(str), instr, err, v);
            assert(iter == str + sizeof(str) - 1);
            assert(err == instr.failbit);
            assert(v == -HUGE_VAL);
        }
    }

    instr.imbue(locale(locale(), new my_numpunct));
    if constexpr (is_same_v<Flt, float>) {
        v                     = -1;
        const char str[]      = "456_78_9;5";
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(str, str + sizeof(str), instr, err, v);
        assert(iter == str + sizeof(str) - 1);
        assert(err == instr.goodbit);
        assert(v == 456789.5);
    } else {
        v                     = -1;
        const char str[]      = "123_456_78_9;125";
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(str, str + sizeof(str), instr, err, v);
        assert(iter == str + sizeof(str) - 1);
        assert(err == instr.goodbit);
        assert(v == 123456789.125);
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
        std::istringstream f(str);
        f >> v;
        assert(v == 0x125p-1);
        assert(f.good());
    }
}

int main() {
    test<float>();
    test<double>();
    test<long double>();
}
