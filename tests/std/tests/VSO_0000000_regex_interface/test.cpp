// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <functional>
#include <iterator>
#include <list>
#include <regex>
#include <sstream>
#include <string>

using namespace std;
using namespace std::regex_constants;

void test_devdiv_165070_regex_should_accept_wchar_t() {
    // basic_regex's constructor from input iterators, as well as the
    // sub_match/element comparators, were internally using std::string.
    // This caused the ctor to truncate wchar_t to char, and caused the
    // sub_match/wchar_t comparators to fail to compile.
    //
    // In order to verify the ctor fix, this test needs to use wchar_ts
    // above 255.
    //
    // L"\x043A\x043E\x0448\x043A\x0430" is "koshka", Russian for "cat".
    // ("sh" is a single Russian letter.)
    const wstring cat(L"\x043E.\x043A");

    wistringstream iss(cat);

    const istream_iterator<wchar_t, wchar_t> begin(iss);
    const istream_iterator<wchar_t, wchar_t> end;

    const wregex r(begin, end);
    wcmatch m;

    assert(regex_search(L"\x043A\x043E\x0448\x043A\x0430", m, r));
    assert(m[0] == L"\x043E\x0448\x043A");
    assert(m[0] != L'\x043E');
    assert(L'\x043E' != m[0]);
    assert(m[0] < L'\x043F');
    assert(L'\x043F' >= m[0]);
}

void test_devdiv_822474_match_results_should_be_ready_after_regex_search() {
    // DevDiv-822474 "<regex>: match_results::ready() returns false after regex_search called [libs-conformance]"

    // N3797 28.10.1 [re.results.const]/3:
    // match_results(const Allocator& a = Allocator());
    // Postconditions: ready() returns false. size() returns 0.

    // 28.11.2 [re.alg.match]/3:
    // Postconditions: m.ready() == true in all cases.
    // If the function returns false, then the effect on parameter m is
    // unspecified except that m.size() returns 0 and m.empty() returns true.

    const regex abc("abc");
    const regex meowpurr("me(ow)pu(rr)");

    cmatch m1;

    // Test not-ready.
    assert(!m1.ready());
    assert(m1.size() == 0);

    // Test not-ready ==> empty.
    assert(!regex_match("xyz", m1, abc));
    assert(m1.ready());
    assert(m1.size() == 0);

    cmatch m2;

    // Test not-ready ==> full.
    assert(regex_match("abc", m2, abc));
    assert(m2.ready());
    assert(m2.size() == 1);

    // Test full ==> full.
    assert(regex_match("meowpurr", m2, meowpurr));
    assert(m2.ready());
    assert(m2.size() == 3);

    // Test full ==> empty.
    assert(!regex_match("xyz", m2, abc));
    assert(m2.ready());
    assert(m2.size() == 0);
}

void test_dev10_900584_should_handle_quantified_alternates() {
    const string s("12x34xx56x78");
    smatch m;
    const regex r("^([0-9]|[0-9]x[0-9])+xx");

    assert(regex_search(s, m, r));
    assert(m[0] == "12x34xx");
}

void test_devdiv_903531_regex_should_have_correct_suffix_matching() {
    auto test_search = [](const string& s, const string& reg, const string::const_iterator match_begin,
                           const string::const_iterator match_end) {
        smatch m;
        const regex r(reg);

        assert(regex_search(s, m, r));

        assert(m.size() == 1);
        assert(!m.empty());

        assert(m.prefix().first == s.begin());
        assert(m.prefix().second == m[0].first);
        assert(m.prefix().matched == (m.prefix().first != m.prefix().second));

        assert(m.suffix().first == m[0].second);
        assert(m.suffix().second == s.end());
        assert(m.suffix().matched == (m.suffix().first != m.suffix().second));

        assert(m[0].first == match_begin);
        assert(m[0].second == match_end);
        assert(m[0].matched);
    };

    const string s("aaabbbbccccc");

    const auto b_begin = s.begin() + 3;
    const auto b_end   = s.begin() + 7;

    test_search(s, "a+b+c+", s.begin(), s.end());
    test_search(s, "b+c+", b_begin, s.end());
    test_search(s, "a+b+", s.begin(), b_end);
    test_search(s, "b+", b_begin, b_end);

    smatch m;
    const regex r("a+b+c+");

    assert(regex_match(s, m, r));

    assert(m.size() == 1);
    assert(!m.empty());

    assert(m.prefix().first == s.begin());
    assert(m.prefix().second == s.begin());
    assert(!m.prefix().matched);

    assert(m.suffix().first == s.end());
    assert(m.suffix().second == s.end());
    assert(!m.suffix().matched);

    assert(m[0].first == s.begin());
    assert(m[0].second == s.end());
    assert(m[0].matched);
}

