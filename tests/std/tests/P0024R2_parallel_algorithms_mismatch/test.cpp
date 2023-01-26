// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <execution>
#include <forward_list>
#include <iterator>
#include <list>
#include <utility>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

constexpr char one_char    = static_cast<char>(1);
const auto my_not_equal_to = [](auto a, auto b) { return a != b; };

template <template <class...> class Container>
void test_case_mismatch_signatures(const size_t testSize) {
    Container<char> defaults(testSize);
    Container<char> ones(testSize, one_char);

    using ExpectedType = pair<typename Container<char>::iterator, typename Container<char>::iterator>;

    assert((mismatch(par, defaults.begin(), defaults.end(), defaults.begin())
            == ExpectedType{defaults.end(), defaults.end()}));
    assert((
        mismatch(par, defaults.begin(), defaults.end(), ones.begin()) == ExpectedType{defaults.begin(), ones.begin()}));

    assert((mismatch(par, defaults.begin(), defaults.end(), defaults.begin(), defaults.end())
            == ExpectedType{defaults.end(), defaults.end()}));
    assert((mismatch(par, defaults.begin(), defaults.end(), ones.begin(), ones.end())
            == ExpectedType{defaults.begin(), ones.begin()}));

    assert((mismatch(par, defaults.begin(), defaults.end(), defaults.begin(), my_not_equal_to)
            == ExpectedType{defaults.begin(), defaults.begin()}));
    assert((mismatch(par, defaults.begin(), defaults.end(), ones.begin(), my_not_equal_to)
            == ExpectedType{defaults.end(), ones.end()}));

    assert((mismatch(par, defaults.begin(), defaults.end(), defaults.begin(), defaults.end(), my_not_equal_to)
            == ExpectedType{defaults.begin(), defaults.begin()}));
    assert((mismatch(par, defaults.begin(), defaults.end(), ones.begin(), ones.end(), my_not_equal_to)
            == ExpectedType{defaults.end(), ones.end()}));

    // Also test each counterexample position:
    auto defaultAnswer = defaults.begin();
    auto onesAnswer    = ones.begin();
    for (auto remainingAttempts = quadratic_complexity_case_limit; remainingAttempts != 0; --remainingAttempts) {
        assert((mismatch(defaults.begin(), defaults.end(), ones.begin()) == ExpectedType{defaultAnswer, onesAnswer}));
        assert((mismatch(defaults.begin(), defaults.end(), ones.begin(), ones.end())
                == ExpectedType{defaultAnswer, onesAnswer}));
        if (defaultAnswer == defaults.end()) {
            return;
        }

        *onesAnswer = {};
        ++defaultAnswer;
        ++onesAnswer;
    }
}

template <template <class...> class Container1, template <class...> class Container2>
void test_case_mismatch_lengths(const size_t testSize) {
    Container1<char> defaults(testSize);
    Container2<char> ones(testSize, one_char);
    using It1 = typename Container1<char>::iterator;
    using It2 = typename Container2<char>::iterator;

    auto expectedOnes = ones.insert(ones.end(), one_char);
    assert((mismatch(par, defaults.begin(), defaults.end(), ones.begin(), ones.end(), my_not_equal_to)
            == pair<It1, It2>{defaults.end(), expectedOnes}));
    assert((mismatch(par, ones.begin(), ones.end(), defaults.begin(), defaults.end(), my_not_equal_to)
            == pair<It2, It1>{expectedOnes, defaults.end()}));
}

int main() {
#ifndef _M_CEE // TRANSITION, VSO-1659489
    parallel_test_case(test_case_mismatch_signatures<forward_list>);
    parallel_test_case(test_case_mismatch_signatures<list>);
    parallel_test_case(test_case_mismatch_signatures<vector>);

    parallel_test_case(test_case_mismatch_lengths<vector, vector>);
    parallel_test_case(test_case_mismatch_lengths<list, list>);
    parallel_test_case(test_case_mismatch_lengths<list, vector>);
    parallel_test_case(test_case_mismatch_lengths<vector, list>);
#endif // _M_CEE
}
