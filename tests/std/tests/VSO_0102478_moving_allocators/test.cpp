// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _USE_HETEROGENEOUS_ALLOCATOR_COMPARE_IN_INTERNAL_CHECK
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <forward_list>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <regex>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

void print_report_header() {
    printf("%-30s| %-23s| %-8s| %-9s| %-10s| %-11s| %-10s\n", "Type", "Test", "# Moves", "# Copies", "Result",
        "# Expctd M", "# Expctd C");
}

void print_report_entry(const string& type, const string& test, int num_moves, int num_copies, bool result,
    int num_expected_moves, int num_expected_copies) {
    printf("%-30s  %-23s  %-8d  %-9d  %-10s  %-11d  %-10d\n", type.data(), test.data(), num_moves, num_copies,
        (result ? "Pass" : "FAILED"), num_expected_moves, num_expected_copies);
}

int& get_alloc_moved_count() {
    static int s_alloc_moved_count = 0;
    return s_alloc_moved_count;
}

void reset_alloc_moved() {
    get_alloc_moved_count() = 0;
}

void trigger_alloc_moved() {
    ++get_alloc_moved_count();
}

int& get_alloc_copied_count() {
    static int s_alloc_copied_count = 0;
    return s_alloc_copied_count;
}

void reset_alloc_copied() {
    get_alloc_copied_count() = 0;
}

void trigger_alloc_copied() {
    ++get_alloc_copied_count();
}

struct test_info {
    const char* type_name;
    const char* test_name;
    int count;
};

#if _ITERATOR_DEBUG_LEVEL == 0
constexpr array<test_info, 5> expected_moves_table{{
    {"*", "*", 1},
    {"unordered_set", "*", 2}, // unordered_* moves two allocators, one for list, and one for vector of list iterators.
    {"unordered_multiset", "*", 2},
    {"unordered_map", "*", 2},
    {"unordered_multimap", "*", 2},
}};

constexpr array<test_info, 2> expected_copies_table{{
    {"*", "*", 0}, //
    {"deque", "*", 1}, // deque always causes an additional copy due to creating the proxy allocator
}};
#else // _ITERATOR_DEBUG_LEVEL == 0
constexpr array<test_info, 5> expected_moves_table{{
    {"*", "*", 1},
    {"unordered_set", "*", 2}, // unordered_* have two allocators,
                               // so copy two proxy allocators and have two move operations
    {"unordered_multiset", "*", 2},
    {"unordered_map", "*", 2},
    {"unordered_multimap", "*", 2},
}};

constexpr array<test_info, 6> expected_copies_table{{
    {"*", "*", 1}, // IDL causes an additional copy due to a proxy allocator being created.
    {"vector", "bool", 2}, // with IDL>0, _Vb_val also allocates a proxy,
                           // so the extra copy is copying the allocator for that purpose
    {"unordered_set", "*", 2}, // unordered_* have two allocators,
                               // so copy two proxy allocators and have two move operations
    {"unordered_multiset", "*", 2},
    {"unordered_map", "*", 2},
    {"unordered_multimap", "*", 2},
}};
#endif // _ITERATOR_DEBUG_LEVEL == 0

template <typename ExpectedTable>
int query_expected_table(const ExpectedTable& table, const string& type_name, const string& test_name) {
    const string star{"*"};

    auto exact_match = find_if(begin(table), end(table),
        [&](const test_info& other) { return type_name == other.type_name && test_name == other.test_name; });

    if (exact_match != end(table)) {
        return exact_match->count;
    }

    auto fuzzy_match = find_if(begin(table), end(table),
        [&](const test_info& other) { return type_name == other.type_name && star == other.test_name; });

    if (fuzzy_match != end(table)) {
        return fuzzy_match->count;
    }

    auto fuzziest_match = find_if(begin(table), end(table),
        [&](const test_info& other) { return star == other.type_name && star == other.test_name; });

    if (fuzziest_match != end(table)) {
        return fuzziest_match->count;
    }

    throw runtime_error("Missing entry for " + type_name + ", " + test_name);
}

int get_expected_copies(const string& type_name, const string& test_name) {
    return query_expected_table(expected_copies_table, type_name, test_name);
}

int get_expected_moves(const string& type_name, const string& test_name) {
    return query_expected_table(expected_moves_table, type_name, test_name);
}