void test_dev10_505773_default_constructed_regex_should_not_match_empty_string() {
    regex r;
    assert(!regex_match("", r));
}

template <typename ContainerT>
void test_LWG_2217_sub_match_should_not_slice_nulls() {
    // LWG-2217: "operator==(sub_match, string) slices on embedded '\0's"
    const char input[] = {'a', 'b', '\0', 'c', 'd'};
    const ContainerT subject(begin(input), end(input));
    const regex pattern(".(.).*");

    match_results<typename ContainerT::const_iterator> results;
    assert(regex_match(subject.begin(), subject.end(), results, pattern));

    string test(begin(input), end(input));
    assert(results[0] == test);
    assert(test == results[0]);
    assert(!(results[0] < test));
    assert(!(test < results[0]));
    assert(!(results[0] > test));
    assert(!(test > results[0]));
    test.back() = 'e';
    assert(results[0] != test);
    assert(test != results[0]);
    assert(results[0] < test);
    assert(!(test < results[0]));
    assert(!(results[0] > test));
    assert(test > results[0]);

    assert(results[1] == 'b');
    assert('b' == results[1]);
    assert(!(results[1] < 'b'));
    assert(!(results[1] > 'b'));
    assert(!(results[1] < 'a'));
    assert('a' < results[1]);
    assert(results[1] > 'a');
    assert(!('a' > results[1]));
}

const string strEmpty;
const string strA("a");
const string strAA("aa");
const string strB("b");
const string strGibberish("{match failed}");
const string* const strExamples[] = {&strEmpty, &strA, &strAA, &strB};

