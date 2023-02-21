// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _USE_NAMED_IDL_NAMESPACE 1
#define _SILENCE_EXPERIMENTAL_ERASE_DEPRECATION_WARNING
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#include <array>
#include <bitset>
#include <complex>
#include <cstdio>
#include <deque>
#include <experimental/deque>
#include <experimental/forward_list>
#include <experimental/list>
#include <experimental/map>
#include <experimental/set>
#include <experimental/string>
#include <experimental/unordered_map>
#include <experimental/unordered_set>
#include <experimental/vector>
#include <forward_list>
#include <hash_map>
#include <hash_set>
#include <initializer_list>
#include <iterator>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <valarray>
#include <vector>

#include <instantiate_containers_iterators_common.hpp>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <typename Tpl, typename Fx, size_t... Indices>
void for_each_tuple_element_impl(Tpl&& Tuple, Fx Func, index_sequence<Indices...>) {
    // call Func() on the Indices elements of Tuple
    int ignored[] = {(static_cast<void>(Func(get<Indices>(forward<Tpl>(Tuple)))), 0)...};
    (void) ignored;
}

template <typename Tpl, typename Fx>
void for_each_tuple_element(Tpl&& Tuple, Fx Func) { // call Func() on each element in Tuple
    for_each_tuple_element_impl(
        forward<Tpl>(Tuple), Func, make_index_sequence<tuple_size_v<remove_reference_t<Tpl>>>{});
}

// Use this type to ensure function templates are used instead
// of regular member functions.
template <typename T>
struct convertible_type {
    operator T() const {
        return m_val;
    }
    T m_val;
};

template <typename T, typename U>
void math_operators_test(T lhs, U rhs) {
    USE_VALUE(lhs * rhs);
    USE_VALUE(lhs / rhs);
    USE_VALUE(lhs - rhs);
    USE_VALUE(lhs + rhs);
}

template <typename T, typename U>
void extended_math_operators_test(T lhs, U rhs) {
    USE_VALUE(lhs % rhs);
}

template <typename T, typename U>
void equals_math_operators_test(T lhs, U rhs) {
    USE_VALUE(lhs *= rhs);
    USE_VALUE(lhs /= rhs);
    USE_VALUE(lhs -= rhs);
    USE_VALUE(lhs += rhs);
}

template <typename T, typename U>
void bitwise_and_logical_operators_test(T lhs, U rhs) {
    USE_VALUE(lhs ^ rhs);
    USE_VALUE(lhs & rhs);
    USE_VALUE(lhs | rhs);
    USE_VALUE(lhs << rhs);
    USE_VALUE(lhs >> rhs);
    USE_VALUE(lhs && rhs);
    USE_VALUE(lhs || rhs);
}

template <typename T, typename U>
void logical_operators_test(T lhs, U rhs) {
    USE_VALUE(lhs && rhs);
    USE_VALUE(lhs || rhs);
}

template <typename T>
void typical_forward_only_iterator_test(T value) {
    (void) begin(value);
    (void) cbegin(value);
}

template <typename T>
void typical_iterator_test(T value) {
    typical_forward_only_iterator_test(value);
    (void) rbegin(value);
    (void) crbegin(value);
}

template <typename T>
void typical_random_access_iterator_test(T value) {
    typical_iterator_test(value);

    random_access_iterator_test(begin(value));
    random_access_iterator_test(rbegin(value));
    random_access_iterator_test(cbegin(value));
    random_access_iterator_test(crbegin(value));
}

template <typename T>
void construct_from_iterators_test(T value) {
    auto containers = get_all_iterator_types_for(value);

    for_each_tuple_element(containers, [](auto c) {
        T another(begin(c), end(c));
        T another2(begin(c), end(c));
    });
}

