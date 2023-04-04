// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma warning(disable : 4242) // conversion from 'X' to 'Y', possible loss of data
#pragma warning(disable : 4244) // conversion from 'X' to 'Y', possible loss of data (Yes, duplicated message.)
// Note that this test *should* trigger 4242 and 4244 inside STL headers, because it
// calls std::fill and std::uninitialized_fill with (small type*, small type*, big type)
#pragma warning(disable : 4365) // conversion from 'X' to 'Y', signed/unsigned mismatch
// Note that this test *should* trigger 4365 inside STL headers, because it
// calls std::fill and std::uninitialized_fill with (signed*, signed*, unsigned)
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <type_traits>
#include <vector>

using namespace std;

// This thing is a workaround for C4309 "truncation of constant value"
template <typename T, typename U>
remove_volatile_t<T> cast(U i) {
    return static_cast<remove_volatile_t<T>>(i);
}

// Tests that `fillCall`(buffer, value, startIndex, endIndex) fills [startIndex, endIndex) with `value`
template <typename BuffT, typename CharT, typename Func>
void test_case_fill(CharT value, Func fillCall) {
    const BuffT debugValue = cast<BuffT>(0xCC);
    BuffT buff[1024]       = {};
    for (BuffT& c : buff) {
        c = debugValue;
    }

    const size_t startIndex = 100;
    const size_t endIndex   = 400;
    fillCall(buff, value, startIndex, endIndex);

    for (size_t idx = 0; idx < startIndex; ++idx) {
        assert(buff[idx] == debugValue);
    }

    for (size_t idx = startIndex; idx < endIndex; ++idx) {
        assert(buff[idx] == cast<BuffT>(value));
    }

    for (size_t idx = endIndex; idx < 1024; ++idx) {
        assert(buff[idx] == debugValue);
    }
}

template <typename BuffT, typename CharT>
void test_fill_volatile() {
    const CharT testCases[] = {cast<CharT>(-100), cast<CharT>(-1), cast<CharT>(0), cast<CharT>(1), cast<CharT>(100)};

    for (CharT testCase : testCases) {
        test_case_fill<BuffT>(testCase,
            [](BuffT* buff, CharT value, size_t start, size_t end) { fill(buff + start, buff + end, value); });

        test_case_fill<BuffT>(testCase,
            [](BuffT* buff, CharT value, size_t start, size_t end) { fill_n(buff + start, end - start, value); });
    }
}

template <typename BuffT, typename CharT>
void test_fill() {
    const CharT testCases[] = {cast<CharT>(-100), cast<CharT>(-1), cast<CharT>(0), cast<CharT>(1), cast<CharT>(100)};

    for (CharT testCase : testCases) {
        test_case_fill<BuffT>(testCase,
            [](BuffT* buff, CharT value, size_t start, size_t end) { fill(buff + start, buff + end, value); });

        test_case_fill<BuffT>(testCase,
            [](BuffT* buff, CharT value, size_t start, size_t end) { fill_n(buff + start, end - start, value); });

        test_case_fill<BuffT>(testCase, [](BuffT* buff, CharT value, size_t start, size_t end) {
            uninitialized_fill(buff + start, buff + end, value);
        });

        test_case_fill<BuffT>(testCase, [](BuffT* buff, CharT value, size_t start, size_t end) {
            uninitialized_fill_n(buff + start, end - start, value);
        });
    }
}

class count_copies {
    size_t* number_of_copies;

public:
    explicit count_copies(size_t* c) : number_of_copies(c) {}
    count_copies(const count_copies& other) : number_of_copies(other.number_of_copies) {
        ++*number_of_copies;
    }

    count_copies& operator=(const count_copies&) = delete;
};


// Tests that `fillCall`(backingStorage*, n, /*instance of count_copies*/) is calling constructors
template <typename Func>
void test_uninitialized_fill(Func fillCall) {
    static_assert(sizeof(count_copies) == sizeof(size_t*), "Bad count_copies size");
    size_t actualCopies = 0;
    void* backingStorage[10];
    fillCall(reinterpret_cast<count_copies*>(backingStorage), 10, count_copies(&actualCopies));
    if (actualCopies != 10) {
        printf("Expected 10 copies but got %zu\n", actualCopies);
        abort();
    }

    for (void* storage : backingStorage) {
        static_cast<count_copies*>(storage)->~count_copies();
    }
}

int main() {
    test_fill<char, char>();
    test_fill<char, signed char>();
    test_fill<char, unsigned char>();
    test_fill<signed char, char>();
    test_fill<signed char, signed char>();
    test_fill<signed char, unsigned char>();
    test_fill<unsigned char, char>();
    test_fill<unsigned char, signed char>();
    test_fill<unsigned char, unsigned char>();

#ifdef __cpp_char8_t
    test_fill<char8_t, char8_t>();
    test_fill<char8_t, char>();
    test_fill<char8_t, signed char>();
    test_fill<char8_t, unsigned char>();
    test_fill<char, char8_t>();
    test_fill<signed char, char8_t>();
    test_fill<unsigned char, char8_t>();
#endif // __cpp_char8_t

    test_fill<bool, bool>();

#ifdef __cpp_lib_byte
    test_fill<byte, byte>();
#endif // __cpp_lib_byte

    test_fill<int, int>();
    test_fill<int, char>();
    test_fill<char, int>();

    test_fill_volatile<volatile char, char>(); // Test GH-1183
#ifdef __cpp_lib_byte
    test_fill_volatile<volatile byte, byte>(); // Test GH-1556
#endif // __cpp_lib_byte

    test_uninitialized_fill(
        [](count_copies* buff, size_t n, const count_copies& src) { uninitialized_fill(buff, buff + n, src); });

    test_uninitialized_fill(
        [](count_copies* buff, size_t n, const count_copies& src) { uninitialized_fill_n(buff, n, src); });

    // Validate int is properly converted to bool
    {
        bool output[] = {false, true, false};
        fill(output, output + 3, 5);
        for (const bool& elem : output) {
            assert(elem == true);
        }
    }
    {
        bool output[] = {false, true, false};
        fill_n(output, 3, 5);
        for (const bool& elem : output) {
            assert(elem == true);
        }
    }
    {
        bool output[] = {false, true, false};
        uninitialized_fill(output, output + 3, 5);
        for (const bool& elem : output) {
            assert(elem == true);
        }
    }
    {
        bool output[] = {false, true, false};
        uninitialized_fill_n(output, 3, 5);
        for (const bool& elem : output) {
            assert(elem == true);
        }
    }

    // Test floating-point negative zero
    {
        float output[] = {1.0f, 2.0f, 3.0f};
        fill(output, output + 3, -0.0f);
        for (const float& elem : output) {
            assert(elem == 0.0f); // elem is positive or negative zero
            assert(signbit(elem)); // elem is negative
        }
    }

    // Test (indirectly) _Uninitialized_fill_n with zero
    {
        vector<void*> vec(43, nullptr);
        for (const auto& p : vec) {
            assert(p == nullptr);
        }
    }
}
