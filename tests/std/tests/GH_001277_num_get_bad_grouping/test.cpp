// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <ios>
#include <locale>
#include <string>
#include <type_traits>

using namespace std;

class my_facet : public num_get<char, const char*> {
public:
    explicit my_facet(size_t refs = 0) : num_get<char, const char*>(refs) {}
};

class special_numpunct : public numpunct<char> {
public:
    special_numpunct() : numpunct<char>() {}

protected:
    string do_grouping() const override {
        return "\1\2\1"s;
    }
};

// Test good and bad grouping for integers
template <class Integer, enable_if_t<is_integral_v<Integer>, int> = 0>
void test_good_and_bad_grouping() {
    const my_facet f(1);
    ios instr(nullptr);
    instr.imbue(locale(locale(), new special_numpunct));

    instr.setf(ios_base::fmtflags{}, ios_base::basefield);

    Integer v = 0;
    {
        v                     = static_cast<Integer>(-1);
        const char sep_str[]  = "0,17,7";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == static_cast<Integer>(0177));
    }
    {
        v                     = static_cast<Integer>(-1);
        const char sep_str[]  = "01,77";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.failbit);
        assert(v == static_cast<Integer>(0177));
    }
    {
        v                     = static_cast<Integer>(-1);
        const char sep_str[]  = "1,72,9";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == static_cast<Integer>(1729));
    }
    {
        v                     = static_cast<Integer>(-1);
        const char sep_str[]  = "172,9";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.failbit);
        assert(v == static_cast<Integer>(1729));
    }
    {
        v                     = static_cast<Integer>(-1);
        const char sep_str[]  = "0x1,72,9";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == static_cast<Integer>(0x1729));
    }
    {
        v                     = static_cast<Integer>(-1);
        const char sep_str[]  = "0x1,729";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.failbit);
        assert(v == static_cast<Integer>(0x1729));
    }
}

// Test good and bad grouping for floating-point numbers
template <class Flt, enable_if_t<is_floating_point_v<Flt>, int> = 0>
void test_good_and_bad_grouping() {
    const my_facet f(1);
    ios instr(nullptr);
    instr.imbue(locale(locale(), new special_numpunct));

    Flt v = 0;
    {
        v                     = static_cast<Flt>(-1);
        const char sep_str[]  = "1,72,9";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == static_cast<Flt>(1729.0));
    }
    {
        v                     = static_cast<Flt>(-1);
        const char sep_str[]  = "1,72,9.0";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == static_cast<Flt>(1729.0));
    }
    {
        v                     = static_cast<Flt>(-1);
        const char sep_str[]  = "1,72,9.125";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == static_cast<Flt>(1729.125));
    }
    {
        v                     = static_cast<Flt>(-1);
        const char sep_str[]  = "0x1,72,9";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == static_cast<Flt>(0x1729.0p0));
    }
    {
        v                     = static_cast<Flt>(-1);
        const char sep_str[]  = "0x1,72,9.0";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == static_cast<Flt>(0x1729.0p0));
    }
    {
        v                     = static_cast<Flt>(-1);
        const char sep_str[]  = "0x1,72,9.3";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == static_cast<Flt>(0x1729.3p0));
    }
    {
        v                     = static_cast<Flt>(-1);
        const char sep_str[]  = "1,729";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.failbit);
        assert(v == static_cast<Flt>(1729.0));
    }
    {
        v                     = static_cast<Flt>(-1);
        const char sep_str[]  = "1,729.0";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.failbit);
        assert(v == static_cast<Flt>(1729.0));
    }
    {
        v                     = static_cast<Flt>(-1);
        const char sep_str[]  = "1,729.125";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.failbit);
        assert(v == static_cast<Flt>(1729.125));
    }
    {
        v                     = static_cast<Flt>(-1);
        const char sep_str[]  = "0x172,9";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.failbit);
        assert(v == static_cast<Flt>(0x1729.0p0));
    }
    {
        v                     = static_cast<Flt>(-1);
        const char sep_str[]  = "0x172,9.0";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.failbit);
        assert(v == static_cast<Flt>(0x1729.0p0));
    }
    {
        v                     = static_cast<Flt>(-1);
        const char sep_str[]  = "0x172,9.3";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.failbit);
        assert(v == static_cast<Flt>(0x1729.3p0));
    }

    // separators at unexpected places
    {
        v                     = static_cast<Flt>(-1);
        const char sep_str[]  = ",1,72,9";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.failbit);
        assert(v == static_cast<Flt>(1729.0));
    }
    {
        v                     = static_cast<Flt>(-1);
        const char sep_str[]  = "1,72,9,,";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.failbit);
        assert(v == static_cast<Flt>(1729.0));
    }
    {
        v                     = static_cast<Flt>(-1);
        const char sep_str[]  = ",+1,72,9";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.failbit);
        assert(v == static_cast<Flt>(1729.0));
    }
    {
        v                     = static_cast<Flt>(-1);
        const char sep_str[]  = "-,172,9,,.0e0";
        const size_t len      = sizeof(sep_str) - 1;
        ios_base::iostate err = instr.goodbit;
        const char* iter      = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.failbit);
        assert(v == static_cast<Flt>(-1729.0));
    }
}

