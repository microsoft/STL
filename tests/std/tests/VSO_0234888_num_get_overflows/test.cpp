// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <locale>
#include <sstream>
#include <string>
#include <type_traits>

#pragma warning(disable : 4984) // if constexpr is a C++17 language extension
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++17-extensions"
#endif // __clang__

using namespace std;

template <class UInt>
void test_case_unsigned(const string& maxValue, const string& maxValuePlusOne) {
    // See N4727 [facet.num.get.virtuals]/3.6
    // * "zero, if the conversion function does not convert the entire field"
    {
        istringstream src("-"s);
        UInt result = 42;
        src >> result;
        assert(result == 0);
        assert(src.rdstate() == (ios_base::failbit | ios_base::eofbit));
    }

    // * "if the field to be converted to a signed integer type" doesn't apply here
    // * "the most positive representable value, if the field to be converted to an
    //   unsigned integer type represents a value that cannot be represented in val".
    {
        istringstream src(maxValuePlusOne);
        UInt result = 42;
        src >> result;
        assert(result == numeric_limits<UInt>::max());
        // If the conversion function does not convert the entire field, or if
        // the field represents a value outside the range of representable values, ios_base::failbit
        // is assigned to err.
        assert(src.rdstate() == (ios_base::failbit | ios_base::eofbit));
    }

    // Check that it's actually assigning, not bitwise-OR-ing in failbit:
    {
        UInt result = 42;
        istringstream src(maxValuePlusOne + " ");
        ios_base::iostate state = ios_base::badbit;
        use_facet<num_get<char>>(src.getloc()).get(src, nullptr, src, state, result);
        assert(state == ios_base::failbit); // asserts that badbit was cleared
    }

    // * "the converted value, otherwise."
    {
        istringstream src("0"s);
        UInt result = 42;
        src >> result;
        assert(result == 0);
        assert(src.rdstate() == ios_base::eofbit);
    }

    {
        istringstream src("-0"s);
        UInt result = 42;
        src >> result;
        assert(result == 0);
        assert(src.rdstate() == ios_base::eofbit);
    }

    {
        istringstream src(maxValue);
        UInt result = 42;
        src >> result;
        assert(result == numeric_limits<UInt>::max());
        assert(src.rdstate() == ios_base::eofbit);
    }

    {
        istringstream src("-" + maxValue);
        UInt result = 42;
        src >> result;
        assert(result == 1);
        assert(src.rdstate() == ios_base::eofbit);
    }
}

template <class Int>
void test_case_signed(
    const string& minValueMinusOne, const string& minValue, const string& maxValue, const string& maxValuePlusOne) {
    // See N4727 [facet.num.get.virtuals]/3.6
    // * "zero, if the conversion function does not convert the entire field"
    {
        istringstream src("-"s);
        Int result = 42;
        src >> result;
        assert(result == 0);
        assert(src.rdstate() == (ios_base::failbit | ios_base::eofbit));
    }

    // * "the most positive (or negative) representable value, if the field to be
    //   converted to a signed integer type represents a value too large positive
    //   (or negative) to be represented in val."
    {
        istringstream src(minValueMinusOne);
        Int result = 42;
        src >> result;
        assert(result == numeric_limits<Int>::min());
        // If the conversion function does not convert the entire field, or if
        // the field represents a value outside the range of representable values, ios_base::failbit
        // is assigned to err.
        assert(src.rdstate() == (ios_base::failbit | ios_base::eofbit));
    }

    {
        istringstream src(maxValuePlusOne);
        Int result = 42;
        src >> result;
        assert(result == numeric_limits<Int>::max());
        assert(src.rdstate() == (ios_base::failbit | ios_base::eofbit));
    }

    // Check that it's actually assigning, not bitwise-OR-ing in failbit:
    if constexpr (!is_same_v<Int, short> && !is_same_v<Int, int>) {
        Int result = 42;
        istringstream src(maxValuePlusOne + " ");
        ios_base::iostate state = ios_base::badbit;
        use_facet<num_get<char>>(src.getloc()).get(src, nullptr, src, state, result);
        assert(state == ios_base::failbit); // asserts that badbit was cleared
    }

    // * "if the field to be converted to an unsigned integer type" doesn't apply here

    // * "the converted value, otherwise."
    {
        istringstream src("0"s);
        Int result = 42;
        src >> result;
        assert(result == 0);
        assert(src.rdstate() == ios_base::eofbit);
    }

    {
        istringstream src("-0"s);
        Int result = 42;
        src >> result;
        assert(result == 0);
        assert(src.rdstate() == ios_base::eofbit);
    }

    {
        istringstream src(minValue);
        Int result = 42;
        src >> result;
        assert(result == numeric_limits<Int>::min());
        assert(src.rdstate() == ios_base::eofbit);
    }

    {
        istringstream src(maxValue);
        Int result = 42;
        src >> result;
        assert(result == numeric_limits<Int>::max());
        assert(src.rdstate() == ios_base::eofbit);
    }

    {
        istringstream src("-" + maxValue);
        Int result = 42;
        src >> result;
        assert(result == -numeric_limits<Int>::max());
        assert(src.rdstate() == ios_base::eofbit);
    }
}

