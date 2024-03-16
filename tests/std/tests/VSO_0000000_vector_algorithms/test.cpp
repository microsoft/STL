// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <functional>
#include <isa_availability.h>
#include <limits>
#include <list>
#include <random>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#if _HAS_CXX20
#include <ranges>
#endif

#include "test_min_max_element_support.hpp"

using namespace std;

#pragma warning(disable : 4984) // 'if constexpr' is a C++17 language extension
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++17-extensions" // constexpr if is a C++17 extension
#endif // __clang__

void initialize_randomness(mt19937_64& gen) {
    constexpr size_t n = mt19937_64::state_size;
    constexpr size_t w = mt19937_64::word_size;
    static_assert(w % 32 == 0, "w should be evenly divisible by 32");
    constexpr size_t k = w / 32;

    vector<uint32_t> vec(n * k);

    random_device rd;
    generate(vec.begin(), vec.end(), ref(rd));

    printf("This is a randomized test.\n");
    printf("DO NOT IGNORE/RERUN ANY FAILURES.\n");
    printf("You must report them to the STL maintainers.\n\n");

    printf("Seed vector: ");
    for (const auto& e : vec) {
        printf("%u,", e);
    }
    printf("\n");

    seed_seq seq(vec.cbegin(), vec.cend());
    gen.seed(seq);
}

#if (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_CEE_PURE)
extern "C" long __isa_enabled;

void disable_instructions(ISA_AVAILABILITY isa) {
    __isa_enabled &= ~(1UL << static_cast<unsigned long>(isa));
}
#endif // (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_CEE_PURE)

constexpr size_t dataCount = 1024;

template <class FwdIt, class T>
ptrdiff_t last_known_good_count(FwdIt first, FwdIt last, T v) {
    ptrdiff_t result = 0;
    for (; first != last; ++first) {
        result += (*first == v);
    }
    return result;
}


template <class T>
void test_case_count(const vector<T>& input, T v) {
    auto expected = last_known_good_count(input.begin(), input.end(), v);
    auto actual   = count(input.begin(), input.end(), v);
    assert(expected == actual);
}

template <class T>
void test_count(mt19937_64& gen) {
    using TD = conditional_t<sizeof(T) == 1, int, T>;
    binomial_distribution<TD> dis(10);
    vector<T> input;
    input.reserve(dataCount);
    test_case_count(input, static_cast<T>(dis(gen)));
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        input.push_back(static_cast<T>(dis(gen)));
        test_case_count(input, static_cast<T>(dis(gen)));
    }
}

template <class FwdIt, class T>
auto last_known_good_find(FwdIt first, FwdIt last, T v) {
    for (; first != last; ++first) {
        if (*first == v) {
            break;
        }
    }
    return first;
}

template <class FwdIt, class T>
auto last_known_good_find_last(FwdIt first, FwdIt last, T v) {
    FwdIt last_save = last;
    for (;;) {
        if (last == first) {
            return last_save;
        }
        --last;
        if (*last == v) {
            return last;
        }
    }
}

template <class T>
void test_case_find(const vector<T>& input, T v) {
    auto expected = last_known_good_find(input.begin(), input.end(), v);
    auto actual   = find(input.begin(), input.end(), v);
    assert(expected == actual);
}

template <class T>
void test_find(mt19937_64& gen) {
    using TD = conditional_t<sizeof(T) == 1, int, T>;
    binomial_distribution<TD> dis(10);
    vector<T> input;
    input.reserve(dataCount);
    test_case_find(input, static_cast<T>(dis(gen)));
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        input.push_back(static_cast<T>(dis(gen)));
        test_case_find(input, static_cast<T>(dis(gen)));
    }
}

#if _HAS_CXX20
template <class T, size_t N>
struct NormalArrayWrapper {
    T m_arr[N];
};

// Also test GH-4454 "vector_algorithms.cpp: __std_find_trivial_unsized_impl assumes N-byte elements are N-aligned"
#pragma pack(push, 1)
template <class T, size_t N>
struct PackedArrayWrapper {
    uint8_t m_ignored; // to misalign the following array
    T m_arr[N];
};
#pragma pack(pop)