template <typename T>
void move_test(T val, const string& type_name, const string& test_name) {
    reset_alloc_moved();
    reset_alloc_copied();

    auto x = move(val);

    int num_moves  = get_alloc_moved_count();
    int num_copies = get_alloc_copied_count();

    int num_expected_moves  = get_expected_moves(type_name, test_name);
    int num_expected_copies = get_expected_copies(type_name, test_name);

    bool passed = (num_moves == num_expected_moves) && (num_copies == num_expected_copies);

    print_report_entry(type_name, test_name, num_moves, num_copies, passed, num_expected_moves, num_expected_copies);

    // no copying of the allocator should occur, only moves.
    assert(passed);
}

template <typename T>
class Alloc {
public:
    typedef T value_type;

    Alloc() = default;

    Alloc(const Alloc&) noexcept {
        trigger_alloc_copied();
    }

    Alloc(Alloc&&) noexcept {
        trigger_alloc_moved();
    }

    template <typename Other>
    Alloc(const Alloc<Other>&) noexcept {
        trigger_alloc_copied();
    }

    template <typename Other>
    Alloc(Alloc<Other>&&) noexcept {
        trigger_alloc_moved();
    }

    template <typename U>
    bool operator==(const Alloc<U>&) const noexcept {
        return true;
    }

    template <typename U>
    bool operator!=(const Alloc<U>&) const noexcept {
        return false;
    }

    T* allocate(size_t sz) const {
        allocator<T> alloc;
        return alloc.allocate(sz);
    }

    void deallocate(T* const p, size_t sz) const noexcept {
        allocator<T> alloc;
        alloc.deallocate(p, sz);
    }
};

template <typename... Ts>
const char* template_name(const vector<Ts...>&) {
    return "vector";
}

template <typename... Ts>
const char* template_name(const list<Ts...>&) {
    return "list";
}

template <typename... Ts>
const char* template_name(const deque<Ts...>&) {
    return "deque";
}

template <typename... Ts>
const char* template_name(const forward_list<Ts...>&) {
    return "forward_list";
}

template <typename... Ts>
const char* template_name(const set<Ts...>&) {
    return "set";
}

template <typename... Ts>
const char* template_name(const multiset<Ts...>&) {
    return "multiset";
}

template <typename... Ts>
const char* template_name(const unordered_set<Ts...>&) {
    return "unordered_set";
}

template <typename... Ts>
const char* template_name(const unordered_multiset<Ts...>&) {
    return "unordered_multiset";
}

template <typename... Ts>
const char* template_name(const map<Ts...>&) {
    return "map";
}

template <typename... Ts>
const char* template_name(const multimap<Ts...>&) {
    return "multimap";
}

template <typename... Ts>
const char* template_name(const unordered_map<Ts...>&) {
    return "unordered_map";
}

template <typename... Ts>
const char* template_name(const unordered_multimap<Ts...>&) {
    return "unordered_multimap";
}

template <typename T, template <typename...> class Container>
struct bind_alloc_container {};

template <typename T>
struct bind_alloc_container<T, vector> {
    using type = vector<T, Alloc<T>>;
};

template <typename T>
struct bind_alloc_container<T, list> {
    using type = list<T, Alloc<T>>;
};

template <typename T>
struct bind_alloc_container<T, deque> {
    using type = deque<T, Alloc<T>>;
};

template <typename T>
struct bind_alloc_container<T, forward_list> {
    using type = forward_list<T, Alloc<T>>;
};

template <typename T>
struct bind_alloc_container<T, set> {
    using type = set<T, less<T>, Alloc<T>>;
};

template <typename T>
struct bind_alloc_container<T, multiset> {
    using type = multiset<T, less<T>, Alloc<T>>;
};

template <typename T>
struct bind_alloc_container<T, unordered_set> {
    using type = unordered_set<T, hash<T>, equal_to<T>, Alloc<T>>;
};

template <typename T>
struct bind_alloc_container<T, unordered_multiset> {
    using type = unordered_multiset<T, hash<T>, equal_to<T>, Alloc<T>>;
};

template <typename Key, typename Value, template <typename...> class Dictionary>
struct bind_alloc_dictionary {};

template <typename Key, typename Value>
struct bind_alloc_dictionary<Key, Value, map> {
    using type = map<Key, Value, less<Key>, Alloc<pair<const Key, Value>>>;
};

template <typename Key, typename Value>
struct bind_alloc_dictionary<Key, Value, multimap> {
    using type = multimap<Key, Value, less<Key>, Alloc<pair<const Key, Value>>>;
};

template <typename Key, typename Value>
struct bind_alloc_dictionary<Key, Value, unordered_map> {
    using type = unordered_map<Key, Value, hash<Key>, equal_to<Key>, Alloc<pair<const Key, Value>>>;
};