// test_case_sub_match_compare(left, op, right)
// verifies that a sub_match made out of left and right causes left op right to be true
// for all 3 sub_match::compare overloads and all *42* sub_match operators
//
// if str is nullptr, uses a sub_match with gibberish iterators but matched = false
template <typename ContainerT, typename CompareFunc>
void test_case_sub_match_compare(const string* const left, CompareFunc compareFunc, const string* const right) {
    typedef typename ContainerT::const_iterator Iterator;

    const bool isLess    = compareFunc(-1, 0);
    const bool isEqual   = compareFunc(0, 0);
    const bool isGreater = compareFunc(1, 0);

    const string& leftStr          = left == nullptr ? strEmpty : *left;
    const string& leftContainerStr = left == nullptr ? strGibberish : *left;
    const ContainerT leftContainer(leftContainerStr.begin(), leftContainerStr.end());
    sub_match<Iterator> leftMatch;
    leftMatch.first   = leftContainer.begin();
    leftMatch.second  = leftContainer.end();
    leftMatch.matched = left != nullptr;

    const string& rightStr          = right == nullptr ? strEmpty : *right;
    const string& rightContainerStr = right == nullptr ? strGibberish : *right;
    const ContainerT rightContainer(rightContainerStr.begin(), rightContainerStr.end());
    sub_match<Iterator> rightMatch;
    rightMatch.first   = rightContainer.begin();
    rightMatch.second  = rightContainer.end();
    rightMatch.matched = right != nullptr;

    printf("test_case_sub_match_compare(%s, OP, %s)\n", leftContainerStr.c_str(), rightContainerStr.c_str());

    // compare(const sub_match&)
    assert(compareFunc(leftMatch.compare(rightMatch), 0));
    // compare(const string_type&)
    assert(compareFunc(leftMatch.compare(rightStr), 0));
    // compare(const value_type*)
    assert(compareFunc(leftMatch.compare(rightStr.c_str()), 0));

    // Operators are in N4567 28.9.2[re.submatch.op] order

    // Operators against sub_match instances (note different order than others!)
    assert(isEqual == (leftMatch == rightMatch));
    assert(isEqual != (leftMatch != rightMatch));
    assert(isLess == (leftMatch < rightMatch));
    assert(isGreater != (leftMatch <= rightMatch));
    assert(isLess != (leftMatch >= rightMatch)); // 5
    assert(isGreater == (leftMatch > rightMatch));

    // Operators with const basic_string& on left
    assert(isEqual == (leftStr == rightMatch));
    assert(isEqual != (leftStr != rightMatch));
    assert(isLess == (leftStr < rightMatch));
    assert(isGreater == (leftStr > rightMatch)); // 10
    assert(isLess != (leftStr >= rightMatch));
    assert(isGreater != (leftStr <= rightMatch));

    // Operators with const basic_string& on right
    assert(isEqual == (leftMatch == rightStr));
    assert(isEqual != (leftMatch != rightStr));
    assert(isLess == (leftMatch < rightStr)); // 15
    assert(isGreater == (leftMatch > rightStr));
    assert(isLess != (leftMatch >= rightStr));
    assert(isGreater != (leftMatch <= rightStr));

    // Operators with const value_type* on the left
    assert(isEqual == (leftStr.c_str() == rightMatch));
    assert(isEqual != (leftStr.c_str() != rightMatch)); // 20
    assert(isLess == (leftStr.c_str() < rightMatch));
    assert(isGreater == (leftStr.c_str() > rightMatch));
    assert(isLess != (leftStr.c_str() >= rightMatch));
    assert(isGreater != (leftStr.c_str() <= rightMatch));

    // Operators with const value_type* on the right
    assert(isEqual == (leftMatch == rightStr.c_str())); // 25
    assert(isEqual != (leftMatch != rightStr.c_str()));
    assert(isLess == (leftMatch < rightStr.c_str()));
    assert(isGreater == (leftMatch > rightStr.c_str()));
    assert(isLess != (leftMatch >= rightStr.c_str()));
    assert(isGreater != (leftMatch <= rightStr.c_str())); // 30

    // Operators with const value_type& on the left
    if (leftStr.size() == 1) {
        assert(isEqual == (leftStr[0] == rightMatch));
        assert(isEqual != (leftStr[0] != rightMatch));
        assert(isLess == (leftStr[0] < rightMatch));
        assert(isGreater == (leftStr[0] > rightMatch));
        assert(isLess != (leftStr[0] >= rightMatch)); // 35
        assert(isGreater != (leftStr[0] <= rightMatch));
    }

    // Operators with const value_type& on the right
    if (rightStr.size() == 1) {
        assert(isEqual == (leftMatch == rightStr[0]));
        assert(isEqual != (leftMatch != rightStr[0]));
        assert(isLess == (leftMatch < rightStr[0]));
        assert(isGreater == (leftMatch > rightStr[0])); // 40
        assert(isLess != (leftMatch >= rightStr[0]));
        assert(isGreater != (leftMatch <= rightStr[0]));
    }
}

template <typename ContainerT>
void test_VSO_177524_sub_match_compare_should_not_construct_unnecessary_basic_strings() {
    test_case_sub_match_compare<ContainerT>(nullptr, equal_to<>{}, nullptr);

    for (const string* const left : strExamples) {
        if (left->empty()) {
            test_case_sub_match_compare<ContainerT>(left, equal_to<>{}, nullptr);
            test_case_sub_match_compare<ContainerT>(nullptr, equal_to<>{}, left);
        } else {
            test_case_sub_match_compare<ContainerT>(left, greater<>{}, nullptr);
            test_case_sub_match_compare<ContainerT>(nullptr, less<>{}, left);
        }

        for (const string* const right : strExamples) {
            if (*left < *right) {
                test_case_sub_match_compare<ContainerT>(left, less<>{}, right);
            } else if (*left > *right) {
                test_case_sub_match_compare<ContainerT>(left, greater<>{}, right);
            } else {
                test_case_sub_match_compare<ContainerT>(left, equal_to<>{}, right);
            }
        }
    }
}

void test_VSO_180466_regex_replace_ARRAY() {
    const regex pattern("d");
    const string format("x");
    const char input[]    = "abcdefg";
    const char expected[] = "abcxefg";
    char buff[sizeof(input)];

    // tests that the following regex_replace calls don't emit _SCL_INSECURE_DEPRECATE_FN warnings
    fill(begin(buff), end(buff), '\0');
    assert(end(buff) == regex_replace(buff, begin(input), end(input), pattern, format));
    assert(equal(begin(buff), end(buff), begin(expected), end(expected)));
    fill(begin(buff), end(buff), '\0');
    assert(end(buff) == regex_replace(buff, begin(input), end(input), pattern, format.c_str()));
    assert(equal(begin(buff), end(buff), begin(expected), end(expected)));
}

