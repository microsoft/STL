// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <random>
#include <type_traits>
#include <vector>

#include <test_vector_algorithms_support.hpp>

#pragma warning(disable : 4984) // if constexpr is a C++17 language extension
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++17-extensions"
#endif // __clang__

using namespace std;

template <class UnderlyingIt>
struct forward_iter_adaptor {
    using iterator_category = forward_iterator_tag;
    using reference         = typename UnderlyingIt::reference;
    using value_type        = typename UnderlyingIt::value_type;
    using pointer           = typename UnderlyingIt::pointer;
    using difference_type   = ptrdiff_t;

    constexpr forward_iter_adaptor() : ptr{} {}
    constexpr explicit forward_iter_adaptor(const UnderlyingIt ptr_) : ptr(ptr_) {}

    constexpr reference operator*() const {
        return *ptr;
    }
    constexpr pointer operator->() const {
        return ptr;
    }
    constexpr forward_iter_adaptor& operator++() {
        ++ptr;
        return *this;
    }
    constexpr forward_iter_adaptor operator++(int) {
        forward_iter_adaptor old;
        ++ptr;
        return old;
    }
    constexpr bool operator==(const forward_iter_adaptor& o) const {
        return ptr == o.ptr;
    }
    constexpr bool operator!=(const forward_iter_adaptor& o) const {
        return ptr != o.ptr;
    }

    UnderlyingIt ptr;
};

template <class FwdIt, class T>
auto last_known_good_search_n(FwdIt first, const FwdIt last, const size_t count, const T val) {
    // Deliberately using simple approach, not smart bidi/random iterators "check from the other end" stuff
    if (count == 0) {
        return first;
    }

    size_t found = 0;
    FwdIt match{};
    for (; first != last; ++first) {
        if (*first == val) {
            ++found;
            if (found == 1) {
                match = first;
            }

            if (found == count) {
                return match;
            }
        } else {
            found = 0;
        }
    }
    return last;
}

template <bool forward_only_iterators, class It, class T>
void test_case_search_n(const It first, const It last, const size_t count, const T val) {
    if constexpr (forward_only_iterators) {
        using iter_type = forward_iter_adaptor<It>;
        test_case_search_n<false>(iter_type(first), iter_type(last), count, val);
    } else {
        const auto expected = last_known_good_search_n(first, last, count, val);
        const auto actual   = search_n(first, last, count, val);
        assert(expected == actual);

#if _HAS_CXX20
        const auto ranges_actual = ranges::search_n(first, last, static_cast<ptrdiff_t>(count), val);
        assert(expected == begin(ranges_actual));
        if (expected == last) {
            assert(end(ranges_actual) == last);
        } else {
            assert(distance(expected, end(ranges_actual)) == static_cast<ptrdiff_t>(count));
        }
#endif // _HAS_CXX20
    }
}

template <class T, bool forward_only_iterators = false>
void test_search_n(mt19937_64& gen) {
    constexpr size_t lengthCount   = 70;
    constexpr size_t patternTrials = 2;
    using TD                       = conditional_t<sizeof(T) == 1, int, T>;
    uniform_int_distribution<TD> dis((numeric_limits<T>::min)(), (numeric_limits<T>::max)());
    vector<T> input_src;
    vector<T> input;
    input_src.reserve(dataCount);
    input.reserve(dataCount);

    for (;;) {
        for (size_t count = 0; count != lengthCount; ++count) {
            input = input_src;

            const T val = static_cast<T>(dis(gen));

            test_case_search_n<forward_only_iterators>(input.begin(), input.end(), count, val);

            if (input.empty()) {
                continue;
            }

            binomial_distribution<size_t> pattern_length_dis(count * 2, 0.5);
            uniform_int_distribution<size_t> pos_dis(0, input.size() - 1);

            for (size_t pattern = 0; pattern != patternTrials; ++pattern) {
                const size_t pattern_length = pattern_length_dis(gen);
                const size_t pattern_pos    = pos_dis(gen);

                if (pattern_length + pattern_pos <= input.size()) {
                    fill_n(input.begin() + static_cast<ptrdiff_t>(pattern_pos), pattern_length, val);

                    test_case_search_n<forward_only_iterators>(input.begin(), input.end(), count, val);
                }
            }
        }

        if (input.size() == dataCount) {
            break;
        }

        input_src.push_back(static_cast<T>(dis(gen)));
    }
}

void test_vector_algorithms(mt19937_64& gen) {
    test_search_n<int8_t>(gen);
    test_search_n<uint8_t>(gen);
    test_search_n<int16_t>(gen);
    test_search_n<uint16_t>(gen);
    test_search_n<int32_t>(gen);
    test_search_n<uint32_t>(gen);
    test_search_n<int64_t>(gen);
    test_search_n<uint64_t>(gen);
}

int main() {
    std::mt19937_64 gen;
    initialize_randomness(gen);

    run_tests_with_different_isa_levels([&] { test_vector_algorithms(gen); });

    // Test only one case with forward iterators. It is a different and complex code path, hence it's worth testing,
    // but it is not vectorized, so there's no point in trying different types.
    test_search_n<short, true>(gen);
}