template <typename Key, typename Value>
struct bind_alloc_dictionary<Key, Value, unordered_multimap> {
    using type = unordered_multimap<Key, Value, hash<Key>, equal_to<Key>, Alloc<pair<const Key, Value>>>;
};

template <typename T, template <typename...> class Container>
using container_t = typename bind_alloc_container<T, Container>::type;

template <typename Key, typename Value, template <typename...> class Dictionary>
using dictionary_t = typename bind_alloc_dictionary<Key, Value, Dictionary>::type;

template <template <typename...> class Container>
void container_test() {
    container_t<int, Container> empty_container;
    string type_name = template_name(empty_container);

    container_t<int, Container> int_container{1, 2, 3};
    container_t<double, Container> double_container{2.5, 5.2, 11.9};
    container_t<bool, Container> bool_container{true, false, true, false};

    move_test(empty_container, type_name, "empty");
    move_test(int_container, type_name, "int");
    move_test(double_container, type_name, "double");
    move_test(bool_container, type_name, "bool");
}

template <template <typename...> class Dictionary>
void dictionary_test() {
    dictionary_t<int, int, Dictionary> empty_dictionary;
    string type_name = template_name(empty_dictionary);

    dictionary_t<int, char, Dictionary> number_letter_dictionary{{0, 'A'}, {1, 'B'}, {2, 'C'}};
    dictionary_t<string, bool, Dictionary> word_check_dictionary{{"notaword", false}, {"test", true}};
    dictionary_t<double, double, Dictionary> double_dictionary{{1.23, 23.4}, {3.14, 15.9}};

    move_test(empty_dictionary, type_name, "empty");
    move_test(number_letter_dictionary, type_name, "int -> char");
    move_test(word_check_dictionary, type_name, "string -> bool");
    move_test(double_dictionary, type_name, "double -> double");
}

void string_test() {
    using str_type   = basic_string<char, char_traits<char>, Alloc<char>>;
    string type_name = "string";

    str_type empty_string;
    str_type copied_from_raw_string = "Test123";
    str_type copy_constructed{copied_from_raw_string};

    move_test(empty_string, type_name, "empty");
    move_test(copied_from_raw_string, type_name, "copied from raw");
    move_test(copy_constructed, type_name, "copy constructed");
}

void match_results_char_test() {
    match_results<const char*, Alloc<csub_match>> mrc;
    string type_name = "match_results";

    move_test(mrc, type_name, "empty");

    basic_regex<char> rgc("(a*)(b+)");
    regex_match("xbbx", mrc, rgc);

    move_test(mrc, type_name, "matched");
}

// This test also checks to ensure that unordered containers that are moved from are still in a valid state.
void test_moved_unordered_set_valid() {
    array<int, 10> nums;
    iota(begin(nums), end(nums), 0);

    function<size_t(int)> hasher([](int val) { return hash<int>()(val); });

    function<bool(int, int)> key_equal([](int lhs, int rhs) { return equal_to<int>()(lhs, rhs); });

    using my_set = unordered_set<int, function<size_t(int)>, function<bool(int, int)>>;

    my_set my_data(500, hasher, key_equal);
    my_data.insert(begin(nums), end(nums));
    my_set my_copy(my_data);
    my_set new_my_data(move(my_data));

    assert(my_copy.size() == nums.size());
    assert(new_my_data.size() == nums.size());

    // my_data is unspecified but valid - the hash function and key equality should still exist and work.
    assert(my_copy.hash_function()(5) == my_data.hash_function()(5));
    assert(my_copy.key_eq()(5, 5) == my_data.key_eq()(5, 5));

    my_data.clear();
    assert(my_data.empty());
    assert(my_copy.bucket_count() > my_data.bucket_count());

    my_data.insert(begin(nums), end(nums));
    assert(my_data.size() == nums.size());

    my_data.erase(5);
    assert(my_data.size() == nums.size() - 1);
}

int main() {
    print_report_header();

    container_test<vector>();
    container_test<list>();
    container_test<deque>();
    container_test<forward_list>();
    container_test<set>();
    container_test<multiset>();
    container_test<unordered_set>();
    container_test<unordered_multiset>();

    dictionary_test<map>();
    dictionary_test<multimap>();
    dictionary_test<unordered_map>();
    dictionary_test<unordered_multimap>();

    string_test();

    match_results_char_test();

    test_moved_unordered_set_valid();
}
