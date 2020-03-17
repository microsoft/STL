// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <assert.h>
#include <forward_list>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <string_view>
#include <utility>
#include <vector>

using namespace std;

template <typename RanIt, typename PredEq = equal_to<>>
void assert_equal(
    pair<RanIt, RanIt> result, RanIt base, ptrdiff_t expectedOffset, ptrdiff_t expectedLength, PredEq eq = {}) {
    const RanIt target    = base + expectedOffset;
    const RanIt targetEnd = target + expectedLength;
    assert(result.first == target);
    assert(result.second == targetEnd);
    assert(equal(result.first, result.second, target, targetEnd, eq));
}

// Tests for Boyer-Moore Table2 construction
// This isn't easily observable from the public interface (other than that matches will be wrong),
// and the algorithm is self contained, so we're testing it directly.
void test_case_boyer_moore_table2_construction(string_view sv, initializer_list<ptrdiff_t> expectedValues) {
    assert(sv.size() == expectedValues.size());
    vector<ptrdiff_t> results(sv.size());
    equal_to<> eq;
    _Build_boyer_moore_delta_2_table(results.data(), sv.data(), static_cast<ptrdiff_t>(sv.size()), eq);
    assert(equal(results.begin(), results.end(), expectedValues.begin(), expectedValues.end()));
}

void test_boyer_moore_table2_construction() {
    // Test cases from Boyer and Moore's original 1977 paper on page 4:
    test_case_boyer_moore_table2_construction("abcxxxabc", {14, 13, 12, 11, 10, 9, 11, 10, 1});
    test_case_boyer_moore_table2_construction("abyxcdeyx", {17, 16, 15, 14, 13, 12, 7, 10, 1});
    // Test case from Knuth, Morris, and Pratt's updated 1977 paper, from which our delta2
    // construction algorithm is derived, on page 20:
    test_case_boyer_moore_table2_construction("badbacbacba", {19, 18, 17, 16, 15, 8, 13, 12, 8, 12, 1});
}

template <typename Searcher, typename Result, typename... Args>
void test_case_copies(const Searcher& searcher, const Result& result, const string_view haystack, const Args... args) {
    {
        const Searcher searcherCopy(searcher);
        const auto result2 = searcherCopy(haystack.begin(), haystack.end());
        assert(result == result2);
    }

    {
        Searcher assignedSearcher(string_view::iterator{}, string_view::iterator{}, args...);
        assignedSearcher   = searcher;
        const auto result2 = assignedSearcher(haystack.begin(), haystack.end());
        assert(result == result2);
    }
}

template <typename Searcher, typename... Args>
void test_case_searcher_found(
    const string_view needle, const string_view haystack, const ptrdiff_t expectedPosition, const Args... args) {
    const Searcher searcher(needle.begin(), needle.end(), args...);
    const auto result = searcher(haystack.begin(), haystack.end());
    assert(result.first == search(haystack.begin(), haystack.end(), searcher));
    assert_equal(result, haystack.begin(), expectedPosition, static_cast<ptrdiff_t>(needle.size()));
    test_case_copies(searcher, result, haystack, args...);
}

template <typename Searcher, typename... Args>
void test_case_searcher_not_found(const string_view needle, const string_view haystack, const Args... args) {
    const Searcher searcher(needle.begin(), needle.end(), args...);
    const auto result = searcher(haystack.begin(), haystack.end());
    assert(result.first == haystack.end());
    assert(result.second == haystack.end());
    test_case_copies(searcher, result, haystack, args...);
}

