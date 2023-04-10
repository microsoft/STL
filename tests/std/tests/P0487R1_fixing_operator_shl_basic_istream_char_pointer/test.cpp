// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <istream>
#include <iterator>
#include <sstream>
#include <string>

using namespace std;

const string testString1          = "TestData1NoSpace";
const string testStringLong       = "TestData1NoSpaceTestData2NoSpaceTestData3NoSpaceTestData4NoSpaceTestData5NoSpace";
const string testStringWhitespace = "Test Data 1 With Space";
const string testStringBeginningWithWhitespace = "     TestData1WithSpace";

const string testStringEmpty;

constexpr unsigned char uchar_max = static_cast<unsigned char>(255);
const auto is_ucharmax            = [](unsigned char elm) { return elm == uchar_max; };

template <typename CharType>
void test_no_whitespace_into_sized_buffer() {
    istringstream stream(testString1);
    CharType buffer[200];
    fill(begin(buffer), end(buffer), static_cast<CharType>(255));
    stream >> buffer;
    assert(equal(begin(testString1), end(testString1), buffer, buffer + size(testString1)));
    assert(all_of(
        buffer + size(testString1) + 1, end(buffer), [](CharType elm) { return elm == static_cast<CharType>(255); }));
    assert(stream.eof());
    assert(!stream.fail());
}

// >> is specified to write the null terminator to the buffer no matter what, even
// if the stream immediately returns EOF; N4810 29.7.4.2.3 [istream.extractors]/9
void test_empty_string_into_sized_buffer() {
    istringstream stream(testStringEmpty);
    unsigned char buffer[2];
    fill(begin(buffer), end(buffer), uchar_max);
    stream >> buffer;
    assert(buffer[0] == '\0');
    assert(buffer[1] == 0xff);
    assert(stream.eof());
    assert(stream.fail());
}

void test_width_exactly_same_as_buffer_size() {
    istringstream stream(testString1);
    unsigned char buffer[2];
    fill(begin(buffer), end(buffer), uchar_max);
    stream.width(2);
    stream >> buffer;
    assert(buffer[0] == 'T');
    assert(buffer[1] == '\0');
    assert(!stream.eof());
    assert(!stream.fail());
}

void test_reset_width_at_end_of_output() {
    istringstream stream(testString1);
    unsigned char buffer[20];
    fill(begin(buffer), end(buffer), uchar_max);
    stream.width(20);
    stream >> buffer;
    assert(reinterpret_cast<char*>(buffer) == testString1);
    assert(all_of(buffer + size(testString1) + 1, end(buffer), is_ucharmax));
    assert(stream.width() == 0);
    assert(stream.eof());
    assert(!stream.fail());
}

void test_only_extract_up_to_width() {
    istringstream stream(testString1);
    unsigned char buffer[20];
    fill(begin(buffer), end(buffer), uchar_max);
    stream.width(1);
    stream >> buffer;
    assert(buffer[0] == '\0');
    assert(all_of(buffer + 1, end(buffer), is_ucharmax));
    assert(!stream.eof());
    assert(stream.fail());
}

void test_only_extract_up_to_width_2() {
    istringstream stream(testString1);
    unsigned char buffer[20];
    fill(begin(buffer), end(buffer), uchar_max);
    stream.width(2);
    stream >> buffer;
    assert(buffer[0] == 'T');
    assert(buffer[1] == '\0');
    assert(all_of(buffer + 2, end(buffer), is_ucharmax));
    unsigned char c = '\0';
    stream >> c;
    assert(c == 'e');
    assert(!stream.eof());
    assert(!stream.fail());
}

// We are using this structure to test that the implementation won't write past the end of a
// sized buffer when getting the size from a deduced template argument. Unfortunately
// this means that in the case where the test fails it will be triggering undefined behavior.
// We could detect failure without doing this by mapping two new pages and then protecting the second
// and aligning the first buffer here right at the page boundary (much like what app verifier can do).
// Considering that this would be highly platform specific and somewhat arcane we've opted for the
// simple solution involving undefined behavior.
//
// In any case if this test comes up in dynamic or static analysis that's why.
struct test_buffers {
    unsigned char buffer0[100];
    unsigned char buffer1[10];
    unsigned char buffer2[100];
};

void test_does_not_extract_past_length() {
    istringstream stream(testStringLong);
    test_buffers buffers = {};
    fill(begin(buffers.buffer0), end(buffers.buffer0), uchar_max);
    fill(begin(buffers.buffer2), end(buffers.buffer2), uchar_max);
    stream >> buffers.buffer1;
    assert(equal(begin(testStringLong), begin(testStringLong) + 9, buffers.buffer1, buffers.buffer1 + 9));
    assert(buffers.buffer1[9] == '\0');
    assert(all_of(begin(buffers.buffer0), end(buffers.buffer0), is_ucharmax));
    assert(all_of(begin(buffers.buffer2), end(buffers.buffer2), is_ucharmax));
    assert(!stream.eof());
    assert(!stream.fail());
}


// In this case both width > 0 and the buffer is too small
// for the data.
void test_does_not_extract_past_length_with_width() {
    istringstream stream(testStringLong);
    test_buffers buffers = {};
    fill(begin(buffers.buffer0), end(buffers.buffer0), uchar_max);
    fill(begin(buffers.buffer2), end(buffers.buffer2), uchar_max);
    stream.width(12);
    stream >> buffers.buffer1;
    assert(equal(begin(testStringLong), begin(testStringLong) + 9, buffers.buffer1, buffers.buffer1 + 9));
    assert(buffers.buffer1[9] == '\0');
    assert(all_of(begin(buffers.buffer2), end(buffers.buffer2), is_ucharmax));
    assert(stream.width() == 0);
    assert(!stream.eof());
    assert(!stream.fail());
}

void test_extract_beginning_with_whitespace() {
    istringstream stream(testStringBeginningWithWhitespace);
    signed char buffer[40];
    fill(begin(buffer), end(buffer), '\xFF');
    stream >> buffer;
    assert(string(reinterpret_cast<char*>(buffer)) == "TestData1WithSpace");
    assert(stream.eof());
    assert(!stream.fail());
}


int main() {
    test_no_whitespace_into_sized_buffer<char>();
    test_no_whitespace_into_sized_buffer<unsigned char>();
    test_no_whitespace_into_sized_buffer<signed char>();
    test_empty_string_into_sized_buffer();
    test_width_exactly_same_as_buffer_size();
    test_reset_width_at_end_of_output();

    test_only_extract_up_to_width();
    test_only_extract_up_to_width_2();
#if _HAS_CXX20 // these are the tests for the new, c++20 signatures
    test_does_not_extract_past_length();
    test_does_not_extract_past_length_with_width();
#endif

    test_extract_beginning_with_whitespace();
}
