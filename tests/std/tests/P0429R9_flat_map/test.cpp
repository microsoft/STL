// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <concepts>
#include <flat_map>
#include <functional>
#include <memory>
#include <type_traits>
#include <vector>

using namespace std;

template <class T, template <class...> class Tmpl>
constexpr bool is_specialization_v = false;
template <template <class...> class Tmpl, class... Ts>
constexpr bool is_specialization_v<Tmpl<Ts...>, Tmpl> = true;

template <typename T>
concept IsFlatMap =
    is_specialization_v<remove_cvref_t<T>, flat_map> || is_specialization_v<remove_cvref_t<T>, flat_multimap>;

template <IsFlatMap T>
bool check_container_requirements(T&&) {
    return true;
}

template <IsFlatMap T>
consteval bool check_reversible_container_requirements() {
    using map_t = remove_cvref_t<T>;
    return is_same_v<reverse_iterator<typename map_t::iterator>, typename map_t::reverse_iterator>
        && is_same_v<reverse_iterator<typename map_t::const_iterator>, typename map_t::const_reverse_iterator>
        && is_same_v<decltype(declval<map_t>().begin()), typename map_t::iterator>
        && is_same_v<decltype(declval<map_t>().end()), typename map_t::iterator>
        && is_same_v<decltype(declval<map_t>().cbegin()), typename map_t::const_iterator>
        && is_same_v<decltype(declval<map_t>().cend()), typename map_t::const_iterator>
        && is_same_v<decltype(declval<map_t>().rbegin()), typename map_t::reverse_iterator>
        && is_same_v<decltype(declval<map_t>().rend()), typename map_t::reverse_iterator>
        && is_same_v<decltype(declval<map_t>().crbegin()), typename map_t::const_reverse_iterator>
        && is_same_v<decltype(declval<map_t>().crend()), typename map_t::const_reverse_iterator>
        && is_convertible_v<typename map_t::iterator, typename map_t::const_iterator>
        && is_convertible_v<typename map_t::reverse_iterator, typename map_t::const_reverse_iterator>;
}

template <IsFlatMap T>
constexpr bool check_reversible_container_requirements(T&&) {
    return check_reversible_container_requirements<T&&>();
}

template <IsFlatMap T>
bool check_requirements(T&& obj) {
    return check_container_requirements(forward<T>(obj)) && check_reversible_container_requirements(forward<T>(obj));
}

template <IsFlatMap T>
bool check_key_content(const T& obj, const typename T::key_container_type& expected) {
    const auto& actual = obj.keys();
    if (actual.size() != expected.size()) {
        return false;
    }
    return ranges::equal(actual, expected);
}

template <IsFlatMap T>
bool check_value_content(const T& obj, const typename T::mapped_container_type& expected) {
    const auto& actual = obj.values();
    if (actual.size() != expected.size()) {
        return false;
    }
    return ranges::equal(actual, expected);
}

template <typename T>
class MyAllocator : public allocator<T> {
public:
    using value_type = T;
    using allocator<T>::allocator;

    static size_t getActiveAllocationCount() {
        return s_allocations.load();
    }

    T* allocate(size_t n) {
        ++s_allocations;
        return allocator<T>::allocate(n);
    }

    T* allocate_at_least(size_t n) {
        ++s_allocations;
        return allocator<T>::allocate_at_least(n);
    }

    void deallocate(T* p, size_t n) noexcept {
        --s_allocations;
        allocator<T>::deallocate(p, n);
    }

private:
    static inline atomic<size_t> s_allocations{0};
};

template <typename T>
class Packaged {
private:
    T value;

public:
    Packaged() : value() {}
    template <typename U>
        requires constructible_from<T, U&&>
    Packaged(U&& u) : value(forward<U>(u)) {}

    T get() const {
        return value;
    }

    void set(T t) {
        value = t;
    }

    friend bool operator==(const Packaged&, const Packaged&) = default;