// GH-4449 <xutility>: ranges::find with unreachable_sentinel / __std_find_trivial_unsized_1 gives wrong result
template <class T, template <class, size_t> class ArrayWrapper>
void test_gh_4449_impl() {
    constexpr T desired_val{11};
    constexpr T unwanted_val{22};

    ArrayWrapper<T, 256> wrapper;
    auto& arr = wrapper.m_arr;

    constexpr int mid1 = 64;
    constexpr int mid2 = 192;

    ranges::fill(arr, arr + mid1, desired_val);
    ranges::fill(arr + mid1, arr + mid2, unwanted_val);
    ranges::fill(arr + mid2, end(arr), desired_val);

    for (int idx = mid1; idx <= mid2; ++idx) { // when idx == mid2, the value is immediately found
        const auto where = ranges::find(arr + idx, unreachable_sentinel, desired_val);

        assert(where == arr + mid2);

        arr[idx] = desired_val; // get ready for the next iteration
    }
}

template <class T>
void test_gh_4449() {
    test_gh_4449_impl<T, NormalArrayWrapper>();
    test_gh_4449_impl<T, PackedArrayWrapper>();
}
#endif // _HAS_CXX20

#if _HAS_CXX23
template <class T>
void test_case_find_last(const vector<T>& input, T v) {
    auto expected = last_known_good_find_last(input.begin(), input.end(), v);
    auto range    = ranges::find_last(input.begin(), input.end(), v);
    auto actual   = range.begin();
    assert(expected == actual);
    assert(range.end() == input.end());
}

template <class T>
void test_find_last(mt19937_64& gen) {
    using TD = conditional_t<sizeof(T) == 1, int, T>;
    binomial_distribution<TD> dis(10);
    vector<T> input;
    input.reserve(dataCount);
    test_case_find_last(input, static_cast<T>(dis(gen)));
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        input.push_back(static_cast<T>(dis(gen)));
        test_case_find_last(input, static_cast<T>(dis(gen)));
    }
}
#endif // _HAS_CXX23

template <class T>
void test_min_max_element(mt19937_64& gen) {
    using Limits = numeric_limits<T>;

    uniform_int_distribution<conditional_t<sizeof(T) == 1, int, T>> dis(Limits::min(), Limits::max());

    vector<T> input;
    input.reserve(dataCount);
    test_case_min_max_element(input);
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        input.push_back(static_cast<T>(dis(gen)));
        test_case_min_max_element(input);
    }
}

template <class T>
void test_min_max_element_floating(mt19937_64& gen) {
    normal_distribution<T> dis(-100000.0, 100000.0);

    constexpr auto input_of_input_size = dataCount / 2;
    vector<T> input_of_input(input_of_input_size);
    input_of_input[0] = -numeric_limits<T>::infinity();
    input_of_input[1] = +numeric_limits<T>::infinity();
    input_of_input[2] = -0.0;
    input_of_input[3] = +0.0;
    for (size_t i = 4; i < input_of_input_size; ++i) {
        input_of_input[i] = dis(gen);
    }

    uniform_int_distribution<size_t> idx_dis(0, input_of_input_size - 1);

    vector<T> input;
    input.reserve(dataCount);
    test_case_min_max_element(input);
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        input.push_back(input_of_input[idx_dis(gen)]);
        test_case_min_max_element(input);
    }
}

void test_min_max_element_pointers(mt19937_64& gen) {
    const short arr[20]{};

    uniform_int_distribution<size_t> dis(0, size(arr) - 1);

    vector<const short*> input;
    input.reserve(dataCount);
    test_case_min_max_element(input);
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        input.push_back(arr + dis(gen));
        test_case_min_max_element(input);
    }
}

