// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <flat_map>
#include <type_traits>

using namespace std;

template <typename T>
bool check_container_requirements(T&&)
    requires _Is_specialization_v<remove_cvref_t<T&&>, flat_map>
{
    return true;
}

template <typename T>
consteval bool check_reversible_container_requirements()
    requires _Is_specialization_v<remove_cvref_t<T>, flat_map>
{
    using map_t = remove_cvref_t<T>;
    bool result = true;
    result &= is_same_v<std::reverse_iterator<typename map_t::iterator>, typename map_t::reverse_iterator>;
    result &= is_same_v<std::reverse_iterator<typename map_t::const_iterator>, typename map_t::const_reverse_iterator>;
    result &= is_same_v<decltype(declval<map_t>().begin()), typename map_t::iterator>;
    result &= is_same_v<decltype(declval<map_t>().end()), typename map_t::iterator>;
    result &= is_same_v<decltype(declval<map_t>().cbegin()), typename map_t::const_iterator>;
    result &= is_same_v<decltype(declval<map_t>().cend()), typename map_t::const_iterator>;
    result &= is_same_v<decltype(declval<map_t>().rbegin()), typename map_t::reverse_iterator>;
    result &= is_same_v<decltype(declval<map_t>().rend()), typename map_t::reverse_iterator>;
    result &= is_same_v<decltype(declval<map_t>().crbegin()), typename map_t::const_reverse_iterator>;
    result &= is_same_v<decltype(declval<map_t>().crend()), typename map_t::const_reverse_iterator>;
    result &= is_convertible_v<typename map_t::iterator, typename map_t::const_iterator>;
    result &= is_convertible_v<typename map_t::reverse_iterator, typename map_t::const_reverse_iterator>;
    return result;
}

template <typename T>
constexpr bool check_reversible_container_requirements(T&&)
    requires _Is_specialization_v<remove_cvref_t<T&&>, flat_map>
{
    return check_reversible_container_requirements<T&&>();
}

template <typename T>
bool check_requirements(T&& obj) {
    return check_container_requirements(forward<T>(obj)) && check_reversible_container_requirements(forward<T>(obj));
}

void test_construction() {
    {
        std::flat_map<int, int> map;
        check_requirements(map);
    }
    {
        std::vector<int> keys = {0, 1, 2, 3, 4, 2};
        std::vector<int> vals = {44, 2324, 635462, 433, 5, 7};
        std::flat_map map(keys, vals);
        check_requirements(map);
    }
}

int main() {
    flat_map<int, int> m;
    assert(check_reversible_container_requirements<>(m));
    return 0;
}