    friend bool operator==(const Packaged& lhs, const T& rhs) {
        return lhs.value == rhs;
    }

    friend auto operator<=>(const Packaged&, const Packaged&) = default;
};

template <typename T>
struct PackagedCompare : less<Packaged<T>> {};

template <typename T>
struct TransparentPackagedCompare : PackagedCompare<T> {
    using is_transparent = void;

    bool operator()(const Packaged<T>& lhs, const Packaged<T>& rhs) const {
        return PackagedCompare<T>::operator()(lhs, rhs);
    }

    bool operator()(const T& lhs, const Packaged<T>& rhs) const {
        return lhs < rhs.get();
    }

    bool operator()(const Packaged<T>& lhs, const T& rhs) const {
        return lhs.get() < rhs;
    }
};

void test_construction() {
    {
        flat_map<int, int> fmap;
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {}));
        assert(check_value_content(fmap, {}));
    }
    {
        vector<int> keys = {0, 1, 2, 3, 4, 2};
        vector<int> vals = {44, 2324, 635462, 433, 5, 7};
        flat_map fmap(keys, vals);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
        assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
    }
    {
        vector<int, MyAllocator<int>> keys = {0, 1, 2, 3, 4, 2};
        vector<int, MyAllocator<int>> vals = {44, 2324, 635462, 433, 5, 7};
        size_t activeAllocations           = MyAllocator<int>::getActiveAllocationCount();
        flat_map fmap(keys, vals, MyAllocator<int>());
        assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
        assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
        assert(MyAllocator<int>::getActiveAllocationCount() > activeAllocations);
    }
    {
        vector<int> keys = {0, 1, 2, 3, 38, 242};
        vector<int> vals = {44, 2324, 635462, 433, 5, 7};
        flat_map fmap(sorted_unique, keys, vals);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {0, 1, 2, 3, 38, 242}));
        assert(check_value_content(fmap, {44, 2324, 635462, 433, 5, 7}));
    }
    {
        PackagedCompare<int> comp;
        flat_map<Packaged<int>, int, PackagedCompare<int>> fmap(comp);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {}));
        assert(check_value_content(fmap, {}));
    }
    {
        PackagedCompare<int> comp;
        MyAllocator<Packaged<int>> alloc;
        flat_map<Packaged<int>, Packaged<int>, PackagedCompare<int>, vector<Packaged<int>, MyAllocator<Packaged<int>>>,
            vector<Packaged<int>, MyAllocator<Packaged<int>>>>
            fmap(comp, alloc);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {}));
        assert(check_value_content(fmap, {}));
    }
    {
        MyAllocator<Packaged<int>> alloc;
        flat_map<Packaged<int>, Packaged<int>, PackagedCompare<int>, vector<Packaged<int>, MyAllocator<Packaged<int>>>,
            vector<Packaged<int>, MyAllocator<Packaged<int>>>>
            fmap(alloc);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {}));
        assert(check_value_content(fmap, {}));
    }
    {
        MyAllocator<Packaged<int>> alloc;
        flat_map<Packaged<int>, int, PackagedCompare<int>, vector<Packaged<int>, MyAllocator<Packaged<int>>>,
            vector<int, MyAllocator<int>>>
            fmap(alloc);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {}));
        assert(check_value_content(fmap, {}));
    }
    {
        PackagedCompare<int> comp;
        MyAllocator<int> alloc;
        vector<Packaged<int>, MyAllocator<Packaged<int>>> keys = {0, 1, 2, 3, 4, 2};
        vector<int, MyAllocator<int>> vals                     = {44, 2324, 635462, 433, 5, 7};
        flat_map fmap(keys, vals, comp, alloc);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
        assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
    }
    {
        TransparentPackagedCompare<int> comp;
        MyAllocator<int> alloc;
        vector<Packaged<int>, MyAllocator<Packaged<int>>> keys = {0, 1, 2, 3, 4, 2};
        vector<int, MyAllocator<int>> vals                     = {44, 2324, 635462, 433, 5, 7};
        flat_map fmap(keys, vals, comp, alloc);
        assert(check_requirements(fmap));
        assert(check_key_content(fmap, {0, 1, 2, 3, 4}));
        assert(check_value_content(fmap, {44, 2324, 635462, 433, 5}));
    }
}