template <class ElementType, size_t VectorSize>
void test_min_max_element_special_cases() {
    constexpr size_t block_size_in_vectors  = 1 << (sizeof(ElementType) * CHAR_BIT);
    constexpr size_t block_size_in_elements = block_size_in_vectors * VectorSize;
    constexpr size_t num_blocks             = 4;
    constexpr size_t tail_size              = 13;
    constexpr size_t array_size             = num_blocks * block_size_in_elements + tail_size;
    constexpr size_t last_block_first_elem  = (num_blocks - 1) * block_size_in_elements;
    constexpr size_t last_vector_first_elem = (block_size_in_vectors - 1) * VectorSize;

    vector<ElementType> v(array_size); // not array to avoid large data on stack

    // all equal
    fill(v.begin(), v.end(), ElementType{1});
    assert(min_element(v.begin(), v.end()) == v.begin());
    assert(max_element(v.begin(), v.end()) == v.begin());
    assert(minmax_element(v.begin(), v.end()).first == v.begin());
    assert(minmax_element(v.begin(), v.end()).second == v.end() - 1);

    // same position in different blocks
    fill(v.begin(), v.end(), ElementType{1});
    for (size_t block_pos = 0; block_pos != num_blocks; ++block_pos) {
        v[block_pos * block_size_in_elements + 20 * VectorSize + 2] = 0;
        v[block_pos * block_size_in_elements + 20 * VectorSize + 5] = 0;
        v[block_pos * block_size_in_elements + 25 * VectorSize + 6] = 2;
        v[block_pos * block_size_in_elements + 25 * VectorSize + 9] = 2;
    }
    assert(min_element(v.begin(), v.end()) == v.begin() + 20 * VectorSize + 2);
    assert(max_element(v.begin(), v.end()) == v.begin() + 25 * VectorSize + 6);
    assert(minmax_element(v.begin(), v.end()).first == v.begin() + 20 * VectorSize + 2);
    assert(minmax_element(v.begin(), v.end()).second == v.begin() + last_block_first_elem + 25 * VectorSize + 9);


    // same block in different vectors
    fill(v.begin(), v.end(), ElementType{1});
    for (size_t vector_pos = 0; vector_pos != block_size_in_vectors; ++vector_pos) {
        v[2 * block_size_in_elements + vector_pos * VectorSize + 2] = 0;
        v[2 * block_size_in_elements + vector_pos * VectorSize + 5] = 0;
        v[2 * block_size_in_elements + vector_pos * VectorSize + 6] = 2;
        v[2 * block_size_in_elements + vector_pos * VectorSize + 9] = 2;
    }
    assert(min_element(v.begin(), v.end()) == v.begin() + 2 * block_size_in_elements + 2);
    assert(max_element(v.begin(), v.end()) == v.begin() + 2 * block_size_in_elements + 6);
    assert(minmax_element(v.begin(), v.end()).first == v.begin() + 2 * block_size_in_elements + 2);
    assert(minmax_element(v.begin(), v.end()).second
           == v.begin() + 2 * block_size_in_elements + last_vector_first_elem + 9);
}

template <class BidIt>
void last_known_good_reverse(BidIt first, BidIt last) {
    for (; first != last && first != --last; ++first) {
        iter_swap(first, last);
    }
}

template <class T>
void test_case_reverse(vector<T>& actual, vector<T>& expected) {
    expected = actual;
    last_known_good_reverse(expected.begin(), expected.end());
    reverse(actual.begin(), actual.end());
    assert(expected == actual);
}

template <class T>
void test_reverse(mt19937_64& gen) {
    vector<T> actual;
    vector<T> expected;
    actual.reserve(dataCount);
    expected.reserve(dataCount);
    test_case_reverse(actual, expected);
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        actual.push_back(static_cast<T>(gen())); // intentionally narrows
        test_case_reverse(actual, expected);
    }
}

template <class T>
void test_case_reverse_copy(vector<T>& input) {
    auto expected = input;
    last_known_good_reverse(expected.begin(), expected.end());
    vector<T> output(input.size(), T{});
    assert(reverse_copy(input.begin(), input.end(), output.begin()) == output.end());
    assert(expected == output);
}