template <typename T>
void construct_tree_containers_from_iterators_test(T value) {
    auto containers = get_all_iterator_types_for(value);

    for_each_tuple_element(containers, [&](auto c) {
        T another(begin(c), end(c));
        T another2(begin(c), end(c), value.key_comp());
        T another3(begin(c), end(c), value.key_comp(), value.get_allocator());
    });
}

template <typename T>
void construct_hash_containers_from_iterators_test(T value) {
    auto containers = get_all_iterator_types_for(value);

    for_each_tuple_element(containers, [&](auto c) {
        T another(begin(c), end(c));
        T another2(begin(c), end(c), value.bucket_count());
        T another3(begin(c), end(c), value.bucket_count(), value.hash_function());
        T another4(begin(c), end(c), value.bucket_count(), value.hash_function(), value.key_eq());
        T another5(
            begin(c), end(c), value.bucket_count(), value.hash_function(), value.key_eq(), value.get_allocator());
    });
}

template <typename T>
void emplace_test(T value) {
    convertible_type<typename T::value_type> element{};
    value.emplace(element);
}

template <typename T>
void emplace_front_test(T value) {
    convertible_type<typename T::value_type> element{};
    value.emplace_front(element);
}

template <typename T>
void emplace_middle_back_test(T value) {
    convertible_type<typename T::value_type> element{};
    value.emplace(cbegin(value), element);
    value.emplace_back(element);
}

template <typename T>
void try_emplace_test(T value) {
    convertible_type<typename T::key_type> key_element{};
    convertible_type<typename T::mapped_type> mapped_element{};
    value.try_emplace(key_element, mapped_element);
    value.try_emplace(cbegin(value), key_element, mapped_element);
    value.try_emplace(move(key_element), mapped_element);
    value.try_emplace(cbegin(value), move(key_element), mapped_element);
}

template <typename T>
void insert_or_assign_test(T value) {
    convertible_type<typename T::key_type> key_element{};
    convertible_type<typename T::mapped_type> mapped_element{};
    value.insert_or_assign(key_element, mapped_element);
    value.insert_or_assign(cbegin(value), key_element, mapped_element);
    value.insert_or_assign(move(key_element), mapped_element);
    value.insert_or_assign(cbegin(value), move(key_element), mapped_element);
}

template <typename T>
void assign_test(T value) {
    T another{};
    another.assign(begin(value), end(value));
}

template <typename T>
void insert_with_iterator_test(T value) {
    T another{};
    another.insert(begin(value), end(value));
}

template <typename T>
void erase_if_test(T value) {
    auto pr1 = [](auto) { return false; };
    std::experimental::fundamentals_v2::erase_if(value, pr1);
#if _HAS_CXX20
    std::erase_if(value, pr1);
#endif // _HAS_CXX20
}

template <typename T>
void erase_test(T value) {
    std::experimental::fundamentals_v2::erase(value, static_cast<typename T::value_type>(1));
#if _HAS_CXX20
    std::erase(value, static_cast<typename T::value_type>(1));
#endif // _HAS_CXX20
}

template <typename T>
void insert_with_iterator_specific_location_test(T value) {
    T another{};
    another.insert(cbegin(another), begin(value), end(value));
}

template <typename T>
void insert_associative_direct_test(T value) {
    convertible_type<typename T::value_type> value_element{};
    value.insert(value_element);
    value.insert(cbegin(value), value_element);
}

template <typename T>
void list_operation_test(T value) {
    auto pr1 = [](int) { return false; };
    auto pr2 = [](int, int) { return false; };

    T another{};
    value.remove_if(pr1);
    value.unique(pr2);
    value.merge(another, pr2);
    value.merge(move(another), pr2);
    value.sort(pr2);
}