template <typename Searcher, typename... Args>
void test_case_searcher(const Args... args) {
    test_case_searcher_found<Searcher>("", "", 0, args...);
    test_case_searcher_found<Searcher>("", "anything", 0, args...);
    test_case_searcher_found<Searcher>("AT-THAT", "WHICH-FINALLY-HALTS.--AT-THAT-POINT", 22, args...);
    test_case_searcher_not_found<Searcher>("AT-THAT", "WHICH-FINALLY-HALTS.--AT?THAT-POINT", args...);
    test_case_searcher_found<Searcher>("beginning", "beginning cat dog elk end", 0, args...);
    test_case_searcher_found<Searcher>("end", "beginning cat dog elk end", 22, args...);
    test_case_searcher_found<Searcher>("repeated", "a, b, c, d repeated repeated a, b, c, d repeated", 11, args...);
    test_case_searcher_found<Searcher>(
        "High bit chars \xCD :D", "Sometimes random text triggers High bit chars \xCD :D", 31, args...);

    test_case_searcher_found<Searcher>("abcd", "ddddabcd", 4, args...);
    test_case_searcher_not_found<Searcher>("abcd", "ddddxbcd", args...);
}

struct FancyHash {
    FancyHash() = delete;
    explicit FancyHash(int) {}
    FancyHash(const FancyHash&) = default;
    FancyHash& operator=(const FancyHash&) = delete;

    size_t operator()(const char c) const {
        return hash<char>{}(c);
    }
};

struct FancyEqual {
    FancyEqual() = delete;
    explicit FancyEqual(int) {}
    FancyEqual(const FancyEqual&) = default;
    FancyEqual& operator=(const FancyEqual&) = delete;

    bool operator()(const char lhs, const char rhs) const {
        return lhs == rhs;
    }
};

struct CaseInsensitiveHashEqual {
    // assumes ASCII
    static char upper(char c) {
        if (c >= 'a' && c <= 'z') {
            c -= 'a' - 'A';
        }

        return c;
    }

    size_t operator()(const char c) const {
        return hash<char>{}(upper(c));
    }

    bool operator()(const char lhs, const char rhs) const {
        return upper(lhs) == upper(rhs);
    }
};

template <class Hash, class Pred_eq>
void test_case_boyer_moore_functors(const Hash& h, const Pred_eq& pr) {
    test_case_searcher<boyer_moore_searcher<string_view::iterator, Hash, Pred_eq>>(h, pr);
    test_case_searcher<boyer_moore_horspool_searcher<string_view::iterator, Hash, Pred_eq>>(h, pr);
}

template <template <class RanIt, class Hash, class Pred_eq> class Searcher>
void test_case_case_insensitive() {
    test_case_searcher_found<Searcher<string_view::iterator, CaseInsensitiveHashEqual, CaseInsensitiveHashEqual>>(
        "At-THaT", "WHICH-FINALLY-HALTS.--aT-ThAT-POINT", 22);
    test_case_searcher_not_found<Searcher<string_view::iterator, CaseInsensitiveHashEqual, CaseInsensitiveHashEqual>>(
        "aT-THAT", "WHICH-FINALLY-HALTS.--At?ThaT-POINT");
}

struct UdtInt {
    int i;
};

struct UdtEq {
    bool operator()(const UdtInt& lhs, const UdtInt& rhs) const {
        return lhs.i == rhs.i;
    }
};

struct UdtHash {
    size_t operator()(const UdtInt& key) const {
        return hash<int>{}(key.i);
    }
};

void test_case_default_searcher_special_cases() {
    const forward_list<UdtInt> needle({{4}, {3}}); // tests forward iterators, and UDTs
    const vector<UdtInt> haystackVec{{1}, {2}, {3}, {4}, {3}, {5}, {6}, {10}, {11}};
    const forward_list<UdtInt> haystackList(haystackVec.begin(), haystackVec.end());
    const default_searcher<forward_list<UdtInt>::const_iterator, UdtEq> searcher(needle.begin(), needle.end());
    const auto vecAnswer  = searcher(haystackVec.begin(), haystackVec.end()); // also tests mismatching iterator types
    const auto listAnswer = searcher(haystackList.begin(), haystackList.end());

    assert(distance(haystackVec.begin(), vecAnswer.first) == 3);
    assert(distance(haystackVec.begin(), vecAnswer.second) == 5);
    assert(equal(vecAnswer.first, vecAnswer.second, needle.begin(), needle.end(), UdtEq{}));

    assert(distance(haystackList.begin(), listAnswer.first) == 3);
    assert(distance(haystackList.begin(), listAnswer.second) == 5);
    assert(equal(listAnswer.first, listAnswer.second, needle.begin(), needle.end(), UdtEq{}));
}