class my_numput : public num_put<char, char*> {
public:
    explicit my_numput(size_t refs = 0) : num_put<char, char*>(refs) {}
};

class another_numpunct : public numpunct<char> {
public:
    another_numpunct() : numpunct<char>() {}

protected:
    string do_grouping() const override {
        return "\2\3\4"s;
    }
};

// Test good and bad grouping for void*
void test_good_and_bad_grouping_pointer() {
    const void* ptr = reinterpret_cast<const void*>(static_cast<uintptr_t>(0xdeadbeef));
    const my_facet f(1);
    const my_numput of(1);
    ios instr(nullptr);
    instr.imbue(locale(locale(), new special_numpunct));

    {
        ios ostr(nullptr);
        ostr.imbue(locale(locale(), new special_numpunct));

        char buf[32];
        const auto written_end = of.put(buf, ostr, '\0', ptr);
        *written_end           = '\0';

        void* p               = nullptr;
        ios_base::iostate err = instr.goodbit;
        const auto read_end   = f.get(buf, buf + sizeof(buf), instr, err, p);
        assert(read_end == written_end);
        assert(err == instr.goodbit);
        assert(p == ptr);
    }
    {
        ios ostr(nullptr);
        ostr.imbue(locale(locale(), new another_numpunct));

        char buf[32];
        const auto written_end = of.put(buf, ostr, '\0', ptr);
        *written_end           = '\0';

        void* p               = nullptr;
        ios_base::iostate err = instr.goodbit;
        const auto read_end   = f.get(buf, buf + sizeof(buf), instr, err, p);
        assert(read_end == written_end);
        assert(err == instr.failbit);
        assert(p == ptr);
    }
}

class mid_zero_numpunct : public numpunct<char> {
public:
    mid_zero_numpunct() : numpunct<char>() {}

protected:
    string do_grouping() const override {
        return "\1\0\2"s;
    }
};

// Also test non-ending unlimited grouping for integers
template <class Integer, enable_if_t<is_integral_v<Integer>, int> = 0>
void test_nonending_unlimited_grouping() {
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

        instr.flags(ios_base::fmtflags{});
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

        instr.flags(ios_base::fmtflags{});
        const char* iter = f.get(sep_str, sep_str + len + 1, instr, err, v);
        assert(iter == sep_str + len);
        assert(err == instr.goodbit);
        assert(v == static_cast<Integer>(0x172229));
    }
}

// Also test non-ending unlimited grouping for FP numbers
template <class Flt, enable_if_t<is_floating_point_v<Flt>, int> = 0>
void test_nonending_unlimited_grouping() {
    const my_facet f(1);
    ios instr(nullptr);
    instr.imbue(locale(locale(), new mid_zero_numpunct));
    Flt v = 0;
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

int main() {
    test_good_and_bad_grouping<unsigned short>();
    test_good_and_bad_grouping<unsigned int>();
    test_good_and_bad_grouping<long>();
    test_good_and_bad_grouping<unsigned long>();
    test_good_and_bad_grouping<long long>();
    test_good_and_bad_grouping<unsigned long long>();
    test_good_and_bad_grouping<float>();
    test_good_and_bad_grouping<double>();
    test_good_and_bad_grouping<long double>();
    test_good_and_bad_grouping_pointer();

    test_nonending_unlimited_grouping<unsigned int>();
    test_nonending_unlimited_grouping<long>();
    test_nonending_unlimited_grouping<unsigned long>();
    test_nonending_unlimited_grouping<long long>();
    test_nonending_unlimited_grouping<unsigned long long>();
    test_nonending_unlimited_grouping<float>();
    test_nonending_unlimited_grouping<double>();
    test_nonending_unlimited_grouping<long double>();
}