template <typename T>
void tree_baseclass_test(T value) {
    // Tests for anything inheriting from _Tree in <xtree>
    convertible_type<typename T::value_type> value_element{};
    convertible_type<typename T::key_type> key_element{};
    value.insert(value_element);
    value.insert(cbegin(value), value_element);
    emplace_test(value);
    value.emplace_hint(cbegin(value), value_element);
    (void) value.find(key_element);

    const T const_value{};
    (void) const_value.find(key_element);

    (void) value.count(key_element);
    (void) value.lower_bound(key_element);
    (void) const_value.lower_bound(key_element);
    (void) value.upper_bound(key_element);
    (void) const_value.upper_bound(key_element);
    (void) value.equal_range(key_element);
    (void) const_value.equal_range(key_element);
}

template <typename T>
void hash_baseclass_test(T value) {
    convertible_type<typename T::value_type> value_element{};
    value.insert(value_element);
    value.insert(cbegin(value), value_element);
    emplace_test(value);
    value.emplace_hint(cbegin(value), value_element);
    T another{};
    another.insert(begin(value), end(value));
}

void array_test() {
    array<int, 5> value{{0, 1, 2, 3, 4}};
    typical_random_access_iterator_test(value);
    swap_test(value);
    comparable_test(value);
    (void) get<0>(value);
    const array<int, 5> another{{0, 1, 2, 3, 4}};
    (void) get<0>(another);
    (void) get<0>(move(value));
    (void) get<0>(move(another));

    array<int, 0> another2{};
    typical_random_access_iterator_test(another2);
    swap_test(another2);
    comparable_test(another2);
}

template <typename T>
void bitset_operations_test(T value) {
    T another = value;
    (void) (value & another);
    (void) (value | another);
    (void) (value ^ another);

    istringstream input("0010100101");
    input >> value;

    ostringstream output;
    output << value;

    hash_test(value);
}

void bitset_test() {
    bitset<5> value{};
    bitset<5> another(string("0100010101"));
    bitset<5> another2("0100010101");

    bitset_operations_test(value);
    bitset_operations_test(another);
    bitset_operations_test(another2);
}

void deque_test() {
    deque<int> value{};
    swap_test(value);
    comparable_test(value);
    typical_random_access_iterator_test(value);
    construct_from_iterators_test(value);
    emplace_middle_back_test(value);
    emplace_front_test(value);
    assign_test(value);
    insert_with_iterator_specific_location_test(value);
    erase_if_test(value);
    erase_test(value);
}

void forward_list_test() {
    static_assert(
        is_nothrow_default_constructible_v<forward_list<int>>, "strengthened noexcept on forward_list::forward_list()");
    static_assert(is_nothrow_move_constructible_v<forward_list<int>>,
        "strengthened noexcept on forward_list::forward_list(forward_list&&)");
    static_assert(is_nothrow_move_assignable_v<forward_list<int>>,
        "strengthened noexcept on forward_list& forward_list::operator=(forward_list&&)");

    forward_list<int> value{};
    swap_test(value);
    comparable_test(value);
    construct_from_iterators_test(value);

    typical_forward_only_iterator_test(value);

    assign_test(value);

    forward_list<int> another;
    another.insert_after(cbegin(another), begin(value), end(value));
    another.emplace_front(1);
    another.emplace_after(cbegin(another), 2);

    list_operation_test(value);
    erase_if_test(value);
    erase_test(value);
}

// Note about hash*_test:
// hash* containers have construction semantics like trees instead of hashes
// so construct_*tree*_containers_from_iterators_test is correct.

void hash_map_test() {
    hash_map<int, int> value{};
    hash_baseclass_test(value);
    construct_tree_containers_from_iterators_test(value);
    swap_test(value);
    equality_test(value);
}

void hash_multimap_test() {
    hash_multimap<int, int> value{};
    hash_baseclass_test(value);
    construct_tree_containers_from_iterators_test(value);
    insert_associative_direct_test(value);
    insert_with_iterator_test(value);
    swap_test(value);
    equality_test(value);
}

void hash_set_test() {
    hash_set<int> value{};
    hash_baseclass_test(value);
    construct_tree_containers_from_iterators_test(value);
    insert_with_iterator_test(value);
    swap_test(value);
    equality_test(value);
}