template <template <class RanIt, class Hash, class Pred_eq> class Searcher>
void test_case_BM_udts() {
    const vector<UdtInt> needle({{4}, {3}});
    const UdtInt haystack[] = {{1}, {2}, {3}, {4}, {3}, {5}, {6}, {10}, {11}};

    Searcher<vector<UdtInt>::const_iterator, UdtHash, UdtEq> searcher(needle.begin(), needle.end());
    const auto answer = searcher(begin(haystack), end(haystack));
    assert_equal(answer, haystack, 3, 2, UdtEq{});
}

template <template <class RanIt, class Hash, class Pred_eq> class Searcher>
void test_case_BM_u8() {
#ifdef __cpp_lib_char8_t
    using BM = Searcher<u8string_view::const_iterator, hash<char8_t>, equal_to<>>;

    // U+1F3C8 AMERICAN FOOTBALL
    const u8string_view fastPattern{u8"ASCII only pattern"};
    const u8string_view slowPattern{u8"Major Game \U0001F3C8 Sunday"};

    const u8string_view fastHaystack{u8"When searching for an ASCII only pattern, we want to use a plain "
                                     u8"256 array lookup"};
    const u8string_view slowHaystack{
        u8"Major Game \U0001F3C8! If the pattern is emoji-enhanced "
        u8"\U0001F3C8, we fall back to a hash table for characters greater than 255, to make sure "
        u8"that we are ready for Major Game \U0001F3C8 Sunday!"};

    const BM fastPatBM(fastPattern.begin(), fastPattern.end());
    const BM slowPatBM(slowPattern.begin(), slowPattern.end());

    const auto fastFast = fastPatBM(fastHaystack.begin(), fastHaystack.end());
    const auto fastSlow = fastPatBM(slowHaystack.begin(), slowHaystack.end());

    const auto slowFast = slowPatBM(fastHaystack.begin(), fastHaystack.end());
    const auto slowSlow = slowPatBM(slowHaystack.begin(), slowHaystack.end());

    assert_equal(fastFast, fastHaystack.begin(), 22, static_cast<ptrdiff_t>(fastPattern.size()));
    assert(fastSlow.first == slowHaystack.end());
    assert(fastSlow.second == slowHaystack.end());
    assert(slowFast.first == fastHaystack.end());
    assert(slowFast.second == fastHaystack.end());
    assert_equal(slowSlow, slowHaystack.begin(), 153, static_cast<ptrdiff_t>(slowPattern.size()));
#endif // __cpp_lib_char8_t
}

template <template <class RanIt, class Hash, class Pred_eq> class Searcher>
void test_case_BM_unicode() {
    using BM = Searcher<u16string_view::const_iterator, hash<char16_t>, equal_to<>>;

    // U+1F3C8 AMERICAN FOOTBALL
    const u16string_view fastPattern{u"ASCII only pattern"};
    const u16string_view slowPattern{u"Major Game \U0001F3C8 Sunday"};

    const u16string_view fastHaystack{u"When searching for an ASCII only pattern, we want to use a plain "
                                      u"256 array lookup"};
    const u16string_view slowHaystack{
        u"Major Game \U0001F3C8! If the pattern is emoji-enhanced "
        u"\U0001F3C8, we fall back to a hash table for characters greater than 255, to make sure "
        u"that we are ready for Major Game \U0001F3C8 Sunday!"};

    const BM fastPatBM(fastPattern.begin(), fastPattern.end());
    const BM slowPatBM(slowPattern.begin(), slowPattern.end());

    const auto fastFast = fastPatBM(fastHaystack.begin(), fastHaystack.end());
    const auto fastSlow = fastPatBM(slowHaystack.begin(), slowHaystack.end());

    const auto slowFast = slowPatBM(fastHaystack.begin(), fastHaystack.end());
    const auto slowSlow = slowPatBM(slowHaystack.begin(), slowHaystack.end());

    assert_equal(fastFast, fastHaystack.begin(), 22, static_cast<ptrdiff_t>(fastPattern.size()));
    assert(fastSlow.first == slowHaystack.end());
    assert(fastSlow.second == slowHaystack.end());
    assert(slowFast.first == fastHaystack.end());
    assert(slowFast.second == fastHaystack.end());
    assert_equal(slowSlow, slowHaystack.begin(), 149, static_cast<ptrdiff_t>(slowPattern.size()));
}