void test_erase_if() {
    {
        vector<int> keys = {0, 1, 2, 3, 4, 2};
        vector<int> vals = {44, 2324, 635462, 433, 5, 7};
        flat_map fmap(keys, vals);
        const auto erased_num = erase_if(
            fmap, [](pair<const int&, const int&> refpr) { return refpr.first % 2 == 0 && refpr.second % 2 != 0; });
        assert(erased_num == 1);
        assert(fmap.size() == 4);
        assert(check_key_content(fmap, {0, 1, 2, 3}));
        assert(check_value_content(fmap, {44, 2324, 635462, 433}));
    }
}

struct Incomplete;
template <class T>
struct MyType {
    T* ptr;

    bool operator==(const MyType&) const  = default;
    auto operator<=>(const MyType&) const = default;
};

void test_pointer_to_incomplete_type() {
    struct Test {
        unique_ptr<flat_map<Test, Test>> ptr;
    };

    Test t;
    flat_map<MyType<Incomplete>, shared_ptr<MyType<Incomplete>>> fmap;
}

void test_insert_unique() {
    flat_map<int, char> fm;

    const auto p1 = fm.insert({10, 'm'});
    assert(p1.first->first == 10);
    assert(p1.first->second == 'm');
    assert(p1.second);

    const flat_map<int, char>::value_type val_pair{90, 'w'};
    const auto p2 = fm.insert(val_pair);
    assert(p2.first->first == 90);
    assert(p2.first->second == 'w');
    assert(p2.second);

    assert(check_key_content(fm, {10, 90}));
    assert(check_value_content(fm, {'m', 'w'}));
}

void test_insert_multi() {
    flat_multimap<int, char> fmm;

    const auto it1 = fmm.insert({10, 'm'});
    assert(it1->first == 10);
    assert(it1->second == 'm');

    const flat_multimap<int, char>::value_type val_pair{90, 'w'};
    const auto it2 = fmm.insert(val_pair);
    assert(it2->first == 90);
    assert(it2->second == 'w');

    assert(check_key_content(fmm, {10, 90}));
    assert(check_value_content(fmm, {'m', 'w'}));
}

// GH-4344 <flat_map> Fix compile errors
void test_gh_4344() {
    flat_map<int, char> fm;

    const auto p1 = fm.try_emplace(10, 'm');
    assert(p1.first->first == 10);
    assert(p1.first->second == 'm');
    assert(p1.second);

    const auto p2 = fm.try_emplace(70, 'e');
    assert(p2.first->first == 70);
    assert(p2.first->second == 'e');
    assert(p2.second);

    const auto p3 = fm.try_emplace(20, 'o');
    assert(p3.first->first == 20);
    assert(p3.first->second == 'o');
    assert(p3.second);

    const auto p4 = fm.try_emplace(90, 'w');
    assert(p4.first->first == 90);
    assert(p4.first->second == 'w');
    assert(p4.second);

    const auto p5 = fm.try_emplace(70, 'X');
    assert(p5.first->first == 70);
    assert(p5.first->second == 'e');
    assert(!p5.second);

    assert(check_key_content(fm, {10, 20, 70, 90}));
    assert(check_value_content(fm, {'m', 'o', 'e', 'w'}));
}

