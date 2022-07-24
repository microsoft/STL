// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <execution>
#include <functional>
#include <iterator>
#include <numeric>
#include <random>
#include <string>
#include <string_view>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

bool starts_with(const string_view target, const string_view test) {
    return target.substr(0, test.size()) == test;
}

constexpr auto alphaNum = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"sv;
vector<string> prepare_alpha_strings(const size_t testSize) {
    vector<string> result(testSize);
    auto letterPicker = alphaNum.begin();
    for (string& testEntry : result) {
        testEntry.push_back(*letterPicker);
        testEntry.push_back('?');
        ++letterPicker;
        if (letterPicker == alphaNum.end()) {
            letterPicker = alphaNum.begin();
        }
    }

    return result;
}

const auto maxAlphaStrings = prepare_alpha_strings(max_parallel_test_case_n);
const auto removeOne       = [](string str) {
    str.pop_back();
    return str;
};
const auto addTen = [](unsigned int x) { return x + 10U; };

bool is_alpha_string(string_view target, const string_view prefix) {
    if (!starts_with(target, prefix)) {
        return false;
    }

    target.remove_prefix(prefix.size());
    while (starts_with(target, alphaNum)) {
        target.remove_prefix(alphaNum.size());
    }

    return starts_with(alphaNum, target);
}

void assert_is_alpha_string_prefix_sum(const vector<string>& actualResults, const string_view prefix) {
    size_t expectedSize = prefix.size();
    for (auto&& actual : actualResults) {
        assert(actual.size() == expectedSize);
        assert(is_alpha_string(actual, prefix));
        ++expectedSize;
    }
}

void test_case_transform_exclusive_scan_bop_parallel(const size_t testSize, mt19937& gen) {
    vector<unsigned int> s(testSize);
    generate(s.begin(), s.end(), ref(gen));
    vector<unsigned int> d(testSize);
    assert(d.end() == transform_exclusive_scan(par, s.cbegin(), s.cend(), d.begin(), 1U, multiplies<>{}, addTen));
    transform_exclusive_scan(s.cbegin(), s.cend(), s.begin(), 1U, multiplies<>{}, addTen);
    assert(s == d);
}

void test_case_transform_exclusive_scan_bop_parallel_associative(const size_t testSize) {
    // string concat is associative but not commutative
    vector<string> results(testSize);
    transform_exclusive_scan(par, maxAlphaStrings.begin(), maxAlphaStrings.begin() + static_cast<ptrdiff_t>(testSize),
        results.begin(), "frobinate"s, plus<>{}, removeOne);
    assert_is_alpha_string_prefix_sum(results, "frobinate");
}

void test_case_transform_exclusive_scan_bop_parallel_associative_in_place(const size_t testSize) {
    // also test where dest == first
    vector<string> results(maxAlphaStrings.begin(), maxAlphaStrings.begin() + static_cast<ptrdiff_t>(testSize));
    transform_exclusive_scan(par, results.begin(), results.end(), results.begin(), "frobinate"s, plus<>{}, removeOne);
    assert_is_alpha_string_prefix_sum(results, "frobinate");
}

struct inputType {
    inputType() = delete;
    /* implicit */ inputType(int) {} // so that the test can make an array
    inputType(const inputType&)            = delete;
    inputType& operator=(const inputType&) = delete;
};

struct transformedType {
    transformedType() = delete;
    /* implicit */ transformedType(int) {}
    transformedType(const transformedType&)            = delete;
    transformedType& operator=(const transformedType&) = delete;
};

struct bopResult {
    bopResult() = delete;
    /* implicit */ bopResult(int) {}
    bopResult(const bopResult&)            = delete;
    bopResult& operator=(const bopResult&) = delete;
};

struct intermediateType {
    intermediateType() = delete;
    explicit intermediateType(int) {} // so that the test can make one of these
    explicit intermediateType(transformedType&&) {} // Intermediate tmp(unary_op(*first))
    // Intermediate tmp(binary_op((one of tmp, move(tmp), unary_op(*first)), unary_op(*first)))
    explicit intermediateType(bopResult&&) {}
    intermediateType(const intermediateType&)            = delete;
    intermediateType(intermediateType&&)                 = default; // tmp = move(tmp)
    intermediateType& operator=(const intermediateType&) = delete;
    intermediateType& operator=(intermediateType&&)      = default;
    // tmp = binary_op((one of tmp, move(tmp), *first), *first)
    intermediateType& operator=(bopResult&&) {
        return *this;
    }
};

struct outputType {
    outputType() = delete;
    /* implicit */ outputType(int) {} // so that the test can make an array
    outputType(const outputType&)            = delete;
    outputType& operator=(const outputType&) = delete;
    outputType& operator=(outputType&&)      = delete;

    // in the first pass
    outputType& operator=(intermediateType&) {
        return *this;
    }
    outputType& operator=(intermediateType&&) {
        return *this;
    }

    // in the second pass
    outputType& operator=(bopResult&&) {
        return *this;
    }
};

struct transformUop {
    transformedType operator()(inputType&) {
        return 0;
    };
};

struct typesBop {
    // tmp = binary_op( (one of tmp, move(tmp), unary_op(*first)), unary_op(*first))
    bopResult operator()(intermediateType&, transformedType&&) {
        return 0;
    }
    bopResult operator()(intermediateType&&, transformedType&&) {
        return 0;
    }
    bopResult operator()(transformedType&&, transformedType&&) {
        return 0;
    }

    // tmp = binary_op( (one of tmp, move(tmp)), (one of tmp, move(tmp)) )
    bopResult operator()(intermediateType&, intermediateType&) {
        return 0;
    }
    bopResult operator()(intermediateType&, intermediateType&&) {
        return 0;
    }
    bopResult operator()(intermediateType&&, intermediateType&) {
        return 0;
    }
    bopResult operator()(intermediateType&&, intermediateType&&) {
        return 0;
    }

    // *result = binary_op(tmp, move(*result))
    bopResult operator()(intermediateType&, outputType&&) {
        return 0;
    }
};

void test_case_transform_exclusive_scan_init_writes_intermediate_type() {
    inputType input[2]{0, 0};
    outputType output[2]{0, 0};
    transform_exclusive_scan(begin(input), end(input), output, intermediateType{0}, typesBop{}, transformUop{});
    transform_exclusive_scan(par, begin(input), end(input), output, intermediateType{0}, typesBop{}, transformUop{});
}

int main() {
    mt19937 gen(1729);

    parallel_test_case(test_case_transform_exclusive_scan_bop_parallel, gen);
    parallel_test_case(test_case_transform_exclusive_scan_bop_parallel_associative);
    parallel_test_case(test_case_transform_exclusive_scan_bop_parallel_associative_in_place);
    test_case_transform_exclusive_scan_init_writes_intermediate_type();
}