void test_VSO_180466_regex_search_missing_Unchecked_call() {
    const regex pattern("d");
    const string checked_input("abcdefg");
    assert(regex_search(begin(checked_input), end(checked_input), pattern));
}

void test_VSO_226914_match_prev_avail() {
    // N.B. assumes our nonstandard multiline behavior. See also: LWG-2343, LWG-2503
    const char bol_haystack[] = {'\n', 'a'};
    const regex bol_anchor(R"(^a)");
    assert(regex_match(bol_haystack + 1, end(bol_haystack), bol_anchor));
    assert(!regex_match(bol_haystack + 1, end(bol_haystack), bol_anchor, match_not_bol));
    assert(regex_match(bol_haystack + 1, end(bol_haystack), bol_anchor, match_prev_avail));
    assert(regex_match(bol_haystack + 1, end(bol_haystack), bol_anchor, match_not_bol | match_prev_avail));

    const char word_haystack_match[] = {'#', 'a'};
    const auto word_haystack_mbegin  = word_haystack_match + 1;
    const auto word_haystack_mend    = end(word_haystack_match);

    const char word_haystack_fail[] = {'b', 'a'};
    const auto word_haystack_fbegin = word_haystack_fail + 1;
    const auto word_haystack_fend   = end(word_haystack_fail);

    const regex word_anchor(R"(\ba)");
    const regex neg_word_anchor(R"(\Ba)");

    assert(regex_match(word_haystack_mbegin, word_haystack_mend, word_anchor));
    assert(!regex_match(word_haystack_mbegin, word_haystack_mend, word_anchor, match_not_bow));
    assert(regex_match(word_haystack_mbegin, word_haystack_mend, word_anchor, match_prev_avail));
    assert(regex_match(word_haystack_mbegin, word_haystack_mend, word_anchor, match_not_bow | match_prev_avail));

    assert(!regex_match(word_haystack_mbegin, word_haystack_mend, neg_word_anchor));
    assert(regex_match(word_haystack_mbegin, word_haystack_mend, neg_word_anchor, match_not_bow));
    assert(!regex_match(word_haystack_mbegin, word_haystack_mend, neg_word_anchor, match_prev_avail));
    assert(!regex_match(word_haystack_mbegin, word_haystack_mend, neg_word_anchor, match_not_bow | match_prev_avail));

    assert(regex_match(word_haystack_fbegin, word_haystack_fend, word_anchor));
    assert(!regex_match(word_haystack_fbegin, word_haystack_fend, word_anchor, match_not_bow));
    assert(!regex_match(word_haystack_fbegin, word_haystack_fend, word_anchor, match_prev_avail));
    assert(!regex_match(word_haystack_fbegin, word_haystack_fend, word_anchor, match_not_bow | match_prev_avail));

    assert(!regex_match(word_haystack_fbegin, word_haystack_fend, neg_word_anchor));
    assert(regex_match(word_haystack_fbegin, word_haystack_fend, neg_word_anchor, match_not_bow));
    assert(regex_match(word_haystack_fbegin, word_haystack_fend, neg_word_anchor, match_prev_avail));
    assert(regex_match(word_haystack_fbegin, word_haystack_fend, neg_word_anchor, match_not_bow | match_prev_avail));
}

int main() {
    test_devdiv_165070_regex_should_accept_wchar_t();
    test_devdiv_822474_match_results_should_be_ready_after_regex_search();
    test_dev10_900584_should_handle_quantified_alternates();
    test_devdiv_903531_regex_should_have_correct_suffix_matching();
    test_dev10_505773_default_constructed_regex_should_not_match_empty_string();
    test_LWG_2217_sub_match_should_not_slice_nulls<string>();
    test_LWG_2217_sub_match_should_not_slice_nulls<list<char>>();
    test_VSO_177524_sub_match_compare_should_not_construct_unnecessary_basic_strings<string>();
    test_VSO_177524_sub_match_compare_should_not_construct_unnecessary_basic_strings<list<char>>();
    test_VSO_180466_regex_replace_ARRAY();
    test_VSO_180466_regex_search_missing_Unchecked_call();
    test_VSO_226914_match_prev_avail();
}