void hash_multiset_test() {
    hash_multiset<int> value{};
    hash_baseclass_test(value);
    construct_tree_containers_from_iterators_test(value);
    insert_with_iterator_test(value);
    swap_test(value);
    equality_test(value);
}

void initializer_list_test() {
    initializer_list<int> value{};
    (void) begin(value);
    (void) end(value);
}

void list_test() {
    list<int> value{};
    swap_test(value);
    comparable_test(value);
    typical_iterator_test(value);
    construct_from_iterators_test(value);
    emplace_middle_back_test(value);
    emplace_front_test(value);
    assign_test(value);
    insert_with_iterator_specific_location_test(value);
    list_operation_test(value);
    erase_if_test(value);
    erase_test(value);
}

void map_test() {
    map<int, int> value{};
    tree_baseclass_test(value);
    swap_test(value);
    comparable_test(value);
    typical_iterator_test(value);
    construct_tree_containers_from_iterators_test(value);
    emplace_test(value);
    try_emplace_test(value);
    insert_or_assign_test(value);
    insert_associative_direct_test(value);
    erase_if_test(value);
}

void multimap_test() {
    multimap<int, int> value{};
    tree_baseclass_test(value);
    swap_test(value);
    comparable_test(value);
    typical_iterator_test(value);
    construct_tree_containers_from_iterators_test(value);
    emplace_test(value);
    insert_with_iterator_test(value);
    insert_associative_direct_test(value);
    erase_if_test(value);
}

void priority_queue_test() {
    priority_queue<int> value{};
    vector<int> cont{};

    priority_queue<int> another(begin(cont), end(cont));
    priority_queue<int> another2(begin(cont), end(cont), less<int>());
    priority_queue<int> another3(begin(cont), end(cont), less<int>(), cont);
    priority_queue<int> another4(cont.get_allocator());
    priority_queue<int> another5(less<int>(), cont.get_allocator());
    priority_queue<int> another6(less<int>(), cont, cont.get_allocator());
    priority_queue<int> another7(another6, cont.get_allocator());
    priority_queue<int> another8(begin(cont), end(cont), less<int>(), move(cont));
    priority_queue<int> another9(less<int>(), move(cont), cont.get_allocator());
    priority_queue<int> another10(move(another9), cont.get_allocator());

    emplace_test(value);
    swap(value, another);
}

template <typename T>
void stack_queue_test() {
    T value{};
    deque<int> cont{};

    T another(cont.get_allocator());
    T another2(value, cont.get_allocator());
    T another3(cont, cont.get_allocator());
    T another4(move(value), cont.get_allocator());
    T another5(move(cont), cont.get_allocator());

    emplace_test(value);
    swap_test(value);
    comparable_test(value);
}

void stack_test() {
    stack_queue_test<stack<int>>();
}

void set_test() {
    set<int> value{};
    tree_baseclass_test(value);
    swap_test(value);
    comparable_test(value);
    typical_iterator_test(value);
    construct_tree_containers_from_iterators_test(value);
    insert_with_iterator_test(value);
    insert_associative_direct_test(value);
    erase_if_test(value);
}

void multiset_test() {
    multiset<int> value{};
    tree_baseclass_test(value);
    swap_test(value);
    comparable_test(value);
    typical_iterator_test(value);
    construct_tree_containers_from_iterators_test(value);
    emplace_test(value);
    insert_with_iterator_test(value);
    insert_associative_direct_test(value);
    erase_if_test(value);
}

void queue_test() {
    stack_queue_test<queue<int>>();
}

template <typename T>
void vector_test_impl() {
    T value{};
    swap_test(value);
    comparable_test(value);
    typical_random_access_iterator_test(value);
    construct_from_iterators_test(value);
    emplace_middle_back_test(value);
    assign_test(value);
    insert_with_iterator_specific_location_test(value);
    erase_if_test(value);
}