template <class T>
void test_reverse_copy(mt19937_64& gen) {
    vector<T> input;
    input.reserve(dataCount);
    test_case_reverse_copy(input);
    for (size_t attempts = 0; attempts < dataCount; ++attempts) {
        input.push_back(static_cast<T>(gen())); // intentionally narrows
        test_case_reverse_copy(input);
    }
}

template <class FwdIt1, class FwdIt2>
FwdIt2 last_known_good_swap_ranges(FwdIt1 first1, const FwdIt1 last1, FwdIt2 dest) {
    for (; first1 != last1; ++first1, ++dest) {
        iter_swap(first1, dest);
    }

    return dest;
}

template <class T>
void test_swap_ranges(mt19937_64& gen) {
    const auto fn = [&]() { return static_cast<T>(gen()); };
    vector<T> left(dataCount);
    vector<T> right(dataCount);
    generate(left.begin(), left.end(), fn);
    generate(right.begin(), right.end(), fn);

    auto leftCopy  = left;
    auto rightCopy = right;

    for (ptrdiff_t attempts = 0; attempts < static_cast<ptrdiff_t>(dataCount); ++attempts) {
        assert(right.begin() + attempts == swap_ranges(left.begin(), left.begin() + attempts, right.begin()));
        last_known_good_swap_ranges(leftCopy.begin(), leftCopy.begin() + attempts, rightCopy.begin());
        assert(left == leftCopy);
        assert(right == rightCopy);

        // also test unaligned input
        const auto endOffset = min(static_cast<ptrdiff_t>(dataCount), attempts + 1);
        assert(
            right.begin() + (endOffset - 1) == swap_ranges(left.begin() + 1, left.begin() + endOffset, right.begin()));
        last_known_good_swap_ranges(leftCopy.begin() + 1, leftCopy.begin() + endOffset, rightCopy.begin());
        assert(left == leftCopy);
        assert(right == rightCopy);
    }
}