template <class Float>
void test_case_float(const string& outOfRangeValue) {
    // reals don't get any of the int special cases and always set 0 on failure
    {
        istringstream src(outOfRangeValue);
        Float result = 17.49f;
        src >> result;
        assert(result == static_cast<Float>(0));
        assert(src.rdstate() == (ios_base::failbit | ios_base::eofbit));
    }

    {
        istringstream src("-" + outOfRangeValue);
        Float result = 17.49f;
        src >> result;
        assert(result == static_cast<Float>(0));
        assert(src.rdstate() == (ios_base::failbit | ios_base::eofbit));
    }

    // Check that it's actually assigning, not bitwise-OR-ing in failbit:
    {
        istringstream src(outOfRangeValue + " ");
        Float result            = 17.49f;
        ios_base::iostate state = ios_base::badbit;
        use_facet<num_get<char>>(src.getloc()).get(src, nullptr, src, state, result);
        assert(state == ios_base::failbit); // asserts that badbit was cleared
    }
}

void test_case_direct_float_without_intermediate_double() {
    // Magic number from LWG-2403 which produces different results when converted to double
    // and then rounded to float vs. being converted to float directly.
    // Extra space at the end to avoid getting eofbit set.
    const auto boundary       = "1.999999821186065729339276231257827021181583404541015625 "s;
    const auto direct         = stof(boundary);
    const auto indirectDouble = stod(boundary);
    const auto indirect       = static_cast<float>(indirectDouble);
    assert(direct != indirect);

    {
        istringstream src(boundary);
        float actual;
        src >> actual;
        assert(src.good());
        assert(actual == direct);
    }

    {
        istringstream src(boundary);
        double actual;
        src >> actual;
        assert(src.good());
        assert(actual == indirectDouble);
        assert(static_cast<float>(actual) == indirect);
    }
}

void test_case_bool() {
    const char* const badCases[] = {"-1", "2", "2147483647", "2147483648", "-2147483648"};
    for (const auto& badCase : badCases) {
        istringstream src(badCase);
        bool result = false;
        src >> result;
        assert(result);
        assert(src.rdstate() == (ios_base::failbit | ios_base::eofbit));
    }

    {
        istringstream src("0"s);
        bool result = true;
        src >> result;
        assert(!result);
        assert(src.rdstate() == ios_base::eofbit);
    }

    {
        istringstream src("1"s);
        bool result = false;
        src >> result;
        assert(result);
        assert(src.rdstate() == ios_base::eofbit);
    }
}

void test_case_void_ptr() {
    {
        istringstream src("0xFFFFFFFF"s);
        void* result;
        src >> result;
        assert(reinterpret_cast<uintptr_t>(result) == 0xFFFFFFFF);
        assert(src.rdstate() == ios_base::eofbit);
    }

#ifdef _WIN64
    {
        istringstream src("0xFFFFFFFFFFFFFFFF"s);
        void* result;
        src >> result;
        assert(reinterpret_cast<uintptr_t>(result) == 0xFFFFFFFFFFFFFFFF);
        assert(src.rdstate() == ios_base::eofbit);
    }
#endif // _WIN64

    {
        istringstream src("0x10000000000000000"s);
        void* result;
        src >> result;
        assert(result == nullptr);
        assert(src.rdstate() == (ios_base::failbit | ios_base::eofbit));
    }

    // Check that it's actually assigning, not bitwise-OR-ing in failbit:
    {
        istringstream src("0x10000000000000000 "s);
        void* result;
        ios_base::iostate state = ios_base::badbit;
        use_facet<num_get<char>>(src.getloc()).get(src, nullptr, src, state, result);
        assert(result == nullptr);
        assert(state == ios_base::failbit); // asserts that badbit was cleared
    }
}

int main() {
    test_case_unsigned<unsigned short>("65535"s, "65536"s);
    // also stress overflow in intermediate conversion to unsigned long:
    test_case_unsigned<unsigned short>("65535"s, "4294967296"s);
    test_case_unsigned<unsigned int>("4294967295"s, "4294967296"s);
    test_case_unsigned<unsigned long>("4294967295"s, "4294967296"s);
    test_case_unsigned<unsigned long long>("18446744073709551615"s, "18446744073709551616"s);

    test_case_signed<short>("-32769"s, "-32768"s, "32767"s, "32768"s);
    // also stress overflow in intermediate conversion to long:
    test_case_signed<short>("-2147483649"s, "-32768"s, "32767"s, "2147483648"s);
    test_case_signed<int>("-2147483649"s, "-2147483648"s, "2147483647"s, "2147483648"s);
    test_case_signed<long>("-2147483649"s, "-2147483648"s, "2147483647"s, "2147483648"s);
    test_case_signed<long long>(
        "-9223372036854775809"s, "-9223372036854775808"s, "9223372036854775807"s, "9223372036854775808"s);

    test_case_float<float>("1e39"s);
    test_case_float<float>("1e399"s);
    test_case_float<double>("1e399"s);
    test_case_float<long double>("1e399"s);

    test_case_direct_float_without_intermediate_double();
    test_case_bool();
    test_case_void_ptr();
}