void vector_test() {
    vector_test_impl<vector<int>>();
    vector_test_impl<vector<bool>>();

    erase_test(vector<int>());
    vector<bool> vb;
    std::experimental::fundamentals_v2::erase(vb, true);
#if _HAS_CXX20
    std::erase(vb, true);
#endif // _HAS_CXX20

    hash_test(vector<bool>());
}

template <typename CharType>
void string_test_impl(const CharType* data) {
    using T = basic_string<CharType>;
    T value = data;
    swap_test(value);
    comparable_test(value);
    typical_random_access_iterator_test(value);
    construct_from_iterators_test(value);

    T another = data;
    another.append(begin(value), end(value));
    assign_test(value);
    insert_with_iterator_specific_location_test(value);

    value.replace(cbegin(value), cend(value), begin(another), end(another));

    const T const_value        = data;
    T mut_value                = data;
    const CharType* cstr_value = data;
    const CharType char_value  = data[0];
    (void) (const_value + const_value);
    (void) (cstr_value + const_value);
    (void) (char_value + const_value);
    (void) (const_value + cstr_value);
    (void) (const_value + char_value);
    (void) (const_value + move(mut_value));
    (void) (move(mut_value) + const_value);
    (void) (move(mut_value) + move(mut_value));
    (void) (cstr_value + move(mut_value));
    (void) (char_value + move(mut_value));
    (void) (move(mut_value) + cstr_value);
    (void) (move(mut_value) + char_value);

    comparable_test(const_value, cstr_value);
    comparable_test(cstr_value, const_value);
    hash_test(value);

    erase_if_test(value);
    erase_test(value);
}

void string_test() {
    string_test_impl("test1");
    string_test_impl(L"test2");
    string_test_impl(u"test3");
    string_test_impl(U"test4");
}

template <typename CharType>
void stringstream_test_impl(const CharType* data) {
    basic_string<CharType> value;

    // basic_istream
    basic_istringstream<CharType> input(data);
    input >> value;
    getline(input, value);

    // basic_ostream
    basic_ostringstream<CharType> output;
    output << value;
}

void stringstream_test() {
    stringstream_test_impl("test1");
    stringstream_test_impl(L"test2");

    // stringstream_test_impl(u"test3"); would instantiate ctype<char16_t>
    // stringstream_test_impl(U"test4"); would instantiate ctype<char32_t>
}

void unordered_map_test() {
    unordered_map<int, int> value{};
    hash_baseclass_test(value);
    swap_test(value);
    equality_test(value);
    construct_hash_containers_from_iterators_test(value);
    (void) begin(value); // no rbegin
    (void) cbegin(value);
    emplace_test(value);
    try_emplace_test(value);
    insert_or_assign_test(value);
    insert_with_iterator_test(value);
    insert_associative_direct_test(value);
    erase_if_test(value);
}

void unordered_multimap_test() {
    unordered_multimap<int, int> value{};
    hash_baseclass_test(value);
    swap_test(value);
    equality_test(value);
    construct_hash_containers_from_iterators_test(value);
    insert_associative_direct_test(value);
    emplace_test(value);
    insert_with_iterator_test(value);
    erase_if_test(value);
}

void unordered_set_test() {
    unordered_set<int> value{};
    hash_baseclass_test(value);
    swap_test(value);
    equality_test(value);
    construct_hash_containers_from_iterators_test(value);
    (void) begin(value); // no rbegin
    (void) cbegin(value);
    emplace_test(value);
    insert_with_iterator_test(value);
    insert_associative_direct_test(value);
    erase_if_test(value);
}

void unordered_multiset_test() {
    unordered_multiset<int> value{};
    hash_baseclass_test(value);
    swap_test(value);
    equality_test(value);
    construct_hash_containers_from_iterators_test(value);
    insert_associative_direct_test(value);
    emplace_test(value);
    insert_with_iterator_test(value);
    erase_if_test(value);
}