void test_vector_algorithms(mt19937_64& gen) {
    test_count<char>(gen);
    test_count<signed char>(gen);
    test_count<unsigned char>(gen);
    test_count<short>(gen);
    test_count<unsigned short>(gen);
    test_count<int>(gen);
    test_count<unsigned int>(gen);
    test_count<long long>(gen);
    test_count<unsigned long long>(gen);

    test_find<char>(gen);
    test_find<signed char>(gen);
    test_find<unsigned char>(gen);
    test_find<short>(gen);
    test_find<unsigned short>(gen);
    test_find<int>(gen);
    test_find<unsigned int>(gen);
    test_find<long long>(gen);
    test_find<unsigned long long>(gen);

#if _HAS_CXX20
    test_gh_4449<uint8_t>();
    test_gh_4449<uint16_t>();
    test_gh_4449<uint32_t>();
    test_gh_4449<uint64_t>();
#endif // _HAS_CXX20

#if _HAS_CXX23
    test_find_last<char>(gen);
    test_find_last<signed char>(gen);
    test_find_last<unsigned char>(gen);
    test_find_last<short>(gen);
    test_find_last<unsigned short>(gen);
    test_find_last<int>(gen);
    test_find_last<unsigned int>(gen);
    test_find_last<long long>(gen);
    test_find_last<unsigned long long>(gen);
#endif // _HAS_CXX23

    test_min_max_element<char>(gen);
    test_min_max_element<signed char>(gen);
    test_min_max_element<unsigned char>(gen);
    test_min_max_element<short>(gen);
    test_min_max_element<unsigned short>(gen);
    test_min_max_element<int>(gen);
    test_min_max_element<unsigned int>(gen);
    test_min_max_element<long long>(gen);
    test_min_max_element<unsigned long long>(gen);

    test_min_max_element_floating<float>(gen);
    test_min_max_element_floating<double>(gen);
    test_min_max_element_floating<long double>(gen);

    test_min_max_element_pointers(gen);

    test_min_max_element_special_cases<int8_t, 16>(); // SSE2 vectors
    test_min_max_element_special_cases<int8_t, 32>(); // AVX2 vectors
    test_min_max_element_special_cases<int8_t, 64>(); // AVX512 vectors

    // Test VSO-1558536, a regression caused by GH-2447 that was specific to 64-bit types on x86.
    test_case_min_max_element(vector<uint64_t>{10, 0x8000'0000ULL, 20, 30});
    test_case_min_max_element(vector<uint64_t>{10, 20, 0xD000'0000'B000'0000ULL, 30, 0xC000'0000'A000'0000ULL});
    test_case_min_max_element(vector<int64_t>{10, 0x8000'0000LL, 20, 30});
    test_case_min_max_element(
        vector<int64_t>{-6604286336755016904, -4365366089374418225, 6104371530830675888, -8582621853879131834});

    test_reverse<char>(gen);
    test_reverse<signed char>(gen);
    test_reverse<unsigned char>(gen);
    test_reverse<short>(gen);
    test_reverse<unsigned short>(gen);
    test_reverse<int>(gen);
    test_reverse<unsigned int>(gen);
    test_reverse<long long>(gen);
    test_reverse<unsigned long long>(gen);
    test_reverse<float>(gen);
    test_reverse<double>(gen);
    test_reverse<long double>(gen);

    test_reverse_copy<char>(gen);
    test_reverse_copy<signed char>(gen);
    test_reverse_copy<unsigned char>(gen);
    test_reverse_copy<short>(gen);
    test_reverse_copy<unsigned short>(gen);
    test_reverse_copy<int>(gen);
    test_reverse_copy<unsigned int>(gen);
    test_reverse_copy<long long>(gen);
    test_reverse_copy<unsigned long long>(gen);
    test_reverse_copy<float>(gen);
    test_reverse_copy<double>(gen);
    test_reverse_copy<long double>(gen);

    test_swap_ranges<char>(gen);
    test_swap_ranges<short>(gen);
    test_swap_ranges<int>(gen);
    test_swap_ranges<unsigned int>(gen);
    test_swap_ranges<unsigned long long>(gen);
}

template <typename Container1, typename Container2>
void test_two_containers() {
    Container1 one                  = {10, 20, 30, 40, 50};
    Container2 two                  = {-1, -1, -1, -1, -1};
    static constexpr int reversed[] = {50, 40, 30, 20, 10};

    assert(reverse_copy(one.begin(), one.end(), two.begin()) == two.end());
    assert(equal(two.begin(), two.end(), begin(reversed), end(reversed)));

    static constexpr int squares[] = {1, 4, 9, 16, 25};
    static constexpr int cubes[]   = {1, 8, 27, 64, 125};
    one.assign(begin(squares), end(squares));
    two.assign(begin(cubes), end(cubes));

    assert(swap_ranges(one.begin(), one.end(), two.begin()) == two.end());
    assert(equal(one.begin(), one.end(), begin(cubes), end(cubes)));
    assert(equal(two.begin(), two.end(), begin(squares), end(squares)));
}

template <typename Container>
void test_one_container() {
    Container x                     = {10, 20, 30, 40, 50};
    static constexpr int reversed[] = {50, 40, 30, 20, 10};

    reverse(x.begin(), x.end());
    assert(equal(x.begin(), x.end(), begin(reversed), end(reversed)));

    test_two_containers<Container, vector<int>>();
    test_two_containers<Container, deque<int>>();
    test_two_containers<Container, list<int>>();
}

template <size_t N>
bool test_randomized_bitset(mt19937_64& gen) {
    string str;
    wstring wstr;
    str.reserve(N);
    wstr.reserve(N);

    while (str.size() != N) {
        uint64_t random_value = gen();

        for (int bits = 0; bits < 64 && str.size() != N; ++bits) {
            const auto character = '0' + (random_value & 1);
            str.push_back(static_cast<char>(character));
            wstr.push_back(static_cast<wchar_t>(character));
            random_value >>= 1;
        }
    }

    const bitset<N> b(str);

    assert(b.to_string() == str);
    assert(b.template to_string<wchar_t>() == wstr);

    return true;
}

template <size_t Base, size_t... Vals>
void test_randomized_bitset_base(index_sequence<Vals...>, mt19937_64& gen) {
    bool ignored[] = {test_randomized_bitset<Base + Vals>(gen)...};
    (void) ignored;
}

template <size_t Base, size_t Count>
void test_randomized_bitset_base_count(mt19937_64& gen) {
    test_randomized_bitset_base<Base>(make_index_sequence<Count>{}, gen);
}

void test_bitset(mt19937_64& gen) {
    assert(bitset<0>(0x0ULL).to_string() == "");
    assert(bitset<0>(0xFEDCBA9876543210ULL).to_string() == "");
    assert(bitset<15>(0x6789ULL).to_string() == "110011110001001");
    assert(bitset<15>(0xFEDCBA9876543210ULL).to_string() == "011001000010000");
    assert(bitset<32>(0xABCD1234ULL).to_string() == "10101011110011010001001000110100");
    assert(bitset<32>(0xFEDCBA9876543210ULL).to_string() == "01110110010101000011001000010000");
    assert(bitset<45>(0x1701D1729FFFULL).to_string() == "101110000000111010001011100101001111111111111");
    assert(bitset<45>(0xFEDCBA9876543210ULL).to_string() == "110101001100001110110010101000011001000010000");
    assert(bitset<64>(0xFEDCBA9876543210ULL).to_string()
           == "1111111011011100101110101001100001110110010101000011001000010000");
    assert(bitset<75>(0xFEDCBA9876543210ULL).to_string()
           == "000000000001111111011011100101110101001100001110110010101000011001000010000");

    assert(bitset<0>(0x0ULL).to_string<wchar_t>() == L"");
    assert(bitset<0>(0xFEDCBA9876543210ULL).to_string<wchar_t>() == L"");
    assert(bitset<15>(0x6789ULL).to_string<wchar_t>() == L"110011110001001");
    assert(bitset<15>(0xFEDCBA9876543210ULL).to_string<wchar_t>() == L"011001000010000");
    assert(bitset<32>(0xABCD1234ULL).to_string<wchar_t>() == L"10101011110011010001001000110100");
    assert(bitset<32>(0xFEDCBA9876543210ULL).to_string<wchar_t>() == L"01110110010101000011001000010000");
    assert(bitset<45>(0x1701D1729FFFULL).to_string<wchar_t>() == L"101110000000111010001011100101001111111111111");
    assert(bitset<45>(0xFEDCBA9876543210ULL).to_string<wchar_t>() == L"110101001100001110110010101000011001000010000");
    assert(bitset<64>(0xFEDCBA9876543210ULL).to_string<wchar_t>()
           == L"1111111011011100101110101001100001110110010101000011001000010000");
    assert(bitset<75>(0xFEDCBA9876543210ULL).to_string<wchar_t>()
           == L"000000000001111111011011100101110101001100001110110010101000011001000010000");

    assert(bitset<64>(0xFEDCBA9876543210ULL).to_string('o', 'x')
           == "xxxxxxxoxxoxxxooxoxxxoxoxooxxooooxxxoxxooxoxoxooooxxooxooooxoooo");
    assert(bitset<64>(0xFEDCBA9876543210ULL).to_string<wchar_t>(L'o', L'x')
           == L"xxxxxxxoxxoxxxooxoxxxoxoxooxxooooxxxoxxooxoxoxooooxxooxooooxoooo");

#ifdef __cpp_lib_char8_t
    assert(bitset<75>(0xFEDCBA9876543210ULL).to_string<char8_t>()
           == u8"000000000001111111011011100101110101001100001110110010101000011001000010000");
#endif // __cpp_lib_char8_t
    assert(bitset<75>(0xFEDCBA9876543210ULL).to_string<char16_t>()
           == u"000000000001111111011011100101110101001100001110110010101000011001000010000");
    assert(bitset<75>(0xFEDCBA9876543210ULL).to_string<char32_t>()
           == U"000000000001111111011011100101110101001100001110110010101000011001000010000"); // not vectorized

    test_randomized_bitset_base_count<512 - 5, 32 + 10>(gen);
}

void test_various_containers() {
    test_one_container<vector<int>>(); // contiguous, vectorizable
    test_one_container<deque<int>>(); // random-access, not vectorizable
    test_one_container<list<int>>(); // bidi, not vectorizable
}

#if _HAS_CXX20
constexpr bool test_constexpr() {
    const int a[] = {20, 10, 30, 30, 30, 30, 40, 60, 50};

    assert(count(begin(a), end(a), 30) == 4);
    assert(ranges::count(a, 30) == 4);

    assert(find(begin(a), end(a), 30) == begin(a) + 2);
    assert(ranges::find(a, 30) == begin(a) + 2);

#if _HAS_CXX23
    assert(begin(ranges::find_last(a, 30)) == begin(a) + 5);
    assert(end(ranges::find_last(a, 30)) == end(a));
#endif // _HAS_CXX23

    assert(min_element(begin(a), end(a)) == begin(a) + 1);
    assert(max_element(begin(a), end(a)) == end(a) - 2);
    assert(get<0>(minmax_element(begin(a), end(a))) == begin(a) + 1);
    assert(get<1>(minmax_element(begin(a), end(a))) == end(a) - 2);

    assert(ranges::min_element(a) == begin(a) + 1);
    assert(ranges::max_element(a) == end(a) - 2);
    assert(ranges::minmax_element(a).min == begin(a) + 1);
    assert(ranges::minmax_element(a).max == end(a) - 2);

    assert(ranges::min(a) == 10);
    assert(ranges::max(a) == 60);
    assert(ranges::minmax(a).min == 10);
    assert(ranges::minmax(a).max == 60);

    int b[size(a)];
    reverse_copy(begin(a), end(a), begin(b));
    assert(equal(rbegin(a), rend(a), begin(b), end(b)));

    int c[size(a)];
    ranges::reverse_copy(a, c);
    assert(equal(rbegin(a), rend(a), begin(c), end(c)));

    reverse(begin(b), end(b));
    assert(equal(begin(a), end(a), begin(b), end(b)));

    swap_ranges(begin(b), end(b), begin(c));
    assert(equal(rbegin(a), rend(a), begin(b), end(b)));
    assert(equal(begin(a), end(a), begin(c), end(c)));

    ranges::swap_ranges(b, c);
    assert(equal(begin(a), end(a), begin(b), end(b)));
    assert(equal(rbegin(a), rend(a), begin(c), end(c)));

    ranges::reverse(c);
    assert(equal(begin(a), end(a), begin(c), end(c)));

    return true;
}

static_assert(test_constexpr());
#endif // _HAS_CXX20

int main() {
#if _HAS_CXX20
    assert(test_constexpr());
#endif // _HAS_CXX20

    mt19937_64 gen;
    initialize_randomness(gen);

    test_vector_algorithms(gen);
    test_various_containers();
    test_bitset(gen);
#ifndef _M_CEE_PURE
#if defined(_M_IX86) || defined(_M_X64)
    disable_instructions(__ISA_AVAILABLE_AVX2);
    test_vector_algorithms(gen);
    test_various_containers();
    test_bitset(gen);

    disable_instructions(__ISA_AVAILABLE_SSE42);
    test_vector_algorithms(gen);
    test_various_containers();
    test_bitset(gen);
#endif // defined(_M_IX86) || defined(_M_X64)
#if defined(_M_IX86)
    disable_instructions(__ISA_AVAILABLE_SSE2);
    test_vector_algorithms(gen);
    test_various_containers();
    test_bitset(gen);
#endif // defined(_M_IX86)
#endif // _M_CEE_PURE
}
