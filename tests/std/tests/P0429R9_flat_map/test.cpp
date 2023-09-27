// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <flat_map>
#include <type_traits>

using namespace std;

template <typename T>
consteval bool check_reversible_container_requirements(T&& obj)
    requires _Is_specialization_v<remove_cvref_t<T&&>, flat_map>
{
    using map_t = remove_cvref_t<T&&>;
    bool result = true;
    result &= is_same_v<std::reverse_iterator<typename map_t::iterator>, typename map_t::reverse_iterator>;
    result &= is_same_v<std::reverse_iterator<typename map_t::const_iterator>, typename map_t::const_reverse_iterator>;
    result &= is_same_v<decltype(obj.begin()), typename map_t::iterator>;
    result &= is_same_v<decltype(obj.end()), typename map_t::iterator>;
    result &= is_same_v<decltype(obj.cbegin()), typename map_t::const_iterator>;
    result &= is_same_v<decltype(obj.cend()), typename map_t::const_iterator>;
    result &= is_same_v<decltype(obj.rbegin()), typename map_t::reverse_iterator>;
    result &= is_same_v<decltype(obj.rend()), typename map_t::reverse_iterator>;
    result &= is_same_v<decltype(obj.crbegin()), typename map_t::const_reverse_iterator>;
    result &= is_same_v<decltype(obj.crend()), typename map_t::const_reverse_iterator>;
    result &= is_convertible_v<typename map_t::iterator, typename map_t::const_iterator>;
    result &= is_convertible_v<typename map_t::reverse_iterator, typename map_t::const_reverse_iterator>;
    return result;
}

int main() {
    flat_map<int, int> m;
    assert(check_reversible_container_requirements(m));
    return 0;
}