template <typename T>
void extended_math_functions_test(T value) {
    (void) abs(value);
    (void) acos(value);
    (void) asin(value);
    (void) atan(value);
    (void) cos(value);
    (void) cosh(value);
    (void) exp(value);
    (void) log(value);
    (void) log10(value);
    (void) pow(value, value);
    (void) sin(value);
    (void) sinh(value);
    (void) sqrt(value);
    (void) tan(value);
    (void) tanh(value);
}

// TRANSITION, VSO-251998
// cbegin() calls begin() fully qualified, so cannot
// find valarray non-member begin().

namespace std {
    template <typename T>
    inline const T* cbegin(valarray<T>& arr) {
        return begin(arr); // unqualified
    }
} // namespace std

void valarray_test() {
    valarray<int> value{};
    value.resize(20);
    iota(begin(value), end(value), 1);

    swap_test(value);
    typical_forward_only_iterator_test(value);

    math_operators_test(1, value);
    math_operators_test(value, 1);
    math_operators_test(value, value);
    extended_math_operators_test(1, value);
    extended_math_operators_test(value, 1);
    extended_math_operators_test(value, value);
    bitwise_and_logical_operators_test(1, value);
    bitwise_and_logical_operators_test(value, 1);
    bitwise_and_logical_operators_test(value, value);

    comparable_test(value, 1);
    comparable_test(1, value);
    comparable_test(value, value);

    valarray<double> val_dbl{};
    extended_math_functions_test(val_dbl);
    (void) atan2(val_dbl, val_dbl);
    (void) atan2(val_dbl, 1.0);
    (void) atan2(1.0, val_dbl);
    (void) pow(val_dbl, 1.0);
    (void) pow(1.0, val_dbl);

    slice sl{0, 1, 2};
    slice_array<int> sa = value[sl];
    (void) sa;

    gslice gsl{0, {1, 2}, {3, 4}};
    gslice_array<int> gsa = value[gsl];

    mask_array<int> ma = value[value > 2];

    valarray<size_t> idx   = {1, 2, 3, 4, 5};
    indirect_array<int> ia = value[idx];

    // LWG-3074 "Non-member functions for valarray should only deduce from the valarray"
    constexpr short sh = 1;
    math_operators_test(sh, value);
    math_operators_test(value, sh);
    extended_math_operators_test(sh, value);
    extended_math_operators_test(value, sh);
    bitwise_and_logical_operators_test(sh, value);
    bitwise_and_logical_operators_test(value, sh);
    comparable_test(value, sh);
    comparable_test(sh, value);
    (void) atan2(val_dbl, sh);
    (void) atan2(sh, val_dbl);
    (void) pow(val_dbl, sh);
    (void) pow(sh, val_dbl);
}

template <typename T>
void complex_test_impl(T value) {
    istringstream input("1");
    input >> value;

    ostringstream output;
    output << value;

    math_operators_test(value, value);
    equals_math_operators_test(value, value);

    extended_math_functions_test(value);
    (void) arg(value);
    (void) conj(value);
    (void) proj(value);
    (void) norm(value);
    (void) polar(1.0, 2.0);
    STATIC_ASSERT(is_same_v<complex<double>, decltype(polar(1.0, 2.0))>);
    (void) polar<float>(1.0, 2.0);
    STATIC_ASSERT(is_same_v<complex<float>, decltype(polar<float>(1.0, 2.0))>);
    (void) polar(1.0);
    (void) imag(value);
    (void) real(value);
    (void) acosh(value);
    (void) asinh(value);
    (void) atanh(value);
    (void) pow(value, 1);
    (void) pow(1, value);
}

template <typename T, typename U>
void complex_test_complex(T lhs, U rhs) {
    equals_math_operators_test(lhs, rhs);
    equals_math_operators_test(rhs, lhs);
}