void test_insert_or_assign() {
    flat_map<int, char> fm;

    const auto p1 = fm.insert_or_assign(10, 'm');
    assert(p1.first->first == 10);
    assert(p1.first->second == 'm');
    assert(p1.second);

    const auto p2 = fm.insert_or_assign(70, 'e');
    assert(p2.first->first == 70);
    assert(p2.first->second == 'e');
    assert(p2.second);

    const auto p3 = fm.insert_or_assign(20, 'o');
    assert(p3.first->first == 20);
    assert(p3.first->second == 'o');
    assert(p3.second);

    const auto p4 = fm.insert_or_assign(90, 'w');
    assert(p4.first->first == 90);
    assert(p4.first->second == 'w');
    assert(p4.second);

    const auto p5 = fm.insert_or_assign(70, 'X');
    assert(p5.first->first == 70);
    assert(p5.first->second == 'X');
    assert(!p5.second);

    assert(check_key_content(fm, {10, 20, 70, 90}));
    assert(check_value_content(fm, {'m', 'o', 'X', 'w'}));

    const auto it6 = fm.insert_or_assign(fm.cbegin(), 10, 'b');
    assert(it6->first == 10);
    assert(it6->second == 'b');

    const char ch  = 'a';
    const auto it7 = fm.insert_or_assign(fm.cbegin(), 20, ch);
    assert(it7->first == 20);
    assert(it7->second == 'a');

    assert(check_key_content(fm, {10, 20, 70, 90}));
    assert(check_value_content(fm, {'b', 'a', 'X', 'w'}));
}

// Test MSVC STL-specific SCARY-ness
template <bool IsUnique, class Comparator, class Alloc1, class Alloc2>
void test_scary_ness_one() { // COMPILE-ONLY
    using Iter      = flat_map<int, int>::iterator;
    using OtherIter = conditional_t<IsUnique, flat_map<int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>,
        flat_multimap<int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>>::iterator;
    static_assert(is_same_v<Iter, OtherIter>);

    using ConstIter = flat_map<int, int>::const_iterator;
    using OtherConstIter =
        conditional_t<IsUnique, flat_map<int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>,
            flat_multimap<int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>>::const_iterator;
    static_assert(is_same_v<ConstIter, OtherConstIter>);

    using Cont      = flat_map<int, int, less<int>, vector<int, Alloc1>, vector<int, Alloc2>>::containers;
    using OtherCont = conditional_t<IsUnique, flat_map<int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>,
        flat_multimap<int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>>::containers;
    static_assert(is_same_v<Cont, OtherCont>);

    using ValueComp = flat_map<int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>::value_compare;
    using OtherValueComp =
        conditional_t<IsUnique, flat_map<int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>,
            flat_multimap<int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>>::value_compare;
    static_assert(is_same_v<ValueComp, OtherValueComp>);
}

void test_scary_ness() { // COMPILE-ONLY
    test_scary_ness_one<true, greater<int>, allocator<int>, allocator<int>>();
    test_scary_ness_one<true, greater<int>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<true, greater<int>, MyAllocator<int>, allocator<int>>();

    test_scary_ness_one<true, less<>, allocator<int>, allocator<int>>();
    test_scary_ness_one<true, less<>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<true, less<>, MyAllocator<int>, allocator<int>>();

    test_scary_ness_one<true, greater<>, allocator<int>, allocator<int>>();
    test_scary_ness_one<true, greater<>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<true, greater<>, MyAllocator<int>, allocator<int>>();

    test_scary_ness_one<false, greater<int>, allocator<int>, allocator<int>>();
    test_scary_ness_one<false, greater<int>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<false, greater<int>, MyAllocator<int>, allocator<int>>();

    test_scary_ness_one<false, less<>, allocator<int>, allocator<int>>();
    test_scary_ness_one<false, less<>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<false, less<>, MyAllocator<int>, allocator<int>>();

    test_scary_ness_one<false, greater<>, allocator<int>, allocator<int>>();
    test_scary_ness_one<false, greater<>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<false, greater<>, MyAllocator<int>, allocator<int>>();
}

int main() {
    test_construction();
    test_pointer_to_incomplete_type();
    test_erase_if();
    test_insert_unique();
    test_insert_multi();
    test_gh_4344();
    test_insert_or_assign();
}
