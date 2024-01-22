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
concept IsFlatMap = is_specialization_v<remove_cvref_t<T>, flat_map>;

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

// Test MSVC STL-specific SCARY-ness
namespace scary_test {
    static_assert(is_same_v<flat_map<int, int>::iterator, flat_map<int, int, greater<int>>::iterator>);
    static_assert(is_same_v<flat_map<int, int>::iterator,
        flat_map<int, int, less<>, vector<int, MyAllocator<int>>, vector<int, MyAllocator<int>>>::iterator>);
    static_assert(is_same_v<flat_map<int, int>::const_iterator, flat_map<int, int, greater<int>>::const_iterator>);
    static_assert(is_same_v<flat_map<int, int>::const_iterator,
        flat_map<int, int, less<>, vector<int, MyAllocator<int>>, vector<int, MyAllocator<int>>>::const_iterator>);

    static_assert(is_same_v<flat_multimap<int, int>::iterator, flat_multimap<int, int, greater<int>>::iterator>);
    static_assert(is_same_v<flat_multimap<int, int>::iterator,
        flat_multimap<int, int, less<>, vector<int, MyAllocator<int>>, vector<int, MyAllocator<int>>>::iterator>);
    static_assert(
        is_same_v<flat_multimap<int, int>::const_iterator, flat_multimap<int, int, greater<int>>::const_iterator>);
    static_assert(is_same_v<flat_multimap<int, int>::const_iterator,
        flat_multimap<int, int, less<>, vector<int, MyAllocator<int>>, vector<int, MyAllocator<int>>>::const_iterator>);

    static_assert(is_same_v<flat_map<int, int>::iterator, flat_multimap<int, int>::iterator>);
    static_assert(is_same_v<flat_map<int, int>::const_iterator, flat_multimap<int, int>::const_iterator>);

    static_assert(is_same_v<flat_map<int, int>::containers, flat_map<int, int, greater<int>>::containers>);
    static_assert(is_same_v<flat_multimap<int, int>::containers, flat_multimap<int, int, greater<int>>::containers>);
    static_assert(is_same_v<flat_map<int, int>::containers, flat_multimap<int, int>::containers>);

    static_assert(is_same_v<flat_map<int, int>::value_compare,
        flat_map<int, int, less<int>, vector<int, MyAllocator<int>>, vector<int, MyAllocator<int>>>::value_compare>);
    static_assert(is_same_v<flat_multimap<int, int>::value_compare,
        flat_multimap<int, int, less<int>, vector<int, MyAllocator<int>>,
            vector<int, MyAllocator<int>>>::value_compare>);
    static_assert(is_same_v<flat_map<int, int>::value_compare, flat_multimap<int, int>::value_compare>);
} // namespace scary_test

int main() {
    test_construction();
    test_pointer_to_incomplete_type();
    test_erase_if();
}