template <typename T, typename U>
void complex_test_pow_common_float(T complex_val, U arithmetic_val) {
    (void) pow(complex_val, arithmetic_val);
    (void) pow(arithmetic_val, complex_val);
}

template <typename T, typename U>
void complex_test_arithmetic(T complex_val, U arithmetic_val) {
    math_operators_test(complex_val, arithmetic_val);
    math_operators_test(arithmetic_val, complex_val);
    equals_math_operators_test(complex_val, arithmetic_val);
    (void) pow(complex_val, arithmetic_val);
    (void) pow(arithmetic_val, complex_val);
}

void complex_test() {
    complex<float> cf        = 1if;
    complex<double> cd       = 1i;
    complex<long double> cld = 1il;

    float f        = 2.0f;
    double d       = 2.0;
    long double ld = 2.0;
    short s        = 2;
    int i          = 2;

    complex_test_impl(cf);
    complex_test_impl(cd);
    complex_test_impl(cld);

    complex_test_complex(cf, cd);
    complex_test_complex(cf, cld);
    complex_test_arithmetic(cf, f);

    complex_test_complex(cd, cf);
    complex_test_complex(cd, cld);
    complex_test_arithmetic(cd, d);

    complex_test_complex(cld, cf);
    complex_test_complex(cld, cd);
    complex_test_arithmetic(cld, ld);

    // no complex_test_pow_common_float for complex<int>
    // complex<float/double/long double> don't have converting
    // constructors for types that are not float/double/long double.

    complex_test_pow_common_float(cf, d);
    complex_test_pow_common_float(cf, ld);

    complex_test_pow_common_float(cd, f);
    complex_test_pow_common_float(cd, ld);

    complex_test_pow_common_float(cld, f);
    complex_test_pow_common_float(cld, d);

    STATIC_ASSERT(is_same_v<decltype(pow(cld, cld)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cld, ld)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cld, cd)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cld, d)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cld, s)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cld, i)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cld, cf)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cld, f)), complex<long double>>);

    STATIC_ASSERT(is_same_v<decltype(pow(cd, cld)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cd, ld)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cd, cd)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cd, d)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cd, s)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cd, i)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cd, cf)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cd, f)), complex<double>>);

    STATIC_ASSERT(is_same_v<decltype(pow(cf, cld)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cf, ld)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cf, cd)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cf, d)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cf, s)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cf, i)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cf, cf)), complex<float>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cf, f)), complex<float>>);

    STATIC_ASSERT(is_same_v<decltype(pow(cld, cld)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(ld, cld)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cd, cld)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(d, cld)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(s, cld)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(i, cld)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cf, cld)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(f, cld)), complex<long double>>);

    STATIC_ASSERT(is_same_v<decltype(pow(cld, cd)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(ld, cd)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cd, cd)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(d, cd)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(s, cd)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(i, cd)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cf, cd)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(f, cd)), complex<double>>);

    STATIC_ASSERT(is_same_v<decltype(pow(cld, cf)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(ld, cf)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cd, cf)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(d, cf)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(s, cf)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(i, cf)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(pow(cf, cf)), complex<float>>);
    STATIC_ASSERT(is_same_v<decltype(pow(f, cf)), complex<float>>);
}


void xhash_test() {
    (void) stdext::hash_value(5);
    (void) stdext::hash_value(string{"cat"});
    (void) stdext::hash_value(static_cast<int*>(nullptr));
    (void) stdext::hash_value(static_cast<void*>(nullptr));
    (void) stdext::hash_value(static_cast<int (*)(int)>(nullptr));
    (void) stdext::hash_value(nullptr);

    (void) stdext::hash_compare<int>()(5);
    (void) stdext::hash_compare<string>()("Hello");
    (void) stdext::hash_compare<const char*>()("World");
    (void) stdext::hash_compare<wstring>()(L"Wello");
    (void) stdext::hash_compare<const wchar_t*>()(L"Horld");

    // rest of xhash is covered by container tests
}