template <template <class RanIt, class Hash, class Pred_eq> class Searcher>
void test_case_BM_unicode32() {
    using BM = Searcher<u32string_view::const_iterator, hash<char32_t>, equal_to<>>;

    // U+1F3C8 AMERICAN FOOTBALL
    const u32string_view fastPattern{U"ASCII only pattern"};
    const u32string_view slowPattern{U"Major Game \U0001F3C8 Sunday"};

    const u32string_view fastHaystack{U"When searching for an ASCII only pattern, we want to use a plain "
                                      U"256 array lookup"};
    const u32string_view slowHaystack{
        U"Major Game \U0001F3C8! If the pattern is emoji-enhanced "
        U"\U0001F3C8, we fall back to a hash table for characters greater than 255, to make sure "
        U"that we are ready for Major Game \U0001F3C8 Sunday!"};

    const BM fastPatBM(fastPattern.begin(), fastPattern.end());
    const BM slowPatBM(slowPattern.begin(), slowPattern.end());

    const auto fastFast = fastPatBM(fastHaystack.begin(), fastHaystack.end());
    const auto fastSlow = fastPatBM(slowHaystack.begin(), slowHaystack.end());

    const auto slowFast = slowPatBM(fastHaystack.begin(), fastHaystack.end());
    const auto slowSlow = slowPatBM(slowHaystack.begin(), slowHaystack.end());

    assert_equal(fastFast, fastHaystack.begin(), 22, static_cast<ptrdiff_t>(fastPattern.size()));
    assert(fastSlow.first == slowHaystack.end());
    assert(fastSlow.second == slowHaystack.end());
    assert(slowFast.first == fastHaystack.end());
    assert(slowFast.second == fastHaystack.end());
    assert_equal(slowSlow, slowHaystack.begin(), 147, static_cast<ptrdiff_t>(slowPattern.size()));
}

int main() {
    test_boyer_moore_table2_construction();

    test_case_searcher<default_searcher<string_view::iterator>>();

    test_case_boyer_moore_functors(hash<char>{}, equal_to<>{});
    test_case_boyer_moore_functors(FancyHash{42}, equal_to<>{});
    test_case_boyer_moore_functors(hash<char>{}, equal_to<char>{});
    test_case_boyer_moore_functors(hash<char>{}, FancyEqual{42});
    test_case_boyer_moore_functors(CaseInsensitiveHashEqual{}, CaseInsensitiveHashEqual{});
    test_case_case_insensitive<boyer_moore_searcher>();
    test_case_case_insensitive<boyer_moore_horspool_searcher>();

    test_case_default_searcher_special_cases();
    test_case_BM_udts<boyer_moore_searcher>();
    test_case_BM_udts<boyer_moore_horspool_searcher>();

    test_case_BM_u8<boyer_moore_searcher>();
    test_case_BM_u8<boyer_moore_horspool_searcher>();

    test_case_BM_unicode<boyer_moore_searcher>();
    test_case_BM_unicode<boyer_moore_horspool_searcher>();

    test_case_BM_unicode32<boyer_moore_searcher>();
    test_case_BM_unicode32<boyer_moore_horspool_searcher>();
}
