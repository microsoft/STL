// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// Note that the standard actually requires these to be copyable. As an extension, we want to ensure we don't copy them,
// because copying some functors (e.g. std::function) is comparatively expensive, and even for relatively cheap to copy
// function objects we care (somewhat) about debug mode perf.
struct no_copy {
    no_copy()                          = default;
    no_copy(const no_copy&)            = delete;
    no_copy(no_copy&&)                 = default;
    no_copy& operator=(const no_copy&) = delete;
    no_copy& operator=(no_copy&&)      = delete;
};

struct is_vowel : no_copy {
    constexpr bool operator()(const char c) const {
        return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
    }
};

struct is_odd : no_copy {
    constexpr bool operator()(const int i) const {
        return i % 2 != 0;
    }
};

struct is_first_odd : no_copy {
    bool operator()(const std::pair<const int, int>& p) const {
        return p.first % 2 != 0;
    }
};

constexpr bool test_string() {
    std::string str1{"cute fluffy kittens"};
    const auto str1_removed = std::erase_if(str1, is_vowel{});
    assert(str1 == "ct flffy kttns");
    assert(str1_removed == 5);

    std::string str2{"asynchronous beat"};
    const auto str2_removed = std::erase(str2, 'a');
    assert(str2 == "synchronous bet");
    assert(str2_removed == 2);

    return true;
}

template <class SequenceContainer>
constexpr bool test_sequence_container() {
    SequenceContainer c{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9, 3, 2, 3, 8, 4, 6, 2, 6, 4, 3, 3, 8, 3, 2, 7, 9, 5,
        0, 2, 8, 8, 4, 1, 9, 7, 1, 6, 9, 3, 9, 9, 3, 7, 5, 1, 0};

    {
        const auto removed1 = std::erase_if(c, is_odd{});
        assert(removed1 == 31);
        const SequenceContainer expected1{4, 2, 6, 8, 2, 8, 4, 6, 2, 6, 4, 8, 2, 0, 2, 8, 8, 4, 6, 0};
        assert(c == expected1);
    }

    {
        const auto removed2 = std::erase(c, 8);
        assert(removed2 == 5);
        const SequenceContainer expected2{4, 2, 6, 2, 4, 6, 2, 6, 4, 2, 0, 2, 4, 6, 0};
        assert(c == expected2);
    }

    return true;
}

// Also test LWG-4135 "The helper lambda of std::erase for list should specify return type as bool"

template <bool B>
struct pinned_condition {
    explicit pinned_condition()                          = default;
    pinned_condition(const pinned_condition&)            = delete;
    pinned_condition& operator=(const pinned_condition&) = delete;

    operator bool() const {
        return B;
    }

    pinned_condition<!B> operator!() const {
        return {};
    }
};

struct lwg_4135_src {
    static constexpr pinned_condition<true> result{};

    friend void operator==(int&, const lwg_4135_src&) = delete;
    friend void operator==(const lwg_4135_src&, int&) = delete;

    friend const pinned_condition<true>& operator==(const lwg_4135_src&, const int&) {
        return result;
    }
    friend const pinned_condition<true>& operator==(const int&, const lwg_4135_src&) {
        return result;
    }
};

template <class ListContainer>
void test_list_erase() {
    ListContainer ls2{42, 1729};
    const auto ls2_removed = std::erase(ls2, lwg_4135_src{});
    assert(ls2.empty());
    assert(ls2_removed == 2);
}

static_assert(test_string());
static_assert(test_sequence_container<std::vector<int>>());

int main() {
    test_string();
    test_sequence_container<std::deque<int>>();
    test_sequence_container<std::forward_list<int>>();
    test_sequence_container<std::list<int>>();
    test_sequence_container<std::vector<int>>();

    test_list_erase<std::forward_list<int>>();
    test_list_erase<std::list<int>>();

    std::map<int, int> m{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}};
    const auto m_removed = std::erase_if(m, is_first_odd{});
    assert((m == std::map<int, int>{{2, 20}, {4, 40}, {6, 60}}));
    assert(m_removed == 4);

    std::multimap<int, int> mm{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}};
    const auto mm_removed = std::erase_if(mm, is_first_odd{});
    assert((mm == std::multimap<int, int>{{2, 20}, {4, 40}, {6, 60}}));
    assert(mm_removed == 4);

    std::set<int> s{1, 2, 3, 4, 5, 6, 7};
    const auto s_removed = std::erase_if(s, is_odd{});
    assert((s == std::set<int>{2, 4, 6}));
    assert(s_removed == 4);

    std::multiset<int> ms{1, 2, 3, 4, 5, 6, 7};
    const auto ms_removed = std::erase_if(ms, is_odd{});
    assert((ms == std::multiset<int>{2, 4, 6}));
    assert(ms_removed == 4);

    // Note that unordered equality considers permutations.

    std::unordered_map<int, int> um{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}};
    const auto um_removed = std::erase_if(um, is_first_odd{});
    assert((um == std::unordered_map<int, int>{{2, 20}, {4, 40}, {6, 60}}));
    assert(um_removed == 4);

    std::unordered_multimap<int, int> umm{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}};
    const auto umm_removed = std::erase_if(umm, is_first_odd{});
    assert((umm == std::unordered_multimap<int, int>{{2, 20}, {4, 40}, {6, 60}}));
    assert(umm_removed == 4);

    std::unordered_set<int> us{1, 2, 3, 4, 5, 6, 7};
    const auto us_removed = std::erase_if(us, is_odd{});
    assert((us == std::unordered_set<int>{2, 4, 6}));
    assert(us_removed == 4);

    std::unordered_multiset<int> ums{1, 2, 3, 4, 5, 6, 7};
    const auto ums_removed = std::erase_if(ums, is_odd{});
    assert((ums == std::unordered_multiset<int>{2, 4, 6}));
    assert(ums_removed == 4);
}
